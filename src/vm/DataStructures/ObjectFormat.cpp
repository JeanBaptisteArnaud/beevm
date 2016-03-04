
#include <iostream>
#include <cstdio>
#include <climits>
#include <cstring>

#include "ObjectFormat.h"
#include "Bee.h"

using namespace std; 
using namespace Bee;

basic_header_t* Bee::basic_header_cast(void* buffer)
{
	return (basic_header_t*)buffer;
}

extended_header_t* Bee::extended_header_cast(void* buffer)
{
	return (extended_header_t*)buffer;
}

oop_t * Bee::headerToObject(void * buffer)
{
	oop_t * object = ((oop_t *)((ulong)buffer + 8));
	return object->_isExtended() ? (oop_t *)((ulong)buffer + 16) : object;
}

oop_t * Bee::headerToObjectCheckProxee(void * buffer)
{	
	oop_t * object = ((oop_t *)((ulong)buffer + 8));
	return 	object->_isProxy() || (!object->_isExtended()) ? object : (oop_t *)((ulong)buffer + 16)  ;
}

ulong Bee::_unthreadedHeaderSizeInBytes(ulong buffer)
{
	oop_t * object = ((oop_t *)(buffer + 8));
	return 	object->_isProxy() || !object->_isExtended() ? 8 : 16;
}




oop_t* Bee::smiConst(int number)
{
	return (oop_t*)((number << 1) | 1);
}

oop_t* Bee::pointerConst(ulong number)
{
	return (oop_t*)(number | 1);
}

oop_t* Bee::asObject(void *smallPointer)
{
	return (oop_t*)asUObject(smallPointer);
}

ulong Bee::asUObject(void *smallPointer)
{
	return (ulong)smallPointer & ~1;
}

// size calculation

ulong oop_t::_basicSize()
{
	return this->basic_header()->size;
}

void oop_t::_basicSize(uchar size)
{
	this->basic_header()->size = (char)size;
}


ulong oop_t::_extendedSize()
{
	return this->extended_header()->size;
}


void oop_t::_extendedSize(ulong size)
{
	this->extended_header()->size = size;
}

ulong oop_t::_size()
{
	int total;
	if (this->_isExtended())
		total = this->_extendedSize();
	else
		total = this->_basicSize();
	if (this->_isBytes() && this->_isZeroTerminated())
		return total - 1;
	else
		return total;
}

ulong oop_t::_sizeInBytes()
{
	if (this->_isBytes())
	{
		int zero;
		if (this->_isZeroTerminated())
			zero = 1;
		else
			zero = 0;
		return (this->_size() + 3 + zero) & -4;

	} else
		return this->_size() * 4;
}



ulong oop_t::_headerSizeInBytes()
{
	if (this->_isExtended())
	{
		return this->_basicSize() * 4;
	} 
	else
	{
		return 8;
	}
}

ulong oop_t::_pointersSize()
{
	if (this->_isBytes())
		return 1;
	else
		return 1 + this->_size();
}

ulong oop_t::_strongPointersSize()
{
	if (this->_isBytes() || this->_hasWeaks())
		return 1;
	else
		return 1 + this->_size();
}


// flags manipulation

bool oop_t::testFlags(unsigned char flag)
{
	return (this->basic_header()->flags & (flag)) == flag;
}

void oop_t::setFlags(unsigned char flag)
{
	this->basic_header()->flags |= flag;
}

void oop_t::unsetFlags(unsigned char flag)
{
	this->basic_header()->flags &= (flag) ^ basic_header_t::ObjectFlagMaxValue;
}

bool oop_t::testExtFlags(unsigned char flag)
{
	return (this->extended_header()->flags & flag) == flag;
}

void oop_t::setExtFlags(unsigned char flag)
{
	this->extended_header()->flags |= flag;
}

void oop_t::unsetExtFlags(unsigned char flag)
{
	this->extended_header()->flags &= (flag ^ basic_header_t::ObjectFlagMaxValue);
}

void oop_t::_beExtended()
{
	this->setFlags(basic_header_t::Flag_isExtended);
}

bool oop_t::_hasWeaks()
{
	return this->testFlags(basic_header_t::Flag_isEphemeron);
}

void oop_t::_haveNoWeaks()
{
	this->unsetFlags(basic_header_t::Flag_isEphemeron);
}

bool oop_t::_isActiveEphemeron()
{
	return this->_hasWeaks() && this->_isEphemeron();
}

bool oop_t::_isEphemeron()
{
	return this->testFlags   (basic_header_t::Flag_isExtended) &&
	       this->testExtFlags(basic_header_t::Flag_isEphemeron);
}

void oop_t::_beSecondGeneration()
{
	this->setFlags(basic_header_t::Flag_generation);
}

bool oop_t::_isSecondGeneration()
{
	return this->testFlags(basic_header_t::Flag_generation);
}

bool oop_t::_isBytes()
{
	return this->testFlags(basic_header_t::Flag_isBytes);
}

bool oop_t::_isExtended()
{
	return this->testFlags(basic_header_t::Flag_isExtended);
}

bool oop_t::_isZeroTerminated()
{
	return this->testFlags(basic_header_t::Flag_zeroTermOrNamed);
}

bool oop_t::_isInRememberedSet()
{
	return this->testFlags(basic_header_t::Flag_isInRememberedSet);
}

void oop_t::_beNotInRememberedSet()
{
	this->unsetFlags(basic_header_t::Flag_isInRememberedSet);
}

void oop_t::_beInRememberedSet()
{
	this->setFlags(basic_header_t::Flag_isInRememberedSet);
}

void oop_t::_beSeenInSpace()
{
	this->unsetFlags(basic_header_t::Flag_unseenInSpace);
}

void oop_t::_beUnseenInSpace()
{
	this->setFlags(basic_header_t::Flag_unseenInSpace);
}

void oop_t::_beUnseenInSpaceFull()
{
	this->setFlags(basic_header_t::Flag_unseenInSpace);
	if (this->_isExtended())
		this->setExtFlags(basic_header_t::Flag_unseenInSpace);
}

void oop_t::_beSeenInSpaceFull()
{
	this->unsetFlags(basic_header_t::Flag_unseenInSpace);
	if (this->_isExtended())
		this->unsetExtFlags(basic_header_t::Flag_unseenInSpace);
}

void oop_t::_beUnseenInLibrary()
{
	this->_beSeenInSpaceFull();
}

void oop_t::_beSeenInLibrary()
{
	this->_beUnseenInSpaceFull();
}

bool oop_t::_hasBeenSeenInSpace()
{
	return !this->_hasBeenSeenInLibrary();
}

bool oop_t::_hasBeenSeenInLibrary()
{
	return this->testFlags(basic_header_t::Flag_unseenInSpace);
}

oop_t * oop_t::nextObject()
{
	ulong completeSizeOfObject = this->_sizeInBytes();
	ulong nextHeader = ((ulong) this + completeSizeOfObject);
	return headerToObject((ulong *) nextHeader);
}

oop_t * oop_t::nextObjectAfterMark()
{

	ulong completeSizeOfObject = this->_sizeInBytes();
	ulong nextHeader = ((ulong) this + completeSizeOfObject);
	return headerToObjectCheckProxee((ulong *)nextHeader);
}



// proxiing method
bool oop_t::_isProxy()
{
	return !this->testFlags(basic_header_t::Flag_unseenInSpace);
}

void oop_t::_setProxee(oop_t *value)
{
	this->slot(-2) = (oop_t*)value->_rotate();
}

oop_t* oop_t::_getProxee()
{
	return (oop_t*)this->slot(-2)->_unrotate();
}

ulong oop_t::_rotate()
{
	return rotateRight((ulong)this, 8);
}

ulong oop_t::_unrotate()
{
	return rotateLeft((ulong)this, 8);
}

ulong oop_t::_unthreadedSize()
{
	debug("add test to check that _unthreadedSize works correctly in all cases");
	oop_t *object = this->_getProxee();
	oop_t *nextOrFlags;
	
	do {
		nextOrFlags = (oop_t*)object->slot(0)->_unrotate();
		if (nextOrFlags->isSmallInteger())
			break;
		
		object = nextOrFlags;
	}
	while (true);

	ulong flags = (ulong)nextOrFlags;
	return (flags & basic_header_t::Flag_isExtended) == 0 ? (flags >> 8) & 0xFF : this->_extendedSize();
}

void oop_t::_threadWithAt(oop_t *other, long index)
{
	other->slot(index) = this->slot(-2);
	this->_setProxee((oop_t *) &other->slot(index));
}

// tool
ulong Bee::rotateLeft(ulong n, unsigned int c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);
	c &= mask;
	return (n << c) | (n >> ((-c) & mask));
}

ulong Bee::rotateRight(ulong n, unsigned int c)
{
	const unsigned int mask = (CHAR_BIT * sizeof(n) - 1);
	c &= mask;
	return (n >> c) | (n << ((-c) & mask));
}


void _halt()
{
	perror("_halt encountered");
}

// should work with symbols (check needed)
bool oop_t::equalsStr(const char *aString)
{
	return strncmp((char*)this, aString, this->_size()) == 0;
}


bool oop_t::equalsByteArray(oop_t *other)
{
	if (this->_size() != other->_size())
		return false;

	for (ulong i = 0; i < this->_size(); i++)
	{
		if (this->byte(i) != other->byte(i))
			return false;
	}

	return true;
}
