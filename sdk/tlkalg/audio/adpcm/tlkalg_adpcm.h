/********************************************************************************************************
 * @file	tlkalg_adpcm.h
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
#ifndef TLKALG_ADPCM_H
#define TLKALG_ADPCM_H





void tlkalg_adpcm_init(int pre, int idx);

void tlkalg_adpcm_pcmToAdpcm(sint16 *pInput, int len, sint16 *pOutput);
int  tlkalg_adpcm_adpcmToPcm(uint08 *pInput, sint16 *pOutput, int outlen);




#endif //TLKALG_ADPCM_H

