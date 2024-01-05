/********************************************************************************************************
 * @file	tlkusb_msc.c
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



extern const tlkusb_modCtrl_t sTlkUsbMscModCtrl;
extern const tlkusb_modDesc_t sTlkUsbMscModDesc;
const tlkusb_module_t gTlkUsbMscModule = {
	TLKUSB_MODTYPE_MSC,
	&sTlkUsbMscModDesc,
	&sTlkUsbMscModCtrl,
};
static uint08 sTlkUsbMscUnitCount = 0;
static tlkusb_msc_disk_t *spTlkUsbMscUnit[TLKUSB_MSC_UNIT_COUNT];





int tlkusb_msc_init(void)
{
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkusb_msc_appendDisk
 * Descript: This function use to append disk for MSC device.
 * Params:
 * 		@pUnit[IN]--msc disk structure.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/
int tlkusb_msc_appendDisk(tlkusb_msc_disk_t *pUnit)
{
	if(pUnit == nullptr || pUnit->blkSize == 0 || (pUnit->blkSize & 0x3F) != 0
		|| pUnit->blkCount < 100 || pUnit->Read == nullptr || pUnit->Write == nullptr){
		return -TLK_EPARAM;
	}
	spTlkUsbMscUnit[sTlkUsbMscUnitCount++] = pUnit;
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkusb_msc_getDiskCount
 * Descript: This function use to get disk count.
 * Params: None.
 * Return: Number of disk count.
 * Others: None.
*******************************************************************************/
uint08 tlkusb_msc_getDiskCount(void)
{
	return sTlkUsbMscUnitCount;
}
/******************************************************************************
 * Function: tlkusb_msc_getDisk
 * Descript: This function use to get disk.
 * Params:
 * 		@volNum[IN]--index.
 * Return: Msc disk .
 * Others: None.
*******************************************************************************/
tlkusb_msc_disk_t *tlkusb_msc_getDisk(uint08 volNum)
{
	if(volNum >= sTlkUsbMscUnitCount) return nullptr;
	return spTlkUsbMscUnit[volNum];
}









#endif //#if (TLK_USB_MSC_ENABLE)


