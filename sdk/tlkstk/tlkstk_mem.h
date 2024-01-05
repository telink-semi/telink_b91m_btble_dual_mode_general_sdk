/********************************************************************************************************
 * @file	tlkstk_mem.h
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
#ifndef TLKSTK_MEM_H
#define TLKSTK_MEM_H


#if (TLK_CFG_TEST_ENABLE)
#define TLKSTK_MEM_TOTAL_SIZE		(15*1024)
#else
#define TLKSTK_MEM_TOTAL_SIZE		(13*1024)
#endif

#define TLKSTK_BT_STRUCT_ALIGN_4(sStruct)	((sizeof(sStruct) + 3) & 0xFFFC)


int tlkstk_mem_init(void);

void *tlkstk_malloc(uint32 size);

void *tlkstk_calloc(uint32 size);

void *tlkstk_realloc(void *ptr, uint32 size);

void tlkstk_free(void *ptr);

void tlkstk_print(void);



#endif //TLKSTK_MEM_H

