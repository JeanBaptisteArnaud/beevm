/*
 * GCSpaceInfo.cpp
 *
 *  Created on: 4 janv. 2016
 *      Author: jbapt
 */

#include "GCSpaceInfo.h"
#include "stdlib.h"

using namespace Bee;

#define INFO_SIZE 24

GCSpaceInfo::GCSpaceInfo(){
	contents = (unsigned char *) malloc(INFO_SIZE);
}

GCSpaceInfo::GCSpaceInfo(ulong *address, ulong size)
{
	this->contents = (unsigned char *) malloc(INFO_SIZE);
	this->sizeInBytes = INFO_SIZE;

	this->setBase(address);
	this->setNextFree(address);
	this->setCommitedLimit(address+size);
	this->setSoftLimit(address+size);
	this->setReservedLimit(address+size);
}


unsigned short GCSpaceInfo::shortAt(ulong offset) {
	return (contents[offset]);
}

ulong * GCSpaceInfo::at(ulong offset) {
	ulong* position = (ulong *)&contents[offset];
	return (ulong*)((*position)>>1);
}

void GCSpaceInfo::atPut(ulong offset, ulong * value) {
	ulong* position = (ulong *)&contents[offset];
	*position = ((ulong)value)<<1;
}


//ulong * GCSpaceInfo::at(ulong offset) {
//	return (ulong *)(((contents[offset] / 2))
//			+ ((contents[offset + 1] << 15)));
//}
//
//void GCSpaceInfo::atPut(ulong offset, ulong * pointer) {
//	ulong vPointer = (ulong) pointer;
//	contents[offset] = (unsigned short) ((vPointer & 0x7FFF) * 2);
//	contents[offset + 1] = (unsigned short)(vPointer >> 15);
//}

ulong * GCSpaceInfo::getBase() {
	return this->at(4);
}
ulong * GCSpaceInfo::getBase_3() {
	return this->at(16);
}
ulong * GCSpaceInfo::getCommitedLimit() {
	return this->at(12);
}
ulong * GCSpaceInfo::getNextFree() {
	return this->at(4);
}
ulong * GCSpaceInfo::getReservedLimit() {
	return this->at(20);
}
ulong * GCSpaceInfo::getSoftLimit() {
	return this->at(0);
}

void GCSpaceInfo::setBase(ulong * value) {
	this->atPut(4, value);
}

void GCSpaceInfo::setBase_3(ulong * value) {
	this->atPut(16, value);
}
void GCSpaceInfo::setCommitedLimit(ulong * value) {
	this->atPut(12, value);
}
void GCSpaceInfo::setNextFree(ulong * value) {
	this->atPut(8, value);
}

void GCSpaceInfo::setReservedLimit(ulong * value) {
	this->atPut(20, value);
}
void GCSpaceInfo::setSoftLimit(ulong* value) {
	this->atPut(0, value);
}

void GCSpaceInfo::setContents(ulong * value){
	contents = (unsigned char *) value;
}

void GCSpaceInfo::freeMock(){
	free(contents);

}

GCSpaceInfo GCSpaceInfo::newSized(ulong size)
{
	ulong align = size + 0xFFF;
	ulong address = (ulong)malloc(align);
	ulong final = (address + 0xFFF) & ~0xFFF;
	return GCSpaceInfo ((ulong *)final, size);
}

GCSpaceInfo GCSpaceInfo::currentTo()
{
	GCSpaceInfo returnValue = GCSpaceInfo();
	returnValue.setContents((ulong *) 0x100416C8);
	return returnValue;
}

GCSpaceInfo GCSpaceInfo::currentFrom()
{
	GCSpaceInfo returnValue = GCSpaceInfo();
	returnValue.setContents((ulong *) 0x100416B0);
	return returnValue;
}

GCSpaceInfo GCSpaceInfo::old()
{
	GCSpaceInfo returnValue = GCSpaceInfo();
	returnValue.setContents((ulong *) 0x100406B0);
	return returnValue;
}





