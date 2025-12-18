/********************************************************************************************************
 * @file	tlkdbg_usbvcd.c
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_USB_VCD_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_usbvcd.h"
#include "drivers.h"


#define tlkdbg_usbvcd_rcd(d)   reg_usb_ep8_dat=d   //VCD Record



static uint32 sTlkDbgUsbVcdTicks;


void tlkdbg_usbvcd_init(void)
{
	
}

void tlkdbg_usbvcd_reset(void)
{
	
}

bool tlkdbg_usbvcd_isBusy(void)
{
	return true;
}

void tlkdbg_usbvcd_handler(void)
{
	if(sTlkDbgUsbVcdTicks == 0 || clock_time_exceed(sTlkDbgUsbVcdTicks, 10000)){
		sTlkDbgUsbVcdTicks = clock_time() | 1;
		tlkdbg_usbvcd_ref();
		tlkdbg_usbvcd_sync();
	}
}



_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_ref(void)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(0x20);
	int t=clock_time();
	tlkdbg_usbvcd_rcd(t);
	tlkdbg_usbvcd_rcd(t>>8);
	tlkdbg_usbvcd_rcd(t>>16);
	tlksys_restore_interrupt();
}
// 4-byte sync word: 00 00 00 00
_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_sync(void)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(0);
	tlkdbg_usbvcd_rcd(0);
	tlkdbg_usbvcd_rcd(0);
	tlkdbg_usbvcd_rcd(0);
	tlksys_restore_interrupt();
}      
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_tick(uint08 id)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(0x20|(id&31));
	int t=clock_time();
	tlkdbg_usbvcd_rcd(t);
	tlkdbg_usbvcd_rcd(t>>8);
	tlkdbg_usbvcd_rcd(t>>16);
	tlksys_restore_interrupt();
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_level(uint08 id, uint08 level)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(((level)?0x60:0x40)|(id&31));
	int t=clock_time();
	tlkdbg_usbvcd_rcd(t);
	tlkdbg_usbvcd_rcd(t>>8);
	tlkdbg_usbvcd_rcd(t>>16);
	tlksys_restore_interrupt();
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_event(uint08 id)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(0x00|(id&31));
	tlksys_restore_interrupt();
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_byte(uint08 id, uint08 value)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(0x80|(id&63));
	tlkdbg_usbvcd_rcd(value);
	tlksys_restore_interrupt();
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_usbvcd_word(uint08 id, uint16 value)
{
	tlksys_disable_interrupt();
	tlkdbg_usbvcd_rcd(0xc0|(id&63));
	tlkdbg_usbvcd_rcd(value);
	tlkdbg_usbvcd_rcd((value)>>8);
	tlksys_restore_interrupt();
}




#endif //#if (TLKDBG_CFG_USB_VCD_ENABLE)
#endif //#if (TLK_DBG_VCD_ENABLE)

