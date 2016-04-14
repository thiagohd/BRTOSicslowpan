/*
 * Copyright (c) 2015, Universidade Federal de Santa Maria.
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
 * This file is part of the uFSM real time operating system.
 *
 * Author: Carlos H. Barriquello <barriquello@gmail.com>
 *
 */
/**
 * \file contiki-main.c
 * Contiki OS task
 * \author
 * Carlos H. Barriquello <barriquello@gmail.com>
 *
 */

#ifdef __cplusplus
extern "C"
#endif

/* Contiki compatibility tests */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "contiki-net.h"
#include "sys/clock.h"

#include "sys/etimer.h"

#include "dev/serial-line.h"
#include "dev/slip.h"
#include "platform-conf.h"

#include "BRTOS.h"

extern BRTOS_Queue *USB;
BRTOS_Sem *Contiki_Sem;

int main_minimal_net(void);
int another_net(void);

void contiki_main(void);

/* for printf support on CW */
extern void InitializeUART(void);
extern char ReadUARTN(void);
extern void WriteUARTN(char c);

#include "printf_lib.h"
#define printf(...)	printf_lib(__VA_ARGS__)
#define fprintf(...)
#define PRINTF(...)	printf_lib(__VA_ARGS__)

void print_lladdrs(void);

PROCINIT(&tcpip_process);

#include "board-frdm-kl25z-conf.h"
#include "init-net.h"

/* ---- MAIN Contiki Loop --- */

void contiki_main(void) {

	another_net();
	//main_minimal_net();

	for (;;) {
		DelayTask(1000);
	} /* loop forever */

}

#include <stdarg.h>

void debug_printf(char *format, ...) {

#if 0	
	va_list argptr;
	char buffer[256];

	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

#if WITH_GUI
	OutputDebugString(buffer);
#else /* WITH_GUI */
	//fputs(buffer, stderr);
#endif /* WITH_GUI */
#endif
	while (1) {
#if BRTOS_CPU == COLDFIRE_V1
		__RESET_WATCHDOG();
#endif
	}
}
/*-----------------------------------------------------------------------------------*/
void uip_log(char *message) {
	printf("%s\n", message);
}
/*-----------------------------------------------------------------------------------*/
void log_message(const char *part1, const char *part2) {
	printf("%s%s\n", part1, part2);
}

/*-----------------------------------------------------------------------------------*/

#if NETSTACK_CONF_WITH_IPV6
/*---------------------------------------------------------------------------*/
static void sprint_ip6(uip_ip6addr_t addr) {
	unsigned char i = 0;
	unsigned char zerocnt = 0;
	unsigned char numprinted = 0;
	unsigned char notskipped = 0;
	char thestring[40];
	char *result = thestring;

	*result++ = '[';
	while (numprinted < 8) {
		if ((addr.u16[i] == 0) && (zerocnt == 0)) {
			while (addr.u16[zerocnt + i] == 0) {
				zerocnt++;
			}
			if (zerocnt == 1 && notskipped) {
				*result++ = '0';
				numprinted++;
				notskipped = 1;
				continue;
			}
			i += zerocnt;
			numprinted += zerocnt;
		} else {
			result += sprintf(result, "%x",
					(unsigned int) (uip_ntohs(addr.u16[i])));
			i++;
			numprinted++;
		}
		if (numprinted != 8) {
			*result++ = ':';
		}
	}
	*result++ = ']';
	*result = 0;
	PRINTF("%s", thestring);
}
#endif /* NETSTACK_CONF_WITH_IPV6 */
/*---------------------------------------------------------------------------*/
void start_udp_server_task(void);

int another_net(void) {

	clock_init();



	random_init(MOTE_ID);
	init_net(MOTE_ID);


	process_init();

	start_radio_task();

	process_start(&etimer_process, NULL);

	ctimer_init();
	rtimer_init();

	procinit_init();

	autostart_start(autostart_processes);
	start_udp_server_task();

	PRINTF("Processes running\n");

	OSSemBinaryCreate(0, &Contiki_Sem);

	PRINTF("\n*******%s online*******\n\r", CONTIKI_VERSION_STRING);

	while (1) {

		int n;

		do {
			n = process_run();
		} while (n > 0);

		//OSSemPend(Contiki_Sem, 0);
	}
}
/*****************************************************************************/
// declare a LL MAC address
const linkaddr_t addr = { { 0x00, 0x00, 0x00, 0x06, 0x98, 0x00, 0x02, 0x32 } };

int main_minimal_net(void) {

	// TODO: Pegar MAC Address do Microchip MRF24J40MA
	clock_init();

	linkaddr_set_node_addr((linkaddr_t*) &addr);
	memcpy(&uip_lladdr.addr, &linkaddr_node_addr.u8, sizeof(uip_lladdr.addr));

#if 1	
	queuebuf_init();
	netstack_init();
#endif  	

	process_init();
	/* procinit_init initializes RPL which sets a ctimer for the first DIS */
	/* We must start etimers and ctimers,before calling it */
	process_start(&etimer_process, NULL);
	ctimer_init();

	procinit_init();

#if AUTOSTART_ENABLE
	autostart_start(autostart_processes);
#endif
	OSSemBinaryCreate(0, &Contiki_Sem);

	PRINTF("\n*******%s online*******\n\r", CONTIKI_VERSION_STRING);

	while (1) {

		int n;
		//char c;
		//INT8U ret,poll;
		//clock_time_t next_event;

		do {
			n = process_run();
			//mrf24j40_get_rssi();
		} while (n > 0);

		OSSemPend(Contiki_Sem, 0);
	}
}
void print_lladdrs(void) {
	int i, a;
	printf("Tentative link-local IPv6 address ");

	for (a = 0; a < UIP_DS6_ADDR_NB; a++) {
		if (uip_ds6_if.addr_list[a].isused) {
			for (i = 0; i < 7; ++i) {
				printf(
						"%02x%02x:", uip_ds6_if.addr_list[a].ipaddr.u8[i * 2], uip_ds6_if.addr_list[a].ipaddr.u8[i * 2 + 1]);
			}
			printf(
					"%02x%02x\n", uip_ds6_if.addr_list[a].ipaddr.u8[14], uip_ds6_if.addr_list[a].ipaddr.u8[15]);
		}
	}
}

