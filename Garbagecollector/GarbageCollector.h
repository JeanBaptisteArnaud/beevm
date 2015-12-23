/*
 * GarbageCollector.h
 *
 *  Created on: 17 de dic. de 2015
 *      Author: javier
 */

#ifndef GARBAGECOLLECTOR_H_
#define GARBAGECOLLECTOR_H_

#include "../DataStructures/VMArray.h"
#include "../DataStructures/GCSpace.h"
#include "../DataStructures/ReferencedVMArray.h"
#include "../DataStructures/VMMemory.h"

namespace Bee {

class ReferencedVMArray1 {

	long size();
};


class GarbageCollector {
protected:
	GCSpace fromSpace;
	GCSpace toSpace;
	GCSpace oldSpace;
	GCSpace auxSpace;
	VMArray ephemerons;
	VMArray weakContainers;
	ReferencedVMArray rescuedEphemerons;
	ReferencedVMArray rememberSet;
	ReferencedVMArray literalsReferences;
	ReferencedVMArray nativizedMethods;
	ReferencedVMArray classCheckReferences;
	VMArray stack;
	VMArray unknowns;
	unsigned long residueObject;
public:

	GarbageCollector();
	virtual ~GarbageCollector() {};

	static GarbageCollector* currentFlipper();

	void collect();

protected:
	static GarbageCollector *flipper;

	void loadSpaces();
	void initLocals();
	void clearPolymorphicMethodCache();
	unsigned long dereference(unsigned long pointer);
	void follow(unsigned long pointer);
	void follow(unsigned long pointer, int count, unsigned long start);
	void followStack();
	void rescueEphemerons();
	bool followEphemeronsCollectingUnknowns();
	void rescueEphemeron(unsigned long ephemeron);
	void someEphemeronsRescued();
	void makeRescuedEphemeronsNonWeak();
	void fixWeakContainers();
	void forgetNativeObjects();
	void saveSpaces();

	virtual unsigned long framePointerToStartWalkingTheStack() = 0;
	virtual void fixReferencesOrSetTombstone(unsigned long) = 0;
};

extern "C" {
__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_H_ */
