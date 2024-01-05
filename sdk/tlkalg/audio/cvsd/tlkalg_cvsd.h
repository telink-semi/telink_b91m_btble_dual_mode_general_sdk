/********************************************************************************************************
 * @file	tlkalg_cvsd.h
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
#ifndef TLKALG_CVSD_H
#define TLKALG_CVSD_H



#define TLKALG_CVSD_FRAME_SIZE     120
#define TLKALG_CVSD_CACHE_SIZE     (2*6*TLKALG_CVSD_FRAME_SIZE) //1440
#define TLKALG_CVSD_PARAM_SIZE     224



int  tlkalg_cvsd_init(uint08 *pParamBuff, uint08 *pCacheBuff, uint16 paramLen, uint16 cacheLen);
void tlkalg_cvsd_updateParam(uint08 *pParamBuff, uint08 *pCacheBuff, uint16 paramLen, uint16 cacheLen);
bool tlkalg_cvsd_enc(short *pcm2, int samples, unsigned char* bs);
bool tlkalg_cvsd_dec(unsigned char *bs, int bytes, short *pcm2);
bool tlkalg_cvsd_decMute(short *pcm, int samples);




#endif //TLKALG_CVSD_H

