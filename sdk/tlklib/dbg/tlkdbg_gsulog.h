/********************************************************************************************************
 * @file	tlkdbg_gsulog.h
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
#ifndef TLKDBG_GSU_LOG_H
#define TLKDBG_GSU_LOG_H

#if (TLKDBG_CFG_GSU_LOG_ENABLE)


//GSU - GPIO simulate UART

#define TLKDBG_GSU_LOG_GPIO_PIN        GPIO_PA2
#define TLKDBG_GSU_LOG_BAUD_RATE       1000000

#define TLKDBG_GSU_LOG_CACHE_SIZE        256
#define TLKDBG_GSU_LOG_BUFFER_SIZE       2048
#define TLKDBG_GSU_LOG_IRQ_CACHE_SIZE    128


void tlkdbg_gsulog_init(void);;
void tlkdbg_gsulog_reset(void);
bool tlkdbg_gsulog_isBusy(void);
void tlkdbg_gsulog_handler(void);

void tlkdbg_gsulog_print(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, va_list args);
void tlkdbg_gsulog_array(char *pSign, char *pHead, char *fileName, uint lineNumb, const char *format, uint08 *pData, uint16 dataLen);


void tlkdbg_gsulog_sendU08s(char *pSign, void *pStr, uint08 val0, uint08 val1, uint08 val2, uint08 val3);
void tlkdbg_gsulog_sendU16s(char *pSign, void *pStr, uint16 val0, uint16 val1, uint16 val2, uint16 val3);
void tlkdbg_gsulog_sendU32s(char *pSign, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);
void tlkdbg_gsulog_sendData(char *pSign, char *pStr, uint08 *pData, uint16 dataLen);

void tlkdbg_gsulog_sendStatus(uint08 status, uint08 buffNumb, uint08 *pData, uint16 dataLen);



#endif //#if (TLKDBG_CFG_GSU_LOG_ENABLE)

#endif //TLKDBG_GSU_LOG_H

