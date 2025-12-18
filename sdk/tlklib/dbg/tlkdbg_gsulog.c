/********************************************************************************************************
 * @file	tlkdbg_gsulog.c
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
#if (TLKDBG_CFG_GSU_LOG_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_gsulog.h"
#include "drivers.h"

//GSU - GPIO simulate UART

#define TLKDBG_GSULOG_NEWLINE_MODE1_ENABLE         1

static void tlkdbg_gsulog_send(uint08 *pData, uint16 dataLen);
static void tlkdbg_gsulog_putchar(uint08 byte);


static uint32 sTlkDbgGsuLogGpioPin;
static uint32 sTlkDbgGsuLogBitIntv;

static uint16 sTlkDbgGsuLogSerial;
static uint08 sTlkDbgGsuLogCache[TLKDBG_GSU_LOG_CACHE_SIZE+4];
static uint08 sTlkDbgGsuLogBuffer[TLKDBG_GSU_LOG_BUFFER_SIZE];
static tlkapi_fifo_t sTlkDbgGsuLogFifo;



void tlkdbg_gsulog_init(void)
{
	tlkapi_fifo_init(&sTlkDbgGsuLogFifo, false, false, sTlkDbgGsuLogBuffer, TLKDBG_GSU_LOG_BUFFER_SIZE);
	
	sTlkDbgGsuLogGpioPin = TLKDBG_GSU_LOG_GPIO_PIN;
	sTlkDbgGsuLogBitIntv = SYSTEM_TIMER_TICK_1S/TLKDBG_GSU_LOG_BAUD_RATE;
	gpio_function_en(sTlkDbgGsuLogGpioPin);
	gpio_set_up_down_res(sTlkDbgGsuLogGpioPin, GPIO_PIN_PULLUP_1M);
	gpio_output_en(sTlkDbgGsuLogGpioPin);
	gpio_set_high_level(sTlkDbgGsuLogGpioPin);
}

void tlkdbg_gsulog_reset(void)
{
	tlkapi_fifo_clear(&sTlkDbgGsuLogFifo);
}

bool tlkdbg_gsulog_isBusy(void)
{
	if(tlkapi_fifo_isEmpty(&sTlkDbgGsuLogFifo)) return false;
	else return true;
}

void tlkdbg_gsulog_handler(void)
{
	int ret;
	while(!tlkapi_fifo_isEmpty(&sTlkDbgGsuLogFifo)){
		ret = tlkapi_fifo_read(&sTlkDbgGsuLogFifo, sTlkDbgGsuLogCache, TLKDBG_GSU_LOG_CACHE_SIZE);
		if(ret <= 0) break;
		tlkdbg_gsulog_send(sTlkDbgGsuLogCache, ret);
	}
}


void tlkdbg_gsulog_print(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args)
{
	uint16 serial;
	uint16 dataLen;

	serial = sTlkDbgGsuLogSerial ++;

	tlkdbg_setPrintBuffer(sTlkDbgGsuLogCache, TLKDBG_GSU_LOG_CACHE_SIZE);
	
	printf("[%04x]",serial);
	if(pSign != nullptr) printf(pSign);
	if(pHead != nullptr) printf(pHead);
	if(fileName != nullptr){
		printf("(%s//%03d)", fileName, lineNumb);
	}
	vprintf(format, args);

	tlkdbg_setPrintBuffer(nullptr, 0);
	
	tlksys_disable_interrupt();
	dataLen = ((uint16)sTlkDbgGsuLogCache[1] << 8) | sTlkDbgGsuLogCache[0];
	#if (TLKDBG_GSULOG_NEWLINE_MODE1_ENABLE)
	sTlkDbgGsuLogCache[dataLen+2+0] = '\r';
	sTlkDbgGsuLogCache[dataLen+2+1] = '\n';
	tlkapi_fifo_write(&sTlkDbgGsuLogFifo, sTlkDbgGsuLogCache+2, dataLen+2);
	#else
	sTlkDbgGsuLogCache[dataLen+2+0] = '\n';
	tlkapi_fifo_write(&sTlkDbgGsuLogFifo, sTlkDbgGsuLogCache+2, dataLen+1);
	#endif
	tlksys_restore_interrupt();
}
void tlkdbg_gsulog_array(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen)
{
	uint16 index;
	uint16 serial;
	uint16 optLen;

	serial = sTlkDbgGsuLogSerial ++;

	if(tlkapi_fifo_idleLen(&sTlkDbgGsuLogFifo) < 128) return;
	
	tlkdbg_setPrintBuffer(sTlkDbgGsuLogCache, TLKDBG_GSU_LOG_CACHE_SIZE);
		
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
	optLen = ((uint16)sTlkDbgGsuLogCache[1] << 8) | sTlkDbgGsuLogCache[0];
	#if (TLKDBG_GSULOG_NEWLINE_MODE1_ENABLE)
	sTlkDbgGsuLogCache[optLen+2+0] = '\r';
	sTlkDbgGsuLogCache[optLen+2+1] = '\n';
	tlkapi_fifo_write(&sTlkDbgGsuLogFifo, sTlkDbgGsuLogCache+2, optLen+2);
	#else
	sTlkDbgGsuLogCache[optLen+2+0] = '\n';
	tlkapi_fifo_write(&sTlkDbgGsuLogFifo, sTlkDbgGsuLogCache+2, optLen+1);
	#endif
	tlksys_restore_interrupt();
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_gsulog_sendU08s(char *pSign, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3)
{
	uint08 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_gsulog_sendData(pSign, pStr, (uint08*)buffer, 4);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_gsulog_sendU16s(char *pSign, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3)
{
	uint16 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_gsulog_sendData(pSign, pStr, (uint08*)buffer, 8);
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_gsulog_sendU32s(char *pSign, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3)
{
	uint32 buffer[4];
	buffer[0] = val0;
	buffer[1] = val1;
	buffer[2] = val2;
	buffer[3] = val3;
	tlkdbg_gsulog_sendData(pSign, pStr, (uint08*)buffer, 16);
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_gsulog_sendData(char *pSign, char *pStr, uint08 *pData, uint16 dataLen)
{
	uint16 strLen;
	uint16 tempVar;
	uint16 buffLen;
	uint16 serial;
	uint08 signLen;
	uint08 pBuff[TLKDBG_GSU_LOG_IRQ_CACHE_SIZE+2];

	serial = sTlkDbgGsuLogSerial ++;
	
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
	
	if(buffLen+signLen+16 > TLKDBG_GSU_LOG_IRQ_CACHE_SIZE) return;
	if(signLen != 0){
		tmemcpy(pBuff+buffLen, pSign, signLen);
		buffLen += signLen;
	}
	
	if(buffLen+strLen+4 > TLKDBG_GSU_LOG_IRQ_CACHE_SIZE) strLen = TLKDBG_GSU_LOG_IRQ_CACHE_SIZE-buffLen-4;
	if(strLen != 0){
		tmemcpy(pBuff+buffLen, pStr, strLen);
		buffLen += strLen;
	}
	if(dataLen != 0){
		pBuff[buffLen++] = ':';
		tempVar = tlkapi_arrayToStr(pData, dataLen, (char*)(pBuff+buffLen), TLKDBG_GSU_LOG_IRQ_CACHE_SIZE-2-buffLen, ' ');
		buffLen += tempVar;
	}
	tlksys_disable_interrupt();
	#if (TLKDBG_GSULOG_NEWLINE_MODE1_ENABLE)
	pBuff[buffLen++] = '\r';
	#endif
	pBuff[buffLen++] = '\n';
	tlkapi_fifo_write(&sTlkDbgGsuLogFifo, pBuff, buffLen);
	tlksys_restore_interrupt();
}



static void tlkdbg_gsulog_send(uint08 *pData, uint16 dataLen)
{
	uint16 index;
	for(index=0; index<dataLen; index++){
		tlkdbg_gsulog_putchar(pData[index]);
	}
}
static void tlkdbg_gsulog_putchar(uint08 byte)
{
	uint08 index = 0;
	uint32 time1 = 0;
	uint32 time2 = 0;
	uint08 bits[10] = {0};
	uint08 bit0 = reg_gpio_out(sTlkDbgGsuLogGpioPin) & (~(sTlkDbgGsuLogGpioPin & 0xff));
	uint08 bit1 = reg_gpio_out(sTlkDbgGsuLogGpioPin) | (sTlkDbgGsuLogGpioPin & 0xff);

	bits[0] = bit0;
	bits[1] = (byte & 0x01)? bit1 : bit0;
	bits[2] = ((byte>>1) & 0x01)? bit1 : bit0;
	bits[3] = ((byte>>2) & 0x01)? bit1 : bit0;
	bits[4] = ((byte>>3) & 0x01)? bit1 : bit0;
	bits[5] = ((byte>>4) & 0x01)? bit1 : bit0;
	bits[6] = ((byte>>5) & 0x01)? bit1 : bit0;
	bits[7] = ((byte>>6) & 0x01)? bit1 : bit0;
	bits[8] = ((byte>>7) & 0x01)? bit1 : bit0;
	bits[9] = bit1;

	tlksys_disable_interrupt();
	time1 = reg_system_tick;
	for(index=0; index<10; index++){
		time2 = time1;
		while(time1-time2 < sTlkDbgGsuLogBitIntv){
			time1 = reg_system_tick;
		}
		reg_gpio_out(sTlkDbgGsuLogGpioPin) = bits[index];
	}
	tlksys_restore_interrupt();
}



#endif //#if (TLKDBG_CFG_GSU_LOG_ENABLE)
#endif //#if (TLK_DBG_LOG_ENABLE)

