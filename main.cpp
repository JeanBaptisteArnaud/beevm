/*
 * main.cpp
 *
 *  Created on: 17 de dic. de 2015
 *      Author: javier
 */

#include "main.h"

#include <cstddef>
#include <iostream>

using namespace std;
using namespace Bee;


GarbageCollector* GarbageCollector::flipper = NULL;


GarbageCollector::GarbageCollector()
{

}

GarbageCollector* GarbageCollector::currentFlipper()
{
	if (flipper == NULL)
		flipper = new GarbageCollector();
	return flipper;
}

void GarbageCollector::collect()
{
	cout << "recolectando!" << endl;
}

extern "C" {
void collect(int val)
{
	GarbageCollector::currentFlipper()->collect();
}
}

