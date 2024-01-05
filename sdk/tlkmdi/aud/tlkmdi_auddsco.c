/********************************************************************************************************
 * @file     tlkmdi_auddsco.c
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
#if (TLK_MDI_AUDDSCO_ENABLE)
#include "drivers.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkalg/audio/cvsd/tlkalg_cvsd.h"
#include "tlkmdi/aud/tlkmdi_auddsco.h"
#include "tlkmdi/bt/tlkmdi_bthfp.h"
#include "tlkstk/bt/bth/bth_sco.h"

#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkdev/sys/tlkdev_codec.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlksys/prt/tlkpti_phone.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"



#define TLKMDI_AUDDSCO_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_DSCO << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDDSCO_DBG_SIGN       "[DSCO]"

#define TLKMDI_DSCO_PACKET_LOSS_FLAG        1
#define TLKMDI_DSCO_SCO_LOSS_TEST           0
#define TLKMDI_DSCO_PLC_ENABLE1             1


extern void btc_sco_regDataCB(void *prx, void *ptx);
extern int btp_hfp_getCodec(uint16 aclHandle, uint08 *pCodec);

static void tlkmdi_dsco_spkHandler(void);

static int tlkmdi_dsco_msbcEncode(uint08 index, uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_dsco_msbcDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_dsco_cvsdEncode(uint08 index, uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_dsco_cvsdDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);

static int tlkmdi_dsco_getPcmData(uint08 index, sint16 *pBuffer);

static void tlkmdi_dsco_initCodec(uint08 index, bool enable);
static bool tlkmdi_dsco_initBuffer(uint08 index, bool enable);
static bool tlkmdi_dsco_mallocAlgBuffer(uint08 index);
static void tlkmdi_dsco_freeAlgBuffer(uint08 index);

static void tlkmdi_dsco_addSpkEncFrame(uint08 id, uint16 scoHandle, uint08 *p, int len);
static void tlkmdi_dsco_getMicEncFrame(uint08 id, uint16 scoHandle, uint08 *p, int len);


static uint08 tlkmdi_dsco_getScoByHandle(uint16 aclHandle);
static uint08 tlkmdi_dsco_getIdleSco();

const unsigned char sTlkMdiDscoMSBCSilencePkt[60] = {0x00,0x00,
	0xad,0x00,0x00,0xc5,0x00,0x00,0x00,0x00, 0x77,0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,
	0xdb,0x6d,0xdd,0xb6,0xdb,0x77,0x6d,0xb6, 0xdd,0xdb,0x6d,0xb7,0x76,0xdb,0x6d,0xdd,
	0xb6,0xdb,0x77,0x6d,0xb6,0xdd,0xdb,0x6d, 0xb7,0x76,0xdb,0x6d,0xdd,0xb6,0xdb,0x77,
	0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,0xdb, 0x6c,0x00 
};

uint08 sTlkMdiDscoMicMuteIndex = 0;
_attribute_bt_data_retention_ unsigned char sTlkMdiAudDscoByte1[4] = {0x08, 0x38, 0xc8, 0xf8};


static tlkapi_qfifo_t sTlkMdiDscoSpkFifo;
static tlkapi_qfifo_t sTlkMdiDscoMicFifo;
static tlkapi_qfifo_t sTlkMdiDscoSpk01Fifo;
static tlkapi_qfifo_t sTlkMdiDscoMic01Fifo;


static tlkmdi_dsco_ctrl_t sTlkMdiDscoCtrl[TLKMDI_DSCO_MAX_NUMB];
static tlkmdi_dsco_buff_t sTlkMdiDscoBuff[TLKMDI_DSCO_MAX_NUMB];

static uint08 sTlkMdiAudDscoLength = 0;
static uint08 sTlkMdiAudDscoFrame[120];
static uint08 sTlkMdiAudDscoLength1 = 0;
static uint08 sTlkMdiAudDscoFrame1[120];

static tlkalg_sbc_plcState_t gTlkalgSbcPlcState;
static tlkalg_sbc_plcState_t gTlkalgSbcPlcState1;


int tlkmdi_auddsco_init(void)
{
	btc_sco_regDataCB(tlkmdi_dsco_addSpkEncFrame, tlkmdi_dsco_getMicEncFrame);	
	return TLK_ENONE;
}


int tlkmdi_auddsco_enable(uint08 *pData,uint16 dataLen)
{
	if(tlkmdi_auddsco_isBusy()) tlkmdi_auddsco_disable();

	if(dataLen < 10) return -TLK_EPARAM;
	uint16 aclHandle0,aclHandle1;
	uint16 scoHandle0,scoHandle1;
	uint08 codec0,codec1;
	
	aclHandle0 = ((pData[1] << 8) | pData[0]);
	scoHandle0 = ((pData[3] << 8) | pData[2]);
	codec0 = pData[4];
	aclHandle1 = ((pData[6] << 8) | pData[5]);
	scoHandle1 = ((pData[8] << 8) | pData[7]);
	codec1 = pData[9];

	sTlkMdiDscoCtrl[0].aclHandle = aclHandle0;
	sTlkMdiDscoCtrl[0].scoHandle = scoHandle0;
	sTlkMdiDscoCtrl[0].codec = codec0;
	sTlkMdiDscoCtrl[1].aclHandle = aclHandle1;
	sTlkMdiDscoCtrl[1].scoHandle = scoHandle1;
	sTlkMdiDscoCtrl[1].codec = codec1;

	tlkmdi_auddsco_switch(sTlkMdiDscoCtrl[0].aclHandle, TLK_STATE_OPENED);
	//tlkdbg_delayForPrint(1000);
	tlkmdi_auddsco_switch(sTlkMdiDscoCtrl[1].aclHandle, TLK_STATE_OPENED);

	return (sTlkMdiDscoCtrl[0].enable && sTlkMdiDscoCtrl[1].enable);
}
int tlkmdi_auddsco_disable()
{
	tlkmdi_auddsco_switch(sTlkMdiDscoCtrl[0].aclHandle, TLK_STATE_CLOSED);
	tlkmdi_auddsco_switch(sTlkMdiDscoCtrl[1].aclHandle, TLK_STATE_CLOSED);

	sTlkMdiDscoCtrl[0].aclHandle = 0;
	sTlkMdiDscoCtrl[0].scoHandle = 0;
	sTlkMdiDscoCtrl[1].aclHandle = 0;
	sTlkMdiDscoCtrl[1].scoHandle = 0;

	return (!sTlkMdiDscoCtrl[0].enable && !sTlkMdiDscoCtrl[1].enable);
}
bool tlkmdi_auddsco_isBusy(void)
{
	return (sTlkMdiDscoCtrl[0].enable && sTlkMdiDscoCtrl[1].enable);
}

uint tlkmdi_auddsco_intval(void)
{
	return 1000;
}

bool tlkmdi_auddsco_irqProc(void)
{
	if(!sTlkMdiDscoCtrl[0].enable && !sTlkMdiDscoCtrl[1].enable) return false;
	tlkmdi_dsco_spkHandler();
	if(sTlkMdiDscoCtrl[0].enable || sTlkMdiDscoCtrl[1].enable) return true;
	else return false;
}

bool tlkmdi_auddsco_switch(uint16 handle, uint08 status)
{
	bool isSucc;
	bool enable;

	uint08 scoIndex;

	scoIndex = tlkmdi_dsco_getScoByHandle(handle);
	if(scoIndex >= TLKMDI_DSCO_MAX_NUMB) scoIndex = tlkmdi_dsco_getIdleSco();
	
	if(scoIndex >= TLKMDI_DSCO_MAX_NUMB) {
		tlkapi_error(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_audsco_switch - invalid handle:%d %d", handle,scoIndex);
		return false;
	}
	tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_auddsco_switch - handle:%d status:%d scoIndex:%d", handle,status,scoIndex);

	if(status == TLK_STATE_OPENED) {
		btc_sco_regDataCB(tlkmdi_dsco_addSpkEncFrame, tlkmdi_dsco_getMicEncFrame);	
		enable = true;
	}
	else enable = false;

	if(sTlkMdiDscoCtrl[scoIndex].enable == enable) return true;

	if(!enable) sTlkMdiDscoCtrl[scoIndex].enable = false;
	isSucc = tlkmdi_dsco_initBuffer(scoIndex,enable);
	if(!isSucc) enable = false;

	sTlkMdiDscoCtrl[scoIndex].numb = 0;
	sTlkMdiDscoCtrl[scoIndex].enable = enable;
	sTlkMdiDscoCtrl[scoIndex].dropMicNumb = 0;
	sTlkMdiDscoCtrl[scoIndex].spkReadOffs = 0;
	sTlkMdiDscoCtrl[scoIndex].micReadOffs = 0;
	sTlkMdiDscoCtrl[scoIndex].dropSpkNumb = 0;
	sTlkMdiDscoCtrl[scoIndex].spkBuffLen = tlkdev_codec_getSpkBuffLen();
	sTlkMdiDscoCtrl[scoIndex].micBuffLen = tlkdev_codec_getMicBuffLen();

	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_muteSpkBuff();
	#endif

	tlkmdi_dsco_initCodec(scoIndex,enable);

	if(enable){
		tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_auddsco_switch: enable");
		tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_DSCO, handle);
	}else{
		tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_auddsco_switch: disable");
		tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_DSCO, handle);
	}

	return isSucc;
}
static void tlkmdi_dsco_initCodec(uint08 index ,bool enable)
{
	if(!enable || sTlkMdiDscoBuff[index].pEncodeBuffer == nullptr){
		tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initCodec: disable");
		sTlkMdiDscoCtrl[index].dec_func = nullptr;
		sTlkMdiDscoCtrl[index].enc_func = nullptr;
		tlkalg_cvsd_init(nullptr, nullptr, 0, 0);
		tlkalg_sbc_encInit(nullptr);
		tlkalg_sbc_decInit(nullptr);
	}else{
		tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initCodec: enable");
		if(TLKMDI_DSCO_CODEC_ID_CVSD == sTlkMdiDscoCtrl[index].codec){
			tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initCodec: CVSD init");
			tlkalg_cvsd_init(sTlkMdiDscoBuff[index].pEncodeBuffer, sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_CVSD_PARAM_SIZE,
				TLKMDI_DSCO_CVSD_PARAM_SIZE, TLKMDI_DSCO_CVSD_CACHE_SIZE);
			sTlkMdiDscoCtrl[index].dec_func = tlkmdi_dsco_cvsdDecode;
			sTlkMdiDscoCtrl[index].enc_func = tlkmdi_dsco_cvsdEncode;
	    }else{
	    	tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initCodec: enter MSBC");
			tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initCodec: SBC init");
			tlkalg_sbc_encInit(sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_SBC_ENC_BUFF_OFFS);
			tlkalg_sbc_decInit(sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_SBC_DEC_BUFF_OFFS);
			sTlkMdiDscoCtrl[index].dec_func = tlkmdi_dsco_msbcDecode;
			sTlkMdiDscoCtrl[index].enc_func = tlkmdi_dsco_msbcEncode;
	    }
	}
}

static bool tlkmdi_dsco_initBuffer(uint08 index,bool enable)
{
	bool isEnable;

	isEnable = enable;
	if(isEnable && !tlkmdi_dsco_mallocAlgBuffer(index)){
		isEnable = false;
	}
	if(!isEnable){
		tlkmdi_dsco_freeAlgBuffer(index);
	}
	if(!isEnable){
		if(index == 0) {
			tlkapi_qfifo_reset(&sTlkMdiDscoSpkFifo);
			tlkapi_qfifo_reset(&sTlkMdiDscoMicFifo);
		}else if(index == 1) {
			tlkapi_qfifo_reset(&sTlkMdiDscoSpk01Fifo);
			tlkapi_qfifo_reset(&sTlkMdiDscoMic01Fifo);
		}		
	}
	else{
		uint16 i;
		uint16 encBuffLen = TLKMDI_DSCO_SPK_ENC_BUFF_SIZE*TLKMDI_DSCO_SPK_ENC_BUFF_NUMB;
		uint16 decBuffLen = TLKMDI_DSCO_MIC_ENC_BUFF_SIZE*TLKMDI_DSCO_MIC_ENC_BUFF_NUMB;
		uint08 *pEncBuffer = sTlkMdiDscoBuff[index].pCacheBuffer;
		uint08 *pDecBuffer = sTlkMdiDscoBuff[index].pCacheBuffer+encBuffLen;
		
		if(index == 0) {
			tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initBuffer: spk fifo");
			tlkapi_qfifo_init(&sTlkMdiDscoSpkFifo, TLKMDI_DSCO_SPK_ENC_BUFF_NUMB, TLKMDI_DSCO_SPK_ENC_BUFF_SIZE, pEncBuffer, encBuffLen);
			tlkapi_qfifo_init(&sTlkMdiDscoMicFifo, TLKMDI_DSCO_MIC_ENC_BUFF_NUMB, TLKMDI_DSCO_MIC_ENC_BUFF_SIZE, pDecBuffer, decBuffLen);
		}else if(index == 1) {
			tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_initBuffer: spk01 fifo");
			tlkapi_qfifo_init(&sTlkMdiDscoSpk01Fifo, TLKMDI_DSCO_SPK_ENC_BUFF_NUMB, TLKMDI_DSCO_SPK_ENC_BUFF_SIZE, pEncBuffer, encBuffLen);
			tlkapi_qfifo_init(&sTlkMdiDscoMic01Fifo, TLKMDI_DSCO_MIC_ENC_BUFF_NUMB, TLKMDI_DSCO_MIC_ENC_BUFF_SIZE, pDecBuffer, decBuffLen);
		}else {
			return false;
		}
		for(i=0; i<TLKMDI_DSCO_MIC_ENC_BUFF_NUMB; i++){
			tmemcpy(pDecBuffer, sTlkMdiDscoMSBCSilencePkt, 60);
			pDecBuffer[0] = 0x01;
			pDecBuffer[1] = sTlkMdiAudDscoByte1[i & 3];
			pDecBuffer += TLKMDI_DSCO_MIC_ENC_BUFF_SIZE;
		}
	}
	if(isEnable == enable) return true;
	else return false;
}

_attribute_bt_ram_code_
static void tlkmdi_dsco_addSpkEncFrame(uint08 id, uint16 scoHandle, uint08 *pData, int dataLen) // NOTE: This is in IRQ
{
	uint08 *pBuffer;
	if(pData == nullptr || dataLen == 0 || pData[0] != BTH_HCI_TYPE_SCO_DATA) return;
	if( (!sTlkMdiDscoCtrl[0].enable) || (!sTlkMdiDscoCtrl[1].enable) ) return;
	uint16 tempLen;
	uint16 syncHead = pData[1] + (pData[2] << 8);
	if(scoHandle == sTlkMdiDscoCtrl[0].scoHandle) {
		pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiDscoSpkFifo);
			if(pBuffer == nullptr) return;
			pBuffer[0] = 0;
			pBuffer[1] = 0;
			if((syncHead & 0x3000) == BTH_HCI_SYNC_HDR_PSF_GOOD){
				tempLen = pData[3];
				if(tempLen > TLKMDI_DSCO_SPK_ENC_BUFF_SIZE) tempLen = TLKMDI_DSCO_SPK_ENC_BUFF_SIZE;
				if(tempLen != 0) {
					tmemcpy(pBuffer+2, &pData[4], tempLen);
				}
			}else{
				pBuffer[0] = TLKMDI_DSCO_PACKET_LOSS_FLAG;
			}
			tlkapi_qfifo_dropBuff(&sTlkMdiDscoSpkFifo);
	}else if(scoHandle == sTlkMdiDscoCtrl[1].scoHandle) {
		pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiDscoSpk01Fifo);
			if(pBuffer == nullptr) return;
			pBuffer[0] = 0;
			pBuffer[1] = 0;
			if((syncHead & 0x3000) == BTH_HCI_SYNC_HDR_PSF_GOOD){
				tempLen = pData[3];
				if(tempLen > TLKMDI_DSCO_SPK_ENC_BUFF_SIZE) tempLen = TLKMDI_DSCO_SPK_ENC_BUFF_SIZE;
				if(tempLen != 0) {
					tmemcpy(pBuffer+2, &pData[4], tempLen);
				}
			}else{
				pBuffer[0] = TLKMDI_DSCO_PACKET_LOSS_FLAG;
			}
			tlkapi_qfifo_dropBuff(&sTlkMdiDscoSpk01Fifo);
	}else {
			return;
	}
}

_attribute_bt_ram_code_
static void tlkmdi_dsco_getMicEncFrame(uint08 id, uint16 scoHandle, uint08 *pBuff, int buffLen) // NOTE: This is in IRQ
{
	uint08 *pData;

	if(!sTlkMdiDscoCtrl[0].enable || !sTlkMdiDscoCtrl[1].enable) return;
	if(scoHandle == sTlkMdiDscoCtrl[0].scoHandle) {
		pData = tlkapi_qfifo_getData(&sTlkMdiDscoMicFifo);
		if(pData != nullptr){
			if(buffLen > TLKMDI_DSCO_MIC_ENC_BUFF_SIZE){
				buffLen = TLKMDI_DSCO_MIC_ENC_BUFF_SIZE;
			}
			tmemcpy(pBuff, pData, buffLen);
			tlkapi_qfifo_dropData(&sTlkMdiDscoMicFifo);
		}
	}else if(scoHandle == sTlkMdiDscoCtrl[1].scoHandle) {
		pData = tlkapi_qfifo_getData(&sTlkMdiDscoMic01Fifo);
		if(pData != nullptr) {
			if(buffLen > TLKMDI_DSCO_MIC_ENC_BUFF_SIZE) {
				buffLen = TLKMDI_DSCO_MIC_ENC_BUFF_SIZE;
			}
			tmemcpy(pBuff, pData, buffLen);
			tlkapi_qfifo_dropData(&sTlkMdiDscoMic01Fifo);
		}
	}else {
		return;
	}
}

static void tlkmdi_dsco_spkHandler(void)
{
	int count = 8;
	uint08 *pBuffer;

	int np1,np2;

	while(count--) {
		np1 = 0;
		np2 = 0;
		pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiDscoMic01Fifo);
		if(pBuffer != nullptr && sTlkMdiDscoCtrl[1].enc_func != nullptr){
			np1 = tlkmdi_dsco_getPcmData(0,(sint16*)sTlkMdiDscoBuff[0].pTempBuffer);
			if(np1){
				if(sTlkMdiDscoCtrl[1].codec == TLKMDI_DSCO_CODEC_ID_CVSD) {
					tlkalg_cvsd_updateParam(sTlkMdiDscoBuff[1].pEncodeBuffer, sTlkMdiDscoBuff[1].pEncodeBuffer+TLKMDI_DSCO_CVSD_PARAM_SIZE,
											TLKMDI_DSCO_CVSD_PARAM_SIZE, TLKMDI_DSCO_CVSD_CACHE_SIZE);
				}else if(sTlkMdiDscoCtrl[1].codec == TLKMDI_DSCO_CODEC_ID_MSBC){
					tlkalg_sbc_encUpdateParam(sTlkMdiDscoBuff[1].pEncodeBuffer+TLKMDI_DSCO_SBC_ENC_BUFF_OFFS);
				}
				sTlkMdiDscoCtrl[1].enc_func(1,(uint08*)sTlkMdiDscoBuff[0].pTempBuffer,np1,(uint08*)pBuffer);
				tlkapi_qfifo_dropBuff(&sTlkMdiDscoMic01Fifo);
			}
		}
		
		pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiDscoMicFifo);
			if(pBuffer != nullptr && sTlkMdiDscoCtrl[0].enc_func != nullptr){
				np2 = tlkmdi_dsco_getPcmData(1,(sint16*)sTlkMdiDscoBuff[1].pTempBuffer);
				if(np2) {	
					if(sTlkMdiDscoCtrl[0].codec == TLKMDI_DSCO_CODEC_ID_CVSD) {
						tlkalg_cvsd_updateParam(sTlkMdiDscoBuff[0].pEncodeBuffer, sTlkMdiDscoBuff[0].pEncodeBuffer+TLKMDI_DSCO_CVSD_PARAM_SIZE,
												TLKMDI_DSCO_CVSD_PARAM_SIZE, TLKMDI_DSCO_CVSD_CACHE_SIZE);
					}else if(sTlkMdiDscoCtrl[0].codec == TLKMDI_DSCO_CODEC_ID_MSBC) {
						tlkalg_sbc_encUpdateParam(sTlkMdiDscoBuff[0].pEncodeBuffer+TLKMDI_DSCO_SBC_ENC_BUFF_OFFS);
					}
					sTlkMdiDscoCtrl[0].enc_func(0,(uint08*)sTlkMdiDscoBuff[1].pTempBuffer,np2,(uint08*)pBuffer);
					tlkapi_qfifo_dropBuff(&sTlkMdiDscoMicFifo);
				}
			}
			if(np1 == 0 && np2 == 0) break;
	}
}


static void tlkmdi_audsco_makeValidFrame(uint08 scoIndex,uint08 *pData)
{
	uint08 index;
	uint16 length;
	uint08 *pFrame;


	if(scoIndex == 0){
		length = sTlkMdiAudDscoLength;
		pFrame = sTlkMdiAudDscoFrame;
	}else if(scoIndex == 1){
		length = sTlkMdiAudDscoLength1;
		pFrame = sTlkMdiAudDscoFrame1;
	}else {
		length = 0;
		pFrame = nullptr;
	}
	if(pData[0] == TLKMDI_DSCO_PACKET_LOSS_FLAG || TLKMDI_DSCO_CODEC_ID_MSBC != sTlkMdiDscoCtrl[scoIndex].codec){
		if(length != 0) length = 0;
		return;
	}
	if(pData[2] == 0x01 && (pData[3] & 0x0F) == 0x08 && pData[4] == 0xAD){
		if(length != 0) length = 0;
		return;
	}
	tmemcpy(pFrame+length, pData+2, 60);
	length += 60;
	for(index=0; index<length; index++){
		if(pFrame[index] == 1 && (pFrame[index+1] & 0x0F) == 0x08
			&& pFrame[index+2] == 0xAD) break;
	}
	if(index == length){
		tmemcpy(pFrame, pFrame+(length-3), 3);
		length = 3;
		pData[0] = TLKMDI_DSCO_PACKET_LOSS_FLAG;
	}else{
		length = length-index;
		if(length < 60){
			pData[0] = TLKMDI_DSCO_PACKET_LOSS_FLAG;
		}else{
			tmemcpy(pData+2, pFrame+index, 60);
			index += 60;
			length -= 60;
		}
		if(length != 0){
			tmemcpy(pFrame, pFrame+index, length);
		}
	}
	if(scoIndex == 0){
		sTlkMdiAudDscoLength = length;
	}else if(scoIndex == 1){
		sTlkMdiAudDscoLength1 = length;
	}
}
static int tlkmdi_dsco_getPcmData(uint08 index,sint16 *pBuffer)
{
	uint08 *pData = nullptr;
	#if TLKMDI_DSCO_PLC_ENABLE1
	tlkalg_sbc_plcState_t *pState = nullptr;
	#endif
	if(index == 0) {
		pData = tlkapi_qfifo_getData(&sTlkMdiDscoSpkFifo);
		#if TLKMDI_DSCO_PLC_ENABLE1
		pState = &gTlkalgSbcPlcState;
		#endif
	}else if(index == 1) {
		pData = tlkapi_qfifo_getData(&sTlkMdiDscoSpk01Fifo);
		#if TLKMDI_DSCO_PLC_ENABLE1
		pState = &gTlkalgSbcPlcState1;
		#endif
	}

	if(pData == nullptr || sTlkMdiDscoCtrl[index].dec_func == nullptr) return 0;

	tlkmdi_audsco_makeValidFrame(index,pData);
	if(pData[0] != TLKMDI_DSCO_PACKET_LOSS_FLAG){
		if(sTlkMdiDscoCtrl[index].codec == TLKMDI_DSCO_CODEC_ID_CVSD) {
			tlkalg_cvsd_updateParam(sTlkMdiDscoBuff[index].pEncodeBuffer, sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_CVSD_PARAM_SIZE,
			TLKMDI_DSCO_CVSD_PARAM_SIZE, TLKMDI_DSCO_CVSD_CACHE_SIZE);
		}else{
			tlkalg_sbc_decUpdateParam(sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_SBC_DEC_BUFF_OFFS);
		}
		sTlkMdiDscoCtrl[index].dec_func(pData, 60, (uint08*)pBuffer);
		#if TLKMDI_DSCO_PLC_ENABLE1
		tlkalg_sbc_plcGoodFrame(pState, pBuffer, pBuffer);
		#endif
	}else{
		#if TLKMDI_DSCO_PLC_ENABLE1
		if(TLKMDI_DSCO_CODEC_ID_MSBC == sTlkMdiDscoCtrl[index].codec){
			tlkalg_sbc_decUpdateParam(sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_SBC_DEC_BUFF_OFFS);
			tlkalg_msbc_decData((uint08 *)&gTlkalgSbcPlcIndices0[0], 57, (uint08*)pBuffer);
		}else{
			tlkalg_cvsd_updateParam(sTlkMdiDscoBuff[index].pEncodeBuffer, sTlkMdiDscoBuff[index].pEncodeBuffer+TLKMDI_DSCO_CVSD_PARAM_SIZE,
			TLKMDI_DSCO_CVSD_PARAM_SIZE, TLKMDI_DSCO_CVSD_CACHE_SIZE);
			sTlkMdiDscoCtrl[index].dec_func(pData, 60, (uint08*)pBuffer);
		}
		tlkalg_sbc_plcBadFrame(pState, pBuffer, pBuffer);
        #endif
	}
	if(index == 0) {
		tlkapi_qfifo_dropData(&sTlkMdiDscoSpkFifo);
	}else if(index == 1) {
		tlkapi_qfifo_dropData(&sTlkMdiDscoSpk01Fifo);
	}
	
	if(sTlkMdiDscoCtrl[index].dropSpkNumb != 0) sTlkMdiDscoCtrl[index].dropSpkNumb --;
	if(sTlkMdiDscoCtrl[index].dropSpkNumb != 0) return 0;
	
	return 120 * 2;
}
static int tlkmdi_dsco_msbcEncode(uint08 index, uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	////////////////////// msbc format ////////////////////////////////
	//msbc, 57, 240, ad_0_0
	//msbc, 57, 240, ad_75_54,_a4
	uint32 dlen = 0x00;
	pOut[0] = 0x01;
	pOut[1] = sTlkMdiAudDscoByte1[sTlkMdiDscoCtrl[index].numb & 3];
	
	tlkalg_sbc_encData((const uint08*)pSrc, srcLen, pOut + 2, 64, &dlen);
	sTlkMdiDscoCtrl[index].numb ++;
	if(dlen == 57) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkmdi_dsco_msbcDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	uint32 dlen;
	tlkalg_sbc_decData((const uint08*)pSrc + 4, srcLen, pOut, 240, &dlen, 1, 0x01);
	if(dlen == 240){
//		tlkapi_trace(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_msbcDecode: success");
		return TLK_ENONE;
	}else{
//		tlkapi_error(TLKMDI_AUDDSCO_DBG_FLAG, TLKMDI_AUDDSCO_DBG_SIGN, "tlkmdi_dsco_msbcDecode: failure");
		return -TLK_EFAIL;
	}
}
static int tlkmdi_dsco_cvsdEncode(uint08 index ,uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	if(tlkalg_cvsd_enc((short *)pSrc, srcLen/2, pOut)) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkmdi_dsco_cvsdDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	bool isSucc;
	if(pSrc[0] == TLKMDI_DSCO_PACKET_LOSS_FLAG){
		isSucc = tlkalg_cvsd_decMute((short*)pOut, srcLen * 2);
	}else{
		isSucc = tlkalg_cvsd_dec(pSrc+2, srcLen, (short *)pOut);
	}
	if(isSucc) return TLK_ENONE;
	else return -TLK_EFAIL;
}

static bool tlkmdi_dsco_mallocAlgBuffer(uint08 index)
{
	bool isOK = true;

	if(sTlkMdiDscoBuff[index].pTempBuffer == nullptr){
		sTlkMdiDscoBuff[index].pTempBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_DSCO_TEMP_BUFFER_SIZE);
	}
	if(sTlkMdiDscoBuff[index].pCacheBuffer == nullptr){
		sTlkMdiDscoBuff[index].pCacheBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_DSCO_CACHE_BUFFER_SIZE);
	} 
	if(sTlkMdiDscoBuff[index].pEncodeBuffer == nullptr){
		sTlkMdiDscoBuff[index].pEncodeBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_DSCO_ENCODE_BUFFER_SIZE);
	}
	if(sTlkMdiDscoBuff[index].pTempBuffer == nullptr || sTlkMdiDscoBuff[index].pCacheBuffer == nullptr
		|| sTlkMdiDscoBuff[index].pEncodeBuffer == nullptr){
		isOK = false;
	}
	return isOK;
}
static void tlkmdi_dsco_freeAlgBuffer(uint08 index)
{
	if(sTlkMdiDscoBuff[index].pTempBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiDscoBuff[index].pTempBuffer);
		sTlkMdiDscoBuff[index].pTempBuffer = nullptr;
	}
	if(sTlkMdiDscoBuff[index].pCacheBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiDscoBuff[index].pCacheBuffer);
		sTlkMdiDscoBuff[index].pCacheBuffer = nullptr;
	}
	if(sTlkMdiDscoBuff[index].pEncodeBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiDscoBuff[index].pEncodeBuffer);
		sTlkMdiDscoBuff[index].pEncodeBuffer = nullptr;
	}
}

static uint08 tlkmdi_dsco_getScoByHandle(uint16 aclHandle)
{
	uint08 index;
	for(index = 0;index < TLKMDI_DSCO_MAX_NUMB;index++) {
		if(aclHandle == sTlkMdiDscoCtrl[index].aclHandle) break;
	}
	if(index == TLKMDI_DSCO_MAX_NUMB) return TLKMDI_DSCO_MAX_NUMB;
	else return index;
}
static uint08 tlkmdi_dsco_getIdleSco()
{
	uint08 index;
	for(index = 0;index < TLKMDI_DSCO_MAX_NUMB;index++) {
		if(sTlkMdiDscoCtrl[index].aclHandle == 0) break;
	}
	if(index == TLKMDI_DSCO_MAX_NUMB) return TLKMDI_DSCO_MAX_NUMB;
	else return index;
}

#endif


