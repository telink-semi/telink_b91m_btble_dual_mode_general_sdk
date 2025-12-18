/********************************************************************************************************
 * @file	tlkmdi_usb.c
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
#if (TLK_MDI_MSC_ENABLE)
#include "tlkmdi_msc.h"
#if (TLK_USB_MSC_ENABLE)
#include "tlklib/usb/msc/tlkusb_msc.h"
#endif
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/tlkdev.h"
#endif



#if (TLK_USB_MSC_ENABLE && TLK_DEV_STORE_ENABLE)
static const tlkusb_msc_disk_t sTlkMdiMscDisk0;
#endif



int tlkmdi_msc_init(void)
{
	/* increase 1mA when test low power, so disable USB when PM used */
	#if (TLK_USB_MSC_ENABLE && TLK_DEV_STORE_ENABLE)
	tlkusb_msc_appendDisk((tlkusb_msc_disk_t*)&sTlkMdiMscDisk0);
	#endif
	
	return TLK_ENONE;
}




#if (TLK_USB_MSC_ENABLE && TLK_DEV_STORE_ENABLE)
static int tlkmdi_msc_diskInit(void)
{
	return TLK_ENONE;
}
static int tlkmdi_msc_diskRead(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb)
{
	if(tlkdev_state(TLKDEV_DID_SDCARD0) == TLKDEV_STATE_CLOSED){
		tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	}
	return tlkdev_read(TLKDEV_DID_SDCARD0, blkOffs<<9, pBuff, blkNumb<<9); //param0 = addr = (blkOffs<<9) = blkOffs*sTlkMdiMscDisk0.blkSize
}
static int tlkmdi_msc_diskWrite(uint08 *pData, uint32 blkOffs, uint16 blkNumb)
{
	if(tlkdev_state(TLKDEV_DID_SDCARD0) == TLKDEV_STATE_CLOSED){
		tlkdev_open(TLKDEV_DID_SDCARD0, 0);
	}
	return tlkdev_write(TLKDEV_DID_SDCARD0, blkOffs<<9, pData, blkNumb<<9); //param0 = addr = (blkOffs<<9) = blkOffs*sTlkMdiMscDisk0.blkSize
}
static const tlkusb_msc_disk_t sTlkMdiMscDisk0 = {
	true, //isReady;
	true, //hotPlug; //1-Enable, 0-Disable
	512,//blkSize;
	0x0003E000,//blkCount; 128M-2M=126M
	"TLK-DISK", //pVendorStr; //<=8
	"Telink Disk Demo", //pProductStr; //<=16
	"1.02", //pVersionStr; //<=4
	tlkmdi_msc_diskInit,//int(*Init)(void);
	tlkmdi_msc_diskRead,//int(*Read)(uint32 blkOffs, uint16 blkNumb, uint08 *pBuff);
	tlkmdi_msc_diskWrite,//int(*Write)(uint32 blkOffs, uint16 blkNumb, uint08 *pData);
};
#endif


#endif //#if (TLK_MDI_MSC_ENABLE)

