/*
 * GenerationalGC.h
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GARBAGECOLLECTOR_GENERATIONALGC_H_
#define GARBAGECOLLECTOR_GENERATIONALGC_H_

#include "GarbageCollector.h"
#include "../DataStructures/GCSpace.h"

namespace Bee {

class GenerationalGC: public GarbageCollector
{

public:
	GenerationalGC();
	~GenerationalGC();

	void initialize();
	void initLocals();

	void collect();

	void setUpLocals();
	void setUpNonLocals();
	void doCollect();


	void purgeRoots();
	void followRoots();

	void addRoot(oop_t *object);
	bool arenaIncludes(oop_t *object);
	oop_t* copyTo(oop_t *object, GCSpace &to);
	oop_t* moveToOldOrTo(oop_t* object);


//protected:
	bool hasToPurge(oop_t *object);
	oop_t** codeCacheReferenceAtOffset(ulong offset);
	void moveToOldAll(ReferencedVMArray &objects);
	void followRememberedSet();
	void purgeLiteralsReferences();
	void purgeRememberedSet();
	bool checkReachablePropertyOf(oop_t *ephemeron);
	void followCodeCacheReferences();
	void moveClassCheckReferences();
	void cleanRememberedSet();
	void addInterrupt();
	void fixReferencesFromNativeMethods();
	void flipSpaces();
	void updateSpacesDelta();
	oop_t* moveToOldSpace(oop_t* object);
	oop_t* moveToToSpace(oop_t* object);

	void spacesDelta(ulong delta);
	ulong spacesDelta();
	ulong* framePointerToStartWalkingTheStack();
	void fixReferencesOrSetTombstone(oop_t *weakContainer);
	void followCountStartingAt(slot_t *slot, int count, long start);
	void someEphemeronsRescued();
	void holdReferenceTo(oop_t *object);
};

}

#endif /* GARBAGECOLLECTOR_GENERATIONALGC_H_ */
