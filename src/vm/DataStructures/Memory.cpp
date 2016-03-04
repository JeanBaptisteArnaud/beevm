/*
 * Memory.cpp
 *
 *  Created on: 14 janv. 2016
 *      Author: jbapt
 */


#include <iostream>
#include <vector>

#include "Memory.h"
#include "VMVariables.h"
#include "../GarbageCollector/GenerationalGC.h"
#include "../GarbageCollector/MarkAndCompactGC.h"

using namespace Bee;
using namespace std;

Memory::Memory()
{
	flipper = new GenerationalGC();
	compactor = new MarkAndCompactGC();
}

void Memory::growIfTime()
{
	if (fromSpace->percentageOfCommitedUsed() > 90)
		this->commitMoreSpace();
}

void Memory::commitMoreSpace()
{
	fromSpace->commitDelta(1024 * 1024);
	toSpace->commitDelta(1024 * 1024);
}

void Memory::scavengeFromSpace()
{
	flipper->collect();
}

void Memory::collectOldSpace()
{
	compactor->collect();
}


void Memory::useHostVMVariables()
{
	flipper->useHostVMVariables();
	compactor->useHostVMVariables();
}

void Memory::updateFromHostVM()
{
	GCSpaceInfo info;
	info = VMVariablesProxy::hostVMFromSpace();
	fromSpace->loadFrom(info);
	info = VMVariablesProxy::hostVMToSpace();
	  toSpace->loadFrom(info);
	info = VMVariablesProxy::hostVMOldSpace();
	 oldSpace->loadFrom(info);

	KnownObjects::emptyArray = emptyArray;
}

void Memory::updateToHostVM()
{
	GCSpaceInfo info;
	info = VMVariablesProxy::hostVMFromSpace();
	fromSpace->saveTo(info);
	info = VMVariablesProxy::hostVMToSpace();
	  toSpace->saveTo(info);
	info = VMVariablesProxy::hostVMOldSpace();
	 oldSpace->saveTo(info);
}

// next methods still need checking
// not needed in vm
//void Memory::addSpace(GCSpace *space)
//{
//	spaces.push_back(space);
//}

//void Memory::addCollectedSpace(GCSpace * space)
//{
//	collectedSpaces.push_back(space);
//}

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


void Memory::releaseEverything()
{
	cout << "Memory::releaseEverything is a stub" << endl;
}

