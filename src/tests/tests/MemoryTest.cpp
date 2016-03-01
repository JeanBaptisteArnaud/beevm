
#include <cstdlib>
#include <iostream>
#include <cute/cute.h>
#include <cute/cute_suite.h>

#include "DataStructures/Memory.h"

extern cute::suite make_suite_MemoryTest();

using namespace std;
using namespace Bee;


ulong* VM()
{
	
	ulong  size    = 0x42000;
	ulong* address = Bee::_commit( 0x10000000, size);
	
	if (!address)
		osError();

	GCSpaceInfo info = GCSpaceInfo((ulong)address, size);
	
	GCSpace *space   = new GCSpace();
	space->loadFrom(info);
	return address;
}

void singleton()
{
	//Memory * m = Memory::current();
	//Memory * m2 = Memory::current();
	//m->createNextSpace();
	//ASSERTM("Not a singleton",
	//		(ulong ) m2->nextSpace->getBase()
	//				== (ulong ) m->nextSpace->getBase());
}

void mooooooooore()
{
	//Memory * m = Memory::current();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->acquireMoreSpace();
	//m->releaseEverything();
}

void release()
{
	//Memory * m = Memory::current();
	//ulong * address = m->VM();
	//Memory::current()->releaseEverything();
	//address = (Memory::current())->VM();
	//Memory::current()->releaseEverything();
	//address = (Memory::current())->VM();
	//Memory::current()->releaseEverything();
	//address = (Memory::current())->VM();
	//Memory::current()->releaseEverything();
	//address = Memory::current()->VM();
	//Memory::current()->releaseEverything();
}


cute::suite make_suite_MemoryTest()
{
	cute::suite s;
//	s.push_back(CUTE(singleton));
//	s.push_back(CUTE(mooooooooore));
	s.push_back(CUTE(release));
	//s.push_back(CUTE(rotateLeftTest));
	//s.push_back(CUTE(objectFlagManipulation));
	//s.push_back(CUTE(virtualBehavior));
	//s.push_back(CUTE(newGCSpaceShallowCopy));
	//s.push_back(CUTE(basicAt));
	//s.push_back(CUTE(shallowCopyBytes));
	//s.push_back(CUTE(shallowCopyBytes2));
	//s.push_back(CUTE(shallowCopyBytes3));
	//s.push_back(CUTE(shallowCopyBytesExtended));
	//s.push_back(CUTE(shallowCopyExtended));
	//s.push_back(CUTE(synchronousGCSpace));
	return s;
}

