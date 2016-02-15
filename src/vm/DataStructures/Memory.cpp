/*
 * Memory.cpp
 *
 *  Created on: 14 janv. 2016
 *      Author: jbapt
 */


#include <iostream>
#include <vector>
#include <windows.h>

#include "Memory.h"
#include "../GarbageCollector/GenerationalGC.h"

using namespace Bee;
using namespace std;

Memory::Memory()
{
	flipper = new GenerationalGC();
}

void Memory::growIfTime()
{
	if (fromSpace->percentageOfCommitedUsed() > 90 )
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

