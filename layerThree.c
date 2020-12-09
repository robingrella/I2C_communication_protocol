#include "iicFunctions.h"

#define WRITE 0
#define READ 1
#define LAST 1
#define NOTLAST 0

int iicWriteTransaction(struct iic_paket *w_ptr)
{
	int isError = 0;								// error layer 2 = -1, success 0
	int i;

	char w_address = w_ptr->dev_address;			// saves device address in w_address
													// last bit will be 1
	BITSET(w_address, WRITE);						// write operation

	isError = wrapperStart();
	if(isError == -1)
	{
		// errorno in layer 1
		return -1;
	}

	isError = transmitByte(w_address);				// send byte with address of device, set W/R to W
	if(isError == -1)
	{
		return -1;
	}
	isError = iicReceiveAck(!ACK);
	if(isError == -1)
	{
		return -1;
	}

	delay();

	// for every byte
	for(i=0;i<w_ptr->no_of_bytes;i++)
	{
		isError = transmitByte(w_ptr->buffer[i]);		// send data byte to device
		if(isError == -1)
		{
			return -1;
		}									// wait
		delay();
		isError = iicReceiveAck(!ACK);						// ack for data
		if(isError == -1)
		{
			return -1;
		}
	}

	isError = wrapperStop();							// stop bit to end transmission
	if(isError == -1)
	{
		// errorno in layer 1
		return -1;
	}
	delay();

	return 0;		// success
}

int iicReadTransaction(struct iic_paket* r_ptr)
{
	int isError = 0;												// error layer 2 = -1, success 0

	isError = wrapperStart();
	if(isError == -1)
	{
		return -1;
	}

	isError = transmitByte(r_ptr->dev_address+READ);				// transmit adress of device, set W/R to R
	if(isError == -1)
	{
		return -1;
	}

	delay();

	isError = iicReceiveAck(!ACK);
	if(isError == -1)
	{
		return -1;
	}

	// for every byte
	int i;
	for(i = 0;i < r_ptr->no_of_bytes;i++)
	{
		if(i < r_ptr->no_of_bytes - 1)
		{
			isError = receiveByte(&r_ptr->buffer[i], NOTLAST);		// receive data byte from device
		}
		else
		{
			isError = receiveByte(&r_ptr->buffer[i], LAST);			// last byte to receive
		}
		if(isError == -1)
		{
			return -1;
		}
	}

	isError = wrapperStop();					// stop bit to end transmission
	if(isError == -1)
	{
		// errorno in layer 1
		return -1;
	}

	return 0;
}
