/*
 * ReferencedVMArray.h
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef DATASTRUCTURES_REFERENCEDVMARRAY_H_
#define DATASTRUCTURES_REFERENCEDVMARRAY_H_

#include "VMArray.h"


namespace Bee {

class ReferencedVMArray: public VMArray {
public:
	ReferencedVMArray();
	oop_t **referer;
	oop_t *behavior;

	void contentsFromReferer();
	void emptyReserving(ulong size);
	void loadMDAFrom(ReferencedVMArray * other);
	void setReferer(oop_t **address);
	oop_t** objectReferer();
	void forget();
	void grow();
	void loadContentsMDA();
	void setContentsMDA();
	void updateReference();
	void updateFromReferer();
};

}

#endif /* DATASTRUCTURES_REFERENCEDVMARRAY_H_ */
