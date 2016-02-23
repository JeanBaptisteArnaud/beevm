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

class KnownObjects
{
public:
	static oop_t *nil;
	static oop_t *stTrue;
	static oop_t *stFalse;

	static oop_t *emptyArray;

	static void initialize(oop_t *aNil, oop_t *aTrue, oop_t *aFalse);
	static void initializeFromHostVM();

};

}

#endif /* _KNOWNOBJECTS_H_ */
