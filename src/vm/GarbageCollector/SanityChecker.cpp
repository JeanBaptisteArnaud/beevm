
#include <cstdio>
#include <sstream>

#include "SanityChecker.h"
#include "../DataStructures/GCSpace.h"

using namespace Bee;
using namespace std;

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

EqualChecker EqualChecker::current;

void EqualChecker::checkHeaderBits(oop_t *newObject, oop_t *oldCopy)
{
	oop_t **oldHeader = &oldCopy->slot(-2);
	oop_t **newHeader = &newObject->slot(-2);

	ulong mutableFlags = (basic_header_t::Flag_isInRememberedSet | basic_header_t::Flag_isEphemeron) << 24;

	if (((ulong)*oldHeader & ~mutableFlags) != ((ulong)*newHeader & ~mutableFlags))
			errorInSlot(newObject, -2, oldCopy, *newHeader, *oldHeader);

	if (oldCopy->_isExtended())
	{
		oop_t **oldSize = &oldCopy->slot(-3);
		oop_t **newSize = &newObject->slot(-3);
		if (*oldSize != *newSize)
			errorInSlot(newObject, -2, oldCopy, *newSize, *oldSize);

		oop_t **oldHeaderExt = &oldCopy->slot(-4);
		oop_t **newHeaderExt = &newObject->slot(-4);

		if (((ulong)*oldHeaderExt & ~mutableFlags) != ((ulong)*newHeaderExt & ~mutableFlags))
			errorInSlot(newObject, -4, oldCopy, *newHeaderExt, *oldHeaderExt);
	}
}

void EqualChecker::checkSlotsOf(oop_t *newObject, oop_t *oldCopy)
{
	int size = oldCopy->_pointersSize();

	for (int i = -1; i < size - 1; i++)
	{
		oop_t *slotNew = newObject->slot(i);
		oop_t *slotOld = oldCopy->slot(i);
		oop_t *other = old->includes(slotNew) ? newToOld[slotNew] : slotNew;
		if (slotOld != other)
		{
			if (ccStart <= (ulong*)slotOld && (ulong*)slotOld <= ccEnd) // pointers from CMs to code cache are nilled
				continue;
			if (slotNew == memory->residueObject && newObject->_hasWeaks())
				continue;

			errorInSlot(newObject, i, oldCopy, slotNew, slotOld);
		}
	}
}

void EqualChecker::checkAllEqual(Memory *aMemory, GCSpace *space)
{
	memory = aMemory;
	old = space;
	oop_t *next = old->firstObject();
	while ((ulong *)next <= old->getNextFree())
	{
		oop_t *following = next->nextObject();

		ulong oldAddress = (ulong)newToOld[next];
		if (oldAddress != NULL)
		{
			oop_t *oldCopy  = (oop_t*)(oldAddress + copyDelta);
			checkHeaderBits(next, oldCopy);
			checkSlotsOf(next, oldCopy);
		}
		next = following;
	}

}

void EqualChecker::newPosition(oop_t *oldObject, oop_t *newObject)
{
	newToOld[newObject] = oldObject;
	oldToNew[oldObject] = newObject;

	//if ((ulong)oldObject < (ulong)old->base + 0x1000 || (ulong)newObject < (ulong)old->base + 0x1000)
	//{
	//	char message[200];
	//	sprintf_s(message, "new object 0x%p points to old 0x%p", newObject, oldObject);
	//	debug(message);
	//}
}

ulong **codeCacheStart = (ulong**)0x1003FDA0;
ulong *codeCacheMaxSize = (ulong*)0x1003FDA8;

void EqualChecker::reset()
{
	ccStart = *codeCacheStart;
	ccEnd   = (ulong*)((ulong)ccStart + *codeCacheMaxSize);
	newToOld.clear();
}

void EqualChecker::copyMemory(Memory *aMemory, GCSpace *space)
{
	memory = aMemory;
	old = space;

	GCSpace *from = memory->fromSpace;
	GCSpace *to   = memory->toSpace;

	ulong size = from->reservedSize() + to->reservedSize() + old->commitedSize();

	if (copy == NULL)
		copy = (uchar*)_commit(NULL, size);

	ulong base = from->base < to->base ? asUObject(from->base) : asUObject(to->base);
	copyDelta = (ulong)copy - (ulong)base;

	
	ulong baseFrom = asUObject(from->base);
	ulong sizeFrom = from->commitedSize();
	ulong copyFrom = baseFrom + copyDelta;

	ulong baseOld = asUObject(old->base);
	ulong copyOld = baseOld + copyDelta;


	memcpy((void*)copyFrom, (void*)baseFrom, sizeFrom);
//	memcpy(copy2, (ulong*)base2, size2);
	memcpy((void*)copyOld, (void*)baseOld, old->commitedSize());

	
	ostringstream message;
	message << "distance from base to copy is 0x" << hex << copyDelta;
	message << "\ncopy first is 0x" << hex << (ulong)copy << " | copy from is 0x" << hex << (ulong)copyFrom;
	message << "\ncopy old is 0x" << hex << (ulong)copyOld;
	debug(message.str().c_str());
}


void Checker::error(const char *msg)
{
	Bee::error(msg);
}

void Checker::errorIn(oop_t *object, long index)
{
	char msg[200];
#ifdef _WIN32
	sprintf_s(msg, "wrong object 0x%p at slot %d (0x%p)", object, index, &object->slot(index));
#else
	sprintf(msg, "wrong object 0x%p at slot %d (0x%p)", object, (int)index, &object->slot(index));
#endif
	error(msg);
}

void EqualChecker::errorInSlot(oop_t *object, long index, oop_t *oldCopy, oop_t *newSlot, oop_t *oldSlot)
{
	char msg[200];
#ifdef _WIN32
	sprintf_s(msg, "wrong object 0x%p at slot %d (0x%p)\nCopy (0x%p) pointed to 0x%p which means 0x%p", 
		  object, index, newSlot, oldCopy, oldSlot, oldToNew[oldSlot]);
#else
	sprintf(msg, "wrong object 0x%p at slot %d (0x%p)", object, (int)index, &object->slot(index));
#endif
	error(msg);
}
