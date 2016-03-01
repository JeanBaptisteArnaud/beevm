/*
 * GCSpace.h
 *
 *  Created on: 18 dec. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GCSPACE_H_
#define GCSPACE_H_

#include <iostream>

#include "Bee.h"
#include "GCSpaceInfo.h"

namespace Bee
{

class GCSpace
{
public:
	GCSpace();

	static GCSpace  dynamicNew(ulong size);
	static GCSpace* dynamicNewP(ulong size);

	void dynamicFree();

	// accessor
	void debug();
	ulong reservedSize();
	ulong used();
	
	void loadFrom(GCSpace &from);
	void loadFrom(GCSpaceInfo &info);
	void saveTo(GCSpaceInfo &info);
	void reset();


	void dispenseReservedSpace();

	ulong* allocateIfPossible(ulong size);
	ulong* allocateUnsafe(ulong size);

	oop_t* objectFromBuffer(ulong *buffer, ulong headerSize);
	oop_t* shallowCopy(oop_t *object);
	oop_t* shallowCopyGrowingTo(oop_t *array , ulong newSize);
	oop_t* firstObject();

	void commitMoreMemory();
	void commitMoreMemoryIfNeeded();
	ulong commitSized(ulong total);
	ulong commitDelta(ulong delta);
	void decommitSlack();
	
	int percentageOfCommitedUsed();

	bool includes(oop_t *object);
	bool isBelowNextFree(oop_t * object);
	bool isReferredBy(oop_t *object);

	
	ulong* getBase();
	ulong* getNextFree();
	ulong* getCommitedLimit();
	ulong* getReservedLimit();
	ulong* getRegionBase();
	ulong* getSoftLimit();
	void setBase(ulong * localBase);
	void setNextFree(ulong * localNextFree);
	void setCommitedLimit(ulong * localCommitedLimit);
	void setReservedLimit(ulong * localReservedLimit);
	void setSoftLimit(ulong * localSoftLimit);
	void setRegionBase(ulong  * localRegionBase);

	void print(std::ostream &stream);


	static const ulong instVarCount = 8;


// protected:
	ulong *info;
	ulong *base;
	ulong *commitedLimit;
	ulong *nextFree;
	ulong *reservedLimit;
	ulong *softLimit;
	ulong *base_3;
	ulong *regionBase;

	//ulong *softLimit;
	//ulong *base;
	//ulong *nextFree;
	//ulong *commitedLimit;
	//ulong *base_3;
	//ulong *reservedLimit;
	//ulong *regionBase;

};

}

#endif /* GCSPACE_H_ */
