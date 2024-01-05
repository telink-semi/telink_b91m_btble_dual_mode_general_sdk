/********************************************************************************************************
 * @file	tlkalg_resample.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkalg/audio/resample/tlkalg_resample.h"


extern void my_resample44to16_init(tlkalg_resampleCtrl_t *pCtrl);
extern uint my_resample44to16_data(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

extern void my_resample16to44_init(tlkalg_resampleCtrl_t *pCtrl);
extern uint my_resample16to44_data(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

extern void my_resample44to48_init(tlkalg_resampleCtrl_t *pCtrl);
extern uint my_resample44to48_data(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

extern void my_resample48to44_init(tlkalg_resampleCtrl_t *pCtrl);
extern uint my_resample48to44_data(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

extern void my_resample48to16_init(tlkalg_resampleCtrl_t *pCtrl);
extern uint my_resample48to16_data(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

extern void my_resample16to48_init(tlkalg_resampleCtrl_t *pCtrl);
extern uint my_resample16to48_data(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);


void tlkalg_resample_44to16Init(tlkalg_resampleCtrl_t *pCtrl)
{
    my_resample44to16_init(pCtrl);
}
uint tlkalg_resample_44to16Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo)
{
    return my_resample44to16_data(pCtrl, pSrc, srcLen, pDst, isStereo);
}

void tlkalg_resample_16to44Init(tlkalg_resampleCtrl_t *pCtrl)
{
	my_resample16to44_init(pCtrl);
}
uint tlkalg_resample_16to44Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo)
{
    return my_resample16to44_data(pCtrl, pSrc, srcLen, pDst, isStereo);
}

void tlkalg_resample_44to48Init(tlkalg_resampleCtrl_t *pCtrl)
{
	my_resample44to48_init(pCtrl);
}
uint tlkalg_resample_44to48Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo)
{
    return my_resample44to48_data(pCtrl, pSrc, srcLen, pDst, isStereo);
}

void tlkalg_resample_48to44Init(tlkalg_resampleCtrl_t *pCtrl)
{
	my_resample48to44_init(pCtrl);
}
uint tlkalg_resample_48to44Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo)
{
    return my_resample48to44_data(pCtrl, pSrc, srcLen, pDst, isStereo);
}

void tlkalg_resample_16to48Init(tlkalg_resampleCtrl_t *pCtrl)
{
	my_resample16to48_init(pCtrl);
}
uint tlkalg_resample_16to48Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo)
{
    return my_resample16to48_data(pCtrl, pSrc, srcLen, pDst, isStereo);
}

void tlkalg_resample_48to16Init(tlkalg_resampleCtrl_t *pCtrl)
{
	my_resample48to16_init(pCtrl);
}
uint tlkalg_resample_48to16Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo)
{
    return my_resample48to16_data(pCtrl, pSrc, srcLen, pDst, isStereo);
}

