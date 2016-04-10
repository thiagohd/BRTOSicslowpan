#include <stdint.h>

/* MCU and OS includes */
#include "BRTOS.h"
#include "MKL25Z4.h"
#include "system.h"
#include "xwdt.h"

/* Config. files */
#include "BoardConfig.h"
#include "tasks.h"        /* for tasks prototypes */

void main_app(void);

int main(void) {
	// Init system clock
#if (USB_DEMO != 1)
	xSysCtlClockSet(48000000, xSYSCTL_XTAL_8MHZ | xSYSCTL_OSC_MAIN);
#else
	/* System clock initialization */
	/* SIM_SCGC5: PORTA=1 */
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK; /* Enable clock gate for ports to enable pin routing */
	/* SIM_CLKDIV1: OUTDIV1=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */SIM_CLKDIV1 =
			(SIM_CLKDIV1_OUTDIV1(0x01) | SIM_CLKDIV1_OUTDIV4(0x01)); /* Update system prescalers */
	/* SIM_SOPT2: PLLFLLSEL=1 */SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; /* Select PLL as a clock source for various peripherals */
	/* SIM_SOPT1: OSC32KSEL=3 */SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0x03); /* LPO 1kHz oscillator drives 32 kHz clock for various peripherals */
	/* SIM_SOPT2: TPMSRC=1 */SIM_SOPT2 = (uint32_t) ((SIM_SOPT2
			& (uint32_t) ~(uint32_t) (SIM_SOPT2_TPMSRC(0x02)))
			| (uint32_t) (SIM_SOPT2_TPMSRC(0x01))); /* Set the TPM clock */
	/* PORTA_PCR18: ISF=0,MUX=0 */PORTA_PCR18 &=
			(uint32_t) ~(uint32_t) ((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
	/* PORTA_PCR19: ISF=0,MUX=0 */PORTA_PCR19 &=
			(uint32_t) ~(uint32_t) ((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));
	/* Switch to FBE Mode */
	/* MCG_C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */MCG_C2 =
			(MCG_C2_RANGE0(0x02) | MCG_C2_EREFS0_MASK);
	/* OSC0_CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */OSC0_CR =
			OSC_CR_ERCLKEN_MASK;
	/* MCG_C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */MCG_C1 =
			(MCG_C1_CLKS(0x02) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
	/* MCG_C4: DMX32=0,DRST_DRS=0 */MCG_C4 &= (uint8_t) ~(uint8_t)(
			(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));
	/* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=1,PRDIV0=1 */MCG_C5 =
			(MCG_C5_PLLSTEN0_MASK | MCG_C5_PRDIV0(0x01));
	/* MCG_C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */MCG_C6 = MCG_C6_VDIV0(0x00);
	while ((MCG_S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
	}
	while ((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
	}
	/* Switch to PBE Mode */
	/* MCG_C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=0 */MCG_C6 = (MCG_C6_PLLS_MASK
			| MCG_C6_VDIV0(0x00));
	while ((MCG_S & 0x0CU) != 0x08U) { /* Wait until external reference clock is selected as MCG output */
	}
	while ((MCG_S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until locked */
	}
	/* Switch to PEE Mode */
	/* MCG_C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */MCG_C1 =
			(MCG_C1_CLKS(0x00) | MCG_C1_FRDIV(0x03) | MCG_C1_IRCLKEN_MASK);
	while ((MCG_S & 0x0CU) != 0x0CU) { /* Wait until output of the PLL is selected */
	}
	/* MCG_C6: CME0=1 */MCG_C6 |= MCG_C6_CME0_MASK; /* Enable the clock monitor */
	/*** End of PE initialization code after reset ***/

	PMC_REGSC |= 0x01;
#endif

	// Initialize system and tasks
	main_app();

	return 0;
}
