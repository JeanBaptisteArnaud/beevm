
#include <Windows.h>

#include "../vm/DataStructures/Memory.h"

Bee::Memory *memory;

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD fwdReason, LPVOID reserved)
{
	if (fwdReason == DLL_PROCESS_ATTACH)
	{

	}
	
	return TRUE;
}

void setMemory(Bee::Memory *aMemory)
{
	memory = aMemory;
	memory->useHostVMVariables();
}


void scavengeHostVMFromSpace()
{
	memory->updateFromHostVM();
	memory->scavengeFromSpace();
	memory->updateToHostVM();
}

