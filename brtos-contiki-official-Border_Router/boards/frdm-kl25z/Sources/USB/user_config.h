/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 * @file user_config.h
 * @author
 * @version
 * @date May-28-2009
 * @brief The file contains User Modifiable Macros for Virtual COM Loopback
 *        Application
 *****************************************************************************/
#include "derivative.h"

#define  HIGH_SPEED_DEVICE	(0)

/*
   Below two MACROS are required for Virtual COM Loopback
   Application to execute
*/
#define LONG_SEND_TRANSACTION       /* support to send large data pkts */
#define LONG_RECEIVE_TRANSACTION    /* support to receive large data pkts */

#define USB_PACKET_SIZE  uint_16 /* support 16/32 bit packet size */

#ifndef _MCF51JM128_H
/* Use double buffered endpoints 5 & 6. To be only used with S08 cores */
#define DOUBLE_BUFFERING_USED
#endif
