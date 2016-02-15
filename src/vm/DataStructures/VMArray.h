/*
 * VMArray.h
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GCSpace.h"

#ifndef DATASTRUCTURES_VMARRAY_H_
#define DATASTRUCTURES_VMARRAY_H_

namespace Bee {

class VMArray
{
public:
	VMArray();

	void setSpace(GCSpace *argSpace);
	ulong* init();
	void add(oop_t *object);
	void addAll(oop_t *array);

	void emptyWith(oop_t *);
	void emptyReserving(ulong size);

	void grow();
	void grow(ulong);

	bool includes(oop_t *);

	oop_t* nextFree();
	void nextFree(oop_t *newSize);

	oop_t* size();
	void size(oop_t *newSize);

	void  push(oop_t *object);
	oop_t* pop();
	bool  isEmpty();
	void  reset();
	slot_t& operator[](ulong i);


public:  // would like protected: but this eases testing
	GCSpace *space;
	oop_t *contents;

};

}

#endif /* DATASTRUCTURES_VMARRAY_H_ */
