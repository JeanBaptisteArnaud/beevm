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


#include <iostream>

using namespace std;
namespace Bee {

class GarbageCollector {

public:
	GarbageCollector();
	virtual ~GarbageCollector() {};
	static GarbageCollector* currentFlipper();
	void collect();
	GCSpace localSpace;
	GCSpace fromSpace;
	GCSpace toSpace;

	GCSpace oldSpace;
	ReferencedVMArray rememberSet;
	void tombstone(oop_t *object);
	void rescueEphemerons();
	void rememberIfWeak (oop_t *object);
	VMArray stack;

	ReferencedVMArray literalsReferences;
	ReferencedVMArray rescuedEphemerons;

	void fixWeakContainers();

protected:
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
	void followFrameCountStartingAt(slot_t *frame, ulong count , ulong start);
	//void followCountStartingAt(ulong * root,ulong  count ,ulong  start);

	bool followEphemeronsCollectingUnknowns();
	virtual void followCountStartingAt(slot_t *roots, int count, long start) = 0;
	virtual ulong* framePointerToStartWalkingTheStack() = 0;
	virtual bool checkReachablePropertyOf(oop_t *ephemeron) = 0;
	virtual void someEphemeronsRescued() = 0;
	virtual void fixReferencesOrSetTombstone(oop_t *weakArray) = 0;

	// instance variables

	GCSpace auxSpace;
	VMArray ephemerons;
	VMArray weakContainers;
	VMArray unknowns;

	ReferencedVMArray nativizedMethods;
	ReferencedVMArray classCheckReferences;

	oop_t* residueObject;

	HostVMVariables vm;
	static GarbageCollector *flipper;

};

extern "C" {
__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_H_ */
