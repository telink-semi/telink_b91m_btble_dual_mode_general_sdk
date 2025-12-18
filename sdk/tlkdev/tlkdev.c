/********************************************************************************************************
 * @file	tlkdev.c
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
#if (TLK_CFG_DEV_ENABLE)
#include "tlkdev.h"
#include "tlkdev_list.h"
#if (TLK_DEV_SERIAL_ENABLE)
#include "tlkdev/sys/tlkdev_serial.h"
#endif
#if (TLK_CFG_SYS_ENABLE)
#include "tlksys/tlksys_stdio.h"
#endif
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/sys/tlkdev_store.h"
#endif
#if (TLK_DEV_SRAM_ENABLE)
#include "tlkdev/sys/tlkdev_sram.h"
#endif
#if (TLK_DEV_LCD_ENABLE)
#include "tlkdev/sys/tlkdev_lcd.h"
#endif
#if (TLK_DEV_GSENSOR_ENABLE)
#include "tlkdev/sys/tlkdev_gsensor.h"
#endif 
#if (TLK_DEV_TOUCH_ENABLE)
#include "tlkdev/sys/tlkdev_touch.h"
#endif
#if (TLK_DEV_LED_ENABLE)
#include "tlkdev/sys/tlkdev_led.h"
#endif
#if (TLK_DEV_KEY_ENABLE)
#include "tlkdev/sys/tlkdev_key.h"
#endif
#if (TLK_DEV_BATTERY_ENABLE)
#include "tlkdev/sys/tlkdev_battery.h"
#endif
#if (TLKDEV_KEYBOARD_ENABLE)
#include "tlkdev/sys/tlkdev_keyboard.h"
#endif
#if (TLK_DEV_CODEC_ENABLE)
#include "tlkdev/sys/tlkdev_codec.h"
#endif
#if (TLK_DEV_USB_ENABLE)
#include "tlkdev/sys/tlkdev_usb.h"
#endif


#if (TLK_CFG_PM_ENABLE)
static bool tlkdev_check_isBusy(void);
static void tlkdev_enter_sleep(uint mcuMode);
static void tlkdev_leave_sleep(uint wakeSrc);
#endif


/******************************************************************************
 * Function: tlkdev_init
 * Descript: Initialize all devices used in the system.
 * Params: None.
 * Return: TLK_ENONE is success, other value is failure.
 * Notice:
 *     1.The device that performs this operation first backs up its own device 
 *       in the system device copy.
 *     2.Only devices in the system device copy can be mounted by calling 
 *       "tlkdev_mount()".
*******************************************************************************/
int tlkdev_init(void)
{
	tlkdev_list_init();
	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_battery_init();
	#endif
	#if (TLK_DEV_KEY_ENABLE)
	tlkdev_key_init();
	#endif
	#if (TLK_DEV_LED_ENABLE)
	tlkdev_led_init();
	#endif
	#if (TLK_DEV_USB_ENABLE)
	tlkdev_usb_init();
	#endif
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_init();
	#endif
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_store_init();
	#endif
	#if (TLK_DEV_SRAM_ENABLE)
	tlkdev_sram_init();
	#endif
	#if (TLK_DEV_LCD_ENABLE)
	tlkdev_lcd_init();
	#endif
	#if (TLK_DEV_GSENSOR_ENABLE)
	tlkdev_gsensor_init();
	#endif
	#if (TLK_DEV_TOUCH_ENABLE)
	tlkdev_touch_init();
	#endif
	#if (TLKDEV_KEYBOARD_ENABLE)
	tlkdev_keyboard_init();
	#endif
	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_init();
	#endif
	#if (TLK_CFG_PM_ENABLE)
	tlksys_pm_appendEnterSleepCB(tlkdev_enter_sleep);
	tlksys_pm_appendLeaveSleepCB(tlkdev_leave_sleep);
	tlksys_pm_appendBusyCheckCB(tlkdev_check_isBusy, "tlkdev");
	#endif
	
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Deinitialize all devices used in the system.
 * Params: None.
 * Return: None.
 *****************************************************************************/
void tlkdev_deinit(void)
{
	
}

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Mount the device to the device list.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_mount(uint16 did) //DID--Refer TLKDEV_DID_ENUM
{
	const tlkdev_node_t *pNode;
	
	pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode != nullptr) return -TLK_EREPEAT;
	
	pNode = tlkdev_getEctypeNode(did, nullptr);
	if(pNode == nullptr) return -TLK_ENOOBJECT;
	
	if(pNode->Init != nullptr && pNode->Init(did) != TLK_ENONE){
		return -TLK_EINIT;
	}
	
	return tlkdev_appendActiveNode(pNode);
}

/******************************************************************************
 * Function: tlkdev_unmount
 * Descript: Unmount the device from the device list.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_unmount(uint16 did)
{
	const tlkdev_node_t *pNode;
	
	pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr) return -TLK_ENOOBJECT;
	
	if(pNode->Close != nullptr){
		pNode->Close(did);
	}
	
	return tlkdev_removeActiveNode(did);
}

/******************************************************************************
 * Function: tlkdev_open
 * Descript: Open the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_open(uint16 did, uint32 param)
{
	const tlkdev_node_t *pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr || pNode->Open == nullptr) return -TLK_ENOSUPPORT;
	return pNode->Open(did, param);
}

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Close the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: TLK_ENONE is success, other value is failure.
 *****************************************************************************/
int tlkdev_close(uint16 did)
{
	const tlkdev_node_t *pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr || pNode->Close == nullptr) return -TLK_ENOSUPPORT;
	return pNode->Close(did);
}

/******************************************************************************
 * Function: tlkdev_mount
 * Descript: Get the state of the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 * Return: The state of the mounted device. Refer TLKDEV_STATE_ENUM.
 *****************************************************************************/
int tlkdev_state(uint16 did)
{
	const tlkdev_node_t *pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr){
		if(tlkdev_getEctypeNode(did, nullptr) == nullptr) return TLKDEV_STATE_UNKNOWN;
		else return TLKDEV_STATE_UNMOUNT;
	}
	if(pNode->State == nullptr) return TLKDEV_STATE_UNKNOWN;
	return pNode->State(did);
}

/******************************************************************************
 * Function: tlkdev_read
 * Descript: Read the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 *     @param[IN]--Parameters used to control read operations. 
 *     @pBuff[OUT]--The storage area where data is read.
 *     @buffLen[IN]--The length of the buffer.
 * Return: The length of the read, and a negative value means failure.
 *****************************************************************************/
int tlkdev_read(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	const tlkdev_node_t *pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr || pNode->Read == nullptr) return -TLK_ENOSUPPORT;
	return pNode->Read(did, param, pBuff, buffLen);
}

/******************************************************************************
 * Function: tlkdev_write
 * Descript: Write the mounted device.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 *     @param[IN]--Parameters used to control write operations.
 *     @pData[IN]--Data to be written. 
 *     @dataLen[IN]--The length of the data.
 * Return: Write length. Negative values mean failure, others mean success.
 *****************************************************************************/
int tlkdev_write(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	const tlkdev_node_t *pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr || pNode->Write == nullptr) return -TLK_ENOSUPPORT;
	return pNode->Write(did, param, pData, dataLen);
}

/******************************************************************************
 * Function: tlkdev_ioctl
 * Descript: Perform input and output operations on mounted devices.
 * Params: 
 *     @did[IN]--Device ID. Refer TLKDEV_DID_ENUM
 *     @opcode[IN]--IO operation code. Refer TLKDEV_OPCODE_ENUM.
 *     @param0[IN/OUT]--The first input or output parameter.
 *     @param1[IN/OUT]--The second input or output parameter.
 * Return: Negative values mean failure, others mean success.
 *****************************************************************************/
int tlkdev_ioctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	const tlkdev_node_t *pNode = tlkdev_getActiveNode(did, nullptr);
	if(pNode == nullptr || pNode->Ioctl == nullptr) return -TLK_ENOSUPPORT;
	return pNode->Ioctl(did, opcode, param0, param1);
}


#if (TLK_CFG_PM_ENABLE)
static bool tlkdev_check_isBusy(void)
{
	bool isBusy = false;
	#if (TLK_DEV_SERIAL_ENABLE)
	if(!isBusy) isBusy = tlkdev_serial_isBusy();
	#endif
	#if (TLK_DEV_KEY_ENABLE)
	if(!isBusy) isBusy = tlkdev_key_isBusy();
	#endif
	#if (TLK_DEV_LED_ENABLE)
	if(!isBusy) isBusy = tlkdev_led_isBusy();
	#endif
	#if (TLK_DEV_USB_ENABLE)
	if(!isBusy) isBusy = tlkdev_usb_isBusy();
	#endif
	return isBusy;
}
static void tlkdev_enter_sleep(uint mcuMode)
{
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_store_enterSleep(mcuMode);
	#endif
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_enterSleep(mcuMode);
	#endif
	#if (TLK_DEV_KEY_ENABLE)
	tlkdev_key_enterSleep(mcuMode);
	#endif
	#if (TLK_DEV_LED_ENABLE)
	tlkdev_led_enterSleep(mcuMode);
	#endif
	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_battery_enterSleep(mcuMode);
	#endif
	#if (TLKDEV_KEYBOARD_ENABLE)
	tlkdev_keyboard_enterSleep(mcuMode);
	#endif
	#if (TLK_DEV_USB_ENABLE)
	tlkdev_usb_enterSleep(mcuMode);
	#endif
}
static void tlkdev_leave_sleep(uint wakeSrc)
{
	#if (TLK_DEV_STORE_ENABLE)
	tlkdev_store_leaveSleep(wakeSrc);
	#endif
	#if (TLK_DEV_SERIAL_ENABLE)
	tlkdev_serial_leaveSleep(wakeSrc);
	#endif
	#if (TLK_DEV_KEY_ENABLE)
	tlkdev_key_leaveSleep(wakeSrc);
	#endif
	#if (TLK_DEV_LED_ENABLE)
	tlkdev_led_leaveSleep(wakeSrc);
	#endif
	#if (TLK_DEV_BATTERY_ENABLE)
	tlkdev_battery_leaveSleep(wakeSrc);
	#endif
	#if (TLKDEV_KEYBOARD_ENABLE)
	tlkdev_keyboard_leaveSleep(wakeSrc);
	#endif
	#if (TLK_DEV_USB_ENABLE)
	tlkdev_usb_leaveSleep(wakeSrc);
	#endif
}
#endif


#endif //#if (TLK_CFG_DEV_ENABLE)

