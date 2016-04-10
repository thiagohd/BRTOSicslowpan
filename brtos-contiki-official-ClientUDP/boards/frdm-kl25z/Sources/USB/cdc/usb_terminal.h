/****************************************************************************
*
*            Copyright (c) 2006-2007 by CMX Systems, Inc.
*
* This software is copyrighted by and is the sole property of
* CMX.  All rights, title, ownership, or other interests
* in the software remain the property of CMX.  This
* software may only be used in accordance with the corresponding
* license agreement.  Any unauthorized use, duplication, transmission,
* distribution, or disclosure of this software is expressly forbidden.
*
* This Copyright notice may not be removed or modified without prior
* written consent of CMX.
*
* CMX reserves the right to modify this software without notice.
*
* CMX Systems, Inc.
* 12276 San Jose Blvd. #511
* Jacksonville, FL 32223
* USA
*
* Tel:  (904) 880-1840
* Fax:  (904) 880-1632
* http: www.cmx.com
* email: cmx@cmx.com
*
***************************************************************************/


#ifndef _USB_TERMINAL_H_
#define _USB_TERMINAL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "hcc_terminal.h"

extern int usb_terminal_add_cmd(command_t *cmd);
extern int usb_terminal_delete_cmd(command_t *cmd);
extern void usb_terminal_init(unsigned char (*putch_)(char));
extern void usb_terminal_process(void);
extern void print_usb(char *s);
extern void putchar_usb(char c);
void SetSilentMode(char mode);
unsigned char TerminalBackup(char *backup);

#ifdef __cplusplus
}
#endif
#endif

/****************************** END OF FILE **********************************/
