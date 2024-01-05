/********************************************************************************************************
 * @file	tlkmdi_audMemory.c
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
#if (TLK_MDI_AUDIO_ENABLE)
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"
#include "tlklib/mem/tlkmem1.h"

#define TLKMDI_AUDMEM_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDMEM_DBG_SIGN       "[MDI]"

volatile static tlkmem_mem1_t sMdiAudMemGlobal = 0;
static uint08 sTlkMdiAudMemBuffer[TLKMDI_AUDMEM_TOTAL_SIZE] = {0};

int tlkmdi_audmem_init(void)
{
	sMdiAudMemGlobal = tlkmem1_init(true, false, sTlkMdiAudMemBuffer, TLKMDI_AUDMEM_TOTAL_SIZE);
	if(sMdiAudMemGlobal == 0) return -TLK_EPARAM;
	return TLK_ENONE;
}

void *tlkmdi_audmem_malloc(uint32 size)
{
	return tlkmem1_malloc(sMdiAudMemGlobal, size);
}

void *tlkmdi_audmem_calloc(uint32 size)
{
	return tlkmem1_calloc(sMdiAudMemGlobal, size);
}

void *tlkmdi_audmem_realloc(void *ptr, uint32 size)
{
	return tlkmem1_realloc(sMdiAudMemGlobal, ptr, size);
}

void tlkmdi_audmem_free(void *ptr)
{
	tlkmem1_free(sMdiAudMemGlobal, ptr);
}

void tlkmdi_audmem_print(void)
{
	tlkmem1_print(sMdiAudMemGlobal);
}


#endif // #if (TLK_MDI_AUDIO_ENABLE)

