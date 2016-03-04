
#ifndef _SLLINFO_H_
#define _SLLINFO_H_

#include "GCSpace.h"

namespace Bee
{

class SLLInfo
{
public:
	SLLInfo();
	~SLLInfo();

	oop_t* firstObject();
	ulong* getBase();
	ulong* getNextFree();
	ulong* end();
	ulong version();
	ulong dataSize();
	bool isBelowNextFree(oop_t *object);

	ulong* contents;
};

}


#endif // ~ _SLLINFO_H_

