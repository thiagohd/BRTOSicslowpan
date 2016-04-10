/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 * Author: Oliver Schmidt <ol.sc@web.de>
 *
 */

#include "contiki.h"
#include "BRTOS.h"

static unsigned long long clock;

/*---------------------------------------------------------------------------*/
void
clock_init(void)
{
	clock = 0;
}


/*-----------------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{ 
  return (clock_time_t)(clock);
}
/*-----------------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
  return (unsigned long)(clock_time_t)(clock / configTICK_RATE_HZ);
}
/*-----------------------------------------------------------------------------------*/

void clock_delay(unsigned int delay)
{

}

void clock_delay_usec(uint16_t dt){
    int i=0;
	for(i=0;i<dt*6;i++)
    {
    };
}

/* BRTOS Timer Hook is used to implement Contiki's clock */
extern BRTOS_Sem *Contiki_Sem;

void BRTOS_TimerHook(void)
{
	clock_time_t next_event;
	clock++;
#if 0
	count_sec++;

	if(count_sec >= CLOCK_CONF_SECOND)
	{
		count_sec = 0;
		seconds++;
	}
#endif

	next_event = etimer_next_expiration_time();
	if(next_event){
		next_event -= clock_time();
		if (next_event == 0)
		{
			etimer_request_poll();
			OSSemPost(Contiki_Sem);
		}
	}
}

