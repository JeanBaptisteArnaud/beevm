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
	array->slot(1) = object;
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	array->slot(2) = string;
	oop_t * stringPointer = string->_asPointer();
	oop_t * leakedPointer  = ((compactor->oldSpace).shallowCopy(mockedObjects.newString("a String")))->_asPointer();
	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));
	byteArray->byte(0) = 1;
	byteArray->byte(1) = 2;
	byteArray->byte(2) = 3;
	oop_t * byteArrayPointer = byteArray->_asPointer();
	array->slot(3) = byteArray;

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


cute::suite make_suite_GCMarkAndCompactTest()
{
	cute::suite s;

		s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompact));

	return s;
}
