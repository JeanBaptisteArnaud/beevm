/*
 * main.h
 *
 *  Created on: 17 de dic. de 2015
 *      Author: javier
 */

#ifndef MAIN_H_
#define MAIN_H_

namespace Bee
{
    class GarbageCollector
    {
    public:
        GarbageCollector();

        static GarbageCollector* currentFlipper();

        void collect();

    private:
        static GarbageCollector *flipper;

    };

	extern "C" {
		__declspec(dllexport) void collect(int val);
	}

}



#endif /* MAIN_H_ */
