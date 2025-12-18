/********************************************************************************************************
 * @file	tlkmdi_audsrc.h
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
#ifndef TLKMDI_SRC_H
#define TLKMDI_SRC_H

#if (TLK_MDI_AUDSRC_ENABLE)


#define TLKMDI_SRC_FRAME_SIZE              85 //For 32bitPool
#define TLKMDI_SRC_PKT_BUFF_SIZE           680 //676, fixed, not change
#define TLKMDI_SRC_SBC_ENC_BUFF_SIZE       3368

//Total buffer = MP3(36776)+PKT(680)+ENC(3368)
//             = 36776+680+3368 = 40824+3*16 = 40872



#define TLKMDI_SRC_FRAME_MAX_NUMB          7 //(TLKMDI_SRC_PKT_BUFF_SIZE-13)/TLKMDI_SRC_FRAME_SIZE

#define TLKMDI_SRC_WAIT_RECFG_TIMEOUT      3000000 //Unit:us




typedef struct{
	uint08 running;
	uint08 enable;
	uint16 handle;
	uint32 refTime;
	uint32 lagTime;
	uint08 sndFrame;
	uint08 isSuppSet;
	uint16 seqNumber;
	uint32 unitTime;
	uint32 timeStamp;

	uint08 pktNumb; //Frame number in one media packet
	uint08 frmSize; //85
	uint16 pktTime; //128*pktNumb

	uint08 sendFail;
	uint08 cacheFrm; //Cache Frames
	uint08 isPause;
	uint08 mp3State;
	uint08 waitStart;
	uint16 playIndex;
	uint32 waitTimer;
	uint32 sampleRate;
}tlkmdi_audsrc_ctrl_t;


/******************************************************************************
 * Function: tlkmdi_audsrc_init
 * Descript: Initialize playback parameters and logic. 
 * Params: None.
 * Return: 'TLK_ENONE' means success, otherwise failure.
*******************************************************************************/
int tlkmdi_audsrc_init(void);


int tlkmdi_audsrc_start(uint16 handle, uint32 param);
int tlkmdi_audsrc_close(uint16 handle);

void tlkmdi_audsrc_timer(void);

/******************************************************************************
 * Function: tlkmdi_audsrc_switch
 * Descript: Change A2DP SRC status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsrc_switch(uint16 handle, uint08 status);

bool tlkmdi_audsrc_fPlay(bool isRewind, bool isStart);

bool tlkmdi_audsrc_toNext(void);
bool tlkmdi_audsrc_toPrev(void);


/******************************************************************************
 * Function: tlkmdi_audsrc_isBusy
 * Descript: Is A2DP source block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_audsrc_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_audsrc_intval
 * Descript: Get A2DP source Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsrc_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audsrc_irqProc
 * Descript: Call A2DP SRC mp3 handler and fill handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsrc_irqProc(void);






#endif //#if (TLK_MDI_AUDSRC_ENABLE)

#endif //TLKMDI_SRC_H

