/********************************************************************************************************
 * @file	tlkdev_key.c
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
#if (TLK_DEV_KEY_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkdrv/ext/key/tlkdrv_key.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev_key.h"
#include "drivers.h"


#define TLKDEV_KEY_DBG_FLAG       TLKDEV_SYS_DBG_FLAG
#define TLKDEV_KEY_DBG_SIGN       "[KEY]"

static int tlkdev_key_nodeInit(uint16 did);
static int tlkdev_key_nodeOpen(uint16 did, uint32 param);
static int tlkdev_key_nodeClose(uint16 did);
static int tlkdev_key_nodeState(uint16 did);
static int tlkdev_key_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_key_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_key_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);
static void tlkdev_key_nodeEvtCB(uint08 keyID, uint08 evtID, uint08 isPress);


#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
static const tlkdev_node_t scTlkDevKey1 = {
	TLKDEV_DID_KEY1,  //.devID;
	"KEY1",           //.pName;
	tlkdev_key_nodeInit,  //.Init
	tlkdev_key_nodeOpen,  //.Open
	tlkdev_key_nodeClose, //.Close
	tlkdev_key_nodeState, //.State
	tlkdev_key_nodeRead,  //.Read
	tlkdev_key_nodeWrite, //.Write
	tlkdev_key_nodeIoctl, //.Ioctl
};
static const tlkdev_node_t scTlkDevKey2 = {
	TLKDEV_DID_KEY2,  //.devID;
	"KEY2",           //.pName;
	tlkdev_key_nodeInit,  //.Init
	tlkdev_key_nodeOpen,  //.Open
	tlkdev_key_nodeClose, //.Close
	tlkdev_key_nodeState, //.State
	tlkdev_key_nodeRead,  //.Read
	tlkdev_key_nodeWrite, //.Write
	tlkdev_key_nodeIoctl, //.Ioctl
};
#endif

/******************************************************************************
 * Function: tlkdrv_key_init
 * Descript: Initial the timer of the key.
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkdev_key_init(void)
{
	tlkdrv_key_init();
	
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
		|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	tlkdev_appendEctypeNode(&scTlkDevKey1);
	tlkdev_appendEctypeNode(&scTlkDevKey2);
#endif
	
	return TLK_ENONE;
}
void tlkdev_key_deinit(void)
{
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
		|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	tlkdev_removeEctypeNode(TLKDEV_DID_KEY1);
	tlkdev_removeEctypeNode(TLKDEV_DID_KEY2);
#endif
}

bool tlkdev_key_isBusy(void)
{
	return tlkdrv_key_isBusy();
}
void tlkdev_key_enterSleep(uint mcuMode)
{
	tlkdrv_key_enterSleep(mcuMode);
}
void tlkdev_key_leaveSleep(uint wakeSrc)
{
	tlkdrv_key_leaveSleep(wakeSrc);
}



static int tlkdev_key_nodeInit(uint16 did)
{
	return TLK_ENONE;
}
static int tlkdev_key_nodeOpen(uint16 did, uint32 param)
{
#if (TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
	if(did == TLKDEV_DID_KEY1){
		return tlkdrv_key_insert(TLKDEV_DID_KEY1, TLKDEV_KEY_EVTMSK_ALL, GPIO_PA3, 0, GPIO_PIN_PULLUP_1M, tlkdev_key_nodeEvtCB);
	}else if(did == TLKDEV_DID_KEY2){
		return tlkdrv_key_insert(TLKDEV_DID_KEY2, TLKDEV_KEY_EVTMSK_ALL, GPIO_PA2, 0, GPIO_PIN_PULLUP_1M, tlkdev_key_nodeEvtCB);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2)
	if(did == TLKDEV_DID_KEY1){
		return tlkdrv_key_insert(TLKDEV_DID_KEY1, TLKDEV_KEY_EVTMSK_ALL, GPIO_PD5, 0, GPIO_PIN_PULLUP_1M, tlkdev_key_nodeEvtCB);
	}else if(did == TLKDEV_DID_KEY2){
		return tlkdrv_key_insert(TLKDEV_DID_KEY2, TLKDEV_KEY_EVTMSK_ALL, GPIO_PE7, 0, GPIO_PIN_PULLUP_1M, tlkdev_key_nodeEvtCB);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
	if(did == TLKDEV_DID_KEY1){
		return tlkdrv_key_insert(TLKDEV_DID_KEY1, TLKDEV_KEY_EVTMSK_ALL, GPIO_PD5, 0, GPIO_PIN_PULLUP_1M, tlkdev_key_nodeEvtCB);
	}else if(did == TLKDEV_DID_KEY2){
		return tlkdrv_key_insert(TLKDEV_DID_KEY2, TLKDEV_KEY_EVTMSK_ALL, GPIO_PE7, 0, GPIO_PIN_PULLUP_1M, tlkdev_key_nodeEvtCB);
	}	
#endif
	return -TLK_ENOSUPPORT;
}
static int tlkdev_key_nodeClose(uint16 did)
{
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) \
		|| (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0))
	if(did == TLKDEV_DID_KEY1){
		return tlkdrv_key_remove(TLKDEV_DID_KEY1, GPIO_PIN_UP_DOWN_FLOAT, false);
	}else if(did == TLKDEV_DID_KEY2){
		return tlkdrv_key_remove(TLKDEV_DID_KEY2, GPIO_PIN_UP_DOWN_FLOAT, false);
	}
#endif
	return -TLK_ENOSUPPORT;
}
static int tlkdev_key_nodeState(uint16 did)
{
	tlkdrv_key_unit_t *pUnit = tlkdrv_key_getUsedUnit(did);
	if(pUnit == nullptr) return TLKDEV_STATE_CLOSED;
	if(pUnit->flags == TLKDRV_KEY_FLAG_NONE) return TLKDEV_STATE_OPENED;
	return TLKDEV_STATE_BUSYING;
}
static int tlkdev_key_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_key_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_key_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdrv_key_enterSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdrv_key_leaveSleep(param0);
		return TLK_ENONE;
	}
	return -TLK_ENOSUPPORT;
}
static void tlkdev_key_nodeEvtCB(uint08 keyID, uint08 evtID, uint08 isPress)
{
	uint08 buffer[3] = {keyID, evtID, isPress};
//	tlkapi_trace(TLKDEV_KEY_DBG_FLAG, TLKDEV_KEY_DBG_SIGN,
//			"tlkdev_key_nodeEvtCB: keyID-%d, evtID-%d, isPress-%d",
//			keyID, evtID, isPress);
	tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_DEVICE, TLKPTI_DEV_MSGID_KEY_EVENT_REPORT, buffer, 3);
}



#endif //#if (TLK_DEV_KEY_ENABLE)

