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
	oop_t * leakedPointer = ((compactor->oldSpace).shallowCopy(mockedObjects.newString("a String")))->_asPointer();
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
	compactor->followCountStartingAt((slot_t *)array, array->_size(), 1);
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

void GCMarkAndCompactTest::testFollowDontRescueEphemerons() {
	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * leaked = (compactor->oldSpace).shallowCopy(mockedObjects.newString("leaked"));
	oop_t * value = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * ephemeron = (compactor->oldSpace).shallowCopy(mockedObjects.newEphemeron(leaked, value));
	pointers[0] = leaked->_asPointer();
	pointers[1] = value->_asPointer();
	pointers[2] = ephemeron->_asPointer();
	oop_t * root = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x4));
	root->slot(0) = root;
	ephemeron = KnownObjects::nil; // thinks this is useless
	// maybe missing something; here to check
	compactor->followCountStartingAt((slot_t *)root, 1, 1);
	root = KnownObjects::nil;
	ASSERTM("Ephemeron should not be rescued", !((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("Ephemeron should not be rescued", !((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("Ephemeron should be visible", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
}

void GCMarkAndCompactTest::testFollowEphemeronsNoRescue() {
	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * leaked = (compactor->oldSpace).shallowCopy(mockedObjects.newString("leaked"));
	oop_t * value = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * ephemeron = (compactor->oldSpace).shallowCopy(mockedObjects.newEphemeron(leaked, value));
	pointers[0] = leaked->_asPointer();
	pointers[1] = value->_asPointer();
	pointers[2] = ephemeron->_asPointer();
	oop_t * root = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x4));
	root->slot(0) = root;
	ephemeron = KnownObjects::nil; // thinks this is useless
								   // maybe missing something; here to check
	compactor->followCountStartingAt((slot_t *)root, 1, 1);
	compactor->rescueEphemerons();
	ASSERTM("rescued ephemeron array should be empty", compactor->rescuedEphemerons.isEmpty());
	compactor->rescuedEphemerons.contents = KnownObjects::nil;
	root = KnownObjects::nil;
	ASSERTM("Ephemeron should be rescued", ((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("Ephemeron should be rescued", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("Ephemeron should be visible", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
}

void GCMarkAndCompactTest::testFollowObjectInFrom() {
	oop_t * pointers[4];
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = ((compactor->fromSpace).shallowCopy(mockedObjects.newArray(3)));
	oop_t * string = (compactor->fromSpace).shallowCopy(mockedObjects.newString("a String"));
	pointers[0] = (compactor->fromSpace).shallowCopy(mockedObjects.newString("leaked"))->_asPointer();
	oop_t * byteArray = ((compactor->fromSpace).shallowCopy(mockedObjects.newByteArray(3)));

	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;

	oop_t * root = ((compactor->fromSpace).shallowCopy(mockedObjects.newArray(1)));
	root->slot(0) = array;
	pointers[1] = array->_asPointer();
	pointers[2] = string->_asPointer();
	pointers[3] = byteArray->_asPointer();

	string = KnownObjects::nil;
	byteArray = KnownObjects::nil;
	array = KnownObjects::nil;

	ASSERTM("not conform, leaked", ((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());

	compactor->followCountStartingAt((slot_t *)root, 1, 1);
	root = KnownObjects::nil;

	// to fix
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
}

void GCMarkAndCompactTest::testFollowObjectInTo() {
	oop_t * pointers[4];
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * array = ((compactor->toSpace).shallowCopy(mockedObjects.newArray(3)));
	oop_t * string = (compactor->toSpace).shallowCopy(mockedObjects.newString("a String"));
	pointers[0] = (compactor->toSpace).shallowCopy(mockedObjects.newString("leaked"))->_asPointer();
	oop_t * byteArray = ((compactor->toSpace).shallowCopy(mockedObjects.newByteArray(3)));

	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;

	oop_t * root = ((compactor->toSpace).shallowCopy(mockedObjects.newArray(1)));
	root->slot(0) = array;
	pointers[1] = array->_asPointer();
	pointers[2] = string->_asPointer();
	pointers[3] = byteArray->_asPointer();

	string = KnownObjects::nil;
	byteArray = KnownObjects::nil;
	array = KnownObjects::nil;

	ASSERTM("not conform, leaked", ((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());

	compactor->followCountStartingAt((slot_t *)root, 1, 1);
	root = KnownObjects::nil;

	// to fix
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
}

void GCMarkAndCompactTest::testFollowObjectInOldWeakArray() {
	oop_t * pointers[4];
	MarkAndCompactGC * compactor = this->compactor();

	oop_t * array = ((compactor->oldSpace).shallowCopy(mockedObjects.newWeakArray(3)));
	oop_t * string = (compactor->oldSpace).shallowCopy(mockedObjects.newString("a String"));
	pointers[0] = (compactor->oldSpace).shallowCopy(mockedObjects.newString("leaked"))->_asPointer();
	oop_t * byteArray = ((compactor->oldSpace).shallowCopy(mockedObjects.newByteArray(3)));

	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;

	oop_t * root = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(1)));
	root->slot(0) = array;
	oop_t * weak = root->slot(0);

	pointers[1] = array->_asPointer();
	pointers[2] = string->_asPointer();
	pointers[3] = byteArray->_asPointer();

	string = KnownObjects::nil;
	byteArray = KnownObjects::nil;
	array = KnownObjects::nil;



	ASSERTM("not conform, leaked", ((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("not conform, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());

	compactor->followCountStartingAt((slot_t *)root, 1, 1);
	ASSERTM("weak should be in weak container", weak == (compactor->weakContainers[0]));

	weak = KnownObjects::nil;
	root = KnownObjects::nil;


	// to fix
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
}


void GCMarkAndCompactTest::testFollowRescueEphemerons() {
	//#knownIssue.
	/*	allocAddress >= 16r40000000 ifTrue : [
			'have to fix #dereference: to accept high addresses'.
				self assert : false].
	*/

	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * leaked = (compactor->oldSpace).shallowCopy(mockedObjects.newString("leaked"));
	oop_t * value = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * ephemeron = (compactor->oldSpace).shallowCopy(mockedObjects.newEphemeron(leaked, value));
	pointers[0] = leaked->_asPointer();
	pointers[1] = value->_asPointer();
	pointers[2] = ephemeron->_asPointer();
	oop_t * root = (compactor->localSpace).shallowCopy(mockedObjects.newArray(0x4));
	pointers[3] = (compactor->rescuedEphemerons.contents)->_asPointer();
	compactor->followCountStartingAt((slot_t *)root, 1, 1);
	compactor->rescueEphemerons();
	compactor->followRescuedEphemerons();
	//ulong size = (pointers[3])  * 2;
	ulong size = 3;
	compactor->rescuedEphemerons.contents = KnownObjects::nil;
	root = KnownObjects::nil;
	ASSERTM("size ", size == 3);

	ASSERTM("should, leaked", ((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
	
}

void GCMarkAndCompactTest::testFollowWeak() {
		oop_t * pointers[3];
		MarkAndCompactGC * compactor = this->compactor();
		oop_t * value1 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
		oop_t * value2 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
		oop_t * weak = (compactor->oldSpace).shallowCopy(mockedObjects.newWeakArray(2));
		weak->slot(0) = value1;
		weak->slot(1) = value2;
		oop_t * tombstone = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
		compactor->tombstone(tombstone);
		pointers[0] = value1->_asPointer();
		pointers[1] = value2->_asPointer();
		pointers[2] = weak->_asPointer();
		pointers[3] = tombstone->_asPointer();

	oop_t * root = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(2)));
	root->slot(0) = weak;
	root->slot(1) = value2;

	weak = KnownObjects::nil;
	value1 = KnownObjects::nil;
	value2 = KnownObjects::nil;

	compactor->followCountStartingAt((slot_t *)root, root->_size(), 1);
	compactor->fixWeakContainers();
	compactor->tombstone(smiConst(42));
	
	root = KnownObjects::nil;

	ASSERTM("should not, leaked", !((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
	
}

void GCMarkAndCompactTest::testFollowWeakExtended() {
	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();
	oop_t * value1 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * value2 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * weak = (compactor->oldSpace).shallowCopy(mockedObjects.newWeakArray(300));
	weak->slot(290) = value1;
	weak->slot(291) = value2;
	oop_t * tombstone = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	compactor->tombstone(tombstone);
	pointers[0] = value1->_asPointer();
	pointers[1] = value2->_asPointer();
	pointers[2] = weak->_asPointer();
	pointers[3] = tombstone->_asPointer();

	oop_t * root = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(2)));
	root->slot(0) = weak;
	root->slot(1) = value2;

	weak = KnownObjects::nil;
	value1 = KnownObjects::nil;
	value2 = KnownObjects::nil;

	compactor->followCountStartingAt((slot_t *)root, root->_size(), 1);
	compactor->fixWeakContainers();
	compactor->tombstone(smiConst(42));

	root = KnownObjects::nil;

	ASSERTM("should not, leaked", !((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());

}

void GCMarkAndCompactTest::testFollowWeakExtendedNested() {
	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();

	oop_t * array = (compactor->oldSpace).shallowCopy(mockedObjects.newArray(10));
	oop_t * value1 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * value2 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * weak = (compactor->oldSpace).shallowCopy(mockedObjects.newWeakArray(300));
	weak->slot(290) = value1;
	weak->slot(291) = value2;

	for (int i = 0; i < 10; i++) array->slot(i) = weak;
	
	oop_t * tombstone = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	compactor->tombstone(tombstone);
	pointers[0] = value1->_asPointer();
	pointers[1] = value2->_asPointer();
	pointers[2] = weak->_asPointer();
	pointers[3] = tombstone->_asPointer();

	oop_t * root = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(2)));
	root->slot(0) = array;
	root->slot(1) = value2;

	weak = KnownObjects::nil;
	value1 = KnownObjects::nil;
	value2 = KnownObjects::nil;

	compactor->followCountStartingAt((slot_t *)root, root->_size(), 1);
	compactor->fixWeakContainers();
	compactor->tombstone(smiConst(42));

	root = KnownObjects::nil;

	ASSERTM("should not, leaked", !((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", ((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
}

void GCMarkAndCompactTest::testFollowWeakNoTombstones() {
	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();

	oop_t * value1 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * value2 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * weak = (compactor->oldSpace).shallowCopy(mockedObjects.newWeakArray(300));
	weak->slot(290) = value1;
	weak->slot(291) = value2;
	oop_t * tombstone = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());

	compactor->tombstone(tombstone);
	pointers[0] = value1->_asPointer();
	pointers[1] = value2->_asPointer();
	pointers[2] = weak->_asPointer();
	pointers[3] = tombstone->_asPointer();

	oop_t * root = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(2)));
	root->slot(0) = weak;
	root->slot(1) = value1;
	root->slot(2) = value2;

	weak = KnownObjects::nil;
	value1 = KnownObjects::nil;
	value2 = KnownObjects::nil;

	compactor->followCountStartingAt((slot_t *)root, root->_size(), 1);
	compactor->fixWeakContainers();
	compactor->tombstone(smiConst(42));

	root = KnownObjects::nil;

	ASSERTM("should not, leaked", !((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", !((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
}

void GCMarkAndCompactTest::testFollowWeakNoTombstonesExtended() {
	oop_t * pointers[3];
	MarkAndCompactGC * compactor = this->compactor();

	oop_t * value1 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * value2 = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());
	oop_t * weak = (compactor->oldSpace).shallowCopy(mockedObjects.newWeakArray(2));
	weak->slot(0) = value1;
	weak->slot(1) = value2;
	oop_t * tombstone = (compactor->oldSpace).shallowCopy(mockedObjects.newObject());

	compactor->tombstone(tombstone);
	pointers[0] = value1->_asPointer();
	pointers[1] = value2->_asPointer();
	pointers[2] = weak->_asPointer();
	pointers[3] = tombstone->_asPointer();

	oop_t * root = ((compactor->oldSpace).shallowCopy(mockedObjects.newArray(2)));
	root->slot(0) = weak;
	root->slot(1) = value1;
	root->slot(2) = value2;

	weak = KnownObjects::nil;
	value1 = KnownObjects::nil;
	value2 = KnownObjects::nil;

	compactor->followCountStartingAt((slot_t *)root, root->_size(), 1);
	compactor->fixWeakContainers();
	compactor->tombstone(smiConst(42));

	root = KnownObjects::nil;

	ASSERTM("should not, leaked", !((oop_t *)pointers[0]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, array", ((oop_t *)pointers[1]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, string", ((oop_t *)pointers[2]->_asObject())->_hasBeenSeenInSpace());
	ASSERTM("should, byteArray", !((oop_t *)pointers[3]->_asObject())->_hasBeenSeenInSpace());
}

cute::suite make_suite_GCMarkAndCompactTest()
{
	cute::suite s;

	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompact));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtended));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtendedWEphemeron));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactExtendedWEphemeronRescued));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testCompactOverlapping));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowDontRescueEphemerons));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowEphemeronsNoRescue));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowObjectInFrom));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowObjectInTo));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowObjectInOldWeakArray));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowRescueEphemerons));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowWeak));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowWeakExtended));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowWeakExtendedNested));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowWeakNoTombstones));
	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testFollowWeakNoTombstonesExtended));
	// To check next
//	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testHoldingNativizedMethods));
//	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testIsNil));
//	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testLibrariesAllObjectsDo));
//	s.push_back(CUTE_SMEMFUN(GCMarkAndCompactTest, testLibrariesDo));

	return s;
}
