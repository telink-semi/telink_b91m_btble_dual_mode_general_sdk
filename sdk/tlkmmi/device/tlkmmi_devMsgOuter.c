/********************************************************************************************************
 * @file	tlkmmi_devMsgOuter.c
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
#if (TLKMMI_DEVICE_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_dev.h"
#include "tlkmmi_devMsgOuter.h"
#include "tlkdev/tlkdev.h"


static int tlkmmi_device_recvMountDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_device_recvUnmountDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_device_recvOpenDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_device_recvCloseDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_device_recvReadDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_device_recvWriteDeal(uint08 *pData, uint08 dataLen);
static int tlkmmi_device_recvIoctlDeal(uint08 *pData, uint08 dataLen);


int tlkmmi_device_outerMsgHandler(uint08 msgID, uint08 *pData, uint08 dataLen)
{
	tlkapi_trace(TLKMMI_DEV_DBG_FLAG, TLKMMI_DEV_DBG_SIGN, "tlkmmi_device_outerMsgHandler: msgID-%d dataLen-%d", msgID, dataLen);
	
	if(pData == nullptr || dataLen < 2) return -TLK_EPARAM;
	
	if(msgID == TLKPRT_COMM_CMDID_DEV_MOUNT){
		return tlkmmi_device_recvMountDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DEV_UNMOUNT){
		return tlkmmi_device_recvUnmountDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DEV_OPEN){
		return tlkmmi_device_recvOpenDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DEV_CLOSE){
		return tlkmmi_device_recvCloseDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DEV_READ){
		return tlkmmi_device_recvReadDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DEV_WRITE){
		return tlkmmi_device_recvWriteDeal(pData, dataLen);
	}else if(msgID == TLKPRT_COMM_CMDID_DEV_IOCTL){
		return tlkmmi_device_recvIoctlDeal(pData, dataLen);
	}else{
		tlkmmi_device_sendCommRsp(msgID, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_ENOSUPPORT, nullptr, 0);
	}
	
	return TLK_ENONE;
}


static int tlkmmi_device_recvMountDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;

	did = ((pData[1] & 0xFF) << 8) | pData[0];
	if(tlkdev_mount(did) == TLK_ENONE){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_MOUNT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_MOUNT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
	return TLK_ENONE;
}
static int tlkmmi_device_recvUnmountDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;
	
	did = ((pData[1] & 0xFF) << 8) | pData[0];
	if(tlkdev_unmount(did) == TLK_ENONE){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_UNMOUNT, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_UNMOUNT, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
	return TLK_ENONE;
}
static int tlkmmi_device_recvOpenDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;
	uint32 param;
	
	if(dataLen < 6) return -TLK_EPARAM;

	did = ((pData[1] & 0xFF) << 8) | pData[0];
	param = ((pData[5] & 0xFF) << 24) | ((pData[4] & 0xFF) << 16) | ((pData[3] & 0xFF) << 8) | pData[2];
	if(tlkdev_open(did, param) == TLK_ENONE){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_OPEN, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_OPEN, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
	return TLK_ENONE;
}
static int tlkmmi_device_recvCloseDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;
	
	did = ((pData[1] & 0xFF) << 8) | pData[0];
	if(tlkdev_close(did) == TLK_ENONE){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_CLOSE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_CLOSE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}
	return TLK_ENONE;
}
static int tlkmmi_device_recvReadDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;
	uint32 param;
	uint32 readLen;

	did = ((pData[1] & 0xFF) << 8) | pData[0];
	param = ((pData[5] & 0xFF) << 24) | ((pData[4] & 0xFF) << 16) | ((pData[3] & 0xFF) << 8) | pData[2];
	readLen = ((pData[9] & 0xFF) << 24) | ((pData[8] & 0xFF) << 16) | ((pData[7] & 0xFF) << 8) | pData[6];
	if(readLen == 0) return -TLK_EPARAM;
	if(readLen > 512) return -TLK_EOVERFLOW;
	
	uint08 buffer[readLen];
	if(tlkdev_read(did, param, buffer, readLen) < 0){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_READ, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_READ, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, buffer, readLen);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_device_recvWriteDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;
	uint32 param;
		
	if(dataLen < 6) return -TLK_EPARAM;

	did = ((pData[1] & 0xFF) << 8) | pData[0];
	param = ((pData[5] & 0xFF) << 24) | ((pData[4] & 0xFF) << 16) | ((pData[3] & 0xFF) << 8) | pData[2];
	if(tlkdev_write(did, param, pData+6, dataLen-6) < 0){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_WRITE, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_WRITE, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
	
	return TLK_ENONE;
}
static int tlkmmi_device_recvIoctlDeal(uint08 *pData, uint08 dataLen)
{
	uint16 did;
	uint16 opcode;
	uint32 param;
	uint32 param1;
	
	if(dataLen < 12) return -TLK_EPARAM;

	did = ((pData[1] & 0xFF) << 8) | pData[0];
	opcode = ((pData[3] & 0xFF) << 8) | pData[2];
	param = ((pData[7] & 0xFF) << 24) | ((pData[6] & 0xFF) << 16) | ((pData[5] & 0xFF) << 8) | pData[4];
	param1 = ((pData[11] & 0xFF) << 24) | ((pData[10] & 0xFF) << 16) | ((pData[9] & 0xFF) << 8) | pData[8];
	if(tlkdev_ioctl(did, opcode, param, param1) < 0){
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_IOCTL, TLKPRT_COMM_RSP_STATUE_FAILURE, TLK_EFAIL, nullptr, 0);
	}else{
		tlkmmi_device_sendCommRsp(TLKPRT_COMM_CMDID_DEV_IOCTL, TLKPRT_COMM_RSP_STATUE_SUCCESS, TLK_ENONE, nullptr, 0);
	}
	return TLK_ENONE;
}



#endif  // (TLKMMI_DEVICE_ENABLE)
