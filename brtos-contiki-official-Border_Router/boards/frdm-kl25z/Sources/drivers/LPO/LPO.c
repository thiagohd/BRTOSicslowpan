/* MODULE LPTMR0. */

#include "LPO.h"
#include "cpu.h"

/*
** ===================================================================
**     Method      :  LPTMR0_Init (component Init_LPTMR)
**     Description :
**         This method initializes registers of the LPTMR module
**         according to the Peripheral Initialization settings.
**         Call this method in user code to initialize the module. By
**         default, the method is called by PE automatically; see "Call
**         Init method" property of the component for more details.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LPO_Init(unsigned int module)
{
/* NVIC_IPR7: PRI_28=0xC0 */
NVIC_IPR7 = (uint32_t)((NVIC_IPR7 & (uint32_t)~(uint32_t)(
           NVIC_IP_PRI_28(0x3F)
          )) | (uint32_t)(
           NVIC_IP_PRI_28(0xC0)
          ));

/* SIM_SCGC5: LPTMR=1 */
SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;
/* LPTMR0_CSR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,TCF=0,TIE=0,TPS=0,TPP=0,TFC=0,TMS=0,TEN=0 */
LPTMR0_CSR = LPTMR_CSR_TPS(0x00);
/* LPTMR0_CMR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,COMPARE=0 */
LPTMR0_CMR = LPTMR_CMR_COMPARE(module);
/* LPTMR0_CSR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,TCF=1,TIE=1,TPS=0,TPP=0,TFC=0,TMS=0,TEN=0 */
LPTMR0_CSR = (LPTMR_CSR_TCF_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TPS(0x00));
/* LPTMR0_PSR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,PRESCALE=0,PBYP=1,PCS=1 */
LPTMR0_PSR = LPTMR_PSR_PRESCALE(0x00) |
             LPTMR_PSR_PBYP_MASK |
             LPTMR_PSR_PCS(0x01);
/* LPTMR0_CSR: TCF=0,TEN=1 */
LPTMR0_CSR = (uint32_t)((LPTMR0_CSR & (uint32_t)~(uint32_t)(
              LPTMR_CSR_TCF_MASK
             )) | (uint32_t)(
              LPTMR_CSR_TEN_MASK
             ));

	enable_irq(28);

}
