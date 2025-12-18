/********************************************************************************************************
 * @file	tlkos_atomic.c
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
#include "tlklib/os/tlkos_atomic.h"
#include "tlklib/os/tlkos_kernel.h"



void tlkos_atomic_set(tlkos_atomic_t *at, uint val)
{
	tlkos_disable_interrupt();
	at->val = val;
	tlkos_restore_interrupt();
}
uint tlkos_atomic_read(volatile tlkos_atomic_t *pAtomic)
{
	uint val;
	tlkos_disable_interrupt();
	val = pAtomic->val;
	tlkos_restore_interrupt();
	return val;
}

void tlkos_atomic_add(uint step, volatile tlkos_atomic_t *pAtomic)
{
	tlkos_disable_interrupt();
	pAtomic->val += step;
	tlkos_restore_interrupt();
}
void tlkos_atomic_sub(uint step, volatile tlkos_atomic_t *pAtomic)
{
	tlkos_disable_interrupt();
	pAtomic->val -= step;
	tlkos_restore_interrupt();
}

uint tlkos_atomic_addAndReturn(uint step, volatile tlkos_atomic_t *pAtomic)
{
	int temp;
	tlkos_disable_interrupt();
	temp = pAtomic->val;
	temp += step;
	pAtomic->val = temp;
	tlkos_restore_interrupt();
	return temp;
}
uint tlkos_atomic_subAndReturn(uint step, volatile tlkos_atomic_t *pAtomic)
{
	uint temp;
	tlkos_disable_interrupt();
	temp = pAtomic->val;
	temp -= step;
	pAtomic->val = temp;
	tlkos_restore_interrupt();
	return temp;
}



#endif //#if (TLK_CFG_OS_ENABLE)

