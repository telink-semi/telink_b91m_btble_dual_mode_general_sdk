/********************************************************************************************************
 * @file	tlkdrv_iisslv_b92.c
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
#if (TLKDRV_CODEC_IISSLV_ENABLE)
#include "tlkdrv_iisslv_b92.h"
#include "drivers.h"


static bool tlkdrv_iisslv_isOpen(uint08 subDev);
static int tlkdrv_iisslv_init(uint08 subDev);
static int tlkdrv_iisslv_open(uint08 subDev);
static int tlkdrv_iisslv_pause(uint08 subDev);
static int tlkdrv_iisslv_close(uint08 subDev);
static int tlkdrv_iisslv_reset(uint08 subDev);
static int tlkdrv_iisslv_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1);

static int  tlkdrv_iisslv_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk);
static void tlkdrv_iisslv_disable(void);

static int tlkdrv_iisslv_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iisslv_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);


extern uint16  gTlkDrvCodecSpkBuffLen;
extern uint16  gTlkDrvCodecMicBuffLen;
extern uint08 *gpTlkDrvCodecSpkBuffer;
extern uint08 *gpTlkDrvCodecMicBuffer;

extern const uint16 scTlkDrvCodec8kParam[5];
extern const uint16 scTlkDrvCodec16kParam[5];
extern const uint16 scTlkDrvCodec32kParam[5];
extern const uint16 scTlkDrvCodec48kParam[5];
extern const uint16 scTlkDrvCodec44k1Param[5];



const tlkdrv_codec_modinf_t gcTlkDrvIisSlvInf = {
	tlkdrv_iisslv_isOpen, //IsOpen
	tlkdrv_iisslv_init, //Init
	tlkdrv_iisslv_open, //Open
	tlkdrv_iisslv_pause, //Pause
	tlkdrv_iisslv_close, //Close
	tlkdrv_iisslv_reset, //Reset
	tlkdrv_iisslv_config, //Config
};
static tlkdrv_iisslv_t sTlkDrvIisSlvCtrl = {0};

static bool tlkdrv_iisslv_isOpen(uint08 subDev)
{
	if(!sTlkDrvIisSlvCtrl.isInit || !sTlkDrvIisSlvCtrl.isOpen) return false;
	if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		if(sTlkDrvIisSlvCtrl.micIsEn) return true;
		else return false;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		if(sTlkDrvIisSlvCtrl.spkIsEn) return true;
		else return false;
	}else{
		return true;
	}
}
static int tlkdrv_iisslv_init(uint08 subDev)
{
	tmemset(&sTlkDrvIisSlvCtrl, 0, sizeof(tlkdrv_iisslv_t));

	sTlkDrvIisSlvCtrl.isInit = true;
	sTlkDrvIisSlvCtrl.isOpen = false;
	sTlkDrvIisSlvCtrl.isMute = false;
	sTlkDrvIisSlvCtrl.micVol = 60;
	sTlkDrvIisSlvCtrl.bitDepth = 16;
	sTlkDrvIisSlvCtrl.sampleRate = 48000;
			
	return TLK_ENONE;
}
static int tlkdrv_iisslv_open(uint08 subDev)
{
	if(!sTlkDrvIisSlvCtrl.isInit) return -TLK_ESTATUS;
	if(sTlkDrvIisSlvCtrl.isOpen) return -TLK_EREPEAT;
	
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0) sTlkDrvIisSlvCtrl.micIsEn = true;
	else sTlkDrvIisSlvCtrl.micIsEn = false;
	if((subDev & TLKDRV_CODEC_SUBDEV_SPK) != 0) sTlkDrvIisSlvCtrl.spkIsEn = true;
	else sTlkDrvIisSlvCtrl.spkIsEn = false;
	tlkdrv_iisslv_enable(sTlkDrvIisSlvCtrl.bitDepth, sTlkDrvIisSlvCtrl.channel,
		sTlkDrvIisSlvCtrl.sampleRate, sTlkDrvIisSlvCtrl.micIsEn, sTlkDrvIisSlvCtrl.spkIsEn);
	
	sTlkDrvIisSlvCtrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_iisslv_pause(uint08 subDev)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_iisslv_close(uint08 subDev)
{
	if(!sTlkDrvIisSlvCtrl.isInit || !sTlkDrvIisSlvCtrl.isOpen) return -TLK_ESTATUS;
	
	tlkdrv_iisslv_disable();
	
	sTlkDrvIisSlvCtrl.isOpen = false;
	sTlkDrvIisSlvCtrl.micIsEn = false;
	sTlkDrvIisSlvCtrl.spkIsEn = false;
	
	return TLK_ENONE;
}
static int tlkdrv_iisslv_reset(uint08 subDev)
{
	bool isOpen = sTlkDrvIisSlvCtrl.isOpen;
	
	if(isOpen) tlkdrv_iisslv_close(subDev);
	if(isOpen) tlkdrv_iisslv_open(subDev);
	
	return TLK_ENONE;
}
static int tlkdrv_iisslv_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	int ret = -TLK_ENOSUPPORT;
	switch(opcode){
		case TLKDRV_CODEC_OPCODE_SET_MUTE:  //param:[uint08]isMute
			ret = tlkdrv_iisslv_setMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_MUTE:
			ret = tlkdrv_iisslv_getMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_VOLUME: //param:[uint08]volume
			ret = tlkdrv_iisslv_setVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_VOLUME:
			ret = tlkdrv_iisslv_getVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_CHANNEL: //param:[uint08]chnCnt-1/2/3
			ret = tlkdrv_iisslv_setChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_CHANNEL:
			ret = tlkdrv_iisslv_getChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH: //param:[uint08]bitDepth-8,16,20,24,32
			ret = tlkdrv_iisslv_setBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_BIT_DEPTH:
			ret = tlkdrv_iisslv_getBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE: //param:[uint32]bitDepth-8,16,20,24,32
			ret = tlkdrv_iisslv_setSampleRateDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE:
			ret = tlkdrv_iisslv_getSampleRateDeal(subDev, param0, param1);
			break;
	}
	return ret;
}

static int tlkdrv_iisslv_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 mute = param0 & 0xFF;
	if(mute != 0x00 && mute != 0x01) return -TLK_EPARAM;
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	sTlkDrvIisSlvCtrl.isMute = mute;
	return TLK_ENONE;
}
static int tlkdrv_iisslv_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	return sTlkDrvIisSlvCtrl.isMute;
}
static int tlkdrv_iisslv_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 micVol = param0 & 0xFF;
	if(micVol > 100) return -TLK_EPARAM;
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	sTlkDrvIisSlvCtrl.micVol = micVol;
	return TLK_ENONE;
}
static int tlkdrv_iisslv_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	return sTlkDrvIisSlvCtrl.micVol;
}
static int tlkdrv_iisslv_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 channel = param0 & 0xFF;
	if(channel != 0x01 && channel != 0x02 && channel != 0x03) return -TLK_EPARAM;
	sTlkDrvIisSlvCtrl.channel = channel;
	return TLK_ENONE;
}
static int tlkdrv_iisslv_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIisSlvCtrl.channel;
}
static int tlkdrv_iisslv_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 bitDapth = param0 & 0xFF;
	if(bitDapth != 16 && bitDapth != 20 && bitDapth != 24){
		return -TLK_ENOSUPPORT;
	}
	sTlkDrvIisSlvCtrl.bitDepth = bitDapth;
	return TLK_ENONE;
}
static int tlkdrv_iisslv_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIisSlvCtrl.bitDepth;
}
static int tlkdrv_iisslv_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint32 sampleRate = param0;
	if(sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000 && sampleRate != 44100 && sampleRate != 48000){
		return -TLK_EPARAM;
	}
	sTlkDrvIisSlvCtrl.sampleRate = sampleRate;
	return TLK_ENONE;
}
static int tlkdrv_iisslv_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrvIisSlvCtrl.sampleRate;
}


static int tlkdrv_iisslv_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk)
{
	uint08 dataWdith = 0xFF;
	uint08 drvChannel = 0xFF;
	i2s_pin_config_t pinConfig = {
		.bclk_pin   	= TLKDRV_IISSLV_BCLIK_PIN,
		.adc_lr_clk_pin = TLKDRV_IISSLV_ADC_CLK_PIN,
		.adc_dat_pin    = TLKDRV_IISSLV_ADC_DAT_PIN,
		.dac_lr_clk_pin = TLKDRV_IISSLV_DAC_CLK_PIN,
		.dac_dat_pin    = TLKDRV_IISSLV_DAC_DAT_PIN,
	};
	audio_i2s_config_t iisConfig = {
		.i2s_select = TLKDRV_IISSLV_CHN,
		.i2s_mode = I2S_I2S_MODE,
		.pin_config = &pinConfig,
		.data_width = I2S_BIT_16_DATA,
		.master_slave_mode = I2S_AS_SLAVE_EN,
		.sample_rate = (uint16*)scTlkDrvCodec32kParam,
	};
	const uint16 *pSampleParam = nullptr;

	if(!enMic && !enSpk){
		tlkapi_error(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_iisslv_enable: mic or spk is all disable");
		return -TLK_EPARAM;
	}

	if(sampleRate == 8000) pSampleParam = scTlkDrvCodec8kParam;
	else if(sampleRate == 16000) pSampleParam = scTlkDrvCodec16kParam;
	else if(sampleRate == 32000) pSampleParam = scTlkDrvCodec32kParam;
	else if(sampleRate == 44100) pSampleParam = scTlkDrvCodec44k1Param;
	else if(sampleRate == 48000) pSampleParam = scTlkDrvCodec48kParam;
	if(bitDepth == 16) dataWdith = I2S_BIT_16_DATA;
	else if(bitDepth == 20) dataWdith = I2S_BIT_20_DATA;
	else if(bitDepth == 24) dataWdith = I2S_BIT_24_DATA;
	if(channel == 0x03) drvChannel = I2S_STEREO;
	else drvChannel = I2S_MONO;
	if(pSampleParam == nullptr || dataWdith == 0xFF){
		tlkapi_error(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_iisslv_enable: sampleRate or bitDepth is not supported");
		return -TLK_EPARAM;
	}

	iisConfig.data_width = dataWdith;
	iisConfig.sample_rate = (uint16*)pSampleParam;
		
	audio_power_on();
	audio_i2s_config_init(&iisConfig);
	
	if(enMic && gpTlkDrvCodecMicBuffer != nullptr && gTlkDrvCodecMicBuffLen != 0){
		audio_i2s_input_output_t inputParam = {
			.i2s_select = iisConfig.i2s_select,
			.data_width = dataWdith,
			.i2s_ch_sel = drvChannel,
			.fifo_chn = TLKDRV_CODEC_MIC_FIFO,
			.dma_num = TLKDRV_CODEC_MIC_DMA,
			.data_buf = gpTlkDrvCodecMicBuffer,
			.data_buf_size = gTlkDrvCodecMicBuffLen,
		};
		if(gpTlkDrvCodecSpkBuffer == nullptr || gTlkDrvCodecSpkBuffLen == 0){
			tlkapi_error(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_iisslv_enable: spk not ready");
			return -TLK_EPARAM;
		}
		audio_i2s_input_init(&inputParam);
		audio_rx_dma_chain_init(inputParam.fifo_chn,inputParam.dma_num,(unsigned short*)inputParam.data_buf,inputParam.data_buf_size);
		audio_rx_dma_en(inputParam.dma_num);
	}
	if(enSpk && gpTlkDrvCodecSpkBuffer != nullptr && gTlkDrvCodecSpkBuffLen != 0){
		audio_i2s_input_output_t outputParam = {
			.i2s_select = iisConfig.i2s_select,
			.data_width = dataWdith,
			.i2s_ch_sel = drvChannel,
			.fifo_chn = TLKDRV_CODEC_SPK_FIFO,
			.dma_num = TLKDRV_CODEC_SPK_DMA,
			.data_buf = gpTlkDrvCodecSpkBuffer,
			.data_buf_size = gTlkDrvCodecSpkBuffLen,
		};
		if(gpTlkDrvCodecMicBuffer == nullptr || gTlkDrvCodecMicBuffLen == 0){
			tlkapi_error(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_iisslv_enable: mic not ready");
			return -TLK_EPARAM;
		}
		audio_i2s_output_init(&outputParam);
		audio_tx_dma_chain_init(outputParam.fifo_chn,outputParam.dma_num,(unsigned short*)outputParam.data_buf,outputParam.data_buf_size);
		audio_tx_dma_en(outputParam.dma_num);
	}

	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN,
		"tlkdrv_iisslv_enable: bitDepth[%d], channel[%d], sampleRate[%d]", 
		sTlkDrvIisSlvCtrl.bitDepth, sTlkDrvIisSlvCtrl.channel, sTlkDrvIisSlvCtrl.sampleRate);
	
	return TLK_ENONE;
}
static void tlkdrv_iisslv_disable(void)
{
	tlkapi_trace(TLKDRV_CODEC_DBG_FLAG, TLKDRV_CODEC_DBG_SIGN, "tlkdrv_iisslv_disable: 001");
	
	audio_power_down();
	audio_rx_dma_dis(TLKDRV_CODEC_MIC_DMA);
	audio_tx_dma_dis(TLKDRV_CODEC_SPK_DMA);
}




#endif //#if (TLKDRV_CODEC_IISSLV_ENABLE)

#endif //#if (MCU_CORE_TYPE == MCU_CORE_B92)
