
#ifndef _SLLINFO_H_
#define _SLLINFO_H_

#include "GCSpace.h"

namespace Bee
{

class SLLInfo : public GCSpace
{
public:

	oop_t * contents;


	SLLInfo();
	~SLLInfo();
	ulong * getBase();
	ulong * getNextFree();
	oop_t * end();
	ulong version();
	ulong dataSize();
};

}


#endif // ~ _SLLINFO_H_

