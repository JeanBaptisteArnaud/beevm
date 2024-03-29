
#include "GCTest.h"

#include "DataStructures/GCSpaceInfo.h"
#include "DataStructures/ReferencedVMArray.h"
#include "DataStructures/Memory.h"
#include "DataStructures/ObjectFormat.h"

#include "GarbageCollector/GenerationalGC.h"
#include "GarbageCollector/MarkAndCompactGC.h"

using namespace Bee;

oop_t* GCTest::global(const std::string &name)
{
	oop_t * result = mockedLocal.get(name);
	if(result == NULL) 
		return mockedFrom.get(name);
		else return result;
}

GCTest::GCTest() :
	mockedFrom(false),
	mockedLocal(true)
{
}

// was gcForTesting
Memory *GCTest::memoryForTesting()
{
	Memory *memory = new Memory();
	memory->fromSpace = GCSpace::dynamicNewP( 8 * 1024);
	memory->toSpace   = GCSpace::dynamicNewP( 8 * 1024);
	memory->oldSpace  = GCSpace::dynamicNewP(64 * 1024);


	memory->rememberedSet             = mockedFrom.newArray(0x200, memory->oldSpace);
	memory->literalsReferences        = mockedFrom.newArray(0x200, memory->oldSpace);
	memory->nativizedMethods          = mockedFrom.newArray(0x10,  memory->oldSpace);
	memory->codeCacheObjectReferences = mockedFrom.newArray(0x10,  memory->oldSpace);
	memory->rescuedEphemerons         = mockedFrom.newArray(0x200, memory->oldSpace);

	memory->rememberedSet            ->slot(0) = smiConst(2); // size = 0
	memory->literalsReferences       ->slot(0) = smiConst(2); // size = 0
	memory->nativizedMethods         ->slot(0) = smiConst(2); // size = 0
	memory->codeCacheObjectReferences->slot(0) = smiConst(2); // size = 0
	memory->rescuedEphemerons        ->slot(0) = smiConst(2); // size = 0

	memory->residueObject = mockedLocal.newObject();
	
	memory->flipper->memory = memory;
	memory->compactor->memory = memory;

	GCSpace space = GCSpace::dynamicNewUncommited(32 * 1024 * 1024);
	memory->flipper->localSpace.loadFrom(space);
	memory->compactor->localSpace.loadFrom(space);

	memory->flipper->initialize();
	memory->compactor->initialize();

	memory->flipper->useOwnVMVariables();
	memory->compactor->useOwnVMVariables();

	memory->flipper->vm.framePointerToStartWalkingTheStack(mockedLocal.stackPtr());
	memory->compactor->vm.framePointerToStartWalkingTheStack(mockedLocal.stackPtr());

	*memory->flipper->vm.JIT_globalLookupCache = (oop_t*)new ulong[0x4000];
	*memory->compactor->vm.JIT_globalLookupCache = *memory->flipper->vm.JIT_globalLookupCache;

	memory->flipper->updateFromMemory();
	memory->compactor->updateFromMemory();

	return memory;
}

void GCTest::releaseMemoryForTesting(Memory *memory)
{
	memory->fromSpace->dynamicFree();
	memory->toSpace  ->dynamicFree();
	memory->oldSpace ->dynamicFree();

	delete memory->fromSpace;
	delete memory->toSpace;
	delete memory->oldSpace;

	memory->flipper->localSpace.dynamicFree();

	delete *memory->flipper->vm.JIT_globalLookupCache;
	delete memory;
}

bool GCTest::isArray(oop_t *object)
{
	return object->behavior() == global("Array behavior");
}

bool GCTest::isString(oop_t *object)
{
	return object->behavior() == global("String behavior");
}

bool GCTest::isByteArray(oop_t *object)
{
	return object->behavior() == global("ByteArray behavior");
}


bool GCTest::isSameArray(oop_t *object, oop_t *anotherObject)
{
	ulong sizeA = object->_size();
	ulong sizeB = anotherObject->_size();
	if (sizeA != sizeB)
		return false;

	//if(object[-2] != anotherObject[-2]) return false; Flags will be different

	if (object->behavior() != anotherObject->behavior())
		return false;

	if (object->_isExtended() && anotherObject->_isExtended())
	{
		//if(object[-4] != anotherObject[-4]) return false; Flags will be different
		if (object->extended_header()->size != anotherObject->extended_header()->size)
			return false;
	}

	for (ulong index = 0; index < sizeA; index++)
	{
		if (object->slot(index) != anotherObject->slot(index))
			return false;
	}
	return true;
}
