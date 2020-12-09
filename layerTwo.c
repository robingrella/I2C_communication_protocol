#include "iicFunctions.h"

int transmitByte(char t_byte)
{
	volatile unsigned char mask = 0x80;		// mask for bittest
	int isError = 0;				// error layer 1 = -1, success = 0
	signed char i;
	for(i=7;i>=0;i--)
	{
		if(BITTEST(mask, t_byte))
		{
			isError = iicSendBit(1);
		}
		else
		{
			isError = iicSendBit(0);
		}

		mask = mask >> 1;  // right- shift

		if(isError==-1)
		{
			return -1;
		}
	}
	// success
	return 0;
}

int receiveByte(char *r_byte, char last)
{
	int isError = 0;
	char r_buffer = 0x00;
	signed char i;
	for(i=7;i>=0;i--)
	{
		int isError = iicReceiveBit(&r_buffer);		// error layer 1 = -1, success = 0
		if(isError==-1)
		{
			return -1;		// Error
		}
		*r_byte = *r_byte | (r_buffer << i );		// ptr pos + 1
	}

	isError = iicSendBit(last);
	if(isError==-1)
	{
		return -1;		// Error
	}
	return 0;			// success
}

int wrapperStart(void)
{
	return iicSendStartBit();		// wrapper start bit function for layer 3
}

int wrapperStop(void)
{
	return iicSendStopBit();		// wrapper stop bit function for layer 3
}

int iicReceiveAck(char ack)
{
	int isError = 0;
	char ackBit;
	ackBit = 0x00;									// ack is either 1 or 0, depending on device and bus and function

	isError = iicReceiveBit(&ackBit);
	if(isError == 0)
	{
		if(ackBit == ack)
		{
			return 0;								// if receivece ackBit is wanted ack, return success
		}
	}
	return -1;										// slave took too long to give ack
}


