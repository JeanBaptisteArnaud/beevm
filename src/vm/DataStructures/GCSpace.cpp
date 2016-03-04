/*
 * GCSpace.cpp
 *
 *  Created on: 18 dec. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include <cstdlib>
#include <iostream>


#include "GCSpace.h"
#include "ObjectFormat.h"
#include "GCSpaceInfo.h"
#include "KnownObjects.h"
#include "Memory.h"

using namespace Bee;
using namespace std;

GCSpace::GCSpace()
{
}

GCSpace GCSpace::dynamicNew(ulong size)
{
	GCSpaceInfo info((ulong)Bee::_commit(0, size), size);

	GCSpace result;
	result.loadFrom(info);

	return result;
}

GCSpace* GCSpace::dynamicNewP(ulong size)
{
	GCSpaceInfo info((ulong)Bee::_commit(0, size), size);

	GCSpace *result = new GCSpace;
	result->loadFrom(info);

	return result;
}

void GCSpace::dynamicFree()
{
	Bee::_free(base, 0);
}

void GCSpace::loadFrom(GCSpace &space)
{
	this->setSoftLimit(space.getSoftLimit());
	this->setBase(space.getBase());
	this->setCommitedLimit(space.getCommitedLimit());
	this->setReservedLimit(space.getReservedLimit());
	this->setNextFree(space.getNextFree());
	this->setRegionBase(space.getRegionBase());
}

void GCSpace::loadFrom(GCSpaceInfo &info)
{
	this->setSoftLimit(info.getSoftLimit());
	this->setBase(info.getBase());
	this->setCommitedLimit(info.getCommitedLimit());
	this->setReservedLimit(info.getReservedLimit());
	this->setNextFree(info.getNextFree());
	this->setRegionBase(info.getBase());
}

void GCSpace::reset()
{
	this->setNextFree(this->getBase());
}

bool GCSpace::includes(oop_t *object)
{
	return !object->isSmallInteger() &&
	       asUObject(this->base) <= (ulong)object &&
	       (ulong)object         <= asUObject(this->commitedLimit);
}

bool GCSpace::isBelowNextFree(oop_t *object)
{
	return  (ulong *) object <= this->getNextFree();
}


bool GCSpace::isReferredBy(oop_t *object)
{
	if (object->isSmallInteger())
		return false;

	ulong size = object->_pointersSize();
	for (long i = -1; i < (long)size - 1; i++)
	{
		oop_t *slot = object->slot(i);
		if (!slot->isSmallInteger() && this->includes(slot))
			return true;
	}
	return false;
}


ulong GCSpace::reservedSize()
{
	return asUObject(reservedLimit) - asUObject(base);
}

ulong GCSpace::commitedSize()
{
	return asUObject(commitedLimit) - asUObject(base);
}

ulong GCSpace::used()
{
	return asUObject(nextFree) - asUObject(base);
}


void GCSpace::saveTo(GCSpaceInfo &info)
{
	info.setBase(base);
	info.setBase_3(base_3);
	info.setCommitedLimit(commitedLimit);
	info.setNextFree(nextFree);
	info.setReservedLimit(reservedLimit);
	info.setSoftLimit(softLimit);
}


void GCSpace::dispenseReservedSpace()
{
	this->softLimit = this->commitedLimit;
}

ulong* GCSpace::allocateIfPossible(ulong size)
{
// lockedAllocate probably do not need the mutex
	ulong answer = asUObject(this->getNextFree());
	ulong next = answer + size;
	if (next > asUObject(softLimit))
		return 0;
	
	this->setNextFree((ulong *)pointerConst(next));
	return (ulong*)answer;
}

// returns a native pointer
ulong* GCSpace::allocateUnsafe(ulong size)
{
// lockedAllocate probably do not need the mutex
	ulong answer = asUObject(this->getNextFree());
	ulong next = answer + size;
	if (next > asUObject(commitedLimit))
		this->commitMoreMemory(next);
	this->setNextFree((ulong*)pointerConst(next));
	return (ulong*)answer;
}

oop_t* GCSpace::objectFromBuffer(ulong *buffer, ulong headerSize)
{
	return (oop_t*) ((ulong)buffer + headerSize);
}

oop_t* GCSpace::shallowCopy(oop_t *object)
{
	ulong size = object->_sizeInBytes();
	ulong headerSize = object->_headerSizeInBytes();
	ulong *allocation = this->allocateUnsafe(headerSize + size);
	oop_t *copy = objectFromBuffer(allocation, headerSize);

	long first = -(long)(headerSize/4);
	long last  = size / 4;
	for (int index = first; index < last; index++)
	{
		copy->slot(index) = object->slot(index);
	}

	copy->_beNotInRememberedSet();
	return copy;
}

oop_t* GCSpace::shallowCopyGrowingTo(oop_t *array, ulong newSize)
{
	ulong headerSize = 4;
	ulong total = (headerSize + newSize) * 4;
	ulong *buffer = this->allocateUnsafe(total);
	oop_t *copy = this->objectFromBuffer(buffer, 16);

	for (int index = -2; index < (int)array->_size(); index++)
	{
		copy->slot(index) = array->slot(index);
	}

	copy->_beExtended();
	copy->_basicSize(4);
	copy->_extendedSize(newSize);
	
	copy->slot(-4) = copy->slot(-2);
	
	for (ulong index = array->_size(); index < newSize; index++)
	{
		copy->slot(index) = KnownObjects::nil;
	}

	copy->_beNotInRememberedSet();
	return copy;
}

oop_t * GCSpace::firstObject()
{
	return headerToObject(asObject(this->getBase()));
}

oop_t * GCSpace::firstObjectAfterMark()
{
	return headerToObjectCheckProxee(asObject(this->getBase()));
}


void GCSpace::commitMoreMemory(ulong neededLimit)
{
	if (neededLimit >= asUObject(reservedLimit))
		error("out of reserved space");

	ulong padding = 0x8000;
	ulong newLimit = (neededLimit + padding) & -0x1000;

	ulong limit = this->commitSized(newLimit - asUObject(regionBase));
	if (limit != (asUObject(commitedLimit)))
	//if (limit != (asUObject(regionBase) & -0x1000))
		error("commiting more memory failed");
	
	commitedLimit = (ulong*)pointerConst(newLimit);
}

// was hostVMGrow, not needed for now (for ever?)
void GCSpace::commitMoreMemoryIfNeeded()
{
	if (nextFree <= commitedLimit)
		return;
	if (nextFree >= reservedLimit)
		error("out of reserved space");
	ulong newLimit = ((ulong) nextFree + 0x8000);
	ulong * localAddress = _commit((ulong) base, newLimit - (ulong) base);
	if (!localAddress)
		osError();

	commitedLimit = (ulong *)pointerConst(newLimit);
}

ulong GCSpace::commitSized(ulong total)
{
	ulong previouslyUsed = asUObject(commitedLimit)-asUObject(base);
	ulong difference = total-previouslyUsed;
	return (ulong)_commit(asUObject(commitedLimit), difference);
}

ulong GCSpace::commitDelta(ulong delta)
{
	return (ulong)_commit(asUObject(commitedLimit), delta);
}

void GCSpace::decommitSlack()
{
	ulong limit  = (long)this->getNextFree();
	ulong padded =  (limit + 0xFFF) && 0xFFFFF000;
	long delta = (long)this->getCommitedLimit() - padded;
	
	if (delta < 0)
		this->commitMoreMemoryIfNeeded();

	if (delta > 0)
	{
		_decommit((ulong*)padded, (ulong*)delta);
		this->setCommitedLimit((ulong*) padded);
	}
}

int GCSpace::percentageOfCommitedUsed()
{
	return (int)(double(used()) * 100 / asUObject(commitedLimit));
}



ulong* GCSpace::getBase()
{
	return base;
}

ulong* GCSpace::getNextFree()
{
	return nextFree;
}

ulong* GCSpace::getCommitedLimit()
{
	return commitedLimit;
}

ulong* GCSpace::getReservedLimit()
{
	return reservedLimit;
}

ulong* GCSpace::getSoftLimit()
{
	return softLimit;
}

ulong* GCSpace::getRegionBase()
{
	return regionBase;
}



void GCSpace::setBase(ulong *localBase)
{
	base = localBase;
}

void GCSpace::setRegionBase(ulong *localRegionBase)
{
	regionBase = localRegionBase;
}

void GCSpace::setNextFree(ulong *localNextFree)
{
	nextFree = localNextFree;
}

void GCSpace::setCommitedLimit(ulong *localCommitedLimit)
{
	commitedLimit = localCommitedLimit;
}

void GCSpace::setReservedLimit(ulong *localReservedLimit)
{
	reservedLimit = localReservedLimit;
}

void GCSpace::setSoftLimit(ulong *localSoftLimit)
{
	softLimit = localSoftLimit;
}

void GCSpace::print(ostream &stream)
{
	stream << "Base: " << base << "\t";
	stream << "nextFree:" << nextFree << "\t";
	stream << "softLimit:" << softLimit << "\t";
	stream << "commitedLimit:" << commitedLimit << "\t";
	stream << "reservedLimit:" << reservedLimit;
//	stream << "regionBase:" << regionBase << endl;
//	stream << "Base_3:" << base_3 << endl;
}
