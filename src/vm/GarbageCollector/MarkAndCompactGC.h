/*
 * MarkAndCompactGC.h
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GARBAGECOLLECTOR_MARKANDCOMPACTGC_H_
#define GARBAGECOLLECTOR_MARKANDCOMPACTGC_H_

#include "GarbageCollector.h"
#include "../DataStructures/GCSpace.h"

namespace Bee {

class MarkAndCompactGC: public GarbageCollector
{

public:
	MarkAndCompactGC();
	~MarkAndCompactGC();

	//virtual void useOwnVMVariables();
	virtual void useHostVMVariables();

	void doCollect();
	void unmarkRememberedSet();
	void initAuxSpace(); 
	void unseeWellKnownObjects(); 
	void flushCodeCache();
	void followAll();
	void setNewPositionsAndCompact();
	void updateOldSpace();
	void updateNativeRescuedEphemerons();
	void allocateArrays();
	void resetFrom();
	void decommitSlack();
	void disableRememberedSet();
	void someEphemeronsRescued();
	void unseeCharacters();
	void unseeSKernelMeta();
	void unseeLibraryObjects();
	void followWellKnownObjects();
	void followExtraRoots();
	void followRescuedEphemerons();
	void prepareForCompact();
	void allocateWeakContainersArray();
	void allocateEphemeronsArray();
	void forgetNativeObjects();
	void followCountStartingAt(slot_t *frame, int size, long startIndex);
	void setNewPositions(GCSpace * space);
	void compact(GCSpace * space);
	void librariesDo();
	void initNonLocals();
	bool arenaIncludes(oop_t *);
	ulong librariesArraySize();
	void fixReferencesOrSetTombstone(oop_t * weakContainer);
	bool checkReachablePropertyOf(oop_t * ephemeron);
	void addInterrupt();

	ReferencedVMArray tempArray;
	GCSpace characters;
	GCSpace sKernelMeta;

	//SLLInfo library;


};

}

#endif /* GARBAGECOLLECTOR_MARKANDCOMPACTGC_H_ */
