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
#include "hcc_types.h"
#include "usb_terminal.h"
#include "virtual_com.h"
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro definitions
 *****************************************************************************/
#define MAX_CMDS      22
#define MAX_CMD_SIZE  8
/*****************************************************************************
 * Local types.
 *****************************************************************************/
/* none */

/*****************************************************************************
 * External references.
 *****************************************************************************/
/* none */

/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
//static void usb_print_greeting(void);
static void usb_cmd_help(char *param);
static void usb_print_prompt(void);
static int usb_find_command(char *name);

/*****************************************************************************
 * Module variables.
 *****************************************************************************/
static const command_t usb_help_cmd = {
  "help", usb_cmd_help, "Prints help about commands. "
};

static char			SilentMode = 0; 
static char 		usb_cmd_line[0x20];
static hcc_u8 		usb_cmd_line_ndx;

static hcc_u8 		usb_n_cmd;
static command_t 	*usb_cmds[MAX_CMDS];

static unsigned char (*putch)(char);


void putchar_usb(char c)
{
    while(c != (char)putch(c)){};
}
/*****************************************************************************
 * Name:
 *    print
 * In:
 *    s: string
 * Out:
 *    n/a
 *
 * Description:
 *    Print the specified string.
 * Assumptions:
 *
 *****************************************************************************/
void print_usb(char *s)
{
  while(*s)
  {
      while(*s != (char)putch(*s))
      {
    	  DelayTask(1);
      };
      s++;
  }
}



/*****************************************************************************
 * Name:
 *    skipp_space
 * In:
 *    cmd_line: string to parse
 *    start: start at offset
 * Out:
 *    index of first non space character
 *
 * Description:
 *
 * Assumptions:
 *
 *****************************************************************************/
int usb_skipp_space(char *usb_cmd_line, int start)
{
  /* Skip leading whitespace. */
  while(usb_cmd_line[start] == ' ' || usb_cmd_line[start] == '\t')
  {
    start++;
  }
  return(start);
}

/*****************************************************************************
 * Name:
 *    find_word
 * In:
 *    cmd_line - pointer to string to be processed
 *    start    - start offset of word
 *
 * Out:
 *    Index of end of word.
 *
 * Description:
 *    Will find the end of a word (first space, tab or end of line).
 *
 * Assumptions:
 *    --
 *****************************************************************************/
int usb_find_word(char *usb_cmd_line, int start)
{
  /* Find end of this word. */
  while(usb_cmd_line[start] != ' ' && usb_cmd_line[start] != '\t' 
        && usb_cmd_line[start] != '\0')
  {
    start++;
  }

  return(start);
}

/*****************************************************************************
 * Name:
 *    cmp_str
 * In:
 *    a - pointer to string one
 *    b - pointer to string two
 * Out:
 *    0 - strings differ
 *    1 - strings are the same
 * Description:
 *    Compare two strings.
 *
 * Assumptions:
 *    --
 *****************************************************************************/
int usb_cmp_str(char *a, char *b)
{
  int x=0;
  do
  {
    if (a[x] != b[x])
    { 
      return(0);
    }
    x++;
  } while(a[x] != '\0' && b[x] != '\0');

  return(a[x]==b[x] ? 1 : 0);
}


/*****************************************************************************
* Name:
*    cmd_help
* In:
*    param - pointer to string containing parameters
*
* Out:
*    N/A
*
* Description:
*    List supported commands.
*
* Assumptions:
*    --
*****************************************************************************/
static void usb_cmd_help(char *param)
{
  int x;
  int y;
  char *name;

  (void)param;
  print_usb("\r\nI understand the following commands:\r\n");

  for(x=0; x < usb_n_cmd; x++)
  {
    print_usb("  ");
    name = (char*)usb_cmds[x]->txt;
    y = 0;
    while(*name)
    {
      y++;
      name++;
    }
    print_usb((char *)usb_cmds[x]->txt);
    for(y;y<MAX_CMD_SIZE;y++)
    {
      print_usb(" ");
    }
    print_usb((char *)usb_cmds[x]->help_txt);
    print_usb("\r\n");
  }
  print_usb("\r\n");
}


/*****************************************************************************
* Name:
*    print_prompt
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Prints the prompt string.
*
* Assumptions:
*    --
*****************************************************************************/
static void usb_print_prompt(void)
{
  print_usb("\r\n>");
}

/*****************************************************************************
* Name:
*    print_greeting
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    --
*
* Assumptions:
*    --
*****************************************************************************/
#if 0
static void usb_print_greeting(void)
{
  print_usb("BRTOS Started!\r\n");
}
#endif
/*****************************************************************************
* Name:
*    find_command
* In:
*    name - pointer to command name string
*
* Out:
*    number - Index of command in "commands" array.
*    -1     - Command not found.
*
* Description:
*    Find a command by its name.
*
* Assumptions:
*    --
*****************************************************************************/
static int usb_find_command(char *name)
{
  int x;
  for(x=0; x < usb_n_cmd; x++)
  { /* If command found, execute it. */
    if (usb_cmp_str(name, (char *) usb_cmds[x]->txt))
    {
      return(x);
    }
  }
  return(-1);
}


/*****************************************************************************
* Name:
*    terminal_init
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Inicialise the terminal. Set local varaibles to a default value, print
*    greeting message and prompt.
*
* Assumptions:
*    --
*****************************************************************************/
void usb_terminal_init(unsigned char (*putch_)(char))
{
  usb_cmd_line[sizeof(usb_cmd_line)-1]='\0';
  usb_cmd_line_ndx=0;

  usb_cmds[0]=(void *)&usb_help_cmd;
  usb_n_cmd=1;
  
  putch=putch_;

  //usb_print_greeting();
  //usb_print_prompt();
}

/*****************************************************************************
* Name:
*    terminal_proces
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Main loop of terminal application. gathers input, and executes commands.
*
* Assumptions:
*    --
*****************************************************************************/
void usb_terminal_process(void)
{
  INT8U data;
  char c;
  
  while(1)
  {
	(void)OSQueuePend(USB, &data, 0);
	c=(char)data;
    
    if ((c != '\n') && (c != '\r'))
    {
      if (c != 0x7F)
      {
    	  if (SilentMode == FALSE)
    	  {
    		  while(c!=(char)(*putch)(c)){};
    	  }
      }else
      {
        if (usb_cmd_line_ndx)
        {
          while(c!=(char)(*putch)(c)){};
        }
      }
    }
    
    /* Execute command if enter is received, or usb_cmd_line is full. */
    if ((c=='\r') || (usb_cmd_line_ndx == sizeof(usb_cmd_line)-2))
    {
      int usb_start = usb_skipp_space(usb_cmd_line, 0);
      int usb_end = usb_find_word(usb_cmd_line, usb_start);
      int usb_x;

      /* Separate command string from parameters, and close
         parameters string. */
      usb_cmd_line[usb_end]=usb_cmd_line[usb_cmd_line_ndx]='\0';

      /* Identify command. */
      usb_x=usb_find_command(usb_cmd_line+usb_start);
      
      /* Command not found. */
      if (usb_x == -1)
      {
        print_usb("\r\nUnknown command!\r\n");
      }
      else
      {
        (*usb_cmds[usb_x]->func)(usb_cmd_line+usb_end+1);
      }
      usb_cmd_line_ndx=0;
      usb_print_prompt();      
      SetSilentMode(FALSE);
    }
    else
    { /* Put character to usb_cmd_line. */
      if (c=='\b')
      {
        if (usb_cmd_line_ndx > 0)
        {
          usb_cmd_line[usb_cmd_line_ndx]='\0';
          usb_cmd_line_ndx--;
        }
      }
      else if(c=='\n'){
        continue;
      }
      else
      {
          if (c == 0x7F)
          {    	  
              if (usb_cmd_line_ndx)
              {
                  usb_cmd_line[usb_cmd_line_ndx]=0;
                  usb_cmd_line_ndx--;
                  usb_cmd_line[usb_cmd_line_ndx]=0;            	  
              }
          }else
          {
              usb_cmd_line[usb_cmd_line_ndx++]=c;        	  
          }
      }
    }    
   
    if (SilentMode == FALSE)
    {
    	cdc_process();
    }
  }
}


void SetSilentMode(char mode)
{
	if ((mode == TRUE) || (mode == FALSE))
	{
		SilentMode = mode;
	}
}


unsigned char TerminalBackup(char *backup)
{
	unsigned char i = 0;
	
	for(i=0;i<usb_cmd_line_ndx;i++)
	{
		*backup++ = usb_cmd_line[i];
		if (i >= (31)) break;
	}
	
	usb_cmd_line_ndx = 0;
	
	return i;
}


/*****************************************************************************
* Name:
*    terminal_add_cmd
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Main loop of terminal application. gathers input, and executes commands.
*
* Assumptions:
*    --
*****************************************************************************/
int usb_terminal_add_cmd(command_t *cmd)
{
  if (usb_n_cmd >= MAX_CMDS)
  {
    return(1);
  }

  usb_cmds[usb_n_cmd]=cmd;
  usb_n_cmd++;
  return(0);
}

/*****************************************************************************
* Name:
*    terminal_delete_cmd
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Main loop of terminal application. gathers input, and executes commands.
*
* Assumptions:
*    --
*****************************************************************************/
int usb_terminal_delete_cmd(command_t *cmd)
{
  int x;

  for(x=0; x<usb_n_cmd; x++)
  {
    if (usb_cmds[x] == cmd)
    {
      while(x<usb_n_cmd-1)
      {
        usb_cmds[x]=usb_cmds[x+1];
        x++;
      }

      usb_cmds[x]=0;
      usb_n_cmd--;
      
      return(0);
    }
  }
  return(1);
}

#ifdef __cplusplus
}
#endif
/****************************** END OF FILE **********************************/
