
#include <windows.h>
#include <iostream>

#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "GCSpaceTest.h"

#include "DataStructures/ObjectFormat.h"
#include "DataStructures/GCSpace.h"
#include "DataStructures/Memory.h"

extern cute::suite make_suite_GCSpaceTest();

using namespace Bee;
using namespace std;

GCSpaceTest::GCSpaceTest() 
{
	this->setUp();
}

GCSpaceTest::~GCSpaceTest()
{
	this->tearDown();
}

void GCSpaceTest::setUp()
{
	mockedObjects.initializeKnownObjects();
}

void GCSpaceTest::tearDown()
{
}


void GCSpaceTest::testAllocate()
{
	// test doesn't exist in smalltalk. maybe add it?
	GCSpace localSpace = GCSpace::dynamicNew(400 * 1024);
	ASSERTM("init wrong", localSpace.getNextFree() == localSpace.getBase());
	ulong *address = localSpace.allocateIfPossible(1024);
	address = localSpace.allocateIfPossible(1024);
	ASSERTM("nextFree simple",
			(ulong)localSpace.getNextFree() == (ulong)localSpace.getBase() + 1024 + 1024);
	address = localSpace.allocateIfPossible(1024 * 38);
	ASSERTM("nextFree limit",
			(ulong)localSpace.getNextFree() == (ulong)localSpace.getBase() + (1024 * 40));
	address = localSpace.allocateIfPossible(1024 * 30);
}


bool checkValuenewArray1024(oop_t *array)
{
	for (int index = 0; index < 1024; index++)
	{
		if (array->slot(index) != smiConst(index))
			return false;
	}
	return true;
}


void GCSpaceTest::testExtendedGrowingTo()
{
	oop_t *array  = mockedObjects.newArray(1024);
	GCSpace localSpace = GCSpace::dynamicNew(40 * 1024);

	oop_t *copy   = localSpace.shallowCopyGrowingTo(array, 2048);
	oop_t *object = localSpace.shallowCopy(mockedObjects.newObject());
	ASSERTM("copy is not a Array", isArray(copy));
	ASSERTM("size of copy is wrong", copy->_size() == 2048);
	ASSERTM("copy is not consistent (first element diverge)", array->slot(0) == copy->slot(0));
	ASSERTM("copy is not consistent (last element diverge)",  array->slot(1023) == copy->slot(1023));

	ASSERTM("value not correctly copied", checkValuenewArray1024(copy));
//	ASSERTM("The moon is red",
//						(ulong) copy == (ulong)((ulong) localSpace.getBase()) * 2 + 16);
//	ASSERTM("I feel a perturbation in the force",
//			_oop(object) + 8 == (_oop(copy) + (size(copy) * 4) + 16));
	//free(array);

}


void GCSpaceTest::testGCSpace()
{

	//GCSpace localSpace = ??;
	//ASSERTM("base < nextfree", localSpace.getBase() < localSpace.getNextFree());
	//ASSERTM("base < commitedLimit", localSpace.getBase() < localSpace.getCommitedLimit());
	//ASSERTM("commitedLimit <= reservedLimit",
	//		localSpace.getCommitedLimit() <= localSpace.getReservedLimit());
	//ASSERTM("base < commitedLimit", localSpace.getBase() < localSpace.getCommitedLimit());
	//ASSERTM("commitedLimit < reservedLimit",
	//		localSpace.getCommitedLimit() < localSpace.getReservedLimit());
	//ASSERTM("softLimit between base and reservedLimit",
	//		(localSpace.getSoftLimit() <= localSpace.getBase())
	//				&& (localSpace.getSoftLimit() >= localSpace.getReservedLimit()));
	//ASSERTM("nextFree between base and commitedLimit",
	//		(localSpace.getNextFree() <= localSpace.getBase())
	//				&& (localSpace.getNextFree() >= localSpace.getCommitedLimit()));
	//ulong * oldNextFree = localSpace.getNextFree();
	////newArray(1);
	//ASSERTM("Maybe should be test only from Image",
	//		oldNextFree == localSpace.getNextFree());
}

void GCSpaceTest::testGrow()
{
//
//	GCSpace localSpace = flipper->localSpace;
//	oop_t *array = mockedObjects.newArray(3); // 100
//	ulong reservedSize = 4 * 1024 * 100;
//	ulong maxValueQuery = 28;
//	PMEMORY_BASIC_INFORMATION queryAnswer = (PMEMORY_BASIC_INFORMATION) malloc( maxValueQuery);
//	ulong *base = (ulong *) VirtualAlloc((void *)NULL, reservedSize, MEM_RESERVE, PAGE_READWRITE);
//	ASSERTM("Virtual Alloc Fail", base != 0);
//
//	localSpace.setBase(base);
//	localSpace.setRegionBase(base);
//	localSpace.setReservedLimit((ulong*)((ulong) base) + reservedSize);
//	localSpace.setCommitedLimit(base);
//	localSpace.setNextFree((ulong*)((ulong)base + 1024 * 4));
//
//	VirtualQuery((void *) base, queryAnswer, maxValueQuery);
//	ASSERTM("memory not reserve", queryAnswer->State && MEM_RESERVE == MEM_RESERVE);
//
//	localSpace.grow();
//	VirtualQuery((void *) base, queryAnswer, maxValueQuery);
//	ASSERTM("Memory not commit", queryAnswer->State && MEM_COMMIT == MEM_COMMIT);
////	ASSERTM("next free <= commited limit",
////			local.getNextFree() <= local.getCommitedLimit());
//
//	ulong next = (ulong) localSpace.getNextFree() + (4 * 100 + 1024);
//	localSpace.setNextFree((ulong*)next);
//	localSpace.shallowCopy(array);
//// need to simulate that, somehow
////		self
////			assert: (local includes: array);
////			assert: array first = 1;
////			assert: array last = 1.
//	localSpace.setNextFree(base);
//	localSpace.decommitSlack();
//	VirtualQuery((void *) base, queryAnswer, maxValueQuery);
//	ASSERTM("Memory not commit",
//			queryAnswer->State && MEM_RESERVE == MEM_RESERVE);
//	VirtualFree((void *) base, 0, MEM_RELEASE);
//	VirtualQuery((void *) base, queryAnswer, maxValueQuery);
//	ASSERTM("Memory not commit", queryAnswer->State && MEM_FREE == MEM_FREE);
//	free(queryAnswer);
}

void GCSpaceTest::testGrowingTo()
{
	GCSpace localSpace = GCSpace::dynamicNew(4 * 10 * 1024);
	oop_t *array = mockedObjects.newArray(4);
	oop_t *copy = localSpace.shallowCopyGrowingTo(array, 1024);
	oop_t *object = localSpace.shallowCopy(mockedObjects.newObject());

	ASSERTM("copy is not a Array", isArray(copy));
	ASSERTM("size of copy is wrong", copy->_size() == 1024);
	ASSERTM("copy is not consistent (first element diverge)", array->slot(0) == copy->slot(0));
//	ASSERTM((copy count: [:x | x isNil]) = (copy size - array size));
//	ASSERTM("The moon is red",
//			_oop(copy) == _asOop((ulong * )(localSpace.getBase()) + 16));
//	ASSERTM("I feel a perturbation in the force",
//			_oop(object) + 8 == (_oop(copy) + (size(copy) * 4) + 16));
	//free(array);
}

void GCSpaceTest::testNewGCSpaceShallowCopy()
{

	GCSpace space = GCSpace::dynamicNew(4 * 10 * 1024);
	oop_t *array = mockedObjects.newArray(1024);
	oop_t *copy = space.shallowCopy(array);
	ASSERTM("copy is not a Array", isArray(copy));
	ASSERTM("size of copy is wrong", copy->_size() == array->_size());
	ASSERTM("first element diverge", array->slot(0) == copy->slot(0));
	//	Cant touch this tooo doudoudou, doudoudou, doudoudou	assert: array = copy;
//	ASSERTM("I feel a perturbation in the force",
//			_oop(copy) == _asOop(space.getBase() + 8));
}

void GCSpaceTest::testShallowCopy()
{
	// test differs smalltalk's one semantic
	GCSpace localSpace = GCSpace::dynamicNew(4 * 10 * 1024);
	oop_t *array = mockedObjects.newArray(1);
	oop_t *copy = localSpace.shallowCopy(array);
	ASSERTM("the copy not array", isArray(copy));
	ASSERTM("size", copy->_size() == array->_size());
	ASSERTM("first element", array->slot(0) == copy->slot(0));
}

void GCSpaceTest::testShallowCopyBytes()
{

// thinks about that. shallow copy is actual. Check later.
//	testShallowCopyBytes
//		| copy string |
//		string := 'ABCDEFG'.
//		copy := self shallowCopy: string.
//		self
//			assert: copy isString;
//			assert: copy size = string size;
//			assert: copy first = string first;
//			assert: copy = string
}

void GCSpaceTest::testShallowCopyBytes2()
{

	// thinks about that. shallow copy is actual. Check later.
//	testShallowCopyBytes2
//		| copy string |
//		string := 'ABCDEFGI'.
//		copy := self shallowCopy: string.
//		self
//			assert: copy isString;
//			assert: copy size = string size;
//			assert: copy first = string first;
//			assert: copy = string
}

void GCSpaceTest::testShallowCopyBytes3()
{

	// thinks about that. shallow copy is actual. Check later.
//	testShallowCopyBytes3
//		| copy string |
//		string := 'A'.
//		copy := self shallowCopy: string.
//		self
//			assert: copy isString;
//			assert: copy size = string size;
//			assert: copy first = string first;
//			assert: copy = string
}

void GCSpaceTest::testShallowCopyBytesExtended()
{

	// thinks about that. shallow copy is actual. Check later.
//	testShallowCopyBytesExtended
//		| copy string |
//		string := String new: 1027 withAll: $A.
//		copy := self shallowCopy: string.
//		self
//			assert: copy isString;
//			assert: copy size = string size;
//			assert: copy first = string first;
//			assert: copy = string
}

void GCSpaceTest::testShallowCopyExtended()
{

// thinks about that. shallow copy is actual. Check later.
//	testShallowCopyExtended
//		| array copy |
//		array := Array new: 1024 withAll: true.
//		copy := self shallowCopy: array.
//		self
//			assert: copy isArray;
//			assert: copy size = array size;
//			assert: array first = copy first;
//			assert: array = copy
}

void GCSpaceTest::testSynchronousGCSpace()
{

	// thinks about that. shallow copy is actual. Check later.
//	testSynchronousGCSpace
//		| space oldNextFree array |
//		space := SynchronousGCSpace currentFrom.
//		self
//			assert: space base < space nextFree;
//			assert: space base < space commitedLimit;
//			assert: space commitedLimit <= space reservedLimit;
//			assert: (space softLimit between: space base and: space reservedLimit);
//			assert: (space nextFree between: space base and: space commitedLimit).
//		oldNextFree := space nextFree.
//		array := Array new: 20.
//		self deny: oldNextFree = space nextFree
}



cute::suite make_suite_GCSpaceTest()
{
	cute::suite s;

	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testAllocate));
	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testGCSpace));
	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testExtendedGrowingTo));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testGrow));
	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testGrowingTo));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testNewGCSpaceShallowCopy));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testShallowCopy));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testShallowCopyBytes));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testShallowCopyBytes2));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testShallowCopyBytes3));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testShallowCopyBytesExtended));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testShallowCopyExtended));
//	s.push_back(CUTE_SMEMFUN(GCSpaceTest, testSynchronousGCSpace));

	return s;
}

