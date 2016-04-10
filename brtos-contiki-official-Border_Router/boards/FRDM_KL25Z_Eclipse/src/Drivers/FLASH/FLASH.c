#include "FLASH1.h"

LDD_TDeviceData *FLASHPointer = NULL;

void InitFlash(void)
{
	// Inicializa o módulo de memória FLASH
	if (FLASHPointer == NULL)
	{
		FLASHPointer = FLASH1_Init(NULL);
	}
}

void WriteToFlash(LDD_TData *FromPtr, LDD_FLASH_TAddress ToAddress, LDD_FLASH_TDataSize Size)
{
		LDD_FLASH_TOperationStatus OpStatus;

		FLASH1_Write(FLASHPointer, FromPtr, ToAddress, Size);
		do
		{
			FLASH1_Main(FLASHPointer);

			OpStatus = FLASH1_GetOperationStatus(FLASHPointer);
		} while (!((OpStatus == LDD_FLASH_IDLE) | (OpStatus == LDD_FLASH_FAILED)));
}



void ReadFromFlash(LDD_FLASH_TAddress FromAddress, LDD_TData *ToPtr, LDD_FLASH_TDataSize Size)
{
	LDD_FLASH_TOperationStatus OpStatus;

	FLASH1_Read(FLASHPointer, FromAddress, ToPtr, Size);
	do
	{
		FLASH1_Main(FLASHPointer);
		OpStatus = FLASH1_GetOperationStatus(FLASHPointer);
	} while (!((OpStatus == LDD_FLASH_IDLE) | (OpStatus == LDD_FLASH_FAILED)));
}


void EraseFlash(LDD_FLASH_TAddress FromAddress, LDD_FLASH_TDataSize Size)
{
	LDD_FLASH_TOperationStatus OpStatus;

	FLASH1_Erase(FLASHPointer, FromAddress, Size);
	do
	{
		FLASH1_Main(FLASHPointer);

		OpStatus = FLASH1_GetOperationStatus(FLASHPointer);

	} while (!((OpStatus == LDD_FLASH_IDLE) | (OpStatus == LDD_FLASH_FAILED)));
}

