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


using namespace std;
using namespace Bee;


GarbageCollector::GarbageCollector()
{
}

void GarbageCollector::initialize()
{
	this->initLocals();
	this->initNonLocals();
}

void GarbageCollector::initLocals()
{
	//stack.setSpace(&localSpace);
	//unknowns.setSpace(&localSpace);
	//ephemerons.setSpace(&localSpace);
	//weakContainers.setSpace(&localSpace);
	stack.setSpace(&oldSpace);
	unknowns.setSpace(&oldSpace);
	ephemerons.setSpace(&oldSpace);
	weakContainers.setSpace(&oldSpace);

}

void GarbageCollector::initNonLocals()
{
	rememberedSet.setSpace(&oldSpace);
	literalsReferences.setSpace(&oldSpace);
	nativizedMethods.setSpace(&oldSpace);
	classCheckReferences.setSpace(&oldSpace);
	rescuedEphemerons.setSpace(&oldSpace);
}

void GarbageCollector::collect()
{
	this->updateFromMemory();

	this->setUpLocals();
	this->setUpNonLocals();

	this->doCollect();

	this->updateToMemory();
}


void GarbageCollector::setUpLocals()
{
	localSpace.reset();
	stack.emptyReserving(1024);
	unknowns.emptyReserving(1025);
	ephemerons.emptyReserving(1026);
	weakContainers.emptyReserving(1027);
}

void GarbageCollector::setUpNonLocals()
{
	rememberedSet       .updateFromReferer();
	literalsReferences  .updateFromReferer();
	nativizedMethods    .updateFromReferer();
	classCheckReferences.updateFromReferer();
	rescuedEphemerons   .updateFromReferer();
}


void GarbageCollector::useOwnVMVariables()
{
	VMVariables *vars = new VMVariables();
	vm.initializeFor(vars, memory);
	this->initializeFromVMVariables();
}

void GarbageCollector::useHostVMVariables()
{
	vm.initializeForHostVM();
	this->initializeFromVMVariables();
}

void GarbageCollector::initializeFromVMVariables()
{
	rememberedSet       .setReferer(vm.rememberedSet());
	literalsReferences  .setReferer(vm.literalsReferences());
	nativizedMethods    .setReferer(vm.nativizedMethods());
	classCheckReferences.setReferer(vm.codeCacheObjectReferences());
	rescuedEphemerons   .setReferer(vm.rescuedEphemerons());
}

void GarbageCollector::updateFromMemory()
{
	fromSpace.loadFrom(*memory->fromSpace);
	toSpace  .loadFrom(*memory->toSpace);
	oldSpace .loadFrom(*memory->oldSpace);

	this->tombstone(memory->residueObject);
}

void GarbageCollector::updateToMemory()
{
	memory->fromSpace->loadFrom(fromSpace);
	memory->toSpace  ->loadFrom(toSpace);
	memory->oldSpace ->loadFrom(oldSpace);
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
	while (!ephemerons.isEmpty())
	{
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
	for (ulong index = 1; index <= 4000; index++)
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
		if (nextFrame != endMarker)
		{
			ulong start;
			ulong size = ((ulong)nextFrame - (ulong)frame) / 4;
			if (frame[1] == (ulong)vm.debugFrameMarker) // was frame[1]
			{
				this->followCountStartingAt((slot_t*)frame, 5, 3);
				start = 9;
			}
			else
				start = 3;

			this->followFrameCountStartingAt((slot_t*)frame, size, start);
			frame = (ulong *)nextFrame;
		}
		else
			break;
	} while (true);
}

ulong * GarbageCollector::framePointerToStartWalkingTheStack()
{
	return vm.framePointerToStartWalkingTheStack();
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


void GarbageCollector::makeRescuedEphemeronsNonWeak()
{
	for (ulong index = 1; index <= rescuedEphemerons.size()->_asNative(); index++) {
		rescuedEphemerons[index]->_haveNoWeaks();
	}
}

