/*
 * VMMemory.cpp
 *
 *  Created on: 6 janv. 2016
 *      Author: jbapt
 */
#include "KnownObjects.h"
#include "Memory.h"

#include <windows.h>
#include <climits>
#include <iostream>

using namespace Bee;
using namespace std;

oop_t*  KnownObjects::nil;
oop_t*  KnownObjects::stTrue;
oop_t*  KnownObjects::stFalse;


// for now we have vmhost ones
HostVMVariables::HostVMVariables()
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

bool HostVMVariables::globalCacheHasPointersToFrom()
{
	return *JIT_globalLookupCacheHasPointersToFrom;
}

void   HostVMVariables::globalCacheHasPointersToFrom(bool value)
{
	*JIT_globalLookupCacheHasPointersToFrom = value;
}

oop_t* HostVMVariables::globalLookupCacheAt(ulong index)
{
	return JIT_globalLookupCache[index];
}

void HostVMVariables::globalLookupCacheAtPut(ulong index, oop_t *value)
{
	JIT_globalLookupCache[index] = value;
}


bool HostVMVariables::anyCompiledMethodInFromSpace()
{
	return *GC_anyCompiledMethodInFromSpace;
}

void HostVMVariables::anyCompiledMethodInFromSpace(bool value)
{
	*GC_anyCompiledMethodInFromSpace = value;
}

bool HostVMVariables::anyNativizedCompiledMethodInFromSpace()
{
	return *GC_anyNativizedCompiledMethodInFromSpace;
}

void HostVMVariables::anyNativizedCompiledMethodInFromSpace(bool value)
{
	*GC_anyNativizedCompiledMethodInFromSpace = value;
}

ulong HostVMVariables::spacesDelta()
{
	return *GC_spacesDelta;
}

void HostVMVariables::spacesDelta(ulong delta)
{
	*GC_spacesDelta = delta;
}

ulong* HostVMVariables::framePointerToStartWalkingTheStack()
{
	return *GC_framePointerToWalkStack;
}

void HostVMVariables::framePointerToStartWalkingTheStack(ulong *value)
{
	*GC_framePointerToWalkStack = value;
}

char* HostVMVariables::codeCache()
{
	return *JIT_codeCache;
}


