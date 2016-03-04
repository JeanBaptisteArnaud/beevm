
#include "MarkAndCompactGC.h"
#include "GarbageCollector.h"

#include "../DataStructures/ObjectFormat.h"
#include "../DataStructures/KnownObjects.h"
#include "../DataStructures/Memory.h"
#include "../DataStructures/SLLInfo.h"


using namespace Bee;



Bee::MarkAndCompactGC::MarkAndCompactGC()
{
}

Bee::MarkAndCompactGC::~MarkAndCompactGC()
{
}

void MarkAndCompactGC::initNonLocals()
{
	GarbageCollector::initNonLocals();
	tempArray.setSpace(&oldSpace);

	characters.base     = vm.charactersBase();
	characters.nextFree = vm.charactersNextFree();

	sKernelMeta.base     = vm.fixedObjectsStart();
	sKernelMeta.nextFree = vm.fixedObjectsEnd();

}

void MarkAndCompactGC::doCollect()
{
	this->unmarkRememberedSet();
	this->initAuxSpace();
	this->unseeWellKnownObjects();
	this->flushCodeCache();
	this->followAll();
	this->setNewPositionsAndCompact();
	this->updateOldSpace();
	this->makeRescuedEphemeronsNonWeak();
	this->updateNativeRescuedEphemerons();
	this->allocateArrays();
	this->resetFrom();
	this->decommitSlack();
	this->addInterrupt();
}

void MarkAndCompactGC::unmarkRememberedSet()
{
	tempArray.contents = rememberedSet.contents;
	this->disableRememberedSet();
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
	KnownObjects::frameMarker->_beUnseenInLibrary();

	this->unseeCharacters();
	this->unseeSKernelMeta();
	this->unseeLibraryObjects();
}

void MarkAndCompactGC::unseeCharacters()
{
	oop_t * next = characters.firstObject();
	while ((ulong *)next <= characters.getNextFree()) {
		next->_beUnseenInLibrary();
		next = next->nextObject();
	}
}

void MarkAndCompactGC::unseeSKernelMeta()
{
	oop_t * next = sKernelMeta.firstObject();
	while ((ulong *)next <= sKernelMeta.getNextFree()) {
		next->_beUnseenInLibrary();
		next = next->nextObject();
	}
}

void MarkAndCompactGC::unseeLibraryObjects()
{
	SLLInfo  library;
	ulong *array = vm.librariesArrayStart();
	ulong size = this->librariesArraySize();
	for (ulong index = 0; index < size; index++)
	{
		library.contents = (ulong*)array[index];
		oop_t * next = library.firstObject();
		while (library.isBelowNextFree(next)) 
		{
			next->_beUnseenInLibrary();
			next = next->nextObject();
		}
	}
	//library.contents = KnownObjects::nil;
}


ulong MarkAndCompactGC::librariesArraySize()
{
	return ((ulong)vm.librariesArrayEnd() - (ulong)vm.librariesArrayStart()) / 4;
}

void MarkAndCompactGC::flushCodeCache()
{
	vm.flushCodeCache();
	vm.anyNativizedCompiledMethodInFromSpace(false);
}


void MarkAndCompactGC::followAll()
{
	this->followWellKnownObjects();
	this->followStack();
	this->followExtraRoots();
	this->rescueEphemerons();
	this->fixWeakContainers();
	this->followRescuedEphemerons();
}

void MarkAndCompactGC::followWellKnownObjects()
{
	this->followCountStartingAt(vm.wellKnownRoots(), vm.wellKnownRootsSize(), 1);
}

void MarkAndCompactGC::followExtraRoots()
{
	this->followCountStartingAt(vm.extraRoots(), 3, 1);
}

void MarkAndCompactGC::followRescuedEphemerons()
{
	this->followCountStartingAt(&rescuedEphemerons.contents, 1, 1);
}

void MarkAndCompactGC::setNewPositionsAndCompact()
{
	this->setNewPositions(&oldSpace);
	this->setNewPositions(&fromSpace);
	this->prepareForCompact();
	this->compact(&oldSpace);
	this->compact(&fromSpace);
}
void MarkAndCompactGC::setNewPositions(GCSpace * space)
{
	ulong nextBuffer = asUObject(space->getBase());
	ulong nextHeaderSize = _unthreadedHeaderSizeInBytes(nextBuffer);
	oop_t * next = (oop_t *)(nextBuffer + nextHeaderSize);

	while (space->isBelowNextFree(next))
	{
		if (next->_hasBeenSeenInSpace())
		{
			oop_t * newPosition = (oop_t *)(asUObject(auxSpace.getNextFree()) + nextHeaderSize);
			oop_t * reference = next->_getProxee();
			oop_t * headerBits;

			do {
				headerBits = reference->slot(0);
				reference->slot(0) = newPosition;
				oop_t * nextReference = (oop_t *)(headerBits->_unrotate());
				if (nextReference->isSmallInteger()) 
					break; 
				else 
					reference = nextReference;
			} while (true);

			next->slot(-2) = headerBits;
			next->_beSeenInSpace();
			auxSpace.setNextFree((ulong *)pointerConst((ulong)newPosition + next->_sizeInBytes()));
		}

		nextBuffer = (ulong)next + next->_sizeInBytes();
		nextHeaderSize = _unthreadedHeaderSizeInBytes(nextBuffer);
		next = (oop_t *)(nextBuffer + nextHeaderSize);
	}
}

void MarkAndCompactGC::prepareForCompact()
{
	auxSpace.commitMoreMemoryIfNeeded(); // previously was grow()
	auxSpace.reset();
}

void MarkAndCompactGC::compact(GCSpace * space)
{
	oop_t *next = space->firstObject();
	while ((ulong *)next <= space->getNextFree())
	{
		oop_t *following = next->nextObject();

		if (next->_hasBeenSeenInSpace())
		{
			oop_t * copy = auxSpace.shallowCopy(next);
			copy->_beUnseenInSpace();
		}
		next = following;
	}
}

void MarkAndCompactGC::updateOldSpace()
{
	oldSpace.loadFrom(auxSpace);
}

void MarkAndCompactGC::updateNativeRescuedEphemerons()
{
	rescuedEphemerons.updateReference();
}

void MarkAndCompactGC::allocateArrays()
{
	rememberedSet.emptyReserving(0x100);
	literalsReferences.emptyReserving(0x200);
	classCheckReferences.emptyReserving(0x100);
	nativizedMethods.emptyReserving(0x100);
	this->allocateWeakContainersArray();
	this->allocateEphemeronsArray();
	this->forgetNativeObjects();

}

void MarkAndCompactGC::allocateWeakContainersArray()
{
	tempArray.referer = vm.weakContainers();
	tempArray.loadMDAFrom(&nativizedMethods);
	tempArray.emptyReserving(0x100);
}

void MarkAndCompactGC::allocateEphemeronsArray()
{
	tempArray.referer = vm.ephemerons();
	tempArray.loadMDAFrom(&nativizedMethods);
	tempArray.emptyReserving(0x100);
}

void MarkAndCompactGC::forgetNativeObjects()
{
	GarbageCollector::forgetNativeObjects();
	tempArray.forget();
}

void MarkAndCompactGC::resetFrom()
{
	fromSpace.reset();
}

void MarkAndCompactGC::decommitSlack()
{
	oldSpace.decommitSlack();
}

void MarkAndCompactGC::disableRememberedSet()
{
	rememberedSet.emptyReserving(0x100);
}

void MarkAndCompactGC::someEphemeronsRescued() {
	// Should be empty
}


void MarkAndCompactGC::followCountStartingAt(slot_t * root, int size, long base)
{
	//stack = self localStack.
	slot_t *scanned = root;
	long index = base - 1;
	long limit = index + size;
	do
	{
		while (index < limit)
		{
			index = index + 1;

			oop_t *object = scanned[index - 1];
			if (!object->isSmallInteger()) {
				if (this->arenaIncludes(object))
				{
					slot_t * oldScanned = scanned;
					long oldIndex = index;
					if (!(object->_hasBeenSeenInSpace())) {
						if (index < limit) {
							stack.push((oop_t *)scanned);
							stack.push(smiConst(index));
							stack.push(smiConst(limit));
						}
						this->rememberIfWeak(object);
						index = -1;
						limit = index + object->_strongPointersSize();
						scanned = (slot_t *)object;
					}
					object->_threadWithAt((oop_t *)oldScanned, oldIndex - 1);
				}
				else
				{
					if (!object->_hasBeenSeenInLibrary()) {
						object->_beSeenInLibrary();
						if (index < limit) {
							stack.push((oop_t *)scanned);
							stack.push(smiConst(index));
							stack.push(smiConst(limit));
						}
						this->rememberIfWeak(object);

						index = -1;
						limit = index + object->_strongPointersSize();
						scanned = (slot_t *)object;
					}
				}
			}
		}
		if (!stack.isEmpty())
		{
			limit = stack.pop()->_asNative();
			index = stack.pop()->_asNative();
			scanned = (slot_t *)stack.pop();
		}
		else break;
		
	} while (true);
}


bool MarkAndCompactGC::arenaIncludes(oop_t * object)
{
	return oldSpace.includes(object) || fromSpace.includes(object);
}


void MarkAndCompactGC::fixReferencesOrSetTombstone(oop_t *weakContainer)
{
	ulong size = this->arenaIncludes(weakContainer) ?
		weakContainer->_unthreadedSize() : weakContainer->_size();


	for (ulong i = 0; i < size; i++)
	{
		oop_t *instance = weakContainer->slot(i);
		bool seen = this->arenaIncludes(instance) ?
			instance->_hasBeenSeenInSpace() : instance->_hasBeenSeenInLibrary();

		if (!seen)
			weakContainer->slot(i) = residueObject;
	}

	this->followCountStartingAt((slot_t*)weakContainer, size, 1);
}


bool MarkAndCompactGC::checkReachablePropertyOf(oop_t * ephemeron)
{

	oop_t *key = ephemeron->slot(0);
	if (key->isSmallInteger())
		return true;

	return this->arenaIncludes(key) ? key->_hasBeenSeenInSpace() : key->_hasBeenSeenInLibrary();
}

void MarkAndCompactGC::addInterrupt()
{
	//self interrupt: 7
}

