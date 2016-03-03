
#include <sstream>
#include <Windows.h>

#include "../vm/DataStructures/Memory.h"
#include "../vm/DataStructures/GCSpaceInfo.h"
#include "../vm/GarbageCollector/GenerationalGC.h"
#include "../vm/GarbageCollector/MarkAndCompactGC.h"

#include "../vm/GarbageCollector/SanityChecker.h"

using namespace Bee;

Memory *memory;

extern "C" __declspec(dllexport) void setMemory(Memory *aMemory);
extern "C" __declspec(dllexport) void scavengeHostVMFromSpace();
extern "C" __declspec(dllexport) void markAndCompactArena();


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
	memory->flipper   = new(memory->flipper) GenerationalGC();
	memory->compactor = new(memory->compactor) MarkAndCompactGC();

	memory->useHostVMVariables();
	
	memory->flipper->memory = memory;
	memory->flipper->localSpace.loadFrom(GCSpaceInfo::newSized(32 * 1024));
	memory->flipper->initialize();

	memory->compactor->memory = memory;
	memory->compactor->localSpace.loadFrom(GCSpaceInfo::newSized(32 * 1024));
	memory->compactor->initialize();

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

void assertSaneObjects();

void scavengeHostVMFromSpace()
{
	debug("flip starting");
	memory->updateFromHostVM();
	debug((char*)status().c_str());

	memory->scavengeFromSpace();
	
	memory->updateToHostVM();

	debug("flip ended");
	debug((char*)status().c_str());
	
	//debug("checking sanity... ");
	//assertSaneObjects();
	//debug("ok");

}

void markAndCompactArena()
{
	debug("mark and compact starting");
	memory->updateFromHostVM();
	debug((char*)status().c_str());

	memory->collectOldSpace();
	
	memory->updateToHostVM();

	debug("mark and compact ended");
	debug((char*)status().c_str());
	
	//debug("checking sanity... ");
	//assertSaneObjects();
	//debug("ok");

}

void assertSaneObjects()
{
	SanityChecker checker(*memory->fromSpace);
	checker.checkAllSaneIn(*memory->fromSpace);
	checker.checkAllSaneIn(*memory->oldSpace);
}
