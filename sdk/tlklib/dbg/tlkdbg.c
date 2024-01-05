/********************************************************************************************************
 * @file	tlkdbg.c
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
#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#include "tlkdbg_define.h"
#if (TLKDBG_CFG_USB_LOG_ENABLE)
#include "tlkdbg_usblog.h"
#endif
#if (TLKDBG_CFG_USB_VCD_ENABLE)
#include "tlkdbg_usbvcd.h"
#endif
#if (TLKDBG_CFG_USB_DWN_ENABLE)
#include "tlkdbg_usbdwn.h"
#endif
#if (TLKDBG_CFG_GSU_LOG_ENABLE)
#include "tlkdbg_gsulog.h"
#endif
#if (TLKDBG_CFG_GSU_VCD_ENABLE)
#include "tlkdbg_gsuvcd.h"
#endif
#if (TLKDBG_CFG_HPU_LOG_ENABLE)
#include "tlkdbg_hpulog.h"
#endif
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#include "tlkdbg_hpuvcd.h"
#endif
#if (TLKDBG_CFG_HPU_DWN_ENABLE)
#include "tlkdbg_hpudwn.h"
#endif
#if (TLKDBG_CFG_HWU_LOG_ENABLE)
#include "tlkdbg_hwulog.h"
#endif
#if (TLKDBG_CFG_HWU_VCD_ENABLE)
#include "tlkdbg_hwuvcd.h"
#endif

extern bool tlk_debug_logIsEnable1(unsigned int flags);
extern char *tlk_debug_getDbgSign(unsigned int flags);
extern bool tlk_debug_logIsEnable(unsigned int flags, unsigned int dbgFlag);
extern bool tlk_debug_vcdIsEnable(unsigned int flags);

static uint08 sTlkDbgMask = TLKAPI_DBG_FLAG_ALL;

void tlkdbg_init(void)
{
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_init();
	#endif
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_init();
	#endif
	#if (TLKDBG_CFG_USB_DWN_ENABLE)
	tlkdbg_usbdwn_init();
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_init();
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_init();
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_init();
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_init();
	#endif
	#if (TLKDBG_CFG_HPU_DWN_ENABLE)
	tlkdbg_hpudwn_init();
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_init();
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_init();
	#endif
	sTlkDbgMask = TLKAPI_DBG_FLAG_ALL;
}

void tlkdbg_reset(void)
{
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_reset();
	#endif
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_reset();
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_reset();
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_reset();
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_reset();
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_reset();
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_reset();
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_reset();
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}

bool tlkdbg_isBusy(void)
{
	bool isBusy = false;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	if(!isBusy) isBusy = tlkdbg_usblog_isBusy();
	#endif
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	if(!isBusy) isBusy = tlkdbg_usbvcd_isBusy();
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	if(!isBusy) isBusy = tlkdbg_gsulog_isBusy();
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	if(!isBusy) isBusy = tlkdbg_gsuvcd_isBusy();
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	if(!isBusy) isBusy = tlkdbg_hpulog_isBusy();
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	if(!isBusy) isBusy = tlkdbg_hpuvcd_isBusy();
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	if(!isBusy) isBusy = tlkdbg_hwulog_isBusy();
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	if(!isBusy) isBusy = tlkdbg_hwuvcd_isBusy();
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
	return isBusy;
}
#if (TLK_CFG_DBG_ENABLE)
_attribute_ram_code_sec_noinline_ 
#endif
void tlkdbg_handler(void)
{
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_handler();
	#endif
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_handler();
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_handler();
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_handler();
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_handler();
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_handler();
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_handler();
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_handler();
	#endif
}

void tlkdbg_setLevel(TLKAPI_DEBUG_LEVEL_ENUM level)
{
	sTlkDbgMask = level;
}

_attribute_noinline_
void tlkdbg_warn(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_WARN) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_WARN)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	va_list args;
	va_start(args, format);	
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	va_end(args);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_info(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_INFO) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_INFO)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	va_list args;
	va_start(args, format);
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	va_end(args);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_trace(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_TRACE) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_TRACE)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	va_list args;
	va_start(args, format);
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	va_end(args);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_fatal(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_FATAL) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_FATAL)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	va_list args;
	va_start(args, format);
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	va_end(args);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_error(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_ERROR) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_ERROR)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	va_list args;
	va_start(args, format);
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	va_end(args);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_array(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_ARRAY) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_ARRAY)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_assert(bool isAssert, uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_ASSERT) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_ASSERT)) return;
	if(!isAssert) return;
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	va_list args;
	va_start(args, format);
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	va_end(args);
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}

_attribute_noinline_
void tlkdbg_warn1(uint32 flags, char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args)
{
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_WARN)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_WARN_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_info1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_INFO) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_INFO)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_INFO_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_trace1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_TRACE) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_TRACE)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_TRACE_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_fatal1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_FATAL) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_FATAL)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_FATAL_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_error1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_ERROR) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_ERROR)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_ERROR_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_array1(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_ARRAY) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_ARRAY)) return;
	if(pSign == nullptr) pSign = tlk_debug_getDbgSign(flags);
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_array(pSign, TLKAPI_ARRAY_HEAD, fileName, lineNumb, format, pData, dataLen);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_noinline_
void tlkdbg_assert1(bool isAssert, uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, va_list args)
{
	if((sTlkDbgMask & TLK_DEBUG_DBG_FLAG_ASSERT) == 0) return;
	if(!tlk_debug_logIsEnable(flags, TLK_DEBUG_DBG_FLAG_ASSERT)) return;
	if(!isAssert) return;
	if(fileName != nullptr){
		fileName = strrchr(fileName,'/')?strrchr(fileName,'/')+1:fileName;
	}
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_print(pSign, TLKAPI_ASSERT_HEAD, fileName, lineNumb, format, args);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}


_attribute_ram_code_sec_noinline_ 
void tlkdbg_sendU08s(uint32 flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3)
{
	if(!tlk_debug_logIsEnable1(flags)) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_sendU08s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_sendU08s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_sendU08s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_sendU08s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_sendU16s(uint32 flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3)
{
	if(!tlk_debug_logIsEnable1(flags)) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_sendU16s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_sendU16s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_sendU16s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_sendU16s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_ram_code_sec_noinline_ 
void tlkdbg_sendU32s(uint32 flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3)
{
	if(!tlk_debug_logIsEnable1(flags)) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_sendU32s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_sendU32s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_sendU32s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_sendU32s(tlk_debug_getDbgSign(flags), pStr, val0, val1, val2, val3);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}
_attribute_ram_code_sec_noinline_
void tlkdbg_sendData(uint32 flags, char *pStr, uint08 *pData, uint16 dataLen)
{
	if(!tlk_debug_logIsEnable1(flags)) return;
	#if (TLK_CFG_OS_ENABLE)
	tlksys_enter_critical();
	#endif
	#if (TLKDBG_CFG_USB_LOG_ENABLE)
	tlkdbg_usblog_sendData(tlk_debug_getDbgSign(flags), pStr, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_GSU_LOG_ENABLE)
	tlkdbg_gsulog_sendData(tlk_debug_getDbgSign(flags), pStr, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_HPU_LOG_ENABLE)
	tlkdbg_hpulog_sendData(tlk_debug_getDbgSign(flags), pStr, pData, dataLen);
	#endif
	#if (TLKDBG_CFG_HWU_LOG_ENABLE)
	tlkdbg_hwulog_sendData(tlk_debug_getDbgSign(flags), pStr, pData, dataLen);
	#endif
	#if (TLK_CFG_OS_ENABLE)
	tlksys_leave_critical();
	#endif
}



_attribute_ram_code_sec_noinline_
void tlkdbg_vcd_sync(void)
{
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_sync();
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_sync();
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_sync();
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_sync();
	#endif
}
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
_attribute_ram_code_sec_noinline_
void tlkdbg_vcd_tick(uint32 flags, uint08 id)
{
	if(!tlk_debug_vcdIsEnable(flags)) return;
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_tick(id);
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_tick(id);
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_tick(id);
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_tick(id);
	#endif
}
//1-byte (01x_id-5bits) 1-bit data: b=0 or 1.
_attribute_ram_code_sec_noinline_
void tlkdbg_vcd_level(uint32 flags, uint08 id, uint08 level)
{
	if(!tlk_debug_vcdIsEnable(flags)) return;
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_level(id, level);
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_level(id, level);
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_level(id, level);
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_level(id, level);
	#endif
}
//1-byte (000_id-5bits)
_attribute_ram_code_sec_noinline_
void tlkdbg_vcd_event(uint32 flags, uint08 id)
{
	if(!tlk_debug_vcdIsEnable(flags)) return;
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_event(id);
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_event(id);
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_event(id);
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_event(id);
	#endif
}
//2-byte (10-id-6bits) 8-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_vcd_byte(uint32 flags, uint08 id, uint08 value)
{
	if(!tlk_debug_vcdIsEnable(flags)) return;
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_byte(id, value);
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_byte(id, value);
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_byte(id, value);
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_byte(id, value);
	#endif
}
//3-byte (11-id-6bits) 16-bit data
_attribute_ram_code_sec_noinline_
void tlkdbg_vcd_word(uint32 flags, uint08 id, uint16 value)
{
	if(!tlk_debug_vcdIsEnable(flags)) return;
	#if (TLKDBG_CFG_USB_VCD_ENABLE)
	tlkdbg_usbvcd_word(id, value);
	#endif
	#if (TLKDBG_CFG_HPU_VCD_ENABLE)
	tlkdbg_hpuvcd_word(id, value);
	#endif
	#if (TLKDBG_CFG_GSU_VCD_ENABLE)
	tlkdbg_gsuvcd_word(id, value);
	#endif
	#if (TLKDBG_CFG_HWU_VCD_ENABLE)
	tlkdbg_hwuvcd_word(id, value);
	#endif
}


static uint16  sTlkdbgPrintBuffLen;
static uint08 *spTlkdbgPrintBuffer;

void tlkdbg_setPrintBuffer(uint08 *pBuffer, uint16 buffLen)
{
//	spTlkdbgPrintBuffer = nullptr;
	sTlkdbgPrintBuffLen = buffLen;
	spTlkdbgPrintBuffer = pBuffer;
	if(spTlkdbgPrintBuffer != nullptr){
		spTlkdbgPrintBuffer[0] = 0x00;
		spTlkdbgPrintBuffer[1] = 0x00;
	}
}

__attribute__((used)) int _write(int fd, const unsigned char *buf, int size)
{
	uint16 copyLen;
	uint16 dataLen;
	
	(void)fd;
	
	if(spTlkdbgPrintBuffer == nullptr) return size;
	
	dataLen = ((uint16)spTlkdbgPrintBuffer[1] << 8) | spTlkdbgPrintBuffer[0];
	if(dataLen != 0){
		dataLen += 2;
	}else{
		dataLen = 2;
	}
	
	if(dataLen >= sTlkdbgPrintBuffLen) return size;
	if(dataLen+size <= sTlkdbgPrintBuffLen) copyLen = size;
	else copyLen = sTlkdbgPrintBuffLen-dataLen;
	if(copyLen != 0) tmemcpy(spTlkdbgPrintBuffer+dataLen, buf, copyLen);
	dataLen += copyLen;
	
	spTlkdbgPrintBuffer[0] = ((dataLen-2) & 0x00FF);
	spTlkdbgPrintBuffer[1] = ((dataLen-2) & 0xFF00) >> 8;
    return size;
}



#endif //#if (TLK_DBG_LOG_ENABLE)

