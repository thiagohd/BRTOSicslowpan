#include "BRTOS.h"
#include "UART.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xwdt.h"
#include "xhw_memmap.h"
#include "xhw_uart.h"
#include "xuart.h"
#include "xcore.h"
//#include "MKL25Z4.h"


BRTOS_Sem   *SerialTX0;
// Declara uma estrutura de fila
OS_QUEUE SerialPortBuffer0;
// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOS_Queue  *Serial0;


BRTOS_Sem   *SerialTX1;
// Declara uma estrutura de fila
OS_QUEUE SerialPortBuffer1;
// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOS_Queue  *Serial1;

BRTOS_Sem   *SerialTX2;
// Declara uma estrutura de fila
OS_QUEUE SerialPortBuffer2;
// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOS_Queue  *Serial2;

#include "slip.h"
extern BRTOS_Sem *Contiki_Sem;

unsigned char buffer_rcvd[UIP_CONF_BUFFER_SIZE];
unsigned char buffer_rcvd_i = 0;

unsigned long USART0IntHandler(void *pvCBData,
        unsigned long ulEvent,
        unsigned long ulMsgParam,
        void *pvMsgData)
{
	char receive_byte;

	if ((ulEvent & UART_EVENT_RX) == UART_EVENT_RX)
	{
		receive_byte = xHWREGB(UART0_BASE + UART_012_D);

		if (slip_input_byte(receive_byte) == 1) OSSemPost(Contiki_Sem);

		buffer_rcvd[buffer_rcvd_i++]=receive_byte;
		//slip_input_byte(receive_byte);
		//OSSemPost(Contiki_Sem);

#if 0
		if (OSQueuePost(Serial0, receive_byte) == BUFFER_UNDERRUN)
		{
			// Problema: Estouro de buffer
			OSCleanQueue(Serial0);
		}
#endif
	}

	if ((ulEvent & UART_EVENT_TC) == UART_EVENT_TC)
	{
		if ((xHWREGB(UART0_BASE + UART_012_C2) & UART_EVENT_TC) == UART_EVENT_TC)
		{
			UARTIntDisable(UART0_BASE, UART_INT_TC);
			(void)OSSemPost(SerialTX0);
		}
	}

	// ************************
	// Interrupt Exit
	// ************************
	OS_INT_EXIT_EXT();
	// ************************

	return 0;
}

unsigned long USART1IntHandler(void *pvCBData,
        unsigned long ulEvent,
        unsigned long ulMsgParam,
        void *pvMsgData)
{
	char receive_byte;

	if ((ulEvent & UART_EVENT_RX) == UART_EVENT_RX)
	{
		receive_byte = xHWREGB(UART1_BASE + UART_012_D);

		if (OSQueuePost(Serial1, receive_byte) == BUFFER_UNDERRUN)
		{
			// Problema: Estouro de buffer
			//OSCleanQueue(Serial0);
		}
	}

	if ((ulEvent & UART_EVENT_TC) == UART_EVENT_TC)
	{
		if ((xHWREGB(UART1_BASE + UART_012_C2) & UART_EVENT_TC) == UART_EVENT_TC)
		{
			UARTIntDisable(UART1_BASE, UART_INT_TC);
			(void)OSSemPost(SerialTX1);
		}
	}

	// ************************
	// Interrupt Exit
	// ************************
	OS_INT_EXIT_EXT();
	// ************************

	return 0;
}

unsigned long USART2IntHandler(void *pvCBData,
        unsigned long ulEvent,
        unsigned long ulMsgParam,
        void *pvMsgData)
{
	char receive_byte;

	if ((ulEvent & UART_EVENT_RX) == UART_EVENT_RX)
	{
		receive_byte = xHWREGB(UART2_BASE + UART_012_D);

		if (OSQueuePost(Serial2, receive_byte) == BUFFER_UNDERRUN)
		{
			// Problema: Estouro de buffer
			//OSCleanQueue(Serial0);
		}
	}

	if ((ulEvent & UART_EVENT_TC) == UART_EVENT_TC)
	{
		if ((xHWREGB(UART2_BASE + UART_012_C2) & UART_EVENT_TC) == UART_EVENT_TC)
		{
			UARTIntDisable(UART2_BASE, UART_INT_TC);
			(void)OSSemPost(SerialTX2);
		}
	}

	// ************************
	// Interrupt Exit
	// ************************
	OS_INT_EXIT_EXT();
	// ************************

	return 0;
}


xtBoolean UARTPutString(unsigned long ulBase, char *string)
{
	INT8U result = 0;

	while(*string)
	{
		xHWREGB(ulBase + UART_012_D) = *string;
		UARTIntEnable(ulBase, UART_INT_TC);
		if (ulBase == UART0_BASE)
		{
			result = OSSemPend(SerialTX0,5);
		}
		if (ulBase == UART1_BASE)
		{
			result = OSSemPend(SerialTX1,5);
		}
		if (ulBase == UART2_BASE)
		{
			result = OSSemPend(SerialTX2,5);
		}
	    string++;
	}

	return result;
}

xtBoolean UARTPutChar(unsigned long ulBase, char data)
{
	INT8U result = 0;

	xHWREGB(ulBase + UART_012_D) = data;
	UARTIntEnable(ulBase, UART_INT_TC);
	if (ulBase == UART0_BASE)
	{
		result = OSSemPend(SerialTX0,5);
	}
	if (ulBase == UART1_BASE)
	{
		result = OSSemPend(SerialTX1,5);
	}
	if (ulBase == UART2_BASE)
	{
		result = OSSemPend(SerialTX2,5);
	}
	return result;
}

unsigned char UARTGetChar(unsigned long ulBase, unsigned char *data, int timeout)
{
	unsigned char ret=0;
	if (ulBase == UART0_BASE)
	{
		ret = OSQueuePend(Serial0, data, timeout);
	}

	if (ulBase == UART1_BASE)
	{
		ret = OSQueuePend(Serial1, data, timeout);
	}
	if (ulBase == UART2_BASE)
	{
		ret = OSQueuePend(Serial2, data, timeout);
	}

	return ret;
}


void Init_UART0(int baud, int buffer_size)
{
	xtEventCallback UART0_INT_HANDLE = USART0IntHandler;

	// Enable GPIO and UART Clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Remap UART pin to GPIO Port UART0_RX --> PA2 UART0_TX --> PA1
	xSPinTypeUART(UART0RX, PA1);
	xSPinTypeUART(UART0TX, PA2);

	// Set UART clock
	SysCtlPeripheralClockSourceSet(SYSCTL_PERIPH_UART0_S_MCGPLLCLK_2);

	// Disable UART Receive/Transmit
	UARTDisable(UART0_BASE, UART_TX | UART_RX);

	// Configure UART Baud 115200
	UARTConfigSet(UART0_BASE, baud, UART_CONFIG_SAMPLE_RATE_DEFAULT | UART_CONFIG_WLEN_9 | UART_CONFIG_PAR_EVEN | UART_CONFIG_STOP_1);

	if (OSSemCreate(0, &SerialTX0) != ALLOC_EVENT_OK)
	{
	  // Oh Oh
	  // Não deveria entrar aqui !!!
	  while(1){};
	};

	// Só cria fila se for passado um tamanho maior que 0
	if (buffer_size){
		if (OSQueueCreate(&SerialPortBuffer0,buffer_size, &Serial0) != ALLOC_EVENT_OK)
		{
		  // Oh Oh
		  // Não deveria entrar aqui !!!
		  while(1){};
		};
	}

	UARTIntEnable(UART0_BASE, UART_INT_R);
	UARTIntCallbackInit(UART0_BASE, UART0_INT_HANDLE);

	// Enable UART Receive and Transmit
	UARTEnable(UART0_BASE, UART_TX | UART_RX);

	xIntEnable(28);
}



void Init_UART1(void)
{
	xtEventCallback UART1_INT_HANDLE = USART1IntHandler;

	// Enable GPIO and UART Clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// Remap UART pin to GPIO Port UART0_RX --> PA2 UART0_TX --> PA1
	xSPinTypeUART(UART1RX, PE1);
	xSPinTypeUART(UART1TX, PE0);

	// Set UART clock
	SysCtlPeripheralClockSourceSet(SYSCTL_PERIPH_UART0_S_MCGPLLCLK_2);

	// Disable UART Receive/Transmit
	UARTDisable(UART1_BASE, UART_TX | UART_RX);

	// Configure UART Baud 115200
	UARTConfigSet(UART1_BASE, 57600, UART_CONFIG_SAMPLE_RATE_DEFAULT | UART_CONFIG_WLEN_9 | UART_CONFIG_PAR_EVEN | UART_CONFIG_STOP_1);

	if (OSSemCreate(0, &SerialTX1) != ALLOC_EVENT_OK)
	{
	  // Oh Oh
	  // Não deveria entrar aqui !!!
	  while(1){};
	};

	if (OSQueueCreate(&SerialPortBuffer1,128, &Serial1) != ALLOC_EVENT_OK)
	{
	  // Oh Oh
	  // Não deveria entrar aqui !!!
	  while(1){};
	};

	UARTIntEnable(UART1_BASE, UART_INT_R);
	UARTIntCallbackInit(UART1_BASE, UART1_INT_HANDLE);

	// Enable UART Receive and Transmit
	UARTEnable(UART1_BASE, UART_TX | UART_RX);

	xIntEnable(29);
}

void Init_UART2(void)
{
	xtEventCallback UART2_INT_HANDLE = USART2IntHandler;

	// Enable GPIO and UART Clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// Remap UART pin to GPIO Port UART0_RX --> PA2 UART0_TX --> PA1
	xSPinTypeUART(UART2RX, PE23);
	xSPinTypeUART(UART2TX, PE22);

	// Set UART clock
	SysCtlPeripheralClockSourceSet(SYSCTL_PERIPH_UART0_S_MCGPLLCLK_2);

	// Disable UART Receive/Transmit
	UARTDisable(UART2_BASE, UART_TX | UART_RX);

	// Configure UART Baud 115200
	UARTConfigSet(UART2_BASE, 9600, UART_CONFIG_SAMPLE_RATE_DEFAULT | UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_1);

	if (OSSemCreate(0, &SerialTX2) != ALLOC_EVENT_OK)
	{
	  // Oh Oh
	  // Não deveria entrar aqui !!!
	  while(1){};
	};

	if (OSQueueCreate(&SerialPortBuffer2,128, &Serial2) != ALLOC_EVENT_OK)
	{
	  // Oh Oh
	  // Não deveria entrar aqui !!!
	  while(1){};
	};

	UARTIntEnable(UART2_BASE, UART_INT_R);
	UARTIntCallbackInit(UART2_BASE, UART2_INT_HANDLE);

	// Enable UART Receive and Transmit
	UARTEnable(UART2_BASE, UART_TX | UART_RX);

	xIntEnable(30);
}

void UART2_57600(void)
{
	// Disable UART Receive/Transmit
		UARTDisable(UART2_BASE, UART_TX | UART_RX);

		// Configure UART Baud 57600
		UARTConfigSet(UART2_BASE, 57600, UART_CONFIG_SAMPLE_RATE_DEFAULT | UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_1);

		// Enable UART Receive and Transmit
		UARTEnable(UART2_BASE, UART_TX | UART_RX);
}

