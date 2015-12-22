/*
 * GenerationalGC.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GenerationalGC.h"

// TODO nil

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

//	1 to: objects size do: [:index | | object |
//		object := objects at: index.
//		(self arenaIncludes: object) ifTrue: [| moved |
//			moved := object _isProxy
//				ifTrue: [object _proxee]
//				ifFalse: [self moveToOldSpace: object].
//			objects at: index put: moved]]

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
	auxSpace
	loadFrom: toSpace;
	toSpace
	loadFrom: fromSpace;
	fromSpace
	loadFrom: auxSpace;
	toSpace reset;
	this->updateSpacesDelta();
}

void GenerationalGC::updateSpacesDelta() {
//	| delta |
//	self spacesDelta == 0 _asPointer ifFalse: [
//		delta := toSpace base - fromSpace base.
//		self spacesDelta: delta]
}

void GenerationalGC::fixReferencesFromNativeMethods() {

//	1 to: literalsReferences size by: 2 do: [:index | | literal offset reference |
//		literal := literalsReferences at: index.
//		offset := literalsReferences at: index + 1.
//		reference := self codeCacheAtOffset: offset.
//		reference _basicAt: 1 put: literal]
}

void GenerationalGC::cleanRememberSet() {
	//| kept |
	//kept := 0.
	//1 to: rememberSet size do: [:index | | object |
	//	object := rememberSet at: index.
	//	rememberSet at: index put: nil.
	//	(toSpace includes: object) ifFalse: [
	//		kept := kept + 1.
	//		rememberSet at: kept put: object]].
	//rememberSet size: kept
}

void GenerationalGC::addInterrupt() {
	//self interrupt: 11
}

void GenerationalGC::collect() {
	this->loadSpaces();
	this->initLocals();
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
	this->forgetNativeObjects();
	this->addInterrupt();
	this->saveSpaces();
}

