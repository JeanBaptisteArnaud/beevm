#pragma once
#include "D:\Bee\beevm\src\vm\DataStructures\GCSpace.h"

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
