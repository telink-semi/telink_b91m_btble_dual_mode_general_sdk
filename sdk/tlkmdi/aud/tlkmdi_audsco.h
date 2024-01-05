/********************************************************************************************************
 * @file	tlkmdi_audsco.h
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
#ifndef TLKMDI_SCO_H
#define TLKMDI_SCO_H

#if (TLK_MDI_AUDSCO_ENABLE)


#define TLKMDI_SCO_SPK_ENC_BUFF_NUMB        16
#define TLKMDI_SCO_SPK_ENC_BUFF_SIZE        64

#define TLKMDI_SCO_MIC_ENC_BUFF_NUMB        16
#define TLKMDI_SCO_MIC_ENC_BUFF_SIZE        64

#define TLKMDI_SCO_SPK_PCM_BUFF_SIZE        240
#define TLKMDI_SCO_MIC_PCM_BUFF_SIZE        240

//For speex_ns_state, agc_st. This is just for call.
#define TLKMDI_SCO_SPEEX_NS_SIZE            5888//9216//((ns_get_size()+16)/16 + 1024)
#define TLKMDI_SCO_SPEEX_NS_OFFS            0
#define TLKMDI_SCO_AGC_ST_SIZE              1024
#define TLKMDI_SCO_AGC_ST_OFFS              (TLKMDI_SCO_SPEEX_NS_OFFS+TLKMDI_SCO_SPEEX_NS_SIZE)
#define TLKMDI_SCO_SPEEX_BUFFER_SIZE        (TLKMDI_SCO_SPEEX_NS_SIZE+TLKMDI_SCO_AGC_ST_SIZE)

//For aecm_st, aecm_kiss_fft_st_buff, aecm_kiss_ifft_st_buff, aecm_kiss_fftr_st_buff, aecm_kiss_ifftr_st_buff
#define TLKMDI_SCO_AEC_ST_SIZE              6912//((aec_get_size()+16)/16)
#define TLKMDI_SCO_AEC_ST_OFFS              0
#define TLKMDI_SCO_AEC_BUFFER_SIZE          (TLKMDI_SCO_AEC_ST_SIZE)

#define TLKMDI_SCO_AEC_NS_SCRATCH_SIZE  		4096 //(aec_get_scratch_size() > ns_get_scratch_size()? aec_get_scratch_size() : ns_get_scratch_size())
#define TLKMDI_SCO_AEC_NS_SCRATCH_OFFS      	0
#define TLKMDI_SCO_AEC_NS_SCRATCH_BUFFER_SIZE	(TLKMDI_SCO_AEC_NS_SCRATCH_SIZE)

#define TLKMDI_SCO_CVSD_PARAM_SIZE          TLKALG_CVSD_PARAM_SIZE
#define TLKMDI_SCO_CVSD_CACHE_SIZE          TLKALG_CVSD_CACHE_SIZE
#define TLKMDI_SCO_CVSD_BUFFER_SIZE         (TLKMDI_SCO_CVSD_PARAM_SIZE+TLKMDI_SCO_CVSD_CACHE_SIZE)

#define TLKMDI_SCO_SBC_ENC_BUFF_SIZE        3368
#define TLKMDI_SCO_SBC_ENC_BUFF_OFFS        0
#define TLKMDI_SCO_SBC_DEC_BUFF_SIZE        1248
#define TLKMDI_SCO_SBC_DEC_BUFF_OFFS        TLKMDI_SCO_SBC_ENC_BUFF_SIZE
#define TLKMDI_SCO_SBC_BUFFER_SIZE          (TLKMDI_SCO_SBC_ENC_BUFF_SIZE+TLKMDI_SCO_SBC_DEC_BUFF_SIZE)

#if (TLK_ALG_AAC_ENABLE)
#define TLKMDI_SCO_TEMP_BUFFER_SIZE         2048
#else
#define TLKMDI_SCO_TEMP_BUFFER_SIZE         256
#endif
#define TLKMDI_SCO_ENCODE_BUFFER_SIZE       (TLKMDI_SCO_SBC_BUFFER_SIZE) //max(TLKMDI_SCO_SBC_BUFFER_SIZE,TLKMDI_SCO_CVSD_BUFFER_SIZE)
#define TLKMDI_SCO_CACHE_BUFFER_SIZE        (TLKMDI_SCO_SPK_ENC_BUFF_SIZE*TLKMDI_SCO_SPK_ENC_BUFF_NUMB\
	+TLKMDI_SCO_MIC_ENC_BUFF_SIZE*TLKMDI_SCO_MIC_ENC_BUFF_NUMB)

//Total buffer = NS(6912)+AGC(1024)+AEC(6912)+CVSD(1664)+SBC(3368+1248)+TEMP(2048)+CACHE(2048)
//             = 6912+1024+6912+1664+4616+2048+2048 = 25224+7*16 = 25336


#define TLKMDI_SCO_CODEC_ID_CVSD          1
#define TLKMDI_SCO_CODEC_ID_MSBC          2



typedef int(*TlkMdiScoDecFunc)(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
typedef int(*TlkMdiScoEncFunc)(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
typedef int(*TlkMdiScoPlcFunc)(uint08 *pSrc, uint16 srcLen, uint08 *pOut);

typedef struct{
	uint08 *pTempBuffer;
	uint08 *pAecmBuffer;
	uint08 *pSpeexBuffer;
	uint08 *pScratchBuffer;
	uint08 *pCacheBuffer;
	uint08 *pEncodeBuffer;
}tlkmdi_sco_buff_t;



typedef struct{
	uint08 numb;
	uint08 enable;
	uint08 dropSpkNumb;
	uint08 dropMicNumb;
	uint16 aclHandle;
	uint16 scoHandle;
	uint16 spkBuffLen;
	uint16 micBuffLen;
	uint16 spkReadOffs;
	uint16 micReadOffs;
	uint08 codec;
	uint08 resv;
	TlkMdiScoDecFunc dec_func;
	TlkMdiScoEncFunc enc_func;
	TlkMdiScoPlcFunc plc_func;
}tlkmdi_sco_ctrl_t;


/******************************************************************************
 * Function: tlkmdi_audsco_init
 * Descript: Initial the Sco block, register the Sco data callback. and register 
 *           connect and disconnect or codec callback to listen the sco status. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_audsco_init(void);

/******************************************************************************
 * Function: tlkmdi_audsco_enable
 * Descript:  Enable audsco,open icodec.
 * Params: 
 *        @pData[IN]--Include the aclhandle,scohandle and codec type.
 *        @dataLen[IN]--The dataLen.
 * Return: Whether the audsco is enable.
*******************************************************************************/
int tlkmdi_audsco_enable(uint08 *pData,uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_audsco_disable
 * Descript: Disable the audsco. 
 * Params: None.
 * Return: Whether the audsco is disable.
*******************************************************************************/
int tlkmdi_audsco_disable(void);

/******************************************************************************
 * Function: tlkmdi_audsco_init
 * Descript: verify the sco is enable. 
 * Params: None.
 * Return: true is success,other value is false.
*******************************************************************************/
bool tlkmdi_audsco_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_audsco_intval
 * Descript: Get the sco interval which will determine when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsco_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audsco_switch
 * Descript: Change the sco status. 
 * Params:
 *        @handle[IN]--The handle
 *        @status[IN]--The status.
 * Return: The interval value.
*******************************************************************************/
bool tlkmdi_audsco_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_audsco_irqProc
 * Descript: call the mic handler and spkhandler. 
 * Params: None.
 * Return: The true/false.
*******************************************************************************/
bool tlkmdi_audsco_irqProc(void);



#endif //#if (TLK_MDI_AUDSCO_ENABLE)

#endif //TLKMDI_SCO_H

