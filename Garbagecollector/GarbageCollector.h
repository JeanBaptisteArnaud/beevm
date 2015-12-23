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
class GarbageCollector {
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
	unsigned long residueObject = 0;
public:

	GarbageCollector();

	static GarbageCollector* currentFlipper();

	void collect();

protected:
	static GarbageCollector *flipper;

	void loadSpaces();
	void initLocals();
	void clearPolymorphicMethodCache();
	virtual unsigned long framePointerToStartWalkingTheStack() = 0;
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
	virtual void fixReferencesOrSetTombstone(unsigned long) = 0;
	void forgetNativeObjects();
	void saveSpaces();

};

extern "C" {
__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_H_ */
