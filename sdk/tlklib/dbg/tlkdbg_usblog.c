/********************************************************************************************************
 * @file	tlkdbg_usblog.c
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
#if (TLK_DBG_LOG_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_USB_LOG_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_usblog.h"


extern int tlkusb_udb_sendData(uint08 *pData, uint08 dataLen);



static uint16 sTlkDbgUsbLogSerial;
static uint16 sTlkDbgUsbLogOffset;
static uint08 sTlkDbgUsbLogBuffer[(TLKDBG_USBLOG_ITEM_SIZE+2)*TLKDBG_USBLOG_ITEM_NUMB];
static tlkapi_qfifo_t sTlkDbgUsbLogFifo;
static uint08 sTlkDbgUsbLogCache[TLKDBG_USBLOG_ITEM_SIZE+2];


void tlkdbg_usblog_init(void)
{
	tlkapi_qfifo_init(&sTlkDbgUsbLogFifo, TLKDBG_USBLOG_ITEM_NUMB, (TLKDBG_USBLOG_ITEM_SIZE+2),
			sTlkDbgUsbLogBuffer, (TLKDBG_USBLOG_ITEM_SIZE+2)*TLKDBG_USBLOG_ITEM_NUMB);
}

_attribute_ram_code_sec_noinline_
void tlkdbg_usblog_reset(void)
{
	tlkapi_qfifo_clear(&sTlkDbgUsbLogFifo);
}

bool tlkdbg_usblog_isBusy(void)
{
	if(tlkapi_qfifo_isEmpty(&sTlkDbgUsbLogFifo)) return false;
	else return true;
}

_attribute_ram_code_sec_noinline_ 
void tlkdbg_usblog_handler(void)
{
	int ret;
	uint08 *pData;
	uint16 offset;
	uint08 sendLen;
	uint16 dataLen;
	
	pData = tlkapi_qfifo_getData(&sTlkDbgUsbLogFifo);
	if(pData == nullptr) return;
	
	dataLen = ((uint16)pData[1] << 8) | pData[0];
	if(dataLen == 0 || dataLen+2 > TLKDBG_USBLOG_ITEM_SIZE || sTlkDbgUsbLogOffset >= dataLen){
		sTlkDbgUsbLogOffset = 0;
		tlkapi_qfifo_dropData(&sTlkDbgUsbLogFifo);
		return;
	}
	
	offset = 2+sTlkDbgUsbLogOffset;
	if(sTlkDbgUsbLogOffset+64 <= dataLen) sendLen = 64;
	else sendLen = dataLen-sTlkDbgUsbLogOffset;
	#if (TLK_USB_UDB_ENABLE)
	ret = tlkusb_udb_sendData(pData+offset, sendLen);
	if(ret != -TLK_EBUSY) sTlkDbgUsbLogOffset += sendLen;
	#endif
	if(sTlkDbgUsbLogOffset >= dataLen){
		sTlkDbgUsbLogOffset = 0;
		tlkapi_qfifo_dropData(&sTlkDbgUsbLogFifo);
	}
}


void tlkdbg_usblog_print(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args)
{
	uint16 serial;
	uint16 dataLen;

	serial = sTlkDbgUsbLogSerial ++;
	
	if(tlkapi_qfifo_isFull(&sTlkDbgUsbLogFifo)) return;

	tlkdbg_setPrintBuffer(sTlkDbgUsbLogCache+5, TLKDBG_USBLOG_ITEM_SIZE-5);
	
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	if(fileName != nullptr){
		printf("(%s//%03d)", fileName, lineNumb);
	}
	vprintf(format, args);

	tlkdbg_setPrintBuffer(nullptr, 0);
	
	dataLen = ((uint16)sTlkDbgUsbLogCache[6] << 8) | sTlkDbgUsbLogCache[5];
	if(dataLen != 0){
		dataLen += 5;
		sTlkDbgUsbLogCache[0] = (dataLen & 0xFF);
		sTlkDbgUsbLogCache[1] = (dataLen & 0xFF00) >> 8;
		sTlkDbgUsbLogCache[2] = 0x82;
		sTlkDbgUsbLogCache[3] = 0x08;
		sTlkDbgUsbLogCache[4] = 0x22;
		sTlkDbgUsbLogCache[5] = 0x00;
		sTlkDbgUsbLogCache[6] = 0x00;
//		tlksys_disable_interrupt();
		uint08 *pBuff = tlkapi_qfifo_takeBuff(&sTlkDbgUsbLogFifo);
		if(pBuff != nullptr){
			tmemcpy(pBuff, sTlkDbgUsbLogCache, dataLen+2);
		}
//		tlksys_restore_interrupt();
	}
}
void tlkdbg_usblog_array(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen)
{
	uint16 index;
	uint16 serial;

	serial = sTlkDbgUsbLogSerial ++;

	if(tlkapi_qfifo_isFull(&sTlkDbgUsbLogFifo)) return;
	
	tlkdbg_setPrintBuffer(sTlkDbgUsbLogCache+5, TLKDBG_USBLOG_ITEM_SIZE-5);
		
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	if(fileName != nullptr){
		printf("(%s//%03d)", fileName, lineNumb);
	}
	if(format != nullptr) printf("%s", format);
	printf("(%d)", dataLen);
	for(index=0; index<dataLen; index++){
		printf("%02x ", pData[index]);
	}

	tlkdbg_setPrintBuffer(nullptr, 0);
	
	dataLen = ((uint16)sTlkDbgUsbLogCache[6] << 8) | sTlkDbgUsbLogCache[5];
	if(dataLen != 0){
		dataLen += 5;
		sTlkDbgUsbLogCache[0] = (dataLen & 0xFF);
		sTlkDbgUsbLogCache[1] = (dataLen & 0xFF00) >> 8;
		sTlkDbgUsbLogCache[2] = 0x82;
		sTlkDbgUsbLogCache[3] = 0x08;
		sTlkDbgUsbLogCache[4] = 0x22;
		sTlkDbgUsbLogCache[5] = 0x00;
		sTlkDbgUsbLogCache[6] = 0x00;
//		tlksys_disable_interrupt();
		uint08 *pBuff = tlkapi_qfifo_takeBuff(&sTlkDbgUsbLogFifo);
		if(pBuff != nullptr){
			tmemcpy(pBuff, sTlkDbgUsbLogCache, dataLen+2);
		}
//		tlksys_restore_interrupt();
	}
}



_attribute_retention_code_ 
void tlkdbg_usblog_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen)
{
	uint08 *pBuff;
	uint16 buffLen;

	pBuff = tlkapi_qfifo_takeBuff(&sTlkDbgUsbLogFifo);
	if(pBuff == nullptr) return;

	if(dataLen+2 > TLKDBG_USBLOG_ITEM_SIZE) dataLen = TLKDBG_USBLOG_ITEM_SIZE-2;
	
	buffLen = 0;
	pBuff[buffLen++] = (dataLen+2) & 0xFF;
	pBuff[buffLen++] = ((dataLen+2) & 0xFF00) >> 8;
	pBuff[buffLen++] = status;
	pBuff[buffLen++] = buffNumb;
	if(dataLen != 0){
		tmemcpy(pBuff+buffLen, pData, dataLen);
		buffLen += dataLen;
	}
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_usblog_sendU08s(char *pSign, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3)
{
	uint08 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_usblog_sendData(pSign, pStr, (uint08*)buffer, 4);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_usblog_sendU16s(char *pSign, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3)
{
	uint16 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_usblog_sendData(pSign, pStr, (uint08*)buffer, 8);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_usblog_sendU32s(char *pSign, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3)
{
	uint32 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_usblog_sendData(pSign, pStr, (uint08*)buffer, 16);
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_usblog_sendData(char *pSign, char *pStr, uint08 *pData, uint16 dataLen)
{
	uint08 *pBuff;
	uint16 strLen;
	uint16 tempVar;
	uint16 buffLen;
	uint16 serial;
	uint08 signLen;
	uint08 buffer[TLKDBG_USBLOG_ITEM_SIZE+2];

	serial = sTlkDbgUsbLogSerial ++;

	if(tlkapi_qfifo_isFull(&sTlkDbgUsbLogFifo)) return;
	
	strLen = tstrlen(pStr);	
	if(pSign == nullptr) signLen = 0;
	else signLen = tstrlen(pSign);

	buffLen = 0;
	buffer[buffLen++] = 0x00; //(5+extLen+strLen+dataLen) & 0xFF;
	buffer[buffLen++] = 0x00; //((5+extLen+strLen+dataLen) & 0xFF00) >> 8;
	buffer[buffLen++] = 0x82;
	buffer[buffLen++] = 0x08;
	buffer[buffLen++] = 0x22;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	// Add Serial
	buffer[buffLen++] = '[';
	tempVar = (serial & 0xF000)>>12;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	buffer[buffLen++] = tempVar;
	tempVar = (serial & 0x0F00)>>8;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	buffer[buffLen++] = tempVar;
	tempVar = (serial & 0x00F0)>>4;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	buffer[buffLen++] = tempVar;
	tempVar = (serial & 0x000F)>>0;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	buffer[buffLen++] = tempVar;
	buffer[buffLen++] = ']';
	
	if(buffLen+signLen+16 > TLKDBG_USBLOG_ITEM_SIZE) return;
	if(signLen != 0){
		tmemcpy(buffer+buffLen, pSign, signLen);
		buffLen += signLen;
	}
	
	if(buffLen+strLen+4 > TLKDBG_USBLOG_ITEM_SIZE) strLen = TLKDBG_USBLOG_ITEM_SIZE-buffLen-4;
	if(strLen != 0){
		tmemcpy(buffer+buffLen, pStr, strLen);
		buffLen += strLen;
	}
	if(dataLen != 0){
		buffer[buffLen++] = ':';
		tempVar = tlkapi_arrayToStr(pData, dataLen, (char*)(buffer+buffLen), TLKDBG_USBLOG_ITEM_SIZE-2-buffLen, ' ');
		buffLen += tempVar;
	}
	buffer[0] = ((buffLen-2) & 0x00FF);
	buffer[1] = ((buffLen-2) & 0xFF00) >> 8;

	tlksys_disable_interrupt();
	pBuff = tlkapi_qfifo_takeBuff(&sTlkDbgUsbLogFifo);
	if(pBuff != nullptr){
		tmemcpy(pBuff, buffer, buffLen);
	}
	tlksys_restore_interrupt();
}




#endif //#if (TLKDBG_CFG_USB_LOG_ENABLE)
#endif //#if (TLK_DBG_LOG_ENABLE)

