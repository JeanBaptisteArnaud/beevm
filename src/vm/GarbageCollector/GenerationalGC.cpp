/*
 * GenerationalGC.cpp
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include <iostream>

#include "GenerationalGC.h"
#include "GarbageCollector.h"

#include "../DataStructures/ObjectFormat.h"
#include "../DataStructures/KnownObjects.h"

using namespace std;
using namespace Bee;

GenerationalGC::GenerationalGC() {
}

GenerationalGC::~GenerationalGC() {
}

bool GenerationalGC::hasToPurge(oop_t *object) {
	cerr << "hasToPurge" << endl;
	return false;
}

void GenerationalGC::holdReferenceTo(oop_t* object) {
	if (!object->_isInRememberedSet()) {
		object->_beInRememberedSet();
		rememberSet.add(object);
	}
}

void GenerationalGC::someEphemeronsRescued() {
	//this->holdReferenceTo(rescuedEphemerons.contents);
}

void GenerationalGC::purgeLiteralsReference() {
	long kept = 0;
	ulong offset;
	oop_t *literal;
	for (long index = 1; index < literalsReferences.size()->_asNative(); index = index + 2)
	{
		literal = literalsReferences[index];
		if (this->arenaIncludes(literal)) {
			kept = kept + 2;
			offset = (ulong) literalsReferences[index + 1];
			literalsReferences[kept - 1] = literal;
			literalsReferences[kept] = (oop_t*) offset;
		}
	}
}

void GenerationalGC::fixReferencesOrSetTombstone(oop_t * weakContainer) {
	for (ulong index = 0; index < weakContainer->_size(); index++) {
		oop_t *instance = weakContainer->slot(index);
		if (this->arenaIncludes(instance)) {
			if (instance->_isProxy()) {
				weakContainer->slot(index) = instance->_getProxee();
			} else {
				weakContainer->slot(index) = residueObject;
			}
		}
	}
}

void GenerationalGC::purgeRememberSet() {
	long kept = 0;
	for (long index = 1; index <= rememberSet.size()->_asNative(); index++)
	{
		oop_t *object = rememberSet[index];
		rememberSet[index] = KnownObjects::nil;
		if (this->hasToPurge(object)) {
			object->_beNotInRememberedSet();
		} else {
			rememberSet[kept + 1] = object;
		}

		rememberSet.size(smiConst(kept));
	}
}

void GenerationalGC::followRoots() {
	this->followRememberSet();
}

void GenerationalGC::followRememberSet() {
	for (long index = 1; index <= rememberSet.size()->_asNative(); index++)
	{
		this->follow(rememberSet[index]);
	}
}

ulong * GenerationalGC::framePointerToStartWalkingTheStack()
{
	return vm.framePointerToStartWalkingTheStack();
}

ulong* GenerationalGC::codeCacheReferenceAtOffset(ulong offset)
{
	return (ulong*) (vm.codeCache() + offset);
}

void GenerationalGC::moveClassCheckReferences()
{
	oop_t *moved;
	for (long index = 1; index < classCheckReferences.size()->_asNative(); index++)
	{
		ulong offset = (ulong) classCheckReferences[index];
		ulong *reference = codeCacheReferenceAtOffset(offset);
		oop_t *object = (oop_t*) *reference;
		if (this->arenaIncludes(object)) {
			if (object->_isProxy()) {
				moved = object->_getProxee();
			} else {
				moved = moveToOldSpace(object);
			}
			*reference = (ulong) moved;
		}
	}

	vm.anyCompiledMethodInFromSpace(false);
}

void GenerationalGC::moveToOldAll(ReferencedVMArray &objects)
{
	oop_t *moved;
	for (long index = 1; index <= objects.size()->_asNative(); index++)
	{
		slot_t &object = objects[index];
		if (this->arenaIncludes(object)) {
			if (object->_isProxy()) {
				moved = object->_getProxee();
			} else {
				moved = this->moveToOldSpace(object);
			}
			// notice 1-based index
			objects[index] = moved;
		}
	}
}

oop_t* GenerationalGC::moveToOldSpace(oop_t *object)
{
	oop_t * copy = this->copyTo(object, oldSpace);
	this->holdReferenceTo(copy);
	return copy;
}

oop_t* GenerationalGC::copyTo(oop_t *object, GCSpace &to) {
	oop_t *copy = to.shallowCopy(object);
	object->_setProxee(copy);
	return copy;
}

bool GenerationalGC::checkReachablePropertyOf(oop_t *ephemeron) {
	return ephemeron->slot(0)->_isProxy() || !arenaIncludes(ephemeron->slot(0));
}


void GenerationalGC::purgeRoots() {
	this->purgeLiteralsReference();
	this->purgeRememberSet();

}

void GenerationalGC::followCodeCacheReferences()
{
	if (vm.globalCacheHasPointersToFrom())
	{
		vm.globalCacheHasPointersToFrom(false);
		this->clearPolymorphicMethodCache();
	}

	if (vm.anyCompiledMethodInFromSpace())
		this->moveClassCheckReferences();

	if (vm.anyNativizedCompiledMethodInFromSpace())
	{
		vm.anyNativizedCompiledMethodInFromSpace(false);
		this->moveToOldAll(nativizedMethods);
	}
	this->moveToOldAll(literalsReferences);
}

void GenerationalGC::flipSpaces() {
	auxSpace.loadFrom(toSpace);
	toSpace.loadFrom(fromSpace);
	fromSpace.loadFrom(auxSpace);
	toSpace.reset();
	this->updateSpacesDelta();
}

void GenerationalGC::updateSpacesDelta() {
	ulong delta;
	if (!vm.spacesDelta()) {
		delta = toSpace.getBase() - fromSpace.getBase();
		vm.spacesDelta(delta);
	}
}


void GenerationalGC::fixReferencesFromNativeMethods()
{
	for (long index = 0; index < literalsReferences.size()->_asNative(); index = index + 2)
	{
		ulong literal = (ulong) literalsReferences[index];
		ulong offset = (ulong) literalsReferences[index + 1];
		ulong *reference = codeCacheReferenceAtOffset(offset);
		*reference = literal;
	}
}

void GenerationalGC::cleanRememberSet()
{
	int kept = 0;
	oop_t* object = 0;
	for (long index = 0; index < rememberSet.size()->_asNative(); index++)
	{
		object = rememberSet[index];
		rememberSet[index] = KnownObjects::nil;
		if (!(toSpace.includes(object)))
		{
			kept++;
			rememberSet[kept] = object;
		}
	}
	rememberSet.size(smiConst(kept));
}

bool GenerationalGC::arenaIncludes(oop_t *object) {
	return this->fromSpace.includes(object);
}

void GenerationalGC::followCountStartingAt(slot_t *root, int size, long start) {
	//stack = self localStack.
	slot_t *objects = root;
	long index = start - 1;
	long limit = index + size;
	bool first = true;
	while (first | !(stack.isEmpty())) {
		first = false;
		while (index < limit) {
			index = index + 1;
			oop_t *object = objects[index];
			if (this->arenaIncludes(object)) {
				if (object->_isProxy()){
					objects[index] = object->_getProxee();
				}
				else {
					if (index < limit) {
						stack.push((oop_t*)objects);
						stack.push(smiConst(index));
						stack.push(smiConst(limit));
					}
					this->rememberIfWeak(object);
					// in the original I think it is a bug self rememberIfWeak: moved. and move before
					oop_t *moved = this->moveToOldOrTo(object);
					objects[index] = moved;
					this->rememberIfWeak(moved);
					index = - 2; // smalltalk power array base 1
					limit = index + moved->_strongPointersSize();
					objects = (oop_t**)moved;
				}
			}
		}
		if (!stack.isEmpty()) {
			limit = stack.pop()->_asNative();
			index = stack.pop()->_asNative();
			objects = (oop_t **) stack.pop();
		}
	}
}

void GenerationalGC::addInterrupt() {
	//self interrupt: 11
}

void GenerationalGC::initLocals() {
	localSpace.reset();
	stack.setSpace(&localSpace); //emptyReserving: 1024.
	unknowns.setSpace(&localSpace); // emptyReserving: 1025.
	ephemerons.setSpace(&localSpace); // emptyReserving: 1026.
	weakContainers.setSpace(&localSpace); // emptyReserving: 1027
}

void GenerationalGC::initNonLocals() {
	rescuedEphemerons.setSpace(&oldSpace);
	rememberSet.setSpace(&oldSpace);
	literalsReferences.setSpace(&oldSpace);
	nativizedMethods.setSpace(&oldSpace);
	classCheckReferences.setSpace(&oldSpace);
}

void GenerationalGC::addRoot(oop_t *object) {
	rememberSet.add(object);
}

oop_t* GenerationalGC::moveToOldOrTo(oop_t * object) {
	if (object->_isSecondGeneration()) {
		return this->moveToOldSpace(object);
	} else {
		return this->moveToToSpace(object);
	}

}

oop_t* GenerationalGC::moveToToSpace(oop_t *object) {
	object->_beSecondGeneration();
	return this->copyTo(object, toSpace);
}

void GenerationalGC::collect() {
	//this->loadSpaces(); // need to be change
	this->initLocals(); // need to be change
	this->initNonLocals(); // need to be change
	//Tthinks we do not need that:: this->useNativeObjects();
	this->purgeLiteralsReference();
	this->purgeRememberSet();
	this->followCodeCacheReferences();
	this->followRoots();
	this->followStack();
	this->rescueEphemerons();
	this->makeRescuedEphemeronsNonWeak();
	this->fixWeakContainers();
	this->flipSpaces();
	this->fixReferencesFromNativeMethods();
	this->cleanRememberSet();
	////Thinks we do not need that:: this->forgetNativeObjects();
	////Thinks we do not need that:: this->addInterrupt();
	////Thinks we do not need that:: this->saveSpaces();
}

