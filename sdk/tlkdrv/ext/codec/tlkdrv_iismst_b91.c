/********************************************************************************************************
 * @file	tlkdrv_iismst_b91.c
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
#if (TLKDRV_CODEC_IISMST_ENABLE)
#include "tlkdrv_iismst_b91.h"
#include "drivers.h"



static bool tlkdrv_iismst_isOpen(uint08 subDev);
static int tlkdrv_iismst_init(uint08 subDev);
static int tlkdrv_iismst_open(uint08 subDev);
static int tlkdrv_iismst_pause(uint08 subDev);
static int tlkdrv_iismst_close(uint08 subDev);
static int tlkdrv_iismst_reset(uint08 subDev);
static int tlkdrv_iismst_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1);

static int tlkdrv_iismst_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);
static int tlkdrv_iismst_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1);

static int  tlkdrv_iismst_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk);
static void tlkdrv_iismst_disable(void);


extern uint16  gTlkDrvCodecSpkBuffLen;
extern uint16  gTlkDrvCodecMicBuffLen;
extern uint08 *gpTlkDrvCodecSpkBuffer;
extern uint08 *gpTlkDrvCodecMicBuffer;
extern audio_i2s_codec_config_t audio_i2s_codec_config;


const tlkdrv_codec_modinf_t gcTlkDrvIisMstInf_b91 = {
	tlkdrv_iismst_isOpen, //IsOpen
	tlkdrv_iismst_init, //Init
	tlkdrv_iismst_open, //Open
	tlkdrv_iismst_pause, //Pause
	tlkdrv_iismst_close, //Close
	tlkdrv_iismst_reset, //Reset
	tlkdrv_iismst_config, //Config
};
static tlkdrv_iismst_t sTlkDrviismstCtrl = {0};

static bool tlkdrv_iismst_isOpen(uint08 subDev)
{
	if(!sTlkDrviismstCtrl.isInit || !sTlkDrviismstCtrl.isOpen) return false;
	if(subDev == TLKDRV_CODEC_SUBDEV_MIC){
		if(sTlkDrviismstCtrl.micIsEn) return true;
		else return false;
	}else if(subDev == TLKDRV_CODEC_SUBDEV_SPK){
		if(sTlkDrviismstCtrl.spkIsEn) return true;
		else return false;
	}else{
		return true;
	}
}
static int tlkdrv_iismst_init(uint08 subDev)
{
	tmemset(&sTlkDrviismstCtrl, 0, sizeof(tlkdrv_iismst_t));

	sTlkDrviismstCtrl.isInit = true;
	sTlkDrviismstCtrl.isOpen = false;
	sTlkDrviismstCtrl.isMute = false;
	sTlkDrviismstCtrl.micVol = 60;
	sTlkDrviismstCtrl.bitDepth = 16;
	sTlkDrviismstCtrl.sampleRate = 48000;
	
	return TLK_ENONE;
}
static int tlkdrv_iismst_open(uint08 subDev)
{
	if(!sTlkDrviismstCtrl.isInit) return -TLK_ESTATUS;
	if(sTlkDrviismstCtrl.isOpen) return -TLK_EREPEAT;
	
	if((subDev & TLKDRV_CODEC_SUBDEV_MIC) != 0) sTlkDrviismstCtrl.micIsEn = true;
	else sTlkDrviismstCtrl.micIsEn = false;
	if((subDev & TLKDRV_CODEC_SUBDEV_SPK) != 0) sTlkDrviismstCtrl.spkIsEn = true;
	else sTlkDrviismstCtrl.spkIsEn = false;
	tlkdrv_iismst_enable(sTlkDrviismstCtrl.bitDepth, sTlkDrviismstCtrl.channel,
		sTlkDrviismstCtrl.sampleRate, sTlkDrviismstCtrl.micIsEn, sTlkDrviismstCtrl.spkIsEn);
	
	sTlkDrviismstCtrl.isOpen = true;
	return TLK_ENONE;
}
static int tlkdrv_iismst_pause(uint08 subDev)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdrv_iismst_close(uint08 subDev)
{
	if(!sTlkDrviismstCtrl.isInit || !sTlkDrviismstCtrl.isOpen) return -TLK_ESTATUS;
	
	tlkdrv_iismst_disable();
	
	sTlkDrviismstCtrl.isOpen = false;
	sTlkDrviismstCtrl.micIsEn = false;
	sTlkDrviismstCtrl.spkIsEn = false;
	
	return TLK_ENONE;
}
static int tlkdrv_iismst_reset(uint08 subDev)
{
	bool isOpen = sTlkDrviismstCtrl.isOpen;
	
	if(isOpen) tlkdrv_iismst_close(subDev);
	if(isOpen) tlkdrv_iismst_open(subDev);
	
	return TLK_ENONE;
}
static int tlkdrv_iismst_config(uint08 subDev, uint08 opcode, uint32 param0, uint32 param1)
{
	int ret = -TLK_ENOSUPPORT;
	switch(opcode){
		case TLKDRV_CODEC_OPCODE_SET_MUTE:  //param:[uint08]isMute
			ret = tlkdrv_iismst_setMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_MUTE:
			ret = tlkdrv_iismst_getMuteDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_VOLUME: //param:[uint08]volume
			ret = tlkdrv_iismst_setVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_VOLUME:
			ret = tlkdrv_iismst_getVolumeDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_CHANNEL: //param:[uint08]chnCnt-1/2/3
			ret = tlkdrv_iismst_setChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_CHANNEL:
			ret = tlkdrv_iismst_getChannelDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_BIT_DEPTH: //param:[uint08]bitDepth-8,16,20,24,32
			ret = tlkdrv_iismst_setBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_BIT_DEPTH:
			ret = tlkdrv_iismst_getBitDepthDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_SET_SAMPLE_RATE: //param:[uint32]bitDepth-8,16,20,24,32
			ret = tlkdrv_iismst_setSampleRateDeal(subDev, param0, param1);
			break;
		case TLKDRV_CODEC_OPCODE_GET_SAMPLE_RATE:
			ret = tlkdrv_iismst_getSampleRateDeal(subDev, param0, param1);
			break;
	}
	return ret;
}

static int tlkdrv_iismst_setMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 mute = param0 & 0xFF;
	if(mute != 0x00 && mute != 0x01) return -TLK_EPARAM;
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	sTlkDrviismstCtrl.isMute = mute;
	return TLK_ENONE;
}
static int tlkdrv_iismst_getMuteDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	return sTlkDrviismstCtrl.isMute;
}
static int tlkdrv_iismst_setVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 micVol = param0 & 0xFF;
	if(micVol > 100) return -TLK_EPARAM;
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	sTlkDrviismstCtrl.micVol = micVol;
	return TLK_ENONE;
}
static int tlkdrv_iismst_getVolumeDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	if(subDev != TLKDRV_CODEC_SUBDEV_MIC) return -TLK_ENOSUPPORT;
	return sTlkDrviismstCtrl.micVol;
}
static int tlkdrv_iismst_setChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 channel = param0 & 0xFF;
	if(channel != 0x01 && channel != 0x02 && channel != 0x03) return -TLK_EPARAM;
	sTlkDrviismstCtrl.channel = channel;
	return TLK_ENONE;
}
static int tlkdrv_iismst_getChannelDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrviismstCtrl.channel;
}
static int tlkdrv_iismst_setBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint08 bitDapth = param0 & 0xFF;
	if(bitDapth != 8 && bitDapth != 16 && bitDapth != 20 && bitDapth != 24 && bitDapth != 32){
		return -TLK_EPARAM;
	}
	if(bitDapth != 16) return -TLK_ENOSUPPORT;
	sTlkDrviismstCtrl.bitDepth = 16;
	return TLK_ENONE;
}
static int tlkdrv_iismst_getBitDepthDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrviismstCtrl.bitDepth;
}
static int tlkdrv_iismst_setSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	uint32 sampleRate = param0;
	if(sampleRate != 8000 && sampleRate != 16000 && sampleRate != 32000 && sampleRate != 44100 && sampleRate != 48000){
		return -TLK_EPARAM;
	}
	sTlkDrviismstCtrl.sampleRate = sampleRate;
	return TLK_ENONE;
}
static int tlkdrv_iismst_getSampleRateDeal(uint08 subDev, uint32 param0, uint32 param1)
{
	return sTlkDrviismstCtrl.sampleRate;
}


extern audio_i2s_invert_config_t audio_i2s_invert_config;
static int tlkdrv_iismst_enable(uint08 bitDepth, uint08 channel, uint32 sampleRate, bool enMic, bool enSpk)
{
	audio_sample_rate_e rate;
	audio_in_mode_e ain0_mode;
	audio_in_mode_e ain1_mode;
	audio_out_mode_e i2s_aout_mode;
	i2s_data_select_e bitData;

	if(bitDepth != 16 && bitDepth != 24 && channel != 0x01 && channel != 0x02 && channel != 0x03){
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
			ain0_mode = BIT_20_OR_24_STEREO;
			ain1_mode = BIT_20_OR_24_STEREO;
			i2s_aout_mode = BIT_20_OR_24_STEREO_FIFO0;
		}else if(channel == 0x01 || channel == 0x02){
			bitDepth = MONO_BIT_24;
			ain0_mode = BIT_20_OR_24_MONO;
			ain1_mode = BIT_20_OR_24_MONO;
			i2s_aout_mode = BIT_20_OR_24_MONO_FIFO0;
		}else{
			return -TLK_ENOSUPPORT;
		}
		bitData = I2S_BIT_24_DATA;
	}else if(bitDepth == 16){
		if(channel == 0x03){
			bitDepth = STEREO_BIT_16;
			ain0_mode = BIT_16_STEREO;
			ain1_mode = BIT_16_STEREO;
			i2s_aout_mode = BIT_16_STEREO_FIFO0;
		}else if(channel == 0x01 || channel == 0x02){
			bitDepth = MONO_BIT_16;
			ain0_mode = BIT_16_MONO;
			ain1_mode = BIT_16_MONO;
			i2s_aout_mode = BIT_16_MONO_FIFO0;
		}else{
			return -TLK_ENOSUPPORT;
		}
		bitData = I2S_BIT_16_DATA;
	}else{
		return -TLK_ENOSUPPORT;
	}
	
	
	audio_reset();
	codec_reset();
	dma_chn_dis(TLKDRV_CODEC_MIC_DMA);
	dma_chn_dis(TLKDRV_CODEC_SPK_DMA);

	gpio_function_en(I2S_BCK_PC3);	
	gpio_function_en(I2S_ADC_LR_PC4);
	gpio_function_en(I2S_ADC_DAT_PC5);
	gpio_function_en(I2S_DAC_LR_PC6);
	gpio_function_en(I2S_DAC_DAT_PC7);
//	audio_i2s_init(0, 0, 0);
	audio_i2s_set_pin();
	audio_set_chn_wl(bitDepth);
	audio_mux_config(IO_I2S, ain0_mode, ain1_mode, i2s_aout_mode);
	audio_i2s_config(I2S_I2S_MODE, bitData, I2S_M_CODEC_S, &audio_i2s_invert_config);
	audio_set_i2s_clock(rate, AUDIO_RATE_EQUAL, 0);
	audio_clk_en(1,1);
	audio_data_fifo0_path_sel(I2S_DATA_IN_FIFO, I2S_OUT);

	audio_rx_dma_chain_init(TLKDRV_CODEC_MIC_DMA, (uint16*)gpTlkDrvCodecMicBuffer, gTlkDrvCodecMicBuffLen);
	audio_tx_dma_chain_init(TLKDRV_CODEC_SPK_DMA, (uint16*)gpTlkDrvCodecSpkBuffer, gTlkDrvCodecSpkBuffLen);

	return TLK_ENONE;
}

static void tlkdrv_iismst_disable(void)
{
	dma_chn_dis(TLKDRV_CODEC_MIC_DMA);
	dma_chn_dis(TLKDRV_CODEC_SPK_DMA);
	
	audio_reset();
	codec_reset();
	audio_codec_adc_power_down();
	audio_codec_dac_power_down();
	
	gpio_function_en(I2S_BCK_PC3);	
	gpio_function_en(I2S_ADC_LR_PC4);
	gpio_function_en(I2S_ADC_DAT_PC5);
	gpio_function_en(I2S_DAC_LR_PC6);
	gpio_function_en(I2S_DAC_DAT_PC7);

	gpio_input_dis(I2S_BCK_PC3);
	gpio_input_dis(I2S_ADC_LR_PC4);
	gpio_input_dis(I2S_ADC_DAT_PC5);
	gpio_input_dis(I2S_DAC_LR_PC6);
	gpio_input_dis(I2S_DAC_DAT_PC7);
	gpio_output_dis(I2S_BCK_PC3);
	gpio_output_dis(I2S_ADC_LR_PC4);
	gpio_output_dis(I2S_ADC_DAT_PC5);
	gpio_output_dis(I2S_DAC_LR_PC6);
	gpio_output_dis(I2S_DAC_DAT_PC7);
}


#endif //#if (TLKDRV_CODEC_IISMST_ENABLE)

#endif // #if (MCU_CORE_TYPE == MCU_CORE_B91)
