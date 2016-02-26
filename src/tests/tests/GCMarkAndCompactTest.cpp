#include <iostream>
#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "GCMarkAndCompactTest.h"
#include "MockedObjects.h"

#include "DataStructures/ObjectFormat.h"
#include "DataStructures/Memory.h"
#include "GarbageCollector/MarkAndCompactGC.h"

extern cute::suite make_suite_GCMarkAndCompactTest();

using namespace Bee;
using namespace std;

// File myclassTest.h

GCMarkAndCompactTest::GCMarkAndCompactTest()
{
	setUp();
}

GCMarkAndCompactTest::~GCMarkAndCompactTest()
{
	tearDown();
}

MarkAndCompactGC * GCMarkAndCompactTest::compactor()
{
	return memory->compactor;
}

void GCMarkAndCompactTest::setUp()
{
	mockedObjects.initializeKnownObjects();
	memory = this->memoryForTesting();

	compactor()->setUpLocals();
	compactor()->setUpNonLocals();

}

void GCMarkAndCompactTest::tearDown()
{
}

void GCMarkAndCompactTest::testCompact() {
	
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x100));
	oop_t * object = (compactor->oldSpace).shallowCopy(mockedObjects.newArray(3));
	array->slot(0) = object;
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	array->slot(1) = string;
	oop_t * stringPointer = string->_asPointer();
	oop_t * leakedPointer  = ((compactor->oldSpace).shallowCopy(mockedObjects.newString("a String")))->_asPointer();
	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	byteArray->byte(0) = 1;
	byteArray->byte(1) = 2;
	byteArray->byte(2) = 3;
	oop_t * byteArrayPointer = byteArray->_asPointer();
	array->slot(2) = byteArray;

	ASSERTM("leakedPointer should be after string pointer", (ulong)leakedPointer > (ulong)stringPointer);
	ASSERTM("leakedPointer should be before byteArray pointer", (ulong)leakedPointer < (ulong)byteArrayPointer);
	GCSpace * space = &(compactor->oldSpace);
	compactor->initAuxSpace();
	compactor->followCountStartingAt((slot_t *) array, array->_size(),1);
	compactor->followRescuedEphemerons();
	compactor->setNewPositions(space);
	compactor->prepareForCompact();
	compactor->compact(space);
	ASSERTM("", ((array->slot(1)->_basicSize() + 1 + 3) & -4) + ((ulong)array->slot(1)) == (ulong)array->slot(2) - 8);
	ASSERTM("", this->isArray(array->slot(0)));
	//ASSERTM("", this->isString(array->slot(1)));
	//ASSERTM("", this->isByteArray(array->slot(2)));

}

void GCMarkAndCompactTest::testCompactExtended() {
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x100));
	oop_t * object = (compactor->oldSpace).shallowCopy(mockedObjects.newArray(4));
	array->slot(0) = object;
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	array->slot(1) = string;
	oop_t * stringPointer = string->_asPointer();
	long leakedSize = 678;
	oop_t * leakedPointer = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(leakedSize)))->_asPointer();

	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	byteArray->byte(0) = 1;
	byteArray->byte(1) = 2;
	byteArray->byte(2) = 3;
	oop_t * byteArrayPointer = byteArray->_asPointer();
	array->slot(2) = byteArray;
	oop_t * large = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(876)))->_asPointer();
	large->slot(875) = KnownObjects::stTrue;
	array->slot(3) = large;
	ASSERTM("leakedPointer should be after string pointer", (ulong)leakedPointer > (ulong)stringPointer);
	ASSERTM("leakedPointer should be before byteArray pointer", (ulong)leakedPointer < (ulong)byteArrayPointer);
	GCSpace * space = &(compactor->oldSpace);
	compactor->initAuxSpace();
	compactor->followCountStartingAt((slot_t *)array, array->_size(), 1);
	compactor->followRescuedEphemerons();
	compactor->setNewPositions(space);
	compactor->prepareForCompact();
	compactor->compact(space);
	ASSERTM("", ((array->slot(1)->_basicSize() + 1 + 3) & -4) + ((ulong)array->slot(1)) == (ulong)array->slot(2) - 8);
	ASSERTM("slot 0 should be a array", this->isArray(array->slot(0)));
	//ASSERTM("", this->isString(array->slot(1)));
	//ASSERTM("", this->isByteArray(array->slot(2)));
	ASSERTM("slot 3 should be a array", this->isArray(array->slot(3)));
	oop_t * extended = array->slot(3);
	ASSERTM("slot 875 should be true, copy go wrong", extended->slot(875) == KnownObjects::stTrue);
	// assert: (extended count : [:x | x not]) = (extended size - 1 )

}


void GCMarkAndCompactTest::testCompactExtendedWEphemeron() {
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x100));
	oop_t * object = (compactor->oldSpace).shallowCopy(mockedObjects.newArray(4));
	array->slot(0) = object;
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	array->slot(1) = string;
	oop_t * stringPointer = string->_asPointer();
	long leakedSize = 678;
	oop_t * leakedPointer = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(leakedSize)))->_asPointer();

	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	byteArray->byte(0) = 1;
	byteArray->byte(1) = 2;
	byteArray->byte(2) = 3;
	oop_t * byteArrayPointer = byteArray->_asPointer();
	array->slot(2) = byteArray;
	oop_t * large = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(876)))->_asPointer();
	large->slot(875) = KnownObjects::stTrue;
	array->slot(3) = large;
	oop_t *key = (compactor->oldSpace).shallowCopy(mockedObjects.newString("key"));
	oop_t *value = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t *ephemeron = (compactor->oldSpace).shallowCopy(mockedObjects.newEphemeron(key, value));
	array->slot(4) = key;
	array->slot(5) = ephemeron;
	ASSERTM("leakedPointer should be after string pointer", (ulong)leakedPointer > (ulong)stringPointer);
	ASSERTM("leakedPointer should be before byteArray pointer", (ulong)leakedPointer < (ulong)byteArrayPointer);
	GCSpace * space = &(compactor->oldSpace);
	compactor->initAuxSpace();
	compactor->followCountStartingAt((slot_t *)array, array->_size(), 1);
	compactor->followRescuedEphemerons();
	compactor->setNewPositions(space);
	compactor->prepareForCompact();
	compactor->compact(space);
	ASSERTM("", ((array->slot(1)->_basicSize() + 1 + 3) & -4) + ((ulong)array->slot(1)) == (ulong)array->slot(2) - 8);
	ASSERTM("slot 0 should be a array", this->isArray(array->slot(0)));
	//ASSERTM("", this->isString(array->slot(1)));
	//ASSERTM("", this->isByteArray(array->slot(2)));
	ASSERTM("slot 3 should be a array", this->isArray(array->slot(3)));
	oop_t * extended = array->slot(3);
	ASSERTM("slot 875 should be true, copy go wrong", extended->slot(875) == KnownObjects::stTrue);
	// assert: (extended count : [:x | x not]) = (extended size - 1 )
	ASSERTM("No Ephemeron should be rescued", compactor->rescuedEphemerons.isEmpty());
}

void GCMarkAndCompactTest::testCompactExtendedWEphemeronRescued() {
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x100));
	oop_t * object = (compactor->oldSpace).shallowCopy(mockedObjects.newArray(4));
	array->slot(0) = object;
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	array->slot(1) = string;
	oop_t * stringPointer = string->_asPointer();
	long leakedSize = 678;
	oop_t * leakedPointer = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(leakedSize)))->_asPointer();

	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	byteArray->byte(0) = 1;
	byteArray->byte(1) = 2;
	byteArray->byte(2) = 3;
	oop_t * byteArrayPointer = byteArray->_asPointer();
	array->slot(2) = byteArray;
	oop_t * large = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(876)))->_asPointer();
	large->slot(875) = KnownObjects::stTrue;
	array->slot(3) = large;
	oop_t *key = (compactor->oldSpace).shallowCopy(mockedObjects.newString("key"));
	oop_t *value = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t *ephemeron = (compactor->oldSpace).shallowCopy(mockedObjects.newEphemeron(key, value));
	array->slot(4) = KnownObjects::nil;
	array->slot(5) = ephemeron;
	ASSERTM("leakedPointer should be after string pointer", (ulong)leakedPointer > (ulong)stringPointer);
	ASSERTM("leakedPointer should be before byteArray pointer", (ulong)leakedPointer < (ulong)byteArrayPointer);
	GCSpace * space = &(compactor->oldSpace);
	compactor->initAuxSpace();
	compactor->followCountStartingAt((slot_t *)array, array->_size(), 1);
	compactor->followRescuedEphemerons();
	compactor->setNewPositions(space);
	compactor->prepareForCompact();
	compactor->compact(space);
	ASSERTM("", ((array->slot(1)->_basicSize() + 1 + 3) & -4) + ((ulong)array->slot(1)) == (ulong)array->slot(2) - 8);
	ASSERTM("slot 0 should be a array", this->isArray(array->slot(0)));
	//ASSERTM("", this->isString(array->slot(1)));
	//ASSERTM("", this->isByteArray(array->slot(2)));
	ASSERTM("slot 3 should be a array", this->isArray(array->slot(3)));
	oop_t * extended = array->slot(3);
	ASSERTM("slot 875 should be true, copy go wrong", extended->slot(875) == KnownObjects::stTrue);
	// assert: (extended count : [:x | x not]) = (extended size - 1 )
	ASSERTM("1 Ephemeron should be rescued", !(compactor->rescuedEphemerons.isEmpty()));
	ASSERTM("the ephemeron is wrong check the ", (compactor->rescuedEphemerons[0]) == array->slot(5));
}

void GCMarkAndCompactTest::testCompactOverlapping() {
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x100));
	oop_t * object = (compactor->oldSpace).shallowCopy(mockedObjects.newArray(4));
	array->slot(0) = object;
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	array->slot(1) = string;
	oop_t * stringPointer = string->_asPointer();
	long leakedSize = 678;
	oop_t * leakedPointer = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(leakedSize)))->_asPointer();

	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	byteArray->byte(0) = 1;
	byteArray->byte(1) = 2;
	byteArray->byte(2) = 3;
	oop_t * byteArrayPointer = byteArray->_asPointer();
	array->slot(2) = byteArray;
	oop_t * overlapped = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(1024)));
	array->slot(3) = overlapped;
	oop_t * next = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	next->byte(0) = 4;
	next->byte(1) = 5;
	next->byte(2) = 6;
	array->slot(4) = next;

	ASSERTM("leakedPointer should be after string pointer", (ulong)leakedPointer > (ulong)stringPointer);
	ASSERTM("leakedPointer should be before byteArray pointer", (ulong)leakedPointer < (ulong)byteArrayPointer);
	GCSpace * space = &(compactor->oldSpace);
	compactor->initAuxSpace();
	compactor->followCountStartingAt((slot_t *)array, array->_size(), 1);
	compactor->followRescuedEphemerons();
	compactor->setNewPositions(space);
	compactor->prepareForCompact();
	compactor->compact(space);
	ASSERTM("", ((array->slot(1)->_basicSize() + 1 + 3) & -4) + ((ulong)array->slot(1)) == (ulong)array->slot(2) - 8);
	ASSERTM("slot 0 should be a array", this->isArray(array->slot(0)));
	//ASSERTM("", this->isString(array->slot(1)));
	//ASSERTM("", this->isByteArray(array->slot(2)));
	ASSERTM("slot 3 should be a array", this->isArray(array->slot(3)));
	ASSERTM("size of overlapped object is wrong", (array->slot(3))->_size() == 1024);
	//ASSERTM("slot 4 should be a bytearray", this->isByteArray(array->slot(4)));
	ASSERTM("size of next's overlapped object is wrong", array->slot(4)->_size() == 3);
}




cute::suite make_suite_GCMarkAndCompactTest()
{
	cute::suite s;

		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompact));
		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtended));
		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtendedWEphemeron));
		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtendedWEphemeronRescued));
		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactOverlapping));


	return s;
}
