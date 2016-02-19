#ifndef _BEE_H_
#define _BEE_H_

#define NULL 0

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

namespace Bee {

struct basic_header_t;
struct extended_header_t;
struct oop_t;

typedef oop_t* slot_t;


//oop_t* Array() { }


void getOsErrorMessage(ulong code, char buffer[], int buffer_length);
void osError();
void error(char *message);

}

#endif // _BEE_H_