/*
 * GarbageCollector.cpp
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GarbageCollector.h"
#include "GenerationalGC.h"

// just until I fix my environment
#define NULL 0

#include <cstddef>
#include <iostream>

//using namespace std;
using namespace Bee;

GarbageCollector* GarbageCollector::flipper = NULL;

GarbageCollector::GarbageCollector() {
	fromSpace = GCSpace::currentFrom();
	toSpace = GCSpace::currentTo();
	oldSpace = GCSpace::old();
	residueObject = 0;
}

GarbageCollector* GarbageCollector::currentFlipper() {
	if (flipper == NULL)
		flipper = new GenerationalGC();
	return flipper;
}

void GarbageCollector::collect() {

}

bool GarbageCollector::followEphemeronsCollectingUnknowns() {
	//TODO
	return false;
}

void GarbageCollector::rescueEphemeron(unsigned long *ephemeron) {
	//self follow: ephemeron count: ephemeron _extendedSize startingAt: 1.
	//rescuedEphemerons add: ephemeron
	return;
}

void GarbageCollector::someEphemeronsRescued() {
	//self holdReferenceTo: rescuedEphemerons contents
}

void GarbageCollector::rescueEphemerons() {
	bool rescued;//, rescan = 0;
	// need to check probably here.
	// copy or value
	VMArray aux;
	while (!ephemerons.isEmpty()) {
		if (this->followEphemeronsCollectingUnknowns()) {
			aux = ephemerons;
			ephemerons = unknowns;
			unknowns = aux;
		} else {
			for (ulong ephemeronIndex = 0; ephemeronIndex <= ephemerons.size();
					ephemeronIndex++)
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
		MEM_JIT_globalMethodCache[index] = nil;
	}
}

void GarbageCollector::follow(unsigned long *pointer, int count,
		unsigned long start) {
//	| index objects limit |
//	stack := self localStack.
//	objects := root.
//	index := base - 1.
//	limit := index + size.
//	[
//		[index < limit] whileTrue: [| object |
//			index := index + 1.
//			object := objects _basicAt: index.
//			(self arenaIncludes: object) ifTrue: [
//				object _isProxy
//					ifTrue: [objects _basicAt: index put: object _proxee]
//					ifFalse: [| moved |
//						index < limit ifTrue: [
//							stack
//								push: objects;
//								push: index;
//								push: limit].
//						moved := self moveToOldOrTo: object.
//						objects _basicAt: index put: moved.
//						self rememberIfWeak: moved.
//						index := -1.
//						limit := index + moved _strongPointersSize.
//						objects := moved]]].
//		stack isEmpty]
//		whileFalse: [
//			limit := stack pop.
//			index := stack pop.
//			objects := stack pop]

}



void GarbageCollector::followStack() {
//	unsigned long frame = this->framePointerToStartWalkingTheStack();
//	while(memoryAt(frame)){
//		size = nextFrame - frame;
//		basicAT
//	}
//
//
//			(frame _basicAt: 2) == debugFrameMarker
//				ifTrue: [
//					self follow: frame count: 5 startingAt: 3.
//					start := 9]
//				ifFalse: [start := 3].
//			self followFrame: frame count: size startingAt: start.
//			frame := nextFrame]
}

void GarbageCollector::follow(unsigned long *pointer) {
// TODO self rememberIfWeak: pointer.
//this->follow(pointer, _strongPointersSize(pointer), 0);
}

void GarbageCollector::fixWeakContainers() {
	for (ulong index = 1; index <= weakContainers.size(); index++) {
		this->fixReferencesOrSetTombstone(weakContainers[index]);
	}
	weakContainers.reset();
}


void GarbageCollector::forgetNativeObjects() {
//	rememberSet forget.
//	literalsReferences forget.
//	rescuedEphemerons forget.
//	classCheckReferences forget.
//	nativizedMethods forget.
//	residueObject := globalFramePointerToWalkStack := nil
}

void GarbageCollector::saveSpaces() {
//fromSpace save.
//oldSpace save.
//toSpace save
}
void GarbageCollector::makeRescuedEphemeronsNonWeak() {
//	rescuedEphemerons do: [:ephemeron | ephemeron _haveNoWeaks]
}

void GarbageCollector::loadSpaces() {
	fromSpace.load();
	oldSpace.load();
	toSpace.load();
}

