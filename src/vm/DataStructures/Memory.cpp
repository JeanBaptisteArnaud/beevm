/*
 * Memory.cpp
 *
 *  Created on: 14 janv. 2016
 *      Author: jbapt
 */


#include <iostream>

#include "Memory.h"
#include <windows.h>
#include <vector>

using namespace Bee;
using namespace std;

Memory::Memory() {
}

Memory * Memory::singleton = 0;

void GCSpace::garbageCollect()
{
	//	^Memory current garbageCollect: self
}

void Memory::setGC(GarbageCollector * localCollector)
{
	this->collector = localCollector;
}

GCSpace * Memory::growIfNeeded(ulong size)
{
	ulong total = currentSpace->size();
	ulong used = currentSpace->used();
	long free = (long)(total - size);
	
	if ((used / total) > 0.9 || (free < (long)size))
	{
		return this->acquireMoreSpace();
	} 
	else
	{
		return currentSpace;
	}
}

void Memory::addSpace(GCSpace * space)
{
	spaces.push_back(space);
}

void Memory::addCollectedSpace(GCSpace * space)
{
	collectedSpaces.push_back(space);
}

void Memory::initializeCollector()
{
}

void Memory::createPinnedSpace()
{
}

void Memory::startUp()
{
//	this->initializeCollector();
//	this->createNextSpace();
//	this->acquireMoreSpace();
//	this->createPinnedSpace();
}

GCSpace * Memory::acquireMoreSpace()
{
	currentSpace = nextSpace;
	this->addSpace(nextSpace);
	this->createNextSpace();
	this->addCollectedSpace(currentSpace);
	return currentSpace;
}

ulong* Memory::VM()
{
	
	ulong  size    = 0x42000;
	ulong* address = (ulong *)VirtualAlloc((void *) 0x10000000, size,
											  MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	
	if (!address)
		osError();

	GCSpaceInfo info = GCSpaceInfo();
	ulong *limit = (ulong *)((ulong)address + size);
	info.setBase(address);
	info.setNextFree(address);
	info.setCommitedLimit(limit);
	info.setSoftLimit(limit);
	info.setReservedLimit(limit);

	GCSpace *space   = new GCSpace();
	space->setInfo(info);
	//space.load();
	currentSpace = space;
	this->addSpace(space);
	space->setBase(address);
	space->setNextFree(address);
	space->setCommitedLimit(limit);
	space->setSoftLimit(limit);
	space->setReservedLimit(limit);
	return address;
}

GCSpaceInfo Memory::allocateWithoutFinalization(ulong size)
{

	cerr << "should not be here allocateWithoutFinalization" << endl;
	ulong * address = (ulong *) VirtualAlloc((void *) 0, size,
	MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!address)
		cout << GetLastError() << endl;
	GCSpaceInfo info = GCSpaceInfo();
	ulong * limit = (ulong *) (((ulong) address + size));
	info.setBase(address);
	info.setNextFree(address);
	info.setCommitedLimit(limit);
	info.setSoftLimit(limit);
	info.setReservedLimit(limit);
	return info;
}

GCSpace Memory::dynamicNew(ulong size)
{
	GCSpace space = GCSpace();
	ulong * address = (ulong*)VirtualAlloc((void *)0, 1024 * 1024 * 1024, MEM_RESERVE, PAGE_READWRITE);
	ulong * localAddress = (ulong*)VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
	if (!localAddress) 
	{
		cout << GetLastError() << endl;
		cerr << address << endl;
		cerr << localAddress << endl;
		cerr << size << endl;
		cerr << 512 * 1024 << endl;

	}

	GCSpaceInfo info = GCSpaceInfo();
	ulong * limit = (ulong *) (((ulong) localAddress + size));
	info.setBase(localAddress);
	info.setNextFree(localAddress);
	info.setCommitedLimit(limit);
	info.setSoftLimit(limit);
	info.setReservedLimit((ulong *) (512 * 1024));
	space.setInfo(info);
	space.load();
	this->addSpace(&space);
	space.setBase(localAddress);
	space.setNextFree(localAddress);
	space.setCommitedLimit(limit);
	space.setSoftLimit(limit);
	space.setReservedLimit((ulong *) (512 * 1024));
	return space;
}

Memory * Memory::current()
{
	if (singleton == 0) {
		singleton = new Memory();
		singleton->startUp();
	}
	return singleton;
}

void Memory::releaseEverything()
{
	for (int index = spaces.size(); 0 < index; index = spaces.size())
	{
		GCSpace * localSpace = this->spaces.back();
		this->spaces.pop_back();
		localSpace->release();
	}

	if (collector)
		collector->localSpace.release();
	if (collector)
		collector = 0;

	singleton = 0;
}

void Memory::createNextSpace()
{
	nextSpace = new GCSpace();
	nextSpace->setInfo(this->allocateWithoutFinalization(64 * 1024 * 1024));
	nextSpace->load();
	nextSpace->setSoftLimit(
			(ulong *) nextSpace->getSoftLimit() - (64 * 1024 / 2));
}
