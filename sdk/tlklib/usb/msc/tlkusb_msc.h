/********************************************************************************************************
 * @file	tlkusb_msc.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKUSB_MSC_H
#define TLKUSB_MSC_H

#if (TLK_USB_MSC_ENABLE)



typedef struct{
	uint08 isReady;
	uint08 hotPlug; 	//1-Enable, 0-Disable
	uint16 blkSize;		//Block size
	uint32 blkCount;	//Block count
	char *pVendorStr; 	//VENDOR_STRING Length<=8
	char *pProductStr;  //PRODUCT_STRING Length<=16
	char *pVersionStr;  //VERSION_STRING Length<=4
	int(*Init)(void);
	int(*Read)(uint08 *pBuff, uint32 blkOffs, uint16 blkNumb);
	int(*Write)(uint08 *pData, uint32 blkOffs, uint16 blkNumb);
}tlkusb_msc_disk_t;





int tlkusb_msc_init(void);

int tlkusb_msc_appendDisk(tlkusb_msc_disk_t *pUnit);
int tlkusb_msc_removeDisk(tlkusb_msc_disk_t *pUnit);


uint08 tlkusb_msc_getDiskCount(void);
tlkusb_msc_disk_t *tlkusb_msc_getDisk(uint08 volNum);




#endif //#if (TLK_USB_MSC_ENABLE)

#endif //TLKUSB_MSC_H

