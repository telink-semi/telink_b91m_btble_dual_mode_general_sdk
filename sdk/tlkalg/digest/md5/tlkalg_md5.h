/********************************************************************************************************
 * @file	tlkalg_md5.h
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
#ifndef TLKALG_MD5_H
#define TLKALG_MD5_H



#define TLKALG_MD5_HASH_SIZE     (128/8)


typedef struct{
	uint32 lo;
    uint32 hi;
    uint32 a;
    uint32 b;
    uint32 c;
    uint32 d;
    uint08 buff[64];
    uint32 block[16];
}tlkalg_md5_context_t;
typedef struct{
	uint08 value[TLKALG_MD5_HASH_SIZE];
}tlkalg_md5_digest_t;


void tlkalg_md5_init(tlkalg_md5_context_t *pContext);
void tlkalg_md5_update(tlkalg_md5_context_t *pContext, uint08 *pData, uint32 dataLen);
void tlkalg_md5_finish(tlkalg_md5_context_t *pContext, tlkalg_md5_digest_t *pDigest);
void tlkalg_md5_result(uint08 *pData, uint16 dataLen, tlkalg_md5_digest_t *pDigest);



#endif //TLKALG_MD5_H

