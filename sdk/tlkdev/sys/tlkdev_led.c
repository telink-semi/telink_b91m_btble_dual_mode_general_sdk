/********************************************************************************************************
 * @file	tlkmdi_led.c
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
#if (TLK_DEV_LED_ENABLE)
#include "tlkdrv/ext/led/tlkdrv_led.h"
#include "tlkdev/tlkdev.h"
#include "tlkdev/tlkdev_list.h"
#include "tlkdev_led.h"
#include "drivers.h"


#define TLKDEV_LED_DBG_FLAG       TLKDEV_SYS_DBG_FLAG
#define TLKDEV_LED_DBG_SIGN       "[LED]"

static int tlkdev_led_nodeInit(uint16 did);
static int tlkdev_led_nodeOpen(uint16 did, uint32 param);
static int tlkdev_led_nodeClose(uint16 did);
static int tlkdev_led_nodeState(uint16 did);
static int tlkdev_led_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen);
static int tlkdev_led_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen);
static int tlkdev_led_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1);


#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1) || (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0) \
	|| (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3))
static const tlkdev_node_t scTlkDevLed1 = {
	TLKDEV_DID_LED1,  //.devID;
	"LED1",           //.pName;
	tlkdev_led_nodeInit,  //.Init
	tlkdev_led_nodeOpen,  //.Open
	tlkdev_led_nodeClose, //.Close
	tlkdev_led_nodeState, //.State
	tlkdev_led_nodeRead,  //.Read
	tlkdev_led_nodeWrite, //.Write
	tlkdev_led_nodeIoctl, //.Ioctl
};
static const tlkdev_node_t scTlkDevLed2 = {
	TLKDEV_DID_LED2,  //.devID;
	"LED2",           //.pName;
	tlkdev_led_nodeInit,  //.Init
	tlkdev_led_nodeOpen,  //.Open
	tlkdev_led_nodeClose, //.Close
	tlkdev_led_nodeState, //.State
	tlkdev_led_nodeRead,  //.Read
	tlkdev_led_nodeWrite, //.Write
	tlkdev_led_nodeIoctl, //.Ioctl
};
#if ((TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3) || (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3))
static const tlkdev_node_t scTlkDevLed3 = {
	TLKDEV_DID_LED3,  //.devID;
	"LED3",           //.pName;
	tlkdev_led_nodeInit,  //.Init
	tlkdev_led_nodeOpen,  //.Open
	tlkdev_led_nodeClose, //.Close
	tlkdev_led_nodeState, //.State
	tlkdev_led_nodeRead,  //.Read
	tlkdev_led_nodeWrite, //.Write
	tlkdev_led_nodeIoctl, //.Ioctl
};
static const tlkdev_node_t scTlkDevLed4 = {
	TLKDEV_DID_LED4,  //.devID;
	"LED4",           //.pName;
	tlkdev_led_nodeInit,  //.Init
	tlkdev_led_nodeOpen,  //.Open
	tlkdev_led_nodeClose, //.Close
	tlkdev_led_nodeState, //.State
	tlkdev_led_nodeRead,  //.Read
	tlkdev_led_nodeWrite, //.Write
	tlkdev_led_nodeIoctl, //.Ioctl
};
#endif
#endif


/******************************************************************************
 * Function: tlkdev_led_init
 * Descript: Initial I/O and PWM timers of the led
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkdev_led_init(void)
{
	tlkdrv_led_init();
#if (TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
	tlkdev_appendEctypeNode(&scTlkDevLed1);
	tlkdev_appendEctypeNode(&scTlkDevLed2);
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3)
	tlkdev_appendEctypeNode(&scTlkDevLed1);
	tlkdev_appendEctypeNode(&scTlkDevLed2);
	tlkdev_appendEctypeNode(&scTlkDevLed3);
	tlkdev_appendEctypeNode(&scTlkDevLed4);
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
	tlkdev_appendEctypeNode(&scTlkDevLed1);
	tlkdev_appendEctypeNode(&scTlkDevLed2);
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
	tlkdev_appendEctypeNode(&scTlkDevLed1);
	tlkdev_appendEctypeNode(&scTlkDevLed2);
	tlkdev_appendEctypeNode(&scTlkDevLed3);
	tlkdev_appendEctypeNode(&scTlkDevLed4);
#endif
	
	return TLK_ENONE;
}

bool tlkdev_led_isBusy(void)
{
	return tlkdrv_led_isBusy();
}
void tlkdev_led_enterSleep(uint mcuMode)
{
	tlkdrv_led_enterSleep(mcuMode);
}
void tlkdev_led_leaveSleep(uint wakeSrc)
{
	tlkdrv_led_leaveSleep(wakeSrc);
}

/******************************************************************************
 * Function: tlkdev_led_on
 * Descript: Turn on a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_led_on(uint08 ledID)
{
	return tlkdrv_led_on(ledID);
}

/******************************************************************************
 * Function: tlkdev_led_off
 * Descript: Turn off a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_led_off(uint08 ledID)
{
	return tlkdrv_led_off(ledID);
}

/******************************************************************************
 * Function: tlkdev_led_auto
 * Descript: Control led flash,the led flash frequency can be controlled by
 * 			 'onTimerMs' and 'offTimerMs', can also choose to keep the led on
 * 			 or off after the flashing is over.
 * Params: @ledID[IN]--The ledID.
 * 		   @firstOn[IN]--The state of the first work, true is on, false is off.
 * 		   @count[IN]--led flash count.
 * 		   @onTimerMs[IN]--on time.
 * 		   @offTimerMs[IN]--off time.
 * 		   @isKeepOn[IN]--The status after the flash is over, true is on, false is off.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_led_auto(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs, bool isKeepOn) 
{
	return tlkdrv_led_auto(ledID, firstOn, count, onTimerMs, offTimerMs, isKeepOn);
}


static int tlkdev_led_nodeInit(uint16 did)
{
	return TLK_ENONE;
}
static int tlkdev_led_nodeOpen(uint16 did, uint32 param)
{
#if (TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_insert(TLKDEV_DID_LED1, GPIO_PC3, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_insert(TLKDEV_DID_LED2, GPIO_PD4, GPIO_PIN_PULLUP_10K, 1);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_insert(TLKDEV_DID_LED1, GPIO_PD0, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_insert(TLKDEV_DID_LED2, GPIO_PD1, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED3){
		return tlkdrv_led_insert(TLKDEV_DID_LED3, GPIO_PE6, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED4){
		return tlkdrv_led_insert(TLKDEV_DID_LED4, GPIO_PE7, GPIO_PIN_PULLUP_10K, 1);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_insert(TLKDEV_DID_LED1, GPIO_PC3, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_insert(TLKDEV_DID_LED2, GPIO_PD4, GPIO_PIN_PULLUP_10K, 1);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_insert(TLKDEV_DID_LED1, GPIO_PB4, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_insert(TLKDEV_DID_LED2, GPIO_PB4, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED3){
		return tlkdrv_led_insert(TLKDEV_DID_LED3, GPIO_PB6, GPIO_PIN_PULLUP_10K, 1);
	}else if(did == TLKDEV_DID_LED4){
		return tlkdrv_led_insert(TLKDEV_DID_LED4, GPIO_PB7, GPIO_PIN_PULLUP_10K, 1);
	}
#endif

	return -TLK_ENOSUPPORT;
}
static int tlkdev_led_nodeClose(uint16 did)
{
#if (TLKHW_TYPE == TLKHW_TYPE_C1T213A83_V5_1)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_remove(TLKDEV_DID_LED1, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_remove(TLKDEV_DID_LED2, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T213A20_V1_3)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_remove(TLKDEV_DID_LED1, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_remove(TLKDEV_DID_LED2, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED3){
		return tlkdrv_led_remove(TLKDEV_DID_LED3, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED4){
		return tlkdrv_led_remove(TLKDEV_DID_LED4, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A83_V1_2) || (TLKHW_TYPE == TLKHW_TYPE_C1T294A83_V1_0)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_remove(TLKDEV_DID_LED1, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_remove(TLKDEV_DID_LED2, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}
#elif (TLKHW_TYPE == TLKHW_TYPE_C1T266A20_V1_3)
	if(did == TLKDEV_DID_LED1){
		return tlkdrv_led_remove(TLKDEV_DID_LED1, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED2){
		return tlkdrv_led_remove(TLKDEV_DID_LED2, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED3){
		return tlkdrv_led_remove(TLKDEV_DID_LED3, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}else if(did == TLKDEV_DID_LED4){
		return tlkdrv_led_remove(TLKDEV_DID_LED4, GPIO_PIN_UP_DOWN_FLOAT, 0);
	}
#endif

	return -TLK_ENOSUPPORT;
}
static int tlkdev_led_nodeState(uint16 did)
{
	tlkdrv_led_nor_unit_t *pUnit = tlkdrv_led_getUsedUnit(did);
	if(pUnit == nullptr) return TLKDEV_STATE_CLOSED;
	if(pUnit->isOpen || pUnit->isCtrl) return TLKDEV_STATE_BUSYING;
	return TLKDEV_STATE_OPENED;
}
static int tlkdev_led_nodeRead(uint16 did, uint32 param, uint08 *pBuff, uint32 buffLen)
{
	tlkdrv_led_nor_unit_t *pUnit;
	
	if(buffLen == 0) return -TLK_EOVERFLOW;
	
	pUnit = tlkdrv_led_getUsedUnit(did);
	if(pUnit == nullptr) return -TLK_ENOREADY;

	pBuff[0] = pUnit->isOpen;
	
	return 1;
}
static int tlkdev_led_nodeWrite(uint16 did, uint32 param, uint08 *pData, uint32 dataLen)
{
	if(dataLen == 0) return -TLK_EPARAM;
	if(pData[0] == 0x00){
		return tlkdrv_led_off(did);
	}else if(pData[0] == 0x01){
		return tlkdrv_led_on(did);
	}else if(pData[0] != 0x02){
		return -TLK_EPARAM;
	}else{
		tlkdev_led_write_t *pParam;
		if(dataLen < sizeof(tlkdev_led_write_t)) return -TLK_EPARAM;
		pParam = (tlkdev_led_write_t*)pData;
		if(!tlkdrv_led_auto(did, pParam->firstOn, pParam->exeCount, pParam->onTimerMs,
				pParam->offTimerMs, pParam->isKeepOn)){
			return -TLK_EFAIL;
		}
		return TLK_ENONE;
	}
}
static int tlkdev_led_nodeIoctl(uint16 did, uint16 opcode, uint32 param0, uint32 param1)
{
	if(opcode == TLKDEV_OPCODE_ENTER_SLEEP){
		tlkdrv_led_enterSleep(param0);
		return TLK_ENONE;
	}else if(opcode == TLKDEV_OPCODE_LEAVE_SLEEP){
		tlkdrv_led_leaveSleep(param0);
		return TLK_ENONE;
	}
	return -TLK_ENOSUPPORT;
}




#endif //#if (TLK_DEV_LED_ENABLE)

