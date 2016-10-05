#include "strlib.h"
#include "nos_common.h"

char *itoa(INT32 value, char *string, UINT8 radix)
{
      INT8 tmp[33];
      INT8 *tp = tmp;
      INT32 i;
      UINT32 v;
      INT32 sign;
      char *sp;

      if (radix > 36 || radix <= 1)
      {
            return 0;
      }

      sign = (radix == 10 && value < 0);

      if (sign) v = -value;
      else  v = (UINT32)value;

      while (v || tp == tmp)
      {
            i = v % radix;
            v = v / radix;
            if (i < 10)
                *tp++ = i+'0';
            else
                *tp++ = i + 'a' - 10;
      }
 

      sp = string;

      if (sign) *sp++ = '-';

      while (tp > tmp){
	  		*sp++ = *--tp;
      	}

      *sp = 0;

      return string;
}

//There isn't utoa function in the library.
char *utoa(UINT32 value, char *digits, INT8 base)
{

    char *p;
    const char *s = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (base == 0)
        base = 10;
    if (digits == NULL || base < 2 || base > 36)
        return NULL;

    if (value < (INT8) base) {
        digits[0] = s[value];
        digits[1] = '\0';
    } else {
        for (p = utoa(value / ((INT8)base), digits, base);
             *p;
             p++);
        utoa( value % ((INT8)base), p, base);
        
    }

    return digits;
}

// 234.123456; %.4f niter = 4; result = 234.1234
char *ftoa(double Value, char* Buffer, UINT8 niter)
{
     UINT8 i = 0;
     UINT8 count = 0;
     UINT32 IntValue; 
     UINT32 FracPart;
     char buf1[10], buf2[10];
     double Frac;

     if (Value > 0)
     {
     	IntValue = (UINT32)Value;
	Frac = Value - (double) IntValue;
	for (; i<niter; i++)
		Frac = 10.0*Frac;

	FracPart = (UINT32) Frac;
     }
     else if (Value < 0)
     {
     	IntValue = (UINT32) (-Value);
	Frac = -Value - (double) IntValue;
	for (; i<niter; i++)
		Frac = 10.0*Frac;
	FracPart = (UINT32) Frac;
     }
     else
     {
     	Buffer[0] = '0';
     	Buffer[1] = '.';
     	Buffer[2] = '0';
     	Buffer[3] = '\0';
	return Buffer;
     }
    
     utoa(IntValue, buf1, 10);
     utoa(FracPart, buf2, 10);

     if (Value < 0)
     {
        Buffer[count++] = '-';
     }

     i = 0;
     while (buf1[i] != '\0')
     {
	 Buffer[count++] = buf1[i++];
     }

     if (niter != 0)
     {
     	Buffer[count++] = '.';

     	i = 0;
     	while (buf2[i] != '\0')
     	{
		 Buffer[count++] = buf2[i++];
     	}
     }

     //make sure the output is terminated
     Buffer[count] = '\0';

//uart_printf("Int = %d  frac = %d %s %s Buffer = %s \n", IntValue, FracPart, buf1, buf2, Buffer);
     return Buffer;
}
