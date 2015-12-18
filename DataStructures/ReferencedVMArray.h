/*
 * ReferencedVMArray.h
 *
 *  Created on: 18 déc. 2015
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
};

}

#endif /* DATASTRUCTURES_REFERENCEDVMARRAY_H_ */
