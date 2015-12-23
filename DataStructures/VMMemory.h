/*
 * Globales.h
 *
 *  Created on: 18 d�c. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef DATASTRUCTURES_VMMEMORY_H_
#define DATASTRUCTURES_VMMEMORY_H_

// this should manage all the raw access memory

#define NULL 0

#define  _strongPointersSize(pointer) 0

unsigned long *debugFrameMarker = (unsigned long *)0x1001B633;
unsigned long *MEM0x1003EC48    = (unsigned long *)0x1003EC48;
unsigned long *MEM_anyCompiledMethodInFromSpace     = (unsigned long *)0x10041710;
unsigned long *MEM_checkNewNativizedCompiledMethods = (unsigned long *)0x10041714;
unsigned long **MEM_JIT_globalMethodCache = (unsigned long **)0x1002EC20;
unsigned long *MEM_JIT_codeCacheData      = (unsigned long *)0x1002E820;
unsigned long *nil                        = (unsigned long*)0x10026060;
unsigned long *stTrue                     = (unsigned long*)0x10026070;
unsigned long *stFalse                    = (unsigned long*)0x10026080;

unsigned long memoryAt(unsigned long pointer);
void memoryAtPut(unsigned long pointer, unsigned long value);

bool isProxy(unsigned long pointer);
bool proxee(unsigned long pointer);

unsigned long basicAt(unsigned long object, int index);
unsigned long basicAtPut(unsigned long object, int index, unsigned long value);

int size(unsigned long object);
unsigned long framePointer();

#endif /* DATASTRUCTURES_VMMEMORY_H_ */
