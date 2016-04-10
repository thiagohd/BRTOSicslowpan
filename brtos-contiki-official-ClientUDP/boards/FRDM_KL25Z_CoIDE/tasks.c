
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


/* MCU and OS includes */
#include "BRTOS.h"
#include "xhw_types.h"
#include "xsysctl.h"
#include "xgpio.h"
#include "xwdt.h"
#include "xhw_memmap.h"
#include "accel.h"
#include "xhw_ints.h"
#include "xhw_uart.h"
#include "xuart.h"
#include "xcore.h"
#include "xhw_gpio.h"
#include "system.h"
#include "FLASH.h"
#include <string.h>

/* Config. files */
#include "BRTOSConfig.h"
#include "BoardConfig.h"

/* Function prototypes */
#include "tasks.h"



/*************************************************/
/* Task: keep watchdog happy and system time     */
/*************************************************/
void System_Time(void)
{
   /* task setup */
   INT16U i = 0;

   OSResetTime();
   
#if (WATCHDOG == 1)
   /* Initialize the WDT */
   WDTimerInit(WDT_MODE_NORMAL | xWDT_INTERVAL_1K_32MS);
   WDTimerEnable();
#else
	WDTimerDisable();
#endif
  
   /* task main loop */
   for (;;)
   {
	   #if (WATCHDOG == 1)
	   /* Feed WDT counter */
	   WDTimerRestart();
	   #endif

      DelayTask(10); /* wait 10 ticks -> 10 ms */
      
      i++;
      if (i >= 100)
      {
        OSUpdateUptime();
        i = 0;
      }
   }
}


