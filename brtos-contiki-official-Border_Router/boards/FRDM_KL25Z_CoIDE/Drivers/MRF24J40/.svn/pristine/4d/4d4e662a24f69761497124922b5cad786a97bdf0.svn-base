/**********************************************************************************
@file   MRF24J40.c
@brief  Functions to interface with MRF24J40 radio
@authors: Gustavo Weber Denardin
          Carlos Henrique Barriquello

Copyright (c) <2009-2013> <Universidade Federal de Santa Maria>

  * Software License Agreement
  *
  * The Software is owned by the authors, and is protected under 
  * applicable copyright laws. All rights are reserved.
  *
  * The above copyright notice shall be included in
  * all copies or substantial portions of the Software.
  *  
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  * THE SOFTWARE. 
*********************************************************************************/


#include "hardware.h"
#include "BRTOS.h"
#include "MRF24J40.h"
#include "SPI.h"
#include "gpsnet_api.h"


#if (PROCESSOR == COLDFIRE_V1)
#pragma warn_implicitconv off
#endif

#if (PROCESSOR == ARM_Cortex_M0)
#include "FLASH.h"
#endif


/********************** Constantes ***********************************/
#ifdef __GNUC__
	INT8U* mac64Address = (INT8U*)MAC64_MEM_ADDRESS;
#else
	const INT8U mac64Address[8] @MAC64_MEM_ADDRESS = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};
#endif

#define CHANNEL_INIT_VALUE    CHANNEL_26

#if (defined MULTICHANNEL_SUPPORT) && (MULTICHANNEL_SUPPORT==1)
/* table of allowed channels */
#if NUM_ALLOWED_CHANNELS == 1
const INT8U ALLOWED_CHAN_TABLE[NUM_ALLOWED_CHANNELS]={
  CHANNEL_INIT_VALUE
};
#elif NUM_ALLOWED_CHANNELS == 2
const INT8U ALLOWED_CHAN_TABLE[NUM_ALLOWED_CHANNELS]={
  CHANNEL_INIT_VALUE, CHANNEL_20
};
#elif NUM_ALLOWED_CHANNELS == 3
const INT8U ALLOWED_CHAN_TABLE[NUM_ALLOWED_CHANNELS]={
  CHANNEL_INIT_VALUE, CHANNEL_20, CHANNEL_25
};
#elif NUM_ALLOWED_CHANNELS == 4
const INT8U ALLOWED_CHAN_TABLE[NUM_ALLOWED_CHANNELS]={
  CHANNEL_INIT_VALUE, CHANNEL_20, CHANNEL_25,CHANNEL_15
};
#else
 #error " Wrong NUM_ALLOWED_CHANNELS "
#endif


#endif

static INT8U currentRxChannel = (INT8U)CHANNEL_INIT_VALUE;
static INT8U currentTxChannel = (INT8U)CHANNEL_INIT_VALUE;
static INT8U currentTxPower   = (INT8U)TX_POWER_LEVEL;

/* Calibrado para 1us no MCF51QE @ 25,165823 MHz (dentro do laço) */
#if (PROCESSOR == COLDFIRE_V1)
#define DELAY_1US() asm{nop;nop;tpf;tpf;tpf;tpf;tpf;tpf;tpf;tpf;tpf;}
static void delay_us(INT16U qtos_us){  
  while(qtos_us > 0){
    qtos_us--;
    DELAY_1US();
  } 
}
#endif

#if (PROCESSOR == ARM_Cortex_M0)
#define DELAY_1US() __asm						\
						(						\
							"NOP        \n"		\
							"NOP		\n"		\
							"NOP		\n"		\
							"NOP        \n"		\
							"NOP		\n"		\
							"NOP		\n"		\
							"NOP        \n"		\
							"NOP		\n"		\
							"NOP		\n"		\
							"NOP        \n"		\
							"NOP		\n"		\
							"NOP		\n"		\
);

static void delay_us(INT16U qtos_us){
  while(qtos_us > 0){
    qtos_us--;
    DELAY_1US();
  }
}
#endif


static union{
  struct{ 
    INT16U st_RESET;
    INT16U st_CALFIL;
    INT16U st_SLEEP;
    INT16U st_CALVCO;
    INT16U st_TX;
    INT16U st_RX;
    INT16U st_RTSEL1;
    INT16U st_RTSEL2;
  } st_counters;
  INT16U st_counts[8];
} Radio_State_Counter;


void Radio_Count_states (void)
{
  INT8U st = 0;
  st = PHYGetLongRAMAddr(RFSTATE);   
  st = (INT8U)((st & 0xE0) >> 5); 
  Radio_State_Counter.st_counts[st]++;
}


// Inicializa a interrupção externa p/ o radio e os pinos de I/O
void Radio_port_Init(void)
{
#if PROCESSOR == ARM_Cortex_M0
   // Enable pin clock
   PHY_PIN_CLOCK_INIT;

   // Define pins as I/Os
   PHY_CS_AS_IO;
   PHY_RESETn_AS_IO;
   PHY_WAKE_AS_IO;

   // Define pins drive strength to high
   PHY_CS_DS;
   PHY_RESETn_DS;
   PHY_WAKE_DS;
#endif

   PHY_RESETn_LOW;
   PHY_RESETn_DIR_OUT;
   PHY_WAKE_LOW;
   PHY_WAKE_DIR_OUT;
   PHY_CS_HIGH;
   PHY_CS_DIR_OUT;
   
   #ifdef ACTIVITY_LED
   ACTIVITY_LED_AS_IO;
   ACTIVITY_LED_DS;
   ACTIVITY_LED_LOW;
   ACTIVITY_LED_DIR_OUT;
   #endif
   
// Init phy int pin interrupt
#if PROCESSOR == COLDFIRE_V1
   #if (RF_INT_TYPE == RF_INT_TIMER)
    /* TPMSC: TOF=0,TOIE=0,CPWMS=0,CLKSB=0,CLKSA=1,PS2=1,PS1=1,PS0=1 */
    // Bus Clock / 128
    #if (NEEDS_INIT_TIMER == 1)
      RF_TPMSC = 0x0F;                     /* Stop and reset counter */
    
      // Período de 200ms
      RF_MOD = 37500;                      /* Period value setting */
    #endif
    
    (void)(RFFLAG);                      /* Channel 0 int. flag clearing (first part) */  
        
    /* TPMCSC: CH0F=0,CH0IE=1,MS0B=0,MS0A=0,ELS0B=1,ELS0A=0 */
    // Captura de entrada por borda de descida
    RFFLAG = 0x48;                       /* Int. flag clearing (2nd part) and channel 0 contr. register setting */
  #endif
  
  #if (RF_INT_TYPE == RF_INT_KEYB)    
    // Selects falling edge and enables pull-up
    KBI1ES_KBEDG4 = 0;
    
    // Habilita interrupção de teclado KBI4
    RFIE = 1;
    
    // Limpa flag de teclado
    RFIF;
    
    // Habilita interrupções de teclado por edge only
    KBI1SC = 0x02;
  #endif
#endif

#if PROCESSOR == ARM_Cortex_M0
    PHY_INT_ENABLE();
#else
   // hard reset, intialize the variables   
   RFINT_ENABLE;
#endif
   PHY_WAKE_HIGH;
   
   OSSemCreate(0,&RF_Event);
   
}



/*********************************************************************
 * Function:        INT8U MRF24J40Init(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          Success/error
 *
 * Side Effects:    MRF24J40 is initialized
 *
 * Overview:        This function initializes the MRF24J40
 ********************************************************************/
INT8U MRF24J40Init(void)
{  
    volatile INT8U i;
    volatile INT16U j;
    
#if PROCESSOR == COLDFIRE_V1
    iNesting++;       
#endif
      
    PHY_RESETn_LOW;
    for(j=0;j<7000;j++)
    {
    };

    PHY_RESETn_HIGH;
    for(j=0;j<7000;j++)
    {
    };

    /* do a soft reset */
    PHYSetShortRAMAddr(WRITE_SOFTRST,0x07); 
    for(j=0;j<1000;j++){};
    
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
    for(j=0;j<1000;j++){};

    
    // Enable FIFO and TX Enable
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_TXPEMISP, 0x95);    


    /* program the RF and Baseband Register */
    //PHYSetLongRAMAddr(RFCTRL4,0x02);
    
    /* Enable the RX */
    //PHYSetLongRAMAddr(RFRXCTRL,0x01);
    
    /* setup */  
    PHYSetLongRAMAddr(RFCTRL0,0x03);
          
    PHYSetLongRAMAddr(RFCTRL1,RFCTRL1_VAL);
    
    /* enable the RF-PLL */
    PHYSetLongRAMAddr(RFCTRL2,0x80);
    
    /* set TX output power */
    PHYSetLongRAMAddr(RFCTRL3,TX_POWER_LEVEL);
    
    /* program RSSI ADC with 2.5 MHz clock */
    //PHYSetLongRAMAddr(RFCTRL6,0x04);
    PHYSetLongRAMAddr(RFCTRL6,0x90);

    //PHYSetLongRAMAddr(RFCTRL7,0b00000000);
    //PHYSetLongRAMAddr(RFCTRL7,0b10011000);
    PHYSetLongRAMAddr(RFCTRL7,0x80);
    
    PHYSetLongRAMAddr(RFCTRL8,0x10);     
    
    // Disable clockout
    PHYSetLongRAMAddr(SCLKDIV,0x21);


    #if (DEVICE_TYPE != PAN_COORDINATOR)
    // Join network as router
    // and Automatic Acknowledgment enabled
      PHYSetShortRAMAddr(WRITE_RXMCR, 0x04);
    #else
    // Join network as PAN Coordinator
    // and Automatic Acknowledgment enabled
      PHYSetShortRAMAddr(WRITE_RXMCR, 0x0C);
    #endif
    
    /* flush the RX fifo */
    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);    
    
    // Configure Unslotted CSMA-CA Mode
    i = PHYGetShortRAMAddr(READ_TXMCR);
    i = i & 0xDF;
    PHYSetShortRAMAddr(WRITE_TXMCR,i);
    
    // Define Nonbeacon-Enabled Network
    PHYSetShortRAMAddr(WRITE_ORDER,0xFF); 


    /* Program CCA mode using RSSI */
    // 0x80 é o valor default, 0xB8 é o recomendado
    PHYSetShortRAMAddr(WRITE_BBREG2,0xB8);
    //PHYSetShortRAMAddr(WRITE_BBREG2,0x80);
    
    /* Program CCA, RSSI threshold values */
    // Valor Default é 0x00, recomendado é 0x60
    // Determina o níve de sinal que indica o canal como ocupado
    // 0x60 = -69 dBm
    PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x60);
    //PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x00);
    
    /* Enable the packet RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG6,0x40);
    
    /* Program the short MAC Address, 0xffff */
    #if (DEVICE_TYPE != PAN_COORDINATOR)
      PHYSetShortRAMAddr(WRITE_SADRL,0xFF);
      PHYSetShortRAMAddr(WRITE_SADRH,0xFF);
    #else
      PHYSetShortRAMAddr(WRITE_SADRL,0x00);
      PHYSetShortRAMAddr(WRITE_SADRH,0x00);    
    #endif
    
    /* and the short PanId Identifier, 0xffff */
    #if (DEVICE_TYPE != PAN_COORDINATOR)    
      PHYSetShortRAMAddr(WRITE_PANIDL,0xFF);
      PHYSetShortRAMAddr(WRITE_PANIDH,0xFF);
    #else
      PHYSetShortRAMAddr(WRITE_PANIDL,(PANID_INIT_VALUE & 0xFF));
      PHYSetShortRAMAddr(WRITE_PANIDH,((PANID_INIT_VALUE>>8) & 0xFF));      
    #endif
    
    /* Test radio */
    do
    {
        PHYSetShortRAMAddr(WRITE_EADR0,0xCA);
        i = PHYGetShortRAMAddr(READ_EADR0);
        if(i!= 0xCA) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR1,0xF1);
        i = PHYGetShortRAMAddr(READ_EADR1);
        if(i!= 0xF1) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR2,0x05);
        i = PHYGetShortRAMAddr(READ_EADR2);
        if(i!= 0x05) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR3,0xBA);
        i = PHYGetShortRAMAddr(READ_EADR3);
        if(i!= 0xBA) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR4,0xFF);
        i = PHYGetShortRAMAddr(READ_EADR4);
        if(i!= 0xFF) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR5,0x10);
        i = PHYGetShortRAMAddr(READ_EADR5);
        if(i!= 0x10) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR6,0x4E);
        i = PHYGetShortRAMAddr(READ_EADR6);
        if(i!= 0x4E) goto EXIT_ERROR; 
        PHYSetShortRAMAddr(WRITE_EADR7,0x11);
        i = PHYGetShortRAMAddr(READ_EADR7);
        if(i!= 0x11) goto EXIT_ERROR;
        
        break;
        
        EXIT_ERROR:
#if PROCESSOR == COLDFIRE_V1
            iNesting--; 
#endif
            return RADIO_ERROR;  
        
    }while(0);
    
#if (PROCESSOR == ARM_Cortex_M0)
    if ((mac64Address[0] == 0xFF) && (mac64Address[1] == 0xFF) && (mac64Address[2] == 0xFF) && (mac64Address[3] == 0xFF))
    {
    	INT8U tmp_mac64[8] = {EUI_0,EUI_1,EUI_2,EUI_3,EUI_4,EUI_5,EUI_6,EUI_7};

    	// Inicializa o módulo de memória FLASH
    	InitFlash();

    	WriteToFlash((INT8U*)&tmp_mac64, MAC64_MEM_ADDRESS, 8);
    }
#endif
    
    /* Program Long MAC Address*/
    for(i=0;i<8;i++)
    {
        PHYSetShortRAMAddr(WRITE_EADR0+i*2,mac64Address[7-i]);
    }
     
    
    SetChannel(CHANNEL_INIT_VALUE);
    for(j=0;j<1000;j++){};
    
    //i = PHYGetShortRAMAddr(READ_ISRSTS);
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    
    // Habilita interrupções
    PHYSetShortRAMAddr(WRITE_INTMSK,0xF6);
    
    // bypass the errata to make RF communication stable
    PHYSetLongRAMAddr(RFCTRL1, RFCTRL1_VAL);
    
#if PROCESSOR == COLDFIRE_V1
    iNesting--; 
#endif
    return RADIO_OK;
    
}



/*********************************************************************
 * Function:        void MRF24J40Reset(void)
 *
 * PreCondition:    BoardInit (or other initialzation code is required)
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    MRF24J40 is initialized
 *
 * Overview:        This function initializes the MRF24J40 and is required
 *                  before stack operation is available
 ********************************************************************/
void MRF24J40Reset(void)
{  
    volatile INT8U i;
    volatile INT16U j;
    
    Radio_Count_states();
    
    
    PHY_RESETn_LOW;
    for(j=0;j<7000;j++){};

    PHY_RESETn_HIGH;
    for(j=0;j<7000;j++){};  
    
  
    /* do a soft reset */
    PHYSetShortRAMAddr(WRITE_SOFTRST,0x07); 
    for(j=0;j<1000;j++){};
    
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
    for(j=0;j<1000;j++){};    

    
    // Enable FIFO and TX Enable
    //PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_TXPEMISP, 0x95);    
    
    /* setup */  
    PHYSetLongRAMAddr(RFCTRL0,0x03);
          
    PHYSetLongRAMAddr(RFCTRL1,RFCTRL1_VAL);
    
    /* enable the RF-PLL */
    PHYSetLongRAMAddr(RFCTRL2,0x80);
    
    /* set TX output power */
    PHYSetLongRAMAddr(RFCTRL3, currentTxPower);
    
    /* program RSSI ADC with 2.5 MHz clock */
    //PHYSetLongRAMAddr(RFCTRL6,0x04);
    PHYSetLongRAMAddr(RFCTRL6,0x90);

    PHYSetLongRAMAddr(RFCTRL7,0x80);
    
    PHYSetLongRAMAddr(RFCTRL8,0x10);
    
    
    // Disable clockout
    PHYSetLongRAMAddr(SCLKDIV,0x21);
    
    #if (DEVICE_TYPE != PAN_COORDINATOR)
    // Join network as router
    // and Automatic Acknowledgment enabled
      PHYSetShortRAMAddr(WRITE_RXMCR, 0x04);
    #else
    // Join network as PAN Coordinator
    // and Automatic Acknowledgment enabled
      PHYSetShortRAMAddr(WRITE_RXMCR, 0x0C);
    #endif
    
    /* flush the RX fifo */
    PHYSetShortRAMAddr(WRITE_RXFLUSH,0x01);    
    
    // Configure Unslotted CSMA-CA Mode
    i = PHYGetShortRAMAddr(READ_TXMCR);
    i = i & 0xDF;
    PHYSetShortRAMAddr(WRITE_TXMCR,i);
    
    // Define Nonbeacon-Enabled Network
    PHYSetShortRAMAddr(WRITE_ORDER,0xFF);



    /* Program CCA mode using RSSI */
    // 0x80 é o valor default, 0xB8 é o recomendado
    //PHYSetShortRAMAddr(WRITE_BBREG2,0xB8);
    PHYSetShortRAMAddr(WRITE_BBREG2,0xB8);
    //PHYSetShortRAMAddr(WRITE_BBREG2,0x80);
    
    /* Program CCA, RSSI threshold values */
    // Valor Default é 0x00, recomendado é 0x60
    // Determina o níve de sinal que indica o canal como ocupado
    // 0x60 = -69 dBm
    PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x60);
    //PHYSetShortRAMAddr(WRITE_RSSITHCCA,0x00);            
    
    
    /* Enable the packet RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG6,0x40);    
    
    /* Program the short MAC Address, 0xffff */
    #if (DEVICE_TYPE != PAN_COORDINATOR)
      PHYSetShortRAMAddr(WRITE_SADRL,(INT8U)(macAddr & 0xFF));
      PHYSetShortRAMAddr(WRITE_SADRH,(INT8U)(macAddr >> 8));
    #else
      PHYSetShortRAMAddr(WRITE_SADRL,0x00);
      PHYSetShortRAMAddr(WRITE_SADRH,0x00);    
    #endif
    
    /* and the short PanId Identifier, 0xffff */
   // #if (DEVICE_TYPE != PAN_COORDINATOR)  
      PHYSetShortRAMAddr(WRITE_PANIDL,(INT8U)(macPANId & 0xFF));
      PHYSetShortRAMAddr(WRITE_PANIDH,(INT8U)(macPANId >> 8));
    //#else
    //  PHYSetShortRAMAddr(WRITE_PANIDL,0x42);
    //  PHYSetShortRAMAddr(WRITE_PANIDH,0x47);      
    // #endif
    
    // Program Long MAC Address
    for(i=0;i<8;i++)
    {
        PHYSetShortRAMAddr(WRITE_EADR0+i*2,mac64Address[7-i]);
    }     

#if (defined MULTICHANNEL_SUPPORT) && (MULTICHANNEL_SUPPORT==1) 
    SetChannel(currentRxChannel);
#else   
    SetChannel(CHANNEL_INIT_VALUE);
#endif    
    for(j=0;j<1000;j++){};
    
    
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    
    // Habilita interrupções
    PHYSetShortRAMAddr(WRITE_INTMSK,0xF6);
    
    // bypass the errata to make RF communication stable
    PHYSetLongRAMAddr(RFCTRL1, RFCTRL1_VAL);    
}



/*********************************************************************
 * Function:        void SetChannel(BYTE channel)
 *
 * PreCondition:    MRF24J40 is initialized
 *
 * Input:           BYTE channel - this is the channel that you wish
 *                  to operate on.  This should be CHANNEL_11, CHANNEL_12,
 *                  ..., CHANNEL_26. 
 *
 * Output:          None
 *
 * Side Effects:    the MRF24J40 now operates on that channel
 *
 * Overview:        This function sets the current operating channel
 *                  of the MRF24J40
 ********************************************************************/
void SetChannel(INT8U channel)
{
    INT16U z;    
    PHYSetLongRAMAddr(RFCTRL0, (channel | 0x02));
    PHYSetShortRAMAddr(WRITE_RFCTL,0x04);
    for(z=0;z<600;z++){};
    PHYSetShortRAMAddr(WRITE_RFCTL,0x00);
    for(z=0;z<600;z++){};
#if (defined MULTICHANNEL_SUPPORT) && (MULTICHANNEL_SUPPORT==1)
    currentTxChannel = channel;
    //currentRxChannel = channel;
#endif    
}


INT8U GetChannel(void){ 
  return currentRxChannel;
}

void SetRxChannel(INT8U channel)
{
    OS_SR_SAVE_VAR;
    OSEnterCritical();
    currentRxChannel = channel; 
    OSExitCritical();
}

#if (defined MULTICHANNEL_SUPPORT) && (MULTICHANNEL_SUPPORT==1)

/* return allowed channel by index */
INT8U GetAllowedChannels(INT8U index){
  
  if (index >= NUM_ALLOWED_CHANNELS) index = NUM_ALLOWED_CHANNELS-1;
  
  return ALLOWED_CHAN_TABLE[index];

}
#endif


void SetTxPower(INT8U power_level){
    
    OS_SR_SAVE_VAR;
    OSEnterCritical();
    
    currentTxPower = power_level;
         
    iNesting++;
      PHYSetLongRAMAddr(RFCTRL3,currentTxPower);
    iNesting--;
    
    OSExitCritical();
  
}

INT8U GetTxPower(void){
    
    return  currentTxPower;   
}





/*********************************************************************
 * Function:        void PHYSetLongRAMAddr(WORD address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the LONG RAM address
 *                      that you want to write to
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a LONG RAM address
 ********************************************************************/
void PHYSetLongRAMAddr(INT16U address, INT8U value)
{
    INT8U data = 0;
    INT16U local_address = 0;
    OS_SR_SAVE_VAR;
    
    // Enter Critical Section
    OSEnterCritical();
    
    data = value;
    local_address = address;
    
    PHY_CS_LOW;
    SPI_SendChar((((INT8U)(local_address>>3))&0b01111111)|0x80);
    SPI_SendChar((((INT8U)(local_address<<5))&0b11100000)|0x10);
    SPI_SendChar(data);
    PHY_CS_HIGH;
    
    // Exit Critical Section
    OSExitCritical();
}



/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to write to.  Should use the
 *                      WRITE_ThisAddress definition in the MRF24J40 
 *                      include file.
 *                  BYTE value is the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *
 * Overview:        This function writes a value to a short RAM address
 ********************************************************************/
void PHYSetShortRAMAddr(INT8U address, INT8U value)
{
    INT8U data = 0;
    INT8U local_address = 0;
    OS_SR_SAVE_VAR;

    // Enter Critical Section
    OSEnterCritical();
    
    data = value;
    local_address = address;    
    
    PHY_CS_LOW;
    SPI_SendChar(local_address);
    SPI_SendChar(data);
    PHY_CS_HIGH;
    
    // Exit Critical Section
    OSExitCritical();
}



/*********************************************************************
 * Function:        void PHYSetShortRAMAddr(BYTE address, BYTE value)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           BYTE address is the address of the short RAM address
 *                      that you want to read from.  Should use the
 *                      READ_ThisAddress definition in the MRF24J40 
 *                      include file.
 *
 * Output:          BYTE the value read from the specified short register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a short RAM address
 ********************************************************************/
INT8U PHYGetShortRAMAddr(INT8U address)
{
    INT8U local_address = 0;
    INT8U toReturn;
    OS_SR_SAVE_VAR;
    
    // Enter Critical Section
    OSEnterCritical();
    
    local_address = address;
    
    PHY_CS_LOW;
    SPI_SendChar(local_address);
    
    // Sai da condição crítica na função SPIGet
    toReturn = SPI_Get();   
    PHY_CS_HIGH;
    
    // Exit Critical Section
    OSExitCritical();
    
    return toReturn;
}



/*********************************************************************
 * Function:        BYTE PHYGetLongRAMAddr(WORD address)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD address is the address of the long RAM address
 *                      that you want to read from.  
 *
 * Output:          BYTE the value read from the specified Long register
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a value from a long RAM address
 ********************************************************************/
INT8U PHYGetLongRAMAddr(INT16U address)
{
    INT16U local_address = 0;
    INT8U toReturn = 0;
    OS_SR_SAVE_VAR;
    
    // Enter Critical Section
    OSEnterCritical();
    
    local_address = address;    
    
    PHY_CS_LOW;
    SPI_SendChar(((local_address>>3)&0b01111111)|0x80);
    SPI_SendChar(((local_address<<5)&0b11100000));
    
    // Sai da condição crítica na função SPIGet
    toReturn = SPI_Get();
    PHY_CS_HIGH;
    
    // Exit Critical Section
    OSExitCritical();

    return toReturn;
}



/*********************************************************************
 * Function:        void PHYSetDeviceAddress(WORD PANID, WORD shortAddress)
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           WORD PANID is the PANID you want to operate on
 *                  WORD shortAddress is the short address you want to use on that network
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the short address for the IEEE address
 *                      filtering
 ********************************************************************/
void PHYSetDeviceAddress(INT16U PANID, INT16U shortAddress)
{
    PHYSetShortRAMAddr(WRITE_SADRL,(INT8U)(PANID&0x00FF));
    PHYSetShortRAMAddr(WRITE_SADRH,(INT8U)(PANID>>8));
    PHYSetShortRAMAddr(WRITE_PANIDL,(INT8U)(shortAddress & 0x00FF));
    PHYSetShortRAMAddr(WRITE_PANIDH,(INT8U)(shortAddress>>8));
} 


void PHYSetAutoACK(INT8U autoack){

#if (defined AUTO_ACK_CONTROL) && (AUTO_ACK_CONTROL == 1)
  INT8U reg;
  reg = PHYGetShortRAMAddr(READ_RXMCR);
    
  if(autoack){
    PHYSetShortRAMAddr(WRITE_RXMCR,reg | 0x04);
  }else{
    PHYSetShortRAMAddr(WRITE_RXMCR,reg & ~0x04);
  }  
#else
  (void)autoack;  
#endif  
}

#if (NESTING_INT == 1)
#if PROCESSOR == COLDFIRE_V1
#pragma TRAP_PROC SAVE_NO_REGS
#endif
void Radio_Interrupt(void)
#else
interrupt void Radio_Interrupt(void)
#endif
{
#if PROCESSOR == COLDFIRE_V1
  // ************************
  // Entrada de interrupção
  // ************************
  OS_INT_ENTER();  
  // ************************
#endif

  // Tratamento da interrupção
  (void)RFFLAG;                       // Lê o registrador
  RFIF;                               // Limpa a flag de interrupção do registrador
  
  // Desabilita interrupção de hardware ligada ao pino INT
  RFIINT_DISABLE;

  #if (NESTING_INT == 1)
  OS_ENABLE_NESTING();
  #endif  
  
  OSSemPost(RF_Event);

  // ************************
  // Saída de interrupção
  // ************************
#if PROCESSOR == COLDFIRE_V1
  OS_INT_EXIT();   
  // ************************
#endif

#if PROCESSOR == ARM_Cortex_M0
	OS_INT_EXIT_EXT();
	// ************************
#endif
}
