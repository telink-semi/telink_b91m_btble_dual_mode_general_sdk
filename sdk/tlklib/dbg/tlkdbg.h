/********************************************************************************************************
 * @file	tlkdbg.h
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
#ifndef TLKDBG_H
#define TLKDBG_H

#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)



void tlkdbg_init(void);
void tlkdbg_reset(void);

bool tlkdbg_isBusy(void);
void tlkdbg_handler(void);

void tlkdbg_setLevel(TLKAPI_DEBUG_LEVEL_ENUM level);

void tlkdbg_warn(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
void tlkdbg_info(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
void tlkdbg_trace(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
void tlkdbg_fatal(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
void tlkdbg_error(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);
void tlkdbg_array(uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen);
void tlkdbg_assert(bool isAssert, uint32 flags, char *pSign, char *fileName, uint lineNumb, const char *format, ...);


void tlkdbg_sendData(uint32 flags, char *pStr, uint08 *pData, uint16 dataLen);
void tlkdbg_sendU08s(uint32 flags, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3);
void tlkdbg_sendU16s(uint32 flags, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3);
void tlkdbg_sendU32s(uint32 flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);

void tlkdbg_vcd_sync(void);
void tlkdbg_vcd_tick(uint32 flags, uint08 id);
void tlkdbg_vcd_level(uint32 flags, uint08 id, uint08 level);
void tlkdbg_vcd_event(uint32 flags, uint08 id);
void tlkdbg_vcd_byte(uint32 flags, uint08 id, uint08 value);
void tlkdbg_vcd_word(uint32 flags, uint08 id, uint16 value);

void tlkdbg_setPrintBuffer(uint08 *pBuffer, uint16 buffLen);


#endif //#if (TLK_DBG_LOG_ENABLE)

#endif //TLKDBG_H

