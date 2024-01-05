/********************************************************************************************************
 * @file	tlkalg_sbcDec.h
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
#ifndef TLKALG_SBC_DEC_H
#define TLKALG_SBC_DEC_H



typedef	struct
{
	sint16 proto_4_40[40];
	sint16 proto_8_80[80];
	sint16 costab_4[32];
	sint16 costab_8[128];
	sint08 loudness_4[4][4];
	sint08 loudness_8[4][8];
	sint16 V[2][160];
}tlkalg_sbc_decParam_t;

int tlkalg_sbc_decInit(void *p);

//uint32 tlkalg_sbc_decData(const uint08* buf, uint32 len, uint08 *outbuf, uint32 outbuf_len, uint32* out_len, int msbc);
uint32 tlkalg_sbc_decData(const uint08* buf, uint32 len, uint08 *outbuf, uint32 outbuf_len, uint32* out_len, int msbc,uint08 sbc_out_chn_mask);

int tlkalg_sbc_dec_channel0(unsigned char *ps, int len, unsigned char *pd);
int tlkalg_sbc_dec_channel1(unsigned char *ps, int len, unsigned char *pd);
int tlkalg_sbc_dec_stereo(unsigned char *ps, int len, unsigned char *pd);


int tlkalg_msbc_decData(unsigned char *ps, int len, unsigned char *pd);

void tlkalg_sbc_decUpdateParam(void *p);



#endif //TLKALG_SBC_DEC_H

