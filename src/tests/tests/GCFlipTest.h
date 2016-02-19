
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

	void setUp();
	void tearDown();

	GenerationalGC* flipper();
	GCSpace* fromSpace();
	GCSpace* toSpace();
	GCSpace* oldSpace();

	void testGCCollect();
	void testCopyToFlip();
	void testCopyToOld();
	void testCopyToOldBug();
	void testEphemeron();
	void testEphemeronOnce();
	void testFollowObject();
	void testFollowObjectAndCheckGraph();
	void testFollowObjectCheckGraphAndOop();
	void testGCReferencesAfterCollect();
	void testPurgeEmptyRoots();
	void testPurgeLiteralsWithNewObject();
	void testPurgeLiteralsWithOldObject();

	void testRescuedEphemeron();
	void testRescuedEphemeronNoRescuedByValue();
	void testRescueEphemeronRescuedInRoots();
	void testRescueNoEphemerons();

	void testStackCallbackTP19064();
	void testStackFollowObjectAndCheckGraph();
	void testStackFollowObjectCallbackHole();
	void testStackFollowObjectNestedBlock();

	void testTombstone();
	void testWeakContainer();
	void testWeakContainerExtended();

	Memory *memory;
};

}

#endif // ~ _GCFLIPTEST_H_
