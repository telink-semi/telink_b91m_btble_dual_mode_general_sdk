/********************************************************************************************************
 * @file	tlksys_mutex.h
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
#ifndef TLKSYS_MUTEX_H
#define TLKSYS_MUTEX_H

#if (TLK_CFG_SYS_ENABLE)



int  tlksys_mutex_init(void);
void tlksys_mutex_deinit(void);

int  tlksys_mutex_lock(uint08 procID);
int  tlksys_mutex_unlock(uint08 procID);



#endif //#if (TLK_CFG_SYS_ENABLE)

#endif //TLKSYS_MUTEX_H

