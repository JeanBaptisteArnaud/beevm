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
		// compact family
		void testCompact();
		void testCompactExtended();
		void testCompactExtendedWEphemeron();
		void testCompactExtendedWEphemeronRescued();
		void testCompactOverlapping();
		void testFollowDontRescueEphemerons();
		void testFollowEphemeronsNoRescue();
		void testFollowObjectInFrom();
		void testFollowObjectInTo();
		void testFollowObjectInOldWeakArray();
		void testFollowRescueEphemerons();
		void testFollowWeak();
		void testFollowWeakExtended();
		void testFollowWeakExtendedNested();
		void testFollowWeakNoTombstones();
		void testFollowWeakNoTombstonesExtended();
		void testHoldingNativizedMethods();
		void testIsNil();
		void testLibrariesAllObjectsDo();
		void testLibrariesDo();

		MarkAndCompactGC * compactor();
		GCSpace& oldSpace();

		Memory *memory;
	};

}
#endif // ~ _GCMARKANDCOMPACTTEST_H_
