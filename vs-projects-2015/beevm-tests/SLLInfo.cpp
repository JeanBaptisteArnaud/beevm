#include "SLLInfo.h"


using namespace Bee;

SLLInfo::SLLInfo()
{
}


SLLInfo::~SLLInfo()
{
}

ulong * SLLInfo::getBase() {
	return (ulong *) contents->slot(13)->_asPointer();
}

ulong * SLLInfo::getNextFree() {
	return this->end();
}

oop_t * SLLInfo::end() {
	return (ulong *) contents->slot(14)->_asPointer();
}

