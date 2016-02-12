/*
 * GCSpaceTest.h
 *
 *  Created on: 29 janv. 2016
 *      Author: jbapt
 */

#ifndef GCSPACETEST_H_
#define GCSPACETEST_H_

#include "GCTest.h"

#include "GarbageCollector/GenerationalGC.h"
#include "DataStructures/GCSpace.h"

namespace Bee
{

class GCSpaceTest : public GCTest
{
protected:
	void setUp();
	void tearDown();

public:
	GCSpaceTest();
	~GCSpaceTest();

	GenerationalGC * flipper;
	void testAllocate();
	void testGCSpace();
	void testExtendedGrowingTo();
	void testGrow();
	void testGrowingTo();
	void testNewGCSpaceShallowCopy();
	void testShallowCopy();
	void testShallowCopyBytes();
	void testShallowCopyBytes2();
	void testShallowCopyBytes3();
	void testShallowCopyBytesExtended();
	void testShallowCopyExtended();
	void testSynchronousGCSpace();

};

}

#endif /* GCSPACETEST_H_ */
