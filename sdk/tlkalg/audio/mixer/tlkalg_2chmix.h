/********************************************************************************************************
 * @file	tlkalg_2chmix.h
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
#ifndef TLKALG_2CHMIX_H
#define TLKALG_2CHMIX_H


#define TLKMDI_PLAY_2CH_LEFT      (16384)  //round(sqrt(1/2)*pow2(15))
#define TLKMDI_PLAY_2CH_RIGHT     (16384)  //round(sqrt(1/2)*pow2(15))


void tlkalg_2chnmix(short* pLeft, short* pRight, short* pOut, int stride, int length);




#endif //TLKALG_2CHMIX_H

