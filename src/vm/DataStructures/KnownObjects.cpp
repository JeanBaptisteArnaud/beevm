/*
 * VMMemory.cpp
 *
 *  Created on: 6 janv. 2016
 *      Author: jbapt
 */
#include "KnownObjects.h"
#include "Memory.h"

#include <windows.h>
#include <climits>
#include <iostream>

using namespace Bee;
using namespace std;

#define HOSTVM_NIL    (oop_t*)0x10026060
#define HOSTVM_TRUE   (oop_t*)0x10026070
#define HOSTVM_FALSE  (oop_t*)0x10026080

oop_t  *KnownObjects::nil;
oop_t  *KnownObjects::stTrue;
oop_t  *KnownObjects::stFalse;
oop_t  *KnownObjects::emptyArray;
Memory *KnownObjects::currentMemory;

void Bee::initializeKnownObjects(oop_t *aNil, oop_t *aTrue, oop_t *aFalse, oop_t *emptyArray, Memory *aMemory)
{
	KnownObjects::nil     = aNil;
	KnownObjects::stTrue  = aTrue;
	KnownObjects::stFalse = aFalse;
	KnownObjects::emptyArray = emptyArray;
	KnownObjects::currentMemory = aMemory;
}

void Bee::initializeKnownObjectsOnHostVM(Memory *aMemory)
{
	cout << "FIXME: find a way to get an empty array" << endl;
	initializeKnownObjects(HOSTVM_NIL, HOSTVM_TRUE, HOSTVM_FALSE, HOSTVM_NIL, aMemory);
}




