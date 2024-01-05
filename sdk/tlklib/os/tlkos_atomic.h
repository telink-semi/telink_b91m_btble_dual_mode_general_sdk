/********************************************************************************************************
 * @file	tlkos_atomic.h
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
#ifndef TLKOS_ATOMIC_H
#define TLKOS_ATOMIC_H

#if (TLK_CFG_OS_ENABLE)



typedef struct{
	volatile uint val;
}tlkos_atomic_t;


void tlkos_atomic_set(tlkos_atomic_t *pAtomic, uint val);
uint tlkos_atomic_read(volatile tlkos_atomic_t *pAtomic);

void tlkos_atomic_add(uint step, volatile tlkos_atomic_t *pAtomic);
void tlkos_atomic_sub(uint step, volatile tlkos_atomic_t *pAtomic);

uint tlkos_atomic_addAndReturn(uint step, volatile tlkos_atomic_t *pAtomic);
uint tlkos_atomic_subAndReturn(uint step, volatile tlkos_atomic_t *pAtomic);



#endif //#if (TLK_CFG_OS_ENABLE)

#endif //TLKOS_ATOMIC_H

