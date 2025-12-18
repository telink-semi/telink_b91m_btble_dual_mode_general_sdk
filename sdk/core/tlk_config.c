/********************************************************************************************************
 * @file	tlk_config.c
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
#include "tlk_config.h"
#include "tlk_define.h"
#include "tlkapi/tlkapi_stdio.h"
#include "drivers.h"


#define TLK_CONFIG_INFO_SIGN        0x3C
#define TLK_CONFIG_INFO_VERS        0x02
#define TLK_CONFIG_INFO_ADDR        TLK_CFG_FLASH_SYS_CONFIG_ADDR
#define TLK_CONFIG_INFO_SIZE        16


#define TLK_CONFIG_WORK_MODE_OFFSET		0
#define TLK_CONFIG_WORK_MODE_LENGTH		1
#define TLK_CONFIG_USB_MODE_OFFSET		TLK_CONFIG_WORK_MODE_OFFSET + TLK_CONFIG_WORK_MODE_LENGTH
#define TLK_CONFIG_USB_MODE_LENGTH		1
#define TLK_CONFIG_BAUDRATE_OFFSET		TLK_CONFIG_USB_MODE_OFFSET + TLK_CONFIG_USB_MODE_LENGTH
#define TLK_CONFIG_BAUDRATE_LENGTH		4



static uint08 sTlkWorkMode = TLK_WORK_MODE_NORMAL;
static uint08 sTlkUsbMode = 0;//TLKUSB_MODTYPE_ENUM
static uint32 sTlkSerialBaudrate = 0xFFFFFFFF;
static tlkapi_save_ctrl_t sTlkConfigSave;

unsigned int sTlkFlashCap = 0x200000;
static unsigned int sTlkConfigInfoAddr = 0;


unsigned int tlkcfg_getFLashCap(void)
{
	unsigned int  flash_mid = 0;
	unsigned char flash_cap = 0;
	flash_mid = flash_read_mid();
	flash_cap = ((flash_mid & 0x00ff0000)>>16);

	if(flash_cap == FLASH_SIZE_1M){
		return 0x100000;
	}else if(flash_cap == FLASH_SIZE_2M){
		return 0x200000;
	}else if(flash_cap == FLASH_SIZE_4M){
		return 0x400000;
	}else if(flash_cap == FLASH_SIZE_8M){
		return 0x800000;
	}else if(flash_cap == FLASH_SIZE_16M){
		return 0x1000000;
	}else{
		return 0x200000;
	}
}

unsigned int tlkcfg_getFlashAddr(unsigned int offset)
{
	if(sTlkFlashCap == 0){
		sTlkFlashCap = tlkcfg_getFLashCap();
	}
	if(sTlkFlashCap == 0){
		return 0;
	}
	return offset + sTlkFlashCap - 0x100000;
}

int tlkcfg_load(void)
{
	uint08 buffer[TLK_CONFIG_INFO_SIZE] = {0};
	
	sTlkFlashCap = tlkcfg_getFLashCap();

	sTlkConfigInfoAddr = tlkcfg_getFlashAddr(TLK_CONFIG_INFO_ADDR);
	if(sTlkConfigInfoAddr == 0){
		return -TLK_EFAIL;
	}

	sTlkWorkMode = TLK_WORK_MODE_NORMAL;
	tlkapi_save2_init(&sTlkConfigSave, TLK_CONFIG_INFO_SIGN, TLK_CONFIG_INFO_VERS,
		TLK_CONFIG_INFO_SIZE, sTlkConfigInfoAddr);
	if(tlkapi_save2_load(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE) <= 0){
		buffer[TLK_CONFIG_WORK_MODE_OFFSET] = sTlkWorkMode;
		buffer[TLK_CONFIG_USB_MODE_OFFSET] = sTlkUsbMode;
		buffer[TLK_CONFIG_BAUDRATE_OFFSET] = (sTlkSerialBaudrate & 0xFF000000) >> 24;
		buffer[TLK_CONFIG_BAUDRATE_OFFSET+1] = (sTlkSerialBaudrate & 0xFF0000) >> 16;
		buffer[TLK_CONFIG_BAUDRATE_OFFSET+2] = (sTlkSerialBaudrate & 0xFF00) >> 8;
		buffer[TLK_CONFIG_BAUDRATE_OFFSET+3] = (sTlkSerialBaudrate & 0xFF);
		tlkapi_save2_save(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
		return TLK_ENONE;
	}
	#if (TLK_CFG_TEST_ENABLE)
	if(buffer[0] >= TLK_WORK_MODE_NORMAL && buffer[0] <= TLK_WORK_MODE_TEST_USR){
		sTlkWorkMode = buffer[0];
	}
	#endif
	
	#if (TLK_CFG_USB_ENABLE)
	if(buffer[1] >= 0 && buffer[1] <= 5){
		sTlkUsbMode = buffer[1];
	}
	#endif
	
	#if (TLK_DEV_SERIAL_ENABLE)
	sTlkSerialBaudrate = 0;
	sTlkSerialBaudrate |= ((buffer[2] & 0xFF) << 24);
	sTlkSerialBaudrate |= ((buffer[3] & 0xFF) << 16);
	sTlkSerialBaudrate |= ((buffer[4] & 0xFF) << 8);
	sTlkSerialBaudrate |= ((buffer[5] & 0xFF));
	#endif
	return TLK_ENONE;
}


uint tlkcfg_getWorkMode(void)
{
	return sTlkWorkMode;
}
void tlkcfg_setWorkMode(TLK_WORK_MODE_ENUM wmode)
{
	#if (TLK_CFG_TEST_ENABLE)
	uint08 buffer[TLK_CONFIG_INFO_SIZE] = {0};

	if(sTlkWorkMode == wmode) return;
	if(!(wmode >= TLK_WORK_MODE_NORMAL && wmode <= TLK_WORK_MODE_TEST_USR)){
		return;
	}
	sTlkWorkMode = wmode;
	tlkapi_save2_load(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	buffer[0] = sTlkWorkMode;
	tlkapi_save2_save(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	#endif //#if (TLK_CFG_TEST_ENABLE)
}
uint08 tlkcfg_getUsbMode(void)
{
	return sTlkUsbMode;
}
void tlkcfg_setUsbMode(uint08 umode)
{
	#if (TLK_CFG_USB_ENABLE)
	uint08 buffer[TLK_CONFIG_INFO_SIZE] = {0};
	
	if(sTlkUsbMode == umode) return;
	if(!(umode >= 0 && umode <= 5)){
		return;
	} 
	sTlkUsbMode = umode;
	tlkapi_save2_load(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	buffer[1] = sTlkUsbMode;
	tlkapi_save2_save(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	#endif//#if (TLK_CFG_USB_ENABLE)
}
uint32 tlkcfg_getSerialBaudrate(void)
{
	return sTlkSerialBaudrate;
}
void tlkcfg_setSerialBaudrate(uint32 baudrate)
{
	#if (TLK_DEV_SERIAL_ENABLE)
	uint08 buffer[TLK_CONFIG_INFO_SIZE] = {0};
	if(baudrate == sTlkSerialBaudrate || baudrate < 9600) return;
	
	sTlkSerialBaudrate = baudrate;
	tlkapi_save2_load(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	buffer[2] = (sTlkSerialBaudrate & 0xFF000000) >> 24;
	buffer[3] = (sTlkSerialBaudrate & 0xFF0000) >> 16;
	buffer[4] = (sTlkSerialBaudrate & 0xFF00) >> 8;
	buffer[5] = (sTlkSerialBaudrate & 0xFF);
	tlkapi_save2_save(&sTlkConfigSave, buffer, TLK_CONFIG_INFO_SIZE);
	#endif//#if (TLK_DEV_SERIAL_ENABLE)
}



