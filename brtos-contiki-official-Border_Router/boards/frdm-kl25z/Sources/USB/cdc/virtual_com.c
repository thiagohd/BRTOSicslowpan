/******************************************************************************
 * Includes
 *****************************************************************************/
#include <MKL25Z4.h>
#include "derivative.h"
#include "types.h"          /* Contains User Defined Data Types */
#include "usb_cdc.h"        /* USB CDC Class Header File */
#include "virtual_com.h"    /* Virtual COM Application Header File */
#include "hcc_terminal.h"
#include "usb_terminal_commands.h"
#include <stdio.h>

/* skip the inclusion in dependency state */
#ifndef __NO_SETJMP
	#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>


/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void USB_App_Callback(uint_8 controller_ID,
                        uint_8 event_type, void* val);
static void USB_Notify_Callback(uint_8 controller_ID,
                        uint_8 event_type, void* val);
/*****************************************************************************
 * Local Variables
 *****************************************************************************/
/* Virtual COM Application start Init Flag */
static volatile boolean start_app = FALSE;
/* Virtual COM Application Carrier Activate Flag */
//static volatile boolean start_transactions = FALSE;
static volatile boolean start_transactions = TRUE;
/* Receive Buffer */
//static uint_8 g_curr_recv_buf[DATA_BUFF_SIZE];
/* Send Buffer */
static uint_8 g_curr_send_buf[DATA_BUFF_SIZE];
/* Receive Data Size */
static uint_8 g_recv_size;
/* Send Data Size */
static uint_8 g_send_size;

static INT8U 	is_message_sent = 0;

BRTOS_Sem   	*USB_Sem;

// Declara uma estrutura de fila
OS_QUEUE 		USBRXBuffer;
// Declara um ponteiro para o bloco de controle da Porta Serial
BRTOS_Queue 	*USB;

/******************************************************************************
*
*   @name        TestApp_Init
*
*   @brief       This function is the entry for the Virtual COM Loopback app
*
*   @param       None
*
*   @return      None
*****************************************************************************
* This function starts the Virtual COM Loopback application
*****************************************************************************/

void cdc_Init(void)
{
   //uint_8   error;

   g_recv_size = 0;

	// Create USB semaphore
	if (OSSemCreate(0,&USB_Sem) != ALLOC_EVENT_OK)
   {
     // Oh Oh
     // N�o deveria entrar aqui !!!
     while(1){};
   };

	// Cria uma fila de recepcao para a porta serial
	if (OSQueueCreate(&USBRXBuffer,USB_RX_BUFF_SIZE, &USB) != ALLOC_EVENT_OK)
	{
		// Oh Oh
		// N�o deveria entrar aqui !!!
		while(1){};
	};

    /* Initialize the USB interface */
	//error =
    (void)USB_Class_CDC_Init(CONTROLLER_ID,USB_App_Callback,
                                NULL,USB_Notify_Callback, TRUE);
}

void USB_Init(void)
{
		/* 1. Configure USB to be clocked from PLL */
		SIM_SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK;

		/* 3. Enable USB-OTG IP clocking */
		SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);

		/* old documentation writes setting this bit is mandatory */
		USB0_USBTRC0 = 0x40;

		/* Configure enable USB regulator for device */
		SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;

        NVIC_ICER |= (1<<24);	 /* Clear any pending interrupts on USB */
        NVIC_ISER |= (1<<24);	 /* Enable interrupts from USB module */
}


uint_8 GetStart_transactions(void)
{
	return (start_transactions);
}


void cdc_process(void)
{
    static uint_8 status 	 = 0;
    uint_8 		  sem_status = 0;
	uint_8 size = g_send_size;

    /*check whether enumeration is complete or not */
     if((start_app==TRUE) && (start_transactions==TRUE))
     {
 		g_send_size = 0;

 		UserEnterCritical();
 		is_message_sent = 1;
 		UserExitCritical();

 		status = USB_Class_CDC_Interface_DIC_Send_Data(CONTROLLER_ID, g_curr_send_buf,size);
 		sem_status = OSSemPend(USB_Sem,500);

 		if (sem_status != OK)
 		{
 			UserEnterCritical();
 			is_message_sent = 0;
 			UserExitCritical();
 		}

         if(status != USB_OK)
         {
             /* Send Data Error Handling Code goes here */
         	status = 0;
         }
     }else
     {
    	  while(GetStart_transactions() == FALSE)
    	  {
    		  DelayTask(100);
    	  }
     }
}

unsigned char cdc_putch(char c)
{
  uint_8 r=(unsigned char)c;
  /* Store character into current buffer. */
  if (g_send_size < sizeof(g_curr_send_buf))
  {
	g_curr_send_buf[g_send_size++]=(unsigned char)c;
	
#if 0	
    if ((c == '\r') || (c == '>'))
    {
      cdc_process();
    }
#endif    
  }
  else
  {
    cdc_process();
    r++;
  }
  return((uint_8)r);
}

/******************************************************************************
 *
 *    @name        USB_App_Callback
 *
 *    @brief       This function handles Class callback
 *
 *    @param       controller_ID    : Controller ID
 *    @param       event_type       : Value of the event
 *    @param       val              : gives the configuration value
 *
 *    @return      None
 *
 *****************************************************************************
 * This function is called from the class layer whenever reset occurs or enum
 * is complete. After the enum is complete this function sets a variable so
 * that the application can start.
 * This function also receives DATA Send and RECEIVED Events
 *****************************************************************************/

static void USB_App_Callback (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 event_type,      /* [IN] value of the event */
    void* val               /* [IN] gives the configuration value */
)
{
    UNUSED (controller_ID)
    UNUSED (val)
    if(event_type == USB_APP_BUS_RESET)
    {
        start_app=FALSE;
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {
#if HIGH_SPEED_DEVICE
    	// prepare for the next receive event
    	USB_Class_CDC_Interface_DIC_Recv_Data(&controller_ID,
    	                                      (uint_8_ptr)g_cdcBuffer,
    	                                      DIC_BULK_OUT_ENDP_PACKET_SIZE);
#endif
        start_app=TRUE;
    }
    else if((event_type == USB_APP_DATA_RECEIVED)&&
            (start_transactions == TRUE))
    {
        /* Copy Received Data buffer to Application Buffer */
        USB_PACKET_SIZE BytesToBeCopied;
        APP_DATA_STRUCT* dp_rcv = (APP_DATA_STRUCT*)val;
        uint_8 index;
        BytesToBeCopied = (USB_PACKET_SIZE)((dp_rcv->data_size > DATA_BUFF_SIZE) ?
                                      DATA_BUFF_SIZE:dp_rcv->data_size);
        for(index = 0; index<BytesToBeCopied ; index++)
        {
			if (OSQueuePost(USB, dp_rcv->data_ptr[index]) == BUFFER_UNDERRUN)
			{
			  //OSCleanQueue(USB);				
			}
        }
    }
    else if((event_type == USB_APP_SEND_COMPLETE) && (start_transactions == TRUE))
    {
		if (is_message_sent)
		{
			is_message_sent = 0;
			OSSemPost(USB_Sem);
        }
        /* Previous Send is complete. Queue next receive */
#if HIGH_SPEED_DEVICE
    	(void)USB_Class_CDC_Interface_DIC_Recv_Data(CONTROLLER_ID, g_cdcBuffer, DIC_BULK_OUT_ENDP_PACKET_SIZE);
#else
        (void)USB_Class_CDC_Interface_DIC_Recv_Data(CONTROLLER_ID, NULL, 0);
#endif
    }

    return;
}

/******************************************************************************
 *
 *    @name        USB_Notify_Callback
 *
 *    @brief       This function handles PSTN Sub Class callbacks
 *
 *    @param       controller_ID    : Controller ID
 *    @param       event_type       : PSTN Event Type
 *    @param       val              : gives the configuration value
 *
 *    @return      None
 *
 *****************************************************************************
 * This function handles USB_APP_CDC_CARRIER_ACTIVATED and
 * USB_APP_CDC_CARRIER_DEACTIVATED PSTN Events
 *****************************************************************************/

static void USB_Notify_Callback (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 event_type,      /* [IN] PSTN Event Type */
    void* val               /* [IN] gives the configuration value */
)
{
    UNUSED (controller_ID)
    UNUSED (val)
    if(start_app == TRUE)
    {
        if(event_type == USB_APP_CDC_CARRIER_ACTIVATED)
        {
            start_transactions = TRUE;
            //echo ("BRTOS Started!", FALSE);
        }
        else if(event_type == USB_APP_CDC_CARRIER_DEACTIVATED)
        {
            //start_transactions = FALSE;
        }
    }
    return;
}
