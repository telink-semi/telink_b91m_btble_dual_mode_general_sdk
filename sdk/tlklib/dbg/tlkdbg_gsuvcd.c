/********************************************************************************************************
 * @file	tlkdbg_gsuvcd.c
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
#if (TLK_DBG_VCD_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_GSU_VCD_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_gsuvcd.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "drivers.h"


//GSU - GPIO simulate UART

static void tlkdbg_gsuvcd_send(uint08 *pData, uint16 dataLen);
static void tlkdbg_gsuvcd_putchar(uint08 byte);
//static bool tlkdbg_gsuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg);

static uint32 sTlkDbgGsuVcdGpioPin;
static uint32 sTlkDbgGsuVcdBitIntv;

static uint08 sTlkdbgGsuVcdBuffer[TLKDBG_GSU_VCD_BUFFER_SIZE];
static tlkapi_fifo_t sTlkDbgGsuVcdFifo;
//static tlkapi_timer_t sTlkDbgGsuVcdTimer;



void tlkdbg_gsuvcd_init(void)
{
	tlkapi_fifo_init(&sTlkDbgGsuVcdFifo, false, false, sTlkdbgGsuVcdBuffer, TLKDBG_GSU_VCD_BUFFER_SIZE);
//	tlkapi_timer_initNode(&sTlkDbgGsuVcdTimer, tlkdbg_gsuvcd_timer, nullptr, 1000);

	sTlkDbgGsuVcdGpioPin = TLKDBG_GSU_VCD_GPIO_PIN;
	sTlkDbgGsuVcdBitIntv = SYSTEM_TIMER_TICK_1S/TLKDBG_GSU_VCD_BAUD_RATE;
	gpio_function_en(sTlkDbgGsuVcdGpioPin);
	gpio_set_up_down_res(sTlkDbgGsuVcdGpioPin, GPIO_PIN_PULLUP_1M);
	gpio_output_en(sTlkDbgGsuVcdGpioPin);
	gpio_set_high_level(sTlkDbgGsuVcdGpioPin);
}


void tlkdbg_gsuvcd_reset(void)
{
	tlkapi_fifo_clear(&sTlkDbgGsuVcdFifo);
}
bool tlkdbg_gsuvcd_isBusy(void)
{
	if(tlkapi_fifo_isEmpty(&sTlkDbgGsuVcdFifo)) return false;
	else return true;
}
void tlkdbg_gsuvcd_handler(void)
{
	int ret;
	uint08 buffer[TLKDBG_GSU_VCD_CACHE_SIZE];
	while(!tlkapi_fifo_isEmpty(&sTlkDbgGsuVcdFifo)){
		ret = tlkapi_fifo_read(&sTlkDbgGsuVcdFifo, buffer, TLKDBG_GSU_VCD_CACHE_SIZE);
		if(ret <= 0) break;
		tlkdbg_gsuvcd_send(buffer, ret);
	}
}

//static bool tlkdbg_gsuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg)
//{
//	tlkdbg_gsuvcd_handler();
//	return true;
//}


_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_ref(void)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	int t=clock_time();
	buffer[buffLen++] = 0x20;
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}
// 4-byte sync word: 00 00 00 00
_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_sync(void)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	buffer[buffLen++] = 0x00;
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}      
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_tick(uint08 id)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	int t=clock_time();
	buffer[buffLen++] = 0x20 | (id&31);
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_level(uint08 id, uint08 level)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	int t=clock_time();
	buffer[buffLen++] = ((level) ? 0x60:0x40) | (id&31);
	buffer[buffLen++] = t & 0xFF;
	buffer[buffLen++] = (t & 0xFF00) >> 8;
	buffer[buffLen++] = (t & 0xFF0000) >> 16;
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_event(uint08 id)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	buffer[buffLen++] = 0x00 | (id&31);
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_byte(uint08 id, uint08 value)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	buffer[buffLen++] = 0x80 | (id&63);
	buffer[buffLen++] = value;
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_gsuvcd_word(uint08 id, uint16 value)
{
	uint08 buffLen = 0;
	uint08 buffer[4];
	tlksys_disable_interrupt();
	buffer[buffLen++] = 0xc0 | (id&63);
	buffer[buffLen++] = value & 0xFF;
	buffer[buffLen++] = (value & 0xFF00) >> 8;
	tlkapi_fifo_write(&sTlkDbgGsuVcdFifo, buffer, buffLen);
	tlksys_restore_interrupt();
}


static void tlkdbg_gsuvcd_send(uint08 *pData, uint16 dataLen)
{
	uint16 index;
	for(index=0; index<dataLen; index++){
		tlkdbg_gsuvcd_putchar(pData[index]);
	}
}
static void tlkdbg_gsuvcd_putchar(uint08 byte)
{
	uint08 index = 0;
	uint32 time1 = 0;
	uint32 time2 = 0;
	uint08 bits[10] = {0};
	uint08 bit0 = reg_gpio_out(sTlkDbgGsuVcdGpioPin) & (~(sTlkDbgGsuVcdGpioPin & 0xff));
	uint08 bit1 = reg_gpio_out(sTlkDbgGsuVcdGpioPin) | (sTlkDbgGsuVcdGpioPin & 0xff);

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
		while(time1-time2 < sTlkDbgGsuVcdBitIntv){
			time1 = reg_system_tick;
		}
		reg_gpio_out(sTlkDbgGsuVcdGpioPin) = bits[index];
	}
	tlksys_restore_interrupt();
}




#endif //#if (TLKDBG_CFG_GSU_VCD_ENABLE)
#endif //#if (TLK_DBG_VCD_ENABLE)

