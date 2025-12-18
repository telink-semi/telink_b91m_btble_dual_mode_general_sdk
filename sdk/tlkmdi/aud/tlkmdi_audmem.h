/********************************************************************************************************
 * @file	tlkmdi_audmem.h
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
#ifndef TLKMDI_MEMORY_H
#define TLKMDI_MEMORY_H

#if (TLK_MDI_AUDIO_ENABLE)
/* refer to TLKPTI_AUD_OPTYPE_ENUM */
#if (TLK_MDI_AUDSRC_ENABLE)
	#define TLKMDI_AUDMEM_TOTAL_SIZE		(40960)
#elif (TLK_MDI_AUDPLAY_ENABLE) || (TLK_MDI_AUDTONE_ENABLE)
	#define TLKMDI_AUDMEM_TOTAL_SIZE		(36788)
#elif (TLK_MDI_AUDSCO_ENABLE)
	#define TLKMDI_AUDMEM_TOTAL_SIZE		(25336)
#elif (TLK_MDI_AUDSNK_ENABLE)
	#define TLKMDI_AUDMEM_TOTAL_SIZE		(15952)
#elif (TLK_MDI_AUDDSCO_ENABLE)
	#define TLKMDI_AUDMEM_TOTAL_SIZE		(8612)
#else
	#define TLKMDI_AUDMEM_TOTAL_SIZE		(512) //Refer TLKAPI_MEM_MIN_SIZE
#endif
#endif	// #if(TLK_MDI_AUDIO_ENABLE)


int tlkmdi_audmem_init(void);

void *tlkmdi_audmem_malloc(uint32 size);

void *tlkmdi_audmem_calloc(uint32 size);

void *tlkmdi_audmem_realloc(void *ptr, uint32 size);

void tlkmdi_audmem_free(void *ptr);

void tlkmdi_audmem_print(void);



#endif //TLKMDI_MEMORY_H

