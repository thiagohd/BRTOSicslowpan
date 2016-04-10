/*
 * CC2500.c
 *
 *  Created on: Dec 17, 2013
 *      Author: Gustavo
 */

#include "CC2500.h"


void TI_CC_Wait(int wait_us)
{
	int i = 0;
	wait_us = wait_us * 3;
	for(i=0; i<wait_us;i++)
	{
		asm("NOP");
	}
}

void TI_CC_SPIStrobe(char strobe)
{
  TI_CC_CSn_LOW;        					// /CS enable
  SPI0_SendChar(strobe);                    // Send strobe
  TI_CC_CSn_HIGH;         					// /CS disable
}

extern volatile unsigned char SPI0Data;
unsigned char TI_CC_SPIStrobeWithReturn(char strobe)
{
  TI_CC_CSn_LOW;        					// /CS enable
  SPI0_SendChar(strobe);                    // Send strobe
  TI_CC_CSn_HIGH;         					// /CS disable
  
  return SPI0Data;
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_HIGH;         					// /CS disable
  TI_CC_Wait(30);
  TI_CC_CSn_LOW;        					// /CS enable
  TI_CC_Wait(30);
  TI_CC_CSn_HIGH;         					// /CS disable
  TI_CC_Wait(45);

  TI_CC_SPIStrobe(TI_CCxxx0_SRES);
}

void TI_CC_SPIWriteReg(unsigned char addr, unsigned char value)
{
  TI_CC_CSn_LOW;        					// /CS enable
  SPI0_SendChar(addr);                      // Send address
  SPI0_SendChar(value);                     // Load data for TX after addr
  TI_CC_CSn_HIGH;         					// /CS disable
}

void TI_CC_SPIWriteBurstReg(unsigned char addr, unsigned char *buffer, char count)
{
  char i;

  TI_CC_CSn_LOW;        							// /CS enable
  SPI0_SendChar(addr | TI_CCxxx0_WRITE_BURST); 		// Send address
  
  for (i = 0; i < count; i++)
  {
	  SPI0_SendChar(buffer[i]);                     // Send Data
  }
  
  TI_CC_CSn_HIGH;         							// /CS disable
}

unsigned char TI_CC_SPIReadReg(unsigned char addr)
{
  unsigned char x;

  TI_CC_CSn_LOW;        						// /CS enable
  SPI0_SendChar(addr | TI_CCxxx0_READ_SINGLE); 	// Send address
  x = SPI0_GetChar();                           // Read data
  TI_CC_CSn_HIGH;         						// /CS disable

  return x;
}

// For status/strobe addresses, the BURST bit selects between status registers
// and command strobes.
char TI_CC_SPIReadStatus(unsigned char addr)
{
  unsigned char status;
  
  TI_CC_CSn_LOW;        						// /CS enable
  SPI0_SendChar(addr | TI_CCxxx0_READ_BURST); 	// Send address
  status = SPI0_GetChar();                      // Read data
  TI_CC_CSn_HIGH;         						// /CS disable

  return status;
}

void TI_CC_SPIReadBurstReg(unsigned char addr, unsigned char *buffer, char count)
{
  unsigned int i;

  TI_CC_CSn_LOW;        						// /CS enable
  SPI0_SendChar(addr | TI_CCxxx0_READ_BURST); 	// Send address
  
  for (i = 0; i < count; i++)
  {
	  buffer[i] = SPI0_GetChar();               // Read data
  }
  TI_CC_CSn_HIGH;         						// /CS disable
}


// Product = CC2500
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2433.000000 MHz
// Channel spacing = 199.950000 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (11) Serial Clock
void writeRFSettings(void)
{
    // Write register settings
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0B);  // GDO2 output pin config.
    TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06);  // GDO0 output pin config.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0xFF);  // Packet length.
    //TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x05);  // Packet automation control.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x04);  // Packet automation control.
    TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05);  // Packet automation control.
    TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x01);  // Device address.
    TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
    //TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x07); // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x0A); // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x5D); // Freq control word, high byte
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0x93); // Freq control word, mid byte.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0xB1); // Freq control word, low byte.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
    TI_CC_SPIWriteReg(TI_CCxxx0_MCSM0 ,   0x18); //MainRadio Cntrl State Machine
    TI_CC_SPIWriteReg(TI_CCxxx0_FOCCFG,   0x1D); // Freq Offset Compens. Config
    TI_CC_SPIWriteReg(TI_CCxxx0_BSCFG,    0x1C); //  Bit synchronization config.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL2, 0xC7); // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL1, 0x00); // AGC control.
    //TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL0, 0xB2); // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL0, 0xB0); // AGC control.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREND1,   0xB6); // Front end RX configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_FREND0,   0x10); // Front end RX configuration.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL3,   0xEA); // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL2,   0x0A); // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL1,   0x00); // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL0,   0x11); // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_FSTEST,   0x59); // Frequency synthesizer cal.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST2,    0x88); // Various test settings.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST1,    0x31); // Various test settings.
    TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x0B);  // Various test settings.
}

// PATABLE (0 dBm output power)
unsigned char paTable[] = {0xFB};
unsigned char paTableLen = 1;


//-----------------------------------------------------------------------------
//  void RFSendPacket(char *txBuffer, char size)
//
//  DESCRIPTION:
//  This function transmits a packet with length up to 63 bytes.  To use this
//  function, GD00 must be configured to be asserted when sync word is sent and
//  de-asserted at the end of the packet, which is accomplished by setting the
//  IOCFG0 register to 0x06, per the CCxxxx datasheet.  GDO0 goes high at
//  packet start and returns low when complete.  The function polls GDO0 to
//  ensure packet completion before returning.
//
//  ARGUMENTS:
//      char *txBuffer
//          Pointer to a buffer containing the data to be transmitted
//
//      char size
//          The size of the txBuffer
//-----------------------------------------------------------------------------
void RFSendPacket(unsigned char *txBuffer, char size)
{
  volatile unsigned char c1 = 0;
  volatile unsigned char c2 = 0;
  volatile unsigned int hwfault=0;
  
  /*disable receive interrupts */
#if 0
  // Desabilita interrupção de teclado KBI7
  KBI1PE_KBI1PE7 = 0;    
  // Limpa flag de teclado
  KBI1SC_KB1ACK = 1;    
  // Desabilita interrupções de teclado por edge only
  KBI1SC = 0x00;
#endif
  
  /* Turn off reciever. We can ignore/drop incoming packets during transmit. */
  TI_CC_SPIStrobe(TI_CCxxx0_SIDLE);
  
  /* Wait for XOSC to be stable and radio in IDLE state */
  while (TI_CC_SPIStrobeWithReturn( TI_CCxxx0_SNOP ) & 0xF0) ;
  
  /* flush the receive FIFO of any residual data */
  TI_CC_SPIStrobe(TI_CCxxx0_SFRX);
  
  /* clear receive interrupt */
    
  // Write TX data
  TI_CC_SPIWriteBurstReg(TI_CCxxx0_TXFIFO, txBuffer, size);
  
  // Change state to TX, initiating
  TI_CC_SPIStrobe(TI_CCxxx0_STX);           				// data transfer
  
  // Wait GDO0 to go hi -> sync TX'ed
  while (TI_CC_GDO0_PIN_READ == 0)
  {
	  hwfault++;
	  if(hwfault>=1000)
	  {
		  // Re-send TX strobe
		  hwfault = 0;
		  TI_CC_SPIStrobe(TI_CCxxx0_STX);		
	  }
  };								
  
  hwfault = 0;
  while (TI_CC_GDO0_PIN_READ == TI_CC_GDO0_PIN)									// Wait GDO0 to clear -> end of pkt
  {
	  hwfault++;
	  
	  c1=TI_CC_SPIReadStatus(TI_CCxxx0_TXBYTES); //(0)verify that tx fifo is empty 
	  c2=TI_CC_SPIStrobeWithReturn(TI_CCxxx0_SNOP); //(15)state will return to 15 or 31
	  
	  // Se buffer limpo e em modo de recepção
	  if ((c1 == 0) && (c2 == 0x1F)) break;
	  
	  if(hwfault>=640)	break;
  };
  
  // Debug - Deve conter --> C1 = 0 e C2= 15 ou 31
  do
  {
	  c1=TI_CC_SPIReadStatus(TI_CCxxx0_TXBYTES); //(0)verify that tx fifo is empty 
	  c2=TI_CC_SPIStrobeWithReturn(TI_CCxxx0_SNOP); //(15)state will return to 15 or 31
  }while(c2 != 0x1F);
  
  /*
     * Flush the transmit FIFO.  It must be flushed so that
     * the next transmit can start with a clean slate.
     */
  TI_CC_SPIStrobe(TI_CCxxx0_SFTX);
  
  // After pkt TX, this flag is set.
  // Has to be cleared before existing  
  // Habilita interrupção de teclado KBI7
#if 0
  KBI1PE_KBI1PE7 = 1;    
  // Limpa flag de teclado
  KBI1SC_KB1ACK = 1;    
  // Habilita interrupções de teclado por edge only
  KBI1SC = 0x02;
#endif
  
}



//-----------------------------------------------------------------------------
//  char RFReceivePacket(char *rxBuffer, char *length)
//
//  DESCRIPTION:
//  Receives a packet of variable length (first byte in the packet must be the
//  length byte).  The packet length should not exceed the RXFIFO size.  To use
//  this function, APPEND_STATUS in the PKTCTRL1 register must be enabled.  It
//  is assumed that the function is called after it is known that a packet has
//  been received; for example, in response to GDO0 going low when it is
//  configured to output packet reception status.
//
//  The RXBYTES register is first read to ensure there are bytes in the FIFO.
//  This is done because the GDO signal will go high even if the FIFO is flushed
//  due to address filtering, CRC filtering, or packet length filtering.
//
//  ARGUMENTS:
//      char *rxBuffer
//          Pointer to the buffer where the incoming data should be stored
//      char *length
//          Pointer to a variable containing the size of the buffer where the
//          incoming data should be stored. After this function returns, that
//          variable holds the packet length.
//
//  RETURN VALUE:
//      char
//          0x80:  CRC OK
//          0x00:  CRC NOT OK (or no pkt was put in the RXFIFO due to filtering)
//-----------------------------------------------------------------------------
char RFReceivePacket(unsigned char *rxBuffer, char *length)
{
  unsigned char status[2];
  char pktLen;

  if ((TI_CC_SPIReadStatus(TI_CCxxx0_RXBYTES) & TI_CCxxx0_NUM_RXBYTES))
  {
    pktLen = TI_CC_SPIReadReg(TI_CCxxx0_RXFIFO); // Read length byte

    if (pktLen <= *length)                  // If pktLen size <= rxBuffer
    {
      TI_CC_SPIReadBurstReg(TI_CCxxx0_RXFIFO, rxBuffer, pktLen); // Pull data
      *length = pktLen;                     // Return the actual size
      TI_CC_SPIReadBurstReg(TI_CCxxx0_RXFIFO, status, 2);
                                            // Read appended status bytes
      return (char)(status[TI_CCxxx0_LQI_RX]&TI_CCxxx0_CRC_OK);
    }                                       // Return CRC_OK bit
    else
    {
      *length = pktLen;                     // Return the large size
      TI_CC_SPIStrobe(TI_CCxxx0_SFRX);      // Flush RXFIFO
      return 0;                             // Error
    }
  }
  else
      return 0;                             // Error
}

