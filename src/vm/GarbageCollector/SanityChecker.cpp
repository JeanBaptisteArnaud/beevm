
#include <cstdio>

#include "SanityChecker.h"
#include "../DataStructures/GCSpace.h"

using namespace Bee;

SanityChecker::SanityChecker(GCSpace &aSpace) :
	space(aSpace), spaceMap(aSpace)
{
	scan();
}

void SanityChecker::scan()
{
	oop_t *object = space.firstObject();

	while (space.isBelowNextFree(object))
	{
		if (object->_isExtended() && object->basic_header()->size != 4)
			this->error("wrong extended header size");

		spaceMap.add(object);
		object = object->nextObject();
	}
}

void SanityChecker::checkSaneFollowing(oop_t *roots)
{
}

void SanityChecker::checkAllSaneIn(GCSpace &space)
{
	oop_t *object = space.firstObject();

	while (space.isBelowNextFree(object))
	{
		this->checkSaneSlotsOf(object);
		object = object->nextObject();
	}
}

void SanityChecker::checkSaneSlotsOf(oop_t *object)
{
	if (object->_isExtended() && object->basic_header()->size != 4)
		this->error("wrong referrer extended header size");

	for (int i = -1; i < (int)object->_pointersSize(); i++)
	{
		if (!spaceMap.isOutsideOrObject(object->slot(i)))
			this->errorIn(object, i);
	}
}


void SanityChecker::error(const char *msg)
{
	Bee::error(msg);
}

void SanityChecker::errorIn(oop_t *object, long index)
{
	char msg[200];
#ifdef WIN32
	sprintf_s(msg, "wrong object 0x%p at slot %d (0x%p)", object, index, &object->slot(index));
#else
	sprintf(msg, "wrong object 0x%p at slot %d (0x%p)", object, (int)index, &object->slot(index));
#endif
	error(msg);
}
