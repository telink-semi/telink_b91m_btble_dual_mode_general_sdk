/********************************************************************************************************
 * @file	tlkstk_mem.c
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_CFG_STK_ENABLE)
#include "tlkstk/tlkstk_stdio.h"
#include "tlklib/mem/tlkmem1.h"
#include "tlkstk/tlkstk_mem.h"


#define TLKSTK_MEM_DBG_FLAG       ((TLK_MAJOR_DBGID_BTH << 24) | (TLK_MINOR_DBGID_BTH_ACL << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKSTK_MEM_DBG_SIGN       "[STK]"

volatile static tlkmem_mem1_t sStkMemGlobal = 0;
static uint08 sStkBtMemBuffer[TLKSTK_MEM_TOTAL_SIZE] = {0};


int tlkstk_mem_init(void)
{
	sStkMemGlobal = tlkmem1_init(true, false, sStkBtMemBuffer, TLKSTK_MEM_TOTAL_SIZE);
	if(sStkMemGlobal == 0) return -TLK_EPARAM;
	return TLK_ENONE;
}

void *tlkstk_malloc(uint32 size)
{
	return tlkmem1_malloc(sStkMemGlobal, size);
}

void *tlkstk_calloc(uint32 size)
{
	return tlkmem1_calloc(sStkMemGlobal, size);
}

void *tlkstk_realloc(void *ptr, uint32 size)
{
	return tlkmem1_realloc(sStkMemGlobal, ptr, size);
}

void tlkstk_free(void *ptr)
{
	tlkmem1_free(sStkMemGlobal, ptr);
}

void tlkstk_print(void)
{
	tlkmem1_print(sStkMemGlobal);
}


#endif // #if (TLK_CFG_STK_ENABLE)

