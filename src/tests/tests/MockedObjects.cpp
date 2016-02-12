
#include "MockedObjects.h"
#include "DataStructures/ObjectFormat.h"
#include "DataStructures/KnownObjects.h"
#include "DataStructures/Memory.h"

using namespace std;
using namespace Bee;


MockedObjects::MockedObjects()
{
	GCSpaceInfo info = GCSpaceInfo::newSized(4*1024*1024);
	defaultSpace.setInfo(info);
	defaultSpace.load();
}

void MockedObjects::initializeKnownObjects()
{
	mockNil();
	mockTrue();
	mockFalse();
}

// mock vars to replace HostVM pointers
static oop_t*  mock_debugFrameMarker;
static bool    mock_anyCompiledMethodInFromSpace;
static bool    mock_anyNativizedCompiledMethodInFromSpace;
static bool    mock_globalLookupCacheHasPointersToFromSpace;
static oop_t*  mock_JIT_globalLookupCache[4000];
static char*   mock_JIT_codeCache;
static ulong*  mock_framePointerToWalkStack;

void MockedObjects::mockVMValue()
{
	ulong * address = Memory::current()->VM();
	if (!address)
		osError();

	vm.debugFrameMarker = &mock_debugFrameMarker;
	vm.GC_anyCompiledMethodInFromSpace = &mock_anyCompiledMethodInFromSpace;
	vm.GC_anyNativizedCompiledMethodInFromSpace = &mock_anyNativizedCompiledMethodInFromSpace;
	vm.JIT_globalLookupCacheHasPointersToFrom = &mock_globalLookupCacheHasPointersToFromSpace;
	vm.JIT_globalLookupCache = mock_JIT_globalLookupCache;
	vm.JIT_codeCache = &mock_JIT_codeCache;
	vm.GC_framePointerToWalkStack = &mock_framePointerToWalkStack;


	*vm.debugFrameMarker = (oop_t*)0;
	vm.anyCompiledMethodInFromSpace(false);
	vm.anyNativizedCompiledMethodInFromSpace(true);
	vm.globalCacheHasPointersToFrom(true);

	vm.framePointerToStartWalkingTheStack(0);
}

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

oop_t* MockedObjects::mockBasicObject(const string &classname, uchar size, ushort hash, uchar flags, GCSpace &space)
{
	basic_header_t *header = (basic_header_t*)space.allocate(8+size*4);

	this->reference(classname + " behavior", &header->behavior);

	header->size = size;
	header->hash = hash;
	header->flags = flags;

	return header->slots();
}

oop_t* MockedObjects::mockBasicObject(const string &classname, uchar size, ushort hash, uchar flags)
{
	return mockBasicObject(classname, size, hash, flags, defaultSpace);
}

oop_t* MockedObjects::mockExtendedObject(const string &classname, ulong size, ushort hash, uchar flags, GCSpace &space)
{
	extended_header_t *header = (extended_header_t*)space.allocate(16+size*4);

	this->reference(classname + " behavior", &header->basic_header.behavior);

	header->size = size;
	header->hash = hash;
	header->flags = flags;

	header->basicSize = 4;
	header->copyToBasic();

	return header->slots();
}

oop_t* MockedObjects::mockExtendedObject(const string &classname, ulong size, ushort hash, uchar flags)
{
	return mockExtendedObject(classname, size, hash, flags, defaultSpace);
}

void MockedObjects::mockNil() {
	//nil_hdr <0, 3445h, ObjectFlag_reserved1 or ObjectFlag_zeroTermOrNamed or ObjectFlag_notIndexed, UndefinedObject_behavior>
	oop_t *nil = this->mockBasicObject("UndefinedObject", 0, 0x3445, 0x61);
	this->define("nil", nil);

	KnownObjects::nil = nil;
}

void MockedObjects::mockTrue() {
	//true_hdr <0, 0, ObjectFlag_reserved1 or ObjectFlag_zeroTermOrNamed or ObjectFlag_notIndexed, offset True_b>

	oop_t *stTrue = this->mockBasicObject("True", 0, 0, 0x61);
	this->define("true", stTrue);

	KnownObjects::stTrue = stTrue;
}

void MockedObjects::mockFalse() {
	//false_hdr <0, 0, ObjectFlag_reserved1 or ObjectFlag_zeroTermOrNamed or ObjectFlag_notIndexed, offset False_b>
	oop_t *stFalse = this->mockBasicObject("False", 0, 0, 0x61);
	this->define("false", stFalse);

	KnownObjects::stFalse = stFalse;
}


oop_t* MockedObjects::mockObjectFrom()
{
// TODO need to change to real object when pocho send me the header

	return mockArray(3);
}

oop_t* MockedObjects::mockEphemeronFrom(oop_t *key, oop_t *value)
{
//	flags: ObjectFlag_unseenInSpace or ObjectFlag_isEphemeron or ObjectFlag_zeroTermOrNamed or 
//	ObjectFlag_notIndexed or ObjectFlag_isExtended

	oop_t *ephemeron = this->mockExtendedObject("Ephemeron", 3, 0, 0xE5);

	ephemeron->slot(0) = key;
	ephemeron->slot(1) = value;
	ephemeron->slot(2) = KnownObjects::nil;
	return ephemeron;
}


oop_t* MockedObjects::mockArray(ulong slots)
{
	oop_t *array;

	if (slots > 255)
		array = this->mockExtendedObject("Array", slots, 0, basic_header_t::ObjectFlag_isExtended | 1);
	else
		array = this->mockBasicObject("Array", slots, 0, 1);

	for (int index = 0; index <= slots; index++)
		array->slot(index) = smiConst(index);
	
	return array;
}

oop_t* MockedObjects::mockWeakArray() {

	oop_t *array = this->mockExtendedObject("Array", 1024, 0, 0x81);

	for (int index = 0; index <= 1024; index++)
		array->slot(index) = smiConst(index);

	array->setFlags(basic_header_t::ObjectFlag_isEphemeron);
	return array;
}


bool MockedObjects::checkMockArray2(oop_t *object) {
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

bool MockedObjects::checkValueMockArray1024(oop_t *array) {

	for (int index = 0; index <= 1024; index++)
	{
		if (array->slot(index) != smiConst(index))
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

