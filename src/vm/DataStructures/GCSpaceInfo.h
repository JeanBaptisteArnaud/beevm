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
	~GCSpaceInfo();

	GCSpaceInfo(ulong address, ulong size);

	static GCSpaceInfo newSized(ulong size);

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


//protected:
	uchar *contents;
	ulong sizeInBytes;

	bool ownsContents;

};

}

#endif /* GCSPACEINFO_H_ */
