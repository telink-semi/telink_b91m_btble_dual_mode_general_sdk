/********************************************************************************************************
 * @file	tlkapp_dfu.h
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
#ifndef TLKAPP_DFU_H
#define TLKAPP_DFU_H


#define TLKAPP_DFU_MAX_SIZE             (0x200000)
#define TLKAPP_DFU_MIN_SIZE             (0x400)

#define TLKAPP_DFU_LOAD_INNER           1
#define TLKAPP_DFU_LOAD_OUTER           2

#define TLKAPP_DFU_START_FLAG           0x4B
#define TLKAPP_DFU_START_OFFSET         0x20
#define TLKAPP_DFU_START_ADDRESS        0x10000

#define TLKAPP_JUMP_TO_APP()        ((void(*)(void))(0x20000000 + TLKAPP_DFU_START_ADDRESS))()

#define TLKAPP_DFU_SAVE_SIGN        0x3A
#define TLKAPP_DFU_SAVE_VERS        0x03
#define TLKAPP_DFU_SAVE_ADDR        TLK_CFG_FLASH_OTA_PARAM_ADDR
#define TLKAPP_DFU_SAVE_SIZE        sizeof(tlkapp_file_Param_t)


typedef struct{ //32+80=112
	uint08 status;
	uint08 datPos; //Data Position: 1-Inner,
	uint08 optChn;
	uint08 cprSch; //Compress Scheme
	uint08 digSch; //Digest Scheme
	uint08 tranRole;
	uint08 dataPort;
	uint08 waitMask;
	uint16 fileType;
	uint16 unitLens;
	uint16 tranIntv;
	uint16 authSch;
	uint16 fastSch;
	uint16 cryptSch;
	uint32 dataAddr;
	uint32 dealSize; //RecvSize or SendSize
	uint32 fileSize;
	uint32 fileVers;
	uint08 authCode[16];
	uint08 fileSign[16];
	uint08 realSign[16];
	uint08 crypCode[16];
	uint08 fastCode[16];
}tlkapp_file_Param_t; //refer tlkmdi_file_saveParam_t
typedef struct{
	uint08 saveIsOK;
	uint08 reserve0;
	uint16 reserve1;
	uint32 checkDig;
	tlkapi_save_ctrl_t saveCtrl;
	tlkapp_file_Param_t saveParam;
}tlkapp_dfu_ctrl_t;



void tlkapp_dfu_load(void);





#endif /* TLKAPP_DFU_H */

