/********************************************************************************************************
 * @file	tlkmdi_audtone.h
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
#ifndef TLKMDI_AUDTONE_H
#define TLKMDI_AUDTONE_H

#if (TLK_MDI_AUDTONE_ENABLE)



#define TLKMDI_AUDTONE_DECODE_MP3       (1 && TLK_MDI_MP3_ENABLE)
#define TLKMDI_AUDTONE_DECODE_SBC       0
#define TLKMDI_AUDTONE_DECODE_ADPCM     0

#if !(TLKMDI_AUDTONE_DECODE_MP3)
#define TLKMDI_AUDTONE_FLASH_ADDR              0x200000
#define TLKMDI_AUDTONE_SBC_DEC_BUFF_SIZE       1248
#define TLKMDI_AUDTONE_FLASH_HEAD_LENGTH       12
typedef enum{
	TLKMDI_TONE_STATUS_IDLE = 0,
	TLKMDI_TONE_STATUS_WAIT,
	TLKMDI_TONE_STATUS_DONE,
	TLKMDI_TONE_STATUS_PLAY,
}TLKMDI_TONE_STATUS_ENUM;
#endif


typedef enum{
    TLK_TONE_ECODE_ADPCM = 0,
    TLK_TONE_ECODE_SBC,
}TLK_TONE_ECODE_TYPE;



typedef enum{
	TLKMDI_TONE_TYPE_CONNECT = 0x0,		
	TLKMDI_TONE_TYPE_DISCONN,
	TLKMDI_TONE_TYPE_CALL_RING,
	TLKMDI_TONE_TYPE_BI,
	TLKMDI_TONE_TYPE_BO,
	TLKMDI_TONE_TYPE_DING, 
	TLKMDI_TONE_TYPE_DING_DING,
	TLKMDI_TONE_TYPE_MAX
}TLKMDI_TONE_TYPE_ENUM;

/******************************************************************************
 * Function: tlkmdi_audplay_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audtone_init(void);

int tlkmdi_audtone_start(uint16 handle, uint32 param);
int tlkmdi_audtone_close(uint16 handle);


/******************************************************************************
 * Function: tlkmdi_audtone_isBusy
 * Descript: Is the Tone Ctrl is enable. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
bool tlkmdi_audtone_isBusy(void);


/******************************************************************************
 * Function: tlkmdi_audtone_intval
 * Descript: Get Tone Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audtone_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audtone_switch
 * Descript: Change Tone status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_audtone_irqProc
 * Descript: Call Tone mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audtone_irqProc(void);

/******************************************************************************
 * Function: tlkmdi_tone_start
 * Descript: Start tone to playing. 
 * Params:
 *         @index[IN]--The music index.
 * Return: Return TLK_ENONE is success other value is false.
*******************************************************************************/
bool tlkmdi_tone_start(uint16 index);

/******************************************************************************
 * Function: tlkmdi_tone_close
 * Descript: Stop tone to playing. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_close(void);

/******************************************************************************
 * Function: tlkmdi_tone_setParam
 * Descript: Set the playing parameter. 
 * Params: 
 *        @playIndex[IN]--Theplay index.
 *        @playcount[IN]--The play count.
 * Return: None.
*******************************************************************************/
void tlkmdi_tone_setParam(uint16 playIndex, uint08 playCount);



#endif //#if (TLK_MDI_AUDTONE_ENABLE)

#endif //TLKMDI_AUDTONE_H

