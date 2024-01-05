/********************************************************************************************************
 * @file	tlkusb_udb.c
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
#include "drivers.h" 
#include "tlkapi/tlkapi_stdio.h"
#include "tlklib/usb/tlkusb_stdio.h"
#if (TLK_USB_UDB_ENABLE)
#include "tlklib/usb/udb/tlkusb_udbDefine.h"
#include "tlklib/usb/udb/tlkusb_udb.h"
#include <stdio.h>
#include <stdarg.h>
#include "tlklib/dbg/tlkdbg.h"
#include "tlklib/dbg/tlkdbg_config.h"
#include "tlklib/dbg/tlkdbg_usblog.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TLKUSB_UDB_BUFF_SIZE       320

#define TLKUSB_UDB_EDP_OUT_FLAG    (1 << (TLKUSB_UDB_EDP_DBG_OUT & 7))

static bool tlkusb_udb_recvDatDeal(void);
static void tlkusb_udb_recvCmdProc(uint08 *pData, uint16 dataLen, bool *pIsDown);


static TlkUsbUsrDebugCB sTlkUsbUdbDebugCB;
static TlkUsbUsrDebugCB sTlkUsbEqDebugCB;

static uint16 sTlkUsbUdbCmdLength = 0;
static uint08 sTlkUsbUdbCmdBuffer[TLKUSB_UDB_BUFF_SIZE];

extern const tlkusb_modDesc_t sTlkUsbUdbModDesc;
extern const tlkusb_modCtrl_t sTlkUsbUdbModCtrl;
const tlkusb_module_t gTlkUsbUdbModule = {
	TLKUSB_MODTYPE_UDB,
	&sTlkUsbUdbModDesc,
	&sTlkUsbUdbModCtrl,
};


int tlkusb_udb_init(void)
{
	sTlkUsbUdbCmdLength = 0;
	sTlkUsbUdbDebugCB = nullptr;
	
	return TLK_ENONE;
}


void tlkusb_udb_regEqCB(TlkUsbUsrDebugCB cb)
{
	sTlkUsbEqDebugCB = cb;
}
void tlkusb_udb_regDbgCB(TlkUsbUsrDebugCB cb)
{
	sTlkUsbUdbDebugCB = cb;
}

/******************************************************************************
 * Function: tlkusb_udb_sendData
 * Descript: Send data.
 * Params:
 * 		@pData[IN]--Data to send.
 *		@dataLen[IN]--Data Length.
 * Return: TLK_ENONE is SUCCESS.
 * Others: None.
*******************************************************************************/

_attribute_ram_code_ 
int tlkusb_udb_sendData(uint08 *pData, uint08 dataLen)
{
	uint08 index;
	if(dataLen > 64) return -TLK_ELENGTH;
	if(usbhw_is_ep_busy(TLKUSB_UDB_EDP_DBG_IN)) return -TLK_EBUSY;
	
	usbhw_reset_ep_ptr(TLKUSB_UDB_EDP_DBG_IN);
	for(index=0; index<dataLen; index++){
		usbhw_write_ep_data(TLKUSB_UDB_EDP_DBG_IN, pData[index]);
	}
	usbhw_data_ep_ack(TLKUSB_UDB_EDP_DBG_IN);
	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkusb_udb_recvHandler
 * Descript: Receive data handler.
 * Params: None
 * Return: None.
 * Others: None.
*******************************************************************************/
_attribute_ram_code_ 
void tlkusb_udb_recvHandler(void)
{
	bool ready;
	bool isDown = false;
	do{
		ready = tlkusb_udb_recvDatDeal();
		if(ready){
			tlkusb_udb_recvCmdProc(sTlkUsbUdbCmdBuffer, sTlkUsbUdbCmdLength, &isDown);
			if(isDown) sTlkUsbUdbCmdLength = 0;
		}
		#if (TLKDBG_CFG_USB_LOG_ENABLE)
		tlkdbg_usblog_handler();
		#endif
	}while(isDown);
	if(ready){
		if(sTlkUsbUdbCmdBuffer[0] == 0x11){
			if(sTlkUsbUdbDebugCB != nullptr){
				sTlkUsbUdbDebugCB(sTlkUsbUdbCmdBuffer, sTlkUsbUdbCmdLength);
			}
		}else if(sTlkUsbUdbCmdBuffer[0] == 0x12){
			//EQ CB
			if(sTlkUsbEqDebugCB != nullptr){
				sTlkUsbEqDebugCB(sTlkUsbUdbCmdBuffer, sTlkUsbUdbCmdLength);
			}
		}
		sTlkUsbUdbCmdLength = 0;
	}
}


extern void tlkdbg_usblog_reset(void);

_attribute_ram_code_
static bool tlkusb_udb_recvDatDeal(void)
{
	uint08 index;
	uint08 length;
	uint08 buffer[64];

	if(reg_usb_ep_irq_status & TLKUSB_UDB_EDP_OUT_FLAG){
		//clear interrupt flag
		reg_usb_ep_irq_status = TLKUSB_UDB_EDP_OUT_FLAG;
		length = reg_usb_ep_ptr(TLKUSB_UDB_EDP_DBG_OUT);
		reg_usb_ep_ptr(TLKUSB_UDB_EDP_DBG_OUT) = 0;
		for(index=0; index<length; index++){
			buffer[index] = reg_usb_ep_dat(TLKUSB_UDB_EDP_DBG_OUT);
		}
		reg_usb_ep_ctrl(TLKUSB_UDB_EDP_DBG_OUT) = 1;
		if(length != 0 && length+sTlkUsbUdbCmdLength < TLKUSB_UDB_BUFF_SIZE){
			tmemcpy(sTlkUsbUdbCmdBuffer+sTlkUsbUdbCmdLength, buffer, length);
			sTlkUsbUdbCmdLength += length;
		}
		if(length != 0 && length < 64) return true;
	}
	return false;
}
_attribute_ram_code_
static void tlkusb_udb_recvCmdProc(uint08 *pData, uint16 dataLen, bool *pIsDown)
{
	uint08 rsp[64];
	uint08 cmd = pData[0];
	
	//////////////////////////  Memory Read ////////////////////////////////////
	if(cmd == 0x28 && dataLen >= 8){
		#if (TLKDBG_CFG_USB_LOG_ENABLE)
		tlkdbg_usblog_sendStatus(0x81, 8, pData, 12);
		#endif
		rsp[0] = 0x29;
		tmemcpy(rsp + 1, pData+1, 5);
		int type = pData[1];
		uint32 adr = pData[2] | (pData[3] << 8) | (pData[4] << 16) | (pData[5] << 24);
		int n = pData[6] | (pData[7] << 8);
		if(n > 256) n = 256;
		
		if(type == 0){
			tmemcpy(rsp + 6, (void *)(adr | 0), n);
		}else if (type == 1){
			for(int i=0; i<n; i++){
				rsp[i + 6] = analog_read_reg8 (adr + i);
			}
		}else if(type == 2 || type == 3){//flash
			flash_read_page(adr, n, rsp + 6);
		}
		#if (TLKDBG_CFG_USB_LOG_ENABLE)
		tlkdbg_usblog_sendStatus(0x82, 8, rsp, n + 6);
		#endif
	}
	//////////////////////////  Memory Write ////////////////////////////////////
	else if(cmd == 0x2a && dataLen > 6){
		uint08 type = pData[1];
		uint32 addr = pData[2] | (pData[3] << 8) | (pData[4] << 16) | (pData[5] << 24);
		uint32 temp = dataLen - 6;

		rsp[0] = 0x2b;
		tmemcpy(rsp+1, pData+1, 16);

		#if (TLKDBG_CFG_USB_LOG_ENABLE)
		tlkdbg_usblog_sendStatus(0x81, 8, pData, 12);
		#endif
		if(type == 0){				//RAM
			tmemcpy((void *)addr, pData+6, temp);
		}else if(type == 1){			//Analog Register
			for(int i=0; i<temp; i++){
				analog_write_reg8(addr + i, pData[i + 6]);
			}
		}else if(type == 2){			// flash
			if((*pIsDown) && (addr & 0xFFF) == 0){
				flash_erase_sector(addr);
			}
			flash_write_page(addr, temp, pData+6);
		}else if(type == 3){
			uint32 index;
			uint32 nbyte = pData[6];
			if(temp > 1) nbyte += pData[7] << 8;
			if(temp > 2) nbyte += pData[8] << 16;
			if(temp > 3) nbyte += pData[9] << 24;
			if(nbyte == 0) nbyte = 0x100000;
			for(index=0; index<nbyte; index+=4096){
				flash_erase_sector(addr+index);
			}
		}else if(type == 0xFE){ //FW_DOWNLOAD
			tlksys_disable_interrupt();
			#if (TLKDBG_CFG_USB_LOG_ENABLE)
			tlkdbg_usblog_reset();
			#endif
			*pIsDown = true;
		}
		#if (TLKDBG_CFG_USB_LOG_ENABLE)
		tlkdbg_usblog_sendStatus(0x82, 8, rsp, 14);
		#endif
	}
}




#endif //#if (TLK_USB_UDB_ENABLE)

