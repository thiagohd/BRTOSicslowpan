/*
 * Contiki SeedEye Platform project
 *
 * Copyright (c) 2012,
 *  Scuola Superiore Sant'Anna (http://www.sssup.it) and
 *  Consorzio Nazionale Interuniversitario per le Telecomunicazioni
 *  (http://www.cnit.it).
 *
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \addtogroup mrf24j40 MRF24J40 Driver
 *
 * @{
 */

/** 
 * \file   mrf24j40_arch.h
 * \brief  MRF24J40 Specific Arch Conf
 * \author Giovanni Pellerano <giovanni.pellerano@evilaliv3.org>
 * \date   2012-03-21
 */

#ifndef __MRF24J40_ARCH_H__
#define __MRF24J40_ARCH_H__



#include <stdint.h>
#include "xhw_types.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xhw_gpio.h"
#include "system.h"
#include "spi_cox.h"
#include "BRTOS.h"

#include "dev/radio.h"

/* Pin Mapping */
//#define MRF24J40_RESETn                     PORTGbits.RG15
//#define MRF24J40_INT                        PORTAbits.RA15
//#define MRF24J40_CSn                        PORTFbits.RF12
//#define MRF24J40_WAKE                       PORTGbits.RG12

#if PROCESSOR == ARM_Cortex_M0
#define MRF24J40_CS_AS_IO		GPIOPinConfigure(GPIO_PC4_PC4)								///< Defines CS pin as IO
#define MRF24J40_CS_DS			GPIOPadConfigSet(GPIOC_BASE, GPIO_PIN_4, PORT_TYPE_DSE_HIGH)///< Defines CS pin drive strength to high
#define MRF24J40_CS_LOW       	GPIOPinReset(GPIOC_BASE, GPIO_PIN_4)     					///< CS pin = 0
#define MRF24J40_CS_HIGH     	GPIOPinSet(GPIOC_BASE, GPIO_PIN_4)     						///< CS pin = 1
#define MRF24J40_CS_DIR_IN    	xGPIODirModeSet(GPIOC_BASE, GPIO_PIN_4, xGPIO_DIR_MODE_IN)	///< CS direction pin = in
#define MRF24J40_CS_DIR_OUT   	xGPIODirModeSet(GPIOC_BASE, GPIO_PIN_4, xGPIO_DIR_MODE_OUT)	///< CS direction pin = out
#define MRF24J40_RESETn_AS_IO	GPIOPinConfigure(GPIO_PC10_PC10)							///< Defines RESETn pin as IO
#define MRF24J40_RESETn_DS		GPIOPadConfigSet(GPIOC_BASE, GPIO_PIN_10, PORT_TYPE_DSE_HIGH)///< Defines RESETn pin drive strength to high
#define MRF24J40_RESETn_LOW   	GPIOPinReset(GPIOC_BASE, GPIO_PIN_10)     					///< RESET pin = 0
#define MRF24J40_RESETn_HIGH  	GPIOPinSet(GPIOC_BASE, GPIO_PIN_10)     					///< RESET pin = 1
#define MRF24J40_RESETn_DIR_IN  	xGPIODirModeSet(GPIOC_BASE, GPIO_PIN_10, xGPIO_DIR_MODE_IN) ///< RESET direction pin = in
#define MRF24J40_RESETn_DIR_OUT  xGPIODirModeSet(GPIOC_BASE, GPIO_PIN_10, xGPIO_DIR_MODE_OUT)///< RESET direction pin = out
#define MRF24J40_WAKE_AS_IO		GPIOPinConfigure(GPIO_PC11_PC11)							///< Defines WAKE pin as IO
#define MRF24J40_WAKE_DS			GPIOPadConfigSet(GPIOC_BASE, GPIO_PIN_11, PORT_TYPE_DSE_HIGH)///< Defines WAKE pin drive strength to high
#define MRF24J40_WAKE_LOW     	GPIOPinReset(GPIOC_BASE, GPIO_PIN_11)     					///< WAKE pin = 0
#define MRF24J40_WAKE_HIGH    	GPIOPinSet(GPIOC_BASE, GPIO_PIN_11)     					///< WAKE pin = 1
#define MRF24J40_WAKE_DIR_IN    	xGPIODirModeSet(GPIOC_BASE, GPIO_PIN_11, xGPIO_DIR_MODE_IN) ///< WAKE direction pin = in
#define MRF24J40_WAKE_DIR_OUT    xGPIODirModeSet(GPIOC_BASE, GPIO_PIN_11, xGPIO_DIR_MODE_OUT)///< WAKE direction pin = out
#define MRF24J40_PIN_CLOCK_INIT	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC)					///< Init PHY pins clock

#endif

/* Pin Tri-States */
//#define MRF24J40_TRIS_RESETn                TRISGbits.TRISG1
//#define MRF24J40_TRIS_INT                   TRISAbits.TRISA15
//#define MRF24J40_TRIS_CSn                   TRISFbits.TRISF12
//#define MRF24J40_TRIS_WAKE                  TRISGbits.TRISG12

/* RESET low/high */
#define MRF24J40_HARDRESET_LOW()            GPIOPinReset(GPIOC_BASE, GPIO_PIN_10)     					///< RESET pin = 0
#define MRF24J40_HARDRESET_HIGH()           GPIOPinSet(GPIOC_BASE, GPIO_PIN_10)     					///< RESET pin = 1
#define MRF24J40_CSn_LOW()                  GPIOPinReset(GPIOC_BASE, GPIO_PIN_4)     					///< CS pin = 0
#define MRF24J40_CSn_HIGH()                 GPIOPinSet(GPIOC_BASE, GPIO_PIN_4)     						///< CS pin = 1

/* Spi port Mapping */
#define MRF24J40_SPI_PORT_INIT()  			init_SPI(0)
#define MRF24J40_SPI_PORT_WRITE 			SPI0_SendChar
#define MRF24J40_SPI_PORT_READ  			SPI0_GetChar

#define MRF24J40_INT_ENABLE()				int_status = 1;			\
											Enable_INT_Pin()
#define MRF24J40_INTERRUPT_FLAG_CLR()       xHWREG(PORTD_BASE+PORT_ISFR) |= xHWREG(PORTD_BASE+PORT_ISFR)  // Limpa flag da interrupção externa
#define MRF24J40_INTERRUPT_ENABLE_CLR()		int_status = 0;			\
											GPIOPinIntDisable(GPIOD_BASE, GPIO_PIN_4)  					// desabilita interrupção externa
#define MRF24J40_INTERRUPT_ENABLE_SET()		int_status = 1;			\
											GPIOPinIntEnable(GPIOD_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE)   // habilita interrupção externa


#define MRF24J40_PIN_INIT()        				\
do {                                        	\
   MRF24J40_PIN_CLOCK_INIT;						\
   MRF24J40_CS_AS_IO;							\
   MRF24J40_RESETn_AS_IO;						\
   MRF24J40_WAKE_AS_IO;							\
   MRF24J40_CS_DS;								\
   MRF24J40_RESETn_DS;							\
   MRF24J40_WAKE_DS;							\
   MRF24J40_RESETn_LOW;							\
   MRF24J40_RESETn_DIR_OUT;						\
   MRF24J40_WAKE_LOW;							\
   MRF24J40_WAKE_DIR_OUT;						\
   MRF24J40_CS_HIGH;							\
   MRF24J40_CS_DIR_OUT;							\
   MRF24J40_INT_ENABLE();						\
   MRF24J40_WAKE_HIGH;							\
} while(0)



#endif /* __MRF24J40_ARCH_H__ */

/** @} */
