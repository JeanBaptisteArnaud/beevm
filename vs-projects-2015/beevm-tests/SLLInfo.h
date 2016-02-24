#pragma once
#include "D:\Bee\beevm\src\vm\DataStructures\GCSpace.h"

namespace Bee
{

	class SLLInfo : public GCSpace
	{
	public:
		SLLInfo();
		~SLLInfo();
		ulong * getBase();
		ulong * getNextFree();
		oop_t * end();
		ulong version();
		ulong dataSize();

	protected:
		oop_t * contents;

	};
}
