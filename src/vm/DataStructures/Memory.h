/*
 * Memory.h
 *
 *  Created on: 14 janv. 2016
 *      Author: jbapt
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "GCSpace.h"
#include "VMArray.h"
#include <vector>

namespace Bee
{

class GenerationalGC;

// a smalltalk object
class Memory
{
public:
	Memory();

	void initializeCollector();
	void startUp();
	void createPinnedSpace();
	void releaseEverything();

//protected:
// up to date
	void scavengeFromSpace();
	void growIfTime();
	void commitMoreSpace();


// for testing only
//	void addSpace(GCSpace *aSpace);
//	void addCollectedSpace(GCSpace *aSpace);


	static const ulong instVarCount = 8;

public: // for testing

	GCSpace **spaces;
	GCSpace **collectedSpaces;
	GenerationalGC *flipper;
	GCSpace *pinnedSpace;
	GCSpace *fromSpace;
	GCSpace *toSpace;
	GCSpace *oldSpace;

	oop_t *rememberedSet;
	oop_t *literalsReferences;
	oop_t *codeCacheObjectReferences;
	oop_t *nativizedMethods;
	oop_t *rescuedEphemerons;
	oop_t *residueObject;
};

}

#endif /* MEMORY_H_ */
