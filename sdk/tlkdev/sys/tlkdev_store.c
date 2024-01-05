/********************************************************************************************************
 * @file	tlkdev_store.c
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
#include "tlkapi/tlkapi_stdio.h"
#if (TLK_DEV_STORE_ENABLE)
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdrv/ext/store/tlkdrv_store.h"
#include "tlkdev/sys/tlkdev_store.h"



static int tlkdev_store_nodeInit(uint16 did);
static int tlkdev_store_nodeOpen(uint16 did, uint32 param);
static int tlkdev_store_nodeClose(uint16 did);
static int tlkdev_store_nodeState(uint16 did);
static int tlkdev_store_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_store_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_store_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);


static const tlkdev_node_t scTlkDevSdcard1 = {
	TLKDEV_DID_SDCARD0,  //.devID;
	"sdcard0",           //.pName;
	tlkdev_store_nodeInit,  //.Init
	tlkdev_store_nodeOpen,  //.Open
	tlkdev_store_nodeClose, //.Close
	tlkdev_store_nodeState, //.State
	tlkdev_store_nodeRead,  //.Read
	tlkdev_store_nodeWrite, //.Write
	tlkdev_store_nodeIoctl, //.Ioctl
};


int tlkdev_store_init(void)
{
	tlkdev_appendEctypeNode(&scTlkDevSdcard1);
	return TLK_ENONE;
}
void tlkdev_store_deinit(void)
{
	tlkdev_removeEctypeNode(TLKDEV_DID_SDCARD0);
}


void tlkdev_store_enterSleep(uint mcuMode)
{
	tlkdrv_store_shutDown(TLKDRV_STORE_DEV_XTSD01G);
}
void tlkdev_store_leaveSleep(uint wakeSrc)
{
	
}

static int tlkdev_store_nodeInit(uint16 did)
{
	if(did == TLKDEV_DID_SDCARD0){
		int ret;
		ret = tlkdrv_store_init(TLKDRV_STORE_DEV_XTSD01G);
		if(ret == TLK_ENONE) ret = tlkdrv_store_open(TLKDRV_STORE_DEV_XTSD01G);
		if(ret == TLK_ENONE) ret = tlkdrv_store_sdInit(TLKDRV_STORE_DEV_XTSD01G);
		tlkdrv_store_close(TLKDRV_STORE_DEV_XTSD01G);
		return ret;
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static int tlkdev_store_nodeOpen(uint16 did, uint32 param)
{
	if(did == TLKDEV_DID_SDCARD0){
		return tlkdrv_store_open(TLKDRV_STORE_DEV_XTSD01G);
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static int tlkdev_store_nodeClose(uint16 did)
{
	if(did == TLKDEV_DID_SDCARD0){
		return tlkdrv_store_close(TLKDRV_STORE_DEV_XTSD01G);
	}else{
		return -TLK_ENOSUPPORT;
	}
}
static int tlkdev_store_nodeState(uint16 did)
{
	if(did == TLKDEV_DID_SDCARD0){
		if(tlkdrv_store_isOpen(TLKDRV_STORE_DEV_XTSD01G)){
			return TLKDEV_STATE_OPENED;
		}
		return TLKDEV_STATE_CLOSED;
	}else{
		return TLKDEV_STATE_CLOSED;
	}
}
static int tlkdev_store_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	if(did == TLKDEV_DID_SDCARD0){
		return tlkdrv_store_read(TLKDRV_STORE_DEV_XTSD01G, param, pBuff, buffLen);
	}else{
		return TLKDEV_STATE_CLOSED;
	}
}
static int tlkdev_store_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	if(did == TLKDEV_DID_SDCARD0){
		return tlkdrv_store_write(TLKDRV_STORE_DEV_XTSD01G, param, pData, dataLen);
	}else{
		return TLKDEV_STATE_CLOSED;
	}
}
static int tlkdev_store_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	uint16 drvDev;
	
	if(did == TLKDEV_DID_SDCARD0){
		drvDev = TLKDRV_STORE_DEV_XTSD01G; 
	}else{
		return -TLK_ENOSUPPORT;
	}

	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdev_store_enterSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdev_store_leaveSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_STORE_ERASE){
		return tlkdrv_store_erase(drvDev, param0, param1);
	}else if(opcode == TLKDEV_OPCODE_STORE_FORMAT){
		return tlkdrv_store_format(drvDev);
	}else if(opcode == TLKDEV_OPCODE_STORE_GET_PAGE_SIZE){
		return tlkdrv_store_getPageSize(drvDev);
	}else if(opcode == TLKDEV_OPCODE_STORE_GET_PAGE_NUMB){
		return tlkdrv_store_getPageNumb(drvDev);
	}else if(opcode == TLKDEV_OPCODE_STORE_GET_SECTOR_SIZE){
		return tlkdrv_store_getSectorSize(drvDev);
	}else if(opcode == TLKDEV_OPCODE_STORE_GET_SECTOR_NUMB){
		return tlkdrv_store_getSectorNumb(drvDev);
	}else if(opcode == TLKDEV_OPCODE_STORE_GET_BLOCK_SIZE){
		return tlkdrv_store_getSdBlockSize(drvDev);
	}else if(opcode == TLKDEV_OPCODE_STORE_GET_BLOCK_NUMB){
		return tlkdrv_store_getSdBlockNumb(drvDev);
	}
	
	return -TLK_ENOSUPPORT;
}


#endif //#if (TLK_DEV_STORE_ENABLE)

