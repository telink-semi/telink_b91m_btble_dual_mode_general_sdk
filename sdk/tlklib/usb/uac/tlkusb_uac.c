/********************************************************************************************************
 * @file	tlkusb_uac.c
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
#include "tlklib/usb/uac/tlkusb_uacSpk.h"
#include "tlklib/usb/uac/tlkusb_uacMic.h"


extern const tlkusb_modCtrl_t sTlkUsbUacModCtrl;
extern const tlkusb_modDesc_t sTlkUsbUacModDesc;



int	sTlkUsbUacMute = 0;
uint32 sTlkUsbUacSamplRate = 16000;
const tlkusb_module_t gTlkUsbUacModule = {
	TLKUSB_MODTYPE_UAC,
	&sTlkUsbUacModDesc,
	&sTlkUsbUacModCtrl,
};


int tlkusb_uac_init(void)
{
	#if (TLKUSB_UAC_SPK_ENABLE)
	tlkusb_uacspk_init();
	#endif
	#if (TLKUSB_UAC_MIC_ENABLE)
	tlkusb_uacmic_init();
	#endif
	
	return TLK_ENONE;
}


/******************************************************************************
 * Function: tlkusb_uacirq_handler
 * Descript: This function use to loop through usb device to transfer audio data.
 * Params: None
 * Return: None.
 * Others: None.
*******************************************************************************/
_attribute_ram_code_sec_noinline_
void tlkusb_uacirq_handler(void)
{
	uint08 irq = reg_usb_ep_irq_status;
	#if (TLKUSB_UAC_MIC_ENABLE)
	if(irq & FLD_USB_EDP7_IRQ){
		tlkusb_uacmic_fillData();
	}
	#endif
	#if (TLKUSB_UAC_SPK_ENABLE)
	if(irq & FLD_USB_EDP6_IRQ){
		tlkusb_uacspk_recvData();
	}
	#endif
	reg_usb_ep_irq_status = 0xFF;
}

/******************************************************************************
 * Function: tlkusb_uac_setFeatureMute
 * Descript: This function use to set feature mute.
 * Params:
 * 		@id[IN]--id.
 * 		@mute[IN]--mute.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkusb_uac_setFeatureMute(int id, int mute)
{
	int b = (id >> 8) & 15;
	if((id & 0xff) < 2) b |= (1 << 4);
	if(mute) sTlkUsbUacMute |= (1 << b);
	else sTlkUsbUacMute &= ~ (1 << b);
}
/******************************************************************************
 * Function: tlkusb_uac_getFeatureMute
 * Descript: This function use to get feature mute.
 * Params:
 * 		@id[IN]--id.
 * Return: Number of feature mute.
 * Others: None.
*******************************************************************************/
uint tlkusb_uac_getFeatureMute(int id)
{
	int b = (id >> 8) & 15;
	if((id & 0xff) < 2) b |= (1 << 4);
	return (sTlkUsbUacMute >> b) & 1;
}



#endif //#if (TLK_USB_UAC_ENABLE)

