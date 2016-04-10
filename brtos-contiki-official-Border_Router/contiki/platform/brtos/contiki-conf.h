#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

#include "BRTOS.h"
#include "platform-conf.h"

/* default contiki-conf */
#ifndef CCIF
#define CCIF
#endif

#ifndef CLIF
#define CLIF
#endif

/*#ifndef LOG_CONF_ENABLED
#define LOG_CONF_ENABLED 1
#include "sys/log.h"
CCIF void debug_printf(char *format, ...);
#endif
*/
typedef unsigned long long clock_time_t;
#define CLOCK_CONF_SECOND 	1000
#define INFINITE_TIME 	 	ULONG_MAX

//#define SLIP_CONF_ANSWER_MAC_REQUEST	1

#ifndef HAVE_STDINT
#define HAVE_STDINT 1
#endif

#if !HAVE_STDINT
typedef INT8S   int8_t;
typedef INT8U   uint8_t;
typedef INT16S  int16_t;
typedef INT16U 	uint16_t;
typedef INT32S  int32_t;
typedef INT32U  uint32_t;
#else
#include "stdint.h"
#endif

/* These names are deprecated, use C99 names. */
typedef INT8U   u8_t;
typedef INT16U  u16_t;
typedef INT32S  s32_t;

typedef uint16_t 	uip_stats_t;

#if BRTOS_ENDIAN == BRTOS_BIG_ENDIAN		
#define UIP_CONF_BYTE_ORDER           UIP_BIG_ENDIAN
#elif BRTOS_ENDIAN == BRTOS_LITTLE_ENDIAN	
#define UIP_CONF_BYTE_ORDER           UIP_LITTLE_ENDIAN
#endif	


/* Not part of C99 but actually present */
int strcasecmp(const char*, const char*);

#ifndef AUTOSTART_ENABLE
#define AUTOSTART_ENABLE 	1
#endif

#ifndef PROJECT_CONF_H
#define PROJECT_CONF_H 		1
#endif

#if PROJECT_CONF_H
#include "project-conf.h"
#endif /* PROJECT_CONF_H */

#endif /* CONTIKI_CONF_H_ */
