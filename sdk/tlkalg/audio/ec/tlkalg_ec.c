/********************************************************************************************************
 * @file	tlkalg_ec.c
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

#if (TLK_ALG_EC_ENABLE)

#include "tlkalg_ec.h"


#include "tlkalg/audio/aec/tlkalg_aec_ns.h"
#include "tlkalg/audio/agc/tlkalg_agc.h"


static SpeexPreprocessState *sTlkAlgEcNsCtrl = nullptr;
static SpeexEchoState       *sTlkAlgEcAecCtrl = nullptr;
static void				    *sTlkAlgEcScratch = nullptr;

static short sTlkAlgEcOutBuffer[256];



void tlkalg_ec_init(uint08 *pNs, uint08 *pAec ,uint08 *pScratch)
{
	sTlkAlgEcNsCtrl  = (SpeexPreprocessState*)pNs;
	sTlkAlgEcAecCtrl = (SpeexEchoState*)pAec;
	sTlkAlgEcScratch = (void*)pScratch;

	#if TLK_ALG_AEC_ENABLE
	if(sTlkAlgEcAecCtrl != nullptr){
		AEC_CFG_PARAS aecParas;
		aecParas.use_pre_emp = 1;    					/* 1: enable pre-emphasis filter, 0: disable pre-emphasis filter */
		aecParas.use_dc_notch = 0;     					/* 1: enable DC removal filter, 0: disable DC removal filter */
		aecParas.sampling_rate = 16000;    				/* sample rate */
		aecParas.frame_size = 120;
		aecParas.filter_length = 120;
		tlka_aec_init(sTlkAlgEcAecCtrl, &aecParas, (void *)sTlkAlgEcScratch);
	}
	#endif
	#if TLK_ALG_EC_ENABLE
	if(sTlkAlgEcNsCtrl != nullptr){
		NS_CFG_PARAS nsParas;
		nsParas.low_shelf_enable = 1,					//low shelf enable
		nsParas.post_gain_enable = 1,					//post gain enable
		nsParas.hf_cutting_enable = 0,					//high filter enable
		nsParas.noise_suppress_default = -15,			//noise suppress
		nsParas.echo_suppress_default = -55,			//echo suppress
		nsParas.echo_suppress_active_default = -45,	//eche suppress active
		nsParas.ns_smoothness = 27853,      			//NS smoothness  QCONST16(0.85f,15)
		nsParas.ns_threshold_low = 0.0f,				//NS threshold
		nsParas.frame_size = 120;
		nsParas.sampling_rate = 16000;
		tlka_ns_init((void *)sTlkAlgEcNsCtrl, (void *)&nsParas, (void *)sTlkAlgEcScratch);
		if(sTlkAlgEcAecCtrl != nullptr){
			tlka_ns_set_parameter(sTlkAlgEcNsCtrl, SPEEX_PREPROCESS_SET_ECHO_STATE, sTlkAlgEcAecCtrl);
		}
	}
	#endif
}

short *tlkalg_ec_frame(uint08 *pMicData, uint08 *pSpkData)
{
#if TLK_ALG_AEC_ENABLE
	if(sTlkAlgEcAecCtrl != nullptr){
		tlka_aec_process_frame(sTlkAlgEcAecCtrl, (const spx_int16_t*)pMicData, (const spx_int16_t*)pSpkData, (spx_int16_t*)sTlkAlgEcOutBuffer);
	}
#else
	uint16 *mic_tp = sTlkAlgEcOutBuffer;
	uint16 *mic_out_tp = (unsigned short *)pMicData;
	for(int i=0;i<120;i++){
		*mic_tp++ = *mic_out_tp++;
	}
#endif
	
	if(sTlkAlgEcNsCtrl != nullptr){
		tlka_ns_process_frame(sTlkAlgEcNsCtrl, sTlkAlgEcOutBuffer);
	}
	
	return sTlkAlgEcOutBuffer;
}


#endif //#if (TLK_ALG_EC_ENABLE)

