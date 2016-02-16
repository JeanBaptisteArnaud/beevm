
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

GCFlipTest::GCFlipTest()
{
	setUp();
}

GCFlipTest::~GCFlipTest()
{
	tearDown();
}

void GCFlipTest::setUp()
{
	mockedObjects.initializeKnownObjects();
	memory = this->memoryForTesting();

	flipper()->setUpLocals();
	flipper()->setUpNonLocals();

}

void GCFlipTest::tearDown()
{
}

GenerationalGC* GCFlipTest::flipper()
{
	return memory->flipper;
}

GCSpace* GCFlipTest::fromSpace()
{
	return &memory->flipper->fromSpace;
}

GCSpace* GCFlipTest::toSpace()
{
	return &memory->flipper->toSpace;
}

GCSpace* GCFlipTest::oldSpace()
{
	return &memory->flipper->oldSpace;
}


void GCFlipTest::testGCCollect()
{
	memory->scavengeFromSpace();
}

void GCFlipTest::testCopyToFlip()
{
	oop_t *array = mockedObjects.newArray(1);
	oop_t *copy  = fromSpace()->shallowCopy(array);
	copy = flipper()->moveToOldOrTo(copy);

	ASSERTM("copy is not an array", isArray(copy));
	ASSERTM("size is different", copy->_size() == array->_size());
	ASSERTM("first element is different", array->slot(0) == copy->slot(0));
	ASSERTM("array are different", isSameArray(array, copy));
	ASSERTM("oop is not in toSpace", toSpace()->includes(copy));
	ASSERTM("remenbered set do not include copy", !(flipper()->rememberedSet.includes(copy)));
}

void GCFlipTest::testCopyToOld()
{
	oop_t *array = mockedObjects.newArray(1);
	array->slot(0) = smiConst(1);
	oop_t *copy = fromSpace()->shallowCopy(array);
	copy->slot(0) = smiConst(3);
	copy = flipper()->moveToOldOrTo(copy);
	copy->slot(0) = smiConst(4);
	ASSERTM("copy is not in toSpace", toSpace()->includes(copy));
	copy = flipper()->moveToOldOrTo(copy);
	copy->slot(0) = smiConst(1);
	ASSERTM("copy is not an array", isArray(copy));
	ASSERTM("size is different", copy->_size() == array->_size());
	ASSERTM("First Element is different", array->slot(0) == copy->slot(0));
	ASSERTM("array are different", isSameArray(array, copy));
	ASSERTM("oopy is not in toSpace", oldSpace()->includes(copy));
	ASSERTM("remmenber set do not include copy", !flipper()->rememberedSet.includes(copy));
}

void GCFlipTest::testCopyToOldBug()
{

	oop_t *array = mockedObjects.newArray(1);
	array->slot(0) = smiConst(1);
	oop_t *copy = fromSpace()->shallowCopy(array);
	copy->slot(0) = smiConst(3);
	copy = flipper()->moveToOldOrTo(copy);
	copy->slot(0) = smiConst(4);
	ASSERTM("copy is not in toSpace", toSpace()->includes(copy));
	copy = flipper()->moveToOldOrTo(flipper()->moveToOldOrTo(copy));
	copy->slot(0) = smiConst(1);
	
	ASSERTM("copy is not an array", isArray(copy));
	ASSERTM("size is different", copy->_size() == array->_size());
	ASSERTM("First Element is different", array->slot(0) == copy->slot(0));
	ASSERTM("array are different", isSameArray(array, copy));
	ASSERTM("oopy is not in toSpace", oldSpace()->includes(copy));
	ASSERTM("remmenber set do not include copy", !(flipper()->rememberedSet.includes(copy)));

}

void GCFlipTest::testEphemeron()
{
	GCSpace default = mockedObjects.setDefaultSpace(fromSpace());
	oop_t *key = mockedObjects.newObject();
	oop_t *value = mockedObjects.newObject();
	oop_t *ephemeron = mockedObjects.newEphemeron(key, value);
	mockedObjects.setDefaultSpace(&default);
	
	oop_t *tombstone = mockedObjects.newObject();
	oop_t *root = mockedObjects.newArray(2);
	root->slot(0) = ephemeron;
	root->slot(1) = key;

	ASSERTM("key change", ephemeron->slot(0) == key);
	ASSERTM("value change", ephemeron->slot(1) == value);

	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->rescueEphemerons();
	ASSERTM("rescued ephemeron is not empty", flipper()->rescuedEphemerons.isEmpty());

	ephemeron = root->slot(0);
	key       = root->slot(1);

	ASSERTM("To space do not include ephemeron", toSpace()->includes(ephemeron));
	ASSERTM("Key is not in to space" , toSpace()->includes(key));
	ASSERTM("key change" , ephemeron->slot(0) == key );
	ASSERTM("value not set" , toSpace()->includes(ephemeron->slot(1)) );
}

void GCFlipTest::testEphemeronOnce()
{
	GCSpace default = mockedObjects.setDefaultSpace(fromSpace());
	oop_t *key = mockedObjects.newObject();
	oop_t *ephemeron = mockedObjects.newEphemeron(key, smiConst(2));
	oop_t *tombstone = mockedObjects.newObject();
	mockedObjects.setDefaultSpace(&default);


	oop_t *root = mockedObjects.newArray(2);
	root->slot(0) = ephemeron;
	root->slot(1) = ephemeron;

	ASSERTM("key change", ephemeron->slot(0) == key);
	ASSERTM("value change", ephemeron->slot(1) == smiConst(2));

	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->rescueEphemerons();

	ASSERTM("rescued ephemeron is empty", !flipper()->rescuedEphemerons.isEmpty());

	ephemeron = flipper()->rescuedEphemerons.pop();
	ASSERTM("rescued ephemeron is not empty", flipper()->rescuedEphemerons.isEmpty());
	ASSERTM("ephemeron value is wrong", ephemeron->slot(1) == smiConst(2));
	ASSERTM("ephemeron is not in to space", toSpace()->includes(ephemeron));
	ASSERTM("key is not in to space", toSpace()->includes(ephemeron->slot(0)));
}

// only works when padding = 2^k
ulong padTo(ulong number, ulong padding)
{
	return (number + padding - 1) & ~(padding-1);
}

void GCFlipTest::testFollowObject()
{
	oop_t *array = fromSpace()->shallowCopy(mockedObjects.newArray(3));
	oop_t *string = fromSpace()->shallowCopy(mockedObjects.newString("a string"));
	fromSpace()->shallowCopy(mockedObjects.newString("leaked"));
	oop_t *byteArray = fromSpace()->shallowCopy(mockedObjects.newByteArray(3));

	long toUsed = toSpace()->used();
	long fromUsed = fromSpace()->used();

	ulong headerBytes = 8;
	ASSERTM("ToSpace not empty 0", toUsed == 0);
	ASSERTM("ToSpace not correctly allocated ", fromUsed == (3*4 + padTo(9,4) + padTo(7,4) + padTo(3,4) + headerBytes * 4));

	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;

	oop_t *root = mockedObjects.newArray(1);
	root->slot(0) = array;

	//string = byteArray = array = nil;
	flipper()->addRoot(root);
	flipper()->followRoots();

	toUsed = toSpace()->used();
	fromUsed = fromSpace()->used();

	ASSERTM("toUsed is not empty", toUsed);
	ASSERTM("toUsed >= current size", toUsed < fromUsed);
	// check the number
	ASSERTM("calculation of pointer", (fromUsed - toUsed) == (headerBytes + padTo(7,4)) );
}

void GCFlipTest::testGCReferencesAfterCollect()
{
// cannot test that.
//	mockVMValue();
//	GenerationalGC * flipper = new GenerationalGC();
//	GCSpace local = GCSpace::dynamicNew(1024 * 1024 * 4 * 6);
//	flipper()->localSpace = local;
//	flipper()->collect();
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

void GCFlipTest::testTombstone()
{

	oop_t *weakArray = fromSpace()->shallowCopy(mockedObjects.newWeakArray());
	oop_t *toGarbage = fromSpace()->shallowCopy(mockedObjects.newArray(3));

	weakArray->slot(0) = toGarbage;
	weakArray->slot(1) = toGarbage;

	oop_t *root = mockedObjects.newArray(3);
	root->slot(0) = weakArray;
	ASSERTM("from space include", flipper()->arenaIncludes(toGarbage));
	oop_t *tombstone = smiConst(42);
	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->fixWeakContainers();
	oop_t *weak = root->slot(0);
	ASSERTM("weak is not in the to space", toSpace()->includes(weak));
	ASSERTM("tombstone fail", weak->slot(1) == tombstone);
	ASSERTM("tombstone fail", weak->slot(0) == tombstone);

}

void GCFlipTest::testFollowObjectAndCheckGraph()
{
	oop_t *array = fromSpace()->shallowCopy(mockedObjects.newArray(3));
	oop_t *string = fromSpace()->shallowCopy(mockedObjects.newString("a String"));
	fromSpace()->shallowCopy(mockedObjects.newString("leaked"));
	oop_t *byteArray = fromSpace()->shallowCopy(mockedObjects.newByteArray(3));

	long toUsed = toSpace()->used();
	long fromUsed = fromSpace()->used();

	ulong headerBytes = 8;

	ASSERTM("ToSpace not empty 0", toUsed == 0);
	ASSERTM("ToSpace not correctly allocated ", fromUsed == (3*4 + padTo(9,4) + padTo(7,4) + padTo(3,4) + headerBytes * 4));

	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;

	oop_t *root = mockedObjects.newArray(1);
	root->slot(0) = array;
	//string = byteArray = array = nil;

	flipper()->addRoot(root);
	flipper()->followRoots();

	toUsed = toSpace()->used();
	fromUsed = fromSpace()->used();

//		assert: fromSize - toSize = (8 + ('leaked' size roundTo: 4)) _asPointer;
//		assert: root first first = 1;
//		assert: root first second = 'a String';
//		assert: root first third = #[1 2 3]

	ASSERTM("toUsed is not empty", toUsed);
	ASSERTM("toUsed >= current size", toUsed < fromUsed);
	ASSERTM("from/to bytes difference is wrong", (fromUsed - toUsed) == headerBytes + padTo(7,4));
	ASSERTM("root 1 change", root->slot(0)->slot(0) == smiConst(1));
	ASSERTM("root 2 newArray 2 change", root->slot(0)->slot(1)->equalsStr("a String") );

	byteArray = mockedObjects.newByteArray(3);
	ASSERTM("root 3 newArray 3 change", root->slot(0)->slot(2)->equalsByteArray(byteArray));

}



void GCFlipTest::testFollowObjectCheckGraphAndOop()
{
	oop_t *array = fromSpace()->shallowCopy(mockedObjects.newArray(3));
	oop_t *string = fromSpace()->shallowCopy(mockedObjects.newString("a String"));
	fromSpace()->shallowCopy(mockedObjects.newString("leaked"));
	oop_t *byteArray = fromSpace()->shallowCopy(mockedObjects.newByteArray(3));

	long toUsed = toSpace()->used();
	long fromUsed = fromSpace()->used();

	ulong headerBytes = 8;

	ASSERTM("ToSpace not empty 0", toUsed == 0);
	ASSERTM("ToSpace not correctly allocated ", fromUsed == (3*4 + padTo(9,4) + padTo(7,4) + padTo(3,4) + headerBytes * 4));

	array->slot(0) = smiConst(1);
	array->slot(1) = string;
	array->slot(2) = byteArray;

	oop_t *root = mockedObjects.newArray(1);
	root->slot(0) = array;
	//string = byteArray = array = nil;

	flipper()->addRoot(root);
	flipper()->followRoots();

	toUsed = toSpace()->used();
	fromUsed = fromSpace()->used();


	ASSERTM("toUsed is not empty", toUsed);
	ASSERTM("toUsed >= current size", toUsed < fromUsed);
	ASSERTM("from/to bytes difference is wrong", (fromUsed - toUsed) == headerBytes + padTo(7,4));
	ASSERTM("root 1 change", root->slot(0)->slot(0) == smiConst(1));
	ASSERTM("root 2 newArray 2 change", root->slot(0)->slot(1)->equalsStr("a String") );

	ASSERTM("flipper didn't save the string", toSpace()->includes(root->slot(0)->slot(1)));

	byteArray = mockedObjects.newByteArray(3);
	ASSERTM("root 3 newArray 3 change", root->slot(0)->slot(2)->equalsByteArray(byteArray));

	ASSERTM("flipper didn't save the byte array", toSpace()->includes(root->slot(0)->slot(2)));
}


void GCFlipTest::testPurgeLiteralsWithNewObject()
{
	oop_t *array = fromSpace()->shallowCopy(mockedObjects.newArray(3));
	ReferencedVMArray * rememberedSet = &(flipper()->rememberedSet);
	//rememberedSet->add(Object); //??
	ReferencedVMArray * literals = &(flipper()->literalsReferences);
	literals->add(array);
	literals->add(smiConst(2));
	flipper()->purgeRoots();
	ASSERTM("literals size ", literals->size()->_asNative() == 2);
	ASSERTM("rememberedSet isEmpty", rememberedSet->isEmpty());

}


cute::suite make_suite_GCFlipTest()
{
	cute::suite s;

	s.push_back(CUTE_SMEMFUN(GCFlipTest, testGCCollect));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToFlip));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToOld));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToOldBug));
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

