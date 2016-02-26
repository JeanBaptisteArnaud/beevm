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



cute::suite make_suite_GCMarkAndCompactTest()
{
	cute::suite s;

		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompact));
		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtended));

	return s;
}
