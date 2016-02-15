
#include "GCTest.h"

#include "DataStructures/GCSpaceInfo.h"
#include "DataStructures/ReferencedVMArray.h"
#include "DataStructures/Memory.h"
#include "DataStructures/ObjectFormat.h"

#include "GarbageCollector/GenerationalGC.h"


using namespace Bee;

oop_t* GCTest::global(const std::string &name)
{
	return mockedObjects.get(name);
}

// was gcForTesting
Memory *GCTest::memoryForTesting()
{
	Memory *memory = new Memory();
	memory->fromSpace = new GCSpace;
	memory->toSpace   = new GCSpace;
	memory->oldSpace  = new GCSpace;
	
	memory->fromSpace->loadFrom(GCSpaceInfo::newSized( 8 * 1024));
	memory->  toSpace->loadFrom(GCSpaceInfo::newSized( 8 * 1024));
	memory-> oldSpace->loadFrom(GCSpaceInfo::newSized(16 * 1024));

	memory->flipper->memory = memory;
	memory->flipper->localSpace.loadFrom(GCSpaceInfo::newSized(32 * 1024));
	memory->flipper->initialize();

	memory->flipper->useOwnVMVariables();

	ulong *stack = new ulong[4];
	stack[3] = 0;
	stack[2] = (ulong)KnownObjects::nil;
	stack[1] = (ulong)KnownObjects::nil; // might want to put a compiledMethod
	stack[0] = (ulong)&stack[3];
	memory->flipper->vm.framePointerToStartWalkingTheStack(stack);
	*memory->flipper->vm.JIT_globalLookupCache = (oop_t*)new ulong[0x4000];

	memory->rememberedSet             = mockedObjects.newArray(0x200, memory->oldSpace);
	memory->literalsReferences        = mockedObjects.newArray(0x200, memory->oldSpace);
	memory->nativizedMethods          = mockedObjects.newArray(0x10,  memory->oldSpace);
	memory->codeCacheObjectReferences = mockedObjects.newArray(0x10,  memory->oldSpace);
	memory->rescuedEphemerons         = mockedObjects.newArray(0x200, memory->oldSpace);

	memory->rememberedSet            ->slot(0) = smiConst(2); // size = 0
	memory->literalsReferences       ->slot(0) = smiConst(2); // size = 0
	memory->nativizedMethods         ->slot(0) = smiConst(2); // size = 0
	memory->codeCacheObjectReferences->slot(0) = smiConst(2); // size = 0
	memory->rescuedEphemerons        ->slot(0) = smiConst(2); // size = 0

	memory->residueObject = mockedObjects.newObject();

	memory->flipper->updateFromMemory();

	return memory;
}


bool GCTest::isArray(oop_t *object)
{
	return object->behavior() == global("Array behavior");
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
