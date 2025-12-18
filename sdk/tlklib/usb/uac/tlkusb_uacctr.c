/********************************************************************************************************
 * @file	tlkusb_uacctr.c
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
#if (TLK_USB_UAC_ENABLE)
#include "drivers.h"
#include "tlklib/usb/uac/tlkusb_uacDefine.h"
#include "tlklib/usb/uac/tlkusb_uacDesc.h"
#include "tlklib/usb/uac/tlkusb_uac.h"
#include "tlklib/usb/uac/tlkusb_uacCtr.h"
#include "tlklib/usb/uac/tlkusb_uacSpk.h"
#include "tlklib/usb/uac/tlkusb_uacMic.h"


static int  tlkusb_uacctrl_init(void);
static void tlkusb_uacctrl_reset(void);
static void tlkusb_uacctrl_deinit(void);
static void tlkusb_uacctrl_handler(void);
static int  tlkusb_uacctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_uacctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_uacctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_uacctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb);
static int  tlkusb_uacctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);
static int  tlkusb_uacctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb);

const tlkusb_modCtrl_t sTlkUsbUacModCtrl = {
	tlkusb_uacctrl_init, //Init
	tlkusb_uacctrl_reset, //Reset
	tlkusb_uacctrl_deinit, //Deinit
	tlkusb_uacctrl_handler, //Handler
	tlkusb_uacctrl_getClassInf, //GetClassInf
	tlkusb_uacctrl_setClassInf, //SetClassInf
	tlkusb_uacctrl_getClassEdp, //GetClassEdp
	tlkusb_uacctrl_setClassEdp, //SetClassEdp
	tlkusb_uacctrl_getInterface, //GetInterface
	tlkusb_uacctrl_setInterface, //SetInterface
};



static int tlkusb_uacctrl_init(void)
{
	uint08 iso;
	uint08 irqMsk = 0;

	reg_usb_ep_buf_addr(TLKUSB_UAC_EDP_MIC) = 0x00;
	reg_usb_ep_buf_addr(TLKUSB_UAC_EDP_SPK) = 0x80;
	reg_usb_ep_max_size = 64;

	iso = reg_usb_iso_mode;
	#if (TLKUSB_UAC_MIC_ENABLE)
	iso |= (1 << TLKUSB_UAC_EDP_MIC);
	irqMsk |= BIT(TLKUSB_UAC_EDP_MIC); //audio in interrupt enable
	#endif
	#if (TLKUSB_UAC_SPK_ENABLE)
	iso |= (1 << TLKUSB_UAC_EDP_SPK);
	irqMsk |= BIT(TLKUSB_UAC_EDP_SPK);
	#endif
	reg_usb_iso_mode = iso;
	reg_usb_ep_irq_mask = irqMsk;
	reg_usb_ep8_fifo_mode = 0;	// no fifo mode
	plic_set_priority(IRQ11_USB_ENDPOINT, 2);
	plic_interrupt_enable(IRQ11_USB_ENDPOINT);
	
	return TLK_ENONE;
}
static void tlkusb_uacctrl_reset(void)
{
	#if (TLKUSB_UAC_SPK_ENABLE)
	reg_usb_ep_ctrl(TLKUSB_UAC_EDP_SPK) = FLD_EP_DAT_ACK;
	#endif
}
static void tlkusb_uacctrl_deinit(void)
{
	plic_interrupt_disable(IRQ11_USB_ENDPOINT);
}
static void tlkusb_uacctrl_handler(void)
{
	
}

static int tlkusb_uacctrl_getClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	int ret = -TLK_ENOSUPPORT;
	uint08 entity = (pSetup->wIndex >> 8);
	uint08 valueH = (pSetup->wValue >> 8) & 0xff;
	switch(entity){
		#if (TLKUSB_UAC_SPK_ENABLE)
		case USB_SPEAKER_FEATURE_UNIT_ID:
			ret = tlkusb_uacspk_getInfCmdDeal(pSetup->bRequest, valueH);
			break;
		#endif
		#if (TLKUSB_UAC_MIC_ENABLE)
		case USB_MIC_FEATURE_UNIT_ID:
			ret = tlkusb_uacmic_getInfCmdDeal(pSetup->bRequest, valueH);
			break;
		#endif
		default:
			break;
	}
	return ret;
}
static int tlkusb_uacctrl_setClassInf(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	int ret;
	uint08 valueH = (pSetup->wValue >> 8) & 0xff;
	uint08 entity = (pSetup->wIndex >> 8) & 0xff;
	
	ret = -TLK_ENOSUPPORT;
	switch(entity){
		#if (TLKUSB_UAC_SPK_ENABLE)
		case USB_SPEAKER_FEATURE_UNIT_ID:
			ret = tlkusb_uacspk_setInfCmdDeal(valueH);
			break;
		#endif
		#if (TLKUSB_UAC_MIC_ENABLE)
		case USB_MIC_FEATURE_UNIT_ID:
			ret = tlkusb_uacmic_setInfCmdDeal(valueH);
			break;
		#endif
		default:
			break;
	}
	return ret;
}
static int tlkusb_uacctrl_getClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	

	return TLK_ENONE;
}
static int tlkusb_uacctrl_setClassEdp(tlkusb_setup_req_t *pSetup, uint08 edpNumb)
{
	int ret;
	uint08 valueH = (pSetup->wValue >> 8) & 0xff;

	ret = -TLK_ENOSUPPORT;
	switch(edpNumb){
		#if (TLKUSB_UAC_SPK_ENABLE)
		case TLKUSB_UAC_EDP_SPK:
			ret = tlkusb_uacspk_setEdpCmdDeal(valueH);
			break;
		#endif
		#if (TLKUSB_UAC_MIC_ENABLE)
		case TLKUSB_UAC_EDP_MIC:
			ret = tlkusb_uacmic_setEdpCmdDeal(valueH);
			break;
		#endif
		default:
			break;
	}	
	return ret;
}
static int tlkusb_uacctrl_getInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	uint08 infNum = (pSetup->wIndex) & 0x07;
	#if (TLKUSB_UAC_MIC_ENABLE)
	if(infNum == TLKUSB_AUD_INF_MIC){
		usbhw_write_ctrl_ep_data(tlkusb_uacmic_getEnable());
	}
	#endif
	#if (TLKUSB_UAC_SPK_ENABLE)
	if(infNum == TLKUSB_AUD_INF_SPK){
		usbhw_write_ctrl_ep_data(tlkusb_uacspk_getEnable());
	}
	#endif
	return TLK_ENONE;
}
static int tlkusb_uacctrl_setInterface(tlkusb_setup_req_t *pSetup, uint08 infNumb)
{
	uint08 enable = (pSetup->wValue) & 0xff;
	uint08 infNum = (pSetup->wIndex) & 0x07;

	#if (TLKUSB_UAC_MIC_ENABLE)
	if(infNum == TLKUSB_AUD_INF_MIC){
		tlkusb_uacmic_setEnable(enable);
	}
	#endif
	#if (TLKUSB_UAC_SPK_ENABLE)
	if(infNum == TLKUSB_AUD_INF_SPK){
		tlkusb_uacspk_setEnable(enable);
	}
	#endif
	return TLK_ENONE;
}



#endif //#if (TLK_USB_UAC_ENABLE)

