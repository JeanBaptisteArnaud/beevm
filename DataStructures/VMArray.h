/*
 * VMArray.h
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef DATASTRUCTURES_VMARRAY_H_
#define DATASTRUCTURES_VMARRAY_H_

namespace Bee {
class VMArray {
public:
	VMArray();
	unsigned long size();
	unsigned long size(unsigned long);
	bool isEmpty();
	void reset();
	unsigned long*& operator[](int i);

private:
	unsigned long *contents;
};

}

#endif /* DATASTRUCTURES_VMARRAY_H_ */
