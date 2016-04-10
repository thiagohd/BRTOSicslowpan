#include "BRTOS.h"
#include "spi_cox.h"

#if (ENABLE_SPI0 == TRUE)
INT8U SPI0Data = 0;
#endif

#if (ENABLE_SPI1 == TRUE)
INT8U SPI1Data = 0;
#endif

void init_SPI(INT8U spi) {
	/* ### Init_SPI init code */

	if ((spi == 0) || (spi == 1)) {
		// Configure SPI 0
#if (ENABLE_SPI0 == TRUE)
		if (spi == 0) {
			/* Enables spi0 clock */
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			SIM_SCGC4 |= SIM_SCGC4_SPI0_MASK;

			// Configure GPIO for SPI (função alternativa)
			PORTC_PCR5 = PORT_PCR_MUX(2);
			PORTC_PCR6 = PORT_PCR_MUX(2);
			PORTC_PCR7 = PORT_PCR_MUX(2);

			(void) SPI0_S; /* Read the status register */
			(void) SPI0_D; /* Read the device register */

			/* SPI0C1: SPIE=0,SPE=0,SPTIE=0,MSTR=0,CPOL=0,CPHA=1,SSOE=0,LSBFE=0 */SPI0_C1 =
					0x00; /* The SPRF interrupt flag is cleared when the SPI1 module is disabled. */

			/* SPI0C2: MODFEN=0,BIDIROE=0,SPISWAI=0,SPC0=0 */
			// Modo bidirecional single-wire desativado
			SPI0_C2 = 0x00;

			/* SPI0BR: SPPR2=0,SPPR1=1,SPPR0=0,SPR3=0,SPR2=0,SPR1=0,SPR0=0 */
			/* BaudRateDivisor = (SPPR + 1) × 2(SPR + 1)
			 Configura o clock da porta SPI p/ 4 Mhz --> 24Mhz / (2+1)*2 */SPI0_BR =
					0x20;

			(void) (SPI0_S == 0); /* Dummy read of the SPI1S registr to clear the MODF flag */

			/* SPI0C1: SPIE=0,SPE=1,SPTIE=0,MSTR=1,CPOL=0,CPHA=0,SSOE=0,LSBFE=0 */SPI0_C1 =
					SPI_C1_MSTR_MASK | SPI_C1_SPE_MASK;

			(void) SPI0_S;
		}
#endif

		// Configure SPI 1
#if (ENABLE_SPI1 == TRUE)
		if (spi == 1) {
			/* Enables spi1 clock */
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			SIM_SCGC4 |= SIM_SCGC4_SPI1_MASK;

			// Configure GPIO for SPI (função alternativa)
			PORTD_PCR5 = PORT_PCR_MUX(2);
			PORTD_PCR6 = PORT_PCR_MUX(2);
			PORTD_PCR7 = PORT_PCR_MUX(2);

			(void) SPI1_S; /* Read the status register */
			(void) SPI1_D; /* Read the device register */

			/* SPI1C1: SPIE=0,SPE=0,SPTIE=0,MSTR=0,CPOL=0,CPHA=1,SSOE=0,LSBFE=0 */SPI1_C1 =
					0x00; /* The SPRF interrupt flag is cleared when the SPI1 module is disabled. */

			/* SPI1C2: MODFEN=0,BIDIROE=0,SPISWAI=0,SPC0=0 */
			// Modo bidirecional single-wire desativado
			SPI1_C2 = 0x00;

			/* SPI1BR: SPPR2=0,SPPR1=1,SPPR0=0,SPR3=0,SPR2=0,SPR1=0,SPR0=0 */
			/* BaudRateDivisor = (SPPR + 1) × 2(SPR + 1)
			 Configura o clock da porta SPI p/ 4 Mhz --> 24Mhz / (2+1)*2 */SPI1_BR =
					0x20;

			(void) (SPI1_S == 0); /* Dummy read of the SPI1S registr to clear the MODF flag */

			/* SPI1C1: SPIE=0,SPE=1,SPTIE=0,MSTR=1,CPOL=0,CPHA=0,SSOE=0,LSBFE=0 */SPI1_C1 =
					SPI_C1_MSTR_MASK | SPI_C1_SPE_MASK;

			(void) SPI1_S;
		}
#endif
	}
	/* ### */
}

#if (ENABLE_SPI0 == TRUE)
// Função para enviar dados pela porta SPI
void SPI0_SendChar(INT8U *data, int size) {
	do {
		while (!(SPI0_S & SPI_S_SPTEF_MASK)) {
		}; /* wait until transmit buffer is empty*/

		(void) SPI0_S;
		SPI0_D = *data++; /* Transmit counter*/

		while (!(SPI0_S & SPI_S_SPRF_MASK)) {
		}; /* wait until receive buffer is full*/

		(void) SPI0_S; // Acknowledge flag
		SPI0Data = SPI0_D; // Acknowledge flag
		size--;
	} while (size);
}

void SPI0_GetChar(INT8U *data, int size) {
	INT8U send = 0xFF;
	do {
		SPI0_SendChar(&send, 1);
		*data++ = SPI0Data;
		size--;
	} while (size);
}

#endif

#if (ENABLE_SPI1 == TRUE)
// Função para enviar dados pela porta SPI
void SPI1_SendChar(INT8U data) {
	while (!(SPI1_S & SPI_S_SPTEF_MASK)) {
	}; /* wait until transmit buffer is empty*/

	(void) SPI1_S;
	SPI1_D = data; /* Transmit counter*/

	while (!(SPI1_S & SPI_S_SPRF_MASK)) {
	}; /* wait until receive buffer is full*/

	(void) SPI1_S; // Acknowledge flag
	SPI1Data = SPI1_D; // Acknowledge flag
}

INT8U SPI1_GetChar(void) {
	SPI1_SendChar(0xFF);
	return SPI1Data;
}

#endif
