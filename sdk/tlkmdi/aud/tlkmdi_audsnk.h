/********************************************************************************************************
 * @file	tlkmdi_audsnk.h
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
#ifndef TLKMDI_SNK_H
#define TLKMDI_SNK_H


#define TLKMDI_SNK_DEC_TEMP_SIZE           4096
#define TLKMDI_SNK_RAW_RCV_BUFF_SIZE       10560 //SBC:128*n, AAC1024*n
#define TLKMDI_SNK_SBC_DEC_BUFF_SIZE       1248

//Total buffer = TEMP(4096)+RECV(10560)+DEC(1248)
//             = 4096+10560+1248 = 15904 + 3*16 = 15952


#define TLKMDI_SNK_SBC_FREQ_MASK      0xC0
#define TLKMDI_SNK_SBC_FREQ_16000     0x00
#define TLKMDI_SNK_SBC_FREQ_32000     0x40
#define TLKMDI_SNK_SBC_FREQ_44100     0x80
#define TLKMDI_SNK_SBC_FREQ_48000     0xC0

#define TLKMDI_SNK_SBC_BLOCK_MASK     0x30
#define TLKMDI_SNK_SBC_BLOCK_4        0x00
#define TLKMDI_SNK_SBC_BLOCK_8        0x10
#define TLKMDI_SNK_SBC_BLOCK_12       0x20
#define TLKMDI_SNK_SBC_BLOCK_16       0x30

#define TLKMDI_SNK_SBC_SUBBAND_MASK     0x01
#define TLKMDI_SNK_SBC_SUBBAND_4        0x00
#define TLKMDI_SNK_SBC_SUBBAND_8        0x10

#define TLKMDI_SNK_SBC_TYPE_MASK              0x0C
#define TLKMDI_SNK_SBC_TYPE_MONO              0x00
#define TLKMDI_SNK_SBC_TYPE_DUAL_MODE         0x04
#define TLKMDI_SNK_SBC_TYPE_STEREO            0x08
#define TLKMDI_SNK_SBC_TYPE_JOINT_STEREO      0x0C	


typedef int(*tlkmdi_snk_decFunc)(uint08 *pData, int len, uint08 *pOut);



/******************************************************************************
 * Function: tlkmdi_audsnk_init
 * Descript: Initial the A2DP sink block, register the data callback. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_audsnk_init(void);

int tlkmdi_audsnk_start(uint16 handle, uint32 param);
int tlkmdi_audsnk_close(uint16 handle);

void tlkmdi_audsnk_timer(void);

bool tlkmdi_audsnk_fPlay(bool isRewind, bool isStart);
bool tlkmdi_audsnk_toNext(void);
bool tlkmdi_audsnk_toPrev(void);

/******************************************************************************
 * Function: tlkmdi_audsnk_switch
 * Descript: Change A2DP SNK status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsnk_switch(uint16 handle, uint08 status);


/******************************************************************************
 * Function: tlkmdi_audsnk_isBusy
 * Descript: Is A2DP sink block enable. 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_audsnk_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_audsnk_intval
 * Descript: Get A2DP sink Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_audsnk_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audsnk_irqProc
 * Descript: Call A2DP SNK spk handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_audsnk_irqProc(void);



/******************************************************************************
 * Function: tlkmdi_snk_addEncFrame
 * Descript: A2DP SNK Encode the data. 
 * Params: 
 *        @pData[IN]--The data.
 *        @dataLen[IN]--The data length.
 * Return: None.
*******************************************************************************/
void tlkmdi_snk_addEncFrame(uint16 aclHandle, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_snk_setBuffer
 * Descript: A2DP SNK alloc buffer for audio data. 
 * Params: 
 *        @pBuffer[IN]--The data buffer.
 *        @buffLen[IN]--The buffer length.
 * Return: TLK_ENONE is success, others value is false.
*******************************************************************************/
int	 tlkmdi_snk_setBuffer(uint08 *pBuffer, uint32 buffLen);

uint tlkmdi_snk_getTimer(void);


#endif //TLKMDI_SNK_H

