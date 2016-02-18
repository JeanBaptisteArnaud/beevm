#include <cute/cute.h>
#include <cute/cute_suite.h>

#include <DataStructures/GCSpaceInfo.h>

extern cute::suite make_suite_GCSpaceInfoTest();

using namespace Bee;
using namespace std;

ulong smallPointerMaxValue = 0x80000000;

GCSpaceInfo mockSpaceForTest()
{
	return GCSpaceInfo::newSized(100);
}

void atTests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.atPut(0, (unsigned long *) value);
		if (mock.at(0) != (unsigned long *) value)
		{
			ostringstream ostr;
			ostr << "v:";
			ostr << value;
			ostr << " readV:";
			ostr << mock.at(0);
			FAILM(ostr.str());
		}

		//ASSERTM("at", mock.at(0) == value);
	}
}

void base_3Tests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.setBase_3((unsigned long *) value);
		ostringstream ostr;
		ostr << "commitedLimit v:";
		ostr << value;
		ostr << " readV:";
		ostr << mock.getBase_3();
		ASSERTM(ostr.str(), (ulong)mock.getBase_3() == value);
	}
}

void commitedLimitTests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.setCommitedLimit((unsigned long *)value);
		ostringstream ostr;
		ostr << "commitedLimit v:";
		ostr << value;
		ostr << " readV:";
		ostr << mock.getCommitedLimit();
		ASSERTM(ostr.str(), (ulong)mock.getCommitedLimit() == value);
	}
}

void nextFreeTests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.setNextFree((unsigned long *)value);
		ostringstream ostr;
		ostr << "nextFree v:";
		ostr << value;
		ostr << " readV:";
		ostr << mock.getNextFree();
		ASSERTM(ostr.str(), (ulong)mock.getNextFree() == value);
	}
}

void reservedLimitTests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.setReservedLimit((unsigned long *)value);
		ostringstream ostr;
		ostr << "reservedLimit v:";
		ostr << value;
		ostr << " readV:";
		ostr << mock.getReservedLimit();
		ASSERTM(ostr.str(), (ulong)mock.getReservedLimit() == value);
	}
}

void softLimitTests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.setSoftLimit((unsigned long *)value);
		ostringstream ostr;
		ostr << "softLimit v:";
		ostr << value;
		ostr << " readV:";
		ostr << mock.getSoftLimit();
		ASSERTM(ostr.str(), (ulong)mock.getSoftLimit() == value);
	}
}

void baseTests()
{
	GCSpaceInfo mock = mockSpaceForTest();
	ASSERTM("size", mock.sizeInBytes == 24);
	for (unsigned long value = 0; value < smallPointerMaxValue; value++)
	{
		mock.setBase((unsigned long *)value);
		ostringstream ostr;
		ostr << "base v:";
		ostr << value;
		ostr << " readV:";
		ostr << mock.getBase();
		ASSERTM(ostr.str(), (ulong)mock.getBase() == value);
	}
}

cute::suite make_suite_GCSpaceInfoTest()
{
	cute::suite s;
	//Long time so just test it one by one
	//s.push_back(CUTE(atTests));
	//s.push_back(CUTE(baseTests));
	//s.push_back(CUTE(softLimitTests));
	//s.push_back(CUTE(reservedLimitTests));
	//s.push_back(CUTE(commitedLimitTests));
	//s.push_back(CUTE(nextFreeTests));
	//s.push_back(CUTE(base_3Tests));
	return s;
}

