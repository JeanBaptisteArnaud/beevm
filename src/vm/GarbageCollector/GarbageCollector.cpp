/*
 * GarbageCollector.cpp
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include <cstddef>
#include <iostream>

#include "GarbageCollector.h"

#include "../DataStructures/VMArray.h"
#include "../DataStructures/GCSpace.h"
#include "../DataStructures/ReferencedVMArray.h"
#include "../DataStructures/ObjectFormat.h"
#include "../DataStructures/KnownObjects.h"


// just until I fix my environment
#define NULL 0

using namespace std;
using namespace Bee;

GarbageCollector* GarbageCollector::flipper = NULL;

GarbageCollector::GarbageCollector() {
	fromSpace = GCSpace::currentFrom();
	toSpace = GCSpace::currentTo();
	oldSpace = GCSpace::old();
	residueObject = 0;
}

void GarbageCollector::collect() {

}

void GarbageCollector::rescueEphemeron(oop_t *ephemeron) {
	this->followCountStartingAt((slot_t*)ephemeron, ephemeron->_extendedSize(), 1);
	rescuedEphemerons.add(ephemeron);
	return;
}

bool GarbageCollector::followEphemeronsCollectingUnknowns() {
	bool rescan = false;
	while (!ephemerons.isEmpty()) {
		oop_t *ephemeron = ephemerons.pop();
		if (!(ephemeron == KnownObjects::nil)) {
			if (this->checkReachablePropertyOf(ephemeron)) {
				this->followCountStartingAt((slot_t*)ephemeron, ephemeron->_extendedSize(), 1);
				rescan = true;
			} else {
				unknowns.add(ephemeron);
			}
		}
	}
	return rescan;
}

void GarbageCollector::rescueEphemerons() {
	bool rescued = false;
	VMArray aux;
	while (!ephemerons.isEmpty()) {
		if (this->followEphemeronsCollectingUnknowns()) {
			aux = ephemerons;
			ephemerons = unknowns;
			unknowns = aux;
		} else {
			for (long ephemeronIndex = 0; ephemeronIndex < ephemerons.size()->_asNative();	ephemeronIndex++)
				this->rescueEphemeron(unknowns[ephemeronIndex]);
			rescued = true;
		}
		unknowns.reset();
	}

	if (rescued)
		this->someEphemeronsRescued();
}

void GarbageCollector::clearPolymorphicMethodCache() {
	for (int index = 1; index <= 0x4000; index++) {
		vm.globalLookupCacheAtPut(index, KnownObjects::nil);
	}
}

void GarbageCollector::followStack() {
	ulong *frame = this->framePointerToStartWalkingTheStack();
	ulong nextFrame = *frame;
	while (nextFrame) {
		ulong start;
		ulong size = (ulong) nextFrame - (ulong) frame / 4;
		if (frame[1] == (ulong)*vm.debugFrameMarker) {
			this->followCountStartingAt((slot_t*)frame, 5, 3);
			start = 9;
		} else
			start = 3;
		this->followFrameCountStartingAt((slot_t*)frame, size, start);
		frame = (ulong *) nextFrame;
		nextFrame = frame[0];
	}
}

void GarbageCollector::followFrameCountStartingAt(slot_t *frame, ulong count, ulong start) {
	cerr << "Need to implement " << "followCountStartingAt" << endl;
//	followFrame: frame count: size startingAt: startIndex
//		| start index gapMarker callbackFrame |
//		gapMarker := 2 _asPointer _asObject.
//		start := index := startIndex.
//		[index < size] whileTrue: [| object |
//			object := frame _basicAt: index.
//			object == gapMarker ifTrue: [
//				callbackFrame := frame _basicAt: index + 1.
//				self follow: frame count: index - start startingAt: start.
//				self follow: callbackFrame count: 5 startingAt: 1.
//				index := callbackFrame _asPointer - frame _asPointer // 4 _asPointer.
//				start := (index := index + 7) + 1].
//			index := index + 1].
//		self follow: frame count: size - start + 1 startingAt: start

}

void GarbageCollector::follow(oop_t *object)
{
	this->rememberIfWeak(object);
	this->followCountStartingAt((slot_t*)object, object->_strongPointersSize(), 0);
}

void GarbageCollector::rememberIfWeak(oop_t *object)
{
	if (object->_hasWeaks())
		this->addWeakContainer(object);
}

void GarbageCollector::addWeakContainer(oop_t *object)
{
	if (object->_isActiveEphemeron()) {
		ephemerons.add(object);
	} else
		weakContainers.add(object);
}

void GarbageCollector::fixWeakContainers()
{
	for (long index = 1; index <= weakContainers.size()->_asNative(); index++) {
		this->fixReferencesOrSetTombstone(weakContainers[index]);
	}
	weakContainers.reset();
}
void GarbageCollector::tombstone(oop_t *object)
{
	this->residueObject = object;
}

void GarbageCollector::forgetNativeObjects()
{
	cerr << "Need to implement " << "forgetNativeObjects" << endl;
//	rememberSet forget.
//	literalsReferences forget.
//	rescuedEphemerons forget.
//	classCheckReferences forget.
//	nativizedMethods forget.
//	residueObject := globalFramePointerToWalkStack := nil
}

void GarbageCollector::saveSpaces()
{
//fromSpace save.
//oldSpace save.
//toSpace save
}

void GarbageCollector::makeRescuedEphemeronsNonWeak()
{
	for (long index = 1; index <= rescuedEphemerons.size()->_asNative(); index++) {
		rescuedEphemerons[index]->_haveNoWeaks();
	}
}

void GarbageCollector::loadSpaces()
{
//	fromSpace.load();
//	oldSpace.load();
//	toSpace.load();
}
;
