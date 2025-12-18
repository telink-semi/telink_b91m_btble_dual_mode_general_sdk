/********************************************************************************************************
 * @file	tlkusb_mscCtrl.c
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
#if (TLK_USB_MSC_ENABLE)
#include "drivers.h"
#include "tlklib/usb/msc/tlkusb_mscDefine.h"
#include "tlklib/usb/msc/tlkusb_msc.h"
#include "tlklib/usb/msc/tlkusb_mscDesc.h"
#include "tlklib/usb/msc/tlkusb_mscCtrl.h"
#include "tlklib/usb/msc/tlkusb_mscScsi.h"


static int  tlkusb_mscctrl_init(void);
static void tlkusb_mscctrl_reset(void);
static void tlkusb_mscctrl_deinit(void);
static void tlkusb_mscctrl_handler(void);
static int  tlkusb_mscctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_mscctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_mscctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_mscctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_mscctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_mscctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);

const tlkusb_modCtrl_t sTlkUsbMscModCtrl = {
	tlkusb_mscctrl_init, //Init
	tlkusb_mscctrl_reset, //Reset
	tlkusb_mscctrl_deinit, //Deinit
	tlkusb_mscctrl_handler, //Handler
	tlkusb_mscctrl_getClassInf, //GetClassInf
	tlkusb_mscctrl_setClassInf, //SetClassInf
	tlkusb_mscctrl_getClassEdp, //GetClassEdp
	tlkusb_mscctrl_setClassEdp, //SetClassEdp
	tlkusb_mscctrl_getInterface, //GetInterface
	tlkusb_mscctrl_setInterface, //SetInterface
};




static int tlkusb_mscctrl_init(void)
{
	int index;
	uint08 isoMode;
	tlkusb_msc_disk_t *pDisk;
	
	tlkusb_msc_scsiInit();
	
	reg_usb_ep_buf_addr(TLKUSB_MSC_EDP_IN) = 0x00;
	reg_usb_ep_buf_addr(TLKUSB_MSC_EDP_OUT) = 0x80;
	
	for(index=0; index<TLKUSB_MSC_UNIT_COUNT; index++){
		pDisk = tlkusb_msc_getDisk(index);
		if(pDisk != nullptr && pDisk->Init != nullptr){
			pDisk->Init();
		}
	}
	
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC | FLD_CTRL_EP_AUTO_INTF);
	reg_usb_ep_max_size = 64;

	isoMode = reg_usb_iso_mode;
	isoMode &= ~ BIT(TLKUSB_MSC_EDP_IN);
	isoMode &= ~ BIT(TLKUSB_MSC_EDP_OUT);
	reg_usb_iso_mode = isoMode;
	reg_usb_ep8_fifo_mode = 0;
	usbhw_reset_ep_ptr(TLKUSB_MSC_EDP_OUT);
	reg_usb_ep_ctrl(TLKUSB_MSC_EDP_OUT) = FLD_EP_DAT_ACK;
			
	return TLK_ENONE;
}
static void tlkusb_mscctrl_reset(void)
{
	reg_usb_ep_ctrl(TLKUSB_MSC_EDP_OUT) = FLD_EP_DAT_ACK; 
	tlkusb_msc_scsiReset();
}
static void tlkusb_mscctrl_deinit(void)
{
	plic_interrupt_disable(IRQ11_USB_ENDPOINT);
}
static void tlkusb_mscctrl_handler(void)
{
	tlkusb_msc_scsiHandler();
}


static int tlkusb_mscctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	if(infNumb == TLKUSB_MSC_INF_MSC){
		if(pSetup->bRequest == MS_REQ_GetMaxLUN){
			uint08 count = tlkusb_msc_getDiskCount();
			if(count != 0){
				usbhw_write_ctrl_ep_data(count-1);
				return TLK_ENONE;
			}
		}else if(pSetup->bRequest == MS_REQ_MassStorageReset){
			return TLK_ENONE;
		}
	}
	return -TLK_ENOSUPPORT;
}



static int tlkusb_mscctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return -TLK_ENOSUPPORT;
}
static int tlkusb_mscctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_mscctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_mscctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return TLK_ENONE;
}
static int tlkusb_mscctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return TLK_ENONE;
}






#endif //#if (TLK_USB_MSC_ENABLE)


