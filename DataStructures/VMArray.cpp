/*
 * VMArray.cpp
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "VMArray.h"

using namespace Bee;

VMArray::VMArray() {
}

long VMArray::nextFree()
{
	return contents[0] - 1;
}

long VMArray::size()
{
	return nextFree() - 1;
}

long VMArray::nextFree(unsigned long newSize)
{
	contents[0] = newSize + 1;
}


long VMArray::size(unsigned long newSize)
{
	nextFree(newSize + 1);
}

bool VMArray::isEmpty()
{
	return size() == 0;
}

void VMArray::reset()
{
	size(0);
}

unsigned long*& VMArray::operator[](int i)
{
	contents[i];
}
