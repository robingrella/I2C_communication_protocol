#include "iicFunctions.h"

extern unsigned char errorNumber;

void writeError(unsigned char erno)
{
	errorNumber = erno;
}

void delay()
{
	volatile int i = 0;
	for(i;i<DELAY;i++);		// wait for device to write on
}
