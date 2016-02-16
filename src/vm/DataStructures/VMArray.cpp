/*
 * VMArray.cpp
 *
 *  Created on: 18 dic. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include <iostream>

#include "VMArray.h"
#include "Memory.h"
#include "ObjectFormat.h"
#include "KnownObjects.h"


using namespace Bee;
using namespace std;


VMArray::VMArray()
{
}


void VMArray::setSpace(GCSpace * argSpace)
{
	space = argSpace;
}


void VMArray::emptyWith(oop_t * array)
{
	contents = array;
	this->size(0);
}

void VMArray::emptyReserving(ulong size)
{
	oop_t * array = space->shallowCopyGrowingTo(KnownObjects::emptyArray, size);
	array->_beFullUnseenInSpace();
	this->emptyWith(array);
}

void VMArray::grow()
{
	contents = space->shallowCopyGrowingTo(contents, contents->_size() * 2);
}

void VMArray::grow(ulong slots)
{
	contents = space->shallowCopyGrowingTo(contents, contents->_size() + slots);
}

oop_t* VMArray::nextFree()
{
	return contents->slot(0)->smiPlusNative(-1);
}

oop_t* VMArray::size()
{
	return nextFree()->smiPlusNative(-1);
}

void VMArray::nextFree(oop_t *newSize)
{
	contents->slot(0) = newSize->smiPlusNative(1);
}


void VMArray::size(oop_t *newSize)
{
	nextFree(newSize->smiPlusNative(1));
}

bool VMArray::isEmpty()
{
	return size() == smiConst(0);
}

void VMArray::reset()
{
	size(smiConst(0));
}

oop_t* VMArray::pop()
{
	oop_t *value = contents->slot(this->size()->_asNative());
	//contents[this->size()] = nil; Maybe reset to nil it is the point to have VM Array
	this->size(this->size()->smiPlusNative(-1));
	return value;

}

void VMArray::push(oop_t *value)
{
	contents->slot(size()->smiPlusNative(1)->_asNative()) = value;
	//contents[this->size()] = nil; Maybe reset to nil it is the point to have VM Array
	this->size(this->size()->smiPlusNative(1));
}


void VMArray::add(oop_t *value)
{
	if (this->size()->_asNative() + 1 >= (long)contents->_size())
		this->grow();

	this->size(this->size()->smiPlusNative(1));
	contents->slot(this->size()->_asNative()) = value;
}


bool VMArray::includes(oop_t *value)
{
	long last = this->size()->_asNative();
	for (long index = 1; index < last; index++)
	{
		if (contents->slot(index)== value)
			return true;
	}

	return false;
}


void VMArray::addAll(oop_t *array)
{
	ulong needed = array->_size();

	if (this->size()->_asNative() + needed >= contents->_size())
		this->grow(needed);

	for (ulong index = 0; index < needed; index++)
	{
		this->add(array->slot(index));
	}
}

slot_t& VMArray::operator[](ulong i)
{
	return contents->slot(i);
}

