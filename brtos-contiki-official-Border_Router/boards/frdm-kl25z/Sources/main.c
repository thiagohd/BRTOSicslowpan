/* ###################################################################
**     Filename    : main.c
**     Project     : brtos-frdm-kl25z-cw10
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-12, 15:24, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "INT_SVCall.h"
#include "INT_PendableSrvReq.h"
#include "INT_SysTick.h"
#include "INT_USB0.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

#include "BRTOS.h"
#include "tasks.h"

void  Mcu_Init(void);

void  Mcu_Init(void)
{
#if ((USB_DEMO == 1))
		/* System clock initialization */
		/* SIM_SCGC5: PORTA=1 */
		SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;   /* Enable clock gate for ports to enable pin routing */
		/* SIM_CLKDIV1: OUTDIV1=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
		SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0x01) | SIM_CLKDIV1_OUTDIV4(0x01)); /* Update system prescalers */
		/* SIM_SOPT2: PLLFLLSEL=1 */
		SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; /* Select PLL as a clock source for various peripherals */
		/* SIM_SOPT1: OSC32KSEL=3 */
		SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0x03); /* LPO 1kHz oscillator drives 32 kHz clock for various peripherals */
		/* SIM_SOPT2: TPMSRC=1 */
		SIM_SOPT2 = (uint32_t)((SIM_SOPT2 & (uint32_t)~(uint32_t)(
					 SIM_SOPT2_TPMSRC(0x02)
					)) | (uint32_t)(
					 SIM_SOPT2_TPMSRC(0x01)
					));                      /* Set the TPM clock */
		/* PORTA_PCR18: ISF=0,MUX=0 */
		PORTA_PCR18 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
		/* PORTA_PCR19: ISF=0,MUX=0 */
		PORTA_PCR19 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
		/* Switch to FBE Mode */
		/* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
		MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
		/* OSC0_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
		OSC0_CR = OSC_CR_ERCLKEN_MASK;
		/* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
		MCG_C1 = (MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
		/* MCG_C4: DMX32=0,DRST_DRS=0 */
		MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
		/* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=1,PRDIV0=1 */
		MCG_C5 = (MCG_C5_PLLSTEN0_MASK | MCG_C5_PRDIV0(0x01));
		/* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
		MCG_C6 = MCG_C6_VDIV0(0x00);
		while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
		}
		while((MCG_S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
		}
		/* Switch to PBE Mode */
		/* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=0 */
		MCG_C6 = (MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x00));
		while((MCG_S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
		}
		while((MCG_S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until locked */
		}
		/* Switch to PEE Mode */
		/* MCG_C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
		MCG_C1 = (MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
		while((MCG_S & 0x0CU) != 0x0CU) {    /* Wait until output of the PLL is selected */
		}
		/* MCG_C6: CME0=1 */
		MCG_C6 |= MCG_C6_CME0_MASK;          /* Enable the clock monitor */
		/*** End of PE initialization code after reset ***/

		PMC_REGSC |= 0x01;
		#endif

}

void main_app(void);

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
   Mcu_Init();
   
   main_app();
  
#if 0   
  /* call BRTOS initialization */
  	BRTOS_Init();
  	
  	if(InstallTask(&System_Time,"System Time",320,31,NULL) != OK)
  	{
  		// Oh Oh, não deveria entrar aqui !!!
  		while(1){};
  	};
  	
  #if USB_DEMO
  	if(InstallTask(&Task_USB,"USB Handler",768,30,NULL) != OK)
  	{
  		// Oh Oh, não deveria entrar aqui !!!
  		while(1){};
  	};
  #endif	
  		
  	
	if (BRTOSStart() != OK)
	{
		for(;;){};
	}
#endif	
    
  

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
