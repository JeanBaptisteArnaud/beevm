/*
 * GCTests.h
 *
 *  Created on: 29 janv. 2016
 *      Author: jbapt
 */

#ifndef GCTEST_H_
#define GCTEST_H_

#include "MockedObjects.h"

namespace Bee {

class GCTest
{
protected:
	oop_t* global(const std::string &name);

	Memory *memoryForTesting();

// helpers for originally smalltalk methods (may be polymorphic)

	bool isSameArray(oop_t *object, oop_t *anotherObject);
	bool isArray(oop_t *object);

protected:
	MockedObjects mockedObjects;
};

}

#endif /* GCTEST_H_ */
