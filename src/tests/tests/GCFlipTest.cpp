
#include <iostream>
#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "GCFlipTest.h"
#include "MockedObjects.h"

#include "DataStructures/ObjectFormat.h"
#include "DataStructures/Memory.h"
#include "GarbageCollector/GenerationalGC.h"


extern cute::suite make_suite_GCTest();

using namespace Bee;
using namespace std;

// File myclassTest.h


void GCFlipTest::setUp()
{
	mockedObjects.initializeKnownObjects();
	flipper = new GenerationalGC();
	
	Memory::current()->setGC(flipper);
	GCSpace local = GCSpace::dynamicNew(1024 * 1024 * 4 * 6);
	flipper->localSpace = local;
	flipper->initLocals();
	flipper->initNonLocals();
}

void GCFlipTest::tearDown()
{
	Memory::current()->releaseEverything();
}


void GCFlipTest::testGCCollect()
{
	flipper->collect();
}

void GCFlipTest::testCopyToFlip()
{
	oop_t *array = mockedObjects.mockArray(1);
	oop_t *copy  = flipper->fromSpace.shallowCopy(array);
	copy = flipper->moveToOldOrTo(copy);
	ASSERTM("copy is not an array", isArray(copy));
	ASSERTM("size is different", copy->_size() == array->_size());
	ASSERTM("First Element is different", array->slot(0) == copy->slot(0));
	ASSERTM("array are different", isSameArray(array, copy));
	ASSERTM("oopy is not in toSpace", flipper->toSpace.includes(copy));
	ASSERTM("remmenber set do not include copy",
			!(flipper->rememberSet.includes(copy)));
}

void GCFlipTest::testCopyToOld()
{
	oop_t *array = mockedObjects.mockArray(1);
	array->slot(0) = smiConst(1);
	oop_t *copy = flipper->fromSpace.shallowCopy(array);
	copy->slot(0) = smiConst(3);
	copy = flipper->moveToOldOrTo(copy);
	copy->slot(0) = smiConst(4);
	ASSERTM("copy is not in toSpace", flipper->toSpace.includes(copy));
	copy = flipper->moveToOldOrTo(copy);
	copy->slot(0) = smiConst(1);
	ASSERTM("copy is not an array", isArray(copy));
	ASSERTM("size is different", copy->_size() == array->_size());
	ASSERTM("First Element is different", array->slot(0) == copy->slot(0));
	ASSERTM("array are different", isSameArray(array, copy));
	ASSERTM("oopy is not in toSpace", flipper->oldSpace.includes(copy));
	ASSERTM("remmenber set do not include copy",
			!(flipper->rememberSet.includes(copy)));
}

void GCFlipTest::testCopyToOldBee() {

	oop_t *array = mockedObjects.mockArray(1);
	array->setSlot(0, smiConst(1));
	oop_t *copy = flipper->fromSpace.shallowCopy(array);

	copy->setSlot(0, smiConst(3));
	copy = flipper->moveToOldOrTo(copy);
	copy->setSlot(0, smiConst(4));
	
	ASSERTM("copy is not in toSpace", flipper->toSpace.includes(copy));

	copy = flipper->moveToOldOrTo(flipper->moveToOldOrTo(copy));
	copy->setSlot(0, smiConst(1));
	
	ASSERTM("copy is not an array", isArray(copy));
	ASSERTM("size is different", copy->_size() == array->_size());
	ASSERTM("First Element is different", array->slot(0) == copy->slot(0));
	ASSERTM("array are different", isSameArray(array, copy));
	ASSERTM("oopy is not in toSpace", flipper->oldSpace.includes(copy));
	ASSERTM("remmenber set do not include copy", !(flipper->rememberSet.includes(copy)));

}

void GCFlipTest::testEphemeron() {
	oop_t *key = mockedObjects.mockObjectFrom();
	oop_t *value = mockedObjects.mockObjectFrom();
	oop_t *ephemeron = mockedObjects.mockEphemeronFrom(key, value);
	oop_t *tombstone = mockedObjects.mockObjectFrom();
	oop_t *root = mockedObjects.mockArray(1024);
	root->slot(0) = ephemeron;
	root->slot(1) = key;

	ASSERTM("key change", ephemeron->slot(0) == key);
	ASSERTM("value change", ephemeron->slot(1) == value);

	flipper->addRoot(root);
	flipper->tombstone(tombstone);
	flipper->followRoots();
	flipper->rescueEphemerons();
	ASSERTM("rescued ephemeron is not empty",
			flipper->rescuedEphemerons.isEmpty());

	ephemeron = root->slot(0);
	key       = root->slot(1);

	ASSERTM("To space do not include ephemeron",
			(flipper->toSpace.includes(ephemeron)));
//	ASSERTM("Key is not in to space" ,(flipper->toSpace.includes(key)));
//	ASSERTM("key change" , ephemeron[1] == (ulong) key);
//	ASSERTM("value not set" ,(flipper->toSpace.includes((unsigned long *) ephemeron[2])));
}

void GCFlipTest::testEphemeronOnce() {
	oop_t *key = mockedObjects.mockObjectFrom();
	oop_t *ephemeron = mockedObjects.mockEphemeronFrom(key, smiConst(2));
	oop_t *tombstone = mockedObjects.mockObjectFrom();
	oop_t *root = mockedObjects.mockArray(1024);
	root->slot(0) = ephemeron;
	root->slot(1) = ephemeron;

	ASSERTM("key change", ephemeron->slot(0) == key);
	ASSERTM("value change", ephemeron->slot(1) == smiConst(2));

	flipper->addRoot(root);
	flipper->tombstone(tombstone);
	flipper->followRoots();
	flipper->rescueEphemerons();

	ASSERTM("rescued ephemeron is empty",
			!(flipper->rescuedEphemerons.isEmpty()));

	ephemeron = flipper->rescuedEphemerons.pop();
	ASSERTM("rescued ephemeron is not empty",
			flipper->rescuedEphemerons.isEmpty());
	ASSERTM("??", ephemeron->slot(1) == smiConst(2));
	//	ASSERTM("Ephemeron is not in to space" ,(flipper->toSpace.includes(ephemeron)));
	//	ASSERTM("Key is not in to space" ,(flipper->toSpace.includes(key)));
}

void GCFlipTest::testFollowObject() {
	oop_t *array = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	oop_t *string = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	flipper->fromSpace.shallowCopy(mockedObjects.mockArray(1024));
	oop_t *byteArray = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	oop_t *root = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));

	long flipSize = flipper->toSpace.getNextFree() - flipper->toSpace.getBase();
	long currentSize = flipper->fromSpace.getNextFree() - flipper->fromSpace.getBase();

	ASSERTM("ToSpace not empty 0", flipSize == 0);
	ASSERTM("ToSpace not correctly allocated ", currentSize == (1024 + 32));


	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;
	root->slot(0) = array;
	//string = byteArray = array = nil;
	flipper->addRoot(root);
	flipper->followRoots();

	flipSize = flipper->toSpace.getNextFree() - flipper->toSpace.getBase();
	currentSize = flipper->fromSpace.getNextFree() - flipper->fromSpace.getBase();

	ASSERTM("flipSize is not empty", flipSize);
	ASSERTM("flipSize >= current size", flipSize < currentSize);
	// check the number
	ASSERTM("calculation of pointer", (currentSize - flipSize) == (1035));
}

void GCFlipTest::testGCReferencesAfterCollect() {
// cannot test that.
//	mockVMValue();
//	GenerationalGC * flipper = new GenerationalGC();
//	Memory::current()->setGC(flipper);
//	GCSpace local = GCSpace::dynamicNew(1024 * 1024 * 4 * 6);
//	flipper->localSpace = local;
//	flipper->collect();
//	testGCReferencesSetupFramePointer
//		| flipper local |
//		flipper := GenerationalGC new.
//		local := GCSpace dynamicNew: 1024 * 1024 * 4 * 6.
//		flipper localSpace: local.
//		self
//			execute: [:proxy | proxy initLocals; setupFramePointer]
//			proxying: flipper.
//		self
//			assert: (flipper instVars
//				conform: [:var | self checkValid: var using: local])
}

void GCFlipTest::testTombstone() {

	oop_t *weakArray = flipper->fromSpace.shallowCopy(mockedObjects.mockWeakArray());
	oop_t *toGarbage = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));

	weakArray->setSlot(0, toGarbage);
	weakArray->setSlot(1, toGarbage);

	oop_t *root = mockedObjects.mockArray(3);
	root->setSlot(0, weakArray);
	ASSERTM("from space include", flipper->arenaIncludes(toGarbage));
	oop_t *tombstone = smiConst(42);
	flipper->addRoot(root);
	flipper->tombstone(tombstone);
	flipper->followRoots();
	flipper->fixWeakContainers();
	oop_t *weak = root->slot(0);
	ASSERTM("weak is not in the to space", flipper->toSpace.includes(weak));
	ASSERTM("tombstone fail", weak->slot(1) == tombstone);
	ASSERTM("tombstone fail", weak->slot(0) == tombstone);

}

void GCFlipTest::testFollowObjectAndCheckGraph() {
	oop_t *string = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	flipper->fromSpace.shallowCopy(mockedObjects.mockArray(1024));
	oop_t *byteArray = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	oop_t *root = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	long flipSize = flipper->toSpace.getNextFree() - flipper->toSpace.getBase();
	long currentSize = flipper->fromSpace.getNextFree()
			- flipper->fromSpace.getBase();

	ASSERTM("ToSpace not empty 0", flipSize == 0);
	ASSERTM("ToSpace not correctly allocated ", currentSize == (1024 + 32));

	root->slot(0) = smiConst(1);
	root->slot(1) = string;
	root->slot(2) = byteArray;
	//string = byteArray = array = nil;
	flipper->addRoot(root);
	flipper->followRoots();

	flipSize = flipper->toSpace.getNextFree() - flipper->toSpace.getBase();
	currentSize = flipper->fromSpace.getNextFree() - flipper->fromSpace.getBase();

//	self
//		assert: toSize < fromSize;
//		assert: fromSize - toSize = (8 + ('leaked' size roundTo: 4)) _asPointer;
//		assert: root first first = 1;
//		assert: root first second = 'a String';
//		assert: root first third = #[1 2 3]

	ASSERTM("flipSize is not empty", flipSize);
	ASSERTM("flipSize >= current size", flipSize < currentSize);

	// check the number
//	ASSERTM("calculation of pointer", (currentSize - flipSize) == (1035));
//	ASSERTM("array ", (currentSize - flipSize) == (1035));
	ASSERTM("root 1 change", root->slot(0) == smiConst(1));
	ASSERTM("root 2 mockArray 2 change", mockedObjects.checkMockArray2(root->slot(1)));
	ASSERTM("root 3 mockArray 3 change", mockedObjects.checkMockArray2(root->slot(2)));

}



void GCFlipTest::testFollowObjectCheckGraphAndOop() {
	// no need it
	oop_t *string = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	flipper->fromSpace.shallowCopy(mockedObjects.mockArray(1024));
	oop_t *byteArray = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	oop_t *root = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	long flipSize = flipper->toSpace.getNextFree() - flipper->toSpace.getBase();
	long currentSize = flipper->fromSpace.getNextFree()
			- flipper->fromSpace.getBase();

	ASSERTM("ToSpace not empty 0", flipSize == 0);
	ASSERTM("ToSpace not correctly allocated ", currentSize == (1024 + 32));

	root->slot(0) = smiConst(1);
	root->slot(1) = string;
	root->slot(2) = byteArray;
	//string = byteArray = array = nil;
	flipper->addRoot(root);
	flipper->followRoots();

	flipSize    = flipper->toSpace.getNextFree()   - flipper->toSpace.getBase();
	currentSize = flipper->fromSpace.getNextFree() - flipper->fromSpace.getBase();
//	self
//		assert: toSize < fromSize;
//		assert: fromSize - toSize = (8 + ('leaked' size roundTo: 4)) _asPointer;
//		assert: root first first = 1;
//		assert: root first second = 'a String';
//		assert: root first third = #[1 2 3]
	ASSERTM("flipSize is not empty", flipSize);
	ASSERTM("flipSize >= current size", flipSize < currentSize);
	// check the number
//	ASSERTM("calculation of pointer", (currentSize - flipSize) == (1035));
//	ASSERTM("array ", (currentSize - flipSize) == (1035));
	ASSERTM("root 1 change", root->slot(0) == smiConst(1));
	ASSERTM("root 2 mockArray 2 change", mockedObjects.checkMockArray2(root->slot(1)));
	ASSERTM("root 3 mockArray 3 change", mockedObjects.checkMockArray2(root->slot(2)));

}


void GCFlipTest::testPurgeLiteralsWithNewObject() {
	oop_t *array = flipper->fromSpace.shallowCopy(mockedObjects.mockArray(3));
	ReferencedVMArray * rememberSet = &(flipper->rememberSet);
	//rememberSet->add(Object); //??
	ReferencedVMArray * literals = &(flipper->literalsReferences);
	literals->add(array);
	literals->add(smiConst(2));
	flipper->purgeRoots();
	ASSERTM("literals size ", literals->size()->_asNative() == 2);
	ASSERTM("rememberSet isEmpty", rememberSet->isEmpty());

}


cute::suite make_suite_GCTest() {
	cute::suite s;

	s.push_back(CUTE_SMEMFUN(GCFlipTest, testGCCollect));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToFlip));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToOld));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToOldBee));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testEphemeron));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testFollowObject));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testGCReferencesAfterCollect));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testTombstone));

	// ==> to make it work need to do VMArrayReferenced
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testEphemeronOnce));

	// bug in the Follow not all array is copied
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testFollowObjectAndCheckGraph));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testFollowObjectCheckGraphAndOop));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testPurgeLiteralsWithNewObject));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testGCReferencesAfterCollect));

	//s.push_back(CUTE(softLimitTests));
	//s.push_back(CUTE(reservedLimitTests));
	//s.push_back(CUTE(commitedLimitTests));
	//s.push_back(CUTE(nextFreeTests));
	//s.push_back(CUTE(base_3Tests));
	return s;
}

