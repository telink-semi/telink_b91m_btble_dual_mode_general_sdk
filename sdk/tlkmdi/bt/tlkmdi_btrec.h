/********************************************************************************************************
 * @file	tlkmdi_btrec.h
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
#ifndef TLKMDI_BTREC_H
#define TLKMDI_BTREC_H

#if (TLK_MDI_BTREC_ENABLE)



/******************************************************************************
 * File: tlkmdi_btrec
 * Auth: telink
 * Func: This file manages the logic of callback, including active and passive
 *       callback modes. Active mode is when we initiate a page and then connect
 *       to each other; In passive mode, scan is enabled and connection requests
 *       are accepted.
*******************************************************************************/

#define TLKMDI_BTREC_TIMEOUT          100000 //100ms
#define TLKMDI_BTREC_TIMEOUT_MS       100

#define TLKMDI_BTREC_OPEN_WAIT        (200000/TLKMDI_BTREC_TIMEOUT) //If you start it for the first time, wait for a period of time before entering the flow

#define TLKMDI_BTREC_SCAN_STEP        (500000/TLKMDI_BTREC_TIMEOUT) //During each switchover to scan, the scan time is increased to improve the connection probability
#define TLKMDI_BTREC_SCAN_WAIT        (200000/TLKMDI_BTREC_TIMEOUT) //After the scan is complete, you need to wait for a period of time before switching to the page state.
#define TLKMDI_BTREC_PAGE_WAIT        (300000/TLKMDI_BTREC_TIMEOUT) //After the page is complete, you need to wait for a period of time before switching to the scan state.
#define TLKMDI_BTREC_INIT_WAIT        (300000/TLKMDI_BTREC_TIMEOUT) //After the page is complete, you need to wait for a period of time before switching to the scan state.
#define TLKMDI_BTREC_KEEP_WAIT        (300000/TLKMDI_BTREC_TIMEOUT)
#define TLKMDI_BTREC_KEEP_WAIT1       (500000/TLKMDI_BTREC_TIMEOUT)
#define TLKMDI_BTREC_STOP_WAIT        (200000/TLKMDI_BTREC_TIMEOUT)


#define TLKMDI_BTREC_PAGE_COUNT_DEF   3

#define TLKMDI_BTREC_KEEP_TIME_DEF    (60000/TLKMDI_BTREC_TIMEOUT_MS)

#define TLKMDI_BTREC_PAGE_TIME_DEF    (10000000/TLKMDI_BTREC_TIMEOUT) //
#define TLKMDI_BTREC_PAGE_TIME_MIN    (5000000/TLKMDI_BTREC_TIMEOUT)  //
#define TLKMDI_BTREC_PAGE_TIME_MAX    (60000000/TLKMDI_BTREC_TIMEOUT) //

#define TLKMDI_BTREC_SCAN_TIME_DEF    (3000000/TLKMDI_BTREC_TIMEOUT)  //
#define TLKMDI_BTREC_SCAN_TIME_MIN    (500000/TLKMDI_BTREC_TIMEOUT)   //
#define TLKMDI_BTREC_SCAN_TIME_MAX    (30000000/TLKMDI_BTREC_TIMEOUT) //

#define TLKMDI_BTREC_SCAN_STEP_DEF    (300000/TLKMDI_BTREC_TIMEOUT)  //
#define TLKMDI_BTREC_SCAN_STEP_MAX    (5000000/TLKMDI_BTREC_TIMEOUT) //


#define TLKMDI_BTREC_SCAN_COUNT       5


typedef enum{
	TLKMDI_BTREC_KEEP_MODE_NONE = 0,
	TLKMDI_BTREC_KEEP_MODE_INQUIRY_SCAN,
	TLKMDI_BTREC_KEEP_MODE_PAGE_SCAN,
	TLKMDI_BTREC_KEEP_MODE_BOTH_SCAN,
}TLKMDI_BTREC_KEEP_MODE_ENUM;
typedef enum{
	TLKMDI_BTREC_ACTIVE_MODE_NONE = 0x00,
	TLKMDI_BTREC_ACTIVE_MODE_PAGE = 0x01,
	TLKMDI_BTREC_ACTIVE_MODE_SCAN = 0x02,
	TLKMDI_BTREC_ACTIVE_MODE_BOTH = 0x03,
}TLKMDI_BTREC_ACTIVE_MODE_ENUM;

typedef enum{
	TLKMDI_BTREC_STATE_IDLE = 0,
	TLKMDI_BTREC_STATE_INIT,
	TLKMDI_BTREC_STATE_PAGE,
	TLKMDI_BTREC_STATE_SCAN,
	TLKMDI_BTREC_STATE_KEEP,
	TLKMDI_BTREC_STATE_STOP,
}TLKMDI_BTREC_STATE_ENUM;

typedef enum{
	TLKMDI_BTREC_STAGE_NONE = 0,
	//TLKMDI_BTREC_STATE_INIT
	TLKMDI_BTREC_STAGE_INIT_NONE = 0,
	TLKMDI_BTREC_STAGE_INIT_CLOSE_SCAN,
	TLKMDI_BTREC_STAGE_INIT_SET_SCHEDULE,
	TLKMDI_BTREC_STAGE_INIT_WAIT,
	//TLKMDI_BTREC_STATE_PAGE
	TLKMDI_BTREC_STAGE_PAGE_NONE = 0,
	TLKMDI_BTREC_STAGE_PAGE_START,
	TLKMDI_BTREC_STAGE_PAGE_WAIT0,
	TLKMDI_BTREC_STAGE_PAGE_CLOSE,
	TLKMDI_BTREC_STAGE_PAGE_WAIT1,
	//TLKMDI_BTREC_STATE_SCAN
	TLKMDI_BTREC_STAGE_SCAN_NONE = 0,
	TLKMDI_BTREC_STAGE_SCAN_START,
	TLKMDI_BTREC_STAGE_SCAN_WAIT0,
	TLKMDI_BTREC_STAGE_SCAN_CLOSE,
	TLKMDI_BTREC_STAGE_SCAN_WAIT1,
	//TLKMDI_BTREC_STATE_KEEP
	TLKMDI_BTREC_STAGE_KEEP_NONE= 0,
	TLKMDI_BTREC_STAGE_KEEP_CLOSE_PAGE,
	TLKMDI_BTREC_STAGE_KEEP_CLOSE_SCAN,
	TLKMDI_BTREC_STAGE_KEEP_WAIT,
	TLKMDI_BTREC_STAGE_KEEP_START,
	TLKMDI_BTREC_STAGE_KEEP_RUN,
	//TLKMDI_BTREC_STATE_STOP
	TLKMDI_BTREC_STAGE_STOP_NONE = 0,
	TLKMDI_BTREC_STAGE_STOP_PAGE,
	TLKMDI_BTREC_STAGE_STOP_SCAN,
	TLKMDI_BTREC_STAGE_STOP_WAIT,
}TLKMDI_BTREC_STAGE_ENUM;

typedef void(*TlkMdiBtRecOverCallback)(void);

typedef struct{
	uint08 state;
	uint08 stage;
	uint16 timeout;
	uint08 actMode;
	uint08 tempCount;
	uint08 pageCount;
	uint08 scanCount;
	uint08 scanMode;
	uint08 keepMode;
	uint16 keepTime;
	uint16 ptimeout; //Page Timeout For Controller
	uint16 pageTime;
	uint16 pageWait;
	uint16 scanTime;
	uint16 scanStep;
	uint16 scanWait;
	uint32 devClass;
	uint08 pageAddr[6]; //The device to be connected back.
	tlkapi_timer_t timer;
}tlkmdi_btrec_t;


/******************************************************************************
 * Function: tlkmdi_btrec_init
 * Descript: Trigger the reconnect initial.
 * Params: None.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_init(void);

/******************************************************************************
 * Function: tlkmdi_btrec_reset
 * Descript: Trigger reset the reconnect Control Block.
 * Params: None.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_reset(void);

/******************************************************************************
 * Function: tlkmdi_btrec_start
 * Descript: Trigger start to reconnect.
 * Params:
 *        @pageAddr[IN]--The peer address.
 *        @devClass[IN]--The device type.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_start(uint08 *pPageAddr, uint32 devClass, bool enPage, bool enScan);

/******************************************************************************
 * Function: tlkmdi_btrec_close
 * Descript: Trigger stop reconnect.
 * Params: None.
 * Return: The TLK_ENONE is success or thers is faiure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_close(void); //keepTime: unit-s;

/******************************************************************************
 * Function: tlkmdi_btrec_regCB
 * Descript: Register the reconnect result callback.
 * Params: None.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmdi_btrec_regCB(TlkMdiBtRecOverCallback overCB);

/******************************************************************************
 * Function: tlkmdi_btrec_isInBusy
 * Descript: Check the reconnect status.
 * Params: None.
 * Return: true /false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_btrec_isInBusy(void);
bool tlkmdi_btrec_isInPage(void);
bool tlkmdi_btrec_isInScan(void);
bool tlkmdi_btrec_isInKeep(void);
bool tlkmdi_btrec_isInStop(void);

/******************************************************************************
 * Function: tlkmdi_btrec_getPageAddr
 * Descript: Get the page device address under reconnecting.
 * Params: None.
 * Return: BT address.
 * Others: None.
*******************************************************************************/
uint08 *tlkmdi_btrec_getPageAddr(void);

/******************************************************************************
 * Function: Set the scan parameter.
 * Descript: Set the reconnect parameter.
 * Params: None.
 * Return: TLK_ENONE is success,other value is failure.
 * Others: None.
*******************************************************************************/
int tlkmdi_btrec_setKeepMode(TLKMDI_BTREC_KEEP_MODE_ENUM keepMode, uint16 keepTime); //keepTime: unit-s;
int tlkmdi_btrec_setPageParam(uint16 pageTime, uint08 pageCount); //pageTime: unit-ms;
int tlkmdi_btrec_setScanParam(uint16 scanTime, uint16 scanStep, bool enInqScan, bool enPageScan); //scanTime-scanStep: unit-ms;

int tlkmdi_btrec_connectCancel(uint08 btAddr[6]);



#endif //#if (TLK_MDI_BTREC_ENABLE)

#endif //TLKMDI_BTREC_H

