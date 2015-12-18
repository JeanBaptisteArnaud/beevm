/*
 * GarbageCollector.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GarbageCollector.h"
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

void GarbageCollector::loadSpaces() {
	fromSpace.load();
	oldSpace.load();
	toSpace.load();
}

