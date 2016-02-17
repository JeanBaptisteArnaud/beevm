/*
 * GarbageCollector.h
 *
 *  Created on: 17 de dic. de 2015
 *      Author: javier
 */

#ifndef GARBAGECOLLECTOR_H_
#define GARBAGECOLLECTOR_H_

#include "../DataStructures/KnownObjects.h"
#include "../DataStructures/GCSpace.h"
#include "../DataStructures/ReferencedVMArray.h"
#include "../DataStructures/VMArray.h"
#include "../DataStructures/VMVariables.h"


#include <iostream>

using namespace std;
namespace Bee {

class GarbageCollector {

public:
	GarbageCollector();
	virtual ~GarbageCollector() {};

	void useOwnVMVariables();
	void useHostVMVariables();

	void updateFromMemory();
	void collect();
	void tombstone(oop_t *object);
	void rescueEphemerons();
	void rememberIfWeak (oop_t *object);

	void fixWeakContainers();

//protected:
	void addWeakContainer (oop_t *object);
	void loadSpaces();
	void initLocals();
	void clearPolymorphicMethodCache();
	void follow(oop_t *object);
	void follow(slot_t *slot, int count, ulong start);
	void followStack();

	void rescueEphemeron(oop_t *ephemeron);

	void makeRescuedEphemeronsNonWeak();
	void forgetNativeObjects();
	void saveSpaces();
	void followFrameCountStartingAt(slot_t *frame, ulong size , ulong startIndex);
	//void followCountStartingAt(ulong * root,ulong  count ,ulong  start);

	bool followEphemeronsCollectingUnknowns();
	virtual void followCountStartingAt(slot_t *roots, int count, long start) = 0;
	virtual ulong* framePointerToStartWalkingTheStack() = 0;
	virtual bool checkReachablePropertyOf(oop_t *ephemeron) = 0;
	virtual void someEphemeronsRescued() = 0;
	virtual void fixReferencesOrSetTombstone(oop_t *weakArray) = 0;

public: // for testing

	// instance variables
	Memory *memory;
	GCSpace fromSpace;
	GCSpace toSpace;
	GCSpace oldSpace;
	GCSpace auxSpace;
	GCSpace localSpace;

	oop_t* residueObject;
	VMVariablesProxy vm;

	// referenced arrays, their referers need to be updated when the internal content is moved
	ReferencedVMArray rememberedSet;
	ReferencedVMArray literalsReferences;
	ReferencedVMArray rescuedEphemerons;
	ReferencedVMArray nativizedMethods;
	ReferencedVMArray classCheckReferences;

	// temporary growable arrays, they are discarded when GC finishes
	// we may want to converte these to std::vectors or similar in the future
	VMArray weakContainers;
	VMArray ephemerons;
	VMArray stack;
	VMArray unknowns;


};

extern "C" {
//__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_H_ */
