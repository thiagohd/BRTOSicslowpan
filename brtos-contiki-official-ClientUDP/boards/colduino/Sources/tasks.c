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
#include "drivers.h"
#include "tasks.h"
#include "AppConfig.h"
#include "virtual_com.h"
#include "usb_terminal.h"
#include "usb_terminal_commands.h"
#include "led_onboard.h"

#pragma warn_implicitconv off

void System_Time(void)
{
	// task setup
	INT16U milis = 0;
	INT16U segundos = 0;

	OSResetTime();

	led_onboard_init();
	
	led_onboard_on();
	
	#if RTC_PRESENTE
		while(Init_Calendar() == FALSE)
		{
			DelayTask(10);
			#if (WATCHDOG == 1)
				__RESET_WATCHDOG();
			#endif			
		}
	#endif	
	
	led_onboard_off();

	// task main loop
	for (;;)
	{
		#if (WATCHDOG == 1)
			__RESET_WATCHDOG();
		#endif     

		DelayTask(10);
		milis += 10;
		
		if (milis >= 1000)
		{
			milis = 0;
			
			OSUpdateUptime();
			OSUpdateCalendar();
			
			segundos++;
			if (segundos == 3600)
			{
				segundos = 0;
				
				#if RTC_PRESENTE				
					Resync_calendar();  // resync with RTC every 3600s = 60min = 1h
				#endif				
				
			}
		}

#if (defined FATFS_ENABLE && FATFS_ENABLE == 1)
		///////////////////////////////////////////////////
		///   FatFS Timer Handler                       ///
		///////////////////////////////////////////////////
		disk_timerproc();
		///////////////////////////////////////////////////   
#endif
	}

}

#include "user_config.h"
#if (USB_CLASS_TYPE == BRTOS_USB_MSD)
void Mass_Storage_Device_Task(void)
{
	/* task setup */
	(void)MSD_Init(); /* Initialize the USB Test Application */

	while(1)
	{
		/* Call the application task */
		MSD_Task();
	}
}
#endif

#if (USB_CLASS_TYPE == BRTOS_USB_CDC)

#define TERM_UART1  1
#define TERM_UART2  1

#define TERM1_PINS   UART1_PTA1_PTA2
//#define TERM2_PINS   UART2_PTE5_PTE6
#define TERM2_PINS   UART2_PTF1_PTF2

void Terminal_Task(void)
{
	/* task setup */
	(void) CDC_Init(); /* Initialize the USB CDC Application */

	usb_terminal_init(cdc_putch);
	
#if TERM_UART1
	#define UART1_BUFSIZE	128
	#if UART1_MUTEX	
		uart_init(1,9600,UART1_BUFSIZE,TRUE,UART1_MUTEX_PRIO);
	#else
		uart_init(1,9600,UART1_BUFSIZE,FALSE,0);
	#endif
#endif		
	
#if TERM_UART2
	#define UART2_BUFSIZE	128
	#if UART2_MUTEX		
		uart_init(2,9600,UART2_BUFSIZE,TRUE,UART2_MUTEX_PRIO);
	#else
		uart_init(2,9600,UART2_BUFSIZE,FALSE,0);
	#endif	

#endif	

	(void) usb_terminal_add_cmd((command_t*) &usb_ver_cmd);
	(void) usb_terminal_add_cmd((command_t*) &usb_top_cmd);
	(void) usb_terminal_add_cmd((command_t*) &usb_rst_cmd);
	(void) usb_terminal_add_cmd((command_t*) &usb_temp_cmd);
	(void) usb_terminal_add_cmd((command_t*) &setget_time_cmd);
	(void) usb_terminal_add_cmd((command_t*) &cat_cmd);
	(void) usb_terminal_add_cmd((command_t*) &ls_cmd);
	(void) usb_terminal_add_cmd((command_t*) &cd_cmd);
	(void) usb_terminal_add_cmd((command_t*) &mount_cmd);
	(void) usb_terminal_add_cmd((command_t*) &sr_cmd);
	(void) usb_terminal_add_cmd((command_t*) &rm_cmd);
	(void) usb_terminal_add_cmd((command_t*) &rn_cmd);
	(void) usb_terminal_add_cmd((command_t*) &cr_cmd);
	(void) usb_terminal_add_cmd((command_t*) &mkdir_cmd);
	(void) usb_terminal_add_cmd((command_t*) &cp_cmd);
	(void) usb_terminal_add_cmd((command_t*) &wt_cmd);
	(void) usb_terminal_add_cmd((command_t*) &echo_cmd);
	(void) usb_terminal_add_cmd((command_t*) &echo_stdout_cmd);
	(void) usb_terminal_add_cmd((command_t*) &esp_cmd);	

	while (1)
	{
		/* Call the application task */
		usb_terminal_process();
	}
}
#endif
INT16S CoreTemp = 0;

void Tarefa_termometro(void)
{
	/* task setup */
	INT16U digital_temp = 0;
	INT16U bandgap = 0;
	INT16S temperature = 0;

	ADSetup(NormalPower, HighSpeed, ShortSampleTime, 20, 12);

	/* task main loop */
	for (;;)
	{
		(void) DelayTask(30000); /* 30s */

		//////////////////////////////////////////////////////////////
		// Adquire temperatura do core
		//////////////////////////////////////////////////////////////
		UserEnterCritical();
		digital_temp = ADConvert(TEMP_SENSOR_CH);
		UserExitCritical();

		UserEnterCritical();
		bandgap = ADConvert(BANDGAP_CH);
		UserExitCritical();

		digital_temp = (1170 * digital_temp) / bandgap;

		if (digital_temp > 701)
		{
			temperature = 25 - (((digital_temp - 701) * 1000) / 1646);
		}
		else
		{
			temperature = 25 - (((digital_temp - 701) * 1000) / 1769);
		}

		UserEnterCritical();
		CoreTemp = temperature;
		UserExitCritical();
		//////////////////////////////////////////////////////////////         
	}
}



#if 0
// Declares a queue structure for the keyboard
OS_QUEUE KeyboardBuffer;
BRTOS_Queue *qKeyboard;
// 
void HMI(void)
{
	/* task setup */
	INT8U key = 0;

	/* task main loop */
	for (;;)
	{

		if (!OSQueuePend(qKeyboard, &key, 500))
		{

			switch (key)
			{

			case UP_KEY :
				break;

			case DOWN_KEY :
				break;
			}
		}
	}
}

void Keyboard_Handler(void)
{
	// task setup
	INT16U ADSample = 0;
	INT8U Key = NO_KEY;

	// Initialize ACM keyboard for channel 1  
	ACM_Keyb_Setup(Enable, Enable, Rising, ACM_CHANNEL1);

	if (OSSemCreate(0, &sKeyboard) != ALLOC_EVENT_OK)
	{
		while (1){};
	};

	if (OSQueueCreate(&KeyboardBuffer, 64, &qKeyboard) != ALLOC_EVENT_OK)
	{
		while (1){};
	};

	// task main loop
	for (;;)
	{
		// Wait for a keyboard interrupt
		OSSemPend(sKeyboard, 0);
		DelayTask(50);

		// Converts the value of AD to discover the pressed key
		UserEnterCritical();
		ADSample = ADConvert(KEYB_CHANNEL);
		UserExitCritical();

		UserEnterCritical();
		ADSample += ADConvert(KEYB_CHANNEL);
		UserExitCritical();

		ADSample = ADSample >> 1;

		// Find out which key was pressed
		Key = FindKey(ADSample);

		// Copy the key to the keyboard buffer
		if (Key != NO_KEY)
		{
			if (OSQueuePost(qKeyboard, Key) == BUFFER_UNDERRUN)
			{
				// Buffer overflow
				OSCleanQueue(qKeyboard);
			}
		}

		// Enable interrupt to the next key detection
		DelayTask(200);
		ACMEnable();
	}
}
#endif

#if 0
void BRTOS_TimerHook(void)
{

}
#endif

