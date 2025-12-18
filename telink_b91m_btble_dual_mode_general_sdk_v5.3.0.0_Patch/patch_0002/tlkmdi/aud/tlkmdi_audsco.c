/********************************************************************************************************
 * @file	tlkmdi_audsco.c
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
#if (TLK_MDI_AUDSCO_ENABLE)
#include "drivers.h"
#include "tlkmdi/aud/tlkmdi_audio.h"
#include "tlkalg/audio/sbc/tlkalg_sbc.h"
#include "tlkalg/audio/cvsd/tlkalg_cvsd.h"
#include "tlkmdi/aud/tlkmdi_audsco.h"
#if TLK_ALG_EC_ENABLE
#include "tlkalg/audio/ec/tlkalg_ec.h"
#include "tlkalg/audio/aec/tlkalg_aec_ns.h"
#include "tlkalg/audio/agc/tlkalg_agc.h"
#endif
#if TLK_ALG_AGC_ENABLE
#include "tlkalg/audio/agc/tlkalg_agc.h"
#endif

#include "tlkstk/bt/bth/bth_stdio.h"
#include "tlkdev/sys/tlkdev_codec.h"
#include "tlksys/prt/tlkpti_audio.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "tlksys/prt/tlkpti_phone.h"
#include "tlksys/tlksys_stdio.h"
#include "tlkmdi/aud/tlkmdi_audmem.h"

#define TLKMDI_AUDSCO_DOUBLE_CHANNEL_ENABLE     1


#define TLKMDI_AUDSCO_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_SCO << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_AUDSCO_DBG_SIGN       "[MDI]"


#define TLKMDI_SCO_PACKET_LOSS_FLAG        1
#define TLKMDI_SCO_SCO_LOSS_TEST           0
#define TLKMDI_SCO_PLC_ENABLE1             1


extern void btc_sco_regDataCB(void *prx, void *ptx);

static void tlkmdi_sco_micHandler(void);
static void tlkmdi_sco_spkHandler(void);

static int tlkmdi_sco_msbcEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_sco_msbcDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_sco_cvsdEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
static int tlkmdi_sco_cvsdDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut);

static int tlkmdi_sco_getPcmData(sint16 *pBuffer);

static void tlkmdi_sco_initCodec(bool enable);
static bool tlkmdi_sco_initBuffer(bool enable);
static bool tlkmdi_sco_mallocAlgBuffer(void);
static void tlkmdi_sco_freeAlgBuffer(void);

static void tlkmdi_sco_addSpkEncFrame(uint08 id,uint16 scoHandle, uint08 *p, int len);
static void tlkmdi_sco_getMicEncFrame(uint08 id,uint16 scoHandle, uint08 *p, int len);


const unsigned char sTlkMdiScoMSBCSilencePkt[60] = {0x00,0x00,
	0xad,0x00,0x00,0xc5,0x00,0x00,0x00,0x00, 0x77,0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,
	0xdb,0x6d,0xdd,0xb6,0xdb,0x77,0x6d,0xb6, 0xdd,0xdb,0x6d,0xb7,0x76,0xdb,0x6d,0xdd,
	0xb6,0xdb,0x77,0x6d,0xb6,0xdd,0xdb,0x6d, 0xb7,0x76,0xdb,0x6d,0xdd,0xb6,0xdb,0x77,
	0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,0xdb, 0x6c,0x00 
};

static uint08 *sTlkMdiScoAgcBuffer = nullptr;
static tlkapi_qfifo_t sTlkMdiScoSpkFifo;
static tlkapi_qfifo_t sTlkMdiScoMicFifo;

static tlkmdi_sco_ctrl_t sTlkMdiScoCtrl;

static tlkmdi_sco_buff_t sTlkMdiScoBuff;
static uint08 sTlkMdiAudScoLength = 0;
static uint08 sTlkMdiAudScoFrame[120];

#if TLKMDI_SCO_SCO_LOSS_TEST
uint16 sTlkMdiScoErrIndex = 0;
uint16 sTlkMdiScoErrCount = 0;
#endif

static tlkalg_sbc_plcState_t gTlkalgSbcPlcState;

#define TLKMDI_SCO_PCM_FROM_UART		   0

#if (TLKMDI_SCO_PCM_FROM_UART)

#include "tlkdev/tlkdev.h"

#define TLKMDI_SCO_SERIAL_AUDIO_DATA_LEN   120
#define TLKMDI_SCO_SERIAL_RXBUFF_NUMB      8
#define TLKMDI_SCO_SERIAL_RXBUFF_SIZE      256
__attribute__((aligned(4)))
static uint08 sTlkMdiScoRecvBuffer[TLKMDI_SCO_SERIAL_RXBUFF_NUMB*(TLKMDI_SCO_SERIAL_RXBUFF_SIZE+4)];

static uint32 tlkmdi_audsco_serialInput(uint08 *pData, uint16 dataLen);
static int tlkmdi_audsco_serialInit(void);

static int tlkmdi_audsco_serialInit(void)
{
	uint32 baudrate = 2000000;

	tlkdev_mount(TLKDEV_DID_SERIAL0);
	tlkdev_ioctl(TLKDEV_DID_SERIAL0, TLKDEV_OPCODE_SERIAL_SET_BAUDRATE, baudrate, 0);
	tlkdev_ioctl(TLKDEV_DID_SERIAL0, TLKDEV_OPCODE_SERIAL_REG_RECV_CB, (uint32)tlkmdi_audsco_serialInput, 0);
	tlkdev_ioctl(TLKDEV_DID_SERIAL0, TLKDEV_OPCODE_SERIAL_SET_RX_QFIFO,
				((TLKMDI_SCO_SERIAL_RXBUFF_NUMB & 0xFFFF)<<16) |((TLKMDI_SCO_SERIAL_RXBUFF_SIZE+4) & 0xFFFF),
				(uint32)sTlkMdiScoRecvBuffer);
	tlkdev_open(TLKDEV_DID_SERIAL0, 0);
	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_serialInit:");

	return TLK_ENONE;
}

static uint32 tlkmdi_audsco_serialInput(uint08 *pData, uint16 dataLen)	// 3.75ms 120bytes
{
	// gpio_toggle(GPIO_PC0);
	static uint16 dataIndex = 0;
	static uint08 micData[240];
	uint16 checkSum = 0;
	uint16 seqNumb;
	uint16 dataHead;
	uint08 *pBuffer;
	uint08 *pPcmData;

	// tlkapi_array(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_serialInput:", pData, 16);
	if (pData == nullptr || dataLen != TLKMDI_SCO_SERIAL_AUDIO_DATA_LEN + 8) return TLK_EPARAM;

	dataHead = pData[1] << 8 | pData[0];
	seqNumb  = pData[3] << 8 | pData[2];
	if (dataHead != 0xA5A5) {
		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_serialInput: error dataHead:0x%x", dataHead);
		return TLK_EPARAM;
	}
	
	for (int index = 0; index < 126; index++) {
		checkSum += pData[index];
	}
	checkSum &= 0xFFFF;
	if (checkSum != (pData[127] << 8 | pData[126])) {
		tlkapi_warn(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_serialInput: checkSum error, sequence numb:0x%x", seqNumb);
	}
	
	pPcmData = pData + 4;
	if (dataIndex % 2 == 0) 
		tmemcpy(&micData[0], pPcmData, TLKMDI_SCO_SERIAL_AUDIO_DATA_LEN);
	else
		tmemcpy(&micData[120], pPcmData, TLKMDI_SCO_SERIAL_AUDIO_DATA_LEN);
	
	if (dataIndex % 2 == 1) {
		pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiScoMicFifo);
		if(pBuffer == nullptr || sTlkMdiScoCtrl.enc_func == nullptr) return TLK_EPARAM;

		sTlkMdiScoCtrl.enc_func(micData, 240, pBuffer);

		if(sTlkMdiScoCtrl.dropMicNumb != 0) sTlkMdiScoCtrl.dropMicNumb --;
		if(sTlkMdiScoCtrl.dropMicNumb != 0) return TLK_EPARAM;

		tlkapi_qfifo_dropBuff(&sTlkMdiScoMicFifo);
	}
	dataIndex++;

	return TLK_ENONE;
}

_attribute_bt_ram_code_
static void tlkmdi_sco_addSpkNone(uint08 id, uint16 scoHandle,uint08 *pData, int dataLen)
{
	return;
}

#endif 	//#if (TLKMDI_SCO_PCM_FROM_UART)

/******************************************************************************
 * Function: tlkmdi_audsco_init
 * Descript: Initial the Sco block, register the Sco data callback. and register 
 *           connect and disconnect or codec callback to listen the sco status. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_audsco_init(void)
{
	#if (TLKMDI_SCO_PCM_FROM_UART)	
	tlkmdi_audsco_serialInit();
	btc_sco_regDataCB(tlkmdi_sco_addSpkNone, tlkmdi_sco_getMicEncFrame);
	#else
	btc_sco_regDataCB(tlkmdi_sco_addSpkEncFrame, tlkmdi_sco_getMicEncFrame);		
	#endif
	return TLK_ENONE;
}


int tlkmdi_audsco_enable(uint08 *pData,uint16 dataLen)
{
	if(sTlkMdiScoCtrl.enable) tlkmdi_audsco_disable();
	if(dataLen < 5 ) return -TLK_EPARAM;

	uint16 aclHandle,scoHandle;
	uint08 codec;
	
	aclHandle = ((pData[1] << 8) | pData[0]);
	scoHandle = ((pData[3] << 8) | pData[2]);
	codec = pData[4];

	sTlkMdiScoCtrl.aclHandle = aclHandle;
	sTlkMdiScoCtrl.scoHandle = scoHandle;
	sTlkMdiScoCtrl.codec = codec;

	tlkmdi_audsco_switch(sTlkMdiScoCtrl.aclHandle,TLK_STATE_OPENED);
	tlkmdi_audio_sendStartEvt(TLKPTI_AUD_OPTYPE_SCO, sTlkMdiScoCtrl.aclHandle);
	return sTlkMdiScoCtrl.enable;
}
int tlkmdi_audsco_disable(void)
{
	tlkmdi_audsco_switch(sTlkMdiScoCtrl.aclHandle,TLK_STATE_CLOSED);
	tlkmdi_audio_sendCloseEvt(TLKPTI_AUD_OPTYPE_SCO, sTlkMdiScoCtrl.aclHandle);
	sTlkMdiScoCtrl.aclHandle = 0;
	sTlkMdiScoCtrl.scoHandle = 0;
	sTlkMdiScoCtrl.codec = 0;

	return !sTlkMdiScoCtrl.enable;
}

/******************************************************************************
 * Function: tlkmdi_audsco_switch
 * Descript: Change the sco status. 
 * Params:
 *        @handle[IN]--The handle
 *        @status[IN]--The status.
 * Return: The interval value.
*******************************************************************************/
bool tlkmdi_audsco_switch(uint16 handle, uint08 status)
{
	bool isSucc;
	bool enable;
	uint16 aecSize;
	uint16 nsSize;

	aecSize = tlka_aec_get_size();
	nsSize = tlka_ns_get_size();
	if(TLKMDI_SCO_AEC_ST_SIZE < aecSize){
		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "current aec_size[%d] > TLKMDI_SCO_AEC_ST_SIZE", aecSize);
		return false;
	}
	if(TLKMDI_SCO_SPEEX_NS_SIZE < nsSize){
		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "current ns_size[%d] > TLKMDI_SCO_SPEEX_NS_SIZE", nsSize);
		return false;
	}
	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_switch - handle:%d status:%d ", handle,status);

	
	if(status == TLK_STATE_OPENED) {
		btc_sco_regDataCB(tlkmdi_sco_addSpkEncFrame, tlkmdi_sco_getMicEncFrame);	
		enable = true;
	}
	else{
		enable = false;
	}

	if(sTlkMdiScoCtrl.enable == enable) return true;

	if(!enable) sTlkMdiScoCtrl.enable = false;
	isSucc = tlkmdi_sco_initBuffer(enable);
	if(!isSucc) enable = false;

	sTlkMdiScoCtrl.numb = 0;
	sTlkMdiScoCtrl.enable = enable;
	sTlkMdiScoCtrl.dropMicNumb = 0;
	sTlkMdiScoCtrl.spkReadOffs = 0;
	sTlkMdiScoCtrl.micReadOffs = 0;
	sTlkMdiScoCtrl.dropSpkNumb = 0;
	#if (TLK_DEV_CODEC_ENABLE)
	sTlkMdiScoCtrl.spkBuffLen = tlkdev_codec_getSpkBuffLen();
	sTlkMdiScoCtrl.micBuffLen = tlkdev_codec_getMicBuffLen();
	#endif

	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_muteSpkBuff();
	#endif

	tlkmdi_sco_initCodec(enable);
	if(enable){
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_switch: enable");
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_open(TLKDEV_CODEC_SUBDEV_BOTH, TLKDEV_CODEC_CHANNEL_LEFT, TLKDEV_CODEC_BITDEPTH_16, 16000);
		tlkdev_codec_setSpkOffset(320);
		#endif
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_SCO, TLK_STATE_OPENED);
	}else{
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_audsco_switch: disable");
		#if (TLK_DEV_CODEC_ENABLE)
		tlkdev_codec_close();
		#endif
		tlkmdi_audio_sendStatusChangeEvt(TLKPRT_COMM_AUDIO_CHN_SCO, TLK_STATE_CLOSED);
	}

	return isSucc;
}


/******************************************************************************
 * Function: tlkmdi_audsco_init
 * Descript: verify the sco is enable. 
 * Params: None.
 * Return: true is success,other value is false.
*******************************************************************************/
bool tlkmdi_audsco_isBusy(void)
{
	return sTlkMdiScoCtrl.enable;
}

/******************************************************************************
 * Function: tlkmdi_audsco_intval
 * Descript: Get the sco interval which will determine when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsco_intval(void)
{
	return 1000;
}


/******************************************************************************
 * Function: tlkmdi_audsco_irqProc
 * Descript: call the mic handler and spkhandler. 
 * Params: None.
 * Return: The true/false.
*******************************************************************************/
bool tlkmdi_audsco_irqProc(void)
{
	if(!sTlkMdiScoCtrl.enable) return false;
	#if (!TLKMDI_SCO_PCM_FROM_UART)	
	tlkmdi_sco_micHandler();
	tlkmdi_sco_spkHandler();
	#endif	// #if (!TLKMDI_SCO_PCM_FROM_UART)	
	if(sTlkMdiScoCtrl.enable) return true;
	else return false;
}

_attribute_bt_ram_code_
static void tlkmdi_sco_addSpkEncFrame(uint08 id, uint16 scoHandle,uint08 *pData, int dataLen) // NOTE: This is in IRQ
{
	uint08 *pBuffer;

	if(pData == nullptr || dataLen == 0 || pData[0] != BTH_HCI_TYPE_SCO_DATA) return;
	if(!sTlkMdiScoCtrl.enable) return;

	uint16 tempLen;
	uint16 syncHead = pData[1] + (pData[2] << 8);
	pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiScoSpkFifo);
	if(pBuffer == nullptr) return;
	pBuffer[0] = 0;
	pBuffer[1] = 0;
	#if TLKMDI_SCO_SCO_LOSS_TEST
	sTlkMdiScoErrIndex ++ ;
	if(sTlkMdiScoErrIndex >= 100){
		sTlkMdiScoErrCount = 0;
		sTlkMdiScoErrIndex = 0;
	}
	#endif
	if((syncHead & 0x3000) == BTH_HCI_SYNC_HDR_PSF_GOOD){
		tempLen = pData[3];
		if(tempLen > TLKMDI_SCO_SPK_ENC_BUFF_SIZE) tempLen = TLKMDI_SCO_SPK_ENC_BUFF_SIZE;
		if(tempLen != 0) tmemcpy(pBuffer+2, &pData[4], tempLen);
	}else{
		#if TLKMDI_SCO_SCO_LOSS_TEST
		sTlkMdiScoErrCount ++;
		#endif
        pBuffer[0] = TLKMDI_SCO_PACKET_LOSS_FLAG;
	}
	tlkapi_qfifo_dropBuff(&sTlkMdiScoSpkFifo);
}

uint08 sTlkMdiMicMuteIndex = 0;
_attribute_bt_data_retention_ unsigned char sTlkMdiAudScoByte1[4] = {0x08, 0x38, 0xc8, 0xf8};
_attribute_bt_data_retention_ unsigned char sTlkMdiAudScoSilencePkt[64] = {0x00,0x00,
	0xad,0x00,0x00,0xc5,0x00,0x00,0x00,0x00, 0x77,0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,
	0xdb,0x6d,0xdd,0xb6,0xdb,0x77,0x6d,0xb6, 0xdd,0xdb,0x6d,0xb7,0x76,0xdb,0x6d,0xdd,
	0xb6,0xdb,0x77,0x6d,0xb6,0xdd,0xdb,0x6d, 0xb7,0x76,0xdb,0x6d,0xdd,0xb6,0xdb,0x77,
	0x6d,0xb6,0xdd,0xdb,0x6d,0xb7,0x76,0xdb, 0x6c,0x00,0x00,0x00,0x00,0x00};
_attribute_bt_ram_code_
static void tlkmdi_sco_getMicEncFrame(uint08 id,uint16 scoHandle, uint08 *pBuff, int buffLen) // NOTE: This is in IRQ
{
	uint08 *pData;

	if(!sTlkMdiScoCtrl.enable) return;
	
	pData = tlkapi_qfifo_getData(&sTlkMdiScoMicFifo);
	if(pData != nullptr){
		if(buffLen > TLKMDI_SCO_MIC_ENC_BUFF_SIZE){
			buffLen = TLKMDI_SCO_MIC_ENC_BUFF_SIZE;
		}
		tmemcpy(pBuff, pData, buffLen);
		tlkapi_qfifo_dropData(&sTlkMdiScoMicFifo);
	}else{
		if(sTlkMdiScoCtrl.codec == TLKMDI_SCO_CODEC_ID_CVSD){
			tmemset(pBuff, 0x55, buffLen);
		}else if(sTlkMdiScoCtrl.codec != TLKMDI_SCO_CODEC_ID_MSBC){
			tmemset(pBuff, 0x00, buffLen);
		}else{
			u8 copyLen = 0;
			if(buffLen > 64) copyLen = 64;
			else copyLen = buffLen;
			sTlkMdiAudScoSilencePkt[0] = 0x01;
			sTlkMdiAudScoSilencePkt[1] = sTlkMdiAudScoByte1[sTlkMdiMicMuteIndex & 0x03];
			sTlkMdiMicMuteIndex ++;
			tmemcpy(pBuff, sTlkMdiAudScoSilencePkt, copyLen);
		}
	}
}

static void tlkmdi_sco_micHandler(void)
{
	#if (TLK_DEV_CODEC_ENABLE)
	uint16 offset = 0;
	#endif
	uint08 *pMicPtr;
	uint08 *pBuffer;
	short spkData[120];
	short micData[120];
		
	pBuffer = tlkapi_qfifo_getBuff(&sTlkMdiScoMicFifo);
	if(pBuffer == nullptr || sTlkMdiScoCtrl.enc_func == nullptr) return;

	#if (TLK_DEV_CODEC_ENABLE)
	if(!tlkdev_codec_readMicData((uint08*)micData, 240, &offset)){
		return;
	}
	#endif
		
	#if TLK_ALG_EC_ENABLE
	#if (TLK_DEV_CODEC_ENABLE)
	tlkdev_codec_readSpkData((uint08*)spkData, 240, sTlkMdiScoCtrl.spkReadOffs);
	#endif
	pMicPtr = (uint08*)tlkalg_ec_frame((uint08*)micData, (uint08*)spkData);
	sTlkMdiScoCtrl.micReadOffs += 240;
	sTlkMdiScoCtrl.spkReadOffs += 240;
	if(sTlkMdiScoCtrl.micReadOffs >= sTlkMdiScoCtrl.micBuffLen){
		sTlkMdiScoCtrl.micReadOffs -= sTlkMdiScoCtrl.micBuffLen;
	}
	if(sTlkMdiScoCtrl.spkReadOffs >= sTlkMdiScoCtrl.spkBuffLen){
		sTlkMdiScoCtrl.spkReadOffs -= sTlkMdiScoCtrl.spkBuffLen;
	}
	#else
	pMicPtr = (uint08*)micData;
	#endif
	if(pMicPtr == nullptr) return;
	
    /* mixing tone here for remote HF device or AG device */
	#if TLK_ALG_AGC_ENABLE
	sint16 pcm_agc[120];
	sint16* pcm_agc_ptr=pcm_agc;
	sint16* pcm_ptr = (sint16*)pMicPtr;
	tlka_agc_process(sTlkMdiScoAgcBuffer, (const short* const*)&pcm_ptr, 120, (short* const*)&pcm_agc_ptr);
	#endif
	
	#if TLK_ALG_AGC_ENABLE
	sTlkMdiScoCtrl.enc_func((uint08*)pcm_agc, 240, (uint08*)pBuffer);
	#else
	sTlkMdiScoCtrl.enc_func((uint08*)pMicPtr, 240, (uint08*)pBuffer);
	#endif

	if(sTlkMdiScoCtrl.dropMicNumb != 0) sTlkMdiScoCtrl.dropMicNumb --;
	if(sTlkMdiScoCtrl.dropMicNumb != 0) return;
	
	tlkapi_qfifo_dropBuff(&sTlkMdiScoMicFifo);
}
static void tlkmdi_sco_spkHandler(void)
{
	int count = 8;
	uint16 volume;

	if( !sTlkMdiScoCtrl.enable || sTlkMdiScoBuff.pTempBuffer == nullptr) return;
	while(count--){
		int np = 0;
		#if (TLK_DEV_CODEC_ENABLE)
		if(tlkdev_codec_getSpkIdleLen() > 512){
			np = tlkmdi_sco_getPcmData((sint16*)sTlkMdiScoBuff.pTempBuffer); //tvoice_get_playback_data, tmusic_get_playback_data, app_music_getPlaybackData
		}
		#else
		np = tlkmdi_sco_getPcmData((sint16*)sTlkMdiScoBuff.pTempBuffer);
		#endif
		if(np == 0) break;
		
		if(np){
            sint16 *dec_pcm = (int16_t *)sTlkMdiScoBuff.pTempBuffer;
            int temp;
			int samples = 120;
			volume = tlkmdi_audio_getVoiceCalVolume();
            for(int i = 0; i < samples; i++){
                temp = ((int)dec_pcm[i] * volume) >> TLKMDI_AUDIO_VOLUME_EXPAND;
                dec_pcm[i] = temp;
            }
			#if (TLK_DEV_CODEC_ENABLE)
			tlkdev_codec_fillSpkBuff((uint08*)sTlkMdiScoBuff.pTempBuffer, np);
			#endif
		}
	}
}


static void tlkmdi_audsco_makeValidFrame(uint08 *pData)
{
	uint08 index;
	
	if(pData[0] == TLKMDI_SCO_PACKET_LOSS_FLAG || TLKMDI_SCO_CODEC_ID_MSBC != sTlkMdiScoCtrl.codec){
		if(sTlkMdiAudScoLength != 0) sTlkMdiAudScoLength = 0;
		return;
	}
	if(pData[2] == 0x01 && (pData[3] & 0x0F) == 0x08 && pData[4] == 0xAD){
		if(sTlkMdiAudScoLength != 0) sTlkMdiAudScoLength = 0;
		return;
	}
	tmemcpy(sTlkMdiAudScoFrame+sTlkMdiAudScoLength, pData+2, 60);
	sTlkMdiAudScoLength += 60;
	for(index=0; index<sTlkMdiAudScoLength; index++){
		if(sTlkMdiAudScoFrame[index] == 1 && (sTlkMdiAudScoFrame[index+1] & 0x0F) == 0x08
			&& sTlkMdiAudScoFrame[index+2] == 0xAD) break;
	}
	if(index == sTlkMdiAudScoLength){
		tmemcpy(sTlkMdiAudScoFrame, sTlkMdiAudScoFrame+(sTlkMdiAudScoLength-3), 3);
		sTlkMdiAudScoLength = 3;
		pData[0] = TLKMDI_SCO_PACKET_LOSS_FLAG;
	}else{
		sTlkMdiAudScoLength = sTlkMdiAudScoLength-index;
		if(sTlkMdiAudScoLength < 60){
			pData[0] = TLKMDI_SCO_PACKET_LOSS_FLAG;
		}else{
			tmemcpy(pData+2, sTlkMdiAudScoFrame+index, 60);
			index += 60;
			sTlkMdiAudScoLength -= 60;
		}
		if(sTlkMdiAudScoLength != 0){
			tmemcpy(sTlkMdiAudScoFrame, sTlkMdiAudScoFrame+index, sTlkMdiAudScoLength);
		}
	}
}
static int tlkmdi_sco_getPcmData(sint16 *pBuffer)
{
	uint08 *pData;
				
	pData = tlkapi_qfifo_getData(&sTlkMdiScoSpkFifo);
	if(pData == nullptr || sTlkMdiScoCtrl.dec_func == nullptr) return 0;

	tlkmdi_audsco_makeValidFrame(pData);
	if(pData[0] != TLKMDI_SCO_PACKET_LOSS_FLAG){
		sTlkMdiScoCtrl.dec_func(pData, 60, (uint08*)pBuffer);
		#if TLKMDI_SCO_PLC_ENABLE1
		tlkalg_sbc_plcGoodFrame(&gTlkalgSbcPlcState, pBuffer, pBuffer);
		#endif
	}else{
		#if TLKMDI_SCO_PLC_ENABLE1
		if(TLKMDI_SCO_CODEC_ID_MSBC == sTlkMdiScoCtrl.codec){
			tlkalg_msbc_decData((uint08 *)&gTlkalgSbcPlcIndices0[0], 57, (uint08*)pBuffer);
		}else{
			sTlkMdiScoCtrl.dec_func(pData, 60, (uint08*)pBuffer);
		}
		tlkalg_sbc_plcBadFrame(&gTlkalgSbcPlcState, pBuffer, pBuffer);
        #endif
	}
	tlkapi_qfifo_dropData(&sTlkMdiScoSpkFifo);
	
	if(sTlkMdiScoCtrl.dropSpkNumb != 0) sTlkMdiScoCtrl.dropSpkNumb --;
	if(sTlkMdiScoCtrl.dropSpkNumb != 0) return 0;
	
	return 120 * 2;
}

static int tlkmdi_sco_msbcEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	////////////////////// msbc format ////////////////////////////////
	//msbc, 57, 240, ad_0_0
	//msbc, 57, 240, ad_75_54,_a4
	uint32 dlen = 0x00;
	pOut[0] = 0x01;
	pOut[1] = sTlkMdiAudScoByte1[sTlkMdiScoCtrl.numb & 3];
	
	tlkalg_sbc_encData((const uint08*)pSrc, srcLen, pOut + 2, 64, &dlen);
	sTlkMdiScoCtrl.numb ++;
	if(dlen == 57) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkmdi_sco_msbcDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	uint32 dlen;
	tlkalg_sbc_decData((const uint08*)pSrc + 4, srcLen, pOut, 240, &dlen, 1, 0x01);
	if(dlen == 240){
//		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_msbcDecode: success");
		return TLK_ENONE;
	}else{
//		tlkapi_error(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_msbcDecode: failure");
		return -TLK_EFAIL;
	}
}
static int tlkmdi_sco_cvsdEncode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	if(tlkalg_cvsd_enc((short *)pSrc, srcLen/2, pOut)) return TLK_ENONE;
	else return -TLK_EFAIL;
}
static int tlkmdi_sco_cvsdDecode(uint08 *pSrc, uint16 srcLen, uint08 *pOut)
{
	bool isSucc;
	if(pSrc[0] == TLKMDI_SCO_PACKET_LOSS_FLAG){
		isSucc = tlkalg_cvsd_decMute((short*)pOut, srcLen * 2);
	}else{
		isSucc = tlkalg_cvsd_dec(pSrc+2, srcLen, (short *)pOut);
	}
	if(isSucc) return TLK_ENONE;
	else return -TLK_EFAIL;
}


static void tlkmdi_sco_initCodec(bool enable)
{
    tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec,");

	if(!enable || sTlkMdiScoBuff.pEncodeBuffer == nullptr){
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: disable");
		sTlkMdiScoCtrl.dec_func = nullptr;
		sTlkMdiScoCtrl.enc_func = nullptr;
		tlkalg_cvsd_init(nullptr, nullptr, 0, 0);
		tlkalg_sbc_encInit(nullptr);
		tlkalg_sbc_decInit(nullptr);
	}else{
		tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: enable");
		if(TLKMDI_SCO_CODEC_ID_CVSD == sTlkMdiScoCtrl.codec){
			tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: CVSD init");
			tlkalg_cvsd_init(sTlkMdiScoBuff.pEncodeBuffer, sTlkMdiScoBuff.pEncodeBuffer+TLKMDI_SCO_CVSD_PARAM_SIZE,
				TLKMDI_SCO_CVSD_PARAM_SIZE, TLKMDI_SCO_CVSD_CACHE_SIZE);
			sTlkMdiScoCtrl.dec_func = tlkmdi_sco_cvsdDecode;
			sTlkMdiScoCtrl.enc_func = tlkmdi_sco_cvsdEncode;
	    }else{
	    	tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: enter MSBC");
			tlkapi_trace(TLKMDI_AUDSCO_DBG_FLAG, TLKMDI_AUDSCO_DBG_SIGN, "tlkmdi_sco_initCodec: SBC init");
			tlkalg_sbc_encInit(sTlkMdiScoBuff.pEncodeBuffer+TLKMDI_SCO_SBC_ENC_BUFF_OFFS);
			tlkalg_sbc_decInit(sTlkMdiScoBuff.pEncodeBuffer+TLKMDI_SCO_SBC_DEC_BUFF_OFFS);
			sTlkMdiScoCtrl.dec_func = tlkmdi_sco_msbcDecode;
			sTlkMdiScoCtrl.enc_func = tlkmdi_sco_msbcEncode;
	    }
	}
}
static bool tlkmdi_sco_initBuffer(bool enable)
{
	bool isEnable;
	#if (TLK_ALG_AGC_ENABLE)
	AGC_CFG_Param agcParam;
	#endif
	isEnable = enable;
	if(isEnable && !tlkmdi_sco_mallocAlgBuffer()){
		isEnable = false;
	}
	if(!isEnable){
		tlkmdi_sco_freeAlgBuffer();
	}
	if(!isEnable){
		#if (TLK_ALG_EC_ENABLE)
		tlkalg_ec_init(nullptr, nullptr, nullptr);
		#endif
		#if (TLK_ALG_AGC_ENABLE)
		agcParam.targetLevelDbfs = 3;
		agcParam.compressionGaindB = 30;
		agcParam.fs = 16000;
		sTlkMdiScoAgcBuffer = nullptr;
		tlka_agc_init(nullptr, agcParam);
		#endif
		tlkapi_qfifo_reset(&sTlkMdiScoSpkFifo);
		tlkapi_qfifo_reset(&sTlkMdiScoMicFifo);
	}
	else{
		#if (TLK_ALG_EC_ENABLE)
		tlkalg_ec_init(sTlkMdiScoBuff.pSpeexBuffer+TLKMDI_SCO_SPEEX_NS_OFFS,
						sTlkMdiScoBuff.pAecmBuffer+TLKMDI_SCO_AEC_ST_OFFS,
						sTlkMdiScoBuff.pScratchBuffer+TLKMDI_SCO_AEC_NS_SCRATCH_OFFS);
		#endif
		#if (TLK_ALG_AGC_ENABLE)
		agcParam.targetLevelDbfs = 3;
		agcParam.compressionGaindB = 30;
		agcParam.fs = 16000;
		sTlkMdiScoAgcBuffer = sTlkMdiScoBuff.pSpeexBuffer+TLKMDI_SCO_AGC_ST_OFFS;
		tlka_agc_init(sTlkMdiScoAgcBuffer, agcParam);
		#endif
		uint16 index;
		uint16 encBuffLen = TLKMDI_SCO_SPK_ENC_BUFF_SIZE*TLKMDI_SCO_SPK_ENC_BUFF_NUMB;
		uint16 decBuffLen = TLKMDI_SCO_MIC_ENC_BUFF_SIZE*TLKMDI_SCO_MIC_ENC_BUFF_NUMB;
		uint08 *pEncBuffer = sTlkMdiScoBuff.pCacheBuffer;
		uint08 *pDecBuffer = sTlkMdiScoBuff.pCacheBuffer+encBuffLen;
		tlkapi_qfifo_init(&sTlkMdiScoSpkFifo, TLKMDI_SCO_SPK_ENC_BUFF_NUMB, TLKMDI_SCO_SPK_ENC_BUFF_SIZE, pEncBuffer, encBuffLen);
		tlkapi_qfifo_init(&sTlkMdiScoMicFifo, TLKMDI_SCO_MIC_ENC_BUFF_NUMB, TLKMDI_SCO_MIC_ENC_BUFF_SIZE, pDecBuffer, decBuffLen);
		for(index=0; index<TLKMDI_SCO_MIC_ENC_BUFF_NUMB; index++){
			tmemcpy(pDecBuffer, sTlkMdiScoMSBCSilencePkt, 60);
			pDecBuffer[0] = 0x01;
			pDecBuffer[1] = sTlkMdiAudScoByte1[index & 3];
			pDecBuffer += TLKMDI_SCO_MIC_ENC_BUFF_SIZE;
		}
	}
	if(isEnable == enable) return true;
	else return false;
}

static bool tlkmdi_sco_mallocAlgBuffer(void)
{
	bool isOK = true;

	if(sTlkMdiScoBuff.pTempBuffer == nullptr){
		sTlkMdiScoBuff.pTempBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SCO_TEMP_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pScratchBuffer == nullptr){
		sTlkMdiScoBuff.pScratchBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SCO_AEC_NS_SCRATCH_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pAecmBuffer == nullptr){
		sTlkMdiScoBuff.pAecmBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SCO_AEC_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pSpeexBuffer == nullptr){
		sTlkMdiScoBuff.pSpeexBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SCO_SPEEX_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pCacheBuffer == nullptr){
		sTlkMdiScoBuff.pCacheBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SCO_CACHE_BUFFER_SIZE);
	} 
	if(sTlkMdiScoBuff.pEncodeBuffer == nullptr){
		sTlkMdiScoBuff.pEncodeBuffer = (uint08*)tlkmdi_audmem_calloc(TLKMDI_SCO_ENCODE_BUFFER_SIZE);
	}
	if(sTlkMdiScoBuff.pSpeexBuffer == nullptr || sTlkMdiScoBuff.pAecmBuffer == nullptr
		|| sTlkMdiScoBuff.pTempBuffer == nullptr || sTlkMdiScoBuff.pCacheBuffer == nullptr
		|| sTlkMdiScoBuff.pEncodeBuffer == nullptr){
		isOK = false;
	}
	return isOK;
}
static void tlkmdi_sco_freeAlgBuffer(void)
{
	if(sTlkMdiScoBuff.pTempBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiScoBuff.pTempBuffer);
		sTlkMdiScoBuff.pTempBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pScratchBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiScoBuff.pScratchBuffer);
				sTlkMdiScoBuff.pScratchBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pAecmBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiScoBuff.pAecmBuffer);
		sTlkMdiScoBuff.pAecmBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pSpeexBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiScoBuff.pSpeexBuffer);
		sTlkMdiScoBuff.pSpeexBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pCacheBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiScoBuff.pCacheBuffer);
		sTlkMdiScoBuff.pCacheBuffer = nullptr;
	}
	if(sTlkMdiScoBuff.pEncodeBuffer != nullptr){
		tlkmdi_audmem_free(sTlkMdiScoBuff.pEncodeBuffer);
		sTlkMdiScoBuff.pEncodeBuffer = nullptr;
	}
}


#endif //#if (TLK_MDI_AUDSCO_ENABLE)

