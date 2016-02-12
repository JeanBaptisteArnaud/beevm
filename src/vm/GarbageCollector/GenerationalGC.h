/*
 * GenerationalGC.h
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GARBAGECOLLECTOR_GENERATIONALGC_H_
#define GARBAGECOLLECTOR_GENERATIONALGC_H_

#include "GarbageCollector.h"
#include "../DataStructures/GCSpace.h"

namespace Bee {

class GenerationalGC: public GarbageCollector {

public:
	GenerationalGC();
	~GenerationalGC();
	void collect();
	oop_t* moveToOldOrTo(oop_t* object);
	void initLocals();
	void initNonLocals();
	void addRoot(oop_t *object);
	void followRoots();
	bool arenaIncludes(oop_t *object);
	oop_t* copyTo(oop_t *object, GCSpace &to);

	void purgeRoots();

protected:
	bool hasToPurge(oop_t *object);
	ulong* codeCacheReferenceAtOffset(ulong offset);
	void moveToOldAll(ReferencedVMArray &objects);
	void followRememberSet();
	void purgeLiteralsReference();
	void purgeRememberSet();
	bool checkReachablePropertyOf(oop_t *ephemeron);
	void followCodeCacheReferences();
	void moveClassCheckReferences();
	void cleanRememberSet();
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

extern "C" {
__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_GENERATIONALGC_H_ */
