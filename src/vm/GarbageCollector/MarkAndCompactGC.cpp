
#include "MarkAndCompactGC.h"
#include "GarbageCollector.h"

#include "../DataStructures/ObjectFormat.h"
#include "../DataStructures/KnownObjects.h"
#include "../DataStructures/Memory.h"

using namespace Bee;

void MarkAndCompactGC::addInterrupt()
{
	//self interrupt: 7
}


void MarkAndCompactGC::collect()
{
	this->loadSpaces();
	this->initLocals();
	this->unmarkRememberSet();
	this->initAuxSpace();
	this->unseeWellKnownObjects();
	this->flushCodeCache();
	this->clearPolymorphicMethodCache();
	this->followAll();
	this->setNewPositionsAndCompact();
	this->updateOldSpace();
	this->makeRescuedEphemeronsNonWeak();
	this->updateNativeRescuedphemerons();
	this->allocateArrays();
	this->resetFrom();
	this->decommitSlack();
	this->addInterrupt();
	this->saveSpaces();
}

void MarkAndCompactGC::unmarkRememberSet()
{
	tempArray.contents = rememberedSet.contents;
	this->disableRememberSet();
	for (int i = 1; i < tempArray.size()->_asNative(); i++)
	{
		oop_t* object = tempArray[i];
		object->_beNotInRememberedSet();
	}
}

void MarkAndCompactGC::initAuxSpace()
{
	auxSpace.loadFrom(oldSpace);
	auxSpace.reset();
}

void MarkAndCompactGC::unseeWellKnownObjects()
{
	KnownObjects::nil->_beUnseenInLibrary();
	KnownObjects::stTrue->_beUnseenInLibrary();
	KnownObjects::stFalse->_beUnseenInLibrary();

	(*vm.debugFrameMarker)->_beUnseenInLibrary();

	this->unseeCharacters();
	this->unseeSKernelMeta();
	this->unseeLibraryObjects();
}

void MarkAndCompactGC::unseeCharacters()
{
	// characters.objectsDo(object _beUnseenInLibrary);
}

void MarkAndCompactGC::unseeSKernelMeta()
{
	// sLernelMeta.objectsDo(object _beUnseenInLibrary);
}
void MarkAndCompactGC::unseeLibraryObjects()
{
	// self librariesDo: [:sll | sll.objectsDo(object _beUnseenInLibrary)];
}
void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}

void MarkAndCompactGC::()
{
}


