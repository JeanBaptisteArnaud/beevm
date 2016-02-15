
#ifndef _GCFLIPTEST_H_
#define _GCFLIPTEST_H_

#include "GCTest.h"
#include "MockedObjects.h"

namespace Bee
{

class Memory;
class GenerationalGC;
class GCSpace;

class GCFlipTest : public GCTest
{
public:
	GCFlipTest();
	~GCFlipTest();

	void testGCCollect();
	void testCopyToFlip();
	void testCopyToOld();
	void testCopyToOldBug();
	void testEphemeron();
	void testEphemeronOnce();
	void testFollowObjectAndCheckGraph();
	void testFollowObjectCheckGraphAndOop();
	void testPurgeLiteralsWithNewObject();
	void testFollowObject();
	void testGCReferencesAfterCollect();
	void testTombstone();

	void setUp();
	void tearDown();

	GenerationalGC* flipper();
	GCSpace* fromSpace();
	GCSpace* toSpace();
	GCSpace* oldSpace();
	Memory *memory;
};

}

#endif // ~ _GCFLIPTEST_H_
