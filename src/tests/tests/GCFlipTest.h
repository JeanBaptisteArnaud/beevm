
#ifndef _GCFLIPTEST_H_
#define _GCFLIPTEST_H_

#include "GCTest.h"
#include "MockedObjects.h"

namespace Bee
{

class GenerationalGC;

class GCFlipTest : public GCTest
{
public:
	void testGCCollect();
	void testCopyToFlip();
	void testCopyToOld();
	void testCopyToOldBee();
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

	GenerationalGC *flipper;
};

}

#endif // ~ _GCFLIPTEST_H_
