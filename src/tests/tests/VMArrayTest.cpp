
#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "VMArrayTest.h"

#include "DataStructures/ObjectFormat.h"
#include "DataStructures/VMArray.h"

extern cute::suite make_suite_VMArrayTest();

using namespace Bee;


VMArrayTest::VMArrayTest() 
{
	this->setUp();
}

VMArrayTest::~VMArrayTest()
{
	this->tearDown();
}

void VMArrayTest::setUp()
{
	mockedLocal.initializeKnownObjects();
	mockedFrom.setParent(&mockedLocal);
}

void VMArrayTest::tearDown()
{
}

void VMArrayTest::testAdd()
{
	oop_t* contents = mockedLocal.newArray(1024);
	VMArray array;
	array.emptyWith(contents);
	array.reset();
	ASSERTM("Initial Size is wrong", array.size() == smiConst(0));
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	ASSERTM("Size is wrong", array.size() == smiConst(3));
	ASSERTM("NextFree is wrong", array.nextFree() == smiConst(4));
	ASSERTM("Should not be empty", !array.isEmpty());
}

void VMArrayTest::testGrow2()
{
	oop_t* contents = mockedLocal.newArray(1024);
	VMArray array;
	array.emptyWith(contents);
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	ASSERTM("Size is wrong", array.size() == smiConst(3));
	ASSERTM("NextFree is wrong", array.nextFree() == smiConst(4));
	ASSERTM("Should not be empty", !array.isEmpty());
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(3));
	array.add(smiConst(3));
	array.add(smiConst(1));
	array.add(smiConst(2));
	ASSERTM("Size is wrong", array.size() == smiConst(13));
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(3));
	array.add(smiConst(3));
	array.add(smiConst(1));
	array.add(smiConst(2));
	// ASSERTM("raw size is wrong", array.contents->_size() == 30); doesn't make sense
}

void VMArrayTest::testAddAll()
{
	oop_t* contents = mockedLocal.newArray(1024);
	oop_t* other = mockedLocal.newArray(3);
	other->slot(0) = smiConst(5);
	other->slot(1) = smiConst(6);
	other->slot(2) = smiConst(7);
	VMArray array;
	array.emptyWith(contents);
	array.reset();
	ASSERTM("Initial Size is wrong", array.size() == smiConst(0));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(4));
	array.addAll(other);
	ASSERTM("Size is wrong", array.size() == smiConst(6));
	ASSERTM("NextFree is wrong", array.nextFree() == smiConst(7));
	ASSERTM("Should not be empty", !array.isEmpty());
	ulong answer = 1;
	for (long index = 1; index <= array.size()->_asNative(); index++) {
		answer = answer * array[index]->_asNative();
	}
	ASSERTM("loop result should be 5040 ", answer == 5040);
	ASSERTM("First pop should be 7", array.pop() == smiConst(7));
	ASSERTM("First pop should be 6", array.pop() == smiConst(6));
	ASSERTM("First pop should be 5", array.pop() == smiConst(5));
	ASSERTM("Should not be empty", !array.isEmpty());
}

void VMArrayTest::testDo()
{
	oop_t* contents = mockedLocal.newArray(1024);
	VMArray array;
	array.emptyWith(contents);
	array.reset();
	ASSERTM("Initial Size is wrong", array.size() == smiConst(0));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(4));
	ASSERTM("Size is wrong", array.size() == smiConst(3));
	ASSERTM("NextFree is wrong", array.nextFree() == smiConst(4));
	ASSERTM("Should not be empty", !array.isEmpty());
	ulong answer = 1;
	for (long index = 1; index <= array.size()->_asNative(); index++) {
		answer = answer * array[index]->_asNative();
	}
	ASSERTM("loop result should be 2", answer == 24);
	ASSERTM("First pop should be 4", array.pop() == smiConst(4));
	ASSERTM("First pop should be 3", array.pop() == smiConst(3));
	ASSERTM("First pop should be 2", array.pop() == smiConst(2));
	ASSERTM("Should be empty", array.isEmpty());
}

void VMArrayTest::testGrow()
{
	VMArray array;
	GCSpace space = GCSpace::dynamicNew(16*1024);
	array.setSpace(&space);
	array.emptyReserving(100);
	ulong size = array.contents->_size();

	ASSERTM("Size is wrong", array.contents->_size() >= 100);

	for (ulong i = 0; i < 99; i++)
		array.push(smiConst(1));

	ASSERTM("Size is wrong", array.contents->_size() == size);

	array.push(smiConst(1));

	ASSERTM("Size is wrong", array.contents->_size() > 100);
	ASSERTM("Size is wrong", array.contents->_size() > size);
}

void VMArrayTest::testPop()
{
	oop_t* contents = mockedLocal.newArray(1024);
	VMArray array;
	array.emptyWith(contents);
	array.reset();
	ASSERTM("Initial Size is wrong", array.size() == smiConst(0));
	array.add(smiConst(1));
	array.add(smiConst(2));
	array.add(smiConst(3));
	ASSERTM("Size is wrong", array.size() == smiConst(3));
	ASSERTM("NextFree is wrong", array.nextFree() == smiConst(4));
	ASSERTM("Should not be empty", !array.isEmpty());
	ASSERTM("First pop should be 3", array.pop() == smiConst(3));
	ASSERTM("First pop should be 2", array.pop() == smiConst(2));
	ASSERTM("First pop should be 1", array.pop() == smiConst(1));
	ASSERTM("Should be empty", array.isEmpty());
}

void VMArrayTest::testReset()
{
	oop_t* contents = mockedLocal.newArray(1024);
	oop_t* other    = mockedLocal.newArray(3); // 3
	other->slot(0) = smiConst(5);
	other->slot(1) = smiConst(6);
	other->slot(2) = smiConst(7);
	VMArray array;
	array.emptyWith(contents);
	array.reset();
	ASSERTM("Initial Size is wrong", array.size() == smiConst(0));
	array.add(smiConst(2));
	array.add(smiConst(3));
	array.add(smiConst(4));
	array.addAll(other);
	ASSERTM("Size is wrong", array.size() == smiConst(6));
	ASSERTM("NextFree is wrong", array.nextFree() == smiConst(7));
	ASSERTM("Should not be empty here", !array.isEmpty());
	ulong answer = 1;
	for (long index = 1; index <= array.size()->_asNative(); index++)
	{
		answer = answer * array[index]->_asNative();
	}

	ASSERTM("loop result should be 5040 ", answer == 5040);
	ASSERTM("First pop should be 7", array.pop() == smiConst(7));
	ASSERTM("First pop should be 6", array.pop() == smiConst(6));
	ASSERTM("First pop should be 5", array.pop() == smiConst(5));
	ASSERTM("Should not be empty", !array.isEmpty());
	array.reset();
	ASSERTM("Should be empty after reset", array.isEmpty());
}

void VMArrayTest::testUpdateReference()
{
//	testUpdateReference
//		| space array referer size |
//		space := GCSpace dynamicNew: 1024 * 4 * 10.
//		array := ReferencedVMArray new on: space.
//		referer := Array new: 1.
//		size := array contents size.
//		self
//			execute: [:vmArray |
//				vmArray emptyReserving: 100.
//				referer at: 1 put: vmArray contents.
//				vmArray contents: nil; referer: referer oop.
//				self assert: vmArray contents == referer first.
//				size timesRepeat: [vmArray push: 1]]
//			proxying: array.
//		self
//			assert: array contents size > size;
//			assert: array contents == referer first
}

cute::suite make_suite_VMArrayTest()
{
	cute::suite s;

	s.push_back(CUTE_SMEMFUN(VMArrayTest, testAdd));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testAddAll));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testDo));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testGrow));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testGrow2));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testPop));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testReset));
	s.push_back(CUTE_SMEMFUN(VMArrayTest, testUpdateReference));
	return s;
}

