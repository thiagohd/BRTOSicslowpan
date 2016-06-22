
#include "BRTOS.h"
#include "utils.h"

INT32U LWordSwap(INT32U u32DataSwap)
{
    INT32U u32Temp;
    u32Temp= (u32DataSwap & 0xFF000000) >> 24;
    u32Temp+=(u32DataSwap & 0xFF0000)   >> 8;
    u32Temp+=(u32DataSwap & 0xFF00)     << 8;
    u32Temp+=(u32DataSwap & 0xFF)       << 24;
    return(u32Temp);
}


void Print4Digits(INT16U number, INT8U align, CHAR8 *buff)
{
    INT8U caracter = 0;
    INT8U mil,cent,dez;
    INT8U escreve_zero = FALSE;
    INT32U i = 0;   
       
    if (number < 10000)
    {
      if (align == ZEROS_ALIGN)
      {
        escreve_zero = TRUE;
      }

      mil = (number / 1000);
      
      caracter = mil + '0';
      
      if(caracter != '0')
      {
        *(buff + i) = caracter;
        i++;
        escreve_zero = TRUE;
      }
      else
      {
        if(escreve_zero == TRUE)
        {
          *(buff + i) = caracter;
          i++;
        }
        else
        {
          if (align == SPACE_ALIGN)
          {
            *(buff + i) = ' ';
            i++;
          }
        }
      }
      
      cent = ((number - mil*1000)/100);
      caracter = cent + '0';
      if(caracter != '0')
      {
        *(buff + i) = caracter;
        i++;
        escreve_zero = TRUE;
      }
      else
      {
        if(escreve_zero == TRUE)
        {
          *(buff + i) = caracter;
          i++;
        }
        else
        {
          if (align == SPACE_ALIGN)
          {
            *(buff + i) = ' ';
            i++;
          }
        }
      }
      
      dez = ((number - 1000*mil - cent*100)/10);
      caracter = dez + '0';
      if(caracter != '0')
      {    
        *(buff + i) = caracter;
        i++;
      }
      else
      {
        if(escreve_zero == TRUE)
        {
          *(buff + i) = caracter;
          i++;
        }
        else
        {
          if (align == SPACE_ALIGN)
          {
            *(buff + i) = ' ';
            i++;
          }
        }
      }
      
      caracter = (number%10) + '0';
      *(buff + i) = caracter;
      i++;
      *(buff + i) = 0;
    }
}



void Print3Digits(INT16U number, INT8U align, CHAR8 *buff)
{
    INT8U caracter = 0;
    INT8U cent,dez;
    INT8U escreve_zero = FALSE;    
    INT32U i = 0;   
    
    if (number < 1000)
    {
      if (align == ZEROS_ALIGN)
      {
        escreve_zero = TRUE;
      }    
      
      cent = (number/100);
      caracter = cent + '0';
      
      if(caracter != '0')
      {
        *(buff + i) = caracter;
        i++;
        escreve_zero = TRUE;
      }
      else
      {
        if(escreve_zero == TRUE)
        {
          *(buff + i) = caracter;
          i++;
        }
        else
        {
          if (align == SPACE_ALIGN)
          {
            *(buff + i) = ' ';
            i++;
          }
        }
      }
      
      dez = ((number - cent*100)/10);
      caracter = dez + '0';

      if(caracter != '0')
      {
        *(buff + i) = caracter;
        i++;
        escreve_zero = TRUE;
      }
      else
      {
        if(escreve_zero == TRUE)
        {
          *(buff + i) = caracter;
          i++;
        }
        else
        {
          if (align == SPACE_ALIGN)
          {
            *(buff + i) = ' ';
            i++;
          }
        }
      }
          
      caracter = (number%10) + '0';
      *(buff + i) = caracter;
      i++;
      *(buff + i) = 0;      
    }
}



void Print2Digits(INT8U number, INT8U align, CHAR8 *buff)
{
    INT8U caracter = 0;
    INT8U dez;
    INT8U escreve_zero = FALSE;
    INT32U i = 0;    
    
    if (number < 100)
    {
      if (align == ZEROS_ALIGN)
      {
        escreve_zero = TRUE;
      }

      dez = number/10;
      caracter = dez + '0';

      if(caracter != '0')
      {
        *(buff + i) = caracter;
        i++;
        escreve_zero = TRUE;
      }
      else
      {
        if(escreve_zero == TRUE)
        {
          *(buff + i) = caracter;
          i++;
        }
        else
        {
          if (align == SPACE_ALIGN)
          {
            *(buff + i) = ' ';
            i++;
          }
        }
      }
      
      caracter = (number%10) + '0';
      *(buff + i) = caracter;
      i++;
      *(buff + i) = 0;
    }
}
