/*
 * Memory.h
 *
 *  Created on: 14 janv. 2016
 *      Author: jbapt
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "../GarbageCollector/GarbageCollector.h"
#include "GCSpace.h"
#include "VMArray.h"
#include <vector>

namespace Bee {

class Memory {

	static Memory * singleton;
protected:


	//Collector

public:
	GarbageCollector * collector;
	GCSpace * currentSpace;
	std::vector<GCSpace * > spaces;
	std::vector<GCSpace * > collectedSpaces;
	GCSpace * nextSpace;
	static Memory * current();
	Memory();
	GCSpace * growIfNeeded(ulong size);
	GCSpace dynamicNew(ulong size);
	ulong * obtainFreeSpaceAndAllocate(ulong size);
	ulong * VM();
	GCSpaceInfo allocateWithoutFinalization(ulong);
	void initializeCollector();
	void setGC(GarbageCollector * collector);
	void startUp();
	void createPinnedSpace();
	void releaseEverything();
	GCSpace * acquireMoreSpace();
	void createNextSpace();
	void addSpace(GCSpace *);
	void addCollectedSpace(GCSpace *);
};

}

#endif /* MEMORY_H_ */
