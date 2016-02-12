/*
 * ReferencedVMArray.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "ReferencedVMArray.h"
#include "ObjectFormat.h"
#include "KnownObjects.h"

using namespace Bee;

ReferencedVMArray::ReferencedVMArray() : referer(NULL)
{
}

void ReferencedVMArray::contentsFromReferer() {
	contents = ((oop_t*)referer->_asObject())->slot(0);
}

void ReferencedVMArray::emptyReserving(ulong size){
	// VMArray::emptyReserving(size); FIXME: implement in superclass
	this->updateReference();
	this->setContentsMDA();
}

void ReferencedVMArray::loadMDAFrom(ReferencedVMArray * other){
	behavior = other->behavior;
}
void ReferencedVMArray::setReferer(oop_t *address) {
	referer = address->_asPointer();
	this->contentsFromReferer();
	this->loadContentsMDA();
}
void ReferencedVMArray::forget() {
	contents = behavior = KnownObjects::nil;
}

void ReferencedVMArray::grow() {
	VMArray::grow();
	this->updateReference();
}

void ReferencedVMArray::loadContentsMDA(){
	behavior = contents->behavior();
}
void ReferencedVMArray::setContentsMDA() {
	if (behavior != KnownObjects::nil) contents->setBehavior(behavior);
}

void ReferencedVMArray::updateReference() {
	if (referer != KnownObjects::nil)
		((oop_t*)referer->_asObject())->slot(0) = contents;
}
