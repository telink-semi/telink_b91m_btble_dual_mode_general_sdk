/********************************************************************************************************
 * @file	tlkmmi_audioInfo.h
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
#ifndef TLKMMI_AUDIO_INFO_H
#define TLKMMI_AUDIO_INFO_H


#define TLKMDI_AUDIO_VOLUME_SAVE_SIGN        0x3A
#define TLKMDI_AUDIO_VOLUME_SAVE_VERS        0x02
#define TLKMDI_AUDIO_VOLUME_SAVE_ADDR        TLK_CFG_FLASH_VOLUME_ADDR
#define TLKMDI_AUDIO_VOLUME_SAVE_SIZE        12


typedef struct{
	uint08 tone;
	uint08 music;
	uint08 voice;
	uint08 headset;
	uint08 resv001;
	uint08 enReport;
	uint16 interval;
	uint08 resv002[4];
}tlkmmi_audio_infoItem_t;
typedef struct{
	uint08 isChange;
	uint08 reserved;
	uint16 interval;
	tlkapi_save_ctrl_t save;
	tlkmmi_audio_infoItem_t item;
}tlkmmi_audio_infoCtrl_t;


/******************************************************************************
 * Function: tlkmmi_audio_infoInit
 * Descript: Initial the audio info.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_infoInit(void);

/******************************************************************************
 * Function: tlkmmi_audio_infoIsChange
 * Descript: Check whether audio info has changed.
 * Params:
 * Return: Operating results. TRUE means changed, others means no changed.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_infoIsChange(void);

/******************************************************************************
 * Function: tlkmmi_audio_infoIsUpdate
 * Descript: Check whether audio info is need update.
 * Params:
 * Return: Operating results. TRUE means changed, others means no changed.
 * Others: None.
*******************************************************************************/
bool tlkmmi_audio_infoIsUpdate(void);

/******************************************************************************
 * Function: tlkmmi_audio_infoLoad
 * Descript: load the audio info.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_infoLoad(void);

/******************************************************************************
 * Function: tlkmmi_audio_infoSave
 * Descript: save the audio info.
 * Params:
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmmi_audio_infoSave(void);

/******************************************************************************
 * Function: tlkmdi_audio_infoGetVolume
 * Descript: Get the volume.
 * Params:
 *         @optype[IN]--The Audio operate type.
 *         @pVolume[IN]--The current audio volume.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoGetVolume(uint08 voltype, uint08 *pVolume);

/******************************************************************************
 * Function: tlkmdi_audio_infoGetReport
 * Descript: Get the audio report.
 * Params: @pEnable[IN]--enable the report or not.
 *         @pInterval[IN]--The interval.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoGetReport(uint08 *pEnable, uint16 *pInterval);

/******************************************************************************
 * Function: tlkmdi_audio_infoSetVolume
 * Descript: Set the audio volume.
 * Params:
 *         @voltype[IN]--The operate type.
 *         @volume[IN]--The volume.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoSetVolume(uint08 voltype, uint08 volume);

/******************************************************************************
 * Function: tlkmmi_audio_infoInit
 * Descript: Enable the audio report info.
 * Params:
 *         @enable[IN]--The enable value.
 *         @interval[IN]--The time value.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_audio_infoSetReport(uint08 enable, uint16 interval);



#endif //TLKMMI_AUDIO_INFO_H

