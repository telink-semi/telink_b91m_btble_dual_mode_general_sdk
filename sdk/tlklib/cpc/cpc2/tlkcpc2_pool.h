/********************************************************************************************************
 * @file	tlkmdi_atsPool.h
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
#ifndef TLKCPC2_POOL_H
#define TLKCPC2_POOL_H

#if (TLK_CFG_COMM_ENABLE)



int tlkcpc2_pool_init(void);

void tlkcpc2_pool_reset(bool isForce);

int tlkcpc2_pool_getIdleNumb(void);
int tlkcpc2_pool_getSendNumb(void);
int tlkcpc2_pool_getCacheNumb(void);

bool tlkcpc2_pool_isInSendList(uint08 sendNumb);

tlkcpc2_pool_item_t *tlkcpc2_pool_takeIdleItem(void);
tlkcpc2_pool_item_t *tlkcpc2_pool_takeSendItem(void);
tlkcpc2_pool_item_t *tlkcpc2_pool_takeCacheItem(void);

bool tlkcpc2_pool_cacheItem(tlkcpc2_pool_item_t *pItem);
bool tlkcpc2_pool_removeCacheItem(uint08 sendNumb);
bool tlkcpc2_pool_activeCacheItem(uint08 sendNumb);

bool tlkcpc2_pool_backupCopyItem(void);
bool tlkcpc2_pool_activeCopyItem(void);

bool tlkcpc2_pool_pushIdleItem(tlkcpc2_pool_item_t *pItem, bool isFirst);
bool tlkcpc2_pool_pushCopyItem(tlkcpc2_pool_item_t *pItem, bool isFirst);
bool tlkcpc2_pool_pushSendItem(tlkcpc2_pool_item_t *pItem, bool isFirst);
bool tlkcpc2_pool_pushCacheItem(tlkcpc2_pool_item_t *pItem, bool isFirst);

bool tlkcpc2_pool_isHaveItem(uint08 *pList, uint08 numb, uint08 item);
bool tlkcpc2_pool_pushItem(uint08 *pList, uint08 *pNumb, uint08 item, bool isFirst);
tlkcpc2_pool_item_t *tlkcpc2_pool_takeItem(uint08 *pList, uint08 *pNumb);



#endif //#if (TLK_TEST_ATS_ENABLE)

#endif //TLKCPC2_POOL_H

