/*
 * usb_terminal_commands.c
 *
 *  Created on: 12/05/2011
 *      Author: gustavo
 */

#include "usb_terminal.h"
#include "usb_terminal_commands.h"
#include "virtual_com.h"
#include "debug_stack.h"
#include "BRTOS.h"
#include "utils.h"

#define CONSOLE_BUFFER_SIZE 128

INT8U entradas[CONSOLE_BUFFER_SIZE]; //vetor para a entrada de dados


// BRTOS version Command
void usb_cmd_ver(char *param)
{
  (void)*param;
  print_usb("\n\r");
  print_usb((CHAR8*)version);
  print_usb("\n\r");
}

const command_t usb_ver_cmd = {
  "ver", usb_cmd_ver, "BRTOS Version"
};

// TOP Command (similar to the linux command)
void usb_cmd_top(char *param)
{
  (void)*param;
  print_usb("\n\r");
#if (COMPUTES_CPU_LOAD == 1)
  Transmite_CPU_Load(USE_USB);
#endif
  Transmite_Uptime(USE_USB);
  Transmite_RAM_Ocupada(USE_USB);
  Transmite_Task_Stacks(USE_USB);
}

const command_t usb_top_cmd = {
  "top", usb_cmd_top, "BRTOS TOP"
};

// Print a string in the terminal
void cmd_echo(char *param)
{
	print_usb("\n\r");
	print_usb((CHAR8*)param);
}

const command_t echo_cmd = {
  "echo", cmd_echo, "Print the string in the terminal"
};


// Function used to print a string in the terminal by other tasks
void echo (char *string, char Terminalbackup)
{
    INT8U backup_cnt = 0;
    INT8U i = 0;
    char backup[(CONSOLE_BUFFER_SIZE/2)];
	char *command_start = "echo ";
    char *command_end   = "\n\r";

    if (Terminalbackup == TRUE)
    {
    	backup_cnt = TerminalBackup(&backup[0]);
    }

    SetSilentMode(TRUE);
    while (*command_start)
    {
			if (OSQueuePost(USB, *command_start++) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSCleanQueue(USB);
			}
    }

	while (*string)
    {
			if (OSQueuePost(USB, *string++) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSCleanQueue(USB);
			}
    }

    while (*command_end)
    {
			if (OSQueuePost(USB, *command_end++) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSCleanQueue(USB);
			}
    }

    if (Terminalbackup == TRUE)
    {
		for(i=0;i<backup_cnt;i++)
		{
			if (OSQueuePost(USB, backup[i]) == BUFFER_UNDERRUN)
			{
			  // Problema: Estouro de buffer
			  OSCleanQueue(USB);
			}
		}
    }
}

// Core temperature Command
//extern INT16S temperature;
void usb_cmd_temp(char *param)
{
  INT16S temperature_temp;
  CHAR8  string[8];
  (void)*param;

  UserEnterCritical();
  //temperature_temp = temperature;
  temperature_temp = 0;
  UserExitCritical();

  PrintDecimal(temperature_temp, string);
  print_usb("\n\r");
  print_usb((CHAR8*)&string[3]);
  print_usb(" degrees");
}

const command_t usb_temp_cmd = {
  "temp", usb_cmd_temp, "Show core temperature"
};



#if 0
// Reason of Reset Command
void usb_cmd_rst(char *param)
{
  (void)*param;
  print_usb("\n\r");
  Reason_of_Reset(USE_USB);
}

const command_t usb_rst_cmd = {
  "rst", usb_cmd_rst, "CPU Reason of the Reset"
};

// SD Card Read Command
void cmd_cat(char *param)
{  
  INT8U i       = 0;
  INT8U retorno = 0;
  CHAR8 name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8 name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
    name2[i] = 0;
  }      
  
  i = 0;  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name1,name2,(INT8U*)&entradas[0],1);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)ReadFile(name1,VERBOSE_ON);
  }
  else
  {
	  printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }  
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }
}

const command_t cat_cmd = {
  "cat", cmd_cat, "SD Card - Read file"
};

// SD Card List Files Command
void cmd_ls(char *param)
{  
  INT8U i       = 0;
  INT8U retorno = 0;  
  INT8U name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  
  
  (void)*param;
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
  }  

  retorno = API_COMMAND_OK;
  if(retorno==API_COMMAND_OK)
  {
    (void)ListFiles(name1);
  }
  else
  {
	  printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }  
}

const command_t ls_cmd = {
  "ls", cmd_ls, "SD Card - List files"
};


// SD Card Change Dir Command
void cmd_cd(char *param)
{  
  INT8U i       = 0;
  INT8U retorno = 0;
  CHAR8 name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8 name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
    name2[i] = 0;
  }      
  
  i = 0;  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name1,name2,(INT8U*)&entradas[0],1);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)ChangeDir(name1,VERBOSE_ON);
  }
  else
  {
	  printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }  
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }
}

const command_t cd_cmd = {
  "cd", cmd_cd, "SD Card - Change Dir"
};

// Mount SD Card Command
void cmd_mount(char *param)
{  
  (void)*param;
  
  // Initialize SD card
  (void)InitSD(VERBOSE_ON);
}

const command_t mount_cmd = {
  "mount", cmd_mount, "Mount SD Card"
};


// Safely remove SD Card Command
void cmd_sr(char *param)
{  
  (void)*param;
  
  // Remove SD card safely
  (void)SafeRemoveSD(VERBOSE_ON);
}

const command_t sr_cmd = {
  "sr", cmd_sr, "Safely remove the SD Card"
};

#if (NO_WRITE_FUNCTIONS == 0)
// SD Card Delete Command
void cmd_rm(char *param)
{  
  INT8U     i       = 0;
  INT8U     retorno = 0;
  CHAR8     name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8     name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
    name2[i] = 0;
  }      
  
  i = 0;  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name1,name2,(INT8U*)&entradas[0],1);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)DeleteFile(name1, VERBOSE_ON);
  }
  else
  {
	printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }  
}

const command_t rm_cmd = {
  "rm", cmd_rm, "SD Card - Delete File or Directory"
};


// SD Card Rename Command
void cmd_rn(char *param)
{  
  INT8U     i       = 0;
  INT8U     retorno = 0;
  CHAR8 name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8 name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
    name2[i] = 0;
  }      
  i = 0;
  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name2,name1,(INT8U*)&entradas[0],2);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)RenameFile(name1,name2,VERBOSE_ON);
  }
  else
  {
	printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }  
}

const command_t rn_cmd = {
  "rn", cmd_rn, "SD Card - Rename File"
};


// SD Card Create File Command
void cmd_cr(char *param)
{  
  INT8U i       = 0;
  INT8U retorno = 0;
  CHAR8 name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8 name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
  }  
  
  i = 0;
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name1,name2,(INT8U*)&entradas[0],1);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)CreateFile(name1,VERBOSE_ON);
  }
  else
  {
	printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }
}

const command_t cr_cmd = {
  "cr", cmd_cr, "SD Card - Create file"
};


// SD Card Make Dir Command
void cmd_mkdir(char *param)
{  
  INT8U i       = 0;
  INT8U retorno = 0;
  CHAR8 name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8 name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
    name2[i] = 0;
  }      
  
  i = 0;  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name1,name2,(INT8U*)&entradas[0],1);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)CreateDir(name1,VERBOSE_ON);
  }
  else
  {
	printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }  
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }
}

const command_t mkdir_cmd = {
  "mkdir", cmd_mkdir, "SD Card - Make Dir"
};


// SD Card Copy File Command
void cmd_cp(char *param)
{  
  INT8U     i       = 0;
  INT8U     retorno = 0;
  CHAR8 name1[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos  
  CHAR8 name2[(CONSOLE_BUFFER_SIZE/2)]; //vetor para a leitura dos nomes dos arquivos 
  
  for(i=0;i<(CONSOLE_BUFFER_SIZE/2);i++)
  {
    // Limpa o buffer de entradas
    name1[i] = 0;
    name2[i] = 0;
  }      
  i = 0;
  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  retorno = file_name_verify(name2,name1,(INT8U*)&entradas[0],2);
  
  if(retorno==API_COMMAND_OK)
  {
    (void)CopyFile(name1,name2,VERBOSE_ON);
  }
  else
  {
	printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }  
}

const command_t cp_cmd = {
  "cp", cmd_cp, "SD Card - Copy File"
};


// SD Card Write File Test Command
void cmd_wt(char *param)
{  
  INT8U i       = 0;
  INT8U retorno = 0; 
  
  entradas[0] = 0x20;
  i++;
  
  while(*param)
  {
    if (i < CONSOLE_BUFFER_SIZE)
    {  
      entradas[i] = *param;
      i++;
      param++;
    }else
    {
      break;
    }
  }
  
  // verifica o nome digitado
  //retorno = file_name_verify(name1,name2,(INT8U*)&entradas[0],1);
  retorno = API_COMMAND_OK;
  
  if(retorno==API_COMMAND_OK)
  {
    (void)WriteUptimeLog(VERBOSE_ON);
  }
  else
  {
	printSer(USE_USB, (CHAR8*)SD_API_FILE_INVALID);
  }  
  
  for(i=0;i<CONSOLE_BUFFER_SIZE;i++)
  {
    // Limpa o buffer de entradas
    entradas[i] = 0;
  }
}

const command_t wt_cmd = {
  "wt", cmd_wt, "SD Card - Write Uptime - Test for write file"
};
#endif

#endif
