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

	ulong* stackPtr();
	void addStackVariable(oop_t **value);

	// globals
	oop_t* get(const std::string &name);
	void define   (const std::string &name, oop_t *value);
	void reference(const std::string &name, slot_t *slot);

	// creation
	oop_t* newBasicObject(const std::string &classname, uchar size, ushort hash, uchar flags, Bee::GCSpace &space);
	oop_t* newBasicObject(const std::string &classname, uchar size, ushort hash, uchar flags);
	
	oop_t* newExtendedObject(const std::string &classname, ulong size, ushort hash, uchar flags, Bee::GCSpace &space);
	oop_t* newExtendedObject(const std::string &classname, ulong size, ushort hash, uchar flags);

	oop_t* newObject(const std::string &classname, ulong size, ushort hash, uchar flags, GCSpace &space);

	oop_t*  mockNil();
	oop_t*  mockTrue();
	oop_t*  mockFalse();
	Memory* mockMemory();
	GCSpace* mockGCSpace(ulong size);

	// helpers for tests
	oop_t* newObject();
	oop_t* newArray(ulong slots);
	oop_t* newArray(ulong slots, GCSpace *space);
	oop_t* newByteArray(ulong slots);
	oop_t* newString(const char *value);
	oop_t* newWeakArray(ulong slots);
	oop_t* newEphemeron(oop_t *key, oop_t *value);

	GCSpace setDefaultSpace(GCSpace *newSpace);

	void   freeSimpleObject (oop_t *object);
	void   freeComplexObject(oop_t *object);

	bool   checknewArray2(oop_t *object);

	Bee::GCSpace defaultSpace;
	std::vector<std::pair<slot_t*,std::string> > references;
	std::map<std::string, oop_t*> definitions;

	std::vector<ulong> stack;
	int stackVars;

};

}

#endif // ~ _MOCKEDOBJECTS_H_