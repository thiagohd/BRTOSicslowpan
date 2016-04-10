/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: virtual_nic.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file emulates a USB PORT as Network Card.
*****************************************************************************/ 

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "virtual_nic.h"

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif
#if (!USBCFG_DEV_RNDIS_SUPPORT)
#error This application requires USBCFG_DEV_RNDIS_SUPPORT defined one in usb_device_config.h. Please recompile usbd with this option.
#endif
#if !defined(BSP_DEFAULT_ENET_DEVICE)
#error This application requires BSP_DEFAULT_ENET_DEVICE defined one in bsp. Please recompile bsp with this option.
#endif
extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L ,0 ,0}
};
#endif
/*****************************************************************************
 * Constant and Macro's 
 *****************************************************************************/
/* Base unit for ENIT layer is 1Mbps while for RNDIS its 100bps*/
#define ENET_CONVERT_FACTOR         (10000)
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern usb_endpoints_t usb_desc_ep;
extern usb_desc_request_notify_struct_t  desc_callback;
extern _enet_handle g_enet_handle;
cdc_handle_t g_nic_handle;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg);
uint8_t USB_Notif_Callback(uint8_t event, uint16_t value, uint8_t ** data, uint32_t* size, void* arg); 
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
uint8_t g_line_coding[LINE_CODING_SIZE] = 
{
 /*e.g. 0x00,0x10,0x0E,0x00 : 0x000E1000 is 921600 bits per second */
	(LINE_CODE_DTERATE_IFACE>> 0) & 0x000000FF,
	(LINE_CODE_DTERATE_IFACE>> 8) & 0x000000FF,
	(LINE_CODE_DTERATE_IFACE>>16) & 0x000000FF, 		 
	(LINE_CODE_DTERATE_IFACE>>24) & 0x000000FF,
	 LINE_CODE_CHARFORMAT_IFACE,
	 LINE_CODE_PARITYTYPE_IFACE,
	 LINE_CODE_DATABITS_IFACE
};

uint8_t g_abstract_state[COMM_FEATURE_DATA_SIZE] = 
{
	 (STATUS_ABSTRACT_STATE_IFACE>>0) & 0x00FF,
	 (STATUS_ABSTRACT_STATE_IFACE>>8) & 0x00FF																		
};

uint8_t g_country_code[COMM_FEATURE_DATA_SIZE] = 
{
 (COUNTRY_SETTING_IFACE>>0) & 0x00FF,
 (COUNTRY_SETTING_IFACE>>8) & 0x00FF															  
};
static bool start_app;
static uint8_t * g_recv_buff_ptr = NULL;
static uint8_t * g_pre_recv_ptr = NULL;

static uint8_t g_zero_send = 0x00;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 
/**************************************************************************//*!
 *
 * @name  USB_Get_Line_Coding
 *
 * @brief The function returns the Line Coding/Configuraion
 *
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *coding_data)
{   
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *coding_data = g_line_coding;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Line_Coding
 *
 * @brief The function sets the Line Coding/Configuraion
 *
 * @param handle: handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *coding_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT (handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set line coding data*/
        for (count = 0; count < LINE_CODING_SIZE; count++) 
        {          
            g_line_coding[count] = *((*coding_data+USB_SETUP_PKT_SIZE) + count);
        }
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *feature_data)
{   
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_abstract_state;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t * *feature_data)
{   
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_country_code;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT (handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set Abstract State Feature*/
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_abstract_state[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle: handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t * *feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT (handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_country_code[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}
/*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       Invoked from Main_Task. Calls for all types of 
 *                Initializations
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/ 
void TestApp_Init(void)
{       
    cdc_config_struct_t cdc_config;
        
    /* Initialize USB Structures */    
    cdc_config.cdc_application_callback.callback = USB_App_Callback;
    cdc_config.cdc_application_callback.arg = &g_nic_handle;
    cdc_config.vendor_req_callback.callback = NULL;
    cdc_config.vendor_req_callback.arg = NULL;
    cdc_config.class_specific_callback.callback = USB_Notif_Callback;
    cdc_config.class_specific_callback.arg = &g_nic_handle;
    cdc_config.desc_callback_ptr =  &desc_callback;
    
    g_pre_recv_ptr = (uint8_t *)OS_Mem_alloc_uncached(DIC_BULK_OUT_ENDP_PACKET_SIZE);
    /* Initialize the FEC interface */
    VNIC_FEC_Init(&cdc_config);
    /* Initialize the USB interface */
    USB_Class_CDC_Init(0x0, &cdc_config, &g_nic_handle);

    while (TRUE) 
    {
        /* call the periodic task function */      
        USB_CDC_Periodic_Task();           
    }/* Endwhile */   
} 

/******************************************************************************
 * 
 *    @name       VNIC_USB_Send
 *    
 *    @brief      
 *                  
 *    @param      nic_data_ptr
 *    @param      length
 * 
 *    @return     None
 *   
 *****************************************************************************/
void VNIC_USB_Send(uint8_t * nic_data_ptr, uint32_t length, PCB_PTR  enet_pcb)
{   
    /* if total data to be sent to host is greater than 
       DIC_BULK_IN_ENDP_PACKET_SIZE than break it to max endpoint
       size for bulk In -- approach used to increase performance by avoiding 
       large memory allocations and memcopies in application layer */       
    uint32_t usb_tx_len = length + RNDIS_USB_OVERHEAD_SIZE;  
    uint32_t first_packet_size = 0;
    uint8_t * first_send_buff = NULL;
    uint8_t return_status = USB_OK;
    uint8_t zero_send_flag;
    /* RNDIS Protocol defines 1 byte call of 0x00, if 
    transfer size is multiple of endpoint packet size */
    zero_send_flag = (uint8_t)((usb_tx_len%DIC_BULK_IN_ENDP_PACKET_SIZE) ?
         0 : 1);
    
    /* whichver is smaller but not less than RNDIS_USB_OVERHEAD_SIZE */
    first_packet_size = usb_tx_len > DIC_BULK_IN_ENDP_PACKET_SIZE ? 
        DIC_BULK_IN_ENDP_PACKET_SIZE : usb_tx_len;
    
    if(first_packet_size < RNDIS_USB_OVERHEAD_SIZE)
    {/* for DIC_BULK_IN_ENDP_PACKET_SIZE as 8, 16 or 32
        minimun  first_packet_size has to be either usb_tx_len
        (which is definitely greater than RNDIS_USB_OVERHEAD_SIZE)
        or atleast 64 which is the next allowed multiple of 
        DIC_BULK_IN_ENDP_PACKET_SIZE*/ 
        first_packet_size = usb_tx_len > 64 ? 64 : usb_tx_len;
    }
        
    /* we are reserving one DWORD before packet for distingusihing between 
       packet with USB HEADER and the one only with payload */
    first_send_buff = (uint8_t *)OS_Mem_alloc_zero(first_packet_size + 4);
    /*NOTE : currently OS_Mem_alloc also sets all buffers to 0x00 */   
    if(first_send_buff == NULL)
    {
        #if _DEBUG
            printf("first_send_buff malloc failed: VNIC_USB_Send\n");
        #endif  
        return;
    }

    /* Zero marks Packet with USB HEADER 
       non zero value is used to preserve enet_pcb value before packet*/
    *((uint32_t *)first_send_buff) = (uint32_t)0;       
    
    first_send_buff += 4;/* address for actual payload */
    
    /* Prepare USB Header */
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->message_type = LONG_LE_TO_HOST_CONST(REMOTE_NDIS_PACKET_MSG);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->message_len = LONG_LE_TO_HOST(usb_tx_len);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->data_offset = LONG_LE_TO_HOST_CONST(RNDIS_DATA_OFFSET);
    ((PTR_RNDIS_PKT_MSG_STRUCT)first_send_buff)->data_len = LONG_LE_TO_HOST(length);
    
    /* fill rest of first_send_buff buffers with payload as much as possible */
    OS_Mem_copy(nic_data_ptr, first_send_buff + RNDIS_USB_OVERHEAD_SIZE,
        first_packet_size - RNDIS_USB_OVERHEAD_SIZE);   

    if(!(usb_tx_len > first_packet_size))
    {
        /* free the PCB here itself as it has been copied in g_send_buff */
        PCB_free(enet_pcb);
    }
    return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
        first_send_buff, first_packet_size);                
    if(return_status != USB_OK)
    {
        #if _DEBUG
            printf("First USB Send of VNIC_USB_Send FAILED\n");
        #endif  
        return;
    }

    if(usb_tx_len > first_packet_size)
    {       
        /* enet_pcb address has been appended before buffer pointer used for 
           Send data call on USB */
        *((uint32_t *)(nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE)) - 1) = 
            (uint32_t)enet_pcb;
        
        return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
            nic_data_ptr + (first_packet_size - RNDIS_USB_OVERHEAD_SIZE) , 
            usb_tx_len - first_packet_size);
        if(return_status != USB_OK)
        {
            #if _DEBUG
                printf("Second USB Send of VNIC_USB_Send FAILED\n");
            #endif
            return;
        }               
    }

    if(zero_send_flag == 1)
    {
        return_status = USB_Class_CDC_Send_Data(g_nic_handle, DIC_BULK_IN_ENDPOINT,
            &g_zero_send, sizeof(uint8_t));
        if(return_status != USB_OK)
        {
            #if _DEBUG
                printf("Third USB Send of VNIC_USB_Send FAILED\n");
            #endif
            return;
        }
    }
    
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val,void* arg) 
{
    uint32_t handle;          
    handle = *((uint32_t *)arg);
    if(event_type == USB_DEV_EVENT_BUS_RESET)
    {
        start_app=FALSE; 
    }
    else if(event_type == USB_DEV_EVENT_ENUM_COMPLETE)
    {
        /* Queue next receive */
        (void)USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT,
            g_pre_recv_ptr, DIC_BULK_OUT_ENDP_PACKET_SIZE);
        start_app=TRUE; 
    }
    else if(event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    return;
}

/******************************************************************************
 * 
 *    @name        USB_Notif_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
uint8_t USB_Notif_Callback
(
    uint8_t event, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{    
    cdc_handle_t handle;
    uint8_t error = USB_OK;
    handle = *((cdc_handle_t *)arg);
    switch(event)
    {
		case GET_LINE_CODING:
			error = USB_Get_Line_Coding(handle, value, data);
		break;
		case GET_ABSTRACT_STATE:
			error = USB_Get_Abstract_State(handle, value, data);
		break;
		case GET_COUNTRY_SETTING:
			error = USB_Get_Country_Setting(handle, value, data);
		break;
		case SET_LINE_CODING:
			error = USB_Set_Line_Coding(handle, value, data);
		break;
		case SET_ABSTRACT_STATE:
			error = USB_Set_Abstract_State(handle, value, data);
		break;
		case SET_COUNTRY_SETTING:
			error = USB_Set_Country_Setting(handle, value, data);
		break;
		case USB_DEV_EVENT_DATA_RECEIVED:
			{					
				uint8_t * rndis_pkt_msg_data_ptr = NULL;
				uint32_t frame_remaining_len = 0;
				uint32_t message_len = 0;
				if(TRUE == start_app)
				{
					if(g_recv_buff_ptr == NULL) 	   
					{/* preserve address of lower layer buffer -- to use it later to save memory */
						g_recv_buff_ptr = *data;
					}
					
					if(*data == g_recv_buff_ptr)
					{
						/* indicates first part of received packet */
						message_len = LONG_LE_TO_HOST(*((uint32_t *)(*data) + 1));
						
						if(!(message_len%DIC_BULK_OUT_ENDP_PACKET_SIZE))
						{
							/* RNDIS Protocol defines 1 byte call of 0x00, if 
						   transfer size is multiple of endpoint packet size */
							message_len++;
						}
						rndis_pkt_msg_data_ptr = (uint8_t *)OS_Mem_alloc_zero(message_len); 		  
						if(rndis_pkt_msg_data_ptr == NULL)
						{
#if _DEBUG
								printf("1: memalloc failed in USB_Notif_Callback\n");
#endif	
						}
						
						OS_Mem_copy(*data, rndis_pkt_msg_data_ptr, *size);
						
						frame_remaining_len = message_len - *size;			 
					}
				
					/* this is second part of OUT TRANSFER packet received from host */
					
					if(frame_remaining_len)
					{	/* Since first half of packet will be of DIC_BULK_OUT_ENDP_PACKET_SIZE,
						   Call rest at its offset and return*/
						/* required when ethernet packet + usb header is larger than  
						   DIC_BULK_OUT_ENDP_PACKET_SIZE */  
						error = USB_Class_CDC_Recv_Data(handle,DIC_BULK_OUT_ENDPOINT,
							rndis_pkt_msg_data_ptr+DIC_BULK_OUT_ENDP_PACKET_SIZE,
							frame_remaining_len);	
						return error; 
					}
					
					/* NOTE : if execution control reaches here with rndis_pkt_msg_data_ptr
					   as not equal to NULL then the entire Packet was received in one 
					   transaction */
					 
					if(*data != g_recv_buff_ptr)
					{	/* entire ethernet packet with USB header was not received as
						   one transaction */
						rndis_pkt_msg_data_ptr = 
							*data - DIC_BULK_OUT_ENDP_PACKET_SIZE;
						/* because rest of the packet was received at offset */ 
					}
						
					/* re-calculate message_len as it might have changed because 
					   of 1 byte of zero recv */
					message_len = LONG_LE_TO_HOST(*((uint32_t *)rndis_pkt_msg_data_ptr + 1));
					
					/* Queue Another receive on Bulk Out Endpoint */
					VNIC_FEC_Send(arg, (uint8_t *)(rndis_pkt_msg_data_ptr + 
						RNDIS_USB_OVERHEAD_SIZE), message_len - RNDIS_USB_OVERHEAD_SIZE);	
				
					/* Queue next receive */
					(void)USB_Class_CDC_Recv_Data(g_nic_handle, DIC_BULK_OUT_ENDPOINT,
						g_recv_buff_ptr, DIC_BULK_OUT_ENDP_PACKET_SIZE);
				}
			 }
		break;
		case USB_DEV_EVENT_SEND_COMPLETE:
			{
				if(TRUE == start_app)
				{
					if(NULL == data && NULL == size)
					{
						return error; /*This event comes from USB_Service_Cdc_Notif*/
					}
					if(*data == &g_zero_send)
					{	/* No action is needed on 1 byte of 0x00 send after payload */
						return error;
					}
					
					if(*((uint32_t *)(*data) - 1))
					{	/* non zero value before data pointer indicates enet_pcb value */
						PCB_free((PCB_PTR)(*((uint32_t *)(*data) - 1)));
						/* *((uint32_t *)dp_rcv->data_ptr - 1) has enet_pcb to be freed */
					}
					else
					{
						OS_Mem_free(*data - 4);				   
					}
				}
			}
		break;
		case USB_APP_GET_LINK_SPEED:
			if(TRUE == start_app)
			{
				*((uint32_t *)size)= ENET_get_speed(g_enet_handle);
				*((uint32_t *)size) *= ENET_CONVERT_FACTOR; 
			}
		break;
		case USB_APP_GET_LINK_STATUS:
			if(TRUE == start_app)
			{
				*((bool*)size)= ENET_link_status(g_enet_handle);
			}
		break;
        default:
        error = USBERR_INVALID_REQ_TYPE;
    }
   return error;
}

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls the Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task
(
  uint32_t param
)
{   
    UNUSED_ARGUMENT (param)
    TestApp_Init();  
   
}

/* EOF */
