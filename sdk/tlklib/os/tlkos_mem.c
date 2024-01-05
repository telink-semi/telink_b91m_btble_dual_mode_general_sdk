/********************************************************************************************************
 * @file	tlkos_sem.c
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
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_mem.h"


#if (TLK_OS_FREERTOS_ENABLE)
#include "stddef.h"
extern void * pvPortMalloc( size_t xSize );
extern void vPortFree( void * pv );
#endif


void *tlkos_malloc(uint32 size)
{
#if (TLK_OS_FREERTOS_ENABLE)
	return pvPortMalloc(size);
#else
	return nullptr;
#endif
}
void *tlkos_calloc(uint32 size)
{
#if (TLK_OS_FREERTOS_ENABLE)
	void *ptr = pvPortMalloc(size);
	if(ptr != nullptr && size != 0) tmemset(ptr, 0, size);
	return ptr;
#else
	return nullptr;
#endif
}
void tlkos_free(void *ptr)
{
#if (TLK_OS_FREERTOS_ENABLE)
	vPortFree(ptr);
#endif
}



#endif //#if (TLK_CFG_OS_ENABLE)

