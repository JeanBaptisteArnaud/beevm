/*
 * GCSpace.cpp
 *
 *  Created on: 18 déc. 2015
 *      Author: Arnaud Jean-Baptiste
 */

#include "GCSpace.h"

using namespace Bee;

GCSpace::GCSpace() {
}

GCSpace GCSpace::currentFrom() {
	return new GCSpace();
}

GCSpace GCSpace::currentTo() {
	return new GCSpace();
}
GCSpace GCSpace::old() {
	return new GCSpace();
}

void GCSpace::load(void) {
}
