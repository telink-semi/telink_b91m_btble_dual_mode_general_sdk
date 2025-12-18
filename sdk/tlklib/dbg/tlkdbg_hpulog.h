/********************************************************************************************************
 * @file	tlkdbg_hpulog.h
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
#ifndef TLKDBG_HPU_LOG_H
#define TLKDBG_HPU_LOG_H

#if (TLKDBG_CFG_HPU_LOG_ENABLE)


//HPU - Hardware Protocol UART

#define TLKDBG_HPU_LOG_CACHE_SIZE        256
#define TLKDBG_HPU_LOG_BUFFER_SIZE       2048
#define TLKDBG_HPU_LOG_IRQ_CACHE_SIZE    128

typedef int(*TlkDbgHpuLogSendCB)(uint08 *pData, uint16 dataLen);


void tlkdbg_hpulog_init(void);
void tlkdbg_hpulog_reset(void);
bool tlkdbg_hpulog_isBusy(void);
void tlkdbg_hpulog_handler(void);

void tlkdbg_hpulog_regSendCB(TlkDbgHpuLogSendCB cb, uint16 maxLen);

void tlkdbg_hpulog_print(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args);
void tlkdbg_hpulog_array(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen);

void tlkdbg_hpulog_sendU08s(char *pSign, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3);
void tlkdbg_hpulog_sendU16s(char *pSign, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3);
void tlkdbg_hpulog_sendU32s(char *pSign, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);
void tlkdbg_hpulog_sendData(char *pSign, char *pStr, uint08 *pData, uint16 dataLen);



#endif //#if (TLKDBG_CFG_HPU_LOG_ENABLE)

#endif //TLKDBG_HPU_LOG_H

