
#include "VMVariables.h"
#include "ObjectFormat.h"
#include "GCSpaceInfo.h"
#include "Memory.h"

using namespace Bee;

VMVariables::VMVariables()
{
	debugFrameMarker = (oop_t*)0;

	JIT_globalLookupCacheHasPointersToFrom = true;
	JIT_globalLookupCache = new oop_t*[0x4000];

	// FIXME: initialize it with address of a real code cache
	JIT_codeCache;

	GC_anyNativizedCompiledMethodInFromSpace = true;
	GC_anyCompiledMethodInFromSpace = false;
	GC_spacesDelta = 1;
	GC_framePointerToWalkStack = 0;

}

VMVariables::~VMVariables()
{
	delete JIT_globalLookupCache;
}

void VMVariablesProxy::initializeForHostVM()
{
	debugFrameMarker = (oop_t**)0x1001B633;

	JIT_globalLookupCacheHasPointersToFrom = (bool*)0x1003EC48;
	JIT_globalLookupCache = (oop_t**)0x1002EC20;
	JIT_codeCache = (char **)0x1002E820;

	JIT_flushCodeCache = hostVMFlushCodeCache();

	GC_framePointerToWalkStack = (ulong**)0x100407C4;
	GC_anyNativizedCompiledMethodInFromSpace = (bool*)0x10041714;
	GC_anyCompiledMethodInFromSpace = (bool*)0x10041710;
	GC_spacesDelta = (ulong*)0x1004175C;

	GC_rememberedSet             = hostVMRememberedSet();
	GC_literalsReferences        = hostVMLiteralsReferences();
	GC_nativizedMethods          = hostVMNativizedMethods();
	GC_codeCacheObjectReferences = hostVMCodeCacheObjectReferences();
	GC_rescuedEphemerons         = hostVMRescuedEphemerons();

	GC_weakContainers      = hostVMWeakContainers();
	GC_ephemerons          = hostVMEphemerons();
	GC_tombstone           = hostVMTombstone();
	GC_fixedObjectsStart   = hostVMFixedObjectsStart();
	GC_fixedObjectsEnd     = hostVMFixedObjectsEnd();

	GC_librariesArrayStart = hostVMLibrariesArrayStart();
	GC_librariesArrayEnd   = hostVMLibrariesArrayEnd();
	GC_charactersBase      = hostVMCharactersBase();
	GC_charactersNextFree  = hostVMCharactersNextFree();

	GC_wellKnownRoots      = hostVMWellKnownRoots();
	GC_wellKnownRootsSize  = hostVMWellKnownRootsSize();
	GC_extraRoots          = hostVMExtraRoots();
}

void dummyFunc() {}

void VMVariablesProxy::initializeFor(VMVariables *variables, Memory *memory)
{
	debugFrameMarker = &variables->debugFrameMarker;

	JIT_globalLookupCacheHasPointersToFrom = &variables->JIT_globalLookupCacheHasPointersToFrom;
	JIT_globalLookupCache = variables->JIT_globalLookupCache;
	JIT_codeCache = &variables->JIT_codeCache;

	JIT_flushCodeCache = dummyFunc;

	GC_framePointerToWalkStack = &variables->GC_framePointerToWalkStack;
	GC_anyNativizedCompiledMethodInFromSpace = &variables->GC_anyNativizedCompiledMethodInFromSpace;
	GC_anyCompiledMethodInFromSpace = &variables->GC_anyCompiledMethodInFromSpace;
	GC_spacesDelta = &variables->GC_spacesDelta;

	GC_rememberedSet             = &memory->rememberedSet;
	GC_literalsReferences        = &memory->literalsReferences;
	GC_nativizedMethods          = &memory->nativizedMethods;
	GC_codeCacheObjectReferences = &memory->codeCacheObjectReferences;
	GC_rescuedEphemerons         = &memory->rescuedEphemerons;

	GC_weakContainers      = &variables->weakContainers;
	GC_ephemerons          = &variables->ephemerons;
	GC_tombstone           = &variables->tombstone;
	GC_fixedObjectsStart   = &variables->fixedObjectsStart;
	GC_fixedObjectsEnd     = &variables->fixedObjectsEnd;

	GC_librariesArrayStart = &variables->librariesArrayStart;
	GC_librariesArrayEnd   = &variables->librariesArrayEnd;
	GC_charactersBase      = &variables->charactersBase;
	GC_charactersNextFree  = &variables->charactersNextFree;

	GC_wellKnownRoots      = &variables->wellKnownRoots;
	GC_wellKnownRootsSize  = &variables->wellKnownRootsSize;
	GC_extraRoots          = &variables->extraRoots;
}


bool VMVariablesProxy::globalCacheHasPointersToFrom()
{
	return *JIT_globalLookupCacheHasPointersToFrom;
}

void   VMVariablesProxy::globalCacheHasPointersToFrom(bool value)
{
	*JIT_globalLookupCacheHasPointersToFrom = value;
}

oop_t* VMVariablesProxy::globalLookupCacheAt(ulong index)
{
	return JIT_globalLookupCache[index - 1];
}

void VMVariablesProxy::globalLookupCacheAtPut(ulong index, oop_t *value)
{
	JIT_globalLookupCache[index - 1] = value;
}


bool VMVariablesProxy::anyCompiledMethodInFromSpace()
{
	return *GC_anyCompiledMethodInFromSpace;
}

void VMVariablesProxy::anyCompiledMethodInFromSpace(bool value)
{
	*GC_anyCompiledMethodInFromSpace = value;
}

bool VMVariablesProxy::anyNativizedCompiledMethodInFromSpace()
{
	return *GC_anyNativizedCompiledMethodInFromSpace;
}

void VMVariablesProxy::anyNativizedCompiledMethodInFromSpace(bool value)
{
	*GC_anyNativizedCompiledMethodInFromSpace = value ? 1 : 0;
}

ulong VMVariablesProxy::spacesDelta()
{
	return *GC_spacesDelta;
}

void VMVariablesProxy::spacesDelta(ulong delta)
{
	*GC_spacesDelta = delta;
}

ulong* VMVariablesProxy::framePointerToStartWalkingTheStack()
{
	return *GC_framePointerToWalkStack;
}

void VMVariablesProxy::framePointerToStartWalkingTheStack(ulong *value)
{
	*GC_framePointerToWalkStack = value;
}

char* VMVariablesProxy::codeCache()
{
	return *JIT_codeCache;
}

void VMVariablesProxy::flushCodeCache()
{
	JIT_flushCodeCache();
}

oop_t** VMVariablesProxy::rememberedSet()
{
	return GC_rememberedSet;
}

oop_t** VMVariablesProxy::literalsReferences()
{
	return GC_literalsReferences;
}

oop_t** VMVariablesProxy::nativizedMethods()
{
	return GC_nativizedMethods;
}
oop_t** VMVariablesProxy::codeCacheObjectReferences()
{
	return GC_codeCacheObjectReferences;
}

oop_t** VMVariablesProxy::rescuedEphemerons()
{
	return GC_rescuedEphemerons;
}

// for compactor
oop_t** VMVariablesProxy::weakContainers()
{
	return GC_weakContainers;
}

oop_t** VMVariablesProxy::ephemerons()
{
	return GC_ephemerons;
}

oop_t** VMVariablesProxy::tombstone()
{
	return GC_tombstone;
}

ulong*  VMVariablesProxy::librariesArrayStart()
{
	return *GC_librariesArrayStart;
}

ulong*  VMVariablesProxy::librariesArrayEnd()
{
	return *GC_librariesArrayEnd;
}

ulong*  VMVariablesProxy::charactersBase()
{
	return (ulong*)pointerConst((ulong)*GC_charactersBase);
}

ulong*  VMVariablesProxy::charactersNextFree()
{
	return (ulong*)pointerConst((ulong)*GC_charactersNextFree);
}

ulong*  VMVariablesProxy::fixedObjectsStart()
{
	return *GC_fixedObjectsStart;
}

ulong*  VMVariablesProxy::fixedObjectsEnd()
{
	return *GC_fixedObjectsEnd;
}

oop_t** VMVariablesProxy::wellKnownRoots()
{
	return GC_wellKnownRoots;
}

ulong   VMVariablesProxy::wellKnownRootsSize()
{
	return *GC_wellKnownRootsSize;
}

oop_t** VMVariablesProxy::extraRoots()
{
	return GC_extraRoots;
}


VMVariablesProxy::voidFunc VMVariablesProxy::hostVMFlushCodeCache()
{
	return (voidFunc)0x10008CB0;
}

ulong** VMVariablesProxy::hostVMCharactersBase()
{
	//return (ulong**)0x100260C0;
	return (ulong**)0x10040684;
}

ulong** VMVariablesProxy::hostVMCharactersNextFree()
{
	//return (ulong**)(0x100260C0 + 12 * 8);
	return (ulong**)0x10040688;
}
	
ulong** VMVariablesProxy::hostVMFixedObjectsStart()
{
	return (ulong**)0x1004069C;
}

ulong** VMVariablesProxy::hostVMFixedObjectsEnd()
{
	return (ulong**)0x100406A0;
}


ulong** VMVariablesProxy::hostVMLibrariesArrayStart()
{
	return (ulong**)0x10040750;
}

ulong** VMVariablesProxy::hostVMLibrariesArrayEnd()
{
	return (ulong**)0x10040754;
}

oop_t** VMVariablesProxy::hostVMEphemerons()
{
	return (oop_t**)0x1004078C;
}


oop_t** VMVariablesProxy::hostVMWeakContainers()
{
	return (oop_t**)0x1004077C;
}


oop_t** VMVariablesProxy::hostVMTombstone()
{
	return (oop_t**)0x1004079C;
}


oop_t** VMVariablesProxy::hostVMWellKnownRoots()
{
	return (oop_t**)0x100411E0;
}

ulong*  VMVariablesProxy::hostVMWellKnownRootsSize()
{
	return (ulong *)0x10041694;
}

oop_t** VMVariablesProxy::hostVMExtraRoots()
{
	return (oop_t**)0x10040798;
}


oop_t** VMVariablesProxy::hostVMRememberedSet()
{
	return (oop_t**)0x1004070C;
}

oop_t** VMVariablesProxy::hostVMLiteralsReferences()
{
	return (oop_t**)0x10040720;
}

oop_t** VMVariablesProxy::hostVMNativizedMethods()
{
	return (oop_t**)0x10040730;
}

oop_t** VMVariablesProxy::hostVMCodeCacheObjectReferences()
{
	return (oop_t**)0x100406CC;
}

oop_t** VMVariablesProxy::hostVMRescuedEphemerons()
{
	return (oop_t**)0x100407A8;
}

GCSpaceInfo VMVariablesProxy::hostVMFromSpace()
{
	return GCSpaceInfo::withContents((uchar*)0x100416B0);
}

GCSpaceInfo VMVariablesProxy::hostVMToSpace()
{
	return GCSpaceInfo::withContents((uchar*)0x100416C8);
}

GCSpaceInfo VMVariablesProxy::hostVMOldSpace()
{
	return GCSpaceInfo::withContents((uchar*)0x100406B0);
}

