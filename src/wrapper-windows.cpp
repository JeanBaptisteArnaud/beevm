
#include <iostream>
#include <windows.h>

#include "vm/DataStructures/Bee.h"

using namespace std;
using namespace Bee;





ulong* Bee::_commit(ulong limit, ulong delta)
{
	return (ulong *) VirtualAlloc((ulong *) limit, delta, MEM_COMMIT, PAGE_READWRITE);
}

ulong* Bee::_reserve(ulong limit, ulong delta)
{
	return (ulong *) VirtualAlloc((ulong *) limit, delta, MEM_RESERVE, PAGE_READWRITE);
}

void Bee::_decommit(ulong *limit, ulong *delta)
{
	VirtualFree((void *) limit, (ulong) delta, MEM_DECOMMIT);
}

void Bee::_free(ulong * limit, ulong *delta)
{
	VirtualFree((void *) limit, (ulong) delta, MEM_RELEASE);
}



void Bee::osError()
{
	const int buffer_length = 256;
	char buffer[buffer_length];

	ulong code = GetLastError();
	getOsErrorMessage(code, buffer, buffer_length);
	cout << "Error " << code << " " << buffer << endl;
	system("pause");
	ExitProcess(-1);
}


void Bee::getOsErrorMessage(ulong code, char buffer[], int buffer_length)
{
	int count = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, buffer, buffer_length, 0);

}


void Bee::error(const char *message)
{
	debug(message);
	ulong *bad = (ulong*)0;
	*bad;
	//ExitProcess(-1);
}

void Bee::debug(const char *message)
{
	cout << message << endl;
	OutputDebugString(message);
}

/*
void testVirtualBehavior()
{
	ulong reservedSize = 4 * 1024 * 100;
	ulong maxValueQuery = 28;
	PMEMORY_BASIC_INFORMATION queryAnswer = (PMEMORY_BASIC_INFORMATION)malloc(maxValueQuery);

	ulong *base = NULL;
	VirtualAlloc(base, reservedSize, MEM_COMMIT, PAGE_READWRITE);

	VirtualQuery((void *) base, queryAnswer, maxValueQuery);
	ASSERTM("Memory not commit", queryAnswer->State && MEM_COMMIT == MEM_COMMIT);
	VirtualFree((void *) base, (ulong) reservedSize, MEM_DECOMMIT);
	free(queryAnswer);
}*/


