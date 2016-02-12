
#include "GCTest.h"

#include "DataStructures/GCSpaceInfo.h"
#include "DataStructures/ReferencedVMArray.h"
#include "DataStructures/Memory.h"
#include "DataStructures/ObjectFormat.h"


using namespace Bee;

oop_t* GCTest::global(const std::string &name)
{
	return mockedObjects.get(name);
}

bool GCTest::isArray(oop_t *object)
{
	return object->behavior() == global("Array behavior");
}


bool GCTest::isSameArray(oop_t *object, oop_t *anotherObject)
{
	ulong sizeA = object->_size();
	ulong sizeB = anotherObject->_size();
	if (sizeA != sizeB)
		return false;

	//if(object[-2] != anotherObject[-2]) return false; Flags will be different

	if (object->behavior() != anotherObject->behavior())
		return false;

	if (object->_isExtended() && anotherObject->_isExtended())
	{
		//if(object[-4] != anotherObject[-4]) return false; Flags will be different
		if (object->extended_header()->size != anotherObject->extended_header()->size)
			return false;
	}

	for (ulong index = 0; index < sizeA; index++)
	{
		if (object->slot(index) != anotherObject->slot(index))
			return false;
	}
	return true;
}
