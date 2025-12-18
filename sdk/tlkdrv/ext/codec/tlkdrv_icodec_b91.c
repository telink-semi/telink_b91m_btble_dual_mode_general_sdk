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
#if (MCU_CORE_TYPE == MCU_CORE_B91)
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdrv_codec.h"
#if (TLKDRV_CODEC_ICODEC_ENABLE)
#include "tlkdrv_icodec_b91.h"
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

static int tlkdrv_icodec_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk);
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
	sTlkDrvIcodecCtrl.isMute = false;
	sTlkDrvIcodecCtrl.micVol = 100;
	sTlkDrvIcodecCtrl.channel = 0x01;
	sTlkDrvIcodecCtrl.bitDepth = 16;
	sTlkDrvIcodecCtrl.sampleRate = 48000;
	
	return TLK_ENONE;
}
static int tlkdrv_icodec_open(uint08 subDev)
{
	if(!sTlkDrvIcodecCtrl.isInit) return -TLK_ESTATUS;
	if(sTlkDrvIcodecCtrl.isOpen) return -TLK_EREPEAT;
	
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0) sTlkDrvIcodecCtrl.micIsEn = true;
	else sTlkDrvIcodecCtrl.micIsEn = false;
	if((subDev & TLKDRV_CODEC_SUBDEV_SPK) != 0) sTlkDrvIcodecCtrl.spkIsEn = true;
	else sTlkDrvIcodecCtrl.spkIsEn = false;

	tlkdrv_icodec_enable(sTlkDrvIcodecCtrl.bitDepth, sTlkDrvIcodecCtrl.channel,
		sTlkDrvIcodecCtrl.sampleRate, sTlkDrvIcodecCtrl.micIsEn, sTlkDrvIcodecCtrl.spkIsEn);

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
	uint08 mute = param0 & 0xFF;
	if(mute != 0x00 && mute != 0x01) return -TLK_EPARAM;
	sTlkDrvIcodecCtrl.isMute = mute;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.isMute;
}
static int tlkdrv_icodec_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 micVol = param0 & 0xFF;
	if(micVol > 100) return -TLK_EPARAM;
	sTlkDrvIcodecCtrl.micVol = micVol;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.micVol;
}
static int tlkdrv_icodec_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 channel = param0 & 0xFF;
	if(channel != 0x01 && channel != 0x02 && channel != 0x03) return -TLK_EPARAM;
	sTlkDrvIcodecCtrl.channel = channel;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.channel;
}
static int tlkdrv_icodec_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 bitDapth = param0 & 0xFF;
	if(bitDapth != 8 && bitDapth != 16 && bitDapth != 20 && bitDapth != 24 && bitDapth != 32){
		return -TLK_EPARAM;
	}
	if(bitDapth != 16) return -TLK_ENOSUPPORT;
	sTlkDrvIcodecCtrl.bitDepth = 16;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.bitDepth;
}
static int tlkdrv_icodec_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint32 sampleRate = param0;
	if(sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000 && sampleRate != 44100 && sampleRate != 48000){
		return -TLK_EPARAM;
	}
	sTlkDrvIcodecCtrl.sampleRate = sampleRate;
	return TLK_ENONE;
}
static int tlkdrv_icodec_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIcodecCtrl.sampleRate;
}



static int tlkdrv_icodec_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk)
{
	audio_sample_rate_e rate;
//	audio_in_mode_e ain0_mode;
//	audio_in_mode_e ain1_mode;
//	audio_out_mode_e i2s_aout_mode;
//	i2s_data_select_e bitData;

	if(bitDepth != 16 && bitDepth != 24 && bitDepth != 20 && channel != 0x01 && channel != 0x02 && channel != 0x03){
		return -TLK_ENOSUPPORT;
	}
	if(sampleRate == 8000){
		rate = AUDIO_8K;
	}else if(sampleRate == 16000){
		rate = AUDIO_16K;
	}else if(sampleRate == 24000){
		rate = AUDIO_24K;
	}else if(sampleRate == 32000){
		rate = AUDIO_32K;
	}else if(sampleRate == 44100){
		rate = AUDIO_44EP1K;
	}else if(sampleRate == 48000){
		rate = AUDIO_48K;
	}else{
		return -TLK_ENOSUPPORT;
	}
	if(bitDepth == 24){
		if(channel == 0x03){
			bitDepth = STEREO_BIT_24;
//			ain0_mode = BIT_20_OR_24_STEREO;
//			ain1_mode = BIT_20_OR_24_STEREO;
//			i2s_aout_mode = BIT_20_OR_24_STEREO_FIFO0;
		}else if(channel == 0x01 || channel == 0x02){
			bitDepth = MONO_BIT_24;
//			ain0_mode = BIT_20_OR_24_MONO;
//			ain1_mode = BIT_20_OR_24_MONO;
//			i2s_aout_mode = BIT_20_OR_24_MONO_FIFO0;
		}else{
			return -TLK_ENOSUPPORT;
		}
//		bitData = I2S_BIT_24_DATA;
	}else if(bitDepth == 16){
		if(channel == 0x03){
			bitDepth = STEREO_BIT_16;
//			ain0_mode = BIT_16_STEREO;
//			ain1_mode = BIT_16_STEREO;
//			i2s_aout_mode = BIT_16_STEREO_FIFO0;
		}else if(channel == 0x01 || channel == 0x02){
			bitDepth = MONO_BIT_16;
//			ain0_mode = BIT_16_MONO;
//			ain1_mode = BIT_16_MONO;
//			i2s_aout_mode = BIT_16_MONO_FIFO0;
		}else{
			return -TLK_ENOSUPPORT;
		}
//		bitData = I2S_BIT_16_DATA;
	}else{
		return -TLK_ENOSUPPORT;
	}
	
//	tlkapi_sendU32s(TLKDRV_CODEC_DEBUG_ENABLE, "tlkdrv_icodec_enable: 002", bitDepth, channel, sampleRate, 0);
	
	audio_reset();
	codec_reset();
	dma_chn_dis(TLKDRV_CODEC_MIC_DMA);
	dma_chn_dis(TLKDRV_CODEC_SPK_DMA);

//	gpio_function_en(I2S_BCK_PC3);	
//	gpio_function_en(I2S_ADC_LR_PC4);
//	gpio_function_en(I2S_ADC_DAT_PC5);
//	gpio_function_en(I2S_DAC_LR_PC6);
//	gpio_function_en(I2S_DAC_DAT_PC7);
	audio_init(AMIC_IN_TO_BUF_TO_LINE_OUT, rate, bitDepth);
	audio_rx_dma_chain_init(TLKDRV_CODEC_MIC_DMA, (uint16*)gpTlkDrvCodecMicBuffer, gTlkDrvCodecMicBuffLen);
	audio_tx_dma_chain_init(TLKDRV_CODEC_SPK_DMA, (uint16*)gpTlkDrvCodecSpkBuffer, gTlkDrvCodecSpkBuffLen);

//	audio_set_codec_supply(CODEC_2P8V);

	return TLK_ENONE;
}

static void tlkdrv_icodec_disable(void)
{	
	dma_chn_dis(TLKDRV_CODEC_MIC_DMA);
	dma_chn_dis(TLKDRV_CODEC_SPK_DMA);
	
	audio_reset();
	codec_reset();
	audio_codec_adc_power_down();
	audio_codec_dac_power_down();
}


#endif //#if (TLKDRV_CODEC_ICODEC_ENABLE)
#endif //#if (MCU_CORE_TYPE == MCU_CORE_B91)

