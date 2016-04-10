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

#ifndef LOG_CONF_ENABLED
#define LOG_CONF_ENABLED 1
#include "sys/log.h"
CCIF void debug_printf(char *format, ...);
#endif

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

#define NETSTACK_CONF_WITH_IPV6		  1

/* hack for CW compiler. It is not finding this defines in uip.h */
/* Header sizes. */
#if NETSTACK_CONF_WITH_IPV6
#if defined(__CWCC__)
#define UIP_IPH_LEN    40
#define UIP_FRAGH_LEN  8
#endif
#else /* NETSTACK_CONF_WITH_IPV6 */
#define UIP_IPH_LEN    20    /* Size of IP header */
#endif /* NETSTACK_CONF_WITH_IPV6 */

#define uip_l2_l3_hdr_len (UIP_LLH_LEN + UIP_IPH_LEN + uip_ext_len)
#define uip_l2_l3_icmp_hdr_len (UIP_LLH_LEN + UIP_IPH_LEN + uip_ext_len + UIP_ICMPH_LEN)
#define uip_l3_hdr_len (UIP_IPH_LEN + uip_ext_len)
#define uip_l3_icmp_hdr_len (UIP_IPH_LEN + uip_ext_len + UIP_ICMPH_LEN)


#define LINKADDR_CONF_SIZE              8
#define UIP_CONF_LL_802154              1
#define UIP_CONF_LLH_LEN                0
//#define UIP_CONF_LLH_LEN              14


#ifndef BRTOS_PLATFORM
#define BRTOS_PLATFORM		BOARD_NONE
#endif

#ifndef UIP_CONF_UDP
#define UIP_CONF_UDP                  0
#endif

#ifndef UIP_CONF_TCP
#define UIP_CONF_TCP                  1
#endif

#ifndef UIP_CONF_MAX_LISTENPORTS
#define UIP_CONF_MAX_LISTENPORTS      2
#endif

#ifndef UIP_CONF_MAX_CONNECTIONS
#define UIP_CONF_MAX_CONNECTIONS      1
#endif

#ifndef NETSTACK_CONF_WITH_RIME
#define NETSTACK_CONF_WITH_RIME		  1
#endif

#ifndef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER                 1 
#endif

#ifndef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL				0
#endif

#if BRTOS_PLATFORM == BOARD_NONE
#define UIP_CONF_BUFFER_SIZE          1514 //1280//1514
#define UIP_CONF_TCP_SPLIT            0
#define UIP_CONF_LOGGING              1
#define UIP_CONF_IP_FORWARD           0
#define UIP_CONF_UDP_CHECKSUMS        1

#if NETSTACK_CONF_WITH_IPV6
#define UIP_CONF_IPV6_QUEUE_PKT       	1
#define UIP_CONF_IPV6_CHECKS          	1
#define UIP_CONF_IPV6_REASSEMBLY      	1
//#define UIP_CONF_NETIF_MAX_ADDRESSES  5
#define NBR_TABLE_CONF_MAX_NEIGHBORS    6
#define UIP_CONF_DS6_DEFRT_NBU   		2
#define UIP_CONF_DS6_PREFIX_NBU  		3
#define UIP_CONF_MAX_ROUTES   			4
#define UIP_CONF_DS6_ADDR_NBU    		4
#define UIP_CONF_DS6_MADDR_NBU   		0
#define UIP_CONF_DS6_AADDR_NBU   		0
#define NETSTACK_CONF_NETWORK			sicslowpan_driver//null_network_driver //sicslowpan_driver //rime_driver
#define NETSTACK_CONF_FRAMER			framer_802154
#define NETSTACK_CONF_MAC               nullmac_driver
#define NETSTACK_CONF_RDC               nullrdc_driver
#define NETSTACK_CONF_RADIO             mrf24j40_driver//mrf24j40_driver
#define LINKADDR_CONF_SIZE              8
#else
#define UIP_CONF_IP_FORWARD          1
#endif /* NETSTACK_CONF_WITH_IPV6 */

#define MMEM_CONF_SIZE			   		256
//#define IP64_ADDRMAP_CONF_ENTRIES  	8
//#define PROCESS_CONF_NUMEVENTS	   	10

#define RESOLV_CONF_SUPPORTS_MDNS              0
#define RESOLV_CONF_SUPPORTS_RECORD_EXPIRATION 0

/* Not used but avoids compile errors while sicslowpan.c is being developed */
#define SICSLOWPAN_CONF_COMPRESSION       SICSLOWPAN_COMPRESSION_HC06

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
