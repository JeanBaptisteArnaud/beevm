/*
 * GenerationalGC.h
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GARBAGECOLLECTOR_GENERATIONALGC_H_
#define GARBAGECOLLECTOR_GENERATIONALGC_H_

namespace Bee {
class GenerationalGC: public GarbageCollector {
public:
	GenerationalGC();
	void collect();

protected:
	bool hasToPurge(unsigned long pointer);
	bool arenaIncludes(unsigned long pointer);
	void moveToOldAll(ReferencedVMArray object);
	void followRememberSet();
	void purgeLiteralsReference();
	void purgeRememberSet();
	void followCodeCacheReferences();
	void moveClassCheckReferences();
	void followRoots();
	void cleanRememberSet();
	void addInterrupt();
	void fixReferencesFromNativeMethods();
	void flipSpaces();
	void updateSpacesDelta();
	unsigned long copyTo(unsigned long object, GCSpace to);
	unsigned long moveToOldSpace(unsigned long object);
	unsigned long holdReferenceTo(unsigned long object);
	void fixReferencesOrSetTombstone(unsigned long weakContainer);
};

extern "C" {
__declspec(dllexport) void collect(int val);
}

}

#endif /* GARBAGECOLLECTOR_GENERATIONALGC_H_ */
