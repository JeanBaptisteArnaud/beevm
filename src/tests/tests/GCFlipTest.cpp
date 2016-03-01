
#include <iostream>
#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "GCFlipTest.h"
#include "MockedObjects.h"

#include "DataStructures/ObjectFormat.h"
#include "DataStructures/Memory.h"
#include "GarbageCollector/GenerationalGC.h"


extern cute::suite make_suite_GCFlipTest();

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
	ASSERTM("copy is not in toSpace", toSpace()->includes(copy));
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
	ASSERTM("copy is not in toSpace", oldSpace()->includes(copy));
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
	ASSERTM("copy is not in toSpace", oldSpace()->includes(copy));
	ASSERTM("remmenber set do not include copy", !(flipper()->rememberedSet.includes(copy)));

}

void GCFlipTest::testEphemeron()
{
	GCSpace previous = mockedObjects.setDefaultSpace(fromSpace());
	oop_t *key = mockedObjects.newObject();
	oop_t *value = mockedObjects.newObject();
	oop_t *ephemeron = mockedObjects.newEphemeron(key, value);
	mockedObjects.setDefaultSpace(&previous);
	
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
	ASSERTM("key changed" , ephemeron->slot(0) == key );
	ASSERTM("value not set" , toSpace()->includes(ephemeron->slot(1)) );
}

void GCFlipTest::testEphemeronOnce()
{
	GCSpace previous = mockedObjects.setDefaultSpace(fromSpace());
	oop_t *key = mockedObjects.newObject();
	oop_t *ephemeron = mockedObjects.newEphemeron(key, smiConst(2));
	oop_t *tombstone = mockedObjects.newObject();
	mockedObjects.setDefaultSpace(&previous);


	oop_t *root = mockedObjects.newArray(2);
	root->slot(0) = ephemeron;
	root->slot(1) = ephemeron;

	ASSERTM("key changed", ephemeron->slot(0) == key);
	ASSERTM("value changed", ephemeron->slot(1) == smiConst(2));

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
	ASSERTM("from/to bytes difference is wrong", (fromUsed - toUsed) == (headerBytes + padTo(7,4)) );
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
	oop_t *weakArray = fromSpace()->shallowCopy(mockedObjects.newWeakArray(1));
	oop_t *object = fromSpace()->shallowCopy(mockedObjects.newObject());

	weakArray->slot(0) = object;

	oop_t *root = mockedObjects.newArray(1);
	root->slot(0) = weakArray;

	ASSERTM("from space include", flipper()->arenaIncludes(object));
	
	oop_t *tombstone = smiConst(42);
	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->fixWeakContainers();

	oop_t *weak = root->slot(0);

	ASSERTM("weak is not in the to space", toSpace()->includes(weak));
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

void GCFlipTest::testPurgeEmptyRoots()
{
	flipper()->purgeRoots();
	ASSERTM("literals should be empty", flipper()->literalsReferences.isEmpty());
	ASSERTM("rememberedSet should be empty", flipper()->rememberedSet.isEmpty());
}

void GCFlipTest::testPurgeLiteralsWithNewObject()
{

	ReferencedVMArray *rememberedSet = &flipper()->rememberedSet;
	ReferencedVMArray *literalsReferences = &flipper()->literalsReferences;

	oop_t *anObject = fromSpace()->shallowCopy(mockedObjects.newObject());
	rememberedSet->add(anObject);
	literalsReferences->add(anObject);
	literalsReferences->add(smiConst(2));
	flipper()->purgeRoots();
	ASSERTM("literals size", literalsReferences->size()->_asNative() == 2);
	ASSERTM("rememberedSet should be empty", rememberedSet->isEmpty());
}

void GCFlipTest::testPurgeLiteralsWithOldObject()
{
	ReferencedVMArray *rememberedSet = &flipper()->rememberedSet;
	ReferencedVMArray *literalsReferences = &flipper()->literalsReferences;

	oop_t* anOldObject = mockedObjects.newObject();
	rememberedSet->add(anOldObject);
	literalsReferences->add(anOldObject);
	literalsReferences->add(smiConst(2));
	flipper()->purgeRoots();

	ASSERTM("literals should be empty", literalsReferences->isEmpty());
	ASSERTM("rememberedSet should be empty", rememberedSet->isEmpty());
}

void GCFlipTest::testRescuedEphemeron()
{
	oop_t *key = fromSpace()->shallowCopy(mockedObjects.newObject());
	oop_t *ephemeron = fromSpace()->shallowCopy(mockedObjects.newEphemeron(key, smiConst(2)));
	
	oop_t *root = mockedObjects.newArray(1);
	root->slot(0) = ephemeron;

	oop_t *tombstone = mockedObjects.newObject();

	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->rescueEphemerons();

	ASSERTM("rescued ephemeron is empty", !flipper()->rescuedEphemerons.isEmpty());

	ephemeron = flipper()->rescuedEphemerons.pop();
	key       = ephemeron->slot(0);
	oop_t *value = ephemeron->slot(1);

	ASSERTM("value is wrong" , value == smiConst(2) );
	ASSERTM("to space does not include ephemeron", toSpace()->includes(ephemeron));
	ASSERTM("to space does not include key" , toSpace()->includes(key));
}

void GCFlipTest::testRescuedEphemeronNoRescuedByValue()
{
	oop_t *key   = fromSpace()->shallowCopy(mockedObjects.newObject());
	oop_t *value = fromSpace()->shallowCopy(mockedObjects.newObject());
	oop_t *ephemeron = fromSpace()->shallowCopy(mockedObjects.newEphemeron(key, smiConst(2)));
	
	oop_t *root = mockedObjects.newArray(2);
	root->slot(0) = ephemeron;
	root->slot(1) = key;

	oop_t *tombstone = mockedObjects.newObject();

	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->rescueEphemerons();

	ASSERTM("rescued ephemeron is not empty", flipper()->rescuedEphemerons.isEmpty());

	ephemeron = root->slot(0);
	key       = ephemeron->slot(0);
	value     = ephemeron->slot(1);

	ASSERTM("to space does not include ephemeron", toSpace()->includes(ephemeron));
	ASSERTM("to space does not include key" , toSpace()->includes(key));
	ASSERTM("to space does not include value" , toSpace()->includes(value));
}


void GCFlipTest::testRescueEphemeronRescuedInRoots()
{
	oop_t *key   = fromSpace()->shallowCopy(mockedObjects.newObject());
	oop_t *value = fromSpace()->shallowCopy(mockedObjects.newObject());
	oop_t *ephemeron = fromSpace()->shallowCopy(mockedObjects.newEphemeron(key, smiConst(2)));
	
	oop_t *root = mockedObjects.newArray(1);
	root->slot(0) = ephemeron;

	oop_t *clearRescuedEphemeronsArray = flipper()->localSpace.shallowCopy(flipper()->rescuedEphemerons.contents);

	oop_t *tombstone = mockedObjects.newObject();

	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->rescueEphemerons();
	flipper()->makeRescuedEphemeronsNonWeak();

	ASSERTM("rescued ephemeron is empty", !flipper()->rescuedEphemerons.isEmpty());
	
	flipper()->flipSpaces();
	flipper()->rescuedEphemerons.contents = clearRescuedEphemeronsArray;

	flipper()->rememberedSet[1] = KnownObjects::nil;
	flipper()->purgeRememberedSet();
	flipper()->followRoots();
	flipper()->rescueEphemerons();

	ASSERTM("rescued ephemeron is not empty", flipper()->rescuedEphemerons.isEmpty());
	ASSERTM("rememberedSet shouldn't be empty", !flipper()->rememberedSet.isEmpty());
	ASSERTM("rememberedSet shouldn't be nil", flipper()->rememberedSet[1] != KnownObjects::nil);

	ephemeron = flipper()->rememberedSet[1]->slot(1);
	key   = ephemeron->slot(0);
	value = ephemeron->slot(1);

	ASSERTM("ephemeron value is wrong", value == smiConst(2));
	ASSERTM("old space should include ephemeron",   oldSpace()->includes(ephemeron));
	ASSERTM("old space should include key", oldSpace()->includes(key));
}

void GCFlipTest::testRescueNoEphemerons()
{
	flipper()->rescueEphemerons();
	ASSERTM("rescued ephemeron is not empty", flipper()->rescuedEphemerons.isEmpty());
}

/**
 * usual stack shape is shown next. Most fields are nil in these mocked tests.
 *
 * rightmost? local
 * ...
 * leftmost? local
 * current env
 * prev env (nil)
 * cm (nil)
 * receiver (nil)
 * prev framepointer
 * return address
 **/

void GCFlipTest::testStackCallbackTP19064()
{
	
}

void GCFlipTest::testStackFollowObjectAndCheckGraph()
{
	oop_t *inEnvironmentContext = mockedObjects.newArray(1);
	
	oop_t *arrayInStack = fromSpace()->shallowCopy(mockedObjects.newArray(3));
	
	arrayInStack->slot(1) = fromSpace()->shallowCopy(mockedObjects.newString("a String"));
	
	fromSpace()->shallowCopy(mockedObjects.newString("leaked"));
	
	arrayInStack->slot(2) = fromSpace()->shallowCopy(mockedObjects.newByteArray(3));
	arrayInStack->slot(0) = smiConst(1);

	// This was done in smalltalk to make inEnvironment be put in an environment context. In C that is done manually.
	// inEnvironmentContext->slot(0) = arrayInStack;

	mockedObjects.stackAddTemporary(arrayInStack);
	mockedObjects.stackEnvironment() = mockedObjects.newArray(1);
	mockedObjects.stackEnvironment()->slot(0) = inEnvironmentContext;

	inEnvironmentContext->slot(0) = KnownObjects::nil;

	flipper()->followStack();

	oop_t *movedInStack = mockedObjects.stackTemporary(0);;

	ulong toUsed = toSpace()->used();
	ulong fromUsed = fromSpace()->used();

	ASSERTM("toUsed >= current size", toUsed < fromUsed);

	ulong headerBytes = 8;
	ASSERTM("from/to bytes difference is wrong", (fromUsed - toUsed) == headerBytes + padTo(7,4) );
	ASSERTM("integer was modified", movedInStack->slot(0) == smiConst(1));
	ASSERTM("string was modified", movedInStack->slot(1)->equalsStr("a String") );

	oop_t *byteArray = mockedObjects.newByteArray(3);
	ASSERTM("byte array was modified", movedInStack->slot(2)->equalsByteArray(byteArray));
}

void GCFlipTest::testStackFollowObjectCallbackHole()
{
}

void GCFlipTest::testStackFollowObjectNestedBlock()
{
}

void GCFlipTest::testWeakContainer()
{
	oop_t *weakArray = fromSpace()->shallowCopy(mockedObjects.newWeakArray(1));
	oop_t *object = fromSpace()->shallowCopy(mockedObjects.newObject());

	weakArray->slot(0) = object;

	oop_t *root = mockedObjects.newArray(2);
	root->slot(0) = weakArray;
	root->slot(1) = object;
	
	oop_t *tombstone = mockedObjects.newObject();
	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->fixWeakContainers();

	oop_t *weak = root->slot(0);
	object = root->slot(1);

	ASSERTM("weak is not in the to space", toSpace()->includes(weak));
	ASSERTM("object is not in the to space", toSpace()->includes(object));
	ASSERTM("weak reference shouldn't have been tombstoned", weak->slot(0) != tombstone);
	ASSERTM("weak reference shouldn't have changed", weak->slot(0) == object);
}

void GCFlipTest::testWeakContainerExtended()
{
	oop_t *weakArray = fromSpace()->shallowCopy(mockedObjects.newWeakArray(1024));
	oop_t *object = fromSpace()->shallowCopy(mockedObjects.newObject());

	weakArray->slot(0) = object;

	oop_t *root = mockedObjects.newArray(2);
	root->slot(0) = weakArray;
	root->slot(1) = object;
	
	oop_t *tombstone = mockedObjects.newObject();
	flipper()->addRoot(root);
	flipper()->tombstone(tombstone);
	flipper()->followRoots();
	flipper()->fixWeakContainers();

	oop_t *weak = root->slot(0);
	object = root->slot(1);

	ASSERTM("weak is not in the to space", toSpace()->includes(weak));
	ASSERTM("object is not in the to space", toSpace()->includes(object));
	ASSERTM("weak reference shouldn't have been tombstoned", weak->slot(0) != tombstone);
	ASSERTM("weak reference shouldn't have changed", weak->slot(0) == object);
}


cute::suite make_suite_GCFlipTest()
{
	cute::suite s;

	s.push_back(CUTE_SMEMFUN(GCFlipTest, testGCCollect));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToFlip));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToOld));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testCopyToOldBug));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testEphemeron));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testEphemeronOnce));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testFollowObject));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testFollowObjectAndCheckGraph));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testFollowObjectCheckGraphAndOop));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testGCReferencesAfterCollect));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testPurgeEmptyRoots));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testPurgeLiteralsWithNewObject));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testPurgeLiteralsWithOldObject));

	s.push_back(CUTE_SMEMFUN(GCFlipTest, testRescuedEphemeron));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testRescueEphemeronRescuedInRoots));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testRescueNoEphemerons));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testStackCallbackTP19064));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testStackFollowObjectAndCheckGraph));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testStackFollowObjectCallbackHole));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testStackFollowObjectNestedBlock));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testWeakContainer));
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testWeakContainerExtended));

	
	s.push_back(CUTE_SMEMFUN(GCFlipTest, testTombstone));

	

	//s.push_back(CUTE(softLimitTests));
	//s.push_back(CUTE(reservedLimitTests));
	//s.push_back(CUTE(commitedLimitTests));
	//s.push_back(CUTE(nextFreeTests));
	//s.push_back(CUTE(base_3Tests));
	return s;
}

