#include "iicFunctions.h"

int nvMemInit()
{
	int isError = 0;
	isError = iicInit();

	if(isError == -1)
	{
		return isError;
	}

	return MEMSIZE;
}

int nvMemWrite(struct nv_mem_control *m_ptr)
{
	struct iic_paket paketPtr;

	char bytesToTransmit[9];					// memory address + 8 bytes (1 block of bytes)

	int isError = 0;

	char i, k, l, no_of_blocks, no_of_bytes_left, pos_in_buffer;
	char send_max;
	no_of_blocks = 0;

	paketPtr.dev_address = DEVICEADDRESS;
	paketPtr.buffer = bytesToTransmit;

	bytesToTransmit[0] = m_ptr->nv_mem_address;	// first byte to send is always the memory address of nv device

	i = m_ptr->no_of_bytes;
	if(i <= 0)									// i is no_of_bytes
	{
		return 0;								// nothing to send
	}

	while(i > EIGHTBYTES)						// i is index for a number of bytes for one block
	{											// -> it has to be lower than 8
		i = i - EIGHTBYTES;						// if there is more than 8 bytes in the index, substract with 8
		no_of_blocks++;							// one block for every 8 bytes
	}											// no_of_blocks == 0 means 8 or less bytes

	i = m_ptr->nv_mem_address;					// i is now memory address of nv device

	/* since the memory of the nv device is stored in blocks of 8 bytes,
	 * we need an index for the address inside one of the blocks
	 * i == 0 means nv_mem_address % 8 == 0									 */
	while(i >= EIGHTBYTES)
	{
		i = i - EIGHTBYTES;
	}

	/* i == nv_mem_address % 8
	 * i is byte address starting at block(!) address 0x00
	 * j is position of buffer array in m_ptr				 */
	pos_in_buffer = 0x00;
	for(k=0x00; k < EIGHTBYTES-i; k++)
	{
		bytesToTransmit[k+1] = m_ptr->buffer[pos_in_buffer];			// k + 1 <- memory address in index 0
		pos_in_buffer++;												// position in buffer array of overall data to send
	}

	paketPtr.no_of_bytes = pos_in_buffer+1;								// one byte more to send because of nv_mem_address
	isError = iicWriteTransaction(&paketPtr);
	if(isError == -1)
	{
		return -1;
	}

	if(no_of_blocks > 0)
	{
		no_of_bytes_left = m_ptr->no_of_bytes - (EIGHTBYTES - i);		// substract number of bytes already sent
		bytesToTransmit[0] = bytesToTransmit[0] - i;					// nv device mem address % 8
		for(k=0x00; k < no_of_blocks; k++)								// for every block of bytes
		{
			send_max = 0x08;
			if(no_of_bytes_left < EIGHTBYTES)
			{
				send_max = no_of_bytes_left;
			}
			bytesToTransmit[0] = bytesToTransmit[0] + EIGHTBYTES;		// go one block further

			for(l=0; l < send_max; l++)									// for maximum 8 bytes
			{
				bytesToTransmit[l+1] = m_ptr->buffer[pos_in_buffer];	// l + 1 <- memory address in index 0
				pos_in_buffer++;										// position in buffer array of overall data to send
			}

			paketPtr.no_of_bytes = send_max + 1;						// number of bytes from 1 to 9
			no_of_bytes_left = no_of_bytes_left - send_max;				// substract number of now sent bytes

			isError = iicWriteTransaction(&paketPtr);					// send block
		}
	}

	return isError;														// return 0 success, or -1 error

}

int nvMemRead(struct nv_mem_control *m_ptr)
{
	struct iic_paket paketPtr;
	char mem_address_send = m_ptr->nv_mem_address;

	paketPtr.dev_address = DEVICEADDRESS;					// address of device
	paketPtr.buffer = &mem_address_send;					// memory address
	paketPtr.no_of_bytes = 1;								// one byte for the memory address

	int isError = 0;

	isError = iicWriteTransaction(&paketPtr);

	if(isError == -1)
	{
		return -1;
	}

	paketPtr.buffer = m_ptr->buffer;						// address to save data at copied for paket instance

	paketPtr.no_of_bytes = m_ptr->no_of_bytes;

	isError = iicReadTransaction(&paketPtr);

	return isError;
}
