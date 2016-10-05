#ifndef HARDWARE_H
#define HARDWARE_H

#define UData(Data)	((unsigned long) (Data))

#define __REG(x)	(*(vu_long *)(x))
#define __REGl(x)	(*(vu_long *)(x))
#define __REGw(x)	(*(vu_short *)(x))
#define __REGb(x)	(*(vu_char *)(x))
#define __REG2(x,y)	(*(vu_long *)((x) + (y)))

#define F1stBit(Field)	(UData (1) << FShft (Field))

#define FClrBit(Data, Bit)	(Data = (Data & ~(Bit)))

#endif
