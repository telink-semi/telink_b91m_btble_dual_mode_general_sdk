/********************************************************************************************************
 * @file	tlkdrv_icodec.c
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
#if (MCU_CORE_TYPE == MCU_CORE_B92)
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_codec.h"
#if (TLKDRV_CODEC_ICODEC_ENABLE)
#include "tlkdrv_icodec_b92.h"
#include "drivers.h"


static bool tlkdrv_icodec_isOpen(uint08 subDev);
static int tlkdrv_icodec_init(uint08 subDev);
static int tlkdrv_icodec_open(uint08 subDev);
static int tlkdrv_icodec_pause(uint08 subDev);
static int tlkdrv_icodec_close(uint08 subDev);
static int tlkdrv_icodec_reset(uint08 subDev);
static int tlkdrv_icodec_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1);

static int tlkdrv_icodec_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_icodec_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);

static int tlkdrv_icodec_enable(bool enMic, bool enSpk);
static void tlkdrv_icodec_disable(void);


extern uint16  gTlkDrvCodecSpkBuffLen;
extern uint16  gTlkDrvCodecMicBuffLen;
extern uint08 *gpTlkDrvCodecSpkBuffer;
extern uint08 *gpTlkDrvCodecMicBuffer;


const tlkdrv_codec_modinf_t gcTlkDrvIcodecInf = {
	tlkdrv_icodec_isOpen, //IsOpen
	tlkdrv_icodec_init, //Init
	tlkdrv_icodec_open, //Open
	tlkdrv_icodec_pause, //Pause
	tlkdrv_icodec_close, //Close
	tlkdrv_icodec_reset, //Reset
	tlkdrv_icodec_config, //Config
};
static tlkdrv_icodec_t sTlkDrvIcodecCtrl = {0};


static bool tlkdrv_icodec_isOpen(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit || !sTlkDrvIcodecCtrl.isOpen) return false;
	if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		if(sTlkDrvIcodecCtrl.micIsEn) return true;
		else return false;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		if(sTlkDrvIcodecCtrl.spkIsEn) return true;
		else return false;
	}else{
		return true;
	}
}
static int tlkdrv_icodec_init(uint08 subDev)
{
	tmemset(&sTlkDrvIcodecCtrl, 0, sizeof(tlkdrv_icodec_t));
	
	sTlkDrvIcodecCtrl.isInit = true;
	sTlkDrvIcodecCtrl.isOpen = false;
	sTlkDrvIcodecCtrl.spkChannel = 0x01;
	sTlkDrvIcodecCtrl.spkBitDepth = 16;
	sTlkDrvIcodecCtrl.spkSampleRate = 48000;
	sTlkDrvIcodecCtrl.micChannel = 0x01;
	sTlkDrvIcodecCtrl.micBitDepth = 16;
	sTlkDrvIcodecCtrl.micSampleRate = 48000;	
	
	return TLK_ENONE;
}
static int tlkdrv_icodec_open(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit) return -TLK_ESTATUS;
	if(sTlkDrvIcodecCtrl.isOpen) return -TLK_EREPEAT;

	#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
	gpio_function_en(TLKDRV_ICODEC_POWER_PIN);
	gpio_output_en(TLKDRV_ICODEC_POWER_PIN); 		//enable output
	gpio_input_dis(TLKDRV_ICODEC_POWER_PIN);		//disable input
	gpio_set_up_down_res(TLKDRV_ICODEC_POWER_PIN, GPIO_PIN_PULLDOWN_100K);
	gpio_set_low_level(TLKDRV_ICODEC_POWER_PIN);
	#endif
	
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0) sTlkDrvIcodecCtrl.micIsEn = true;
	else sTlkDrvIcodecCtrl.micIsEn = false;
	if((subDev & TLKDRV_CODEC_SUBDEV_SPK) != 0) sTlkDrvIcodecCtrl.spkIsEn = true;
	else sTlkDrvIcodecCtrl.spkIsEn = false;

	tlkdrv_icodec_enable(sTlkDrvIcodecCtrl.micIsEn, sTlkDrvIcodecCtrl.spkIsEn);
	
	sTlkDrvIcodecCtrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_icodec_pause(uint08 subDev)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_icodec_close(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit || !sTlkDrvIcodecCtrl.isOpen) return -TLK_ESTATUS;

	tlkdrv_icodec_disable();

	sTlkDrvIcodecCtrl.isOpen = false;
	sTlkDrvIcodecCtrl.micIsEn = false;
	sTlkDrvIcodecCtrl.spkIsEn = false;

	#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
	gpio_set_low_level(TLKDRV_ICODEC_POWER_PIN);
	gpio_output_dis(TLKDRV_ICODEC_POWER_PIN);
	gpio_input_dis(TLKDRV_ICODEC_POWER_PIN);
	gpio_set_up_down_res(TLKDRV_ICODEC_POWER_PIN, GPIO_PIN_UP_DOWN_FLOAT);

	gpio_function_en(TLKDRV_ICODEC_DMIC_DATA_PIN);
	gpio_set_low_level(TLKDRV_ICODEC_DMIC_DATA_PIN);
	gpio_output_dis(TLKDRV_ICODEC_DMIC_DATA_PIN);
	gpio_input_dis(TLKDRV_ICODEC_DMIC_DATA_PIN);
	gpio_set_up_down_res(TLKDRV_ICODEC_DMIC_DATA_PIN, GPIO_PIN_UP_DOWN_FLOAT);

	gpio_function_en(TLKDRV_ICODEC_DMIC_CLK0_PIN);
	gpio_set_low_level(TLKDRV_ICODEC_DMIC_CLK0_PIN);
	gpio_output_dis(TLKDRV_ICODEC_DMIC_CLK0_PIN);
	gpio_input_dis(TLKDRV_ICODEC_DMIC_CLK0_PIN);
	gpio_set_up_down_res(TLKDRV_ICODEC_DMIC_CLK0_PIN, GPIO_PIN_UP_DOWN_FLOAT);

	gpio_function_en(TLKDRV_ICODEC_DMIC_CLK1_PIN);
	gpio_set_low_level(TLKDRV_ICODEC_DMIC_CLK1_PIN);
	gpio_output_dis(TLKDRV_ICODEC_DMIC_CLK1_PIN);
	gpio_input_dis(TLKDRV_ICODEC_DMIC_CLK1_PIN);
	gpio_set_up_down_res(TLKDRV_ICODEC_DMIC_CLK1_PIN, GPIO_PIN_UP_DOWN_FLOAT);
	#endif
	
	return TLK_ENONE;
}
static int tlkdrv_icodec_reset(uint08 subDev)
{
	bool isOpen = sTlkDrvIcodecCtrl.isOpen;	
	if(isOpen) tlkdrv_icodec_close(subDev);
	if(isOpen) tlkdrv_icodec_open(subDev);
	return TLK_ENONE;
}
static int tlkdrv_icodec_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	int ret = -TLK_ENOSUPPORT;
	switch(opcode){
		case TLKDRV_CODEC_OPCODE_SET_MUTE:  //param:[uint08]isMute
			ret = tlkdrv_icodec_setMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_MUTE:
			ret = tlkdrv_icodec_getMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_VOLUME: //param:[uint08]volume
			ret = tlkdrv_icodec_setVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_VOLUME:
			ret = tlkdrv_icodec_getVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_CHANNEL: //param:[uint08]chnCnt-1/2/3
			ret = tlkdrv_icodec_setChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_CHANNEL:
			ret = tlkdrv_icodec_getChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH: //param:[uint08]bitDepth-8,16,20,24,32
			ret = tlkdrv_icodec_setBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_BIT_DEPTH:
			ret = tlkdrv_icodec_getBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE: //param:[uint32]bitDepth-8,16,20,24,32
			ret = tlkdrv_icodec_setSampleRateDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE:
			ret = tlkdrv_icodec_getSampleRateDeal(subDev, param0, param1);
			break;
	}
	return ret;
}


static int tlkdrv_icodec_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_icodec_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_icodec_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_icodec_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_icodec_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 channel = param0 & 0xFF;
	if(channel != 0x01 && channel != 0x02 && channel != 0x03) return -TLK_EPARAM;
	if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		sTlkDrvIcodecCtrl.spkChannel = channel;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		sTlkDrvIcodecCtrl.micChannel = channel;
	}else{
		sTlkDrvIcodecCtrl.spkChannel = channel;
		sTlkDrvIcodecCtrl.micChannel = channel;
	}
	return TLK_ENONE;
}
static int tlkdrv_icodec_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		return sTlkDrvIcodecCtrl.spkChannel;
	}else{
		return sTlkDrvIcodecCtrl.micChannel;
	}
}
static int tlkdrv_icodec_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 bitDapth = param0 & 0xFF;
	if(bitDapth != 16 && bitDapth != 20) return -TLK_ENOSUPPORT;
	if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		sTlkDrvIcodecCtrl.spkBitDepth = bitDapth;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		sTlkDrvIcodecCtrl.micBitDepth = bitDapth;
	}else{
		sTlkDrvIcodecCtrl.spkBitDepth = bitDapth;
		sTlkDrvIcodecCtrl.micBitDepth = bitDapth;
	}
	return TLK_ENONE;
}
static int tlkdrv_icodec_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		return sTlkDrvIcodecCtrl.spkBitDepth;
	}else{
		return sTlkDrvIcodecCtrl.micBitDepth;
	}
}
static int tlkdrv_icodec_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint32 sampleRate = param0;
	if(sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000 && sampleRate != 44100 && sampleRate != 48000){
		return -TLK_EPARAM;
	}
	if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		sTlkDrvIcodecCtrl.spkSampleRate = sampleRate;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		sTlkDrvIcodecCtrl.micSampleRate = sampleRate;
	}else{
		sTlkDrvIcodecCtrl.spkSampleRate = sampleRate;
		sTlkDrvIcodecCtrl.micSampleRate = sampleRate;
	}
	return TLK_ENONE;
}
static int tlkdrv_icodec_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		return sTlkDrvIcodecCtrl.spkSampleRate;
	}else{
		return sTlkDrvIcodecCtrl.micSampleRate;
	}
}



static int tlkdrv_icodec_enable(bool enMic, bool enSpk)
{
	uint08 spkSrc = 0xFF;
	uint08 micSrc = 0xFF;
	uint08 spkSRate  = 0;
	uint08 spkDWdith = 0xFF;
	uint08 micSRate  = 0;
	uint08 micDWdith = 0xFF;

	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_icodec_enable: 001");
	if(enSpk){
		if(sTlkDrvIcodecCtrl.spkBitDepth == 16) spkDWdith = CODEC_BIT_16_DATA;
		else if(sTlkDrvIcodecCtrl.spkBitDepth == 20) spkDWdith = CODEC_BIT_20_DATA;
		if(spkDWdith != 0xFF){
			#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
			if(sTlkDrvIcodecCtrl.spkChannel == 0x03){
				spkSrc = DMIC_STREAM0_STEREO;
			}else if(sTlkDrvIcodecCtrl.spkChannel == 0x02){
				spkSrc = DMIC_STREAM0_MONO_R;
			}else{
				spkSrc = DMIC_STREAM0_MONO_L;
			}
			#endif
			#if (TLKHW_TYPE == TLKHW_TYPE_C1T293A33_V1_0)
			if(sTlkDrvIcodecCtrl.spkChannel == 0x03){
				spkSrc = AMIC_STREAM0_STEREO;
			}else if(sTlkDrvIcodecCtrl.spkChannel == 0x02){
				spkSrc = AMIC_STREAM0_MONO_R;
			}else{
				spkSrc = AMIC_STREAM0_MONO_L;
			}
			#endif
		}
		if(sTlkDrvIcodecCtrl.spkSampleRate == 8000) spkSRate = AUDIO_8K;
		else if(sTlkDrvIcodecCtrl.spkSampleRate == 16000) spkSRate = AUDIO_16K;
		else if(sTlkDrvIcodecCtrl.spkSampleRate == 32000) spkSRate = AUDIO_32K;
		else if(sTlkDrvIcodecCtrl.spkSampleRate == 44100) spkSRate = AUDIO_44P1K;
		else if(sTlkDrvIcodecCtrl.spkSampleRate == 48000) spkSRate = AUDIO_48K;
		tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_icodec_enable: 002: %d %d %d", spkSrc, spkSRate, spkDWdith);
		if(gpTlkDrvCodecSpkBuffer == nullptr || gTlkDrvCodecSpkBuffLen == 0 || spkSrc == 0xFF || spkSRate == 0){
			return -TLK_EPARAM;
		}
		tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_icodec_enable: 003");
		tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_icodec_enable: <SPK> %d %d %d", sTlkDrvIcodecCtrl.spkBitDepth, 
			sTlkDrvIcodecCtrl.spkChannel, sTlkDrvIcodecCtrl.spkSampleRate);
	}
	if(enMic){
		if(sTlkDrvIcodecCtrl.micBitDepth == 16) micDWdith = CODEC_BIT_16_DATA;
		else if(sTlkDrvIcodecCtrl.micBitDepth == 20) micDWdith = CODEC_BIT_20_DATA;
		if(micDWdith != 0xFF){
			#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
			if(sTlkDrvIcodecCtrl.micChannel == 0x03){
				micSrc = DMIC_STREAM0_STEREO;
			}else if (sTlkDrvIcodecCtrl.micChannel == 0x02){
				micSrc = DMIC_STREAM0_MONO_R;
			}else{
				micSrc = DMIC_STREAM0_MONO_L;
			}
			#endif
			#if (TLKHW_TYPE == TLKHW_TYPE_C1T293A33_V1_0)
			if(sTlkDrvIcodecCtrl.micChannel == 0x03){
				micSrc = AMIC_STREAM0_STEREO;
			}else if (sTlkDrvIcodecCtrl.micChannel == 0x02){
				micSrc = AMIC_STREAM0_MONO_R;
			}else{
				micSrc = AMIC_STREAM0_MONO_L;
			}
			#endif
		}
		if(sTlkDrvIcodecCtrl.micSampleRate == 8000) micSRate = AUDIO_8K;
		else if(sTlkDrvIcodecCtrl.micSampleRate == 16000) micSRate = AUDIO_16K;
		else if(sTlkDrvIcodecCtrl.micSampleRate == 32000) micSRate = AUDIO_32K;
		else if(sTlkDrvIcodecCtrl.micSampleRate == 44100) micSRate = AUDIO_44P1K;
		else if(sTlkDrvIcodecCtrl.micSampleRate == 48000) micSRate = AUDIO_48K;
		if(gpTlkDrvCodecMicBuffer == nullptr || gTlkDrvCodecMicBuffLen == 0 || micSrc == 0xFF || micSRate == 0){
			return -TLK_EPARAM;
		}
		tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_icodec_enable: <MIC> %d %d %d", sTlkDrvIcodecCtrl.micBitDepth, 
			sTlkDrvIcodecCtrl.micChannel, sTlkDrvIcodecCtrl.micSampleRate);
	}

	audio_codec_init();
	#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
	audio_set_stream0_dmic_pin(TLKDRV_ICODEC_DMIC_DATA_PIN, TLKDRV_ICODEC_DMIC_CLK0_PIN, TLKDRV_ICODEC_DMIC_CLK1_PIN);
	#endif
		
	if(enMic && gpTlkDrvCodecMicBuffer != nullptr && gTlkDrvCodecMicBuffLen != 0){
		audio_codec_stream0_input_t inputParam;
		inputParam.input_src = micSrc;   //The output must be consistent with the number and direction of the input channels, otherwise an exception will occur: INPUT_MONO ==> OUTPUT_MON0 or INPUT_STEREO ==> OUTPUT_STEREO
		inputParam.sample_rate = micSRate;
		inputParam.fifo_num = TLKDRV_CODEC_MIC_FIFO;
		inputParam.data_width = micDWdith;
		inputParam.dma_num = TLKDRV_CODEC_MIC_DMA;
		inputParam.data_buf = gpTlkDrvCodecMicBuffer;
		inputParam.data_buf_size = gTlkDrvCodecMicBuffLen;
		audio_codec_stream0_input_init(&inputParam);
		#if (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
		audio_set_stream0_dig_gain1(CODEC_IN_D_GAIN1_30_DB);
		#endif
		#if (TLKHW_TYPE == TLKHW_TYPE_C1T293A33_V1_0)
		audio_set_stream0_dig_gain1(CODEC_IN_D_GAIN1_18_DB);
		#endif
		audio_rx_dma_chain_init(inputParam.fifo_num,inputParam.dma_num,(unsigned short*)inputParam.data_buf,inputParam.data_buf_size);
		audio_rx_dma_en(TLKDRV_CODEC_MIC_DMA);
	}
	if(enSpk && gpTlkDrvCodecSpkBuffer != nullptr && gTlkDrvCodecSpkBuffLen != 0){
		audio_codec_output_t outputParam;
		outputParam.output_src = spkSrc; //The output must be consistent with the number and direction of the input channels, otherwise an exception will occur: INPUT_MONO ==> OUTPUT_MON0 or INPUT_STEREO ==> OUTPUT_STEREO
		outputParam.sample_rate = spkSRate;
		outputParam.fifo_num = TLKDRV_CODEC_SPK_FIFO;
		outputParam.data_width = spkDWdith;
		outputParam.dma_num = TLKDRV_CODEC_SPK_DMA;
		outputParam.mode = HP_MODE;
		outputParam.data_buf = gpTlkDrvCodecSpkBuffer;
		outputParam.data_buf_size = gTlkDrvCodecSpkBuffLen;
//		outputParam.data_buf = gpTlkDrvCodecMicBuffer;
//		outputParam.data_buf_size = gTlkDrvCodecMicBuffLen;
		audio_codec_stream_output_init(&outputParam);
		audio_tx_dma_chain_init(outputParam.fifo_num,outputParam.dma_num,(unsigned short*)outputParam.data_buf,outputParam.data_buf_size);
		audio_tx_dma_en(TLKDRV_CODEC_SPK_DMA);
	}

	return TLK_ENONE;
}
static void tlkdrv_icodec_disable(void)
{
	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_icodec_disable: 001");

	audio_power_down();
	audio_rx_dma_dis(TLKDRV_CODEC_MIC_DMA);
	audio_tx_dma_dis(TLKDRV_CODEC_SPK_DMA);
}



#endif //#if (TLKDRV_CODEC_ICODEC_ENABLE)
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B92)

