/*
 * KnownObjects.h
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste, Javier Pimas
 */

/**
 * This should manage access to known objects in memory.
 * to see it deeper, look at subclasses: HostVMKnownObjects,
 * MockedKnownObjects, BeeKnownObjects.
**/

#ifndef _KNOWNOBJECTS_H_
#define _KNOWNOBJECTS_H_

#include "Bee.h"

namespace Bee
{

class Memory;

class KnownObjects
{
public:
	static oop_t *nil;
	static oop_t *stTrue;
	static oop_t *stFalse;

	static oop_t *emptyArray;

	static oop_t *Array_behavior;
	static Memory *currentMemory;

};

void initializeKnownObjects(oop_t *aNil, oop_t *aTrue, oop_t *aFalse, oop_t *emptyArray, Memory *aMemory);
void initializeKnownObjectsOnHostVM(Memory *aMemory);


}

#endif /* _KNOWNOBJECTS_H_ */
