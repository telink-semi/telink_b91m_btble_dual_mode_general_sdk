/********************************************************************************************************
 * @file	tlkalg_sbcPlc.h
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
#ifndef TLKALG_SBC_PLC_H
#define TLKALG_SBC_PLC_H



 /*! Construct version number from major/minor/micro values. */
#define TLKALG_SBC_PLC_HF_VERSION_INT(major, minor, micro) (((major) << 16) | ((minor) << 8) | (micro))

/*! Version number to ensure header and binary are matching. */
#define TLKALG_SBC_PLC_HF_VERSION   TLKALG_SBC_PLC_HF_VERSION_INT(1, 0, 5)


#define TLKALG_SBC_PLC_FS 		120 	/* Frame Size */
#define TLKALG_SBC_PLC_N 		256 	/* 16ms - Window Length for pattern matching */
//#define TLKALG_SBC_PLC_M 		64 		/* 4ms - Template for matching */
#define TLKALG_SBC_PLC_M 		64 		/* 4ms - Template for matching */
#define TLKALG_SBC_PLC_LHIST 	(TLKALG_SBC_PLC_N+TLKALG_SBC_PLC_FS-1) 	/* Length of history buffer required */
#define TLKALG_SBC_PLC_SBCRT 	36			/* SBC Reconvergence Time (samples) */
//#define TLKALG_SBC_PLC_SBCRT 	36 			/* SBC Reconvergence Time (samples) */
#define TLKALG_SBC_PLC_OLAL 	16 			/* OverLap-Add Length (samples) */

#if TLKALG_SBC_PLC_FS == 120 /* Frame Size of 120 samples */
#define TLKALG_SBC_PLC_FSIDX           57 /* Frame Size Indexes*/
#define TLKALG_SBC_PLC_NROFBLK         15 /* NumbeR OF BLocKs*/
#define TLKALG_SBC_PLC_CHMODE          0  /* CHannel MODE 0=mono, 1=dual, 2=stereo, 3=joint */
#define TLKALG_SBC_PLC_ALLOCMETHOD     0  /* bit ALLOCation METHOD 0=loudness, 1=SNR */
#define TLKALG_SBC_PLC_NROFSB          8  /* NumbeR OF SubBands */
#define TLKALG_SBC_PLC_BITPOOL         26 /* TLKALG_SBC_PLC_BITPOOL size */
#endif


/* PLC State Information */
typedef struct
{
	float hist[TLKALG_SBC_PLC_LHIST+TLKALG_SBC_PLC_FS+TLKALG_SBC_PLC_SBCRT+TLKALG_SBC_PLC_OLAL];
	short bestlag;
	int   nbf;
	float g;
	int faster_en;
}tlkalg_sbc_plcState_t;



/* Prototypes */
void tlkalg_sbc_plcInit(tlkalg_sbc_plcState_t *plc_state,int faster_en);
void tlkalg_sbc_plcBadFrame(tlkalg_sbc_plcState_t *plc_state, short *ZIRbuf, short *out);
void tlkalg_sbc_plcGoodFrame(tlkalg_sbc_plcState_t *plc_state, short *in, short *out);

extern unsigned char gTlkalgSbcPlcIndices0[];


#define TLKALG_ABC_PLC_FIXED_POINT    0
#if TLKALG_ABC_PLC_FIXED_POINT
typedef struct{
	short hist[TLKALG_SBC_PLC_LHIST + TLKALG_SBC_PLC_FS + TLKALG_SBC_PLC_SBCRT + TLKALG_SBC_PLC_OLAL];
	short bestlag;
	int nbf;
	int g;
	int faster_en;
}tlkalg_sbc_plcStateFix_t;

void tlkalg_sbc_plcInitFix(tlkalg_sbc_plcStateFix_t* plc_state);
void tlkalg_sbc_plcBadFrameFix(tlkalg_sbc_plcStateFix_t* plc_state, short* ZIRbuf, short* out);
void tlkalg_sbc_plcGoodFrameFix(tlkalg_sbc_plcStateFix_t* plc_state, short* in, short* out);
extern tlkalg_sbc_plcStateFix_t my_plc_state_fix;
#endif



#endif //TLKALG_SBC_PLC_H

