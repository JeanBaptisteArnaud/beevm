/*
 * GCSpace.h
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#ifndef GCSPACE_H_
#define GCSPACE_H_

namespace Bee {
class GCSpace {
public:
	GCSpace();
	void load(void);
	static GCSpace currentFrom();
	static GCSpace currentTo();
	static GCSpace old();
	bool includes(unsigned long pointer);
	void loadFrom(GCSpace from);
	void reset();
	unsigned long shallowCopy(unsigned long pointer);
	unsigned long base();

};

}

#endif /* GCSPACE_H_ */
