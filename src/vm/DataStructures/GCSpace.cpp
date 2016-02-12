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

void GCSpace::loadFrom(GCSpace space)
{
	this->setSoftLimit(space.getSoftLimit());
	this->setBase(space.getBase());
	this->setCommitedLimit(space.getCommitedLimit());
	this->setReservedLimit(space.getReservedLimit());
	this->setNextFree(space.getNextFree());
	this->setRegionBase(space.getRegionBase());
}

void GCSpace::reset()
{
}

bool GCSpace::includes(oop_t *object)
{
	return ((ulong) object > (ulong) this->base
			&& (ulong) object < (ulong) this->commitedLimit);
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

void GCSpace::setInfo(GCSpaceInfo localInfo)
{
	info = localInfo;
}

void GCSpace::load()
{
	regionBase = (ulong *) info.getBase();
	base = regionBase;
	base_3 = (ulong *) info.getBase_3();
	commitedLimit = (ulong *) info.getCommitedLimit();
	nextFree = (ulong *) info.getNextFree();

	reservedLimit = (ulong *) info.getReservedLimit();
	softLimit = (ulong *) info.getSoftLimit();
}

ulong GCSpace::size()
{
	return (ulong) reservedLimit - (ulong) base;
}

ulong GCSpace::used()
{
	return (ulong) nextFree - (ulong) base;
}

void GCSpace::save()
{
	info.setBase(base);
	info.setBase_3(base_3);
	info.setCommitedLimit(commitedLimit);
	info.setNextFree(nextFree);
	info.setReservedLimit(reservedLimit);
	info.setSoftLimit(softLimit);
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

GCSpace GCSpace::currentFrom()
{
//	GCSpace space = GCSpace();
//	GCSpaceInfo info = GCSpaceInfo::currentFrom();
//	space.setInfo(info);
//	space.load();
	GCSpace space = GCSpace::dynamicNew(1024 * 12);
	return space;
}

GCSpace GCSpace::currentTo()
{
//	GCSpace space = GCSpace();
//	GCSpaceInfo info = GCSpaceInfo::currentTo();
//	space.setInfo(info);
//	space.load();
	GCSpace space = GCSpace::dynamicNew(1024 * 12);
	return space;
}

GCSpace GCSpace::old()
{
//	GCSpace space = GCSpace();
//	GCSpaceInfo info = GCSpaceInfo::old();
//	space.setInfo(info);
//	space.load();
	GCSpace space = GCSpace::dynamicNew(1024 * 12);
	return space;
}

GCSpace GCSpace::dynamicNew(unsigned long size)
{
	return Memory::current()->dynamicNew(size);
	//	GCSpace space = GCSpace();
//	GCSpaceInfo info = Memory::current()->allocateWithoutFinalization(size);
//	space.setInfo(info);
//	space.load();
	// should be remove
	// return space;
}

void GCSpace::decommitSlack()
{
// -0x1000 === (long) 0xFFFFFC18
	ulong limit  = (long)this->getNextFree();
	ulong padded =  (limit + 0xFFF) && 0xFFFFF000;
	long delta = (long)this->getCommitedLimit() - padded;
	
	if (delta < 0)
		this->grow();

	if (delta > 0)
	{
		_decommit((ulong*)padded, (ulong*)delta);
		this->setCommitedLimit((ulong*) padded);
	}
}

ulong* GCSpace::obtainFreeSpaceAndAllocate(ulong size)
{
	this->dispenseReservedSpace();
	this->garbageCollect();
	this->hostVMGrow();
	return this->allocate(size);
}

GCSpace* GCSpace::hostVMGrow()
{
	if (nextFree <= commitedLimit)
		return this;
	if (nextFree >= reservedLimit)
		cerr << "stamp the reserved limit" << endl;
	ulong newLimit = ((ulong) nextFree + 0x8000);
	ulong * localAddress = _commit((ulong) base, newLimit - (ulong) base);
	if (!localAddress) {
		cout << GetLastError() << endl;
		cerr << base << endl;
		cerr << localAddress << endl;
		cerr << newLimit << endl;
		cerr << 512 * 1024 << endl;

	}
	commitedLimit = (ulong *) newLimit;
	return this;
}

void GCSpace::dispenseReservedSpace()
{
	this->nextFree = this->commitedLimit;
}

ulong* GCSpace::allocate(ulong size)
{
// lockedAllocate probably do not need the mutex
	ulong answer = (ulong) this->getNextFree();
	this->setNextFree((ulong *) (answer + size));
	this->hostVMGrow();
	return (ulong*) answer;
	//return obtainFreeSpaceAndAllocate(size);
}

oop_t* GCSpace::objectFromBuffer(ulong *buffer, ulong headerSize)
{
	return (oop_t*) ((ulong)buffer + headerSize);
}

oop_t* GCSpace::shallowCopy(oop_t *array)
{
// method a little bit different than in Image
	ulong headerSize = array->_headerSizeInBytes();
	ulong total = headerSize + array->_sizeInBytes();
	ulong *buffer = this->allocate(total);
	oop_t *copy = objectFromBuffer(buffer, headerSize);

	long first = -(long)(headerSize/4);
	long last  = array->_size();
	for (int index = first; index < last; index++)
	{
		copy->setSlot(index, array->slot(index));
	}

	copy->_beNotInRememberedSet();
	return copy;
}

oop_t* GCSpace::shallowCopyGrowingTo(oop_t *array, ulong newSize)
{
	ulong headerSize = 4;
	ulong total = (headerSize + newSize) * 4;
	ulong *buffer = this->allocate(total);
	oop_t *copy = this->objectFromBuffer(buffer, 16);

	for (int index = -2; index < (int)array->_size(); index++)
	{
		copy->setSlot(index, array->slot(index));
	}

	copy->_beExtended();
	copy->_basicSize(4);
	copy->_extendedSize(newSize);
	
	copy->setSlot(-4, array->slot(-2));
	
	for (ulong index = (array->_size() + 1); index <= newSize; index++)
	{
		copy->setSlot(index, KnownObjects::nil);
	}

	copy->_beNotInRememberedSet();
	return copy;
}

GCSpace * GCSpace::grow()
{
	return this->hostVMGrow();
	//return Memory::current()->acquireMoreSpace();
}

// from send inliner

ulong* GCSpace::_commit(ulong limit, ulong delta)
{
	return (ulong *) VirtualAlloc((ulong *) limit, delta, MEM_COMMIT,
	PAGE_READWRITE);
}

void GCSpace::_decommit(ulong *limit, ulong *delta)
{
	VirtualFree((void *) limit, (ulong) delta, MEM_DECOMMIT);
}

void GCSpace::_free(ulong * limit, ulong *delta)
{
	VirtualFree((void *) limit, (ulong) delta, MEM_RELEASE);
}

