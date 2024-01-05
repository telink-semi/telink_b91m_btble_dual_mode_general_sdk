/********************************************************************************************************
 * @file	tlkapi_debug.h
 *
 * @brief	This is the header file for BTBLE SDK
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
#ifndef TLKAPI_DEBUG_H
#define TLKAPI_DEBUG_H


#define TLKAPI_DBG_WARN_FLAG        0x02
#define TLKAPI_DBG_INFO_FLAG        0x04
#define TLKAPI_DBG_TRACE_FLAG       0x08
#define TLKAPI_DBG_ERROR_FLAG       0x10
#define TLKAPI_DBG_FATAL_FLAG       0x20
#define TLKAPI_DBG_ARRAY_FLAG       0x40
#define TLKAPI_DBG_ASSERT_FLAG      0x80
#define TLKAPI_DBG_FLAG_ALL         0xFE

#define TLKAPI_WARN_HEAD       "<WARN>"
#define TLKAPI_INFO_HEAD       "<INFO>"
#define TLKAPI_TRACE_HEAD      "<TRACE>"
#define TLKAPI_FATAL_HEAD      "<FATAL>"
#define TLKAPI_ERROR_HEAD      "<ERROR>"
#define TLKAPI_ARRAY_HEAD      "<ARRAY>"
#define TLKAPI_ASSERT_HEAD     "<ASSERT>"


#define tlkapi_warn(flags,pSign,format,args...)      tlkdbg_warn(flags,pSign,nullptr,0,format,##args)
#define tlkapi_info(flags,pSign,format,args...)      tlkdbg_info(flags,pSign,nullptr,0,format,##args)
#define tlkapi_trace(flags,pSign,format,args...)     tlkdbg_trace(flags,pSign,nullptr,0,format,##args)
#define tlkapi_fatal(flags,pSign,format,args...)     tlkdbg_fatal(flags,pSign,nullptr,0,format,##args)
#define tlkapi_error(flags,pSign,format,args...)     tlkdbg_error(flags,pSign,nullptr,0,format,##args)
#define tlkapi_array(flags,pSign,format,pData,dataLen)      tlkdbg_array(flags,pSign,nullptr,0,format,(uint08*)pData,dataLen)
#define tlkapi_assert(isAssert,flags,pSign,format,args...)  tlkdbg_assert(isAssert,flags,pSign,nullptr,0,format,##args)

#define tlkapi_ext_warn(flags,pSign,format,args...)      tlkdbg_warn(flags,pSign,__FILE__,__LINE__,format,##args)
#define tlkapi_ext_info(flags,pSign,format,args...)      tlkdbg_info(flags,pSign,__FILE__,__LINE__,format,##args)
#define tlkapi_ext_trace(flags,pSign,format,args...)     tlkdbg_trace(flags,pSign,__FILE__,__LINE__,format,##args)
#define tlkapi_ext_fatal(flags,pSign,format,args...)     tlkdbg_fatal(flags,pSign,__FILE__,__LINE__,format,##args)
#define tlkapi_ext_error(flags,pSign,format,args...)     tlkdbg_error(flags,pSign,__FILE__,__LINE__,format,##args)
#define tlkapi_ext_array(flags,pSign,format,pData,dataLen)      tlkdbg_array(flags,pSign,__FILE__,__LINE__,format,(uint08*)pData,dataLen)
#define tlkapi_ext_assert(isAssert,flags,pSign,format,args...)  tlkdbg_assert(isAssert,flags,pSign,__FILE__,__LINE__,format,##args)


#define tlkapi_sprintf   tlkdbg_sprintf

#define tlkapi_sendStr(flags,pStr)                        tlkdbg_sendData(flags,pStr,0,0)
#define tlkapi_sendData(flags,pStr,pData,dataLen)         tlkdbg_sendData(flags,pStr,(uint08*)pData,dataLen)
#define tlkapi_sendU08s(flags,pStr,val0,val1,val2,val3)   tlkdbg_sendU08s(flags,pStr,(uint08)(val0),(uint08)(val1),(uint08)(val2),(uint08)(val3))
#define tlkapi_sendU16s(flags,pStr,val0,val1,val2,val3)   tlkdbg_sendU16s(flags,pStr,(uint16)(val0),(uint16)(val1),(uint16)(val2),(uint16)(val3))
#define tlkapi_sendU32s(flags,pStr,val0,val1,val2,val3)   tlkdbg_sendU32s(flags,pStr,(uint32)(val0),(uint32)(val1),(uint32)(val2),(uint32)(val3))


typedef enum{
	TLKAPI_DEBUG_LEVEL_LEVEL1 = TLKAPI_DBG_ASSERT_FLAG | TLKAPI_DBG_FATAL_FLAG | TLKAPI_DBG_ERROR_FLAG
								| TLKAPI_DBG_WARN_FLAG | TLKAPI_DBG_INFO_FLAG | TLKAPI_DBG_TRACE_FLAG
								| TLKAPI_DBG_ARRAY_FLAG,
	TLKAPI_DEBUG_LEVEL_LEVEL2 = TLKAPI_DBG_ASSERT_FLAG | TLKAPI_DBG_FATAL_FLAG | TLKAPI_DBG_ERROR_FLAG
								| TLKAPI_DBG_WARN_FLAG | TLKAPI_DBG_INFO_FLAG,
	TLKAPI_DEBUG_LEVEL_LEVEL3 = TLKAPI_DBG_ASSERT_FLAG | TLKAPI_DBG_FATAL_FLAG | TLKAPI_DBG_ERROR_FLAG
								| TLKAPI_DBG_WARN_FLAG,
	TLKAPI_DEBUG_LEVEL_LEVEL4 = TLKAPI_DBG_ASSERT_FLAG | TLKAPI_DBG_FATAL_FLAG | TLKAPI_DBG_ERROR_FLAG,
	TLKAPI_DEBUG_LEVEL_LEVEL5 = TLKAPI_DBG_ASSERT_FLAG | TLKAPI_DBG_FATAL_FLAG,
}TLKAPI_DEBUG_LEVEL_ENUM;


extern void tlkdbg_setLevel(TLKAPI_DEBUG_LEVEL_ENUM level);

extern void tlkdbg_warn(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
extern void tlkdbg_info(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
extern void tlkdbg_trace(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
extern void tlkdbg_fatal(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
extern void tlkdbg_error(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
extern void tlkdbg_array(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen);
extern void tlkdbg_assert(bool isAssert, uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);


extern int  tlkdbg_sprintf(char *pOut, const char *format, ...);

extern void tlkdbg_sendData(uint32 flags, char *pStr, uint08 *pData, uint16 dataLen);
extern void tlkdbg_sendU08s(uint32 flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3);
extern void tlkdbg_sendU16s(uint32 flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3);
extern void tlkdbg_sendU32s(uint32 flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);

extern void tlkdbg_handler(void);
extern void tlkdbg_delayForPrint(uint32 us);

extern void tlkdbg_vcd_ref(void);
extern void tlkdbg_vcd_sync(void);
extern void tlkdbg_vcd_tick(uint32 flags, uint08 id);
extern void tlkdbg_vcd_level(uint32 flags, uint08 id, uint08 level);
extern void tlkdbg_vcd_event(uint32 flags, uint08 id);
extern void tlkdbg_vcd_byte(uint32 flags, uint08 id, uint08 value);
extern void tlkdbg_vcd_word(uint32 flags, uint08 id, uint16 value);



#endif //TLKAPI_DEBUG_H

