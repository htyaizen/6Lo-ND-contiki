/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"
#include "powertrace.h"

#include <stdio.h> /* For printf() */

#define DEBUG 1
#include "net/ip/uip-debug.h"

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#ifndef PERIOD
#define PERIOD 60
#endif

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client process");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  char *str;
  
  if(uip_newdata()){
     str = uip_appdata;
     str[uip_datalen()] = '\0';
     PRINTF("DATA recv '%s'\n", str);
  }
}
/*---------------------------------------------------------------------------*/
static void
send_packet(void *prt)
{
   static int seq_id;
   char buf[MAX_PAYLOAD_LEN];

   seq_id++;
   PRINTF("DATA send to %d 'Hello %d'\n",
           server_ipaddr.u8[sizeof(server_ipaddr.u8)) - 1], seq_id);
   sprintf(buf,"Hello %d from the client", seq_id);
   uip_udp_packet_sendto(client_conn, buf, strlen(buf), 
                         &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  
  #if 0
  /* Mode 1 - 64 bits inline */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
  #elif 1
  /* Mode 2 - 16 bits inline */
  uip_ip6addr(&server_ipaddr, 0x2001, 5, 0, 0, 0x200, 0, 0, 1);
  #else
  /* Mode 3 - derived from server link-local (MAC) address */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0x0250, 0xc2ff, 0xfea8, 0xcd1a); //redbee-econotag
  #endif

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic;
  static struct ctimer backoff_timer;

  PROCESS_BEGIN();
  PROCESS_PAUSE();

  powertrace_start(CLOCK_SECOND * 2);
 
  PRINTF("UDP client process started\n");
 
  /*create new connection with remote server*/
  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL);
  if(client_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }

  udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT));

  PRINTF("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  PRINTF(" local/remote port %u/%u \n",
          UIP_HTONS(client_conn->lport),UIP_HTONS(client_conn->rport));

   etimer_set(&periodic, SEND_INTERVAL);
   while(1) {
	PROCESS_YIELD();
        if(ev == tcpip_event) {
           tcpip_handler();
         }
       
        if(etimer_expired(&periodic)){
	   etimer_reset(&periodic);
           ctimer_set(&backoff_timer, SEND_TIME, send_packet, NULL);
         }
   }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
