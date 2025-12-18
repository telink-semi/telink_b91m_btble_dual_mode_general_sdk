/********************************************************************************************************
 * @file	tlkalg_sbcEnc.h
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
#ifndef TLKALG_SBC_ENC_H
#define TLKALG_SBC_ENC_H



#define TLKALG_SBC_SAMPLERATE_16000         0
#define TLKALG_SBC_SAMPLERATE_32000         1
#define TLKALG_SBC_SAMPLERATE_44100         2
#define TLKALG_SBC_SAMPLERATE_48000         3




int  tlkalg_sbc_encInit(void *p);
void tlkalg_sbc_encSetType(uint08 sr_type);
void tlkalg_sbc_encSetBitpool(uint08 blocks, uint08 bitpool);
int  tlkalg_sbc_encData(const uint08* buf, uint16 len, uint08 *outbuf, uint32 outbuf_len, uint32* out_len);
int  tlkalg_msbc_encData(unsigned char *ps, int len, unsigned char *pd);
int  tlkalg_sbc_encOrg(unsigned char *ps, int len, unsigned char *pd);
int  tlkalg_sbc_encJoint(unsigned char *ps, int len, unsigned char *pd);
void tlkalg_sbc_encUpdateParam(void *p);



#endif //TLKALG_SBC_ENC_H
