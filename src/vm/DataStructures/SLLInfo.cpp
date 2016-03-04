#include "SLLInfo.h"
#include "ObjectFormat.h"

using namespace Bee;

SLLInfo::SLLInfo()
{
}


SLLInfo::~SLLInfo()
{
}

oop_t* SLLInfo::firstObject()
{
	return headerToObject(asObject(this->getBase()));
}

ulong* SLLInfo::getBase()
{
	return (ulong*)pointerConst(contents[13]);
}

ulong* SLLInfo::getNextFree()
{
	return (ulong*)this->end();
}

ulong* SLLInfo::end()
{
	return (ulong*)pointerConst(contents[14]);
}

bool SLLInfo::isBelowNextFree(oop_t *object)
{
	return  (ulong *) object <= this->getNextFree();
}