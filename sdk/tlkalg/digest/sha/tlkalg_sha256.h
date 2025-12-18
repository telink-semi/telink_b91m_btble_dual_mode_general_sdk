/********************************************************************************************************
 * @file	tlkalg_sha256.h
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
#ifndef TLKALG_SHA256_H
#define TLKALG_SHA256_H


#define TLKALG_SHA256_BLOCK_SIZE       64
#define TLKALG_SHA256_DIGEST_SIZE      (256/8)


typedef struct{
	uint64 length;
	uint32 curlen;
    uint32 state[8];
    uint08 buff[TLKALG_SHA256_BLOCK_SIZE];
}tlkalg_sha256_context_t;
typedef struct{
	uint08 value[TLKALG_SHA256_DIGEST_SIZE];
}tlkalg_sha256_digest_t;


void tlkalg_sha256_init(tlkalg_sha256_context_t *pContext);
void tlkalg_sha256_update(tlkalg_sha256_context_t *pContext, uint08 *pData, uint16 dataLen);
void tlkalg_sha256_finish(tlkalg_sha256_context_t *pContext, tlkalg_sha256_digest_t *pDigest);

void tlkalg_sha256_result(uint08 *pData, uint16 dataLen, tlkalg_sha256_digest_t *pDigest);



#endif //TLKALG_SHA256_H

