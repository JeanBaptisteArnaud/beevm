
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
	this->prepareForCompact;
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
	rememberedSet.setSpace(&oldSpace);
	rememberedSet.emptyReserving(0x100);
	literalsReferences.setSpace(&oldSpace);
	literalsReferences.emptyReserving(0x200);
	classCheckReferences.setSpace(&oldSpace);
	classCheckReferences.emptyReserving(0x100);
	nativizedMethods.setSpace(&oldSpace);
	nativizedMethods.emptyReserving(0x100);
	this->allocateWeakContainersArray();
	this->allocateEphemeronsArray();
	this->forgetNativeObjects();

}

void MarkAndCompactGC::allocateWeakContainersArray()
{
	tempArray.referer = VMVariablesProxy::hostVMWeakContainers();
	tempArray.loadMDAFrom(&nativizedMethods);
	tempArray.setSpace(&oldSpace);
	tempArray.emptyReserving(0x100);
}

void MarkAndCompactGC::allocateEphemeronsArray()
{
	tempArray.referer = VMVariablesProxy::hostVMEphemerons();
	tempArray.loadMDAFrom(&nativizedMethods);
	tempArray.setSpace(&oldSpace);
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

void MarkAndCompactGC::disableRememberSet()
{
	rememberedSet.setSpace(&localSpace);
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

void MarkAndCompactGC::librariesDo() {
	//: aBlock
	//	| array size |
	//	array : = self librariesArray.
	//	size : = self librariesArraySize.
	//	1 to : size do : [:i |
	//	library contents : (array _basicAt: i).
	//	aBlock value : library].
	//	library contents : nil


}

void MarkAndCompactGC::initNonLocals() {
	GarbageCollector::initNonLocals();
	tempArray.setSpace(&oldSpace);
	sKernelMeta.setBase((ulong *) VMVariablesProxy::hostVMFixedObjectsStart()->_asPointer());
	sKernelMeta.setNextFree((ulong *)VMVariablesProxy::hostVMFixedObjectsEnd()->_asPointer());
}

bool MarkAndCompactGC::arenaIncludes(oop_t * object) {
	return oldSpace.includes(object) || fromSpace.includes(object);

}

ulong MarkAndCompactGC::librariesArraySize() {
	return ((ulong)VMVariablesProxy::hostVMLibrariesArrayEnd() - (ulong)VMVariablesProxy::hostVMLibrariesArray()) / 4;
}


void MarkAndCompactGC::fixReferencesOrSetTombstone(oop_t * weakContainer) {
	/*| size |
		size : = (self arenaIncludes : weakContainer)
		ifTrue : [weakContainer _unthreadedSize]
		ifFalse : [weakContainer _size].
		1 to : size do : [:index | | instance seen |
		instance : = weakContainer _basicAt : index.
		seen : = (self arenaIncludes : instance)
		ifTrue : [instance _hasBeenSeenInSpace]
		ifFalse : [instance _hasBeenSeenInLibrary].
		seen ifFalse : [weakContainer _basicAt : index put : residueObject]].
		self follow : weakContainer count : size startingAt : 1*/
}


bool MarkAndCompactGC::checkReachablePropertyOf(oop_t * ephemeron) {
	return false;
	/*	
	| key |
	key : = ephemeron _basicAt : 1.
	key _isSmallInteger ifTrue : [^ true].
	^ (self arenaIncludes : key)
	ifTrue : [key _hasBeenSeenInSpace]
	ifFalse : [key _hasBeenSeenInLibrary]*/
}




	

	
