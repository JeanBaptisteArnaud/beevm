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

void ReferencedVMArray::contentsFromReferer()
{
	contents = *objectReferer();
}

void ReferencedVMArray::updateReference()
{
	if ((oop_t*)referer != KnownObjects::nil)
		*objectReferer() = contents;
}

void ReferencedVMArray::emptyReserving(ulong size){
	 VMArray::emptyReserving(size);
	this->updateReference();
	this->setContentsMDA();
}

void ReferencedVMArray::loadMDAFrom(ReferencedVMArray *other)
{
	behavior = other->behavior;
}

void ReferencedVMArray::setReferer(oop_t **address)
{
	referer = (oop_t**)((oop_t*)address)->_asPointer();
	//this->updateFromReferer(); // not needed here any more
}

void ReferencedVMArray::updateFromReferer()
{
	this->contentsFromReferer();
	this->loadContentsMDA();
}

oop_t** ReferencedVMArray::objectReferer()
{
	return (oop_t**)asObject(referer);
}

void ReferencedVMArray::forget()
{
	contents = behavior = KnownObjects::nil;
}

void ReferencedVMArray::grow()
{
	VMArray::grow();
	this->updateReference();
}

void ReferencedVMArray::loadContentsMDA()
{
	behavior = contents->behavior();
}

void ReferencedVMArray::setContentsMDA()
{
	if (behavior != KnownObjects::nil)
		contents->setBehavior(behavior);
}

