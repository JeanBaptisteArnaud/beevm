/*
 * GenerationalGC.cpp
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include <iostream>

#include "GenerationalGC.h"
#include "GarbageCollector.h"

#include "../DataStructures/ObjectFormat.h"
#include "../DataStructures/KnownObjects.h"
#include "../DataStructures/Memory.h"

using namespace std;
using namespace Bee;

GenerationalGC::GenerationalGC()
{
}

void GenerationalGC::initialize()
{
	this->initLocals();
	this->initNonLocals();
}

GenerationalGC::~GenerationalGC()
{
}

void GenerationalGC::collect()
{
	this->updateFromMemory();

	this->setUpLocals();
	this->setUpNonLocals();

	this->doCollect();

	this->updateToMemory();

	//Thinks we do not need that:: this->addInterrupt();
	//Thinks we do not need that:: this->saveSpaces();
}

void GenerationalGC::initLocals()
{
	//stack.setSpace(&localSpace);
	//unknowns.setSpace(&localSpace);
	//ephemerons.setSpace(&localSpace);
	//weakContainers.setSpace(&localSpace);
	stack.setSpace(&oldSpace);
	unknowns.setSpace(&oldSpace);
	ephemerons.setSpace(&oldSpace);
	weakContainers.setSpace(&oldSpace);

}


void GenerationalGC::setUpLocals()
{
	localSpace.reset();
	stack.emptyReserving(1024);
	unknowns.emptyReserving(1025);
	ephemerons.emptyReserving(1026);
	weakContainers.emptyReserving(1027);
}

void GenerationalGC::setUpNonLocals()
{
	rememberedSet       .updateFromReferer();
	literalsReferences  .updateFromReferer();
	nativizedMethods    .updateFromReferer();
	classCheckReferences.updateFromReferer();
	rescuedEphemerons   .updateFromReferer();
}

void GenerationalGC::doCollect()
{
	this->purgeLiteralsReferences();
	this->purgeRememberedSet();
	this->followCodeCacheReferences();
	this->followRoots();
	this->followStack();
	this->rescueEphemerons();
	this->makeRescuedEphemeronsNonWeak();
	this->fixWeakContainers();
	this->flipSpaces();
	this->fixReferencesFromNativeMethods();
	this->cleanRememberedSet();
}

bool GenerationalGC::arenaIncludes(oop_t *object)
{
	return this->fromSpace.includes(object);
}

void GenerationalGC::addRoot(oop_t *object)
{
	rememberedSet.add(object);
}

bool GenerationalGC::hasToPurge(oop_t *object)
{
	return 	this->arenaIncludes(object) || !fromSpace.isReferredBy(object);
}

void GenerationalGC::holdReferenceTo(oop_t* object)
{
	if (!object->_isInRememberedSet())
	{
		object->_beInRememberedSet();
		this->addRoot(object);
	}
}


oop_t* GenerationalGC::moveToOldOrTo(oop_t * object)
{
	if (object->_isSecondGeneration())
	{
		return this->moveToOldSpace(object);
	} else {
		return this->moveToToSpace(object);
	}

}

oop_t* GenerationalGC::moveToToSpace(oop_t *object)
{
	object->_beSecondGeneration();
	return this->copyTo(object, toSpace);
}



void GenerationalGC::someEphemeronsRescued()
{
	this->holdReferenceTo(rescuedEphemerons.contents);
}

void GenerationalGC::fixReferencesOrSetTombstone(oop_t * weakContainer)
{
	for (ulong index = 0; index < weakContainer->_size(); index++) // was index = 1
	{
		oop_t *instance = weakContainer->slot(index);
		if (this->arenaIncludes(instance))
		{
			if (instance->_isProxy())
			{
				weakContainer->slot(index) = instance->_getProxee();
			} else {
				weakContainer->slot(index) = residueObject;
			}
		}
	}
}

void GenerationalGC::purgeLiteralsReferences()
{
	long kept = 0;
	oop_t *offset;
	oop_t *literal;
	for (long index = 1; index <= literalsReferences.size()->_asNative(); index = index + 2)
	{
		literal = literalsReferences[index];
		if (this->arenaIncludes(literal))
		{
			kept = kept + 2;
			offset = literalsReferences[index + 1];
			literalsReferences[kept - 1] = literal;
			literalsReferences[kept] = offset;
		}
	}

	literalsReferences.size(smiConst(kept));
}

void GenerationalGC::purgeRememberedSet()
{
	long kept = 0;
	for (long index = 1; index <= rememberedSet.size()->_asNative(); index++)
	{
		oop_t *object = rememberedSet[index];
		rememberedSet[index] = KnownObjects::nil;
		if (this->hasToPurge(object))
		{
			object->_beNotInRememberedSet();
		} else {
			kept++;
			rememberedSet[kept] = object;
		}
	}

	rememberedSet.size(smiConst(kept));
}

void GenerationalGC::purgeRoots()
{
	this->purgeLiteralsReferences();
	this->purgeRememberedSet();
}

void GenerationalGC::followRoots()
{
	this->followRememberedSet();
}

void GenerationalGC::followRememberedSet()
{
	for (long index = 1; index <= rememberedSet.size()->_asNative(); index++)
	{
		this->follow(rememberedSet[index]);
	}
}

ulong * GenerationalGC::framePointerToStartWalkingTheStack()
{
	return vm.framePointerToStartWalkingTheStack();
}

oop_t** GenerationalGC::codeCacheReferenceAtOffset(ulong offset)
{
	return (oop_t**) ((ulong)vm.codeCache() + offset);
}

void GenerationalGC::moveClassCheckReferences()
{
	oop_t *moved;
	for (long index = 1; index <= classCheckReferences.size()->_asNative(); index++)
	{
		ulong offset = classCheckReferences[index]->_asNative();
		oop_t **reference = codeCacheReferenceAtOffset(offset);
		oop_t *object = *reference;
		if (this->arenaIncludes(object))
		{
			if (object->_isProxy())
			{
				moved = object->_getProxee();
			} else {
				moved = moveToOldSpace(object);
			}
			*reference = moved;
		}
	}

	vm.anyCompiledMethodInFromSpace(false);
}

void GenerationalGC::moveToOldAll(ReferencedVMArray &objects)
{
	oop_t *moved;
	for (long index = 1; index <= objects.size()->_asNative(); index++)
	{
		slot_t &object = objects[index];
		if (this->arenaIncludes(object))
		{
			if (object->_isProxy())
			{
				moved = object->_getProxee();
			} else {
				moved = this->moveToOldSpace(object);
			}
			// notice 1-based index
			objects[index] = moved;
		}
	}
}

oop_t* GenerationalGC::moveToOldSpace(oop_t *object)
{
	oop_t * copy = this->copyTo(object, oldSpace);
	this->holdReferenceTo(copy);
	return copy;
}

oop_t* GenerationalGC::copyTo(oop_t *object, GCSpace &to)
{
	oop_t *copy = to.shallowCopy(object);
	object->_setProxee(copy);
	return copy;
}

bool GenerationalGC::checkReachablePropertyOf(oop_t *ephemeron)
{
	return ephemeron->slot(0)->_isProxy() || !arenaIncludes(ephemeron->slot(0));
}

void GenerationalGC::followCodeCacheReferences()
{
	if (vm.globalCacheHasPointersToFrom())
	{
		vm.globalCacheHasPointersToFrom(false);
		this->clearPolymorphicMethodCache();
	}

	if (vm.anyCompiledMethodInFromSpace())
		this->moveClassCheckReferences();

	if (vm.anyNativizedCompiledMethodInFromSpace())
	{
		vm.anyNativizedCompiledMethodInFromSpace(false);
		this->moveToOldAll(nativizedMethods);
	}
	this->moveToOldAll(literalsReferences);
}

void GenerationalGC::flipSpaces()
{
	auxSpace.loadFrom(toSpace);
	toSpace.loadFrom(fromSpace);
	fromSpace.loadFrom(auxSpace);
	toSpace.reset();
	this->updateSpacesDelta();
}

void GenerationalGC::updateSpacesDelta()
{
	ulong delta;
	if (!vm.spacesDelta())
	{
		delta = toSpace.getBase() - fromSpace.getBase();
		vm.spacesDelta(delta);
	}
}


void GenerationalGC::fixReferencesFromNativeMethods()
{
	for (long index = 1; index <= literalsReferences.size()->_asNative(); index = index + 2)
	{
		oop_t *literal = literalsReferences[index];
		ulong offset = literalsReferences[index + 1]->_asNative();
		oop_t **reference = codeCacheReferenceAtOffset(offset);
		*reference = literal;
	}
}

void GenerationalGC::cleanRememberedSet()
{
	int kept = 0;
	oop_t* object = 0;
	for (long index = 1; index <= rememberedSet.size()->_asNative(); index++)
	{
		object = rememberedSet[index];
		rememberedSet[index] = KnownObjects::nil;
		if (!(toSpace.includes(object)))
		{
			kept++;
			rememberedSet[kept] = object;
		}
	}
	rememberedSet.size(smiConst(kept));
}


void GenerationalGC::followCountStartingAt(slot_t *root, int size, long start)
{
	//stack = self localStack.
	slot_t *objects = root;
	long index = start - 1;
	long limit = index + size;
	
	do
	{
		while (index < limit)
		{
			index = index + 1;
			oop_t *object = objects[index-1]; // was objects[index]
			if (this->arenaIncludes(object))
			{
				if (object->_isProxy())
				{
					objects[index-1] = object->_getProxee();  // was objects[index]
				}
				else
				{
					if (index < limit)
					{
						stack.push((oop_t*)objects);
						stack.push(smiConst(index));
						stack.push(smiConst(limit));
					}
					// FIXME: this line was not in smalltalk:
					//this->rememberIfWeak(object); // in the original I think it is a bug self rememberIfWeak: moved. and move before
					
					oop_t *moved = this->moveToOldOrTo(object);
					objects[index-1] = moved; // was objects[index]
					this->rememberIfWeak(moved);
					index = - 1;
					limit = index + moved->_strongPointersSize();
					objects = (oop_t**)moved;
				}
			}
		}

		if (!stack.isEmpty())
		{
			limit = stack.pop()->_asNative();
			index = stack.pop()->_asNative();
			objects = (oop_t **) stack.pop();
		}
		else
			break;
	} while(true);

}

void GenerationalGC::addInterrupt()
{
	//self interrupt: 11
}


