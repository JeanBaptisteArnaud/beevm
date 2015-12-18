/*
 * GenerationalGC.cpp
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GenerationalGC.h"

// TODO nil
#define nil 0
#define MEM0x1003EC48 0
#define MEM0x10041710 0
#define MEM0x10041714 0

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
bool GenerationalGC::dereference(unsigned long pointer) {
	// TODO
	return false;

}

void GenerationalGC::purgeLiteralsReference(){
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


void GenerationalGC::purgeRememberSet(){
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


void GenerationalGC::followRememberSet(){
	for (int index = 1; index < rememberSet.size(); index++){
		this->follow(rememberSet[index]);}
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


void GenerationalGC::followCodeCacheReferences(){
		if(this->dereference(MEM0x1003EC48)) {
			MEM0x1003EC48 = 0;
			this->clearPolymorphicMethodCache();
			}
		if(this->dereference(MEM0x10041710))moveClassCheckReferences();
		if(this->dereference(MEM0x10041714))
			{MEM0x10041714 = 0;
			this->moveToOldAll(nativizedMethods);
			}
		this->moveToOldAll(literalsReferences);
	}



void GenerationalGC::collect() {
	this->loadSpaces();
	this->initLocals();
	this->useNativeObjects();
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

