/********************************************************************************************************
 * @file	tlkapp_dfu1.c
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
/********************************************************************************************************
 * Notice: DFU1 is only suitable for ordinary burning mode, in order to solve
 *         the problem of slow burning of BDT. The normal release version of
 *         the program is strictly prohibited to use this method to upgrade,
 *         because there is no verification of the firmware after burning.
 *******************************************************************************************************/
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapp_config.h"
#include "tlkapp.h"
#if (TLKAPP_DFU1_ENABLE)
#include "tlkapp_dfu.h"
#include "tlksys/prt/tlkpto_comm.h"
#include "drivers.h"
#include "tlkapp_serial.h"
#include "tlkapp_dfu1.h"

extern unsigned int tlkcfg_getFLashCap(void);

static tlkapp_dfu1_t sTlkAppDfu1Ctrl;
static void tlkapp_dfu1_recvCmdCB(uint08 mtype, uint08 cmdID, uint08 *pData, uint16 dataLen);
static void tlkapp_dfu1_recvDatCB(uint08 dataID, uint32 numb, uint08 *pData, uint16 dataLen);

static void tlkapp_dfu1_sendStartRspDeal(void);
static void tlkapp_dfu1_sendFlashRspDeal(void);
static void tlkapp_dfu1_sendFSyncRspDeal(bool isStall);
static void tlkapp_dfu1_sendCloseRspDeal(void);

static void tlkapp_dfu1_recvStartCmdDeal(void);
static void tlkapp_dfu1_recvFlashCmdDeal(uint08 *pData, uint16 dataLen);
static void tlkapp_dfu1_recvCloseCmdDeal(uint08 *pData, uint16 dataLen);



int tlkapp_dfu1_init(void)
{
	tmemset(&sTlkAppDfu1Ctrl, 0, sizeof(tlkapp_dfu1_t));
	
	tlkapp_serial_init();
	tlkapp_serial_regCmdCB(tlkapp_dfu1_recvCmdCB);
	tlkapp_serial_regDatCB(tlkapp_dfu1_recvDatCB);
	
	return TLK_ENONE;
}

void tlkapp_duf1_open(void)
{
	tlkapp_serial_open();
	sTlkAppDfu1Ctrl.isOver = false;
	sTlkAppDfu1Ctrl.idleTimer = clock_time()|1;
	sTlkAppDfu1Ctrl.busyTimer = 0;
}

void tlkapp_duf1_close(void)
{
	tlkapp_serial_close();
	sTlkAppDfu1Ctrl.idleTimer = 0;
	sTlkAppDfu1Ctrl.busyTimer = 0;
}

bool tlkapp_duf1_isBusy(void)
{
	if(!sTlkAppDfu1Ctrl.isOver && sTlkAppDfu1Ctrl.idleTimer != 0 
		&& clock_time_exceed(sTlkAppDfu1Ctrl.idleTimer, TLKAPP_DFU1_IDLE_TIMEOUT)){
		sTlkAppDfu1Ctrl.isOver = true;
	}
	if(sTlkAppDfu1Ctrl.isOver) return false;
	return true;
}

void tlkapp_dfu1_handler(void)
{
	if(sTlkAppDfu1Ctrl.idleTimer != 0 && clock_time_exceed(sTlkAppDfu1Ctrl.idleTimer,
		TLKAPP_DFU1_IDLE_TIMEOUT)){
		sTlkAppDfu1Ctrl.isOver = true;
	}
	if(sTlkAppDfu1Ctrl.busyTimer != 0 && clock_time_exceed(sTlkAppDfu1Ctrl.busyTimer,
		TLKAPP_DFU1_BUSY_TIMEOUT)){
		sTlkAppDfu1Ctrl.isOver = true;
		sTlkAppDfu1Ctrl.idleTimer = 0;
		sTlkAppDfu1Ctrl.busyTimer = 0;
	}
	if(sTlkAppDfu1Ctrl.stallTimer != 0 && clock_time_exceed(sTlkAppDfu1Ctrl.stallTimer, 100000)){
		sTlkAppDfu1Ctrl.stallTimer = 0;
		tlkapp_dfu1_sendFSyncRspDeal(true);
	}
	tlkapp_serial_handler();
}


static void tlkapp_dfu1_recvCmdCB(uint08 mtype, uint08 cmdID, uint08 *pData, uint16 dataLen)
{
	if(mtype != TLKPRT_COMM_MTYPE_DBG || cmdID != TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE || dataLen == 0){
		return;
	}

	if(pData[0] == TLKAPP_DFU1_MSGID_START){
		tlkapp_dfu1_recvStartCmdDeal();
	}else if(pData[0] == TLKAPP_DFU1_MSGID_FLASH){
		tlkapp_dfu1_recvFlashCmdDeal(pData+1, dataLen-1);
	}else if(pData[0] == TLKAPP_DFU1_MSGID_CLOSE){
		tlkapp_dfu1_recvCloseCmdDeal(pData+1, dataLen-1);
	}else if(pData[0] == TLKAPP_DFU1_MSGID_REBOOT){
		sTlkAppDfu1Ctrl.isOver = true;
		sTlkAppDfu1Ctrl.idleTimer = 0;
	}
}

static void tlkapp_dfu1_recvDatCB(uint08 dataID, uint32 numb, uint08 *pData, uint16 dataLen)
{
	uint32 offset;
	
	if(dataID != TLKPRT_COMM_SYS_DAT_PORT) return;
	if(sTlkAppDfu1Ctrl.stage != TLKAPP_DFU1_STAGE_DATA) return;

	if(pData[0] != TLKPRT_COMM_SYS_DAT_FW || dataLen == 0){
		return;
	}else{
		pData += 1;
		dataLen -= 1;
	}
	sTlkAppDfu1Ctrl.busyTimer = clock_time() | 1;
	
	if(((sTlkAppDfu1Ctrl.recvNumb+1) & 0xFFFF) < numb){
		if(sTlkAppDfu1Ctrl.stallTimer == 0){
			tlkapp_serial_reset();
			tlkapp_dfu1_sendFSyncRspDeal(true);
			sTlkAppDfu1Ctrl.stallTimer = clock_time()|1;
		}
		return;
	}
	if(((sTlkAppDfu1Ctrl.recvNumb+1) & 0xFFFFF) != numb){
		return;
	}
	if(sTlkAppDfu1Ctrl.writeOffs+dataLen > sTlkAppDfu1Ctrl.writeLens){
		tlkapp_dfu1_sendFSyncRspDeal(false);
		return;
	}

	if(sTlkAppDfu1Ctrl.stallTimer != 0) sTlkAppDfu1Ctrl.stallTimer = 0;

//	if((sTlkAppDfu1Ctrl.writeOffs & 0xFFF) == 0){
//		flash_erase_sector(sTlkAppDfu1Ctrl.startAddr+sTlkAppDfu1Ctrl.writeOffs);
//	}

	offset = sTlkAppDfu1Ctrl.writeOffs;
	sTlkAppDfu1Ctrl.recvNumb = numb;
	sTlkAppDfu1Ctrl.writeOffs += dataLen;
	tlkapp_dfu1_sendFSyncRspDeal(false);
	
	tlkapi_flash_write(sTlkAppDfu1Ctrl.startAddr+offset, pData, dataLen);
}

static void tlkapp_dfu1_recvStartCmdDeal(void)
{
	sTlkAppDfu1Ctrl.isEarse = 0;
	sTlkAppDfu1Ctrl.stage = TLKAPP_DFU1_STAGE_START;
	sTlkAppDfu1Ctrl.recvNumb = 0;
	sTlkAppDfu1Ctrl.busyTimer = clock_time() | 1;
	sTlkAppDfu1Ctrl.idleTimer = 0;
	sTlkAppDfu1Ctrl.stallTimer = 0;
	
	tlkapp_dfu1_sendStartRspDeal();
	
}
static void tlkapp_dfu1_recvFlashCmdDeal(uint08 *pData, uint16 dataLen)
{
	uint32 index;
	uint32 fwSize;
	uint32 offset;
	
	if(sTlkAppDfu1Ctrl.stage != TLKAPP_DFU1_STAGE_START) return;
	if(dataLen < 8) return;
	
	ARRAY_TO_UINT32L(pData, 0, fwSize);
	ARRAY_TO_UINT32L(pData, 4, offset);
	if(offset < TLKAPP_DFU_START_ADDRESS){
		offset = TLKAPP_DFU_START_ADDRESS;
	}
	
	if((offset & 0x0FFF) != 0) return; //4096
	if(offset+fwSize >= tlkcfg_getFLashCap()) return;

	if(!sTlkAppDfu1Ctrl.isEarse){
		sTlkAppDfu1Ctrl.isEarse = true;
		for(index=0; index<fwSize+4095; index+=4096){
			flash_erase_sector(offset+index);
		}
	}

	sTlkAppDfu1Ctrl.stage = TLKAPP_DFU1_STAGE_DATA;
	sTlkAppDfu1Ctrl.recvNumb = 0;
	sTlkAppDfu1Ctrl.startAddr = offset;
	sTlkAppDfu1Ctrl.writeLens = fwSize;
	sTlkAppDfu1Ctrl.writeOffs = 0;
	tlkapp_dfu1_sendFlashRspDeal();

	tlkapp_dfu1_sendFSyncRspDeal(false);
}
static void tlkapp_dfu1_recvCloseCmdDeal(uint08 *pData, uint16 dataLen)
{
	sTlkAppDfu1Ctrl.stage = TLKAPP_DFU1_STAGE_NONE;
	sTlkAppDfu1Ctrl.isOver = true;
	tlkapp_dfu1_sendCloseRspDeal();
	delay_ms(100);
}

static void tlkapp_dfu1_sendStartRspDeal(void)
{
	uint08 buffer[8];
	uint08 buffLen = 0;
	
	buffer[buffLen++] = TLKAPP_DFU1_MSGID_START; //Msg ID
	buffer[buffLen++] = (TLKAPP_SERIAL_RECV_DATA_SIZE & 0xFF); //Maximum length of a single packet
	buffer[buffLen++] = (TLKAPP_SERIAL_RECV_DATA_SIZE & 0xFF00) >> 8;
	tlkapp_serial_sendRsp(TLKPRT_COMM_MTYPE_DBG, TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE,
		TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
}
static void tlkapp_dfu1_sendFlashRspDeal(void)
{
	uint08 buffer[8];
	uint08 buffLen = 0;
	
	buffer[buffLen++] = TLKAPP_DFU1_MSGID_FLASH; //Msg ID
	tlkapp_serial_sendRsp(TLKPRT_COMM_MTYPE_DBG, TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE,
		TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
}
static void tlkapp_dfu1_sendFSyncRspDeal(bool isStall)
{
	uint08 buffer[12];
	uint08 buffLen = 0;
	
	buffer[buffLen++] = TLKAPP_DFU1_MSGID_FSYNC; //Msg ID
	buffer[buffLen++] = isStall;
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.recvNumb & 0xFF); //Recv Number
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.recvNumb & 0xFF00) >> 8;
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.recvNumb & 0xFF0000) >> 16;
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.recvNumb & 0xFF000000) >> 24;
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.writeOffs & 0xFF); //Flash Offset
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.writeOffs & 0xFF00) >> 8;
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.writeOffs & 0xFF0000) >> 16;
	buffer[buffLen++] = (sTlkAppDfu1Ctrl.writeOffs & 0xFF000000) >> 24;
	tlkapp_serial_sendRsp(TLKPRT_COMM_MTYPE_DBG, TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE,
		TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
}
static void tlkapp_dfu1_sendCloseRspDeal(void)
{
	uint08 buffer[8];
	uint08 buffLen = 0;
	
	buffer[buffLen++] = TLKAPP_DFU1_MSGID_CLOSE; //Msg ID
	if(sTlkAppDfu1Ctrl.writeLens != 0 && sTlkAppDfu1Ctrl.writeOffs == sTlkAppDfu1Ctrl.writeLens){
		tlkapp_serial_sendRsp(TLKPRT_COMM_MTYPE_DBG, TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE,
			TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, buffLen);
	}else{
		tlkapp_serial_sendRsp(TLKPRT_COMM_MTYPE_DBG, TLKPRT_COMM_CMDID_DBG_FIRMWARE_UPDATE,
			TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, buffer, buffLen);
	}
}


#endif //#if (TLKAPP_DFU1_ENABLE)

