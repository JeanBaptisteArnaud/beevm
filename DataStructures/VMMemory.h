/*
 * Globales.h
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef DATASTRUCTURES_VMMEMORY_H_
#define DATASTRUCTURES_VMMEMORY_H_

// this should manage all the raw access memory

#define NULL 0
#define nil 0

#define  _strongPointersSize(pointer) 0

unsigned long debugFrameMarker = 0x1001B633;
unsigned long MEM0x1003EC48 = 0;
unsigned long MEM0x10041710 = 0;
unsigned long MEM0x10041714 = 0;
unsigned long nil = 0;

unsigned long memoryAt(unsigned long pointer);

#endif /* DATASTRUCTURES_VMMEMORY_H_ */
