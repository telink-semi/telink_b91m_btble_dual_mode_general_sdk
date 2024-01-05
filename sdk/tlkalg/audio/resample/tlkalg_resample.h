/********************************************************************************************************
 * @file	tlkalg_resample.h
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
#ifndef TLKALG_RESAMPLE_H
#define TLKALG_RESAMPLE_H




#define TLKALG_RESAMPLE_FILTER_MAXLEN        (48*8) //
#define TLKALG_RESAMPLE_DATA_MAXLEN          (120)


/* data struct */
typedef struct{
    uint num_rate;
    uint den_rate;
    int  int_advance;
    int  frac_advance;
    float cutoff;
    int  filter_len;
    uint oversample;
    int  initialised;
    int  started;

    int  last_sample;
    uint samp_frac_num;

    short* sinc_table;
    short src_buf_left[TLKALG_RESAMPLE_FILTER_MAXLEN + TLKALG_RESAMPLE_DATA_MAXLEN];
    short src_buf_right[TLKALG_RESAMPLE_FILTER_MAXLEN + TLKALG_RESAMPLE_DATA_MAXLEN];
}tlkalg_resampleCtrl_t;



void tlkalg_resample_44to16Init(tlkalg_resampleCtrl_t *pCtrl);
uint tlkalg_resample_44to16Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

void tlkalg_resample_16to44Init(tlkalg_resampleCtrl_t *pCtrl);
uint tlkalg_resample_16to44Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

void tlkalg_resample_44to48Init(tlkalg_resampleCtrl_t *pCtrl);
uint tlkalg_resample_44to48Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

void tlkalg_resample_48to44Init(tlkalg_resampleCtrl_t *pCtrl);
uint tlkalg_resample_48to44Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

void tlkalg_resample_48to16Init(tlkalg_resampleCtrl_t *pCtrl);
uint tlkalg_resample_48to16Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);

void tlkalg_resample_16to48Init(tlkalg_resampleCtrl_t *pCtrl);
uint tlkalg_resample_16to48Tran(tlkalg_resampleCtrl_t *pCtrl, short *pSrc, uint srcLen, short *pDst, bool isStereo);



#endif //TLKALG_RESAMPLE_H

