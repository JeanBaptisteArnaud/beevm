#ifndef _BEE_H_
#define _BEE_H_

//#define NULL 0

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

char* strcpy_s(char* dst, int size, const char *src);

namespace Bee {

struct basic_header_t;
struct extended_header_t;
struct oop_t;

typedef oop_t* slot_t;


ulong* _commit(ulong limit, ulong delta);
void _decommit(ulong *limit, ulong *delta);
void _free(ulong * limit, ulong *delta);


void getOsErrorMessage(ulong code, char buffer[], int buffer_length);
void osError();
void error(const char *message);
void debug(const char *message);

}

#endif // _BEE_H_
