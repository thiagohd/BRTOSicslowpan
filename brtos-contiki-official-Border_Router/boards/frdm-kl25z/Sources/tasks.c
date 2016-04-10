
/*********************************************************************************************************
*                                               BRTOS
*                                Brazilian Real-Time Operating System
*                            Acronymous of Basic Real-Time Operating System
*
*                              
*                                  Open Source RTOS under MIT License
*
*
*
*                                              OS Tasks
*
*
*   Author:   Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/


#include "BRTOS.h"
#include "tasks.h"


void System_Time(void)
{
   // task setup
   INT16U i = 0;
   INT16U j = 0;
   
   OSResetTime();
   // task main loop
   for (;;)
   {
	  DelayTask(10);
      
      i++;
      
      if (i >= 100)
      {
        OSUpdateUptime();
        i = 0;
      }
   }
}

#if (USB_DEMO)

#include "usb_terminal.h"
#include "usb_terminal_commands.h"
#include "types.h"          /* Contains User Defined Data Types */
#include "usb_cdc.h"        /* USB CDC Class Header File */
#include "virtual_com.h"    /* Virtual COM Application Header File */

void Task_USB(void)
{
	/* task setup */
    USB_Init();

	/* Initialize the USB Test Application */
	(void)cdc_Init();

    usb_terminal_init(cdc_putch);

  	(void)usb_terminal_add_cmd((command_t*)&usb_ver_cmd);
  	(void)usb_terminal_add_cmd((command_t*)&usb_top_cmd);    
    (void)usb_terminal_add_cmd((command_t*)&echo_cmd);

    while(1)
    {
        /* Call the application task */
    	usb_terminal_process();
    }
}

#endif


