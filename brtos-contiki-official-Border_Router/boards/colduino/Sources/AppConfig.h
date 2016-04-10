///////////////////////////////////////////////////////////////
/////                                                     /////
/////                   User App Defines                  /////
/////                                                     /////
/////             !User App configuration defines!        /////
/////                                                     /////
///////////////////////////////////////////////////////////////
#include "user_config.h"


#define SD_PRESENTE         0
#define RTC_PRESENTE  		0

#if RTC_PRESENTE
#include "Timer_RTC_DS1307.h"
#endif

#if SD_PRESENTE
#include "SD_API.h"
#define SDCARD_MUTEX_PRIORITY 	29
#endif

#define USB_TASK_ENABLED	  0








