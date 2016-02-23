#ifndef _VMARRAY_TEST_H_
#define _VMARRAY_TEST_H_

#include "GCTest.h"

namespace Bee
{

class VMArrayTest : public GCTest
{
public:
	VMArrayTest();
	~VMArrayTest();
	void setUp();
	void tearDown();

	void testAdd();
	void testAddAll();
	void testDo();
	void testGrow();
	void testGrow2();
	void testPop();
	void testReset();
	void testUpdateReference();

};

}

#endif // ~ _VMARRAY_TEST_H_