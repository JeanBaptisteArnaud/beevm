
#include <sstream>
#include <Windows.h>

#include "../vm/DataStructures/Memory.h"
#include "../vm/DataStructures/GCSpaceInfo.h"
#include "../vm/GarbageCollector/GenerationalGC.h"

using namespace Bee;

Memory *memory;

extern "C" __declspec(dllexport) void scavengeHostVMFromSpace();
extern "C" __declspec(dllexport) void setMemory(Memory *aMemory);


BOOL WINAPI DllMain(HINSTANCE hModule, DWORD fwdReason, LPVOID reserved)
{
	if (fwdReason == DLL_PROCESS_ATTACH)
	{

	}
	
	return TRUE;
}

void setMemory(Memory *aMemory)
{
	memory = aMemory;

	KnownObjects::initializeFromHostVM();

	// initialize the already given flipper buffer as a C++ GenerationalGC
	memory->flipper = new(memory->flipper) GenerationalGC();

	memory->useHostVMVariables();
	
	memory->flipper->memory = memory;
	memory->flipper->localSpace.loadFrom(GCSpaceInfo::newSized(32 * 1024));
	memory->flipper->initialize();
}

std::string status()
{
	ostringstream out;
	out << endl << "------------------------------" << endl << "From: " ;
	memory->fromSpace->print(out);

	out << endl << "To:   ";
	memory->  toSpace->print(out);

	out << endl << "Old:  ";
	memory-> oldSpace->print(out);

	out << endl << "==============================" << endl;
	return out.str();
}


void scavengeHostVMFromSpace()
{
	debug("flip starting");
	memory->updateFromHostVM();
	debug((char*)status().c_str());

	memory->scavengeFromSpace();

	memory->updateToHostVM();
	debug("flip ended");
	debug((char*)status().c_str());
}

