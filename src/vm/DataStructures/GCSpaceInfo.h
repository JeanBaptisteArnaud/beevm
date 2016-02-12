/*
 * GCSpaceInfo.h
 *
 *  Created on: 4 janv. 2016
 *      Author: jbapt
 */

#ifndef GCSPACEINFO_H_
#define GCSPACEINFO_H_

#include "Bee.h"

namespace Bee
{

class GCSpaceInfo
{
public:
	GCSpaceInfo();
	GCSpaceInfo(ulong *address, ulong size);

	unsigned short shortAt(ulong);
	ulong * at(ulong);
	void atPut(ulong,ulong *);

	ulong * getBase();
	void setBase( ulong * );
	ulong * getBase_3();
	void setBase_3(ulong * );
	ulong * getCommitedLimit();
	void setCommitedLimit(ulong * );
	void setContents(ulong *);
	ulong * getNextFree();
	void setNextFree(ulong *);
	ulong * getReservedLimit();
	void setReservedLimit(ulong * );
	ulong * getSoftLimit();
	void setSoftLimit(ulong *);
	void freeMock();


	static GCSpaceInfo newSized(ulong size);
	static GCSpaceInfo currentTo();
	static GCSpaceInfo currentFrom();
	static GCSpaceInfo old();

//protected:
	unsigned char *contents;
	ulong sizeInBytes;

};

}

#endif /* GCSPACEINFO_H_ */
