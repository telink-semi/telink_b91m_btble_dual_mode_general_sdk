/********************************************************************************************************
 * @file	tlkalg_sha1.h
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
#ifndef TLKALG_SHA1_H
#define TLKALG_SHA1_H



#define TLKALG_SHA1_HASH_SIZE      (160/8)


typedef struct{
	uint32 state[5];
	uint32 count[2];
	uint08 buff[64];
}tlkalg_sha1_context_t;
typedef struct{
	uint08 value[TLKALG_SHA1_HASH_SIZE];
}tlkalg_sha1_digest_t;



void tlkalg_sha1_init(tlkalg_sha1_context_t *pContext);
void tlkalg_sha1_update(tlkalg_sha1_context_t *pContext, uint08 *pData, uint32 dataLen);
void tlkalg_sha1_finish(tlkalg_sha1_context_t *pContext, tlkalg_sha1_digest_t *pHash);

void tlkalg_sha1_result(uint08 *pData, uint32 dataLen, tlkalg_sha1_digest_t *pDigest);




#endif //TLKALG_SHA1_H

