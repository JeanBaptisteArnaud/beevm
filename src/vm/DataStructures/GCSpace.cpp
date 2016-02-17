/*
 * GCSpace.cpp
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include <cstdlib>
#include <iostream>
#include <windows.h>

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
	GCSpace result;
	result.loadFrom(GCSpaceInfo::newSized(size));

	return result;
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
}

bool GCSpace::includes(oop_t *object)
{
	return ((ulong) object > (ulong) this->base
			&& (ulong) object < (ulong) this->commitedLimit);
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

void GCSpace::release()
{
	_decommit(base, (ulong *) 0 );
	_free(base, (ulong *) 0 );
}

void GCSpace::setInfo(GCSpaceInfo *localInfo)
{
	info = localInfo;
}

void GCSpace::load()
{
	regionBase = (ulong *) info->getBase();
	base = regionBase;
	base_3 = (ulong *) info->getBase_3();
	commitedLimit = (ulong *) info->getCommitedLimit();
	nextFree = (ulong *) info->getNextFree();

	reservedLimit = (ulong *) info->getReservedLimit();
	softLimit = (ulong *) info->getSoftLimit();
}

ulong GCSpace::reservedSize()
{
	return asUObject(reservedLimit) - asUObject(base);
}

ulong GCSpace::used()
{
	return asUObject(nextFree) - asUObject(base);
}


void GCSpace::save()
{
	info->setBase(base);
	info->setBase_3(base_3);
	info->setCommitedLimit(commitedLimit);
	info->setNextFree(nextFree);
	info->setReservedLimit(reservedLimit);
	info->setSoftLimit(softLimit);
}

void GCSpace::debug()
{
	cerr << "regionBase:" << regionBase << endl;
	cerr << "Base:" << base << endl;
	cerr << "Base_3:" << base_3 << endl;
	cerr << "commitedLimit:" << commitedLimit << endl;
	cerr << "nextFree:" << nextFree << endl;
	cerr << "reservedLimit:" << reservedLimit << endl;
	cerr << "softLimit:" << softLimit << endl;
}


ulong GCSpace::asObject(ulong *smallPointer)
{
	return (ulong)((oop_t*)smallPointer)->_asObject();
}


void GCSpace::dispenseReservedSpace()
{
	this->nextFree = this->commitedLimit;
}

ulong* GCSpace::allocateIfPossible(ulong size)
{
// lockedAllocate probably do not need the mutex
	ulong answer = asObject(this->getNextFree());
	ulong next = answer + size;
	if (next > asObject(softLimit))
		return 0;
	
	this->setNextFree((ulong *)pointerConst(next));
	return (ulong*)answer;
}

// returns a native pointer
ulong* GCSpace::allocateUnsafe(ulong size)
{
// lockedAllocate probably do not need the mutex
	ulong answer = asObject(this->getNextFree());
	ulong next = answer + size;
	if (next > asObject(commitedLimit))
		this->commitMoreMemory();
	this->setNextFree((ulong*)pointerConst(next));
	return (ulong*)answer;
}

oop_t* GCSpace::objectFromBuffer(ulong *buffer, ulong headerSize)
{
	return (oop_t*) ((ulong)buffer + headerSize);
}

oop_t* GCSpace::shallowCopy(oop_t *array)
{
	ulong size = array->_sizeInBytes();
	ulong headerSize = array->_headerSizeInBytes();
	ulong *allocation = this->allocateUnsafe(headerSize + size);
	oop_t *copy = objectFromBuffer(allocation, headerSize);

	long first = -(long)(headerSize/4);
	long last  = array->_size();
	for (int index = first; index < last; index++)
	{
		copy->slot(index) = array->slot(index);
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
	
	copy->slot(-4) = array->slot(-2);
	
	for (ulong index = (array->_size() + 1); index <= newSize; index++)
	{
		copy->slot(index) = KnownObjects::nil;
	}

	copy->_beNotInRememberedSet();
	return copy;
}


void GCSpace::commitMoreMemory()
{
	if (asObject(nextFree) >= asObject(reservedLimit))
		error("out of reserved space");

	ulong padding = 0x8000;
	ulong newLimit = (asObject(nextFree) + padding) & -0x1000;

	ulong limit = this->commitSized(newLimit - asObject(regionBase));
	if (limit != (asObject(regionBase) & -0x1000))
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

	commitedLimit = (ulong *) newLimit;
}

ulong GCSpace::commitSized(ulong total)
{
	return (ulong)_commit(asObject(regionBase), total);
}

ulong GCSpace::commitDelta(ulong delta)
{
	return (ulong)_commit(asObject(commitedLimit), delta);
}

void GCSpace::decommitSlack()
{
// -0x1000 === (long) 0xFFFFFC18
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
	return (int)(double(used()) * 100 / asObject(commitedLimit));
}


ulong* GCSpace::_commit(ulong limit, ulong delta)
{
	return (ulong *) VirtualAlloc((ulong *) limit, delta, MEM_COMMIT, PAGE_READWRITE);
}

void GCSpace::_decommit(ulong *limit, ulong *delta)
{
	VirtualFree((void *) limit, (ulong) delta, MEM_DECOMMIT);
}

void GCSpace::_free(ulong * limit, ulong *delta)
{
	VirtualFree((void *) limit, (ulong) delta, MEM_RELEASE);
}

