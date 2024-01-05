/********************************************************************************************************
 * @file	tlkdev_touch.c
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
#if (TLK_DEV_TOUCH_ENABLE)
#include "tlkdrv/ext/touch/tlkdrv_touch.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev/sys/tlkdev_touch.h"



#define TLKDEV_TOUCH_DEV     TLKDRV_TOUCH_DEV_FT3168

static int tlkdev_touch_nodeInit(uint16 did);
static int tlkdev_touch_nodeOpen(uint16 did, uint32 param);
static int tlkdev_touch_nodeClose(uint16 did);
static int tlkdev_touch_nodeState(uint16 did);
static int tlkdev_touch_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_touch_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_touch_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);

static const tlkdev_node_t scTlkDevTouch = {
	TLKDEV_DID_TOUCH,  //.devID;
	"Touch",           //.pName;
	tlkdev_touch_nodeInit,  //.Init
	tlkdev_touch_nodeOpen,  //.Open
	tlkdev_touch_nodeClose, //.Close
	tlkdev_touch_nodeState, //.State
	tlkdev_touch_nodeRead,  //.Read
	tlkdev_touch_nodeWrite, //.Write
	tlkdev_touch_nodeIoctl, //.Ioctl
};


int tlkdev_touch_init(void)
{
	tlkdev_appendEctypeNode(&scTlkDevTouch);
	return tlkdrv_touch_init(TLKDEV_TOUCH_DEV);
}
void tlkdev_touch_deinit(void)
{
	tlkdev_removeEctypeNode(TLKDEV_DID_TOUCH);
}


bool tlkdev_touch_isOpen(void)
{
	return tlkdrv_touch_isOpen(TLKDEV_TOUCH_DEV);
}
bool tlkdev_touch_isReady(void)
{
	return tlkdrv_touch_isReady(TLKDEV_TOUCH_DEV);
}

int tlkdev_touch_open(void)
{
	return tlkdrv_touch_open(TLKDEV_TOUCH_DEV);
}
int tlkdev_touch_close(void)
{
	return tlkdrv_touch_close(TLKDEV_TOUCH_DEV);
}
int tlkdev_touch_getPoint(uint16 *pEvtID, tlkdev_touch_point_t *pPoint)
{
	int ret;
	tlkdrv_touch_point_t point;
	ret = tlkdrv_touch_getPoint(TLKDEV_TOUCH_DEV, &point);
	if(ret == TLK_ENONE){
		if(pEvtID != nullptr) *pEvtID = point.evtID;
		if(pPoint != nullptr){
			pPoint->axisX = point.axisX;
			pPoint->axisY = point.axisY;
		}
	}
	return ret;
}
int tlkdev_touch_getGesture(uint08 *pGstID, uint08 *pNumb, tlkdev_touch_point_t *pPoints, uint08 pointNumb)
{
	int ret;
	tlkdrv_touch_gesture_t gest;
	
	ret = tlkdrv_touch_getGesture(TLKDEV_TOUCH_DEV, &gest);
	if(ret == TLK_ENONE){
		uint08 index;
		if(pGstID != nullptr) *pGstID = gest.gestID;
		if(pNumb != nullptr) *pNumb = gest.number;
		if(pPoints == nullptr) pointNumb = 0;
		for(index=0; index<gest.number && index<pointNumb; index++){
			pPoints[index].axisX = gest.axisX[index];
			pPoints[index].axisY = gest.axisY[index];
		}
	}
	return ret;
}
int tlkdev_touch_handler(uint16 opcode, uint32 param0, uint32 param1)
{
	return tlkdrv_touch_handler(TLKDEV_TOUCH_DEV, opcode, param0, param1);
}


int tlkdev_touch_calcGesture(uint16 x, uint16 y, TLKDEV_TOUCH_ACTID_ENUM actID)
{
	return -TLK_ENOSUPPORT;
}



static int tlkdev_touch_nodeInit(uint16 did)
{
	return TLK_ENONE;
}
static int tlkdev_touch_nodeOpen(uint16 did, uint32 param)
{
	return tlkdrv_touch_open(TLKDEV_TOUCH_DEV);
}
static int tlkdev_touch_nodeClose(uint16 did)
{
	return tlkdrv_touch_close(TLKDEV_TOUCH_DEV);
}
static int tlkdev_touch_nodeState(uint16 did)
{
	if(tlkdrv_touch_isReady(TLKDEV_TOUCH_DEV)){
		return TLKDEV_STATE_BUSYING;
	}else if(tlkdrv_touch_isOpen(TLKDEV_TOUCH_DEV)){
		return TLKDEV_STATE_OPENED;
	}
	return TLKDEV_STATE_CLOSED;
}
static int tlkdev_touch_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	
	return -TLK_ENOSUPPORT;
}
static int tlkdev_touch_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_touch_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	
	return -TLK_ENOSUPPORT;
}



#endif //#if (TLK_DEV_TOUCH_ENABLE)

