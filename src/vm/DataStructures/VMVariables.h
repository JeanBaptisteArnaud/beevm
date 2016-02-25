#ifndef _VM_VARIABLES_H_
#define _VM_VARIABLES_H_

#include "Bee.h"

namespace Bee
{

class GCSpaceInfo;
class Memory;

class VMVariables
{
public:
	VMVariables(Memory *memory);
	~VMVariables();

	oop_t *debugFrameMarker;

	bool    JIT_globalLookupCacheHasPointersToFrom;
	oop_t* *JIT_globalLookupCache;
	char   *JIT_codeCache;

	bool   GC_anyNativizedCompiledMethodInFromSpace;
	bool   GC_anyCompiledMethodInFromSpace;
	ulong  GC_spacesDelta;
	ulong *GC_framePointerToWalkStack;

	// for flipper
	oop_t** rememberedSet;
	oop_t** literalsReferences;
	oop_t** nativizedMethods;
	oop_t** codeCacheObjectReferences;
	oop_t** rescuedEphemerons;

	// added for compactor
	oop_t* weakContainers;
	oop_t* ephemerons;
	oop_t* tombstone;
	oop_t* fixedObjectsStart;
	oop_t* fixedObjectsEnd;

	oop_t* librariesArrayStart;
	oop_t* librariesArrayEnd;
	oop_t* characterBase;
	oop_t* characterNextFree;

	oop_t* wellKnownRoots;
	ulong  wellKnownRootsSize;
	oop_t* extraRoots;
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

	// for flipper
	oop_t** rememberedSet();
	oop_t** literalsReferences();
	oop_t** nativizedMethods();
	oop_t** codeCacheObjectReferences();
	oop_t** rescuedEphemerons();


	// for compactor
	oop_t** weakContainers();
	oop_t** ephemerons();
	oop_t** tombstone();
	oop_t*  fixedObjectsStart();
	oop_t*  fixedObjectsEnd();

	oop_t*  librariesArrayStart();
	oop_t*  librariesArrayEnd();
	oop_t*  characterBase();
	oop_t*  characterNextFree();

	oop_t** wellKnownRoots();
	ulong   wellKnownRootsSize();
	oop_t** extraRoots();

//private:
	oop_t**  debugFrameMarker;
	ulong**  GC_framePointerToWalkStack;

	bool   *JIT_globalLookupCacheHasPointersToFrom;
	oop_t **JIT_globalLookupCache; // a pointer to an array of oop_t*
	char  **JIT_codeCache;

	bool   *GC_anyNativizedCompiledMethodInFromSpace;
	bool   *GC_anyCompiledMethodInFromSpace;
	ulong  *GC_spacesDelta;


	oop_t** GC_rememberedSet;
	oop_t** GC_literalsReferences;
	oop_t** GC_nativizedMethods;
	oop_t** GC_codeCacheObjectReferences;
	oop_t** GC_rescuedEphemerons;


	oop_t** GC_weakContainers;
	oop_t** GC_ephemerons;
	oop_t** GC_tombstone;
	oop_t** GC_fixedObjectsStart;
	oop_t** GC_fixedObjectsEnd;

	oop_t** GC_librariesArrayStart;
	oop_t** GC_librariesArrayEnd;
	oop_t** GC_characterBase;
	oop_t** GC_characterNextFree;

	oop_t** GC_wellKnownRoots;
	ulong*  GC_wellKnownRootsSize;
	oop_t** GC_extraRoots;

private:
	// host vm addresses
	static oop_t** hostVMWeakContainers();
	static oop_t** hostVMEphemerons();
	static oop_t** hostVMTombstone();
	static oop_t** hostVMFixedObjectsStart();
	static oop_t** hostVMFixedObjectsEnd();

	static oop_t** hostVMLibrariesArrayStart();
	static oop_t** hostVMLibrariesArrayEnd();
	static oop_t** hostVMCharacterBase();
	static oop_t** hostVMCharacterNextFree();

	static oop_t** hostVMWellKnownRoots();
	static ulong*  hostVMWellKnownRootsSize();
	static oop_t** hostVMExtraRoots();


	static oop_t** hostVMRememberedSet();
	static oop_t** hostVMLiteralsReferences();
	static oop_t** hostVMNativizedMethods();
	static oop_t** hostVMCodeCacheObjectReferences();
	static oop_t** hostVMRescuedEphemerons();

public:
	static GCSpaceInfo hostVMFromSpace();
	static GCSpaceInfo hostVMToSpace();
	static GCSpaceInfo hostVMOldSpace();
};

}

#endif // ~ _VM_VARIABLES_H_
