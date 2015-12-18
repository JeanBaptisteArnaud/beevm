/*
 * VMArray.h
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef DATASTRUCTURES_VMARRAY_H_
#define DATASTRUCTURES_VMARRAY_H_

namespace Bee {
class VMArray {
public:
	VMArray();
	static VMArray currentFrom();
	static VMArray currentTo();
	static VMArray old();
};

}

#endif /* DATASTRUCTURES_VMARRAY_H_ */
