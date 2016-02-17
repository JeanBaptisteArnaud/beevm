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
#include "../DataStructures/Memory.h"


// just until I fix my environment
#define NULL 0

using namespace std;
using namespace Bee;


GarbageCollector::GarbageCollector()
{
	residueObject = 0;
}

void GarbageCollector::useOwnVMVariables()
{
	vm.initializeFor(new VMVariables());
}

void GarbageCollector::useHostVMVariables()
{
	vm.initializeForHostVM();
}

void GarbageCollector::updateFromMemory()
{
	fromSpace.loadFrom(*memory->fromSpace);
	toSpace  .loadFrom(*memory->toSpace);
	oldSpace .loadFrom(*memory->oldSpace);
}

void GarbageCollector::collect()
{

}

void GarbageCollector::rescueEphemeron(oop_t *ephemeron)
{
	this->followCountStartingAt((slot_t*)ephemeron, ephemeron->_extendedSize(), 1);
	rescuedEphemerons.add(ephemeron);
	return;
}

bool GarbageCollector::followEphemeronsCollectingUnknowns()
{
	bool rescan = false;
	while (!ephemerons.isEmpty())
	{
		oop_t *ephemeron = ephemerons.pop();
		if (ephemeron != KnownObjects::nil)
		{
			if (this->checkReachablePropertyOf(ephemeron))
			{
				this->followCountStartingAt((slot_t*)ephemeron, ephemeron->_extendedSize(), 1);
				rescan = true;
			} else {
				unknowns.add(ephemeron);
			}
		}
	}
	return rescan;
}

void GarbageCollector::rescueEphemerons()
{
	bool rescued = false;
	VMArray aux;
	while (!ephemerons.isEmpty()) {
		if (this->followEphemeronsCollectingUnknowns()) {
			aux = ephemerons;
			ephemerons = unknowns;
			unknowns = aux;
		} else {
			for (long index = 1; index <= unknowns.size()->_asNative(); index++)
				this->rescueEphemeron(unknowns[index]);
			rescued = true;
		}
		unknowns.reset();
	}

	if (rescued)
		this->someEphemeronsRescued();
}

void GarbageCollector::clearPolymorphicMethodCache()
{
	for (int index = 1; index <= 0x4000; index++)
	{
		vm.globalLookupCacheAtPut(index, KnownObjects::nil);
	}
}

void GarbageCollector::followStack()
{
	ulong *frame = this->framePointerToStartWalkingTheStack();
	ulong endMarker = 0;

	do 
	{
		ulong nextFrame = *frame;
		if (nextFrame == endMarker)
		{
			ulong start;
			ulong size = ((ulong)nextFrame - (ulong)frame) / 4;
			if (frame[0] == (ulong)*vm.debugFrameMarker) // was frame[1]
			{
				this->followCountStartingAt((slot_t*)frame, 5, 3);
				start = 9;
			} else
				start = 3;

			this->followFrameCountStartingAt((slot_t*)frame, size, start);
			frame = (ulong *) nextFrame;
		}
		else
			break;
	} while(true);

}

void GarbageCollector::followFrameCountStartingAt(slot_t *frame, ulong size, ulong startIndex)
{
	oop_t *gapMarker = (oop_t*)2;
	ulong start = startIndex;
	ulong index = startIndex;
	
	while (index < size)
	{
		oop_t* object = frame[index-1]; // was index
		if (object == gapMarker)
		{
			oop_t *callbackFrame = frame[index]; // was index+1
			this->followCountStartingAt(frame, index - start, start);
			this->followCountStartingAt((slot_t*)callbackFrame, 5, 1);
			index = ((ulong)callbackFrame - (ulong)frame) / 4;
			index = index + 7;
			start = index + 1;
		}
		index = index + 1;
	}

	this->followCountStartingAt(frame, size - start + 1, start);
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
//	rememberedSet forget.
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
