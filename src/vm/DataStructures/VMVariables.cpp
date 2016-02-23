
#include "VMVariables.h"
#include "ObjectFormat.h"
#include "GCSpaceInfo.h"

using namespace Bee;

VMVariables::VMVariables()
{
	debugFrameMarker = (oop_t*)0;

	JIT_globalLookupCacheHasPointersToFrom = true;
	JIT_globalLookupCache = new oop_t*[4000];

	// FIXME: initialize it with address of a real code cache
	JIT_codeCache;

	GC_anyNativizedCompiledMethodInFromSpace = true;
	GC_anyCompiledMethodInFromSpace = false;
	GC_spacesDelta;
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


	GC_framePointerToWalkStack = (ulong**)0x100407C4;
	GC_anyNativizedCompiledMethodInFromSpace = (bool*)0x10041714;
	GC_anyCompiledMethodInFromSpace = (bool*)0x10041710;
	GC_spacesDelta = (ulong*)0x1004175C;

}


void VMVariablesProxy::initializeFor(VMVariables *variables)
{
	debugFrameMarker = &variables->debugFrameMarker;

	JIT_globalLookupCacheHasPointersToFrom = &variables->JIT_globalLookupCacheHasPointersToFrom;
	JIT_globalLookupCache = variables->JIT_globalLookupCache;
	JIT_codeCache = &variables->JIT_codeCache;

	GC_framePointerToWalkStack = &variables->GC_framePointerToWalkStack;
	GC_anyNativizedCompiledMethodInFromSpace = &variables->GC_anyNativizedCompiledMethodInFromSpace;
	GC_anyCompiledMethodInFromSpace = &variables->GC_anyCompiledMethodInFromSpace;
	GC_spacesDelta = &variables->GC_spacesDelta;
}

oop_t* VMVariablesProxy::hostVMCharacterBase()
{
	return (oop_t*)0x100260C0;
}

oop_t* VMVariablesProxy::hostVMCharacterNextFree()
{
	return (oop_t*)(12 * 8 + 0x100260C0);
}
	

oop_t* VMVariablesProxy::hostVMFixedObjectsEnd()
{
	return *(oop_t**)0x100406A0;
}

oop_t* VMVariablesProxy::hostVMFixedObjectsStart()
{
	return *(oop_t**)0x1004069C;
}

oop_t* VMVariablesProxy::hostVMLibrariesArray()
{
	return *(oop_t**)0x10040750;
}

oop_t* VMVariablesProxy::hostVMLibrariesArrayEnd()
{
	return *(oop_t**)0x10040754;
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
ulong  VMVariablesProxy::hostVMWellKnownRootsSize()
{
	return *(ulong *)0x10041694;
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
	*GC_anyNativizedCompiledMethodInFromSpace = value;
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
