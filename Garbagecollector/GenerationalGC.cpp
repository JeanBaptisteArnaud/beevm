/*
 * GenerationalGC.cpp
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GenerationalGC.h"

//using namespace std;
using namespace Bee;

GenerationalGC::GenerationalGC() {

}

bool GenerationalGC::arenaIncludes(unsigned long *pointer) {
	// TODO
	return false;

}

bool GenerationalGC::hasToPurge(unsigned long *pointer) {
	// TODO
	return false;

}

void GenerationalGC::purgeLiteralsReference() {
	long kept = 0;
	unsigned long  offset;
	unsigned long *literal;
	for (ulong index = 1; index < literalsReferences.size(); index = index + 2) {
		literal = literalsReferences[index];
		if (this->arenaIncludes(literal)) {
			kept = kept + 2;
			offset = (unsigned long)literalsReferences[index + 1];
			literalsReferences[kept - 1] = literal;
			literalsReferences[kept] = (unsigned long *)offset;
		}
	}
}

void GenerationalGC::fixReferencesOrSetTombstone(ulong *weakContainer) {

	unsigned long *referenceOrThombstone;
	for (ulong index = 0; index < _size(weakContainer); index++) {
		ulong *instance = (ulong*)weakContainer[index];
		if (this->arenaIncludes(instance)) {
			if (_isProxy(instance)) {
				referenceOrThombstone = _proxee(instance);
			} else {
				referenceOrThombstone = residueObject;
			}
			weakContainer[index] = (ulong)referenceOrThombstone;
		}
	}
}

void GenerationalGC::purgeRememberSet() {
	long kept = 0;
	for (ulong index = 1; index <= rememberSet.size(); index++) {
		ulong *object = rememberSet[index];
		rememberSet[index] = nil;
		if (this->hasToPurge(object)) {
			_beNotInRememberedSet(object);
		} else {
			rememberSet[kept + 1] = object;
		}

		rememberSet.size(kept);
	}
}

void GenerationalGC::followRoots() {
	this->followRememberSet();
}

void GenerationalGC::followRememberSet() {
	for (ulong index = 1; index < rememberSet.size(); index++) {
		this->follow(rememberSet[index]);
	}
}

unsigned long* GarbageCollector::framePointerToStartWalkingTheStack() {
	return MEM_globalFramePointerToWalkStack;
}

unsigned long* GenerationalGC::codeCacheAtOffset(unsigned long offset)
{
	return (ulong*)(*MEM_JIT_codeCachePointer + offset);
}

void GenerationalGC::moveClassCheckReferences() {
	unsigned long *moved;
	for (ulong index = 1; index < classCheckReferences.size(); index++) {
		unsigned long offset = (ulong)classCheckReferences[index];
		unsigned long *reference = codeCacheAtOffset(offset);
		ulong *object = (ulong*)*reference;
		if (this->arenaIncludes(object)) {
			if (_isProxy(object)) {
				moved = _proxee(object);
			} else {
				moved = moveToOldSpace(object);
			}
			*reference = (unsigned long)moved;
		}
	}

	*MEM_anyCompiledMethodInFromSpace = 0;
}

void GenerationalGC::moveToOldAll(ReferencedVMArray &objects) {
	unsigned long *moved;
	for (ulong index = 1; index <= objects.size(); index++) {
		ulong *object = objects[index];
		if (this->arenaIncludes(object)) {
			if (_isProxy(object)) {
				moved = _proxee(object);
			} else {
				moved = this->moveToOldSpace(object);
			}
			// notice 1-based index
			objects[index] = moved;
		}
	}

}

unsigned long* GenerationalGC::moveToOldSpace(unsigned long *object) {
	unsigned long *copy = this->copyTo(object, oldSpace);
	this->holdReferenceTo(copy);
	return copy;
}

unsigned long GenerationalGC::holdReferenceTo(unsigned long *object) {
	//object _isInRememberSet ifFalse: [
	//	object _beInRememberSet.
	//	self addRoot: object]

}

unsigned long* GenerationalGC::copyTo(unsigned long *object, GCSpace &to) {
	unsigned long *copy = 0;
	copy = to.shallowCopy(object);
	object = _proxee(copy);
	return copy;
}

void GenerationalGC::followCodeCacheReferences()
{
	if (*MEM_polymorphicMethodCacheReferesToNewCM)
	{
		*MEM_polymorphicMethodCacheReferesToNewCM = false;
		this->clearPolymorphicMethodCache();
	}

	if (*MEM_anyCompiledMethodInFromSpace)
		this->moveClassCheckReferences();

	if (*MEM_checkNewNativizedCompiledMethods) {
		*MEM_checkNewNativizedCompiledMethods = false;
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
	unsigned long delta;
	if (!this->spacesDelta()) {
		delta = toSpace.base() - fromSpace.base();
		this->spacesDelta(delta);
	}
}

unsigned long  GenerationalGC::spacesDelta() {
	return memoryAt(0x1004175C);
}

void GenerationalGC::spacesDelta(unsigned long delta) {
	memoryAtPut((unsigned long *)0x1004175C,delta);
}

void GenerationalGC::fixReferencesFromNativeMethods() {

	for (int index = 0; index < literalsReferences.size(); index = index + 2) {
		unsigned long literal , *reference = 0;
		unsigned long offset =0;
		literal = (unsigned long)literalsReferences[index];
		offset = (unsigned long)literalsReferences[index + 1];
		reference = codeCacheAtOffset(offset);
		memoryAtPut(reference, literal);
	}
}

void GenerationalGC::cleanRememberSet() {
	int kept = 0;
	unsigned long *object = 0;
	for (int index = 0; index < rememberSet.size(); index++) {
		object = rememberSet[index];
		rememberSet[index] = nil;
		if (!toSpace.includes(object)) {
			kept++;
			rememberSet[kept] = object;
		}
	}
	rememberSet.size(kept);
}

void GenerationalGC::addInterrupt() {
	//self interrupt: 11
}

void GenerationalGC::collect() {
	this->loadSpaces(); // need to be change
	this->initLocals(); // need to be change
	//Thinks we do not need that:: this->useNativeObjects();
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

