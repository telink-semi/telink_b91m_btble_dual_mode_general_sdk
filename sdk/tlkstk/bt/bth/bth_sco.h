/********************************************************************************************************
 * @file	bth_sco.h
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
#ifndef BTH_SCO_H
#define BTH_SCO_H


#define BTH_SCO_TIMEOUT      (100000)
#define BTH_SCO_TIMEOUT_MS   (100)
#define BTH_SCO_CONN_TIMEOUT (5000000 / BTH_SCO_TIMEOUT) // <256
#define BTH_SCO_DISC_TIMEOUT (5000000 / BTH_SCO_TIMEOUT) // <256


typedef enum {
    BTP_SCO_BUSY_NONE              = 0x00,
    BTP_SCO_BUSY_SEND_CONN_REQUEST = 0x01,
    BTH_SCO_BUSY_SEND_CONN_ACCEPT  = 0x02,
    BTH_SCO_BUSY_SEND_DISC_REQUEST = 0x04,
} BTH_SCO_BUSYS_ENUM;

typedef enum {
    BTH_SCO_LINK_TYPE_SCO  = 0x00,
    BTH_SCO_LINK_TYPE_ESCO = 0x02,
} BTH_SCO_LINK_TYPE_ENUM;

typedef enum {
    BTH_SCO_AIRMODE_CVSD  = 0x00,
    BTH_SCO_AIRMODE_U_LAW = 0x01,
    BTH_SCO_AIRMODE_A_LAW = 0x02,
    BTH_SCO_AIRMODE_TRANS = 0x03,
} BTH_SCO_AIRMODE_ENUM;


/******************************************************************************
 * Function: bth_sco_isConn
 * Descript: Verify that SCO is in the connected state.
 * Params:
 *        @pCodec[IN]--The codec id.
 * Return: True if SCO is connection.
 *******************************************************************************/
bool bth_sco_isConn(uint16 aclHandle);

/******************************************************************************
 * Function: bth_sco_setConnTimeout
 * Descript: Set the time from establishment of the SCO to connection timeout.
 * Params:
 *     @timeout[IN]--The time of connection timeout. Unit-ms, Range[3000~15000].
 * Return: None.
 *******************************************************************************/
void bth_sco_setConnTimeout(uint16 timeout);

/******************************************************************************
 * Function: bth_sco_connect
 * Descript: This interface be used to connect the SCO link which
 *           specify by aclHandle.
 * Params:
 *     @aclHandle[IN]--The acl link handle.
 *     @linkType[IN]--it is a sco or esco link type. Refer to
 *        BTH_SCO_LINK_TYPE_ENUM.
 *     @airMode[IN]--The air mode(cvsd or else). Rerfer to
 *        BTH_SCO_AIRMODE_ENUM
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_sco_connect(uint16 aclHandle, uint16 linkType, uint08 airMode);

/******************************************************************************
 * Function: bth_sco_disconn
 * Descript: This interface be used to disconn the SCO link which
 *           specify by scoHandle.
 * Params: @scoHandle[IN]--The sco link handle.
 *         @reason[IN]--disconnect reason.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_sco_disconn(uint16 scoHandle, uint08 reason);

/******************************************************************************
 * Function: bth_sco_disconn
 * Descript: This interface be used to disconn the SCO link which
 *           specify by scoHandle.
 * Params: @scoHandle[IN]--The sco link handle.
 *         @reason[IN]--disconnect reason.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_sco_disconnByAddr(uint08 btaddr[6], uint08 reason);

/******************************************************************************
 * Function: bth_sco_destroy
 * Descript: This interface be used to destroy the SCO link resource.
 * Params: @scoHandle[IN]--The sco link handle.
 * Return: None.
 *******************************************************************************/
void bth_sco_destroy(uint16 scoHandle);

/******************************************************************************
 * Function: bth_sco_destroyByAclHandle
 * Descript: This interface be used to destroy the SCO link resource.
 * Params: @scoHandle[IN]--The sco link handle.
 * Return: None.
 *******************************************************************************/
void bth_sco_destroyByAclHandle(uint16 aclHandle);

/******************************************************************************
 * Function: bth_sco_sendMute
 * Descript: This interface be used to set the phone mute.
 * Params: @scoHandle[IN]--The sco link handle.
 *         @micSpk[IN]--is mic or speaker.
 *         @enable[IN]--enable or disable.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
int bth_sco_sendMute(uint16 scoHandle, uint08 micSpk, uint08 enable);

/******************************************************************************
 * Function: bth_sco_connCancelEvt
 * Descript: handle sco connect cancel event.
 * Params:
 *        @status[IN]--The status.
 *        @btaddr[IN]--The bt address.
 * Return: None.
 *******************************************************************************/
void bth_sco_connCancelEvt(uint08 status, uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_sco_connectRequestEvt
 * Descript: handle sco connect request event.
 * Params:
 *        @btaddr[IN]--The Bt address.
 *        @devClass[IN]--The device class type.
 * Return: None.
 *******************************************************************************/
void bth_sco_connectRequestEvt(uint08 btaddr[6], uint32 devClass);

/******************************************************************************
 * Function: bth_sco_connectCompleteEvt
 * Descript: handle sco connect complete event.
 * Params:
 *        @status[IN]--The status.
 *        @handle[IN]--The acl handle.
 *        @btaddr[IN]--The Bt address.
 *        @isEncrypt[IN]--is enable encrypt.
 * Return: None.
 *******************************************************************************/
void bth_sco_connectCompleteEvt(uint08 status, uint16 handle, uint08 btaddr[6], uint08 isEncrypt);

/******************************************************************************
 * Function: bth_sco_disconnCompleteEvt
 * Descript: handle sco disconnect complete event.
 * Params:
 *        @handle[IN]--The acl handle.
 *        @reason[IN]--The disconnect reason.
 * Return: None.
 *******************************************************************************/
void bth_sco_disconnCompleteEvt(uint16 handle, uint08 reason);


/******************************************************************************
 * Function: bth_sco_requestEvt
 * Descript: handle sco request event.
 * Params:
 *        @btaddr[IN]--The Bt address.
 *        @devClass[IN]--The device class type.
 *        @linkType[IN]--The link type.
 * Return: None.
 *******************************************************************************/
void bth_sco_requestEvt(uint08 btaddr[6], uint32 devClass, uint08 linkType);

/******************************************************************************
 * Function: bth_sco_connectEvt
 * Descript: handle sco Connect event.
 * Params:
 *        @status[IN]--The Sco status.
 *        @handle[IN]--The sco handle.
 *        @btaddr[IN]--The bt address.
 *        @linktype[IN]--The link type.
 *        @airMode[IN]--The air mode.
 * Return: None.
 *******************************************************************************/
void bth_sco_connectEvt(uint08 status, uint16 handle, uint08 btaddr[6], uint08 linkType, uint08 airMode);

/******************************************************************************
 * Function: bth_sco_requestEvt
 * Descript: handle sco request event.
 * Params:
 *        @handle[IN]--The sco handle.
 *        @reason[IN]--The disconnect reason.
 * Return: None.
 *******************************************************************************/
void bth_sco_disconnEvt(uint16 handle, uint08 reason);


int bth_sco_getHandle(uint16 aclHandle, uint16 offset, uint08 *pScoHandle);
int bth_sco_getAirModeByAclHandle(uint16 aclHandle, uint08 *pAirMode);
int bth_sco_setAirModeByAclHandle(uint16 aclHandle, uint08 airMode);
int bth_sco_setTxRxBandwidth(uint32_t txBandwidth, uint32_t rxBandwidth);


#endif // BTH_SCO_H
