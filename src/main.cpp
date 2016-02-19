/*
 * main.cpp
 *
 *  Created on: 17 de dic. de 2015
 *      Author: javier
 */

#include <cstddef>
#include <iostream>
#include "GarbageCollector.h"

//using namespace std;
using namespace Bee;

extern "C" {
void collect(int val) {
	GarbageCollector::currentFlipper()->collect();
}
}

