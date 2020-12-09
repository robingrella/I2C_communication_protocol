#include "bocas.h"
#include "iicFunctions.h"
/*
 * main.c
 */

#define READ 1
#define WRITE 0
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    P4SEL = 0x00;				// Digital IO
    P4OUT = 0x00;				// LED
    P4DIR = 0xFF;				// output

    /*unsigned char i;
	struct iic_paket test;
	char buffer[9];
	unsigned short j;
	volatile char test_byte;

	if (iicInit() == -1) P4OUT = 0xFF;
	test.dev_address = DEVICEADDRESS;
	test.buffer = buffer;
	test.no_of_bytes = 9;

	buffer[0] = 0x11;
	for (i=1; i<9; i++){
		buffer[i] = 0x01<<i-1;
	}
	if (iicWriteTransaction(&test)==-1)
	{
		delay();
		if (iicWriteTransaction(&test) == -1)
		{
			P4OUT = 0xFF;
			for (;;);
		}
	}

	test.no_of_bytes = 1;
	buffer[0] = 0x10;
	if (iicWriteTransaction(&test)==-1)
	{
		delay();
		if (iicWriteTransaction(&test) == -1)
		{
			P4OUT = 0xFF;
			for (;;);
		}
	}
	test.no_of_bytes = 3;

	test.buffer[2] = 0x00;

	if (iicReadTransaction (&test) == -1)
	{
		for (j=0; j<20000; j++);
		if (iicReadTransaction(&test) == -1)
		{
			P4OUT = 0xFF;
			for (;;);
		}
	}

	test_byte = test.buffer[2];

	for (;;);

	//return 0;*/

    int size;
    struct nv_mem_control mem_ptr;
    char buffer[20];
    char buffer2[8];
    int isError;
    volatile char test_byte;

    unsigned char i;

    size = nvMemInit();

    for(i=0; i<20; i++)
    {
    	buffer[i] = i*2;
    }

    mem_ptr.buffer = buffer;
    mem_ptr.no_of_bytes = 20;
    mem_ptr.nv_mem_address = 0x10;

    isError = nvMemWrite(&mem_ptr);

    for(i=0; i<8; i++)
    {
    	buffer2[i] = 0x00;
    }

    mem_ptr.buffer = buffer2;
    mem_ptr.nv_mem_address = 0x18;
    mem_ptr.no_of_bytes = 8;

    isError = nvMemRead(&mem_ptr);

    if(isError == -1)
    {
    	P4OUT = 0xFF;
    }

    test_byte = mem_ptr.buffer[5];

    for(;;);
}
