
#ifndef _OBJECTHEADER_H_
#define _OBJECTHEADER_H_

#include <cassert>

#ifndef ASSERT
#define ASSERT assert
#endif


#include "Bee.h"

#define SMI_CLEAR_FLAG 0xFFFFFFFE

namespace Bee
{

//#pragma pack (1)
#pragma pack (push,1)
typedef struct basic_header_t
{
	uchar size;
	unsigned short hash;
	uchar flags;
	oop_t *behavior;

	void _size (ulong size)  { ASSERT(size  <= 0xFF);   this->size  = (uchar)size; }
	void _hash (ulong hash)  { ASSERT(hash  <= 0xFFFF); this->hash  = (ushort)hash; }
	void _flags(ulong flags) { ASSERT(flags <= 0xFF);   this->flags = (uchar)flags; }
	void _behavior(oop_t *behavior)  { this->behavior = behavior; }

	oop_t* slots() { return (oop_t*)((ulong)this + 8); }

// flags bits
	static const uchar Flag_unseenInSpace = 1;
	static const uchar Flag_generation = 2;
	static const uchar Flag_isEphemeron = 4;
	static const uchar Flag_isInRememberedSet = 8;
	static const uchar Flag_isBytes = 0x10;
	static const uchar Flag_zeroTermOrNamed = 0x20;
	static const uchar Flag_notIndexed = 0x40;
	static const uchar Flag_isExtended = 0x80;

	static const uchar ObjectFlagMaxValue = 0xFF;

// header byte offsets? Do we need them?
	static const ulong ObjectExtendedShortSize = -15;
	static const ulong ObjectSize = -7;
	static const ulong ObjectExtendedFlags = -12;
	static const ulong ObjectExtendedHash = -15;
	static const ulong ObjectExtendedSize = -2;
	static const ulong ObjectHash = -7;
	static const ulong ObjectFlags = -4;
	static const ulong ObjectHeaderBits = -1;

} basic_header_t;

typedef struct extended_header_t
{
	uchar basicSize; 	// 1
	unsigned short hash;	 	// 2
	uchar flags;		// 1
	ulong size;			// 4
	basic_header_t basic_header;

	void _size (ulong size)  { this->size  = size; }
	void _hash (ulong hash)  { ASSERT(false); this->hash = (ushort)hash;} // check what to do with basic hash
	void _flags(ulong flags) { ASSERT(false);   this->flags = (uchar)flags; } // idem
	void _behavior(oop_t *behavior)  { this->basic_header.behavior = behavior; }
	void _basicSize(ulong size)  { this->basicSize = (uchar)size; }

	void copyToBasic() { basic_header.size = basicSize; basic_header.hash = hash; basic_header.flags = flags; }

	oop_t* slots() { return (oop_t*)((ulong)this + 16); }
} extended_header_t;

#pragma pack (pop)



oop_t* smiConst(int number);
oop_t* pointerConst(ulong number);
oop_t* asObject(void *smallPointer);
ulong  asUObject(void *smallPointer);

struct oop_t
{
public:

	bool isSmallInteger() { return ((ulong)this & 1) == 1; }

	// small integers
	oop_t* smiPlusNative(int b) { return (oop_t*)(((ulong)this & SMI_CLEAR_FLAG) + (ulong)smiConst(b)); }
	
	long   _asNative()  { return (long)this >> 1; }
	void*  _asObject()  { return (void*)((ulong)this & ~1); }
	oop_t* _asPointer() { return isSmallInteger() ? (oop_t*)_asNative() : pointerConst((ulong)this); }


	basic_header_t*    basic_header()   { return (basic_header_t*)((ulong)this-8); }
	extended_header_t* extended_header(){ return (extended_header_t*)((ulong)this-16); }


	ulong _basicSize();
	void  _basicSize(uchar size);
	ulong _extendedSize();
	void  _extendedSize(ulong size);

	ulong _size();
	ulong _sizeInBytes();
	ulong _headerSizeInBytes();

	ulong _pointersSize();
	ulong _strongPointersSize();

	// flags
	bool _isBytes();
	bool _isActiveEphemeron();
	bool _isEphemeron();
	bool _isExtended();
	bool _isInRememberedSet();
	bool _isSecondGeneration();
	bool _isZeroTerminated();
	bool _hasWeaks();

	void _beInRememberedSet();
	void _beNotInRememberedSet();
	void _beSecondGeneration();
	void _beExtended();
	void _haveNoWeaks();
	void _beFullUnseenInSpace();

	bool testFlags    (unsigned char flag);
	bool testExtFlags (unsigned char flag);
	void setFlags     (unsigned char flag);
	void setExtFlags  (unsigned char flag);
	void unsetFlags   (unsigned char flag);
	void unsetExtFlags(unsigned char flag);


	// behavior and slots

	oop_t* behavior() { return this->basic_header()->behavior; }
	oop_t* setBehavior(oop_t *behavior) { return this->basic_header()->behavior = behavior; }
	slot_t& slot(long index) { ulong *pos = (ulong*)this + index; return *((slot_t*)pos); }
	uchar&  byte(long index) { uchar *pos = (uchar*)this + index; return *pos; }


	//done
	ulong _basicAt(int index);
	void  _basicAtPut(int index, unsigned long value);


	// proxying method
	bool _isProxy();
	void _setProxee(oop_t *copy);
	oop_t* _getProxee();

	// for testing
	bool equalsStr(const char *aString);
	bool equalsByteArray(oop_t *other);

};


basic_header_t* basic_header_cast(void* buffer);
extended_header_t* extended_header_cast(void* buffer);

// others

void   _free(ulong * limit, ulong * delta);
void _halt();
oop_t* _framePointer();


unsigned long rotateLeft(unsigned long n, unsigned int c);
unsigned long rotateRight(unsigned long n, unsigned int c);

}

#endif // ~ _OBJECTHEADER_H_