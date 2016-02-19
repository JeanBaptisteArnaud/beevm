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

	// to interface with host vm
	void useHostVMVariables();
	void updateFromHostVM();
	void updateToHostVM();

//protected:
	void scavengeFromSpace();
	void growIfTime();
	void commitMoreSpace();

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
