#include "MKL25Z4.h"

void DAC0_Init(void)
{
  /* SIM_SCGC6: DAC0=1 */
  SIM_SCGC6 |= SIM_SCGC6_DAC0_MASK;

  SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

  /* PORTE_PCR30: ISF=0,MUX=0 */
  PORTE_PCR30 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK | PORT_PCR_MUX(0x07)));

  /* DAC0_C0: DACEN=0,DACRFS=0,DACTRGSEL=0,DACSWTRG=0,LPEN=0,DACBTIEN=0,DACBBIEN=0 */
  DAC0_C0 &= (uint8_t)~(uint8_t)(
              DAC_C0_DACEN_MASK |
              DAC_C0_DACRFS_MASK |
              DAC_C0_DACTRGSEL_MASK |
              DAC_C0_DACSWTRG_MASK |
              DAC_C0_LPEN_MASK |
              DAC_C0_DACBTIEN_MASK |
              DAC_C0_DACBBIEN_MASK
             );
  /* DAC0_SR: ??=0,??=0,??=0,??=0,??=0,DACBFRPTF=0,DACBFRPBF=0 */
  DAC0_SR = 0x00;

  /* DAC0_DAT0L: DATA0=0 */
  DAC0_DAT0L = DAC_DATL_DATA0(0x00);
  /* DAC0_DAT0H: ??=0,??=0,??=0,??=0,DATA1=0 */
  DAC0_DAT0H = DAC_DATH_DATA1(0x00);

  /* DAC0_C1: DMAEN=0,??=0,??=0,DACBFMD=0,DACBFEN=0 */
  DAC0_C1 = 0x00;

  /* DAC0_C0: DACEN=1,DACRFS=0,DACTRGSEL=0,DACSWTRG=0,LPEN=0,DACBTIEN=0,DACBBIEN=0 */
  DAC0_C0 = (uint8_t)((DAC0_C0 & (uint8_t)~(uint8_t)(
             DAC_C0_DACRFS_MASK |
             DAC_C0_DACTRGSEL_MASK |
             DAC_C0_DACSWTRG_MASK |
             DAC_C0_LPEN_MASK |
             DAC_C0_DACBTIEN_MASK |
             DAC_C0_DACBBIEN_MASK
            )) | (uint8_t)(
             DAC_C0_DACEN_MASK
            ));
}

void DAC0_SetData(unsigned short data)
{
	  DAC0_DAT0L = (unsigned char)(data & 0xFF);
	  DAC0_DAT0H = (unsigned char)(data >> 8);
}

