/********************************************************************************************************
 * @file	tlkmem2.h
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
#ifndef TLKMEM2_H
#define TLKMEM2_H

#if (TLK_OS_FREERTOS_ENABLE)

int tlkmem_init(bool enIrqMask, bool enSecMask, uint08 *pBuffer, uint16 buffLen);

void *tlkmem_malloc(uint32 size);

void *tlkmem_calloc(uint32 size);

void tlkmem_free(void *ptr);

void tlkmem_print(void);

#endif //(TLK_OS_FREERTOS_ENABLE)

#endif //TLKMEM2_H

