
#define __STDC_WANT_LIB_EXT1__ 1

#include <cstring>

#include "MockedObjects.h"
#include "DataStructures/Bee.h"
#include "DataStructures/ObjectFormat.h"
#include "DataStructures/KnownObjects.h"
#include "DataStructures/Memory.h"

using namespace std;
using namespace Bee;

#define STACK_SIZE 16
#define STACK_LAST (STACK_SIZE - 1)

MockedObjects::MockedObjects(bool itIsLibrary)
{
	GCSpace allocator = GCSpace::dynamicNew(4*1024*1024);
	defaultSpace.loadFrom(allocator);
	stackTempIndex = STACK_LAST - 5; // -0: end of stack. -1: self. -2: cm. -3: prev env. -4: env. -5: first temp
	seenFlag = itIsLibrary ? 0 : basic_header_t::Flag_unseenInSpace;
	parent = NULL;
}

MockedObjects::~MockedObjects()
{
	defaultSpace.dynamicFree();
}

void MockedObjects::initializeKnownObjects()
{
	uchar flags = seenFlag | basic_header_t::Flag_zeroTermOrNamed | basic_header_t::Flag_notIndexed;
	basic_header_t *header = (basic_header_t*)defaultSpace.allocateUnsafe(8);
	header->size = 0;
	header->hash = 0;
	header->flags = flags;

	define("Object behavior", header->slots());
	header->behavior = header->slots();

	

	oop_t *nil     = mockNil();
	oop_t *stTrue  = mockTrue();
	oop_t *stFalse = mockFalse();
	oop_t *array   = this->newArray(0);
	//Memory *memory  = mockMemory();

	KnownObjects::initialize(nil, stTrue, stFalse);
	KnownObjects::emptyArray = array;

	// create a fake stack with place for all slots
	//stack.reserve(STACK_SIZE);
	for (int i = 0; i < STACK_SIZE; i++)
	//	stack.push_back((ulong)KnownObjects::nil);
		stack[i] = (ulong)KnownObjects::nil;

	stack[0] = (ulong)&stack[STACK_LAST];
	stack[STACK_LAST] = 0;
}

void MockedObjects::stackAddTemporary(oop_t *value)
{
	stack[stackTempIndex] = (ulong)value;
	stackTempIndex--;
}

slot_t& MockedObjects::stackTemporary(ulong index)
{
	return *(slot_t*)&stack[STACK_LAST - 5 - index];
}

slot_t& MockedObjects::stackEnvironment()
{
	return *(slot_t*)&stack[STACK_LAST - 4];
}


ulong* MockedObjects::stackPtr()
{
	return &stack[0];
}


oop_t* MockedObjects::get(const string &name)
{
	if (definitions.find(name) != definitions.end())
		return definitions[name];
	else
	{
		if (parent) {
			return parent->get(name);
		}
		else {
			oop_t * mock = this->newObject();
			define(name, mock);
			return mock;
		}
	}
}

void MockedObjects::define(const string &name, oop_t *value)
{
	definitions[name] = value;
}

void MockedObjects::reference(const std::string &name, slot_t *slot)
{
	oop_t *referred = get(name);
	
	if (referred == NULL)
	{
		references.push_back(pair<slot_t*, string>(slot, name));
		referred = smiConst(678); // put a dummy object in place so that we don't have a bad ptr dangling there
	}

	*slot = referred;
}

oop_t* MockedObjects::newBasicObject(const string &classname, uchar size, ushort hash, uchar flags, GCSpace &space)
{
	basic_header_t *header = (basic_header_t*)space.allocateUnsafe(8+size*4);

	this->reference(classname + " behavior", &header->behavior);

	header->size = size;
	header->hash = hash;
	header->flags = flags;

	return header->slots();
}

oop_t* MockedObjects::newBasicObject(const string &classname, uchar size, ushort hash, uchar flags)
{
	return newBasicObject(classname, size, hash, flags, defaultSpace);
}

oop_t* MockedObjects::newExtendedObject(const string &classname, ulong size, ushort hash, uchar flags, GCSpace &space)
{
	extended_header_t *header = (extended_header_t*)space.allocateUnsafe(16+size*4);

	this->reference(classname + " behavior", &header->basic_header.behavior);

	header->size = size;
	header->hash = hash;
	header->flags = flags;

	header->basicSize = 4;
	header->copyToBasic();

	return header->slots();
}

oop_t* MockedObjects::newExtendedObject(const string &classname, ulong size, ushort hash, uchar flags)
{
	return newExtendedObject(classname, size, hash, flags, defaultSpace);
}

oop_t* MockedObjects::newObject(const string &classname, ulong size, ushort hash, uchar flags, GCSpace &space)
{
	oop_t *result;

	if (size > 255)
		result = this->newExtendedObject(classname, size, hash, basic_header_t::Flag_isExtended | flags, space);
	else
		result = this->newBasicObject(classname, (uchar)size, hash, flags, space);

	return result;
}

oop_t* MockedObjects::mockNil()
{
	//nil_hdr <0, 3445h, Flag_unseenInSpace or Flag_zeroTermOrNamed or Flag_notIndexed, UndefinedObject_behavior>
	oop_t *nil = this->newBasicObject("UndefinedObject", 0, 0x3445, 0x61);
	this->define("nil", nil);

	return nil;
}

oop_t* MockedObjects::mockTrue()
{
	//true_hdr <0, 0, Flag_unseenInSpace or Flag_zeroTermOrNamed or Flag_notIndexed, offset True_b>

	oop_t *stTrue = this->newBasicObject("True", 0, 0, 0x61);
	this->define("true", stTrue);

	return stTrue;
}

oop_t* MockedObjects::mockFalse()
{
	//false_hdr <0, 0, Flag_unseenInSpace or Flag_zeroTermOrNamed or Flag_notIndexed, offset False_b>
	oop_t *stFalse = this->newBasicObject("False", 0, 0, 0x61);
	this->define("false", stFalse);

	return stFalse;
}

oop_t* MockedObjects::newObject()
{
	uchar flags = seenFlag | basic_header_t::Flag_zeroTermOrNamed | basic_header_t::Flag_notIndexed;
	return this->newBasicObject("Object", 0, 0, flags);
}

oop_t* MockedObjects::newEphemeron(oop_t *key, oop_t *value)
{
	uchar flags = basic_header_t::Flag_isEphemeron | basic_header_t::Flag_zeroTermOrNamed |
				basic_header_t::Flag_notIndexed | basic_header_t::Flag_isExtended | seenFlag;

	oop_t *ephemeron = this->newExtendedObject("Ephemeron", 3, 0, flags);

	ephemeron->slot(0) = key;
	ephemeron->slot(1) = value;
	ephemeron->slot(2) = KnownObjects::nil;
	return ephemeron;
}



oop_t* MockedObjects::newArray(ulong slots, GCSpace *space)
{
	oop_t *array = this->newObject("Array", slots, 0, seenFlag, *space);

	for (ulong index = 0; index < slots; index++)
		array->slot(index) = smiConst(index);
	
	return array;
}

oop_t* MockedObjects::newArray(ulong slots)
{
	return this->newArray(slots, &this->defaultSpace);
}

oop_t* MockedObjects::newByteArray(ulong size)
{
	uchar flags = basic_header_t::Flag_isBytes | seenFlag;

	oop_t *array = this->newObject("ByteArray", size, 0, flags, defaultSpace);

	for (ulong index = 0; index <= size; index++)
		array->slot(index) = smiConst(index);
	
	return array;
}


oop_t* MockedObjects::newString(const char *value)
{
	ulong size  = strlen(value) + 1;
	uchar flags = basic_header_t::Flag_isBytes | basic_header_t::Flag_zeroTermOrNamed | seenFlag;

	oop_t *result = this->newObject("String", size, 0, flags, defaultSpace);

	strcpy_s((char*)result, size, value);

	return result;
}

oop_t* MockedObjects::newWeakArray(ulong slots)
{
	oop_t *array = this->newObject("Array", slots, 0, basic_header_t::Flag_isEphemeron | seenFlag, defaultSpace);

	// if extended we have to unset ephemeron/weak flag or it will be confused with an ephemeron
	if(array->_isExtended()) array->unsetExtFlags(basic_header_t::Flag_isEphemeron);

	for (ulong index = 0; index < slots; index++)
		array->slot(index) = KnownObjects::nil;

	return array;
}

GCSpace MockedObjects::setDefaultSpace(GCSpace *newSpace)
{
	GCSpace previous = defaultSpace;
	defaultSpace.loadFrom(*newSpace);
	return previous;
}

void Bee::MockedObjects::setParent(MockedObjects * localParent)
{
	parent = localParent;
}

Memory* MockedObjects::mockMemory()
{
	uchar flags = seenFlag | basic_header_t::Flag_notIndexed | basic_header_t::Flag_zeroTermOrNamed;
	Memory *memory = (Memory*)this->newBasicObject("Memory", Memory::instVarCount, 0, flags);


	memory->fromSpace = this->mockGCSpace(1  * 1024 * 1024);
	memory->toSpace   = this->mockGCSpace(1  * 1024 * 1024);
	memory->oldSpace  = this->mockGCSpace(16 * 1024 * 1024);

	return memory;
}

GCSpace* MockedObjects::mockGCSpace(ulong size)
{
	uchar flags = seenFlag | basic_header_t::Flag_notIndexed | basic_header_t::Flag_zeroTermOrNamed;
	GCSpace *space = (GCSpace*)this->newBasicObject("GCSpace", GCSpace::instVarCount, 0, flags);

	GCSpace allocator = GCSpace::dynamicNew(size);
	space->loadFrom(allocator);

	return space;
}



void freeSimpleObject(oop_t *object)
{
	free(object->basic_header());
}

void freeComplexObject(oop_t *object)
{
	free(object->extended_header());
}

