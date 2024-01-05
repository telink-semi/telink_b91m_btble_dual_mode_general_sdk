/********************************************************************************************************
 * @file	tlkmmi_device.c
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
#if (TLKMMI_DEVICE_ENABLE)
#include "drivers.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkdev/sys/tlkdev_usb.h"
#include "tlkdev/sys/tlkdev_battery.h"
#include "tlkdev/tlkdev.h"
#include "tlkmmi_dev.h"
#include "tlkmmi_devAdapt.h"
#include "tlkmmi_devMsgInner.h"
#include "tlkmmi_devMsgOuter.h"



TLKSYS_MMI_TASK_DEFINE(device, Device);


static int tlkmmi_device_init(uint08 procID, uint16 taskID)
{
	uint08 buffLen;
	uint08 buffer[8];

	buffLen = 0;
	buffer[buffLen++] = TLKPRT_COMM_MTYPE_DEVICE;
	buffer[buffLen++] = taskID & 0xFF;
	buffer[buffLen++] = (taskID & 0xFF00) >> 8;
	tlksys_sendInnerMsg(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_REG_COMM_TASK, buffer, buffLen);
	
	tlkmmi_device_adaptInit(procID);

	#if (TLK_DEV_KEY_ENABLE)
	tlkdev_mount(TLKDEV_DID_KEY1);
	tlkdev_mount(TLKDEV_DID_KEY2);
	#endif
	#if (TLK_DEV_LED_ENABLE)
	tlkdev_mount(TLKDEV_DID_LED1);
	tlkdev_mount(TLKDEV_DID_LED2);
	tlkdev_mount(TLKDEV_DID_LED3);
	tlkdev_mount(TLKDEV_DID_LED4);
	#endif
	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_mount(TLKDEV_DID_BATTERY);
	#endif
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_mount(TLKDEV_DID_SDCARD0);
	#endif
	#if (TLK_DEV_USB_ENABLE)
	tlkdev_mount(TLKDEV_DID_USB);
	#endif
		
	
	return TLK_ENONE;
}


static int tlkmmi_device_start(void)
{
	#if (TLK_DEV_KEY_ENABLE)
	tlkdev_open(TLKDEV_DID_KEY1, 0);
	tlkdev_open(TLKDEV_DID_KEY2, 0);
	#endif
	
	#if (TLK_DEV_LED_ENABLE)
	tlkdev_open(TLKDEV_DID_LED1, 0);
	tlkdev_open(TLKDEV_DID_LED2, 0);
	tlkdev_open(TLKDEV_DID_LED3, 0);
	tlkdev_open(TLKDEV_DID_LED4, 0);
	{
		uint08 ledParam[9] = {0x02, 0x01, 0x00, 0x0A, 0x00, 0xC8, 0x00, 0xF4, 0x01};
		tlkdev_write(TLKDEV_DID_LED1, 0, (uint08*)&ledParam, 0x09);
	}
	#endif

	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_open(TLKDEV_DID_BATTERY, 0);
	#endif

	#if (TLK_DEV_USB_ENABLE)
	tlkdev_open(TLKDEV_DID_USB, 0);
	#endif
	
	return TLK_ENONE;
}
static int tlkmmi_device_pause(void)
{
	return TLK_ENONE;
}
static int tlkmmi_device_close(void)
{
	#if (TLK_DEV_KEY_ENABLE)
	tlkdev_close(TLKDEV_DID_KEY1);
	tlkdev_close(TLKDEV_DID_KEY2);
	#endif
	#if (TLK_DEV_LED_ENABLE)
	tlkdev_close(TLKDEV_DID_LED1);
	tlkdev_close(TLKDEV_DID_LED2);
	tlkdev_close(TLKDEV_DID_LED3);
	tlkdev_close(TLKDEV_DID_LED4);
	#endif
	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_close(TLKDEV_DID_BATTERY);
	#endif
	return TLK_ENONE;
}
static int tlkmmi_device_input(uint08 mtype, uint16 msgID, uint08 *pData, uint16 dataLen)
{	
	if(mtype == TLKPRT_COMM_MTYPE_NONE){
		return tlkmmi_device_innerMsgHandler(msgID, pData, dataLen);
	}else if(mtype == TLKPRT_COMM_MTYPE_DEVICE){
		return tlkmmi_device_outerMsgHandler(msgID, pData, dataLen);
	}
	return -TLK_ENOSUPPORT;
}
static void tlkmmi_device_handler(void)
{
	#if (TLK_DEV_USB_ENABLE)
	tlkdev_usb_handler();
	#endif
}





#endif //#if (TLKMMI_DEVICE_ENABLE)
