#ifndef _MOCKEDOBJECTS_H_
#define _MOCKEDOBJECTS_H_

#include <vector>
#include <string>
#include <map>

#include "DataStructures/Bee.h"
#include "DataStructures/GCSpace.h"
#include "DataStructures/KnownObjects.h"

namespace Bee
{

class MockedObjects
{
public:
	MockedObjects();
	void initializeKnownObjects();

	// globals
	oop_t* get(const std::string &name);
	void define   (const std::string &name, oop_t *value);
	void reference(const std::string &name, slot_t *slot);

	// creation
	oop_t* mockBasicObject(const std::string &classname, uchar size, ushort hash, uchar flags, Bee::GCSpace &space);
	oop_t* mockBasicObject(const std::string &classname, uchar size, ushort hash, uchar flags);
	
	oop_t* mockExtendedObject(const std::string &classname, ulong size, ushort hash, uchar flags, Bee::GCSpace &space);
	oop_t* mockExtendedObject(const std::string &classname, ulong size, ushort hash, uchar flags);

	void mockNil();
	void mockTrue();
	void mockFalse();


	// helpers for tests
	oop_t* mockArray(ulong slots);
	oop_t* mockWeakArray();
	bool   checkValueMockArray1024(oop_t*  localArray);
	void   mockVMValue();
	void   freeSimpleObject (oop_t *object);
	void   freeComplexObject(oop_t *object);
	oop_t* mockEphemeronFrom(oop_t *key, oop_t *value);
	oop_t* mockObjectFrom();
	bool   checkMockArray2(oop_t *object);

	HostVMVariables vm;

	Bee::GCSpace defaultSpace;

	std::vector<std::pair<slot_t*,std::string> > references;
	std::map<std::string, oop_t*> definitions;
};

}

#endif // ~ _MOCKEDOBJECTS_H_