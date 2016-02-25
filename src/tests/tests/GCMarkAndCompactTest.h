#pragma once

#ifndef _GCMARKANDCOMPACTTEST_H_
#define _GCMARKANDCOMPACTTEST_H_

#include "GCTest.h"
#include "MockedObjects.h"

namespace Bee
{

	class Memory;
	class GenerationalGC;
	class GCSpace;

	class GCMarkAndCompactTest : public GCTest
	{
	public:
		GCMarkAndCompactTest();
		~GCMarkAndCompactTest();

		void setUp();
		void tearDown();

		MarkAndCompactGC* compactor();

		void testGCCollect();

		Memory *memory;
	};

}
#endif // ~ _GCMARKANDCOMPACTTEST_H_
