/********************************************************************************************************
 * @file	btp_spp.h
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
#ifndef BTP_SPP_H
#define BTP_SPP_H

#if (TLK_STK_BTP_ENABLE)


typedef void (*BtpSppRecvDataCB)(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen);


typedef struct {
    uint08 state;
    uint08 rfcHandle;
    uint16 aclHandle;
    uint08 credit;
    uint08 resv001;
    uint16 mtuSize;
} btp_spp_item_t;
typedef struct {
    btp_spp_item_t *item;
} btp_spp_ctrl_t;


/******************************************************************************
 * Function: SPP Init interface
 * Descript: This interface be used by User to initial spp resource.
 * Params:
 * 		@count[IN]: refer to TLK_BT_SPP_MAX_NUMB
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value is returned means the initial process fail.
 *******************************************************************************/
int btp_spp_init(uint08 count);

/******************************************************************************
 * Function: SPP deinit interface
 * Descript: This interface be used by User to deinitial spp resource.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
int btp_spp_deinit(void);

/******************************************************************************
 * Function: btp_spp_getMemLen
 * Descript: SPP get the size of the required number of nodes interface.
 * Params:
 * 		@count[IN]: refer to TLK_BT_SPP_MAX_NUMB
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
int btp_spp_getMemLen(uint08 count);

/******************************************************************************
 * Function: SPP Register Callback interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
void btp_spp_regDataCB(BtpSppRecvDataCB dataCB);

/******************************************************************************
 * Function: SPP send Connect interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
int btp_spp_connect(uint16 aclHandle, uint08 channel);


/******************************************************************************
 * Function: SPP send DisConnect interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
int btp_spp_disconn(uint16 aclHandle);

/******************************************************************************
 * Function: SPP send Destroy interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
void btp_spp_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: btp_spp_sendRemoteLineStatus
 * Descript: This interface be used by profile or user to send RLS CMD.
 * Params:
 *     @aclHandle[IN]---The connection handle.
 *     @status[IN]--The L1-L4 bits indicates the Line Status.
 *         If L1 is set to 0, no error have occurred.
 *           L1 = 1 indicates the following errors:
 *         L2-L4:
 *           100 Overrun Error - Received character overwrote an unread character
 *           010 Parity Error - Received character's parity was incorrect
 *           001 Framing Error - a character did not terminate with a stop bit.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int btp_spp_sendRemoteLineStatus(uint16 aclHandle, uint08 status);

/******************************************************************************
 * Function: SPP send Data interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
int btp_spp_sendData(uint16 aclHandle, uint08 *pHead, uint08 headLen, uint08 *pData, uint16 dataLen);

/******************************************************************************
 * Function: SPP get Idle count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
uint08 btp_spp_getIdleCount(void);

/******************************************************************************
 * Function: SPP get Used count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
uint08 btp_spp_getUsedCount(void);

/******************************************************************************
 * Function: SPP get Used count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
btp_spp_item_t *btp_spp_getItem(uint08 index);

/******************************************************************************
 * Function: SPP get Connect count interface
 * Descript:
 * Params:
 * Return:
 *******************************************************************************/
uint08 btp_spp_getConnCount(void);

/******************************************************************************
 * Function: SPP get Idle interface
 * Descript:
 * Params:
 * Return:The spp idle control block.
 *******************************************************************************/
btp_spp_item_t *btp_spp_getIdleItem(void);

/******************************************************************************
 * Function: SPP get used interface
 * Descript:
 * Params:
 * Return:The spp used control block.
 *******************************************************************************/
btp_spp_item_t *btp_spp_getUsedItem(uint16 aclHandle);

/******************************************************************************
 * Function: SPP get Connect interface
 * Descript:
 * Params:
 * Return:The spp connect control block.
 *******************************************************************************/
btp_spp_item_t *btp_spp_getConnItem(uint16 aclHandle);


#endif // #if (TLK_STK_BTP_ENABLE)

#endif /* BTP_SPP_H */
