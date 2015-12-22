/*
 * GenerationalGC.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GenerationalGC.h"

//using namespace std;
using namespace Bee;

GenerationalGC::GenerationalGC() {

}

bool GenerationalGC::arenaIncludes(unsigned long pointer) {
	// TODO
	return false;

}

bool GenerationalGC::hasToPurge(unsigned long pointer) {
	// TODO
	return false;

}

void GenerationalGC::purgeLiteralsReference() {
	long kept, offset, literal = 0;
	for (int index = 0; index < literalsReferences.size(); index = index + 2) {
		literal = literalsReferences[index];
		if (this->arenaIncludes(literal)) {
			kept = kept + 2;
			offset = literalsReferences[index + 1];
			literalsReferences[kept - 1] = literal;
			literalsReferences[kept] = offset;
		}
	}
}

void GenerationalGC::fixReferencesOrSetTombstone(unsigned long weakContainer) {
	// care it is object size !!!!
	unsigned long instance, referenceOrThombstone;
	for (int index = 1; index <= size(weakContainer); index++) {
		instance = basicAt(weakContainer, index);
		if (this->arenaIncludes(instance)) {
			if (isProxy(instance)) {
				referenceOrThombstone = proxee(instance);
			} else {
				referenceOrThombstone = residueObject;
			}
			basicAtPut(weakContainer, index, referenceOrThombstone);
		}
	}
}

void GenerationalGC::purgeRememberSet() {
	long kept, object = 0;
	for (int index = 0; index < rememberSet.size(); index++) {
		object = rememberSet[index];
		rememberSet[index] = nil;
		if (this->hasToPurge(object)) {
			//object _beNotInRememberSet
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
	for (int index = 1; index < rememberSet.size(); index++) {
		this->follow(rememberSet[index]);
	}
}

void GenerationalGC::moveClassCheckReferences() {
//classCheckReferences do: [:offset | | object reference |
//	reference := self codeCacheAtOffset: offset.
//	object := reference _basicAt: 1.
//	(self arenaIncludes: object) ifTrue: [| moved |
//		moved := object _isProxy
//			ifTrue: [object _proxee]
//			ifFalse: [self moveToOldSpace: object].
//		reference _basicAt: 1 put: moved]].
//self checkClassCheckReferences: false
}

void GenerationalGC::moveToOldAll(ReferencedVMArray objects) {
	unsigned long moved, object = 0;
	for (int index = 1; index < objects.size(); index++) {
		object = objects[index];
		if (this->arenaIncludes(object)) {
			if (isProxy(object)) {
				moved = proxee(object);
			} else {
				moved = this->moveToOldSpace(object);
			}
			objects[index] = moved;
		}
	}

}

unsigned long GenerationalGC::moveToOldSpace(unsigned long object) {
	unsigned long copy;
	copy = this->copyTo(object, oldSpace);
	this->holdReferenceTo(copy);
	return copy;
}

unsigned long GenerationalGC::holdReferenceTo(unsigned long object) {
	//object _isInRememberSet ifFalse: [
	//	object _beInRememberSet.
	//	self addRoot: object]

}

unsigned long GenerationalGC::copyTo(unsigned long object, GCSpace to) {
	unsigned long copy = 0;
	copy = to.shallowCopy(object);
	object = proxee(copy);
	return copy;
}

void GenerationalGC::followCodeCacheReferences() {
	if (this->dereference(0x1003EC48)) {
		MEM0x1003EC48 = 0;
		this->clearPolymorphicMethodCache();
	}
	if (this->dereference(0x10041710))
		this->moveClassCheckReferences();
	if (this->dereference(0x10041714)) {
		MEM0x10041714 = 0;
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
//	| delta |
//	self spacesDelta == 0 _asPointer ifFalse: [
//		delta := toSpace base - fromSpace base.
//		self spacesDelta: delta]
}

void GenerationalGC::fixReferencesFromNativeMethods() {
	unsigned long literal, offset, reference = 0;
	for (int index = 0; index < literalsReferences.size(); index++++) {
		literal = literalsReferences[index];
		offset = literalsReferences[index + 1];
		reference = memoryAt(MEM0x1002E820 * 2 + offset);
		memoryAtPut(reference, literal);
	}
}

void GenerationalGC::cleanRememberSet() {
	int kept = 0;
	unsigned long object = 0;
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

