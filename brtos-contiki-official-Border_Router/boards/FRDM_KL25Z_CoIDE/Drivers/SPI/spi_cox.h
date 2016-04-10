/**
* \file SPI.h
* \brief Serial peripheral interface driver function prototypes.
*
*
**/

#include "MKL25Z4.h"
#include "xhw_types.h"
#include "xsysctl.h"

#define ENABLE_SPI0   TRUE
#define ENABLE_SPI1   TRUE

void init_SPI(unsigned char spi);


#if (ENABLE_SPI0 == TRUE)
void SPI0_SendChar(INT8U *data, int size);
void SPI0_GetChar(INT8U *data, int size);
#endif


#if (ENABLE_SPI1 == TRUE)
extern void SPI1_SendChar(unsigned char data);
extern unsigned char SPI1_GetChar(void);
#endif

#define SPI_Init() init_SPI(0)
