/********************************************************************************************************
 * @file	tlkusb_cdcCtrl.c
 *
 * @brief	This is the source file for BTBLE SDK
 *
 * @author	BTBLE GROUP
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLK_USB_CDC_ENABLE)
#include "tlklib/usb/cdc/tlkusb_cdcConfig.h"
#include "tlklib/usb/cdc/tlkusb_cdcDefine.h"
#include "tlklib/usb/cdc/tlkusb_cdc.h"
#include "tlklib/usb/cdc/tlkusb_cdcDesc.h"
#include "tlklib/usb/cdc/tlkusb_cdcCtrl.h"
#include "drivers.h"


#define TLKUSB_CDC_EDP_RX_FLAG    (1 << (TLKUSB_CDC_EDP_RX & 7))
#define TLKUSB_CDC2_EDP_RX_FLAG   (1 << (TLKUSB_CDC2_EDP_RX & 7))


static void tlkusb_cdcctrl_write32(uint32 value);
static void tlkusb_cdcctrl_read32(uint32* value);
static int  tlkusb_cdcctrl_reqHandler(uint08 bRequest, uint16 wValue, uint16 wIndex, uint16 wLength);


static int  tlkusb_cdcctrl_init(void);
static void tlkusb_cdcctrl_reset(void);
static void tlkusb_cdcctrl_deinit(void);
static void tlkusb_cdcctrl_handler(void);
static int  tlkusb_cdcctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_cdcctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_cdcctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_cdcctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_cdcctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_cdcctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);

const tlkusb_modCtrl_t sTlkUsbCdcModCtrl = {
	tlkusb_cdcctrl_init, //Init
	tlkusb_cdcctrl_reset, //Reset
	tlkusb_cdcctrl_deinit, //Deinit
	tlkusb_cdcctrl_handler, //Handler
	tlkusb_cdcctrl_getClassInf, //GetClassInf
	tlkusb_cdcctrl_setClassInf, //SetClassInf
	tlkusb_cdcctrl_getClassEdp, //GetClassEdp
	tlkusb_cdcctrl_setClassEdp, //SetClassEdp
	tlkusb_cdcctrl_getInterface, //GetInterface
	tlkusb_cdcctrl_setInterface, //SetInterface
};

static TlkUsbCdcRecvCB sTlkUsbCdcRecvCB = nullptr;




void tlkusb_cdc_regRecvCB(TlkUsbCdcRecvCB cb)
{
	sTlkUsbCdcRecvCB = cb;
}


static int tlkusb_cdcctrl_init(void)
{
	uint08 iso;

	reg_usb_ep_buf_addr(TLKUSB_CDC_EDP_NOTY) = 0x00;
	reg_usb_ep_buf_addr(TLKUSB_CDC_EDP_TX) = 0x00;
	reg_usb_ep_buf_addr(TLKUSB_CDC_EDP_RX) = 0x40;
	#if (TLKUSB_CDC_SECOND_ENABLE)
	reg_usb_ep_buf_addr(TLKUSB_CDC2_EDP_NOTY) = 0x80;
	reg_usb_ep_buf_addr(TLKUSB_CDC2_EDP_TX) = 0x80;
	reg_usb_ep_buf_addr(TLKUSB_CDC2_EDP_RX) = 0xC0;
	#endif
	
	reg_usb_mdev &= ~BIT(3); //vendor command: bRequest[7] = 0
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC);
	
	iso = reg_usb_iso_mode;
	iso &= ~(1 << TLKUSB_CDC_EDP_RX);
	#if (TLKUSB_CDC_SECOND_ENABLE)
	iso &= ~(1 << TLKUSB_CDC2_EDP_RX);
	#endif
	reg_usb_iso_mode = iso;
	
	return TLK_ENONE;
}
static void tlkusb_cdcctrl_reset(void)
{
	reg_usb_ep_ctrl(TLKUSB_CDC_EDP_RX) = FLD_EP_DAT_ACK;
	#if (TLKUSB_CDC_SECOND_ENABLE)
	reg_usb_ep_ctrl(TLKUSB_CDC2_EDP_RX) = FLD_EP_DAT_ACK;
	#endif
}
static void tlkusb_cdcctrl_deinit(void)
{
	

}
static void tlkusb_cdcctrl_handler(void)
{
	if(reg_usb_ep_irq_status & TLKUSB_CDC_EDP_RX_FLAG){
		uint08 index;
		uint08 length;
		uint08 buffer[TLKUSB_CDC_TXRX_EDPSIZE];
		reg_usb_ep_irq_status = TLKUSB_CDC_EDP_RX_FLAG;
		length = reg_usb_ep_ptr(TLKUSB_CDC_EDP_RX);
		reg_usb_ep_ptr(TLKUSB_CDC_EDP_RX) = 0;
		if(length > TLKUSB_CDC_TXRX_EDPSIZE) length = TLKUSB_CDC_TXRX_EDPSIZE;
		for(index=0; index<length; index++){
			buffer[index] = reg_usb_ep_dat(TLKUSB_CDC_EDP_RX);
		}
		reg_usb_ep_ctrl(TLKUSB_CDC_EDP_RX) = FLD_EP_DAT_ACK;
//		//Just For Test
//		tlkusb_cdc_sendData(TLKUSB_CDC_INF_CCI, buffer, length);
		if(sTlkUsbCdcRecvCB != nullptr){
			sTlkUsbCdcRecvCB(TLKUSB_CDC_INF_CCI, buffer, length);
		}
	}
	#if (TLKUSB_CDC_SECOND_ENABLE)
	if(reg_usb_ep_irq_status & TLKUSB_CDC2_EDP_RX_FLAG){
		uint08 index;
		uint08 length;
		uint08 buffer[TLKUSB_CDC_TXRX_EDPSIZE];
		reg_usb_ep_irq_status = TLKUSB_CDC2_EDP_RX_FLAG;
		length = reg_usb_ep_ptr(TLKUSB_CDC2_EDP_RX);
		reg_usb_ep_ptr(TLKUSB_CDC2_EDP_RX) = 0;
		if(length > TLKUSB_CDC_TXRX_EDPSIZE) length = TLKUSB_CDC_TXRX_EDPSIZE;
		for(index=0; index<length; index++){
			buffer[index] = reg_usb_ep_dat(TLKUSB_CDC2_EDP_RX);
		}
		reg_usb_ep_ctrl(TLKUSB_CDC2_EDP_RX) = FLD_EP_DAT_ACK;
		//Just For Test
//		tlkusb_cdc_sendData(TLKUSB_CDC2_INF_CCI, buffer, length);
		if(sTlkUsbCdcRecvCB != nullptr){
			sTlkUsbCdcRecvCB(TLKUSB_CDC2_EDP_RX, buffer, length);
		}
	}
	#endif
}


static int tlkusb_cdcctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(pSetup->bRequest == TLKUSB_CDC_OPCODE_GetLineEncoding){
		return tlkusb_cdcctrl_reqHandler(pSetup->bRequest, pSetup->wValue, pSetup->wIndex, pSetup->wLength);
	}else if(pSetup->bRequest == 0x20){ //notify
		return TLK_ENONE;
	}
	return -TLK_ENOSUPPORT;
}
static int tlkusb_cdcctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(pSetup->bRequest == TLKUSB_CDC_OPCODE_SetLineEncoding
		|| pSetup->bRequest == TLKUSB_CDC_OPCODE_SetControlLineState){
		return tlkusb_cdcctrl_reqHandler(pSetup->bRequest, pSetup->wValue, pSetup->wIndex, pSetup->wLength);
	}
	return -TLK_ENOSUPPORT;
}
static int tlkusb_cdcctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_cdcctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_cdcctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return TLK_ENONE;
}
static int tlkusb_cdcctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return TLK_ENONE;
}


static tlkusb_cdc_param_t sTlkUsbCdcParam = {
	//.Config =
    {
	    TLKUSB_CDC_INF_CCI, //ControlInterfaceNumber
		TLKUSB_CDC_EDP_TX, // DataINEndpointNumber
		TLKUSB_CDC_TXRX_EDPSIZE, // DataINEndpointSize
		false, // DataINEndpointDoubleBank
		TLKUSB_CDC_EDP_RX,  // DataOUTEndpointNumber
		TLKUSB_CDC_TXRX_EDPSIZE, // DataOUTEndpointSize
		false, // DataOUTEndpointDoubleBank
		TLKUSB_CDC_EDP_NOTY, // NotificationEndpointNumber
		TLKUSB_CDC_NOTIFY_EDPSIZE, // NotificationEndpointSize
		false,  // NotificationEndpointDoubleBank
	},
};
#if (TLKUSB_CDC_SECOND_ENABLE)
static tlkusb_cdc_param_t sTlkUsbCdc2Param = {
	//.Config =
    {
	    TLKUSB_CDC2_INF_CCI, //ControlInterfaceNumber
		TLKUSB_CDC2_EDP_TX, // DataINEndpointNumber
		TLKUSB_CDC_TXRX_EDPSIZE, // DataINEndpointSize
		false, // DataINEndpointDoubleBank
		TLKUSB_CDC2_EDP_RX,  // DataOUTEndpointNumber
		TLKUSB_CDC_TXRX_EDPSIZE, // DataOUTEndpointSize
		false, // DataOUTEndpointDoubleBank
		TLKUSB_CDC2_EDP_NOTY, // NotificationEndpointNumber
		TLKUSB_CDC_NOTIFY_EDPSIZE, // NotificationEndpointSize
		false,  // NotificationEndpointDoubleBank
	},
};
#endif


static int tlkusb_cdcctrl_reqHandler(uint08 bRequest, uint16 wValue, uint16 wIndex, uint16 wLength)
{
	tlkusb_cdc_param_t *pParam;

	if(wIndex != TLKUSB_CDC_INF_CCI) pParam = &sTlkUsbCdcParam;
	#if (TLKUSB_CDC_SECOND_ENABLE)
	else if(wIndex != TLKUSB_CDC2_INF_CCI) pParam = &sTlkUsbCdc2Param;
	#endif
	else return -TLK_EPARAM;

	switch(bRequest){
		case TLKUSB_CDC_OPCODE_GetLineEncoding:
            tlkusb_cdcctrl_write32(pParam->param.BaudRateBPS);
			usbhw_write_ctrl_ep_data(pParam->param.CharFormat);
            usbhw_write_ctrl_ep_data(pParam->param.ParityType);
            usbhw_write_ctrl_ep_data(pParam->param.DataBits);
			break;
		case TLKUSB_CDC_OPCODE_SetLineEncoding:
			tlkusb_cdcctrl_read32(&pParam->param.BaudRateBPS);
			pParam->param.CharFormat  = usbhw_read_ctrl_ep_data();
			pParam->param.ParityType  = usbhw_read_ctrl_ep_data();
			pParam->param.DataBits    = usbhw_read_ctrl_ep_data();
			break;
		case TLKUSB_CDC_OPCODE_SetControlLineState:
			pParam->param.HostToDevice = wValue;
			break;
		case TLKUSB_CDC_OPCODE_SendBreak:
			break;
	}
	return TLK_ENONE;
}
static void tlkusb_cdcctrl_write32(uint32 value)
{
	usbhw_write_ctrl_ep_data(value&0xff);
	usbhw_write_ctrl_ep_data((value>>8)&0xff);
	usbhw_write_ctrl_ep_data((value>>16)&0xff);
	usbhw_write_ctrl_ep_data((value>>24)&0xff);
}
static void tlkusb_cdcctrl_read32(uint32* value)
{
	uint32 temp = 0;
	
	*value = usbhw_read_ctrl_ep_data();
	
	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 8) | (*value); 
	temp = 0;

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 16) | (*value); 
	temp = 0;

	temp =  usbhw_read_ctrl_ep_data();
	*value = (temp << 24) | (*value); 
}




#endif //#if (TLK_USB_CDC_ENABLE)

