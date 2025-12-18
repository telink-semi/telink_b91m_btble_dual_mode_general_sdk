/********************************************************************************************************
 * @file	btp_iap.h
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
#ifndef BTP_IAP_H
#define BTP_IAP_H

#if (TLK_STK_BTP_ENABLE)


#define BTP_IAP_DBG_FLAG ((TLK_MAJOR_DBGID_BTP << 24) | (TLK_MINOR_DBGID_BTP_IAP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define BTP_IAP_DBG_SIGN nullptr


typedef void (*BtpIapRecvDataCB)(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);


typedef struct {
    uint08 state;
    uint08 rfcHandle;
    uint16 aclHandle;
    uint08 credit;
    uint08 resv001;
    uint16 mtuSize;
} btp_iap_item_t;
typedef struct {
    btp_iap_item_t *item;
} btp_iap_ctrl_t;


/******************************************************************************
 * Function: IAP initial interface
 * Descript: Defines trigger the initial flow of IAP source
 * Params:
 * 		@count[IN]--the IAP number, refer to TLK_BT_IAP_MAX_NUMB
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value is returned means the initial process fail.
 *******************************************************************************/
int btp_iap_init(uint08 count);

/******************************************************************************
 * Function: IAP deinitial interface
 * Descript: Defines trigger the initial flow of IAP source
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int btp_iap_deinit(void);

/******************************************************************************
 * Function: btp_iap_getMemLen
 * Descript: IAP get the size of the required number of nodes interface.
 * Params:
 * 		@count[IN]--the IAP number, refer to TLK_BT_IAP_MAX_NUMB
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
int btp_iap_getMemLen(uint08 count);

/******************************************************************************
 * Function: IAP Register Callback interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
void btp_iap_regDataCB(BtpIapRecvDataCB dataCB);


/******************************************************************************
 * Function: IAP send Connect interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
int btp_iap_connect(uint16 aclHandle, uint08 channel);

/******************************************************************************
 * Function: IAP send DisConnect interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
int btp_iap_disconn(uint16 aclHandle);

/******************************************************************************
 * Function: IAP send Destroy interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
void btp_iap_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: IAP send Data interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
int btp_iap_sendData(uint16 aclHandle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: IAP get Idle count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
uint08 btp_iap_getIdleCount(void);

/******************************************************************************
 * Function: IAP get Used count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
uint08 btp_iap_getUsedCount(void);

/******************************************************************************
 * Function: IAP get Connect count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
uint08 btp_iap_getConnCount(void);

/******************************************************************************
 * Function: IAP get Idle interface
 * Descript:
 * Params:
 * Return:The iap control block.
 *******************************************************************************/
btp_iap_item_t *btp_iap_getIdleItem(void);

/******************************************************************************
 * Function: IAP get used interface
 * Descript:
 * Params:
 * Return:The iap control block.
 *******************************************************************************/
btp_iap_item_t *btp_iap_getUsedItem(uint16 aclHandle);

/******************************************************************************
 * Function: IAP get Connected interface
 * Descript:
 * Params:
 * Return:The iap control block.
 *******************************************************************************/
btp_iap_item_t *btp_iap_getConnItem(uint16 aclHandle);


#endif // #if (TLK_STK_BTP_ENABLE)

#endif /* BTP_IAP_H */
