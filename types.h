#ifndef MEMORY_RESOURCES_H
#define MEMORY_RESOURCES_H

// BRAM memory sizes
#define GRAD_MEMORY_SIZE        (8192u)
#define QANGLE_MEMORY_SIZE      (8192u)
#define GRADOFS_MEMORY_SIZE     (48u)
#define GRADWEIGHTS_MEMORY_SIZE (48u)
#define HISTOFS_MEMORY_SIZE     (192u)
#define HISTWEIGHTS_MEMORY_SIZE (192u)
#define BLOCKHIST_MEMORY_SIZE   (36u)

// IP reg 
#define COUNT1        		(4u)
#define COUNT2        		(12u)
#define COUNT4        		(16u)
#define IMGOFFSETX        	(0u)
#define IMGOFFSETY        	(0u)
#define ROWS       		(64u)
#define COLS        		(64u)
#define PT_X        		(0u)
#define PT_Y        		(0u)

#define TRUE      (1u)
#define FALSE     (0u)
 
#define INTERNAL_BUFF_SIZE (4096u)
#define RESPONSE_BUFF_SIZE (144u)

typedef unsigned char boolean;
typedef unsigned int uint32;

typedef enum
{
	IPcore = 0,
	BramA  = 1,
	BramB,
	BramC,
	BramD,
	BramF,
	BramG,
	BramH,
	NumberOfUnits
}DeviceUnit;

typedef union
{
    unsigned int word; //32bita
    unsigned char byte[4]; //1 bajt po clanu niza znaci 32 bita
    
}IntToBytes;

#endif
