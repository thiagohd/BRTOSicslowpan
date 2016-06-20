#include "HAL.h"
#include "MKL25Z4.h"
#include "xhw_types.h"
#include "xsysctl.h"
#include "xhw_ints.h"
#include "xcore.h"
#include "xgpio.h"
#include "system.h"

#if 0
void Clock_init(void)
{
// Init system clock
  /* System clock initialization */
  /* SIM_SCGC5: PORTA=1 */
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;   /* Enable clock gate for ports to enable pin routing */
  /* SIM_CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(0x00) | SIM_CLKDIV1_OUTDIV4(0x01)); /* Update system prescalers */
  /* SIM_SOPT2: PLLFLLSEL=0 */
  SIM_SOPT2 &= (uint32_t)~(uint32_t)(SIM_SOPT2_PLLFLLSEL_MASK); /* Select FLL as a clock source for various peripherals */
  /* SIM_SOPT1: OSC32KSEL=0 */
  SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
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
  /* MCG_SC: FCRDIV=1 */
  MCG_SC = (uint8_t)((MCG_SC & (uint8_t)~(uint8_t)(
			MCG_SC_FCRDIV(0x06)
		   )) | (uint8_t)(
			MCG_SC_FCRDIV(0x01)
		   ));
  /* Switch to FEE Mode */
  /* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=1 */
  MCG_C2 = (MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK | MCG_C2_IRCS_MASK);
  /* OSC0_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
  OSC0_CR = OSC_CR_ERCLKEN_MASK;
  /* MCG_C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
  MCG_C1 = (MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
  /* MCG_C4: DMX32=0,DRST_DRS=1 */
  MCG_C4 = (uint8_t)((MCG_C4 & (uint8_t)~(uint8_t)(
			MCG_C4_DMX32_MASK |
			MCG_C4_DRST_DRS(0x02)
		   )) | (uint8_t)(
			MCG_C4_DRST_DRS(0x01)
		   ));
  /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 */
  MCG_C5 = MCG_C5_PRDIV0(0x00);
  /* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
  MCG_C6 = MCG_C6_VDIV0(0x00);
  while((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
  }
  while((MCG_S & 0x0CU) != 0x00U) {    /* Wait until output of the FLL is selected */
  }
  /*** End of PE initialization code after reset ***/

}


void ReEnable_LLWU(void)
{
	if (LLWU_F2 & LLWU_F2_WUF14_MASK)
	{
		LLWU_F2 |= LLWU_F2_WUF14_MASK;
	}
	LLWU_PE4 =  0x20;
}



void Enable_LLS(void)
{
	/////////////////////////////////////////////////////
	//////////      Low Powe Mode LLS			 //////////
	/////////////////////////////////////////////////////

	// Disables the clock monitor
	MCG_C6 &= ~MCG_C6_CME0_MASK;

	// Enables LLS
	SMC_PMPROT |= SMC_PMPROT_ALLS_DEF;
}

void Enable_LLWU(void)
{
	// Enables LLWU interrupt
	xIntEnable(INT_LLWU);

	/* Initialization of the LLWU module */
	LLWU_PE1 = 0;
	LLWU_PE2 = 0;
	LLWU_PE3 = 0;
	LLWU_PE4 =  0x20;

	/* LLWU_ME: WUME7=0,WUME5=0,WUME4=0,WUME1=0,WUME0=1 */
	LLWU_ME = (uint8_t)((LLWU_ME & (uint8_t)~(uint8_t)(
			   LLWU_ME_WUME7_MASK |
			   LLWU_ME_WUME5_MASK |
			   LLWU_ME_WUME4_MASK |
			   LLWU_ME_WUME1_MASK
			  )) | (uint8_t)(
			   LLWU_ME_WUME0_MASK
			  ));

	/* LLWU_FILT1: FILTF=1,FILTE=0,??=0,FILTSEL=0 */
	LLWU_FILT1 = LLWU_FILT1_FILTF_MASK |
				 LLWU_FILT1_FILTE(0x00) |
				 LLWU_FILT1_FILTSEL(0x00);
	/* LLWU_FILT2: FILTF=1,FILTE=0,??=0,FILTSEL=0 */
	LLWU_FILT2 = LLWU_FILT2_FILTF_MASK |
				 LLWU_FILT2_FILTE(0x00) |
				 LLWU_FILT2_FILTSEL(0x00);
}
#endif

#include "MemMapPtr_KL25Z4.h"
void Enable_INT_Pin(void)
{
	// Enables pin clock
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	// Init pin as GPIO, with pull-up enabled
	PORTD_PCR4 = (uint32_t)((PORTD_PCR4 & (uint32_t)~(uint32_t)(
				  PORT_PCR_ISF_MASK |
				  PORT_PCR_MUX(0x06)
				 )) | (uint32_t)(
				  PORT_PCR_MUX(0x01) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK
				 ));

	// Enable interrupt as falling edge
	GPIOPinIntEnable(GPIOD_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);

	// Enable INT
	xIntEnable(INT_PORTD);
}
