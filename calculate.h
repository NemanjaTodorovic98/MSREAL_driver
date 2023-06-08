#ifndef CALCULATE_H
#define CALCULATE_H

#include "stdlib.h"
#include "stdio.h"

#define TRUE      (1u)
#define FALSE     (0u)

#define READ_ACCESS  ("r")
#define WRITE_ACCESS ("w")
#define WRITE ("a")

#define COMMAND_MESSAGE_LENGTH 8

#define BUFFERS_COUNT 8
#define WORDS_IN_BUFFER 1024

#define NUMBER_OF_BLOCKS_PER_IMAGE (961u)

#define GRAD_QANGLE_SIZE 8192
#define GRADOFS_SIZE     48
#define GRADWEIGHTS_SIZE 48
#define HISTOFS_SIZE     192
#define HISTWEIGHTS_SIZE 192

#define BLOCKHIST_SIZE 36

typedef unsigned char boolean;

typedef union
{
    unsigned int word;
    unsigned char byte[4];
}IntToBytes;

typedef struct
{
	unsigned int imgoffsetx;
	unsigned int imgoffsety;
	unsigned int rows;
	unsigned int cols;
	unsigned int ptx;
	unsigned int pty;
	
}BlockInfo;

#endif
