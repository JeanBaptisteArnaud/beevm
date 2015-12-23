/*
 * ReferencedVMArray.h
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef DATASTRUCTURES_REFERENCEDVMARRAY_H_
#define DATASTRUCTURES_REFERENCEDVMARRAY_H_

namespace Bee {
class ReferencedVMArray: public VMArray {
public:
	ReferencedVMArray();
	static ReferencedVMArray currentFrom();
	static ReferencedVMArray currentTo();
	static ReferencedVMArray old();

	long size();
};

}

#endif /* DATASTRUCTURES_REFERENCEDVMARRAY_H_ */
