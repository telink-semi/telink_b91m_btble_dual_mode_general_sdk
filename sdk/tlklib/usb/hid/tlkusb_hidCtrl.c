/********************************************************************************************************
 * @file	tlkusb_hidCtrl.c
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
#if (TLK_USB_HID_ENABLE)
#include "drivers.h"
#include "tlklib/usb/hid/tlkusb_hidConfig.h"
#include "tlklib/usb/hid/tlkusb_hidDefine.h"
#include "tlklib/usb/hid/tlkusb_hid.h"
#include "tlklib/usb/hid/tlkusb_hidDesc.h"
#include "tlklib/usb/hid/tlkusb_hidCtrl.h"



static int  tlkusb_hidctrl_init(void);
static void tlkusb_hidctrl_reset(void);
static void tlkusb_hidctrl_deinit(void);
static void tlkusb_hidctrl_handler(void);
static int  tlkusb_hidctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_hidctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_hidctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_hidctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_hidctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_hidctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);

const tlkusb_modCtrl_t sTlkUsbHidModCtrl = {
	tlkusb_hidctrl_init, //Init
	tlkusb_hidctrl_reset, //Reset
	tlkusb_hidctrl_deinit, //Deinit
	tlkusb_hidctrl_handler, //Handler
	tlkusb_hidctrl_getClassInf, //GetClassInf
	tlkusb_hidctrl_setClassInf, //SetClassInf
	tlkusb_hidctrl_getClassEdp, //GetClassEdp
	tlkusb_hidctrl_setClassEdp, //SetClassEdp
	tlkusb_hidctrl_getInterface, //GetInterface
	tlkusb_hidctrl_setInterface, //SetInterface
};



static int tlkusb_hidctrl_init(void)
{
	reg_usb_ep_buf_addr(TLKUSB_HID_EDP_MOUSE_IN) = 0x00;
	reg_usb_ep_buf_addr(TLKUSB_HID_EDP_KEYBOARD_IN) = 0x08;

	reg_usb_mdev &= ~BIT(3); //vendor command: bRequest[7] = 0
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC);
	
	return TLK_ENONE;
}
static void tlkusb_hidctrl_reset(void)
{

}
static void tlkusb_hidctrl_deinit(void)
{
	

}
static void tlkusb_hidctrl_handler(void)
{
	
}

static int tlkusb_hidctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	
	return -TLK_ENOSUPPORT;
}
static int tlkusb_hidctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	
	return -TLK_ENOSUPPORT;
}
static int tlkusb_hidctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_hidctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	return TLK_ENONE;
}
static int tlkusb_hidctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return TLK_ENONE;
}
static int tlkusb_hidctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	return TLK_ENONE;
}




#endif //#if (TLK_USB_HID_ENABLE)

