/********************************************************************************************************
 * @file	tlkdbg_hpulog.c
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
#if (TLKDBG_CFG_HPU_LOG_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_hpulog.h"

//HPU - Hardware Protocol UART

static uint16 sTlkDbgHpuLogSerial;
static uint08 sTlkdbgHpuLogCache[TLKDBG_HPU_LOG_CACHE_SIZE+4];
static uint08 sTlkdbgHpuLogBuffer[TLKDBG_HPU_LOG_BUFFER_SIZE];
static tlkapi_fifo_t sTlkDbgHpuLogFifo;
static uint16 sTlkDbgHpuLogSendSize = TLKDBG_HPU_LOG_CACHE_SIZE;
static TlkDbgHpuLogSendCB sTlkDbgHpuLogSendCB = nullptr;


void tlkdbg_hpulog_init(void)
{
	sTlkDbgHpuLogSendSize = TLKDBG_HPU_LOG_CACHE_SIZE;
	sTlkDbgHpuLogSendCB = nullptr;
	tlkapi_fifo_init(&sTlkDbgHpuLogFifo, false, false, sTlkdbgHpuLogBuffer, TLKDBG_HPU_LOG_BUFFER_SIZE);
}

void tlkdbg_hpulog_regSendCB(TlkDbgHpuLogSendCB cb, uint16 maxLen)
{
	sTlkDbgHpuLogSendCB = cb;
	if(maxLen == 0 || maxLen >= TLKDBG_HPU_LOG_CACHE_SIZE){
		sTlkDbgHpuLogSendSize = TLKDBG_HPU_LOG_CACHE_SIZE;
	}else{
		sTlkDbgHpuLogSendSize = maxLen;
	}
}

void tlkdbg_hpulog_reset(void)
{
	tlkapi_fifo_clear(&sTlkDbgHpuLogFifo);
}

bool tlkdbg_hpulog_isBusy(void)
{
	if(tlkapi_fifo_isEmpty(&sTlkDbgHpuLogFifo)) return false;
	else return true;
}

void tlkdbg_hpulog_handler(void)
{
	int ret;
	uint readLen;

	if(sTlkDbgHpuLogSendCB == nullptr) return;
	while(!tlkapi_fifo_isEmpty(&sTlkDbgHpuLogFifo)){
		ret = tlkapi_fifo_readCommon(&sTlkDbgHpuLogFifo, sTlkdbgHpuLogCache, sTlkDbgHpuLogSendSize, false);
		if(ret <= 0) break;
		readLen = ret;
		ret = sTlkDbgHpuLogSendCB(sTlkdbgHpuLogCache, readLen);
		if(ret >= 0 || ret == -TLK_ENOSUPPORT){
			tlkapi_fifo_chgReadPos(&sTlkDbgHpuLogFifo, readLen);
		} else {
			break;
		}
	}
}


void tlkdbg_hpulog_print(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args)
{
	uint16 serial;
	uint16 dataLen;

	serial = sTlkDbgHpuLogSerial ++;

	tlkdbg_setPrintBuffer(sTlkdbgHpuLogCache, TLKDBG_HPU_LOG_CACHE_SIZE);
	
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	if(fileName != nullptr){
		printf("(%s//%03d)", fileName, lineNumb);
	}
	vprintf(format, args);

	tlkdbg_setPrintBuffer(nullptr, 0);
	
	tlksys_disable_interrupt();
	dataLen = ((uint16)sTlkdbgHpuLogCache[1] << 8) | sTlkdbgHpuLogCache[0];
	sTlkdbgHpuLogCache[dataLen+2+0] = '\n';
	tlkapi_fifo_write(&sTlkDbgHpuLogFifo, sTlkdbgHpuLogCache+2, dataLen+1);
	tlksys_restore_interrupt();
}
void tlkdbg_hpulog_array(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen)
{
	uint16 index;
	uint16 serial;
	uint16 optLen;

	serial = sTlkDbgHpuLogSerial ++;

	if(tlkapi_fifo_idleLen(&sTlkDbgHpuLogFifo) < 128) return;
	
	tlkdbg_setPrintBuffer(sTlkdbgHpuLogCache, TLKDBG_HPU_LOG_CACHE_SIZE);
		
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
	
	tlksys_disable_interrupt();
	optLen = ((uint16)sTlkdbgHpuLogCache[1] << 8) | sTlkdbgHpuLogCache[0];
	sTlkdbgHpuLogCache[optLen+2+0] = '\n';
	tlkapi_fifo_write(&sTlkDbgHpuLogFifo, sTlkdbgHpuLogCache+2, optLen+1);
	tlksys_restore_interrupt();
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_hpulog_sendU08s(char *pSign, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3)
{
	uint08 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_hpulog_sendData(pSign, pStr, (uint08*)buffer, 4);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_hpulog_sendU16s(char *pSign, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3)
{
	uint16 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_hpulog_sendData(pSign, pStr, (uint08*)buffer, 8);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_hpulog_sendU32s(char *pSign, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3)
{
	uint32 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_hpulog_sendData(pSign, pStr, (uint08*)buffer, 16);
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_hpulog_sendData(char *pSign, char *pStr, uint08 *pData, uint16 dataLen)
{
	uint16 strLen;
	uint16 tempVar;
	uint16 buffLen;
	uint16 serial;
	uint08 signLen;
	uint08 pBuff[TLKDBG_HPU_LOG_IRQ_CACHE_SIZE+2];

	serial = sTlkDbgHpuLogSerial ++;
	
	strLen = tstrlen(pStr);	
	if(pSign == nullptr) signLen = 0;
	else signLen = tstrlen(pSign);

	buffLen = 0;
	// Add Serial
	pBuff[buffLen++] = '[';
	tempVar = (serial & 0xF000)>>12;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	tempVar = (serial & 0x0F00)>>8;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	tempVar = (serial & 0x00F0)>>4;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	tempVar = (serial & 0x000F)>>0;
	if(tempVar <= 9) tempVar = '0'+tempVar;
	else tempVar = 'a'+(tempVar-10);
	pBuff[buffLen++] = tempVar;
	pBuff[buffLen++] = ']';
	
	if(buffLen+signLen+16 > TLKDBG_HPU_LOG_IRQ_CACHE_SIZE) return;
	if(signLen != 0){
		tmemcpy(pBuff+buffLen, pSign, signLen);
		buffLen += signLen;
	}
	
	if(buffLen+strLen+4 > TLKDBG_HPU_LOG_IRQ_CACHE_SIZE) strLen = TLKDBG_HPU_LOG_IRQ_CACHE_SIZE-buffLen-4;
	if(strLen != 0){
		tmemcpy(pBuff+buffLen, pStr, strLen);
		buffLen += strLen;
	}
	if(dataLen != 0){
		pBuff[buffLen++] = ':';
		tempVar = tlkapi_arrayToStr(pData, dataLen, (char*)(pBuff+buffLen), TLKDBG_HPU_LOG_IRQ_CACHE_SIZE-2-buffLen, ' ');
		buffLen += tempVar;
	}
	tlksys_disable_interrupt();
	pBuff[buffLen++] = '\n';
	tlkapi_fifo_write(&sTlkDbgHpuLogFifo, pBuff, buffLen);
	tlksys_restore_interrupt();
}





#endif //#if (TLKDBG_CFG_HPU_LOG_ENABLE)
#endif //#if (TLK_DBG_LOG_ENABLE)

