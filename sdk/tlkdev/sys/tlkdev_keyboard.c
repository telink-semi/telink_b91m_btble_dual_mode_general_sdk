/********************************************************************************************************
 * @file	tlkdev_keyboard.c
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
#if (TLKDEV_KEYBOARD_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkdrv/ext/keyboard/tlkdrv_keyboard.h"
#include "tlkdev_keyboard.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "drivers.h"

#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))
static int tlkdev_keyboard_nodeInit(uint16 did);
static int tlkdev_keyboard_nodeOpen(uint16 did, uint32 param);
static int tlkdev_keyboard_nodeClose(uint16 did);
static int tlkdev_keyboard_nodeState(uint16 did);
static int tlkdev_keyboard_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_keyboard_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_keyboard_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);
#endif

static void tlkdev_keyboard_handler(void);
static bool tlkdrv_keyboard_timer(tlkapi_timer_t *pTimer, uint32 userArg);
static void tlkdev_keyboard_changedDeal(uint08 *pNewKey, uint08 newNumb);
static bool tlkdev_keyboard_isInList(uint08 keycode, uint08 *pKeyList, uint08 listNumb);


#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))
static const tlkdev_node_t scTlkDevKeyboard = {
	TLKDEV_DID_KEYBOARD,  //.devID;
	"Keyboard",           //.pName;
	tlkdev_keyboard_nodeInit,  //.Init
	tlkdev_keyboard_nodeOpen,  //.Open
	tlkdev_keyboard_nodeClose, //.Close
	tlkdev_keyboard_nodeState, //.State
	tlkdev_keyboard_nodeRead,  //.Read
	tlkdev_keyboard_nodeWrite, //.Write
	tlkdev_keyboard_nodeIoctl, //.Ioctl
};
#endif

#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))
static const uint16 sTlkDevKeyboardColPins[TLKDEV_KEYBOARD_COL_NUMBER] = TLKDEV_KEYBOARD_COL_PINS; //AS driver pin
static const uint16 sTlkDevKeyboardRowPins[TLKDEV_KEYBOARD_ROW_NUMBER] = TLKDEV_KEYBOARD_ROW_PINS; //As scan pin
static const uint08 sTlkDevKeyboardMap[TLKDEV_KEYBOARD_COL_NUMBER*TLKDEV_KEYBOARD_ROW_NUMBER] = TLKDEV_KEYBOARD_VALUE_MAP;
#endif

static tlkdev_keyboard_t sTlkDevKBCtrl;


int tlkdev_keyboard_init(void)
{
	tmemset(&sTlkDevKBCtrl, 0, sizeof(tlkdev_keyboard_t));
	sTlkDevKBCtrl.evtMask = TLKDEV_KEYBOARD_EVTMSK_ALL;

	tlkdrv_keyboard_init();
	tlkapi_timer_initNode(&sTlkDevKBCtrl.timer, tlkdrv_keyboard_timer, (uint32)&sTlkDevKBCtrl.timer, TLKDEV_KEYBORAD_TIMEOUT0);
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))
	tlkdev_appendEctypeNode(&scTlkDevKeyboard);
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}
void tlkdev_keyboard_deinit(void)
{
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))
	tlkdev_removeEctypeNode(TLKDEV_DID_KEYBOARD);
#endif
}

bool tlkdev_keyboard_isBusy(void)
{
	if(sTlkDevKBCtrl.isOpen && sTlkDevKBCtrl.keyNumb != 0) return true;
	return false;
}
void tlkdev_keyboard_enterSleep(uint mcuMode)
{
	if(sTlkDevKBCtrl.isOpen){
		tlkapi_timer_removeNode(&sTlkDevKBCtrl.timer);
	}
}
void tlkdev_keyboard_leaveSleep(uint wakeSrc)
{
	if(sTlkDevKBCtrl.isOpen){
		tlkapi_timer_insertNode(&sTlkDevKBCtrl.timer);
	}
}

static bool tlkdrv_keyboard_timer(tlkapi_timer_t *pTimer, uint32 userArg)
{
	if(!sTlkDevKBCtrl.isOpen) return false;
	
	tlkdev_keyboard_handler();
	
	if(sTlkDevKBCtrl.keyNumb != 0){
		tlkapi_timer_updateNode(&sTlkDevKBCtrl.timer, TLKDEV_KEYBORAD_TIMEOUT1, false);
	}else{
		tlkapi_timer_updateNode(&sTlkDevKBCtrl.timer, TLKDEV_KEYBORAD_TIMEOUT0, false);
	}
	return true;
}

#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))
static int tlkdev_keyboard_nodeInit(uint16 did)
{
	return TLK_ENONE;
}
static int tlkdev_keyboard_nodeOpen(uint16 did, uint32 param)
{
	int ret;
	if(sTlkDevKBCtrl.isOpen) return -TLK_EREPEAT;

	ret = tlkdrv_keyboard_open(sTlkDevKeyboardMap,
		sTlkDevKeyboardColPins, sTlkDevKeyboardRowPins,
		TLKDEV_KEYBOARD_COL_NUMBER, TLKDEV_KEYBOARD_ROW_NUMBER,
		TLKDEV_KEYBOARD_COL_WRITE_LEVEL, TLKDEV_KEYBOARD_ROW_VALID_LEVEL,
		TLKDEV_KEYBOARD_COL_PULLUP, TLKDEV_KEYBOARD_ROW_PULLUP);
	if(ret != TLK_ENONE) return ret;
	
	tlkapi_timer_insertNode(&sTlkDevKBCtrl.timer);
	
	sTlkDevKBCtrl.isOpen = true;
	
	return TLK_ENONE;
}
static int tlkdev_keyboard_nodeClose(uint16 did)
{
	if(!sTlkDevKBCtrl.isOpen) return TLK_ENONE;
	tlkapi_timer_removeNode(&sTlkDevKBCtrl.timer);
	sTlkDevKBCtrl.isOpen = false;
	tlkdrv_keyboard_close();
	tlkdev_keyboard_changedDeal(nullptr, 0);
	return TLK_ENONE;
}
static int tlkdev_keyboard_nodeState(uint16 did)
{
	if(!sTlkDevKBCtrl.isOpen) return TLKDEV_STATE_CLOSED;
	if(sTlkDevKBCtrl.keyNumb == 0) return TLKDEV_STATE_OPENED;
	return TLKDEV_STATE_BUSYING;
}
static int tlkdev_keyboard_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	if(buffLen == 0 || pBuff == nullptr) return -TLK_EPARAM;
	if(!sTlkDevKBCtrl.isOpen) return -TLK_ENOREADY;
	if(buffLen > sTlkDevKBCtrl.keyNumb) buffLen = sTlkDevKBCtrl.keyNumb;
	tmemcpy(pBuff, sTlkDevKBCtrl.keycode, buffLen);
	return buffLen;
}
static int tlkdev_keyboard_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_keyboard_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdev_keyboard_enterSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdev_keyboard_leaveSleep(param0);
		return TLK_ENONE;
	}
	return -TLK_ENOSUPPORT;
}
#endif //#if ((TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3))


static void tlkdev_keyboard_handler(void)
{
	if(!sTlkDevKBCtrl.isOpen) return;
	if(tlkdrv_keyboard_scanKey(true)){
		uint08 keyNumb = 0;
		uint08 buffLen = 8;
		uint08 buffer[8] = {0};
		tlkdrv_keyboard_getKey(buffer, buffLen, &keyNumb);
		tlkdev_keyboard_changedDeal(buffer, keyNumb);
	}
}
static void tlkdev_keyboard_changedDeal(uint08 *pNewKey, uint08 newNumb)
{
	bool isBack;
	uint08 index;
	uint08 rcdNumb;
	uint32 nowTime;
	uint32 timeDiff;
	uint08 rcdKeycode[TLKDEV_KEYBOARD_KEY_MAX_NUMB];
	uint32 rcdKeyTimer[TLKDEV_KEYBOARD_KEY_MAX_NUMB];
		
	isBack = false;
	rcdNumb = 0;
	for(index=0; index<sTlkDevKBCtrl.keyNumb || isBack; index++){
		if(isBack){
			isBack = false;
			if(index != 0) index--;
		}
		if(!tlkdev_keyboard_isInList(sTlkDevKBCtrl.keycode[index], pNewKey, newNumb)){
			rcdKeycode[rcdNumb] = sTlkDevKBCtrl.keycode[index];
			rcdKeyTimer[rcdNumb] = sTlkDevKBCtrl.keyTimer[index];
			rcdNumb ++;
			//Remove key from list
			if(index+1 != sTlkDevKBCtrl.keyNumb){
				uint08 differ = sTlkDevKBCtrl.keyNumb-index-1;
				tmemcpy(sTlkDevKBCtrl.keycode+index, sTlkDevKBCtrl.keycode+index+1, differ);
				tmemcpy(sTlkDevKBCtrl.keyTimer+index, sTlkDevKBCtrl.keyTimer+index+1, differ*4);
				isBack = true;
			}
			sTlkDevKBCtrl.keyNumb --;
		}
	}

	if((sTlkDevKBCtrl.evtMask & TLKDEV_KEYBOARD_EVTMSK_RELEASE) != 0 && rcdNumb != 0){
		uint08 buffer[3+TLKDEV_KEYBOARD_KEY_MAX_NUMB] = {TLKDEV_KEYBOARD_EVTID_RELEASE, false, rcdNumb};
		tmemcpy(buffer+3, rcdKeycode, rcdNumb);
		tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_DEVICE, TLKPTI_DEV_MSGID_KEYBOARD_EVENT_REPORT, buffer, 3+rcdNumb);
	}

	nowTime = clock_time() | 1;
	for(index=0; index<rcdNumb; index++){
		timeDiff = (nowTime-rcdKeyTimer[index]) / SYSTEM_TIMER_TICK_1US;
		if(timeDiff < TLKDEV_KEYBOARD_CLICK_TIME_MAX){
			if((sTlkDevKBCtrl.evtMask & TLKDEV_KEYBOARD_EVTMSK_CLICK) != 0){
				uint08 buffer[3+TLKDEV_KEYBOARD_KEY_MAX_NUMB] = {TLKDEV_KEYBOARD_EVTID_CLICK, false, rcdNumb, rcdKeycode[index]};
				tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_DEVICE, TLKPTI_DEV_MSGID_KEYBOARD_EVENT_REPORT, buffer, 3+1);
			}
		}else if(timeDiff < TLKDEV_KEYBOARD_SHORT_TIME_MAX){
			if((sTlkDevKBCtrl.evtMask & TLKDEV_KEYBOARD_EVTMSK_SHORT) != 0){
				uint08 buffer[3+TLKDEV_KEYBOARD_KEY_MAX_NUMB] = {TLKDEV_KEYBOARD_EVTID_SHORT, false, rcdNumb, rcdKeycode[index]};
				tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_DEVICE, TLKPTI_DEV_MSGID_KEYBOARD_EVENT_REPORT, buffer, 3+1);
			}
		}else{
			if((sTlkDevKBCtrl.evtMask & TLKDEV_KEYBOARD_EVTMSK_LONG) != 0){
				uint08 buffer[3+TLKDEV_KEYBOARD_KEY_MAX_NUMB] = {TLKDEV_KEYBOARD_EVTID_LONG, false, rcdNumb, rcdKeycode[index]};
				tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_DEVICE, TLKPTI_DEV_MSGID_KEYBOARD_EVENT_REPORT, buffer, 3+1);
			}
		}
	}

	rcdNumb = 0;
	for(index=0; index<newNumb; index++){
		if(sTlkDevKBCtrl.keyNumb >= TLKDEV_KEYBOARD_KEY_MAX_NUMB) break;
		if(!tlkdev_keyboard_isInList(pNewKey[index], sTlkDevKBCtrl.keycode, sTlkDevKBCtrl.keyNumb)){
			rcdKeycode[rcdNumb] = pNewKey[index];
			rcdKeyTimer[rcdNumb] = clock_time();
			//Insert key into list
			sTlkDevKBCtrl.keycode[sTlkDevKBCtrl.keyNumb] = rcdKeycode[rcdNumb];
			sTlkDevKBCtrl.keyTimer[sTlkDevKBCtrl.keyNumb] = rcdKeyTimer[rcdNumb];
			sTlkDevKBCtrl.keyNumb ++;
			rcdNumb ++;
		}
	}
	if((sTlkDevKBCtrl.evtMask & TLKDEV_KEYBOARD_EVTMSK_PRESS) != 0 && rcdNumb != 0){
		uint08 buffer[3+TLKDEV_KEYBOARD_KEY_MAX_NUMB] = {TLKDEV_KEYBOARD_EVTMSK_PRESS, true, rcdNumb};
		tmemcpy(buffer+3, rcdKeycode, rcdNumb);
		tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_DEVICE, TLKPTI_DEV_MSGID_KEYBOARD_EVENT_REPORT, buffer, 3+rcdNumb);
	}
}
static bool tlkdev_keyboard_isInList(uint08 keycode, uint08 *pKeyList, uint08 listNumb)
{
	uint08 index;
	if(listNumb == 0) return false;
	for(index=0; index<listNumb; index++){
		if(pKeyList[index] == keycode) break;
	}
	if(index != listNumb) return true;
	return false;
}

#endif //#if (TLKDEV_KEYBOARD_ENABLE)

