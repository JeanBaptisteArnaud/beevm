#ifndef _VM_VARIABLES_H_
#define _VM_VARIABLES_H_

#include "Bee.h"

namespace Bee
{

class GCSpaceInfo;

class VMVariables
{
public:
	VMVariables();
	~VMVariables();

	oop_t *debugFrameMarker;

	bool    JIT_globalLookupCacheHasPointersToFrom;
	oop_t* *JIT_globalLookupCache;
	char   *JIT_codeCache;

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

	// FIXME: migrate those variables to vmvariables, for now support only using host vm addresses
	static oop_t** hostVMWeakContainers();
	static oop_t** hostVMEphemerons();
	static oop_t** hostVMTombstone();
	static oop_t* hostVMFixedObjectsStart();
	static oop_t* hostVMFixedObjectsEnd();

	static oop_t* hostVMLibrariesArray();
	static oop_t* hostVMLibrariesArrayEnd();
	static oop_t* hostVMCharacterBase();
	static oop_t* hostVMCharacterNextFree();

	static oop_t** hostVMWellKnownRoots();
	static ulong hostVMWellKnownRootsSize();
	static oop_t** hostVMExtraRoots();

	oop_t** hostVMRememberedSet();
	oop_t** hostVMLiteralsReferences();
	oop_t** hostVMNativizedMethods();
	oop_t** hostVMCodeCacheObjectReferences();
	oop_t** hostVMRescuedEphemerons();

	static GCSpaceInfo hostVMFromSpace();
	static GCSpaceInfo hostVMToSpace();
	static GCSpaceInfo hostVMOldSpace();

	oop_t**  debugFrameMarker;

//private:
	ulong**  GC_framePointerToWalkStack;

	bool   *JIT_globalLookupCacheHasPointersToFrom;
	oop_t **JIT_globalLookupCache; // a pointer to an array of oop_t*
	char  **JIT_codeCache;

	bool   *GC_anyNativizedCompiledMethodInFromSpace;
	bool   *GC_anyCompiledMethodInFromSpace;
	ulong  *GC_spacesDelta;
};

}

#endif // ~ _VM_VARIABLES_H_
