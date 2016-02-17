
#include <windows.h>
#include <cstdlib>
#include <iostream>

#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "ObjectFormatTest.h"
#include "DataStructures/ObjectFormat.h"
#include "DataStructures/Memory.h"
#include "GarbageCollector/GenerationalGC.h"


extern cute::suite make_suite_VMMemoryTest();

using namespace std;
using namespace Bee;


void ObjectFormatTest::setUp()
{
	mockedObjects.initializeKnownObjects();
}

void ObjectFormatTest::tearDown()
{
}

void ObjectFormatTest::testHeaderOf()
{
	oop_t *object = mockedObjects.newArray(1);
	basic_header_t * h = object->basic_header();

	ASSERTM("Array 1 : Size",     h->size == 3);
	ASSERTM("Array 1 : Hash",     h->hash == 0x37F8);
	ASSERTM("Array 1 : Flags",    h->flags == basic_header_t::Flag_unseenInSpace);
	ASSERTM("Array 1 : behavior", h->behavior == (oop_t*)0x0A0792F0);

	object = mockedObjects.newArray(3);
	h = object->basic_header();
	ASSERTM("Array 2 : Size",     h->size == 3);
	ASSERTM("Array 2 : Hash",     h->hash == 0x0000);
	ASSERTM("Array 2 : Flags",    h->flags == basic_header_t::Flag_unseenInSpace);
	ASSERTM("Array 2 : behavior", h->behavior == (oop_t*)0x0A0792F0);

	object = KnownObjects::stTrue;
	h = object->basic_header();
	
	ASSERTM("True : Size",     h->size == 0);
	ASSERTM("True : Hash",     h->hash == 0x0000);
	ASSERTM("True : Flags",    h->flags ==  (basic_header_t::Flag_unseenInSpace |
											 basic_header_t::Flag_notIndexed |
											 basic_header_t::Flag_zeroTermOrNamed));
	ASSERTM("True : behavior", h->behavior == (oop_t*)0x0A0B6BA0);

	object = KnownObjects::nil;
	h = object->basic_header();
	ASSERTM("Nil : Size",  h->size == 0);
	ASSERTM("Nil : Hash",  h->hash == 0x3445);
	ASSERTM("Nil : Flags", h->flags == (basic_header_t::Flag_unseenInSpace |
										basic_header_t::Flag_notIndexed |
										basic_header_t::Flag_zeroTermOrNamed));
	ASSERTM("Nil : behavior", h->behavior == (oop_t*)0x0A0DDC10);

}

void ObjectFormatTest::testBasicSize()
{
	oop_t *object = mockedObjects.newArray(1);
	ASSERTM("size ", (object->_basicSize() == 3));
	object->_basicSize(122);
	ASSERTM("size ", (object->_basicSize() == 122));

	object = mockedObjects.newArray(1024);
	ASSERTM("size ", (object->_size() == 1024));
	object->_extendedSize(122);
	ASSERTM("size ", (object->_size() == 122));

}

void ObjectFormatTest::testBeExtended()
{
	oop_t *object = mockedObjects.newArray(1024);
	object->_beExtended();
	extended_header_t *h = object->extended_header();
	ASSERTM("sizeInWordBis", (h->basicSize == 4));
	ASSERTM("sizeInWord", (h->basic_header.size == 4));
	ASSERTM("flags",    h->basic_header.flags == (basic_header_t::Flag_unseenInSpace | basic_header_t::Flag_isExtended));
	ASSERTM("flagsBis", h->flags == (basic_header_t::Flag_unseenInSpace | basic_header_t::Flag_isExtended));
	ASSERTM("real size", (h->size == 1024));
}

void ObjectFormatTest::testRotateLeft()
{
	ulong i = 1;
	i = rotateLeft(i, 1);
	ASSERTM("2", i == 2);
	i = rotateLeft(i, 1);
	ASSERTM("4", i == 4);
	i = rotateLeft(i, 1);
	ASSERTM("8", i == 8);
	i = rotateLeft(i, 1);
	ASSERTM("16", i == 16);
	i = rotateLeft(i, 1);
	ASSERTM("32", i == 32);
	i = rotateLeft(i, 1);
	ASSERTM("64", i == 64);
	i = rotateLeft(i, 1);
	ASSERTM("128", i == 128);
	i = rotateLeft(i, 1);
	ASSERTM("256", i == 256);
	i = rotateLeft(i, 1);
	ASSERTM("512", i == 512);
	i = rotateLeft(i, 1);
	ASSERTM("1024", i == 1024);
	i = rotateLeft(i, 1);
	ASSERTM("2048", i == 2048);
	i = rotateLeft(i, 1);
	ASSERTM("4096", i == 4096);
	i = rotateLeft(i, 1);
	ASSERTM("8192", i == 8192);
	i = rotateLeft(i, 1);
	ASSERTM("16384", i == 16384);
	i = rotateLeft(i, 1);
	ASSERTM("32768", i == 32768);
	i = rotateLeft(i, 1);
	ASSERTM("65536", i == 65536);
	i = rotateLeft(i, 1);
	ASSERTM("131072", i == 131072);
	i = rotateLeft(i, 1);
	ASSERTM("262144", i == 262144);
	i = rotateLeft(i, 1);
	ASSERTM("524288", i == 524288);
	i = rotateLeft(i, 1);
	ASSERTM("1048576", i == 1048576);
	i = rotateLeft(i, 1);
	ASSERTM("2097152", i == 2097152);
	i = rotateLeft(i, 1);
	ASSERTM("4194304", i == 4194304);
	i = rotateLeft(i, 1);
	ASSERTM("8388608", i == 8388608);
	i = rotateLeft(i, 1);
	ASSERTM("16777216", i == 16777216);
	i = rotateLeft(i, 1);
	ASSERTM("33554432", i == 33554432);
	i = rotateLeft(i, 1);
	ASSERTM("67108864", i == 67108864);
	i = rotateLeft(i, 1);
	ASSERTM("134217728", i == 134217728);
	i = rotateLeft(i, 1);
	ASSERTM("268435456", i == 268435456);
	i = rotateLeft(i, 1);
	ASSERTM("536870912", i == 536870912);
	i = rotateLeft(i, 1);
	ASSERTM("1073741824", i == 1073741824);
	i = rotateLeft(i, 1);
	ASSERTM("2147483648", i == 2147483648);
	i = rotateLeft(i, 1);
	ASSERTM("loop", i == 1);
	i = rotateLeft(i, 1);
	ASSERTM("beeExtended b", i == 2);
	i = rotateLeft(i, 1);
	ASSERTM("beeExtended b", i == 4);
}

void ObjectFormatTest::testObjectFlagManipulation()
{
	oop_t *object = mockedObjects.newArray(1);

	ASSERTM("check reserved1", !object->testFlags(basic_header_t::Flag_isEphemeron));
	object->setFlags(basic_header_t::Flag_isEphemeron);
	ASSERTM("set reserved1", object->testFlags(basic_header_t::Flag_isEphemeron));
	object->unsetFlags(basic_header_t::Flag_isEphemeron);
	ASSERTM("unset reserved1", !object->testFlags(basic_header_t::Flag_isEphemeron));

	object->setFlags(basic_header_t::Flag_isBytes);
	ASSERTM("_isBytes", object->_isBytes());

	object->_beExtended();
	ASSERTM("_isExtended", object->_isExtended());
	object->unsetFlags(basic_header_t::Flag_isExtended);
	ASSERTM("_isExtended", !object->_isExtended());

	object->setFlags(basic_header_t::Flag_zeroTermOrNamed);
	ASSERTM("_isZeroTerminated", object->_isZeroTerminated());


	oop_t *ephemeron = mockedObjects.newEphemeron(object, object);
	ASSERTM("Ephemeron", ephemeron->_isActiveEphemeron());

	oop_t *weakArray = mockedObjects.newWeakArray(1);
	ASSERTM("should not be Ephemeron", !weakArray->_isEphemeron());
	ASSERTM("but should weak", weakArray->_hasWeaks());

}

void ObjectFormatTest::testVirtualBehavior()
{
	ulong reservedSize = 4 * 1024 * 100;
	ulong maxValueQuery = 28;
	PMEMORY_BASIC_INFORMATION queryAnswer = (PMEMORY_BASIC_INFORMATION)malloc(maxValueQuery);

	ulong *base = NULL;
	VirtualAlloc(base, reservedSize, MEM_COMMIT, PAGE_READWRITE);

	VirtualQuery((void *) base, queryAnswer, maxValueQuery);
	ASSERTM("Memory not commit", queryAnswer->State && MEM_COMMIT == MEM_COMMIT);
	VirtualFree((void *) base, (ulong) reservedSize, MEM_DECOMMIT);
	//VirtualQuery((void *) base, queryAnswer, maxValueQuery);
	//ASSERTM("Memory decommit fail",
	//	(queryAnswer->State && MEM_DECOMMIT) == MEM_DECOMMIT);
	free(queryAnswer);
}

void ObjectFormatTest::testProxying()
{
	//GenerationalGC * flipper = new GenerationalGC();
	//Memory::current()->setGC(flipper);
	//GCSpace local = GCSpace::dynamicNew(1024 * 1024 * 4 * 6);
	//flipper->localSpace = local;
	//flipper->initLocals();
	//flipper->initNonLocals();
	//oop_t *object = flipper->fromSpace.shallowCopy(mockedObjects.newArray(1024));
	//oop_t *copy = flipper->copyTo(object, flipper->toSpace);
	//cerr << object[-2] << endl;
	//cerr << rotateLeft(object[-2],8) << endl;
	//cerr << (ulong) copy << endl;
	//cerr << rotateRight((ulong)copy, 8) << endl;	
}

void ObjectFormatTest::testMulti()
{
	//GenerationalGC * flipper = new GenerationalGC();
	//Memory::current()->setGC(flipper);
	//Memory::current()->releaseEverything();
	//mockedObjects.mockVMValue();
	//flipper = new GenerationalGC();
	//Memory::current()->setGC(flipper);
}

cute::suite make_suite_VMMemoryTest()
{
	cute::suite s;
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testHeaderOf));
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testBasicSize));
//  s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testBeExtended));
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testRotateLeft));
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testObjectFlagManipulation));
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testVirtualBehavior));
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testProxying));
//	s.push_back(CUTE_SMEMFUN(ObjectFormatTest, testMulti));
	return s;
}

