/********************************************************************************************************
 * @file	tlkusb_core.c
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
#include "tlklib/usb/tlkusb_desc.h"
#include "tlklib/usb/tlkusb_core.h"

#if (TLK_CFG_USB_ENABLE)

#include "drivers.h"
#include "tlklib/usb/udb/tlkusb_udb.h"
#include "tlklib/usb/uac/tlkusb_uac.h"


static void tlkusb_ctrlTranSetupProc(void);
static void tlkusb_ctrlTranDataProc(void);
static void tlkusb_ctrlTranStatusProc(void);
static void tlkusb_ctrlTranSetupReqProc(bool isSetupReq);
static void tlkusb_stdD2HDevReqDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_stdD2HInfReqDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_stdH2DInfReqDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_classD2HInfDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_classD2HEdpDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_classH2DInfDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_classH2DEdpDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_vendorD2HInfDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_vendorD2HDevDeal(tlkusb_setup_req_t *pSetup);


static void tlkusb_getInfReqDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_setInfReqDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_getDeviceDescDeal(tlkusb_setup_req_t *pSetup);
static void tlkusb_getReportDescDeal(tlkusb_setup_req_t *pSetup);


typedef struct{
	uint16 usbID;	//usb id
	uint08 stall;	//stall
	uint08 rptr;	
	uint16 cmdLen;	//command length
	uint16 rspLen;	//response length
	uint08 *pRspData;//response data
}tlkusb_ctrl_t;

uint08 gTlkUsbCurModType = 0;
static tlkusb_ctrl_t sTlkUsbCtrl;
static tlkusb_setup_req_t sMmiUsbCtrlReq;


int tlkusb_core_init(uint16 usbID)
{
	tmemset(&sTlkUsbCtrl, 0, sizeof(tlkusb_ctrl_t));
	
	sTlkUsbCtrl.usbID = usbID;
	tlkusb_module_init(gTlkUsbCurModType);
		
	reg_usb_mdev &= ~ BIT(3); //vendor command: bRequest[7] = 0
	
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC | FLD_CTRL_EP_AUTO_INTF);
	
	return TLK_ENONE;
}
/******************************************************************************
 * Function: tlkusb_core_enable
 * Descript: This function use to enable/disable usb core by change the state of DP pin of USB interface.
 * Params: 
 *		@enable[IN]--1:enable 0:disable
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkusb_core_enable(bool enable)
{
	if(enable) usb_dp_pullup_en(1);
	else usb_dp_pullup_en(0);
}

uint08 tlkusb_get_curMode()
{
	return gTlkUsbCurModType;
}

/******************************************************************************
 * Function: tlkusb_core_handler
 * Descript: This function use to loop through usb devices.
 * Params: None
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkusb_core_handler(void)
{
	uint32 irq = usbhw_get_ctrl_ep_irq();
	if(irq & FLD_CTRL_EP_IRQ_SETUP){
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_SETUP);
		tlkusb_ctrlTranSetupProc();
	}
	if(irq & FLD_CTRL_EP_IRQ_DATA){
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_DATA);
		tlkusb_ctrlTranDataProc();
	}
	if(irq & FLD_CTRL_EP_IRQ_STA){
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_STA);
		tlkusb_ctrlTranStatusProc();
	}

	if(reg_usb_irq_mask & FLD_USB_IRQ_RESET_MASK){
		reg_usb_irq_mask |= FLD_USB_IRQ_RESET_MASK;
		tlkusb_module_reset(gTlkUsbCurModType);
    }

	
	sTlkUsbCtrl.stall = 0;
		
	tlkusb_module_handler(gTlkUsbCurModType);
}

/******************************************************************************
 * Function: tlkusb_ctrlSendResponse
 * Descript: This function use to send response.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkusb_ctrlSendResponse(void)
{
	uint16 len;

	if(sTlkUsbCtrl.rspLen == 0) return;
	if(sTlkUsbCtrl.rspLen >= 8) len = 8;
	else len = sTlkUsbCtrl.rspLen;
	sTlkUsbCtrl.rspLen -= len;
	usbhw_reset_ctrl_ep_ptr();
	while(len-- > 0){
		usbhw_write_ctrl_ep_data(*sTlkUsbCtrl.pRspData);
		++sTlkUsbCtrl.pRspData;
	}
}
/******************************************************************************
 * Function: tlkusb_ctrlTranSetupProc
 * Descript: This function use to transfer setup information.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkusb_ctrlTranSetupProc(void)
{
	sTlkUsbCtrl.stall = 0;
	usbhw_reset_ctrl_ep_ptr();
	sMmiUsbCtrlReq.bReqType = usbhw_read_ctrl_ep_data();
	sMmiUsbCtrlReq.bRequest = usbhw_read_ctrl_ep_data();
	sMmiUsbCtrlReq.wValue = usbhw_read_ctrl_ep_u16();
	sMmiUsbCtrlReq.wIndex = usbhw_read_ctrl_ep_u16();
	sMmiUsbCtrlReq.wLength = usbhw_read_ctrl_ep_u16();
	tlkusb_ctrlTranSetupReqProc(true);
	if(sTlkUsbCtrl.stall) usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	else usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
}
/******************************************************************************
 * Function: tlkusb_ctrlTranDataProc
 * Descript: This function use to transfer data.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkusb_ctrlTranDataProc(void)
{
	sTlkUsbCtrl.stall = 0;
	usbhw_reset_ctrl_ep_ptr();
	tlkusb_ctrlTranSetupReqProc(false);
//	tlkusb_ctrlSendResponse();
	if(sTlkUsbCtrl.stall) usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	else usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
}
/******************************************************************************
 * Function: tlkusb_ctrlTranStatusProc
 * Descript: This function use to transfer status.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkusb_ctrlTranStatusProc(void)
{
	if(sTlkUsbCtrl.stall) usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_STALL);
	else usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_ACK);
}
/******************************************************************************
 * Function: tlkusb_ctrlTranSetupReqProc
 * Descript: This function use to deal with request.
 * Params: 
 *		@isSetupReq[IN]--Whether Setup request or not.
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkusb_ctrlTranSetupReqProc(bool isSetupReq)
{
	uint08 bmReqType = sMmiUsbCtrlReq.bReqType;
	
	usbhw_reset_ctrl_ep_ptr();
	switch(bmReqType){
		case (TLKUSB_REQTYPE_DIR_DEV2HOST | TLKUSB_REQTYPE_MAJ_STAND | TLKUSB_REQTYPE_REC_DEVICE):
			if(isSetupReq) tlkusb_stdD2HDevReqDeal(&sMmiUsbCtrlReq);
			tlkusb_ctrlSendResponse();
			break;
		case (TLKUSB_REQTYPE_DIR_DEV2HOST | TLKUSB_REQTYPE_MAJ_STAND | TLKUSB_REQTYPE_REC_INTERFACE):
			if(isSetupReq) tlkusb_stdD2HInfReqDeal(&sMmiUsbCtrlReq);
			tlkusb_ctrlSendResponse();
			break;
		case (TLKUSB_REQTYPE_DIR_HOST2DEV | TLKUSB_REQTYPE_MAJ_STAND | TLKUSB_REQTYPE_REC_INTERFACE):
			if(isSetupReq) tlkusb_stdH2DInfReqDeal(&sMmiUsbCtrlReq);
			break;
		case (TLKUSB_REQTYPE_DIR_DEV2HOST | TLKUSB_REQTYPE_MAJ_CLASS | TLKUSB_REQTYPE_REC_INTERFACE):
			if(isSetupReq) tlkusb_classD2HInfDeal(&sMmiUsbCtrlReq);
			break;
		case (TLKUSB_REQTYPE_DIR_DEV2HOST | TLKUSB_REQTYPE_MAJ_CLASS | TLKUSB_REQTYPE_REC_ENDPOINT):
			if(isSetupReq) tlkusb_classD2HEdpDeal(&sMmiUsbCtrlReq);
			break;
		case (TLKUSB_REQTYPE_DIR_HOST2DEV | TLKUSB_REQTYPE_MAJ_CLASS | TLKUSB_REQTYPE_REC_INTERFACE):
			if(!isSetupReq) tlkusb_classH2DInfDeal(&sMmiUsbCtrlReq);
			break;
		case (TLKUSB_REQTYPE_DIR_HOST2DEV | TLKUSB_REQTYPE_MAJ_CLASS | TLKUSB_REQTYPE_REC_ENDPOINT):
			if(!isSetupReq) tlkusb_classH2DEdpDeal(&sMmiUsbCtrlReq);
			break;
		case (TLKUSB_REQTYPE_DIR_DEV2HOST | TLKUSB_REQTYPE_MAJ_VENDOR | TLKUSB_REQTYPE_REC_INTERFACE):
			if(!isSetupReq) return;
			tlkusb_vendorD2HInfDeal(&sMmiUsbCtrlReq);
			break;
		case (TLKUSB_REQTYPE_DIR_DEV2HOST | TLKUSB_REQTYPE_MAJ_VENDOR | TLKUSB_REQTYPE_REC_DEVICE):
			if(!isSetupReq) return;
			tlkusb_vendorD2HDevDeal(&sMmiUsbCtrlReq);			
			break;
		case (TLKUSB_REQTYPE_DIR_HOST2DEV | TLKUSB_REQTYPE_MAJ_VENDOR | TLKUSB_REQTYPE_REC_DEVICE):	// 0x40
			sTlkUsbCtrl.stall = 1;
			break;
		default:
			sTlkUsbCtrl.stall = 1;
			break;
	}
}

static void tlkusb_stdD2HDevReqDeal(tlkusb_setup_req_t *pSetup)
{
	sTlkUsbCtrl.rspLen = 0;
	if(pSetup->bRequest == TLKUSB_SETUP_REQ_GET_DESCRIPT){
		tlkusb_getDeviceDescDeal(pSetup);
	}
	
	return;
}
static void tlkusb_stdD2HInfReqDeal(tlkusb_setup_req_t *pSetup)
{
	sTlkUsbCtrl.rspLen = 0;
	if(pSetup->bRequest == TLKUSB_SETUP_REQ_GET_DESCRIPT){
		tlkusb_getReportDescDeal(pSetup);
	}else if(pSetup->bRequest == TLKUSB_SETUP_REQ_GET_INTERFACE){
		tlkusb_getInfReqDeal(pSetup);
	}

}
static void tlkusb_stdH2DInfReqDeal(tlkusb_setup_req_t *pSetup)
{
	if(pSetup->bRequest == TLKUSB_SETUP_REQ_SET_INTERFACE){
		tlkusb_setInfReqDeal(pSetup);
	}
}

static void tlkusb_classD2HInfDeal(tlkusb_setup_req_t *pSetup)
{
	int ret;
	uint08 infNum;

	infNum = (pSetup->wIndex & 0xFF);
	if(pSetup->bRequest == 0x00){
		usbhw_write_ctrl_ep_data(0x00);
		return;
	}
	
	ret = tlkusb_module_getClassInf(gTlkUsbCurModType, pSetup, infNum);
	if(ret != TLK_ENONE){
		sTlkUsbCtrl.stall = true;
	}else{
		sTlkUsbCtrl.stall = false;
	}
}
static void tlkusb_classD2HEdpDeal(tlkusb_setup_req_t *pSetup)
{
	int ret;
	uint08 edpNum;

	edpNum = (pSetup->wIndex & 0x0F);
	ret = tlkusb_module_getClassEdp(gTlkUsbCurModType, pSetup, edpNum);
	if(ret != TLK_ENONE){
		sTlkUsbCtrl.stall = true;
	}else{
		sTlkUsbCtrl.stall = false;
	}
}
static void tlkusb_classH2DInfDeal(tlkusb_setup_req_t *pSetup)
{
	int ret;
	uint08 infNum;

	infNum = (pSetup->wIndex & 0xFF);
	ret = tlkusb_module_setClassInf(gTlkUsbCurModType, pSetup, infNum);
	if(ret != TLK_ENONE){
		sTlkUsbCtrl.stall = true;
	}else{
		sTlkUsbCtrl.stall = false;
	}
}
static void tlkusb_classH2DEdpDeal(tlkusb_setup_req_t *pSetup)
{
	int ret;
	uint08 edpNum;
	
	edpNum = (pSetup->wIndex & 0x0F);
	ret = tlkusb_module_setClassEdp(gTlkUsbCurModType, pSetup, edpNum);
	if(ret != TLK_ENONE){
		sTlkUsbCtrl.stall = true;
	}else{
		sTlkUsbCtrl.stall = false;
	}
}
static void tlkusb_vendorD2HInfDeal(tlkusb_setup_req_t *pSetup)
{
	if(pSetup->bRequest == 0xc0){ // Get board version
		usbhw_reset_ctrl_ep_ptr();
		usbhw_write_ctrl_ep_data(sTlkUsbCtrl.usbID);
		usbhw_write_ctrl_ep_data(sTlkUsbCtrl.usbID >> 8);
	}else{
		sTlkUsbCtrl.stall = 1;
	}
}
static void tlkusb_vendorD2HDevDeal(tlkusb_setup_req_t *pSetup)
{
	if(pSetup->bRequest == 0xc0){ // Get board version
		usbhw_reset_ctrl_ep_ptr();
		usbhw_write_ctrl_ep_data(0x40);
		usbhw_write_ctrl_ep_data(0x25);
		usbhw_write_ctrl_ep_data(0x40);
		usbhw_write_ctrl_ep_data(0x05);
		usbhw_write_ctrl_ep_data(0x03);
		usbhw_write_ctrl_ep_data(0x00);
		usbhw_write_ctrl_ep_data(0x01);
		usbhw_write_ctrl_ep_data(0x00);
	}else if(pSetup->bRequest == 0xc6){
		usbhw_reset_ctrl_ep_ptr();
		usbhw_write_ctrl_ep_data(0x04);
	}else{
		sTlkUsbCtrl.stall = 1;
	}
}


static void tlkusb_getInfReqDeal(tlkusb_setup_req_t *pSetup)
{
	int ret;
	uint08 infNum = (pSetup->wIndex) & 0x07;	
	ret = tlkusb_module_getInterface(gTlkUsbCurModType, pSetup, infNum);
	if(ret == TLK_ENONE) sTlkUsbCtrl.stall = 0;
	else sTlkUsbCtrl.stall = 1;
}
static void tlkusb_setInfReqDeal(tlkusb_setup_req_t *pSetup)
{
	int ret;
	uint08 infNum = (pSetup->wIndex) & 0x07;	
	ret = tlkusb_module_setInterface(gTlkUsbCurModType, pSetup, infNum);
	if(ret == TLK_ENONE) sTlkUsbCtrl.stall = 0;
	else sTlkUsbCtrl.stall = 1;
}

static void tlkusb_getDeviceDescDeal(tlkusb_setup_req_t *pSetup)
{
	uint08 value_l = (pSetup->wValue) & 0xff;
	uint08 value_h = (pSetup->wValue >> 8) & 0xff;
	switch(value_h){
		case TLKUSB_TYPE_DEVICE:
			sTlkUsbCtrl.pRspData = tlkusb_getDeviceDesc();
			sTlkUsbCtrl.rspLen = tlkusb_getDeviceLens();
			break;
		case TLKUSB_TYPE_CONFIGURE:
			sTlkUsbCtrl.pRspData = tlkusb_getConfigDesc();
			sTlkUsbCtrl.rspLen = tlkusb_getConfigLens();
			break;
		case TLKUSB_TYPE_STRING:
			if(TLKUSB_STRING_INDEX_LANGUAGE == value_l){
				sTlkUsbCtrl.pRspData = tlkusb_getLanguageDesc();
				sTlkUsbCtrl.rspLen = tlkusb_getLanguageLens();
			}else if(TLKUSB_STRING_INDEX_VENDOR == value_l){
				sTlkUsbCtrl.pRspData = tlkusb_getVendorDesc();
				sTlkUsbCtrl.rspLen = tlkusb_getVendorLens();
			}else if(TLKUSB_STRING_INDEX_PRODUCT == value_l){
				sTlkUsbCtrl.pRspData = tlkusb_getProductDesc();
				sTlkUsbCtrl.rspLen = tlkusb_getProductLens();
			}else if(TLKUSB_STRING_INDEX_SERIAL == value_l){
				sTlkUsbCtrl.pRspData = tlkusb_getSerialDesc();
				sTlkUsbCtrl.rspLen = tlkusb_getSerialLens();
			}else{
				sTlkUsbCtrl.stall = 1;
			}
			break;
		default:
			sTlkUsbCtrl.stall = 1;
			break;
	}
	if(pSetup->wLength < sTlkUsbCtrl.rspLen){
		sTlkUsbCtrl.rspLen = pSetup->wLength;
	}
}
static void tlkusb_getReportDescDeal(tlkusb_setup_req_t *pSetup)
{
	sTlkUsbCtrl.pRspData = tlkusb_module_getInfDesDesc(gTlkUsbCurModType, pSetup);
	sTlkUsbCtrl.rspLen   = tlkusb_module_getInfDesLens(gTlkUsbCurModType, pSetup);
	if(sTlkUsbCtrl.rspLen == 0 || sTlkUsbCtrl.pRspData == nullptr){
		sTlkUsbCtrl.stall = 1;
	}else{
		if(pSetup->wLength < sTlkUsbCtrl.rspLen){
			sTlkUsbCtrl.rspLen = pSetup->wLength;
		}
	}
}


#endif //#if (TLK_CFG_USB_ENABLE)
