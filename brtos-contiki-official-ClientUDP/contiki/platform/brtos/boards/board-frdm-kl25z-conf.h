/*
 * board-frdm-kl25z-conf.h
 *
 *  Created on: Jul 21, 2015
 *      Author: Universidade Tecnológica Federal do Paraná
 */

#ifndef BOARD_FRDM_KL25Z_CONF_H_
#define BOARD_FRDM_KL25Z_CONF_H_

#include "BRTOS.h"

#define BRTOS_ENDIAN           		 	BRTOS_LITTLE_ENDIAN

#define MOTE_ID 						2

// É Border Router?
#if MOTE_ID == 1
#define BORDER_ROUTER
#endif
// Se não, então é Router
#ifndef BORDER_ROUTER
#define ROUTER
#endif

#ifdef BORDER_ROUTER
#define MRF24J40_PAN_COORDINATOR
#endif
#ifdef ROUTER
#define MRF24J40_ROUTER
#endif

#define RF_CHANNEL						13 // Canal de rádio
/*
 * Configurações da NETSTACK
 */

// Pilha de protocolos com IPv6 na camada de rede
#define NETSTACK_CONF_WITH_IPV6		  	1

// Pilha de protocolos com Rime na camada de rede
#define NETSTACK_CONF_WITH_RIME		  	0

// Radio
#define ADD_RSSI_AND_LQI_TO_PACKET

/*
 * Configurações de SLIP
 */

// Serial Line Internet Protocol via USB ou pela UART
#define SLIP_USB						1
#define SLIP_UART						2
#define SLIP_COMM						SLIP_UART

/*
 * Configurações básicas do uIP
 */

// Habilitar protocolos da camada de transporte
#define UIP_CONF_UDP                  	1
#define UIP_CONF_TCP                  	1

// Número máximo de portas a serem escutadas
#define UIP_CONF_MAX_LISTENPORTS      	5

// Número máximo de conexões
#define UIP_CONF_MAX_LISTENPORTS      	8

// Número de buffers de fila utilizados pela NETSTACK, quanto mais melhor.
#define QUEUEBUF_CONF_NUM				12

// Tamanho dos buffers
#define UIP_CONF_BUFFER_SIZE          	(128*6)

#ifndef UIP_CONF_TCP_MSS
#define UIP_CONF_TCP_MSS 				48//UIP_CONF_BUFFER_SIZE - 60
#endif

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW			60 //UIP_CONF_BUFFER_SIZE - 60
#endif
// Hack para otimizar conexões TCP
#define UIP_CONF_TCP_SPLIT            	0
#define UIP_SPLIT_CONF_SIZE				8
#define UIP_CONF_LOGGING              	0
#define UIP_CONF_IP_FORWARD           	0


/*
 * Configurações básicas para NETSTACK com IPv6
 */
#if NETSTACK_CONF_WITH_IPV6
#define UIP_CONF_IPV6_RPL				1	// Ativa roteamento via RPL
#define UIP_CONF_ROUTER 				1	// Determina o dispositivo como sendo roteador
#define UIP_CONF_IPV6_QUEUE_PKT       	1	// Do we do per neighbor queuing during address resolution?
#define UIP_CONF_IPV6_CHECKS          	1	// Do we do IPv6 consistency checks?
#define UIP_CONF_IPV6_REASSEMBLY      	0	// Do we do IPv6 fragmentation?
#define UIP_CONF_NETIF_MAX_ADDRESSES 	3	// Default number of IPv6 addresses associated to the node's interface
#define UIP_CONF_DS6_DEFRT_NBU   		2	// Minimum number of default routes
#define UIP_CONF_DS6_PREFIX_NBU  		3	// Default number of IPv6 prefixes associated to the node's interface
#define UIP_CONF_MAX_ROUTES   			5
#define UIP_CONF_DS6_ADDR_NBU    		5
#define UIP_CONF_DS6_MADDR_NBU   		5	// Habilita o dispositivo a se juntar a 5 grupos de multicast
#define UIP_CONF_DS6_AADDR_NBU   		0	//
#define UIP_CONF_IP_FORWARD				0
#define NETSTACK_CONF_NETWORK			sicslowpan_driver//null_network_driver //sicslowpan_driver //rime_driver
#define NETSTACK_CONF_FRAMER			framer_802154
#define NETSTACK_CONF_MAC               nullmac_driver
#define NETSTACK_CONF_RDC               nullrdc_driver
#define NETSTACK_CONF_RADIO             mrf24j40_driver//mrf24j40_driver//nullradio_driver
/* IPv6 configuration options */
#define NBR_TABLE_CONF_MAX_NEIGHBORS	5	 /* number of neighbors */
#define UIP_CONF_DS6_ROUTE_NBU			5 /* number of routes */
#define UIP_CONF_ND6_SEND_RA			(NETSTACK_CONF_WITH_IPV6 && !UIP_CONF_IPV6_RPL) // Enable/disable RA sending for IPv6 discovery (if not using RPL)
#define UIP_CONF_ND6_SEND_NA 			(NETSTACK_CONF_WITH_IPV6 && !UIP_CONF_IPV6_RPL)
#define UIP_CONF_ND6_REACHABLE_TIME		600000 // default = 30000
#define UIP_CONF_ND6_RETRANS_TIMER		10000 // default = 1000
#else
#define UIP_CONF_IP_FORWARD				1
#endif /* NETSTACK_CONF_WITH_IPV6 */


#define RDC_CONF_HARDWARE_CSMA			1

#define LINKADDR_CONF_SIZE				8

#define MMEM_CONF_SIZE					4096
#define IP64_ADDRMAP_CONF_ENTRIES 		8
#define PROCESS_CONF_NUMEVENTS			8

#define RESOLV_CONF_SUPPORTS_MDNS				0
#define RESOLV_CONF_SUPPORTS_RECORD_EXPIRATION 	0

/* UDP configuration options */
#define UIP_CONF_UDP                            1
#define UIP_CONF_UDP_CHECKSUMS                  1
#define UIP_CONF_UDP_CONNS                      8
#define UIP_CONF_FWCACHE_SIZE					15

#define UIP_CONF_DS6_NBR_NBU					5
#define UIP_CONF_DS6_ROUTE_NBU					5
#define NEIGHBOR_CONF_MAX_NEIGHBORS				5

/* 6lowpan options (for ipv6) */
/* Not used but avoids compile errors while sicslowpan.c is being developed */
#define SICSLOWPAN_CONF_COMPRESSION             SICSLOWPAN_COMPRESSION_HC06
#define SICSLOWPAN_CONF_MAC_MAX_PAYLOAD			(127 - 2)
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS       2 // If we use IPHC compression, how many address contexts do we support
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD   0//63
#define SICSLOWPAN_CONF_FRAG                    1 // Do we support 6lowpan fragmentation?
#define SICSLOWPAN_CONF_MAXAGE                  1//8 // Contiki 2.6 default = 20. Timeout in seconds for packet reassembly at the 6lowpan layer (should be < 60s)
/* General configuration options */
#define UIP_CONF_STATISTICS                     1
#define UIP_CONF_LOGGING                        0
#define UIP_CONF_BROADCAST                      1
#define UIP_CONF_LLH_LEN                        0
#define UIP_CONF_LL_802154                      1	// 1 para configurar manualmente endereço MAC (uip_lladdr)

/* Prevent SDCC compile error when UIP_CONF_ROUTER == 0 */
#if !UIP_CONF_ROUTER
#define UIP_CONF_DS6_AADDR_NBU               1
#endif

#define	UIP_CONF_ICMP_DEST_UNREACH			1



#endif /* BOARD_FRDM_KL25Z_CONF_H_ */
