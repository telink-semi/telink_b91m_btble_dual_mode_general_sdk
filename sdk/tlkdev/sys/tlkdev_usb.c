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
#if (TLK_DEV_USB_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev_usb.h"
#include "drivers.h"
#if (TLK_CFG_USB_ENABLE)
#include "tlklib/usb/tlkusb.h"
#endif


static int tlkdev_usb_nodeInit(uint16 did);
static int tlkdev_usb_nodeOpen(uint16 did, uint32 param);
static int tlkdev_usb_nodeClose(uint16 did);
static int tlkdev_usb_nodeState(uint16 did);
static int tlkdev_usb_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_usb_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_usb_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);


static const tlkdev_node_t scTlkDevUsb = {
	TLKDEV_DID_USB,  //.devID;
	"USB",           //.pName;
	tlkdev_usb_nodeInit,  //.Init
	tlkdev_usb_nodeOpen,  //.Open
	tlkdev_usb_nodeClose, //.Close
	tlkdev_usb_nodeState, //.State
	tlkdev_usb_nodeRead,  //.Read
	tlkdev_usb_nodeWrite, //.Write
	tlkdev_usb_nodeIoctl, //.Ioctl
};


int tlkdev_usb_init(void)
{
	tlkdev_appendEctypeNode(&scTlkDevUsb);
	
	return TLK_ENONE;
}
void tlkdev_usb_deinit(void)
{
	tlkdev_removeEctypeNode(TLKDEV_DID_USB);
}


bool tlkdev_usb_isBusy(void)
{
	return tlkusb_isBusy();
}
void tlkdev_usb_enterSleep(uint mcuMode)
{
	tlkusb_enterSleep(mcuMode);
}
void tlkdev_usb_leaveSleep(uint wakeSrc)
{
	tlkusb_leaveSleep(wakeSrc);
}


void tlkdev_usb_handler(void)
{
	tlkusb_handler();
}


static int tlkdev_usb_nodeInit(uint16 did)
{
	tlkusb_init(0x120); /* increase 1mA when test low power, so disable USB when PM used */
	
	return TLK_ENONE;
}
static int tlkdev_usb_nodeOpen(uint16 did, uint32 param)
{
	return tlkusb_open(param);
}
static int tlkdev_usb_nodeClose(uint16 did)
{
	return tlkusb_close();
}
static int tlkdev_usb_nodeState(uint16 did)
{
	
	return TLKDEV_STATE_CLOSED;
}
static int tlkdev_usb_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	
	return -TLK_ENOSUPPORT;
}
static int tlkdev_usb_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_usb_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdev_usb_enterSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdev_usb_leaveSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_USB_CHANGE_MODE){
		tlkusb_close();
		return tlkusb_open(param0);;
	}
	return -TLK_ENOSUPPORT;
}



#endif //#if (TLK_DEV_USB_ENABLE)

