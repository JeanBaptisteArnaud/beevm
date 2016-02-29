

#ifndef _OBJECTFORMATTEST_H_
#define _OBJECTFORMATTEST_H_

#include "GCTest.h"

namespace Bee
{

class ObjectFormatTest : public GCTest
{
public:

	void setUp();
	void tearDown();

	void testHeaderOf();
	void testBasicSize();
	void testBeExtended();
	void testRotateLeft();
	void testObjectFlagManipulation();
	void testVirtualBehavior();
	void testProxying();
	void testNextObject();
};

}

#endif // ~ _OBJECTFORMATTEST_H_
