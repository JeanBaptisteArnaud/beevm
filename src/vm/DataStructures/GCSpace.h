/*
 * GCSpace.h
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GCSPACE_H_
#define GCSPACE_H_

#include "Bee.h"
#include "GCSpaceInfo.h"

namespace Bee
{

class GCSpace
{
public:
	GCSpace();

	static GCSpace dynamicNew(ulong size);

	// accessor
	void debug();
	ulong reservedSize();
	ulong used();
	ulong* getBase();
	ulong* getNextFree();
	ulong* getCommitedLimit();
	ulong* getReservedLimit();
	ulong* getRegionBase();
	ulong* getSoftLimit();
//	void setInfo(GCSpaceInfo gcspaceinfo); // think we don't need for now/anymore
	void setBase(ulong * localBase);
	void setNextFree(ulong * localNextFree);
	void setCommitedLimit(ulong * localCommitedLimit);
	void setReservedLimit(ulong * localReservedLimit);
	void setSoftLimit(ulong * localSoftLimit);
	void setRegionBase(ulong  * localRegionBase);
	
	void setInfo(GCSpaceInfo *info); // deprecated

	void load();
	void save();
	void loadFrom(GCSpace &from);
	void loadFrom(GCSpaceInfo &info);
	void reset();


	void dispenseReservedSpace();

	ulong* allocateIfPossible(ulong size);
	ulong* allocateUnsafe(ulong size);

	oop_t* objectFromBuffer(ulong *buffer, ulong headerSize);
	oop_t* shallowCopy(oop_t *object);
	oop_t* shallowCopyGrowingTo(oop_t *array , ulong newSize);

	void commitMoreMemory();
	void commitMoreMemoryIfNeeded();
	ulong commitSized(ulong total);
	ulong commitDelta(ulong delta);
	void decommitSlack();
	void release();

	int percentageOfCommitedUsed();

	bool includes(oop_t *object);

	
	ulong* _commit  (ulong limit, ulong delta);
	void   _decommit(ulong *limit, ulong *delta);
	void   _free    (ulong *limit, ulong *delta);


//	GCSpace dynamicNew(ulong size);

	static const ulong instVarCount = 8;


protected:

	ulong asObject(ulong *smallPointer);

	GCSpaceInfo *info;
	ulong *base;
	ulong *nextFree;
	ulong *commitedLimit;
	ulong *reservedLimit;
	ulong *softLimit;
	ulong *base_3;
	ulong *regionBase;

};

}

#endif /* GCSPACE_H_ */
