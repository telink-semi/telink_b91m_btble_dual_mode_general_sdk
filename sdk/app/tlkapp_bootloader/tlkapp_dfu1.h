/********************************************************************************************************
 * @file	tlkapp_dfu1.h
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
/********************************************************************************************************
 * Notice: DFU1 is only suitable for ordinary burning mode, in order to solve
 *         the problem of slow burning of BDT. The normal release version of
 *         the program is strictly prohibited to use this method to upgrade,
 *         because there is no verification of the firmware after burning.
 *******************************************************************************************************/
#ifndef TLKAPP_DFU1_H
#define TLKAPP_DFU1_H

#if (TLKAPP_DFU1_ENABLE)


#define TLKAPP_DFU1_IDLE_TIMEOUT      500000 //us
#define TLKAPP_DFU1_BUSY_TIMEOUT      5000000 //us

typedef enum{
	TLKAPP_DFU1_STAGE_NONE = 0,
	TLKAPP_DFU1_STAGE_START = 1,
	TLKAPP_DFU1_STAGE_DATA  = 2,
}TLKAPP_DFU1_STAGE_ENUM;

typedef enum{
	TLKAPP_DFU1_MSGID_NONE   = 0x00,
	TLKAPP_DFU1_MSGID_START  = 0x01,
	TLKAPP_DFU1_MSGID_FLASH  = 0x02, //Erase Flash
	TLKAPP_DFU1_MSGID_FSYNC  = 0x03, //Fast Sync
	TLKAPP_DFU1_MSGID_CLOSE  = 0x04, //
	TLKAPP_DFU1_MSGID_REBOOT = 0x05,
}TLKAPP_DFU1_MSGID_ENUM;


typedef struct{
	uint08 isOver;
	uint08 stage;
	uint08 isEarse;
	uint08 reseved;
	uint32 recvNumb;
	uint32 writeOffs;
	uint32 writeLens;
	uint32 startAddr;
	uint32 idleTimer;
	uint32 busyTimer;
	uint32 stallTimer;
}tlkapp_dfu1_t;


int tlkapp_dfu1_init(void);

void tlkapp_duf1_open(void);
void tlkapp_duf1_close(void);

bool tlkapp_duf1_isBusy(void);

void tlkapp_dfu1_handler(void);



#endif //#if (TLKAPP_DFU1_ENABLE)

#endif /* TLKAPP_DFU1_H */

