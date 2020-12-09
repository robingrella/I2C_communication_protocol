/*
 * iicFunctions.h
 *
 *  Created on: 16.04.2015
 *      Author: Robin
 */

#ifndef IICFUNCTIONS_H_
#define IICFUNCTIONS_H_

#include "bocas.h"

#define HIGH_16(x)	(x&0xFF00)>>8
#define LOW_16(x)	x&0x00FF
#define BITSET(x,y)	x|=y
#define BITCLR(x,y)	x&=~y
#define BITTEST(x,y) x&y

#define DELAY 20000
#define SIXTEENBIT 65535
#define ACK 0x01
#define EIGHTBYTES 0x08

#define MEMSIZE 256

#define DEVICEADDRESS 0xA0

struct nv_mem_control
{
	unsigned short nv_mem_address;	// address in nv memory
	char *buffer;					// data to be copied
	unsigned short no_of_bytes;		// number of bytes to be copied
};

struct iic_paket
{
	unsigned char dev_address;		// iic device address - 7 bit representation
	char *buffer;					// address of block to be transmitted
	unsigned char no_of_bytes;		// number of bytes in buffer to be transmitted
};

static unsigned char errorNumber = 0;

// layer 1
int iicSendBit(char);									// returns success 0, or error -1
														// parameter: bit to be transmitted at bit pos 0
int iicSendStartBit(void);								// returns success 0, or error -1
int iicSendStopBit(void);								// returns success 0, or error -1
int iicReceiveBit(char*);								// returns success 0, or error -1
														// parameter: bit received at pos 0
int iicInit(void);										// returns success 0, or error -1

// layer 2
int transmitByte(char);									// returns success 0, or error -1
														// parameter: byte to be send
int receiveByte(char*,char);							// returns success 0, or error -1
														// parameter: ptr to be received,
														// char tells slave if byte was last byte (1), or not (0)
int wrapperStart(void);									// returns success 0, or error -1
int wrapperStop(void);									// returns success 0, or error -1
int iicReceiveAck(char);								// returns success 0, or error -1
void writeError(unsigned char);
void delay(void);

// layer 3
int iicWriteTransaction(struct iic_paket*);		// returns success 0, or error -1
int iicReadTransaction(struct iic_paket*);		// returns success 0, or error -1

// layer 4
int nvMemInit(void);							// returns size of nv memory or error = 0xFFFF
int nvMemWrite(struct nv_mem_control*);			// returns success 0, or error -1
int nvMemRead(struct nv_mem_control*);			// returns success 0, or error -1

#endif /* IICFUNCTIONS_H_ */
