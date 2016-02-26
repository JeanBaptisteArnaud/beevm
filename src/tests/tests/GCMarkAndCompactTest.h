#pragma once

#ifndef _GCMARKANDCOMPACTTEST_H_
#define _GCMARKANDCOMPACTTEST_H_

#include "GCTest.h"
#include "MockedObjects.h"

namespace Bee
{

	class Memory;
	class MarkAndCompactGC;
	class GCSpace;

	class GCMarkAndCompactTest : public GCTest
	{
	public:
		GCMarkAndCompactTest();
		~GCMarkAndCompactTest();

		void setUp();
		void tearDown();
		void testCompact();
		void testCompactExtended();

		MarkAndCompactGC * compactor();

		Memory *memory;
	};

}
#endif // ~ _GCMARKANDCOMPACTTEST_H_
