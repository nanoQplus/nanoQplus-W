
#ifndef STRLIB_H
#define STRLIB_H

#include "typedef.h"

char *itoa(INT32 value, char *string, UINT8 radix);
char *utoa(UINT32 value, char *digits, INT8 base);
char *ftoa(double f, char *outbuf, UINT8 niter);

#endif

