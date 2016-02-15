
#include "MockedObjects.h"
#include "DataStructures/ObjectFormat.h"
#include "DataStructures/KnownObjects.h"
#include "DataStructures/Memory.h"

using namespace std;
using namespace Bee;


MockedObjects::MockedObjects()
{
	GCSpaceInfo info = GCSpaceInfo::newSized(4*1024*1024);
	defaultSpace.loadFrom(info);
}

void MockedObjects::initializeKnownObjects()
{
	oop_t *nil     = mockNil();
	oop_t *stTrue  = mockTrue();
	oop_t *stFalse = mockFalse();
	oop_t *array   = this->newArray(0);
	Memory *memory  = mockMemory();

	Bee::initializeKnownObjects(nil, stTrue, stFalse, array, memory);
}

// mock vars to replace HostVM pointers
static oop_t*  mock_debugFrameMarker;
static bool    mock_anyCompiledMethodInFromSpace;
static bool    mock_anyNativizedCompiledMethodInFromSpace;
static bool    mock_globalLookupCacheHasPointersToFromSpace;
static oop_t*  mock_JIT_globalLookupCache[4000];
static char*   mock_JIT_codeCache;
static ulong*  mock_framePointerToWalkStack;


oop_t* MockedObjects::get(const string &name)
{
	if (definitions.find(name) != definitions.end())
		return definitions[name];
	else
		return NULL;
}

void MockedObjects::define(const string &name, oop_t *value)
{
	definitions[name] = value;
}

void MockedObjects::reference(const std::string &name, slot_t *slot)
{
	oop_t *referred = get(name);
	
	if (referred == NULL)
		references.push_back(pair<slot_t*, string>(slot, name));
	
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
	uchar flags = basic_header_t::Flag_unseenInSpace | basic_header_t::Flag_zeroTermOrNamed | basic_header_t::Flag_notIndexed;
	return this->newBasicObject("Object", 0, 0, flags);
}

oop_t* MockedObjects::newEphemeron(oop_t *key, oop_t *value)
{
//	flags: Flag_unseenInSpace or Flag_isEphemeron or Flag_zeroTermOrNamed or 
//	Flag_notIndexed or Flag_isExtended

	oop_t *ephemeron = this->newExtendedObject("Ephemeron", 3, 0, 0xE5);

	ephemeron->slot(0) = key;
	ephemeron->slot(1) = value;
	ephemeron->slot(2) = KnownObjects::nil;
	return ephemeron;
}


oop_t* MockedObjects::newArray(ulong slots, GCSpace *space)
{
	oop_t *array;

	if (slots > 255)
		array = this->newExtendedObject("Array", slots, 0, basic_header_t::Flag_isExtended | basic_header_t::Flag_unseenInSpace, *space);
	else
		array = this->newBasicObject("Array", (uchar)slots, 0, 1, *space);

	for (ulong index = 0; index <= slots; index++)
		array->slot(index) = smiConst(index);
	
	return array;
}

oop_t* MockedObjects::newArray(ulong slots)
{
	return this->newArray(slots, &this->defaultSpace);
}

oop_t* MockedObjects::newWeakArray()
{

	oop_t *array = this->newExtendedObject("Array", 1024, 0, 0x81);

	for (int index = 0; index <= 1024; index++)
		array->slot(index) = smiConst(index);

	array->setFlags(basic_header_t::Flag_isEphemeron);
	return array;
}



Memory* MockedObjects::mockMemory()
{
	uchar flags = basic_header_t::Flag_unseenInSpace | basic_header_t::Flag_notIndexed | basic_header_t::Flag_zeroTermOrNamed;
	Memory *memory = (Memory*)this->newBasicObject("Memory", Memory::instVarCount, 0, flags);


	memory->fromSpace = this->mockGCSpace(1  * 1024 * 1024);
	memory->toSpace   = this->mockGCSpace(1  * 1024 * 1024);
	memory->oldSpace  = this->mockGCSpace(16 * 1024 * 1024);

	return memory;
}

GCSpace* MockedObjects::mockGCSpace(ulong size)
{
	uchar flags = basic_header_t::Flag_unseenInSpace | basic_header_t::Flag_notIndexed | basic_header_t::Flag_zeroTermOrNamed;
	GCSpace *space = (GCSpace*)this->newBasicObject("GCSpace", GCSpace::instVarCount, 0, flags);

	GCSpaceInfo info = GCSpaceInfo::newSized(size);
	space->loadFrom(info);

	return space;
}



bool MockedObjects::checknewArray2(oop_t *object)
{
	ulong sizeA = object->_size();
	if (sizeA != 5)
		return false;

	if (object->behavior() != this->get("Array behavior"))
		return false;

	for (ulong index = 0; index < sizeA; index++) {
		if (object->slot(index) != KnownObjects::nil)
			return false;
	}

	return true;
}

void freeSimpleObject(oop_t *object)
{
	free(object->basic_header());
}

void freeComplexObject(oop_t *object)
{
	free(object->extended_header());
}

