
#include "MarkAndCompactGC.h"
#include "GarbageCollector.h"

#include "../DataStructures/ObjectFormat.h"
#include "../DataStructures/KnownObjects.h"
#include "../DataStructures/Memory.h"

using namespace Bee;

void MarkAndCompactGC::initNonLocals()
{
	GarbageCollector::initNonLocals();
	tempArray.setSpace(&oldSpace);
	
}

void MarkAndCompactGC::useHostVMVariables()
{
	GarbageCollector::useHostVMVariables();

	sKernelMeta.setBase((ulong *) VMVariablesProxy::hostVMFixedObjectsStart()->_asPointer());
	sKernelMeta.setNextFree((ulong *)VMVariablesProxy::hostVMFixedObjectsEnd()->_asPointer());
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
	this->updateNativeRescuedphemerons();
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

	(*vm.debugFrameMarker)->_beUnseenInLibrary();

	this->unseeCharacters();
	this->unseeSKernelMeta();
	this->unseeLibraryObjects();
}

void MarkAndCompactGC::unseeCharacters()
{
	oop_t * next = characters.firstObject();
	while ((ulong *) next <= characters.getNextFree()) {
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
	// self librariesDo: [:sll | sll.objectsDo(object _beUnseenInLibrary)];
}
void MarkAndCompactGC::flushCodeCache()
{
	//this->_flushCodeCache();
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
	this->followCountStartingAt(VMVariablesProxy::hostVMWellKnownRoots(), VMVariablesProxy::hostVMWellKnownRootsSize(), 1);
}

void MarkAndCompactGC::followExtraRoots()
{
	this->followCountStartingAt(VMVariablesProxy::hostVMExtraRoots(), 3, 1);
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
	//| reference headerBits |

	//space.seenObjectsDo (:object :headerSize | | newPosition nextReference |
	//newPosition : = auxSpace nextFree + headerSize.
	//reference : = object _proxee.
	//[
	//	headerBits:= reference _basicAt : 1.
	//			   reference _basicAt : 1 put : newPosition _asObject.
	//nextReference : = headerBits _unrotate.
	//nextReference _isSmallInteger]
	//			 whileFalse : [reference:= nextReference].
	//				 object _basicAt : -1 put : headerBits; _beSeenInSpace.
	//				 auxSpace nextFree : newPosition + object _sizeInBytes _asPointer])

}

void MarkAndCompactGC::prepareForCompact()
{
	auxSpace.commitMoreMemoryIfNeeded(); // previously was grow()
	auxSpace.reset();
}

void MarkAndCompactGC::compact(GCSpace * space)
{
	//space objectsDo : [:object |
	//object _hasBeenSeenInSpace ifTrue : [| moved |
	//moved:= auxSpace shallowCopy : object.
	//moved _beUnseenInSpace]]

}

void MarkAndCompactGC::updateOldSpace()
{
	oldSpace.loadFrom(auxSpace);
}

void MarkAndCompactGC::updateNativeRescuedphemerons()
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
	tempArray.referer = VMVariablesProxy::hostVMWeakContainers();
	tempArray.loadMDAFrom(&nativizedMethods);
	tempArray.emptyReserving(0x100);
}

void MarkAndCompactGC::allocateEphemeronsArray()
{
	tempArray.referer = VMVariablesProxy::hostVMEphemerons();
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



void MarkAndCompactGC::followCountStartingAt(slot_t *frame, ulong size, ulong startIndex)
{
	//| index scanned limit |
	//scanned : = root.
	//index : = base - 1.
	//limit : = index + size.
	//[
	//	[index < limit] whileTrue:[| object |
	//	index:= index + 1.
	//object : = scanned _basicAt : index.
	//object _isSmallInteger ifFalse : [
	//	(self arenaIncludes : object)
	//		ifTrue:[| oldIndex oldScanned |
	//		oldScanned:= scanned.
	//		oldIndex : = index.
	//		object _hasBeenSeenInSpace ifFalse : [
	//			index < limit ifTrue : [
	//				stack
	//					push : scanned;
	//			push: index;
	//			push: limit].
	//				self rememberIfWeak : object].
	//		object _threadWith : oldScanned at : oldIndex]
	//		ifFalse: [
	//					 object _hasBeenSeenInLibrary ifFalse : [
	//						 object _beSeenInLibrary.
	//							 index < limit ifTrue : [
	//								 stack
	//									 push : scanned;
	//							 push: index;
	//							 push: limit].
	//							 self rememberIfWeak : object].
	//						 index : = -1.
	//								 limit : = index + object _strongPointersSize.
	//								 scanned : = object]]].
	//stack isEmpty]
	//	whileFalse: [
	//					limit:= stack pop.
	//						index : = stack pop.
	//						scanned : = stack pop]
}

void MarkAndCompactGC::librariesDo()
{
	//: aBlock
	//	| array size |
	//	array : = self librariesArray.
	//	size : = self librariesArraySize.
	//	1 to : size do : [:i |
	//	library contents : (array _basicAt: i).
	//	aBlock value : library].
	//	library contents : nil
}

bool MarkAndCompactGC::arenaIncludes(oop_t * object)
{
	return oldSpace.includes(object) || fromSpace.includes(object);
}

ulong MarkAndCompactGC::librariesArraySize()
{
	return ((ulong)VMVariablesProxy::hostVMLibrariesArrayEnd() - (ulong)VMVariablesProxy::hostVMLibrariesArray()) / 4;
}

void MarkAndCompactGC::fixReferencesOrSetTombstone(oop_t * weakContainer)
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

	this->followCountStartingAt((slot_t*)weakContainer, size,1);
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

