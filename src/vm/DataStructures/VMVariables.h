#ifndef _VM_VARIABLES_H_
#define _VM_VARIABLES_H_

#include "Bee.h"

namespace Bee
{

class VMVariables
{
public:
	VMVariables();

	oop_t *debugFrameMarker;

	bool   JIT_globalLookupCacheHasPointersToFrom;
	oop_t *JIT_globalLookupCache;
	char  *JIT_codeCache;

	bool   GC_anyNativizedCompiledMethodInFromSpace;
	bool   GC_anyCompiledMethodInFromSpace;
	ulong  GC_spacesDelta;
	ulong *GC_framePointerToWalkStack;
};

class VMVariablesProxy
{
public:
	void initializeForHostVM();
	void initializeFor(VMVariables *variables);

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

#endif // ~ _VM_VARIABLES_H_
