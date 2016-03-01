/*
 * GCSpaceInfo.cpp
 *
 *  Created on: 4 janv. 2016
 *      Author: jbapt
 */

#include "GCSpaceInfo.h"
#include <cstdlib>

using namespace Bee;

#define INFO_SIZE 24

GCSpaceInfo::GCSpaceInfo()
{
	contents = (unsigned char *) malloc(INFO_SIZE);
	ownsContents = true;
}


GCSpaceInfo::GCSpaceInfo(ulong address, ulong size)
{
	this->contents = (unsigned char *) malloc(INFO_SIZE);
	ownsContents = true;

	this->sizeInBytes = INFO_SIZE;
	ulong *base = (ulong*)address;
	ulong *limit = (ulong*)(address+size);
	this->setBase(base);
	this->setNextFree(base);
	this->setCommitedLimit(limit);
	this->setSoftLimit(limit);
	this->setReservedLimit(limit);
}

GCSpaceInfo::~GCSpaceInfo()
{
	if (ownsContents)
		free(contents);
}

GCSpaceInfo GCSpaceInfo::withContents(uchar *contents)
{
	GCSpaceInfo result;
	result.setContents(contents);
	return result;
}

GCSpaceInfo GCSpaceInfo::newSized(ulong size)
{
	ulong align = size + 0xFFF;
	ulong address = (ulong)malloc(align);
	ulong final = (address + 0xFFF) & ~0xFFF;
	return GCSpaceInfo (final, size);
}

void GCSpaceInfo::setContents(uchar *value)
{
	if (ownsContents)
		free(contents);

	contents = value;
	ownsContents = false;
}




ulong * GCSpaceInfo::at(ulong offset)
{
	ulong* position = (ulong *)&contents[offset];
	return (ulong*)(position[0] | 1);
}

void GCSpaceInfo::atPut(ulong offset, ulong *value)
{
	ulong* position = (ulong *)&contents[offset];
	*position = (ulong)value & ~1;
}


ulong* GCSpaceInfo::getBase()
{
	return this->at(4);
}
ulong* GCSpaceInfo::getBase_3()
{
	return this->at(16);
}

ulong* GCSpaceInfo::getCommitedLimit()
{
	return this->at(12);
}

ulong* GCSpaceInfo::getNextFree()
{
	return this->at(8);
}

ulong* GCSpaceInfo::getReservedLimit()
{
	return this->at(20);
}

ulong* GCSpaceInfo::getSoftLimit()
{
	return this->at(0);
}

void GCSpaceInfo::setBase(ulong *value)
{
	this->atPut(4, value);
}

void GCSpaceInfo::setBase_3(ulong *value)
{
	this->atPut(16, value);
}
void GCSpaceInfo::setCommitedLimit(ulong *value)
{
	this->atPut(12, value);
}
void GCSpaceInfo::setNextFree(ulong *value)
{
	this->atPut(8, value);
}

void GCSpaceInfo::setReservedLimit(ulong *value)
{
	this->atPut(20, value);
}
void GCSpaceInfo::setSoftLimit(ulong* value)
{
	this->atPut(0, value);
}






