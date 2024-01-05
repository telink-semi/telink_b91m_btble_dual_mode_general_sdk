/********************************************************************************************************
 * @file	tlkdev_battery.c
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
#if (TLK_DEV_BATTERY_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkdrv/ext/battery/tlkdrv_battery.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev_battery.h"
#include "drivers.h"


#define TLKDEV_BAT_DBG_FLAG         TLKDEV_SYS_DBG_FLAG
#define TLKDEV_BAT_DBG_SIGN         "[BAT]"


static int tlkdev_bat_nodeInit(uint16 did);
static int tlkdev_bat_nodeOpen(uint16 did, uint32 param);
static int tlkdev_bat_nodeClose(uint16 did);
static int tlkdev_bat_nodeState(uint16 did);
static int tlkdev_bat_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_bat_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_bat_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);
static void tlkdev_bat_checkCB(uint08 level, uint16 voltage);


static const tlkdev_node_t scTlkDevBattery1 = {
	TLKDEV_DID_BATTERY,  //.devID;
	"BATTERY",           //.pName;
	tlkdev_bat_nodeInit,  //.Init
	tlkdev_bat_nodeOpen,  //.Open
	tlkdev_bat_nodeClose, //.Close
	tlkdev_bat_nodeState, //.State
	tlkdev_bat_nodeRead,  //.Read
	tlkdev_bat_nodeWrite, //.Write
	tlkdev_bat_nodeIoctl, //.Ioctl
};


/******************************************************************************
 * Function: tlkapp_battery_init
 * Descript: Initial the battery's gpio and register the callback.
 * Params: None.
 * Return: TLK_NONE is success.
 * Others: None.
*******************************************************************************/
int tlkdev_battery_init(void)
{
	tlkdrv_battery_init();
	tlkdev_appendEctypeNode(&scTlkDevBattery1);
	tlkdrv_battery_regCB(tlkdev_bat_checkCB);
	
	return TLK_ENONE;
}


void tlkdev_battery_enterSleep(uint mcuMode)
{
	tlkdrv_battery_enterSleep(mcuMode);
}
void tlkdev_battery_leaveSleep(uint wakeSrc)
{
	tlkdrv_battery_leaveSleep(wakeSrc);
}



static int tlkdev_bat_nodeInit(uint16 did)
{
	return TLK_ENONE;
}
static int tlkdev_bat_nodeOpen(uint16 did, uint32 param)
{
	return tlkdrv_battery_open();
}
static int tlkdev_bat_nodeClose(uint16 did)
{
	return tlkdrv_battery_close();
}
static int tlkdev_bat_nodeState(uint16 did)
{
	if(tlkdrv_battery_isOpen()) return TLKDEV_STATE_OPENED;
	else return TLKDEV_STATE_CLOSED;
}
static int tlkdev_bat_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	int level;
	int voltage;

	if(buffLen < 3) return -TLK_EOVERFLOW;
	
	level = tlkdrv_battery_level();
	if(level < 0) return -TLK_EFAIL;

	voltage = tlkdrv_battery_voltage();

	pBuff[0] = level;
	pBuff[1] = voltage & 0xFF;
	pBuff[2] = (voltage & 0xFF00) >> 8;
	
	return 3;
}
static int tlkdev_bat_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	return -TLK_ENOSUPPORT;
}
static int tlkdev_bat_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdrv_battery_enterSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdrv_battery_leaveSleep(param0);
		return TLK_ENONE;
	}
	return -TLK_ENOSUPPORT;
}

static void tlkdev_bat_checkCB(uint08 level, uint16 voltage)
{
	uint08 battery[4];
		
	battery[0] = level;
	battery[1] = (voltage & 0x00FF);
	battery[2] = (voltage & 0xFF00) >> 8;
	tlksys_sendInnerMsgFromIrq(TLKSYS_TASKID_SYSTEM, TLKPTI_SYS_MSGID_BATTERY_REPORT, battery, 3);
}



#endif //TLK_DEV_BATTERY_ENABLE

