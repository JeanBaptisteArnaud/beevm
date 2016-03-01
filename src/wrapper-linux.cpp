
#include <iostream>
#include <cstring>
#include <sys/mman.h>

#include "DataStructures/Bee.h"

using namespace std;
using namespace Bee;



ulong* Bee::_commit(ulong limit, ulong delta)
{
	ulong* result = (ulong *) mmap((void*)limit, delta, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	return result;
}

void Bee::_decommit(ulong *limit, ulong *delta)
{
	//munmap((void *) limit, (ulong) delta);
}

void Bee::_free(ulong * limit, ulong *delta)
{
	munmap((void *) limit, (ulong) delta);
}

char* strcpy_s(char* dst, int size, const char *src)
{
	return strcpy(dst, src);
}


void Bee::osError()
{
	cout << "Some Error "<< endl;
	system("pause");
	exit(-1);
}



void Bee::error(const char *message)
{
	debug(message);
	ulong *bad = (ulong*)0xbad;
	*bad;
	exit(-1);
}

void Bee::debug(const char *message)
{
	cout << message << endl;
	cerr << message << endl;
}
