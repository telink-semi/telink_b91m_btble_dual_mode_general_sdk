/********************************************************************************************************
 * @file	tlkusb_uacSpk.h
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
#ifndef TLKUSB_AUDIO_SPK_H
#define TLKUSB_AUDIO_SPK_H

#if (TLKUSB_UAC_SPK_ENABLE)

#define	TLKUSB_AUDSPK_VOL_MIN       ((sint16) 0xa000)     /* Volume Minimum Value */
#define	TLKUSB_AUDSPK_VOL_MAX       ((sint16) 0x0300)     /* Volume Maximum Value */
#define TLKUSB_AUDSPK_VOL_RES       0x0180    /* Volume Resolution */
#define TLKUSB_AUDSPK_VOL_DEF       0x8000    /* Volume default */
#define TLKUSB_AUDSPK_VOL_STEP      400		  /* Volume step*/


typedef struct {
	uint08 mute;		/*Whether mute.*/
	uint16 curVol;		/*Current volume.*/
	uint16 volStep;		/*Volume step.*/
	uint32 sampleRate;	/*Sample rate*/
	tlkapi_fifo_t fifo;
}tlkusb_uacspk_ctrl_t;


int tlkusb_uacspk_init(void);

bool tlkusb_uacspk_getEnable(void);
void tlkusb_uacspk_setEnable(bool enable);

uint tlkusb_uacspk_getVolume(void);
void tlkusb_uacspk_setVolume(uint16 volume);
void tlkusb_uacspk_enterMute(bool enable);
void tlkusb_uacspk_autoFlush(bool enable);

int tlkusb_uacspk_setInfCmdDeal(int type);
int tlkusb_uacspk_getInfCmdDeal(int req, int type);

int tlkusb_uacspk_setEdpCmdDeal(int type);


void tlkusb_uacspk_recvData(void);
uint tlkusb_uacspk_readData(uint08 *pBuff, uint16 buffLen, bool isParty);




#endif //#if (TLKUSB_UAC_SPK_ENABLE)

#endif //TLKUSB_AUDIO_SPK_H

