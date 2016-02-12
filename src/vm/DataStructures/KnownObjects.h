/*
 * KnownObjects.h
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste, Javier Pimas
 */

/**
 * This should manage access to known objects in memory.
 * to see it deeper, look at subclasses: HostVMKnownObjects,
 * MockedKnownObjects, BeeKnownObjects.
**/

#ifndef _KNOWNOBJECTS_H_
#define _KNOWNOBJECTS_H_

#include "Bee.h"

namespace Bee
{

class KnownObjects
{
public:
	static oop_t*  nil;
	static oop_t*  stTrue;
	static oop_t*  stFalse;

	static oop_t*  Array_behavior;
};

class HostVMVariables
{
public:
	HostVMVariables();

	bool globalCacheHasPointersToFrom();
	void globalCacheHasPointersToFrom(bool value);

	bool anyCompiledMethodInFromSpace();
	void anyCompiledMethodInFromSpace(bool value);

	bool anyNativizedCompiledMethodInFromSpace();
	void anyNativizedCompiledMethodInFromSpace(bool value);

	ulong spacesDelta();
	void spacesDelta(ulong delta);

	ulong* framePointerToStartWalkingTheStack();
	void framePointerToStartWalkingTheStack(ulong *value);

	char* codeCache();
	oop_t* globalLookupCacheAt(ulong index);
	void globalLookupCacheAtPut(ulong index, oop_t *value);


	oop_t**  debugFrameMarker;

//private:
	ulong**  GC_framePointerToWalkStack;

	bool   *JIT_globalLookupCacheHasPointersToFrom;
	oop_t **JIT_globalLookupCache;
	char  **JIT_codeCache;

	bool   *GC_anyNativizedCompiledMethodInFromSpace;
	bool   *GC_anyCompiledMethodInFromSpace;
	ulong  *GC_spacesDelta;
};

}

#endif /* _KNOWNOBJECTS_H_ */
