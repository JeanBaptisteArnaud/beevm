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

void KnownObjects::initialize(oop_t *aNil, oop_t *aTrue, oop_t *aFalse)
{
	KnownObjects::nil     = aNil;
	KnownObjects::stTrue  = aTrue;
	KnownObjects::stFalse = aFalse;
}

void KnownObjects::initializeFromHostVM()
{
	initialize(HOSTVM_NIL, HOSTVM_TRUE, HOSTVM_FALSE);
}




