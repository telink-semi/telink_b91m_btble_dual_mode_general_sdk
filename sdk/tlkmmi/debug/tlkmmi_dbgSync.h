/********************************************************************************************************
 * @file	tlkmmi_dbgSync.h
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
#ifndef TLKMMI_DBG_SYNC_H
#define TLKMMI_DBG_SYNC_H

#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)


#define TLKMMI_DEBUG_SYNC_TIMEOUT       10000

#define TLKMMI_DEBUG_SEND_BUFFLEN       100  //It should be less than "tlkmdi_comm_getBufferSize()".


typedef enum{
	TLKMMI_DEBUG_SYNC_FLAG_NONE = 0x00,
	TLKMMI_DEBUG_SYNC_FLAG_LOG  = 0x01,
	TLKMMI_DEBUG_SYNC_FLAG_VCD  = 0x02,
	TLKMMI_DEBUG_SYNC_FLAG_LIST = 0x10,
	TLKMMI_DEBUG_SYNC_FLAG_ITEM = 0x20,
	TLKMMI_DEBUG_SYNC_FLAG_SINGLE = 0x40,
}TLKMMI_DEBUG_SYNC_FLAG_ENUM;
typedef enum{
	TLKMMI_DEBUG_SYNC_BUSY_NONE = 0x00,
	TLKMMI_DEBUG_SYNC_BUSY_LIST = 0x01,
	TLKMMI_DEBUG_SYNC_BUSY_ITEM = 0x02,
}TLKMMI_DEBUG_SYNC_BUSY_ENUM;

typedef struct{
	uint08 syncBusys; //Refer TLKMMI_DEBUG_SYNC_BUSY_ENUM.
	uint08 syncFlags; //Refer TLKMMI_DEBUG_SYNC_FLAG_ENUM.
	uint16 listSerial;
	uint16 listNumber;
	uint16 itemSerial;
	uint16 itemNumber;
	uint16 syncListID;
	tlkapi_timer_t timer;
}tlkmmi_debug_sync_t;


int tlkmmi_debug_syncInit(void);

int tlkmmi_debug_syncList(bool isVCD, bool syncItem);
int tlkmmi_debug_syncItem(bool isVCD, uint16 listID);



#endif //#if (TLK_DBG_LOG_ENABLE || TLK_DBG_VCD_ENABLE)

#endif // TLKMMI_DBG_SYNC_H

