
#ifndef _SANITYCHECKER_H_
#define _SANITYCHECKER_H_

#include <vector>
#include <map>

#include "../DataStructures/Bee.h"
#include "../DataStructures/ObjectFormat.h" // remove when moving ObjectBitMap class
#include "../DataStructures/GCSpace.h"  // remove when moving ObjectBitMap class
#include "../DataStructures/Memory.h" 

namespace Bee
{

//class GCSpace;

class ObjectBitMap
{
public:
	ObjectBitMap(GCSpace &aSpace) { space.loadFrom(aSpace); allocate(); }

	void allocate() { size = objectToIndex(asObject(space.nextFree)); oops.resize(size, false); }; 

	void add(oop_t *object) { size_t index = objectToIndex(object); oops[index] = true; }
	bool isOutsideOrObject(oop_t *object)
	{
		if (object->isSmallInteger())
			return true;

		int index = objectToIndex(object);
		if (index < 0 || index >= (int)size) 
			return true; 

		return oops[index];
	}

protected:
	size_t objectToIndex(oop_t *object) { return ((ulong)object - (ulong)space.base) >> 2; }

	GCSpace space;
	std::vector<bool> oops;
	size_t size;
};

class Checker
{
public:
	void error(const char *msg);
	void errorIn(oop_t *object, long index);
};

class SanityChecker : public Checker
{
public:
	SanityChecker(GCSpace &aSpace);

	void scan();
	void checkSaneFollowing(oop_t *roots);
	void checkAllSaneIn(GCSpace &space);
	void checkSaneSlotsOf(oop_t *object);


	GCSpace space;
	ObjectBitMap spaceMap;
};

class EqualChecker : public Checker
{
public:

	void checkHeaderBits(oop_t *newObject, oop_t *oldCopy);
	void checkSlotsOf(oop_t *newObject, oop_t *oldCopy);
	void checkAllEqual(Memory *aMemory, GCSpace *space);

	void newPosition(oop_t *oldObject, oop_t *newObject);

	void reset();
	void copyMemory(Memory *aMemory, GCSpace *space);


	void errorInSlot(oop_t *object, long index, oop_t *oldCopy, oop_t *newSlot, oop_t *oldSlot);

	std::map<oop_t*, oop_t*> newToOld, oldToNew;
	uchar *copy;
	int copyDelta;
	Memory *memory;
	GCSpace *old;

	ulong *ccStart, *ccEnd;
	static EqualChecker current;
};

}

#endif  // ~ _SANITYCHECKER_H_

