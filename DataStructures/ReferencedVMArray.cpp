/*
 * ReferencedVMArray.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "ReferencedVMArray.h"

using namespace Bee;

ReferencedVMArray::ReferencedVMArray() {
}

ReferencedVMArray ReferencedVMArray::currentFrom() {
	return GCSpace();
}

ReferencedVMArray ReferencedVMArray::currentTo() {
	return ReferencedVMArray();
}
ReferencedVMArray ReferencedVMArray::old() {
	return ReferencedVMArray();
}

