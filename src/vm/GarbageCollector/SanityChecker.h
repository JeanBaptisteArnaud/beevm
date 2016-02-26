
#ifndef _SANITYCHECKER_H_
#define _SANITYCHECKER_H_

#include <vector>

#include "../DataStructures/Bee.h"
#include "../DataStructures/ObjectFormat.h" // remove when moving ObjectBitMap class
#include "../DataStructures/GCSpace.h"  // remove when moving ObjectBitMap class

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

class SanityChecker
{
public:
	SanityChecker(GCSpace &aSpace);

	void scan();
	void checkSaneFollowing(oop_t *roots);
	void checkAllSaneIn(GCSpace &space);
	void checkSaneSlotsOf(oop_t *object);
	void error(const char *msg);
	void errorIn(oop_t *object, long index);

	GCSpace space;
	ObjectBitMap spaceMap;
};

}

#endif  // ~ _SANITYCHECKER_H_

