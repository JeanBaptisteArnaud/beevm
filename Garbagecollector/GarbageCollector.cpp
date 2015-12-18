/*
 * GarbageCollector.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GarbageCollector.h"
// just until I fix my environment



#include <cstddef>
#include <iostream>

//using namespace std;
using namespace Bee;

GarbageCollector* GarbageCollector::flipper = NULL;

GarbageCollector::GarbageCollector() {
	fromSpace = GCSpace::currentFrom();
	toSpace = GCSpace::currentTo();
	oldSpace = GCSpace::old();
	auxSpace = new GCSpace();
	ephemerons = new VMArray();
	weakContainers = new VMArray();
	rescuedEphemerons = new ReferencedVMArray();
	rememberSet = new ReferencedVMArray();
	literalsReferences = new ReferencedVMArray();
	nativizedMethods = new ReferencedVMArray();
	classCheckReferences = new ReferencedVMArray();
	stack = new VMArray();
	unknowns = new VMArray();
}

GarbageCollector* GarbageCollector::currentFlipper() {
	if (flipper == NULL)
		flipper = new GarbageCollector();
	return flipper;
}

void GarbageCollector::collect() {

}

void GarbageCollector::clearPolymorphicMethodCache() {
	for (int index = 1; index <= 0x4000; index++) {
		MEM0x1002EC20[index] = nil;
	}
}

void GarbageCollector::follow(unsigned long pointer, int count,
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

unsigned long GarbageCollector::framePointerToStartWalkingTheStack()
{
//| frame |
//frame := globalFramePointerToWalkStack == nil
//	ifTrue: [self _framePointer]
//	ifFalse: [globalFramePointerToWalkStack].
//^self dereference: frame
}


void GarbageCollector::followStack(){
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

void GarbageCollector::follow(unsigned long pointer) {
	// TODO self rememberIfWeak: pointer.
	this->follow(pointer,  _strongPointersSize(pointer) , 0);
}


unsigned long GarbageCollector::dereference(unsigned long pointer) {
	return memoryAt(pointer);
}

void GarbageCollector::loadSpaces() {
	fromSpace.load();
	oldSpace.load();
	toSpace.load();
}

