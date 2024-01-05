/********************************************************************************************************
 * @file	tlkalg_eq.c
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
#if 1//(TLK_ALG_EQ_ENABLE)
#include <nds_filtering_math.h>
#include <math.h>
#include "drivers.h"
#include "tlkalg_eq.h"
#include "tlkstk/inner/tlkstk_utility.h"


typedef struct _eq_para{
	uint8_t eq_nstage;      	///	EQ number of stage.
	uint8_t eq_sample_rate;	   	/// 0:48K ;1:44.1K ;2: 16K
	uint8_t eq_channel;			/// 0:left   1:right
	uint8_t eq_type;			/// 0:TLKALG_EQ_TYPE_MUSIC  1:TLKALG_EQ_TYPE_VOICE_MIC  2:TLKALG_EQ_TYPE_VOICE_SPK
}__attribute__ ((__packed__)) eq_para_t;

typedef struct _eq_mode_para{
	signed short gain;					///Gain range(BBEE/100)		: 	-327.68db ~ +327.67db
	unsigned short freq_c;				///Center frequency range 	:	20~24000
	unsigned short filter_q;			///Q value range(HHGG/1000)	:	0~65.535
	unsigned char filter_type;			///filter type(1~8)			:
	unsigned char cvsd;					///reserved
}eq_mode_para_t;

typedef struct _eq_mode_header{
	unsigned char idx;
	unsigned char max_gain_num;
	unsigned char filter_sum;
	unsigned char all_gain;					///Full-band gain reduction range:0~25.6
}eq_mode_header_t;

typedef struct _eq_mode{
	eq_mode_header_t eq_header;					/// 4 bytes
	eq_mode_para_t eq_para[TLKALG_EQ_FILTER_MAX];		/// 8 + 9 bytes
	int CRC;									/// 4 bytes
}eq_mode_t;

void eq_proc_new(nds_bq_df1_f32_t *para, signed short *ps, signed short *pd, unsigned int nsample, unsigned char shift);
void eq_chg_work(void);
void set_super_listen_mode(uint08 super_listen);
void eq_mode_convert(uint08 *eq_data, eq_mode_t *eq_filter_data);
void eq_state_reset_new_all(void);
void eq_type_get_coef(int sample_rate, TLKALG_EQ_FILTER_TYPE_ENUM type, int freq, float q, float db, float *coef_out);
void eq_coef_create(int sample_rate,eq_mode_t *filter, float *coeff);


//static int eq_mode = 0x00;
static unsigned short eq_run_state = 0x0000;
static eq_mode_t eq_filter;
static eq_mode_t eq_run_filter;
static int eq_voice_stat = 0;
static eq_mode_t eq_filter_voice_mic;
static eq_mode_t eq_filter_voice_spk;
static float32_t coeff_eq_now[TLKALG_EQ_FILTER_MAX*5];
static float32_t coeff_Voice_mic_eq_now[TLKALG_EQ_FILTER_VOICE_MAX*5];
static float32_t coeff_Voice_spk_eq_now[TLKALG_EQ_FILTER_VOICE_MAX*5];
static int eq_flash_addr;
static unsigned char eq_save_buff[TLKALG_EQ_TYPE_LENGTH_MAX] = {0};
//volatile static signed short eq_before_gain[8] = {0};
volatile static signed short eq_after_gain[9] = {0};
volatile static int eq_updata_flag = 0;

static int EQSampleRate = 16000;


unsigned short eq_q_default = 700;

const unsigned char eq_voice_mic_default_tab[] =
{
	///ZZ
	0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0xaa, 0x00, 0xe8, 0x03, 0x02, 0x00,
	0x90, 0x01, 0x08, 0x07, 0xc8, 0x00, 0x00, 0x00,
};

const unsigned char eq_voice_spk_default_tab[] =
{
	///highpass lowpass
	0x00, 0x00, 0x02, 0x05,
	0x00, 0x00, 0xb4, 0x00, 0x84, 0x03, 0x02, 0x00,
	0x00, 0x00, 0xb8, 0x0b, 0x84, 0x03, 0x01, 0x00,
};

const unsigned char eq_supper_listening_tab[] =
{
		///EQ5D 07-25
		0x00, 0x00, 0x08, 0x45,
		0x9c, 0xff, 0x78, 0x00, 0xbc, 0x02, 0x05, 0x00,
		0x96, 0x00, 0x58, 0x02, 0xf4, 0x01, 0x05, 0x00,
		0x64, 0x00, 0xa4, 0x06, 0x58, 0x02, 0x00, 0x00,
		0x38, 0xff, 0xd0, 0x07, 0x88, 0x13, 0x00, 0x00,
		0xa8, 0xfd, 0xb8, 0x0b, 0xdc, 0x05, 0x00, 0x00,
		0xbc, 0x02, 0x7c, 0x15, 0xc4, 0x09, 0x00, 0x00,
		0xce, 0xff, 0x70, 0x17, 0xb8, 0x0b, 0x00, 0x00,
		0x58, 0x02, 0x40, 0x1f, 0x88, 0x13, 0x00, 0x00,
};

const unsigned char eq_sys_default_tab[] =
{
		///07014 EQ4C
		0x00, 0x00, 0x09, 0x15,
		0x38, 0xff, 0x14, 0x00, 0xe8, 0x03, 0x02, 0x00,
		0xae, 0xfc, 0x96, 0x00, 0xbc, 0x02, 0x00, 0x00,
		0x70, 0xfe, 0x2c, 0x01, 0xbc, 0x02, 0x00, 0x00,
		0x3e, 0xfe, 0xe8, 0x03, 0xe8, 0x03, 0x00, 0x00,
		0xc2, 0x01, 0xc4, 0x09, 0xdc, 0x05, 0x00, 0x00,
		0xa2, 0xfe, 0xd8, 0x0e, 0xe8, 0x03, 0x00, 0x00,
		0xa8, 0xfd, 0x88, 0x13, 0xd0, 0x07, 0x00, 0x00,
		0xb4, 0xfb, 0xf4, 0x1a, 0xd0, 0x07, 0x00, 0x00,
		0x2c, 0x01, 0xbc, 0x1b, 0xe8, 0x03, 0x00, 0x00,		//Fc = 7.1kHz
		0x00, 0x00, 0x00, 0x00,								///CRC

};

const unsigned char eq_index_gain[22] = {
		21,//0
		78,//1
		21,//2
		21,//3
		25,//4
		55,//5
		37,//6
		16,//7
		35,//8
		19,//9
		23,//10
		25,//11
		25,//12
		22,//13
		84,//14
		18,//15
		43,//16
		37,//17
		15,//18
		34,//19
		28,//20
		13,//21
};

const unsigned short FREQ_CENTER_TAB[TLKALG_EQ_FILTER_MAX] = {20,150,300,1000,2500,3800,5000,6900,13000};			///07014 EQ4C


eq_para_t g_eq_para = {0, 0, 0, 0};

float32_t state_eq_voice_mic_left[4 * TLKALG_EQ_NSTAGE_VOICE_MIC_MAX] = {0.0};
float32_t state_eq_voice_mic_right[4 * TLKALG_EQ_NSTAGE_VOICE_MIC_MAX] = {0.0};
float32_t state_eq_voice_speaker_left[4 * TLKALG_EQ_NSTAGE_VOICE_SPK_MAX] = {0.0};
float32_t state_eq_voice_speaker_right[4 * TLKALG_EQ_NSTAGE_VOICE_SPK_MAX] = {0.0};

float32_t state_eq_music_left[4 * TLKALG_EQ_NSTAGE_MISIC_SPK_MAX] = {0.0};
float32_t state_eq_music_right[4 * TLKALG_EQ_NSTAGE_MISIC_SPK_MAX] = {0.0};


float32_t coeff_eq_voice_mic_left[5 * TLKALG_EQ_NSTAGE_VOICE_MIC_MAX]={
		0.983146317605274,-1.896277815330497,0.925516368712182,1.896277815330497,-0.908662686317456,//800hz
		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz

		1.382406919700922,-0.529089646178078,0.000170953890723,0.529089646178078,-0.382577873591645,//3000hz

		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz
		};

float32_t coeff_eq_voice_mic_right[5 * TLKALG_EQ_NSTAGE_VOICE_MIC_MAX]={
		0.983146317605274,-1.896277815330497,0.925516368712182,1.896277815330497,-0.908662686317456,//800hz
		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz

		1.382406919700922,-0.529089646178078,0.000170953890723,0.529089646178078,-0.382577873591645,//3000hz

		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz
		};
float32_t coeff_eq_voice_speaker[5 * TLKALG_EQ_NSTAGE_VOICE_SPK_MAX]={
		0.983146317605274,-1.896277815330497,0.925516368712182,1.896277815330497,-0.908662686317456,//800hz
		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz

		1.382406919700922,-0.529089646178078,0.000170953890723,0.529089646178078,-0.382577873591645,//3000hz

		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz
		};

#if !TLKALG_EQ_CFG_SOUNDBAR_EN
float32_t coeff_eq_music[5 * TLKALG_EQ_NSTAGE_MISIC_SPK_MAX]={
		0.641315507164827,-1.276599943182477,0.635576025971789,1.977219207703750,-0.977670827070627,//150hz
		0.983146317605274,-1.896277815330497,0.925516368712182,1.896277815330497,-0.908662686317456,//800hz
		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz
		0.765811941060125,-0.549372341991572,0.549199557503762,0.549372341991572,-0.315011498563887,//8000hz
		1.003711775896785,-1.985834512331222,0.985352001577454,1.985834512331222,-0.989063777474239,//400hz
		1.000661190971452,-1.997706663070697,0.997077914287511,1.997706663070697,-0.997739105258963,//40hz
		1.146341835423319,-1.165202515691840,0.522436558253714,1.165202515691840,-0.668778393677034,//5600hz
		1.081366997900438,-0.199579955157407,0.290135417898591,0.199579955157407,-0.371502415799029,//10000hz
		1.173619595324526,0.621475740306107,-0.015334962023055,-0.621475740306107,-0.158284633301471,//15000hz
};
		
float32_t coeff_eq_music_44p1k[5 * TLKALG_EQ_NSTAGE_MISIC_SPK_MAX]={
		0.641315507164827,-1.276599943182477,0.635576025971789,1.977219207703750,-0.977670827070627,//150hz
		0.983146317605274,-1.896277815330497,0.925516368712182,1.896277815330497,-0.908662686317456,//800hz
		1.018304336102112,-1.823726756975888,0.907922907437080,1.823726756975888,-0.926227243539192,//2300hz
		0.765811941060125,-0.549372341991572,0.549199557503762,0.549372341991572,-0.315011498563887,//8000hz
		1.003711775896785,-1.985834512331222,0.985352001577454,1.985834512331222,-0.989063777474239,//400hz
		1.000661190971452,-1.997706663070697,0.997077914287511,1.997706663070697,-0.997739105258963,//40hz
		1.146341835423319,-1.165202515691840,0.522436558253714,1.165202515691840,-0.668778393677034,//5600hz
		1.081366997900438,-0.199579955157407,0.290135417898591,0.199579955157407,-0.371502415799029,//10000hz
		1.173619595324526,0.621475740306107,-0.015334962023055,-0.621475740306107,-0.158284633301471,//15000hz
 };

#else

float32_t coeff_eq_music[5 * nstage_eq_music]={

		0.4674666667,		-0.8063800000,		0.4670459467,				1.6149000000,		-0.6775000000,
		0.6000000000,		-0.9928200000,		0.5960400000,				1.3206000000,		-0.4186000000,
		0.2000000000,		-0.1928000000,		0.1605000000,				1.6741000000,		-0.7791000000,
		0.0500000000,		0.0235500000,		0.0341800000,				1.7362000000,		-0.8802000000,
		0.0100000000,		0.0162460000,		0.0066790000,				1.7957000000,		-0.9627000000,

};
		
float32_t coeff_eq_music_44p1k[5 * nstage_eq_music]={

		1,-1.9963937997818,0.996444463729858,1.9963937997818,-0.996444463729858,
		1,-1.70224785804749,0.831227481365204,1.70224785804749,-0.831227481365204,
		1,-1.25303387641907,0.7093386054039,1.25303387641907,-0.7093386054039,
		1,-0.713701367378235,0.632847130298615,0.713701367378235,-0.632847130298615,
		1,-0.131019100546837,0.601073980331421,0.131019100546837,-0.601073980331421,

};
#endif

nds_bq_df1_f32_t instance_eq_music_left = {TLKALG_EQ_NSTAGE_MISIC_SPK, state_eq_music_left, coeff_eq_music_44p1k};
nds_bq_df1_f32_t instance_eq_music_right = {TLKALG_EQ_NSTAGE_MISIC_SPK, state_eq_music_right, coeff_eq_music_44p1k};


nds_bq_df1_f32_t instance_eq_voice_mic_left = {TLKALG_EQ_NSTAGE_VOICE_MIC, state_eq_voice_mic_left, coeff_eq_voice_mic_left};
nds_bq_df1_f32_t instance_eq_voice_mic_right = {TLKALG_EQ_NSTAGE_VOICE_MIC, state_eq_voice_mic_right, coeff_eq_voice_mic_right};

nds_bq_df1_f32_t instance_eq_voice_speaker_left = {TLKALG_EQ_NSTAGE_VOICE_SPK, state_eq_voice_speaker_left, coeff_eq_voice_speaker};
nds_bq_df1_f32_t instance_eq_voice_speaker_right = {TLKALG_EQ_NSTAGE_VOICE_SPK, state_eq_voice_speaker_right, coeff_eq_voice_speaker};


////////////////////////////////////////////new EQ mode////////////////////////////////////////////////////////
void tlkalg_eq_setSampleRate(int samplerate, int type)//should be allocated in switch function
{
	EQSampleRate = samplerate;
	eq_state_reset_new_all();
	switch(type)
	{
		case TLKALG_EQ_TYPE_VOICE_SPK: 
			eq_coef_create(EQSampleRate, &eq_filter_voice_spk, coeff_Voice_spk_eq_now);
			break;
		case TLKALG_EQ_TYPE_VOICE_MIC:
			eq_coef_create(EQSampleRate, &eq_filter_voice_mic, coeff_Voice_mic_eq_now);
			break;
		case TLKALG_EQ_TYPE_MUSIC:
			eq_coef_create(EQSampleRate, &eq_run_filter, coeff_eq_now);
			break;
		default:
			break;
	}
}

_attribute_ram_code_ void eq_state_reset_new_all(void)
{
//	my_dump_str_u32s (1, "eq_state_reset_all", 0, 0, 0, 0);

	tmemset (&state_eq_music_left, 0, sizeof (state_eq_music_left));
	tmemset (&state_eq_music_right, 0, sizeof (state_eq_music_right));

	tmemset (&state_eq_voice_mic_left, 0, sizeof (state_eq_voice_mic_left));
	tmemset (&state_eq_voice_mic_right, 0, sizeof (state_eq_voice_mic_right));

	tmemset (&state_eq_voice_speaker_left, 0, sizeof (state_eq_voice_speaker_left));
	tmemset (&state_eq_voice_speaker_right, 0, sizeof (state_eq_voice_speaker_right));
}

void tlkalg_eq_dataProcess(int type, int chn, int sample_rate, sint16* ps, int samples)
{
//	int nstage;
//	if(chn == TLKALG_EQ_CHANNEL_LEFT) nstage = instance_eq_music_left.nstage;
//	else nstage = instance_eq_music_right.nstage;

	if(type == TLKALG_EQ_TYPE_VOICE_MIC||type == TLKALG_EQ_TYPE_VOICE_SPK)
	{
		if(type == TLKALG_EQ_TYPE_VOICE_SPK)
		{
			if(chn)
			{
				instance_eq_voice_speaker_right.coeff = coeff_Voice_spk_eq_now;
				instance_eq_voice_speaker_right.nstage = eq_filter_voice_spk.eq_header.filter_sum;
				instance_eq_voice_speaker_right.state = state_eq_voice_speaker_right;
				eq_proc_new(&instance_eq_voice_speaker_right, ps, ps, samples, 0);
			}
			else
			{
				instance_eq_voice_speaker_left.coeff = coeff_Voice_spk_eq_now;
				instance_eq_voice_speaker_left.nstage = eq_filter_voice_spk.eq_header.filter_sum;
				instance_eq_voice_speaker_left.state = state_eq_voice_speaker_left;
				eq_proc_new(&instance_eq_voice_speaker_left, ps, ps, samples, 0);
			}
		}

		if(type == TLKALG_EQ_TYPE_VOICE_MIC)
		{
			instance_eq_voice_mic_left.coeff = coeff_Voice_mic_eq_now;
			instance_eq_voice_mic_left.nstage = eq_filter_voice_mic.eq_header.filter_sum;
			instance_eq_voice_mic_left.state = state_eq_voice_mic_left;
			eq_proc_new(&instance_eq_voice_mic_left, ps, ps, samples, 0);
		}
	}
	else if( type == TLKALG_EQ_TYPE_MUSIC)
	{
		eq_chg_work();
		if(chn)
		{
			instance_eq_music_right.coeff = coeff_eq_now;
			instance_eq_music_right.nstage = eq_run_filter.eq_header.filter_sum;
			instance_eq_music_right.state = state_eq_music_right;
			eq_proc_new(&instance_eq_music_right, ps, ps, samples, 0);
		}
		else
		{
			instance_eq_music_left.coeff = coeff_eq_now;
			instance_eq_music_left.nstage = eq_run_filter.eq_header.filter_sum;
			instance_eq_music_left.state = state_eq_music_left;
			eq_proc_new(&instance_eq_music_left, ps, ps, samples, 0);
		}
	}
}
int  tlkalg_eq_procss(int type, int chn, int sampleRate, sint16 *pData, int samples)
{
	if(samples > 64) return -TLK_EPARAM;

	if(sampleRate != EQSampleRate){
		tlkalg_eq_setSampleRate(sampleRate, type);
	}
	
	if(chn != TLKALG_EQ_CHANNEL_STEREO){
		tlkalg_eq_dataProcess(type, chn,  sampleRate, pData, samples);
	}else{
		uint08 index;
		sint16 lChnData[64];
		sint16 rChnData[64];
		for(index=0; index<samples; index++){
			lChnData[index] = *pData++;
			rChnData[index] = *pData++;
		}
		tlkalg_eq_dataProcess(type, TLKALG_EQ_CHANNEL_LEFT,  sampleRate, lChnData, samples);
		tlkalg_eq_dataProcess(type, TLKALG_EQ_CHANNEL_RIGHT, sampleRate, rChnData, samples);
		for(index=0; index<samples; index++){
			*pData++ = lChnData[index];
			*pData++ = rChnData[index];
		}
	}
		
	return TLK_ENONE;
}


void eq_mode_convert(unsigned char *eq_data,eq_mode_t *eq_filter_data)
{
	unsigned char *p =  eq_data;
	unsigned char AllGain;
	signed short	temp=0;
	signed short	G2_5k = ((*(p+4))-120),
					G3_8k = ((*(p+5))-120),
					G1k	  = ((*(p+3))-120);
	eq_mode_t *eq_default_tp = (eq_mode_t*)eq_sys_default_tab;

//	my_dump_str_data (TLKALG_EQ_DEBUG_ENABLE, "CHANGE_EQ_data", eq_data, 10);

//	eq_filter_data->eq_header.filter_sum = TLKALG_EQ_FILTER_ANKER_MAX;

#if 1
	///Sound effect stacking gain value (0.1db)
	for(int i = 0;i<TLKALG_EQ_FILTER_ANKER_MAX;i++)
	{
		temp = *p++;
//		my_dump_str_u32s(1, "delta gain", i, temp,0,0);
		eq_filter_data->eq_para[i].gain = eq_default_tp->eq_para[i].gain + (temp-120)*10;
	}

	if(G2_5k >= 20)
	{
		if(G3_8k >= 20)
		{
			if(G1k >= 40)
			{
					AllGain = (G2_5k - (20*G2_5k/60)) + (G3_8k - (20*G3_8k/60)) + (G1k - (40*G1k/60));	//G1k	G2_5k	G3_8k	CAL		SIMU	DIFF
																										//6		6		6		12.1	-11		1.1
																										//4		2		2		6.3		-4.5	1.8

			}
			else
			{
				AllGain = (G2_5k - (20*G2_5k/60)) + (G3_8k - (20*G3_8k/60));	//G1k	G2_5k	G3_8k	CAL		SIMU	DIFF
																				//0		6		6		10.1	-9.9	0.2
																				//3.9	6		6		10.1	-10.6	-0.5
																				//3.9	2		2		4.9		-4.5	0.4
																				//0		2		2		4.9		-3.7	1.2
			}
		}
		else
		{
			if(G1k >= 40)
			{
				AllGain = (G2_5k - (20*G2_5k/60)) + (G1k - (40*G1k/60));	//G1k	G2_5k	G3_8k	CAL		SIMU	DIFF
																			//6		6		0		8.1		-7.8	0.3
																			//6		6		1.9		8.1		-8.8	-0.7
																			//4		2		1.9		6.1		-4.4	1.7
																			//4		2		0		6.1		-3.5	2.6
			}
			else
			{
				AllGain = G2_5k;	//G1k	G2_5k	G3_8k	CAL		SIMU	DIFF
									//3.9	2		1.9		4.1		-4.4	-0.3
									//3.9	6		1.9		8.1		-8.4	-0.3
			}
		}

	}
	else
	{
		if(G3_8k >= 20)
		{
			AllGain = (G3_8k - (20*G3_8k/60));	//G1k	G2_5k	G3_8k	CAL		SIMU	DIFF
												//6		0		6		6.1		-5		1.1
												//6		1.9		6		6.1		-6.9	-0.8
												//6		1.9		2		3.5		-4.8	-1.3	!!cannot cover!!
												//6		0		2		3.5		-2.9	0.6
												//0		0		2		3.5		-2.2	1.3
		}
		else
		{
			if(G2_5k > 0)
			{
				AllGain = G2_5k;	//G1k	G2_5k	G3_8k	CAL		SIMU	DIFF
									//0		1.9		1.9		4		-3.5	0.5
									//6		1.9		1.9		4		-4.7	-0.7
			}
			else
			{
				AllGain = 0;
			}
		}
	}
#define ADDGAIN		0
	if(eq_default_tp->eq_header.all_gain + AllGain > ADDGAIN)
		eq_filter_data->eq_header.all_gain = eq_default_tp->eq_header.all_gain + AllGain - ADDGAIN;
	else
		eq_filter_data->eq_header.all_gain = 0;

//	my_dump_str_data (TLKALG_EQ_DEBUG_ENABLE, "EQ ALL GAIN", &(eq_filter_data->eq_header.all_gain), 1);


#else
	int ts = 0;
	signed short gain_table[9] = {0};
	for(int i = 0;i<TLKALG_EQ_FILTER_ANKER_MAX;i++)
	{
		temp = *p++;
		gain_table[i] = eq_filter_data->eq_para[i].gain = eq_default_tp->eq_para[i].gain + (temp-120)*10;
		if((i>2)&&(i<6)&&gain_table[i]>0)
		{
			tmp0 += gain_table[i];
		}
	}
	if(tmp0>eq_default_tp->eq_para[4].gain)
	{
		ts = tmp0/3;
	}
	temp_gain_max = eq_filter_data->eq_para[0].gain;
	for(int i = 1;i<TLKALG_EQ_FILTER_MAX;i++)
	{
		if(temp_gain_max < eq_filter_data->eq_para[i].gain)
		{
			temp_gain_max = eq_filter_data->eq_para[i].gain;
		}
	}
#endif
}

nds_bq_df1_f32_t *eq_instance_new_get(eq_para_t para)
{
	nds_bq_df1_f32_t *eq_tp = NULL;

	switch(para.eq_type)
	{
	case TLKALG_EQ_TYPE_MUSIC:
		if (TLKALG_EQ_CHANNEL_RIGHT == para.eq_channel)
		{
			instance_eq_music_right.nstage = eq_filter.eq_header.filter_sum;
			instance_eq_music_right.state = state_eq_music_right;
			instance_eq_music_right.coeff = coeff_eq_now;
			eq_tp = &instance_eq_music_right;
		}
		else
		{
			instance_eq_music_left.nstage = eq_filter.eq_header.filter_sum;
			instance_eq_music_left.state = state_eq_music_left;
			instance_eq_music_left.coeff = coeff_eq_now;
			eq_tp = &instance_eq_music_left;
		}
		break;

	case TLKALG_EQ_TYPE_VOICE_MIC:
#if (PROJ_DUALMODE_HEADSET || PROJ_CC_HEADSET)
		if (TLKALG_EQ_CHANNEL_RIGHT == para.eq_channel)
		{
			instance_eq_voice_mic_right.nstage = para.eq_nstage;
			instance_eq_voice_mic_right.state = state_eq_voice_mic_right;
			instance_eq_voice_mic_right.coeff = coeff_eq_voice_mic_right;
			eq_tp = &instance_eq_voice_mic_right;
		}
		else
#endif
		{
			instance_eq_voice_mic_left.nstage = para.eq_nstage;
			instance_eq_voice_mic_left.state = state_eq_voice_mic_left;
			instance_eq_voice_mic_left.coeff = coeff_eq_voice_mic_left;
			eq_tp = &instance_eq_voice_mic_left;
		}
		break;

	case TLKALG_EQ_TYPE_VOICE_SPK:
		if (TLKALG_EQ_CHANNEL_RIGHT == para.eq_channel)
		{
			instance_eq_voice_speaker_right.nstage = para.eq_nstage;
			instance_eq_voice_speaker_right.state = state_eq_voice_speaker_right;
			instance_eq_voice_speaker_right.coeff = coeff_eq_voice_speaker;
			eq_tp = &instance_eq_voice_speaker_right;
		}
		else
		{
			instance_eq_voice_speaker_left.nstage = para.eq_nstage;
			instance_eq_voice_speaker_left.state = state_eq_voice_speaker_left;
			instance_eq_voice_speaker_left.coeff = coeff_eq_voice_speaker;
			eq_tp = &instance_eq_voice_speaker_left;
		}
		break;
	default:
		eq_tp = NULL;
		break;
	}

	return eq_tp;
}

void tlkalg_eq_loadParam(uint32 adr, uint16 index, uint08 super_listen_flag)
{
	uint32_t eq_index_add;

	eq_state_reset_new_all();

	//******************test for no flash address be allocated*************************
//	tmemcpy(&eq_filter,eq_sys_default_tab,sizeof(eq_sys_default_tab));
//	eq_filter.eq_header.max_gain_num = 3;
//	tmemcpy (eq_save_buff, &eq_filter, sizeof (eq_filter));
//	tmemcpy (&eq_run_filter, &eq_filter, sizeof (eq_filter));
//	eq_coef_create(EQSampleRate, &eq_filter, coeff_eq_now);
//	#if TLK_SPI_DBG_ENABLE
//	uint08 tx_buff[100];
//	memcpy(tx_buff, &eq_run_filter, sizeof (eq_run_filter));
//	while (spi_is_busy(1));
//	spi_master_write_dma(1, (unsigned char *)&tx_buff, sizeof (eq_run_filter));

//	uint08 tx_buff1[200];
//	memcpy(tx_buff1, coeff_eq_now, sizeof (coeff_eq_now));
//	while (spi_is_busy(1));
//	spi_master_write_dma(1, (unsigned char *)&tx_buff1, sizeof (coeff_eq_now));
//	#endif
	//*********************************************************************************
	tlkapi_sendStr(TLKALG_EQ_DEBUG_ENABLE,"tlkalg_eq_loadParam");
	if(adr)
	{
		if(eq_voice_stat)
		{
			tmemcpy(&eq_filter_voice_mic,eq_voice_mic_default_tab,sizeof(eq_voice_mic_default_tab));
			tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "Voice_mic_EQ_value", &eq_filter_voice_mic, sizeof(eq_voice_mic_default_tab));
			eq_coef_create(16000, &eq_filter_voice_mic, coeff_Voice_mic_eq_now);

			tmemcpy(&eq_filter_voice_spk,eq_voice_spk_default_tab,sizeof(eq_voice_spk_default_tab));
			tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "Voice_mic_EQ_value", &eq_filter_voice_spk, sizeof(eq_voice_spk_default_tab));
			eq_coef_create(16000, &eq_filter_voice_spk, coeff_Voice_spk_eq_now);

		}

		if((index == 0xffff) || ((index&0x00ff)>0x15 && (index&0x00ff)!=0xfe))
		{
			index = 0x00;
			tmemcpy(&eq_filter,eq_sys_default_tab,sizeof(eq_sys_default_tab));
			eq_filter.eq_header.max_gain_num = 3;
		}
		else
		{
#if 1		//save eq 	for test
			eq_index_add = adr;
			u32 r = irq_disable ();
			flash_read_page(eq_index_add, TLKALG_EQ_TYPE_LENGTH_MAX, (uint08*)&eq_filter);
			irq_restore (r);
			if((index&0x00ff) != eq_filter.eq_header.idx)
			{
				index = 0x00;
				tmemcpy(&eq_filter,eq_sys_default_tab,sizeof(eq_sys_default_tab));
				eq_filter.eq_header.max_gain_num = 3;
				tlkapi_sendU32s (1, "eq_mode_t", index, eq_filter.eq_header.idx, 0, 0);
			}
			else
			{
				if(eq_filter.eq_header.filter_sum>9)
				{
					tmemcpy(&eq_filter, eq_sys_default_tab, sizeof(eq_sys_default_tab));
					eq_filter.eq_header.max_gain_num = 3;
					tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "EQ_STAGE: Err ", &eq_filter.eq_header.filter_sum, 1);
				}
			}
#else
			eq_index_add = adr + index * TLKALG_EQ_TYPE_LENGTH_MAX;
			u32 r = irq_disable ();
			flash_read_page(eq_index_add, TLKALG_EQ_TYPE_LENGTH_MAX, &eq_filter);
			irq_restore (r);
			if(eq_filter.eq_header.filter_sum>9)
			{
				index = 0x00;
				tmemcpy(&eq_filter,eq_sys_default_tab,sizeof(eq_sys_default_tab));
				eq_filter->eq_header.max_gain_num = 3;
				tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "EQ filter_sum Err ", 0, 0);
			}
#endif
		}
		tmemcpy (eq_save_buff, &eq_filter, sizeof (eq_filter));
		tmemcpy (&eq_run_filter, &eq_filter, sizeof (eq_filter));
		
		if(super_listen_flag)
		{
			tmemcpy(&eq_filter,eq_supper_listening_tab,sizeof(eq_supper_listening_tab));
			tmemcpy (&eq_run_filter, &eq_filter, sizeof (eq_filter));
		}

		tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "Music_SPK_EQ_value", &eq_filter, 80);

		eq_coef_create(EQSampleRate, &eq_filter, coeff_eq_now);
		
		tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "EQ enable", 0, 0);
	}
	else
	{
		tlkapi_sendData(TLKALG_EQ_DEBUG_ENABLE, "EQ state : EQ addr are empty and invalid", 0, 0);
	}
}

/**************************test************************/
//	float peak_coeff[5] = {0};
//	eq_get_coef(48000,10000,0.507,-10.0,peak_coeff);
//	my_dump_str_data(TLKALG_EQ_DEBUG_ENABLE, "@@@@eq_get_coef", &peak_coeff, 20);
//out:
//	60 ee 11 3f 24 c2 44 be  da 7d 30 3e 24 c2 44 3e 53 e4 83 3e
//ref(BDT):
//	0x3f11ee60, 0xbe44c229, 0x3e307ddc, 0x3e44c229, 0x3e83e452,
 /*****************************************************/
void eq_type_get_coef ( int sample_rate, TLKALG_EQ_FILTER_TYPE_ENUM type, int freq, float q, float db, float *coef_out)
{
	float fs = (float)sample_rate;
	float f0 = (float)freq;
	float omega = 2.0 * M_PI * f0 / fs;
	float A = powf(10.0, db * 0.025);
	float sn = sinf(omega);
	float cs = cosf(omega);
	float alpha = sn / q * 0.5;

	float b0 = 0.0;
	float b1 = 0.0;
	float b2 = 0.0;
	float a0 = 1.0;
	float a1 = 0.0;
	float a2 = 0.0;

	switch(type)
	{
		case TLKALG_EQ_FILTER_TYPE_PEAKING:
			b0 = 1.0 + (alpha * A);
			b1 = -2.0 * cs;
			b2 = 1.0 - (alpha * A);
			a0 = 1.0 + (alpha / A);
			a1 = -2.0 * cs;
			a2 = 1.0 - (alpha / A);
			break;
		case TLKALG_EQ_FILTER_TYPE_LOWPASS:
			b0 = (1.0 - cs) * 0.5;
			b1 = 1.0 - cs;
			b2 = (1.0 - cs) * 0.5;
			a0 = 1.0 + alpha;
			a1 = -2.0 * cs;
			a2 = 1.0 - alpha;
			break;
		case TLKALG_EQ_FILTER_TYPE_HIGHPASS:
			b0 = (1.0 + cs) * 0.5;
			b1 = -(1.0 + cs);
			b2 = (1.0 + cs) * 0.5;
			a0 = 1.0 + alpha;
			a1 = -2.0 * cs;
			a2 = 1.0 - alpha;
			break;
		case TLKALG_EQ_FILTER_TYPE_BANDPASS:
			b0 = alpha;
			b1 = 0.0;
			b2 = -alpha;
			a0 = 1.0 + alpha;
			a1 = -2.0 * cs;
			a2 = 1.0 - alpha;
			break;
		case TLKALG_EQ_FILTER_TYPE_NORTCH:
			b0 = 1.0;
			b1 = -2.0 * cs;
			b2 = 1.0;
			a0 = 1.0 + alpha;
			a1 = -2.0 * cs;
			a2 = 1.0 - alpha;
			break;
		case TLKALG_EQ_FILTER_TYPE_LOWSHELF:
			b0 = A * ((A + 1.0) - (A - 1.0) * cs + 2.0 * sqrtf(A) * alpha);
			b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cs);
			b2 = A * ((A + 1.0) - (A - 1.0) * cs - 2.0 * sqrtf(A) * alpha);
			a0 = (A + 1.0) + (A - 1.0) * cs + 2.0 * sqrtf(A) * alpha;
			a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cs);
			a2 = (A + 1.0) + (A - 1.0) * cs - 2.0 * sqrtf(A) * alpha;
			break;
		case TLKALG_EQ_FILTER_TYPE_HIGHSHELF:
			b0 = A * ((A + 1.0) + (A - 1.0) * cs + 2.0 * sqrtf(A) * alpha);
			b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cs);
			b2 = A * ((A + 1.0) + (A - 1.0) * cs - 2.0 * sqrtf(A) * alpha);
			a0 = (A + 1.0) - (A - 1.0) * cs + 2.0 * sqrtf(A) * alpha;
			a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cs);
			a2 = (A + 1.0) - (A - 1.0) * cs - 2.0 * sqrtf(A) * alpha;
			break;
	}
	coef_out[0] = b0 / a0;
	coef_out[1] = b1 / a0;
	coef_out[2] = b2 / a0;
	coef_out[3] = -a1 / a0;
	coef_out[4] = -a2 / a0;

}

void eq_coef_create(int sample_rate,eq_mode_t *filter, float *coeff)
{
	float q_value = 0.0;
	float gain = 0.0;
	float eq_all_gain = 0.0;
	int frequency = 0;
	TLKALG_EQ_FILTER_TYPE_ENUM type;
	int ns = filter->eq_header.filter_sum;

	if(filter->eq_header.all_gain>0)
	{
		eq_all_gain = (float)(filter->eq_header.all_gain/10.0);
	}

	for(int i=0;i<ns;i++)
	{
		type = filter->eq_para[i].filter_type;
		q_value = filter->eq_para[i].filter_q/1000.0;
		frequency = filter->eq_para[i].freq_c;
		gain = filter->eq_para[i].gain/100.0;

		eq_type_get_coef(sample_rate, type, frequency, q_value, gain, (coeff+i*5));
		if(i == filter->eq_header.max_gain_num)
		{
			float N = powf(10.0,eq_all_gain/20.0);
			coeff[i*5]= coeff[i*5]/N;
			coeff[i*5+1]= coeff[i*5+1]/N;
			coeff[i*5+2]= coeff[i*5+2]/N;
		}

	}
}

void eq_proc_new(nds_bq_df1_f32_t *para, signed short *ps, signed short *pd, unsigned int nsample, unsigned char shift)
{
	nds_bq_df1_f32_t *nds_bq_p = NULL;

	nds_bq_p = para;
	tlkapi_sendU32s (TLKALG_EQ_DEBUG_ENABLE, "# eq_proc1",
								nds_bq_p->nstage,
								nds_bq_p->state,
								nds_bq_p->coeff,
								nsample);
	if (!(ps && pd && nds_bq_p) || nsample > 1024)
	{
		tlkapi_sendU32s (TLKALG_EQ_DEBUG_ENABLE, "# eq_proc, error",
							ps,
							pd,
							nsample,
							nds_bq_p);

		return;
	}

#if MY_EQ_FLOAT_ENABLE
	float pcm[1024*2];
	my_biquad_filter_float_st_t my_eq_float_st;
	my_eq_float_st.eq_nstage = nds_bq_p->nstage;
	my_eq_float_st.state     = nds_bq_p->state;
	my_eq_float_st.coeff     = nds_bq_p->coeff;

	for (int i = 0; i < nsample; i++)
	{
		//pcm[i] = ((float)ps[i]+0.00000000001f);
		pcm[i] = ps[i] ? (float)ps[i] : 0.000001f;

		if (TLKALG_EQ_CHANNEL_STEREO == para.eq_channel)
			pcm[i + nsample] = ps[i + nsample] ? (float)ps[i + nsample] : 0.000001f;
	}

//	log_task (1, SL01_task_kwd_awaken, 1);

	if (TLKALG_EQ_CHANNEL_STEREO == para.eq_channel)
	{
		biquad_filter_float_cascade_stero_same_coef(&my_eq_float_st, pcm, pcm, nsample);
	}
	else
	{
		biquad_filter_float_cascade_mono(&my_eq_float_st, pcm, pcm, nsample);
	}

//	log_task (1, SL01_task_kwd_awaken, 0);

	for (int i = 0; i < nsample; i++)
	{
		pd[i] = (short)pcm[i];

		if (TLKALG_EQ_CHANNEL_STEREO == para.eq_channel)
			pd[i + nsample] = (short)pcm[i + nsample];
	}

#else
//	tlkapi_sendU32s (TLKALG_EQ_DEBUG_ENABLE, "# eq_proc_music, para",
//			eq_para_music.eq_channel,
//			eq_para_music.eq_nstage,
//			eq_para_music.eq_sample_rate,
//			eq_para_music.eq_type);

#if EQ_PROFILE
	int cyc1=__nds__csrr(NDS_MCYCLE);
#endif

	float x2[1024];				///TODO

	x2[0] = 0;
	for (int i = 0; i < nsample; i++)
	{
		x2[i] = ps[i];
	}

	x2[0] = x2[0] ? x2[0]:0.000001f;

//	log_task (1, SL01_task_kwd_awaken, 1);
	nds_bq_df1_f32(nds_bq_p, x2, x2, nsample);
//	log_task (1, SL01_task_kwd_awaken, 0);

	for (int i = 0; i < nsample; i++)
	{
		float xr = x2[i];
		if (xr >= 0)
		{
			xr += 0.5f;
		}
		else
		{
			xr -= 0.5f;
		}
		pd[i] = xr < -32768 ? -32768 : xr > 32767 ? 32767 : xr;
	}

#if EQ_PROFILE
	int cyc2=__nds__csrr(NDS_MCYCLE);
	int cyc_eq_float= cyc2-cyc1;
//	my_dump_str_data(TLKALG_EQ_DEBUG_ENABLE,"EQ_PROFILE", &cyc_eq_float, 4);
#endif

#endif //MY_EQ_FLOAT_ENABLE
}

void tlkalg_eq_saveParam(void)
{
	unsigned char eq_save_tmp[TLKALG_EQ_TYPE_LENGTH_MAX] = {0};

	if(eq_flash_addr == 0) return;

	u32 r = irq_disable ();
	flash_read_page(eq_flash_addr,(unsigned long)TLKALG_EQ_TYPE_LENGTH_MAX,eq_save_tmp);
	irq_restore (r);

	if(tmemcmp(eq_save_tmp,eq_save_buff,TLKALG_EQ_TYPE_LENGTH_MAX))
	{
		r = irq_disable ();
		flash_erase_sector(eq_flash_addr);
		flash_write_page(eq_flash_addr, (unsigned long)TLKALG_EQ_TYPE_LENGTH_MAX, eq_save_buff);
		irq_restore (r);
		tlkapi_sendData(TLKALG_EQ_DEBUG_ENABLE,"EQ needs to be saved", 0, 0);
	}
	else
	{
		tlkapi_sendData(TLKALG_EQ_DEBUG_ENABLE,"EQ without saving", 0, 0);
	}
}

int super_listen_chg = 0;
int super_listen_stat = 0;
int eq_chg_state = 0;
void set_super_listen_mode(uint08 super_listen)
{
	tlkapi_sendData(TLKALG_EQ_DEBUG_ENABLE, "CMD_SET_SUPPER_LISTENING_handle", 0, 0);
//	audio_codec_flag_set(CODEC_FLAG_EQ_MUSIC_EN , 0);

	super_listen_chg = 1;
	for(int i=0;i<9;i++)
	{
		eq_after_gain[i] = 0x10000 - eq_run_filter.eq_para[i].gain;
	}
	tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "old_gain_EQ", eq_after_gain, 18);

	if(super_listen)		///supper_listen
	{
		tmemcpy(&eq_filter,eq_supper_listening_tab,sizeof(eq_supper_listening_tab));
		super_listen_stat = 1;
		eq_run_state = 0x0000;
		tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "EQ ---> super_listen", &eq_filter, 80);
	}
	else									///reload EQ
	{
		tmemcpy (&eq_filter, eq_save_buff, sizeof (eq_sys_default_tab));
		super_listen_stat = 0;
		eq_run_state = 0x0100;
		tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "super_listen ---> EQ", &eq_filter, 80);
	}

	eq_updata_flag = 1;
	eq_chg_state = 0;
//	audio_codec_flag_set(CODEC_FLAG_EQ_MUSIC_EN , 1);

}

void eq_chg_work(void)
{
	#define ADJUST_SCALE_GAIN_VALUE	0x20
//	signed short all_gain_chg = 0;
	if(eq_updata_flag)
	{
		if(super_listen_chg)
		{
			tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "loading_FUN_EQ", &eq_run_filter, 80);
			tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "old_gain_EQ", eq_after_gain, 18);
			if(super_listen_stat)	//EQ ---> 0 ---> super_listen
			{
				if(eq_chg_state)				// 0 ---> super_listen
				{
					for(int i=0;i<8;i++)
					{
						if(eq_after_gain[i] > 0)
						{
							eq_after_gain[i] = eq_after_gain[i] - ADJUST_SCALE_GAIN_VALUE;
							eq_run_filter.eq_para[i].gain = eq_run_filter.eq_para[i].gain + ADJUST_SCALE_GAIN_VALUE;
							if(eq_after_gain[i] < 0)
							{
								eq_after_gain[i] = 0;
								eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
								eq_run_state |= (0x01<<i);
							}
						}
						else if(eq_after_gain[i] < 0)
						{
							eq_after_gain[i] = eq_after_gain[i] + ADJUST_SCALE_GAIN_VALUE;
							eq_run_filter.eq_para[i].gain  = eq_run_filter.eq_para[i].gain - ADJUST_SCALE_GAIN_VALUE;
							if(eq_after_gain[i] > 0)
							{
								eq_after_gain[i] = 0;
								eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
								eq_run_state |= (0x01<<i);
							}
						}
						else
						{
							eq_run_state |= (0x01<<i);
							eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
						}
					}
				}
				else							// EQ ---> 0
				{
					for(int i=0;i<9;i++)
					{
						if(eq_run_filter.eq_para[i].filter_type == TLKALG_EQ_FILTER_TYPE_HIGHPASS)
						{
							if(!(eq_run_state&(0x01<<i)))
							{
								eq_run_filter.eq_para[i].filter_q = eq_run_filter.eq_para[i].filter_q - 0x64;
								eq_run_filter.eq_para[i].freq_c = eq_run_filter.eq_para[i].freq_c -0x05;
								if(eq_run_filter.eq_para[i].filter_q <= 0x02bc)
								{
									eq_run_filter.eq_para[i].gain = 0;
									eq_after_gain[i] = 0;
									eq_run_state |= (0x01<<i);
								}
							}
						}
						else
						{
							if(eq_after_gain[i] > 0)
							{
								eq_after_gain[i] = eq_after_gain[i] - ADJUST_SCALE_GAIN_VALUE;
								eq_run_filter.eq_para[i].gain = eq_run_filter.eq_para[i].gain + ADJUST_SCALE_GAIN_VALUE;
								if(eq_after_gain[i] < 0)
								{
									eq_after_gain[i] = 0;
									eq_run_filter.eq_para[i].gain = 0;
									eq_run_state |= (0x01<<i);
								}
							}
							else if(eq_after_gain[i] < 0)
							{
								eq_after_gain[i] = eq_after_gain[i] + ADJUST_SCALE_GAIN_VALUE;
								eq_run_filter.eq_para[i].gain  = eq_run_filter.eq_para[i].gain - ADJUST_SCALE_GAIN_VALUE;
								if(eq_after_gain[i] > 0)
								{
									eq_after_gain[i] = 0;
									eq_run_filter.eq_para[i].gain = 0;
									eq_run_state |= (0x01<<i);
								}
							}
							else
							{
								eq_run_state |= (0x01<<i);
								eq_run_filter.eq_para[i].gain = 0;
							}
						}
					}
				}
				if(eq_run_state == 0x1ff)
				{
					if(eq_chg_state)
					{
						eq_updata_flag = 0;
						super_listen_chg = 0;
						eq_chg_state = 0;
						eq_run_state = 0;
						tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "0 ---> super_listen", &eq_run_filter, 80);
					}
					else
					{
						if(eq_run_filter.eq_header.all_gain > eq_filter.eq_header.all_gain)
						{
							eq_run_filter.eq_header.all_gain -= ADJUST_SCALE_GAIN_VALUE;
							if(eq_run_filter.eq_header.all_gain < eq_filter.eq_header.all_gain)
							{
								eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
							}
						}
						else if(eq_run_filter.eq_header.all_gain < eq_filter.eq_header.all_gain)
						{
							eq_run_filter.eq_header.all_gain += ADJUST_SCALE_GAIN_VALUE;
							if(eq_run_filter.eq_header.all_gain > eq_filter.eq_header.all_gain)
							{
								eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
							}
						}
						else
						{
							tmemcpy (&eq_run_filter, &eq_filter, sizeof (eq_supper_listening_tab));

							for(int i = 0;i<8;i++)
							{
								eq_after_gain[i] = eq_run_filter.eq_para[i].gain;
								eq_run_filter.eq_para[i].gain = 0;
							}
							eq_run_state = 0x100;
							eq_chg_state = 1;
							tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "EQ ---> 0", 0, 0);
						}
					}

				}
			}
			else								//super_listen ---> 0 ----> EQ
			{
				if(eq_chg_state)				// 0 ---> EQ
				{
					for(int i=0;i<9;i++)
					{
						if(eq_after_gain[i] > 0)
						{
							eq_after_gain[i] = eq_after_gain[i] - ADJUST_SCALE_GAIN_VALUE;
							eq_run_filter.eq_para[i].gain = eq_run_filter.eq_para[i].gain + ADJUST_SCALE_GAIN_VALUE;
							if(eq_after_gain[i] < 0)
							{
								eq_after_gain[i] = 0;
								eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
								eq_run_state |= (0x01<<i);
							}
						}
						else if(eq_after_gain[i] < 0)
						{
							eq_after_gain[i] = eq_after_gain[i] + ADJUST_SCALE_GAIN_VALUE;
							eq_run_filter.eq_para[i].gain  = eq_run_filter.eq_para[i].gain - ADJUST_SCALE_GAIN_VALUE;
							if(eq_after_gain[i] > 0)
							{
								eq_after_gain[i] = 0;
								eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
								eq_run_state |= (0x01<<i);
							}
						}
						else
						{
							eq_run_state |= (0x01<<i);
							eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
						}
					}
				}
				else			///super_listen ---> 0
				{
					for(int i=0;i<8;i++)
					{
						if(eq_after_gain[i] > 0)
						{
							eq_after_gain[i] = eq_after_gain[i] - ADJUST_SCALE_GAIN_VALUE;
							eq_run_filter.eq_para[i].gain = eq_run_filter.eq_para[i].gain + ADJUST_SCALE_GAIN_VALUE;
							if(eq_after_gain[i] < 0)
							{
								eq_after_gain[i] = 0;
								eq_run_filter.eq_para[i].gain = 0;
								eq_run_state |= (0x01<<i);
							}
						}
						else if(eq_after_gain[i] < 0)
						{
							eq_after_gain[i] = eq_after_gain[i] + ADJUST_SCALE_GAIN_VALUE;
							eq_run_filter.eq_para[i].gain  = eq_run_filter.eq_para[i].gain - ADJUST_SCALE_GAIN_VALUE;
							if(eq_after_gain[i] > 0)
							{
								eq_after_gain[i] = 0;
								eq_run_filter.eq_para[i].gain = 0;
								eq_run_state |= (0x01<<i);
							}
						}
						else
						{
							eq_run_state |= (0x01<<i);
							eq_run_filter.eq_para[i].gain = 0;
						}
					}
				}
				if(eq_run_state == 0x1ff)
				{
					if(eq_chg_state)
					{
						eq_chg_state = 0;
						eq_updata_flag = 0;
						super_listen_chg = 0;
						eq_run_state = 0;
						tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "0 ---> EQ", &eq_run_filter, 80);
					}
					else
					{
						if(eq_run_filter.eq_header.all_gain > eq_filter.eq_header.all_gain)
						{
							eq_run_filter.eq_header.all_gain -= ADJUST_SCALE_GAIN_VALUE;
							if(eq_run_filter.eq_header.all_gain < eq_filter.eq_header.all_gain)
							{
								eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
							}
						}
						else if(eq_run_filter.eq_header.all_gain < eq_filter.eq_header.all_gain)
						{
							eq_run_filter.eq_header.all_gain += ADJUST_SCALE_GAIN_VALUE;
							if(eq_run_filter.eq_header.all_gain > eq_filter.eq_header.all_gain)
							{
								eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
							}
						}
						else
						{
							tmemcpy (&eq_run_filter, &eq_filter, sizeof (eq_sys_default_tab));

							for(int i = 0;i<9;i++)
							{
								eq_after_gain[i] = eq_run_filter.eq_para[i].gain;
								eq_run_filter.eq_para[i].gain = 0;
							}
							eq_run_state = 0;
							eq_chg_state = 1;
							tlkapi_sendData (TLKALG_EQ_DEBUG_ENABLE, "super_listen ---> 0", 0, 0);
						}
					}

				}
			}
		}
		else
		{
			for(int i=0;i<8;i++)
			{
				if(eq_after_gain[i] > 0)
				{
					eq_after_gain[i] = eq_after_gain[i] - 0x0a;
					eq_run_filter.eq_para[i].gain = eq_run_filter.eq_para[i].gain + 0x0a;
					if(eq_after_gain[i] < 0)
					{
						eq_after_gain[i] = 0;
						eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
						eq_run_state |= (0x01<<i);
					}
				}
				else if(eq_after_gain[i] < 0)
				{
					eq_after_gain[i] = eq_after_gain[i] + 0x0a;
					eq_run_filter.eq_para[i].gain  = eq_run_filter.eq_para[i].gain - 0x0a;
					if(eq_after_gain[i] > 0)
					{
						eq_after_gain[i] = 0;
						eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
						eq_run_state |= (0x01<<i);
					}
				}
				else
				{
					eq_run_state |= (0x01<<i);
					eq_run_filter.eq_para[i].gain = eq_filter.eq_para[i].gain;
				}
			}
			if(eq_run_state == 0xff)
			{
				tlkapi_sendU32s(TLKALG_EQ_DEBUG_ENABLE, " EQ_x--> EQ-y", eq_run_filter.eq_header.all_gain, eq_filter.eq_header.all_gain ,eq_run_state,0);
				if(abs_ram(eq_run_filter.eq_header.all_gain - eq_filter.eq_header.all_gain)<0x0a)
				{
					eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
					eq_updata_flag = 0;
					eq_run_state = 0;
					tlkapi_sendData (1, "loading_EQ0", &eq_run_filter, 80);
				}
				else if(eq_run_filter.eq_header.all_gain > eq_filter.eq_header.all_gain)
				{
					eq_run_filter.eq_header.all_gain -= 0x0a;
					if(eq_run_filter.eq_header.all_gain < eq_filter.eq_header.all_gain)
					{
						eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
					}
				}
				else if(eq_run_filter.eq_header.all_gain < eq_filter.eq_header.all_gain)
				{
					eq_run_filter.eq_header.all_gain += 0x0a;
					if(eq_run_filter.eq_header.all_gain > eq_filter.eq_header.all_gain)
					{
						eq_run_filter.eq_header.all_gain = eq_filter.eq_header.all_gain;
					}
				}
			}
		}
		tlkapi_sendU32s (TLKALG_EQ_DEBUG_ENABLE, "eq_sys_inf", eq_run_state, eq_updata_flag, super_listen_chg, eq_chg_state);
		eq_coef_create(EQSampleRate, &eq_run_filter, coeff_eq_now);
	}
}

#endif
