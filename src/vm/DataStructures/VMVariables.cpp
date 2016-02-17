
#include "VMVariables.h"
#include "ObjectFormat.h"

using namespace Bee;

VMVariables::VMVariables()
{
	debugFrameMarker = (oop_t*)0;

	JIT_globalLookupCacheHasPointersToFrom = true;
	JIT_globalLookupCache;
	JIT_codeCache;

	GC_anyNativizedCompiledMethodInFromSpace = true;
	GC_anyCompiledMethodInFromSpace = false;
	GC_spacesDelta;
	GC_framePointerToWalkStack = 0;

}

void VMVariablesProxy::initializeForHostVM()
{
	debugFrameMarker = (oop_t**) 0x1001B633;

	JIT_globalLookupCacheHasPointersToFrom = (bool*) 0x1003EC48;
	JIT_globalLookupCache = (oop_t**) 0x1002EC20;
	JIT_codeCache         = (char **) 0x1002E820;


	GC_framePointerToWalkStack = (ulong**) 0x100407C4;
	GC_anyNativizedCompiledMethodInFromSpace = (bool*) 0x10041714;
	GC_anyCompiledMethodInFromSpace  = (bool*) 0x10041710;
	GC_spacesDelta = (ulong*)0x1004175C;
}

void VMVariablesProxy::initializeFor(VMVariables *variables)
{
	debugFrameMarker = &variables->debugFrameMarker;

	JIT_globalLookupCacheHasPointersToFrom = &variables->JIT_globalLookupCacheHasPointersToFrom;
	JIT_globalLookupCache = &variables->JIT_globalLookupCache;
	JIT_codeCache         = &variables->JIT_codeCache;

	GC_framePointerToWalkStack = &variables->GC_framePointerToWalkStack;
	GC_anyNativizedCompiledMethodInFromSpace = &variables->GC_anyNativizedCompiledMethodInFromSpace;
	GC_anyCompiledMethodInFromSpace  = &variables->GC_anyCompiledMethodInFromSpace;
	GC_spacesDelta = &variables->GC_spacesDelta;
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
	return (*JIT_globalLookupCache)->slot(index);
}

void VMVariablesProxy::globalLookupCacheAtPut(ulong index, oop_t *value)
{
	(*JIT_globalLookupCache)->slot(index) = value;
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
