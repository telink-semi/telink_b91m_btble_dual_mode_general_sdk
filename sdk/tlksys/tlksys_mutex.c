/********************************************************************************************************
 * @file	tlksys_mutex.c
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
#if (TLK_CFG_SYS_ENABLE)
#if (TLK_CFG_OS_ENABLE)
#include "tlkapi/tlkapi_os.h"
#endif
#include "tlksys_stdio.h"
#include "tlksys_mutex.h"
#if (TLK_CFG_OS_ENABLE)
#include "tlkapi/tlkapi_os.h"
#endif


#if (TLK_CFG_OS_ENABLE)
typedef struct{
	tlkos_mutex_t mutex;
}tlktsk_mutex_t;
static tlktsk_mutex_t sTlkSysMutexList[TLKSYS_PROC_MAX_NUMB] = {0};
#endif


int tlksys_mutex_init(void)
{
#if (TLK_CFG_OS_ENABLE)
	uint08 index;
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		if(tlkos_mutex_create(&sTlkSysMutexList[index].mutex) != TLK_ENONE) break;
	}
	if(index != TLKSYS_PROC_MAX_NUMB) return -TLK_EFAIL;
	return TLK_ENONE;
#else
	return TLK_ENONE;
#endif
}
void tlksys_mutex_deinit(void)
{
#if (TLK_CFG_OS_ENABLE)
	uint08 index;
	for(index=0; index<TLKSYS_PROC_MAX_NUMB; index++){
		tlkos_mutex_destroy(&sTlkSysMutexList[index].mutex);
	}
#endif
}

int tlksys_mutex_lock(uint08 procID)
{
#if (TLK_CFG_OS_ENABLE)
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	return tlkos_mutex_lock(&sTlkSysMutexList[procID-1].mutex);
#else
	return TLK_ENONE;
#endif
}
int tlksys_mutex_unlock(uint08 procID)
{
#if (TLK_CFG_OS_ENABLE)
	if(procID == 0 || procID > TLKSYS_PROC_MAX_NUMB) return -TLK_EPARAM;
	return tlkos_mutex_unlock(&sTlkSysMutexList[procID-1].mutex);
#else
	return TLK_ENONE;
#endif
}


#endif //#if (TLK_CFG_SYS_ENABLE)

