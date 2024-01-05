/********************************************************************************************************
 * @file	tlkmdi_btinq.h
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
#ifndef TLKMDI_BTINQ_H
#define TLKMDI_BTINQ_H

#if (TLK_MDI_BTINQ_ENABLE)


#define TLKMDI_BTINQ_TIMEOUT        100000 //us
#define TLKMDI_BTINQ_TIMEOUT_MS     (TLKMDI_BTINQ_TIMEOUT/1000)

#define TLKMDI_BTINQ_ITEM_NUMB      15
#define TLKMDI_BTINQ_NAME_LENS      19

#define TLKMDI_BTINQ_WAIT_GETNAME_TIMEOUT         (3000000/TLKMDI_BTINQ_TIMEOUT)
#define TLKMDI_BTINQ_WAIT_CANCEL_TIMEOUT          (3000000/TLKMDI_BTINQ_TIMEOUT)
#define TLKMDI_BTINQ_WAIT_SWITCH_TIMEOUT          (800000/TLKMDI_BTINQ_TIMEOUT)




typedef enum{
	TLKMDI_BTINQ_DTYPE_MISC     = 0x00, //
	TLKMDI_BTINQ_DTYPE_PC       = 0x01, //PC
	TLKMDI_BTINQ_DTYPE_PHONE    = 0x02, //Phone
	//TLKMDI_BTINQ_DTYPE_NET      = 0x03, //network_dev
	TLKMDI_BTINQ_DTYPE_HEADSET  = 0x04, //headset
	//TLKMDI_BTINQ_DTYPE_KEYBOARD = 0x05, //keyboard
	//TLKMDI_BTINQ_DTYPE_SPP      = 0x0F, //spp_dev
	TLKMDI_BTINQ_DTYPE_UNKNOWN  = 0xFF, 
}TLKMDI_BTINQ_DTYPE_ENUM;

typedef enum{	
	TLKMDI_BTINQ_STATE_IDLE = 0,
	TLKMDI_BTINQ_STATE_INQUIRY,
	TLKMDI_BTINQ_STATE_GETNAME,
	TLKMDI_BTINQ_STATE_CLOSING, 
}TLKMDI_BTINQ_STATE_ENUM;
typedef enum{
	TLKMDI_BTINQ_STAGE_NONE = 0,
	
	//TLKMDI_BTINQ_STATE_INQUIRY
	TLKMDI_BTINQ_INQUIRY_STAGE_CANCEL = 1, //Cancel Get Name
	TLKMDI_BTINQ_INQUIRY_STAGE_WAIT0,
	TLKMDI_BTINQ_INQUIRY_STAGE_START,
	TLKMDI_BTINQ_INQUIRY_STAGE_DOING,
	TLKMDI_BTINQ_INQUIRY_STAGE_CLOSE,
	TLKMDI_BTINQ_INQUIRY_STAGE_WAIT1,

	//TLKMDI_BTINQ_STATE_GETNAME
	TLKMDI_BTINQ_GETNAME_STAGE_START = 1, //Cancel Inquiry
	TLKMDI_BTINQ_GETNAME_STAGE_DOING,
	TLKMDI_BTINQ_GETNAME_STAGE_WAIT0,
	TLKMDI_BTINQ_GETNAME_STAGE_CLOSE,
	TLKMDI_BTINQ_GETNAME_STAGE_WAIT1,
		
	//TLKMDI_BTINQ_STATE_CLOSING
	TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_GETNAME = 1,
	TLKMDI_BTINQ_CLOSING_STAGE_WAIT_GETNAME,
	TLKMDI_BTINQ_CLOSING_STAGE_CANCEL_INQUIRY,
	TLKMDI_BTINQ_CLOSING_STAGE_WAIT_INQUIRY,
	TLKMDI_BTINQ_CLOSING_STAGE_INQUIRY_OVER,
	
}TLKMDI_BTINQ_STAGE_ENUM;

typedef enum{
	TLKMDI_BTINQ_ITEM_STATE_NONE = 0,
	TLKMDI_BTINQ_ITEM_STATE_WAIT,
	TLKMDI_BTINQ_ITEM_STATE_OVER,
}TLKMDI_BTINQ_ITEM_STATE_ENUM;


typedef struct{
	uint08 rssi;
	uint08 state; //TLKMDI_BTINQ_ITEM_STATE_ENUM
	uint08 smode; //Scan Mode
	uint08 dtype;
	uint08 nameLen; //Name Length
	uint16 clkOff;
	uint32 devClass;
	uint08 btaddr[6];
	uint08 btname[TLKMDI_BTINQ_NAME_LENS+1];
}tlkmdi_btinq_item_t;
typedef struct{
	uint08 state;
	uint08 busys;
	uint08 stage;
	uint08 inqType; // pc: 1, phone:2, network_dev :3,	headset 4, keyboard:5,	spp_dev : 0x0f;
	uint08 curNumb;
	uint08 maxNumb;
	uint08 nameIdx;
	uint08 rssiThd;
	uint08 saveDev;
	uint08 getName;
	uint08 inqNumb;
	uint16 inqWind;
	uint16 timeout;
	
	tlkapi_timer_t timer;
	tlkmdi_btinq_item_t item[TLKMDI_BTINQ_ITEM_NUMB];
}tlkmdi_btinq_ctrl_t;


typedef int(*TlkMmiBtInqReportCallBack)(uint32 devClass, uint08 rssi, uint08 nameLen, uint08 *pBtaddr, uint08 *pBtName);
typedef void(*TlkMmiBtInqCompleteCallBack)(void);


/******************************************************************************
 * Function: tlkmdi_btinq_init
 * Descript: Start to Initial the Inquiry.
 * Params: None.
 * Return: Return TLK_ENONE is success, other value is failure.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btinq_init(void);

/******************************************************************************
 * Function: tlkmdi_btinq_isBusy
 * Descript: Check exist a Inquiry.
 * Params: None.
 * Return: Return true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btinq_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_btinq_start
 * Descript: Begins to retrieve the surrounding BT devices.
 * Params:
 *     @inqType[IN]--The inquiry type.
 *     @rssiThd[IN]--The rssi of signal.
 *     @maxNumb[IN]--The report devices number.
 *     @inqWind[IN]--Window for a single query. (unit:s, inqWind <= 30s )
 *     @inqNumb[IN]--The number of Query Windows. 
 *     @isGetName[IN]--whether get Bt device name or not.
 *     @isSave[IN]--Whether to save the queried BT device to the query list.
 * Return: Return TLK_ENONE is success,other value is false.
 * Others: 
 *     If isSave=FALSE, then GetName also needs to be FALSE. This means that 
 *     the system does not get names for devices that do not need to be saved.
 *     Applicable to connecting a device of an unknown device type.
*******************************************************************************/
int  tlkmdi_btinq_start(uint08 inqType, uint08 rssiThd, uint08 maxNumb, 
	 uint08 inqWind, uint08 inqNumb, bool isGetName, bool isSave); //timeout: unit-s

/******************************************************************************
 * Function: tlkmdi_btinq_close
 * Descript: stop a Inquiry.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_close(void);

/******************************************************************************
 * Function: tlkmdi_btinq_reset
 * Descript: reset the Inquiry control block.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_reset(void);


/******************************************************************************
 * Function: tlkmdi_btinq_regCallback
 * Descript: Register the callback.
 * Params: 
 *     @reportCB[IN]--The report callback function.
 *     @completeCB[IN]--Inquiry complete callback function.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_regCallback(TlkMmiBtInqReportCallBack reportCB, 
     TlkMmiBtInqCompleteCallBack completeCB);

/******************************************************************************
 * Function: tlkmdi_btinq_cleanItems
 * Descript: Clear the inquiry number.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btinq_cleanItems(void);

/******************************************************************************
 * Function: tlkmdi_btinq_getItemCount
 * Descript: Get the idle inquiry count.
 * Params: None.
 * Return: The number of idle count.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btinq_getItemCount(void);

/******************************************************************************
 * Function: tlkmdi_btinq_getItemIndex
 * Descript: Get the inquiry item index.
 * Params: 
 *     @pAddr[IN]--The bt address.
 * Return: The index of Item.
 * Others: None.
*******************************************************************************/
int  tlkmdi_btinq_getItemIndex(uint08 *pAddr);

/******************************************************************************
 * Function: tlkmdi_btinq_getItem
 * Descript: Get the inquiry item by index.
 * Params: 
 *     @index[IN]--The index of inquiry control block.
 * Return: The Inquiry control block.
 * Others: None.
*******************************************************************************/
tlkmdi_btinq_item_t *tlkmdi_btinq_getItem(uint08 index);

/******************************************************************************
 * Function: tlkmdi_btinq_getIdleItem
 * Descript: Get the Idle inquiry item.
 * Params: None.
 * Return: The Idle Inquiry control block.
 * Others: None.
*******************************************************************************/
tlkmdi_btinq_item_t *tlkmdi_btinq_getIdleItem(void);

/******************************************************************************
 * Function: tlkmdi_btinq_getUsedItem
 * Descript: Get the Used inquiry item.
 * Params: None.
 * Return: The Used Inquiry control block.
 * Others: None.
*******************************************************************************/
tlkmdi_btinq_item_t *tlkmdi_btinq_getUsedItem(uint08 *pAddr);

void tlkmdi_btinq_printList(void);


#endif //#if (TLK_MDI_BTINQ_ENABLE)

#endif //TLKMDI_BTINQ_H

