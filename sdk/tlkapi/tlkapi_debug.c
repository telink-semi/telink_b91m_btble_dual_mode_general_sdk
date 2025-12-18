/********************************************************************************************************
 * @file	tlkapi_debug.c
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
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "string.h"
#include "drivers.h"
#include "tlk_config.h"
#include "tlk_debug.h"
#include "tlkapi/tlkapi_type.h"
#include "tlkapi/tlkapi_error.h"
#include "tlkapi/tlkapi_qfifo.h"
#include "tlkapi/tlkapi_debug.h"
#include "tlkapi/tlkapi_string.h"
#include "tlklib/dbg/tlkdbg.h"


#if (TLK_DBG_LOG_ENABLE)

void tlkdbg_delayForPrint(uint32 us)
{
	uint32 timer = clock_time();
	while(true){
		#if (TLK_USB_UDB_ENABLE)
		extern void tlkusb_handler(void);
		tlkusb_handler();
		#endif
		tlkdbg_handler();
		if(clock_time_exceed(timer, us)) break;
	}
}

#endif

_attribute_noinline_
int tlkdbg_sprintf(char *pOut, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	return vsprintf(pOut, format, args);
}


#if !(TLK_DBG_LOG_ENABLE)

void tlkdbg_handler(void)
{

}
_attribute_ram_code_sec_
void tlkapi_debug_default(void)
{
	
}
void __attribute__((unused))tlkdbg_warn(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_info(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_trace(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_fatal(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_error(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_array(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_assert(bool isAssert, uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...) __attribute__((weak, alias("tlkapi_debug_default")));

void __attribute__((unused))tlkdbg_warn1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_info1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_trace1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_fatal1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_error1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_array1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_assert1(bool isAssert, uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args) __attribute__((weak, alias("tlkapi_debug_default")));

void __attribute__((unused))tlkdbg_sendData(uint32 flags, char *pStr, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_sendU08s(uint32 flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_sendU16s(uint32 flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_sendU32s(uint32 flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3) __attribute__((weak, alias("tlkapi_debug_default")));

void __attribute__((unused))tlkdbg_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen) __attribute__((weak, alias("tlkapi_debug_default")));
void __attribute__((unused))tlkdbg_delayForPrint(uint32 us) __attribute__((weak, alias("tlkapi_debug_default")));

#endif


#if !(TLK_DBG_VCD_ENABLE)

_attribute_ram_code_sec_
void tlkdbg_vcd_default(void){}
void __attribute__((unused))tlkdbg_vcd_ref(void) __attribute__((weak, alias("tlkdbg_vcd_default")));
void __attribute__((unused))tlkdbg_vcd_sync(void) __attribute__((weak, alias("tlkdbg_vcd_default")));
void __attribute__((unused))tlkdbg_vcd_tick(uint32 flags, uint08 id) __attribute__((weak, alias("tlkdbg_vcd_default")));
void __attribute__((unused))tlkdbg_vcd_level(uint32 flags, uint08 id, uint08 level) __attribute__((weak, alias("tlkdbg_vcd_default")));
void __attribute__((unused))tlkdbg_vcd_event(uint32 flags, uint08 id) __attribute__((weak, alias("tlkdbg_vcd_default")));
void __attribute__((unused))tlkdbg_vcd_byte(uint32 flags, uint08 id, uint08 value) __attribute__((weak, alias("tlkdbg_vcd_default")));
void __attribute__((unused))tlkdbg_vcd_word(uint32 flags, uint08 id, uint16 value) __attribute__((weak, alias("tlkdbg_vcd_default")));


#endif


