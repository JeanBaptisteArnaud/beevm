/*
 * GarbageCollector.h
 *
 *  Created on: 17 de dic. de 2015
 *      Author: javier
 */

#ifndef GARBAGECOLLECTOR_H_
#define GARBAGECOLLECTOR_H_

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
public:

	GarbageCollector();

	static GarbageCollector* currentFlipper();

	void collect();

private:
	static GarbageCollector *flipper;

	void loadSpaces();
	void initLocals();
	void clearPolymorphicMethodCache();
	unsigned long framePointerToStartWalkingTheStack();
	unsigned long dereference(unsigned long pointer);
	void follow(unsigned long pointer);
	void follow(unsigned long pointer, int count, unsigned long start);
	void followStack();

};

extern "C" {
__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_H_ */
