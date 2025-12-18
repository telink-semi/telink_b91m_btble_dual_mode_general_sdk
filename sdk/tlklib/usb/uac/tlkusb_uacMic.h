/********************************************************************************************************
 * @file	tlkusb_uacMic.h
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
#ifndef TLKUSB_AUDIO_MIC_H
#define TLKUSB_AUDIO_MIC_H

#if (TLKUSB_UAC_MIC_ENABLE)


#define TLKUSB_AUDMIC_VOL_MIN      ((sint16) 0x0000) /* Volume Minimum Value */
#define TLKUSB_AUDMIC_VOL_MAX      ((sint16) 0x1e00) /* Volume Maximum Value */
#define	TLKUSB_AUDMIC_VOL_RES      0x0180 /* Volume Resolution */
#define TLKUSB_AUDMIC_VOL_DEF      0x1800 /* Volume default */
#define	TLKUSB_AUDMIC_VOL_STEP     (TLKUSB_AUDMIC_VOL_MAX / TLKUSB_AUDMIC_VOL_RES)/*Volume step*/

typedef struct {
	uint08 mute;		/*Whether mute.*/
	uint16 curVol;		/*Current volume.*/
	uint16 volStep;		/*Volume step.*/
	uint32 sampleRate;	/*Sample rate.*/
	tlkapi_fifo_t fifo;
}tlkusb_uacmic_ctrl_t;



int tlkusb_uacmic_init(void);

void tlkusb_uacmic_regStatusChangeCB(TlkUsbUacStatusChangeCB cb);


bool tlkusb_uacmic_getEnable(void);
void tlkusb_uacmic_setEnable(bool enable);

uint tlkusb_uacmic_getVolume(void);
void tlkusb_uacmic_setVolume(sint16 volume);
void tlkusb_uacmic_enterMute(bool enable);


int tlkusb_uacmic_setInfCmdDeal(int type);
int tlkusb_uacmic_getInfCmdDeal(int req, int type);

int tlkusb_uacmic_setEdpCmdDeal(int type);


void tlkusb_uacmic_autoZero(bool enable);

void tlkusb_uacmic_fillData(void);
bool tlkusb_uacmic_sendData(uint08 *pData, uint16 dataLen, bool isCover);



#endif //#if (TLKUSB_UAC_MIC_ENABLE)

#endif //TLKUSB_AUDIO_MIC_H

