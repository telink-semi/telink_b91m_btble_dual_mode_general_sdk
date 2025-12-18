/********************************************************************************************************
 * @file	bth.h
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
#ifndef BTH_H
#define BTH_H


/******************************************************************************
 * Function: BTH Init interface
 * Descript: This interface be used to initial bth layer.
 * Params:
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 *         If others value is returned means the send process fail.
 *******************************************************************************/
int bth_init(void);
void bth_deinit(void);

int bth_needMemLen(void);

bool bth_isBusy(void);

bool bth_pmIsBusy(void);
uint bth_getAclCount(void);

int bth_sendEnterSleepCmd(void);
int bth_sendLeaveSleepCmd(void);


/******************************************************************************
 * Function: BTH Process interface
 * Descript: This interface be used to process the event and timer evt, seems
 *           like a schdluer of System.
 * Params: None.
 * Return: None.
 *******************************************************************************/
void bth_handler(void);

/******************************************************************************
 * Function: BTH Destroy interface
 * Descript: This interface be used to release the resource which allocate.
 * Params: @aclhandle[IN]--The acl link handle.
 * Return: None.
 *******************************************************************************/
void bth_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: bth_isHeadset
 * Descript: This interface be used to verify peer device is headset or
 *           others device.
 * Params: @aclhandle[IN]--The acl link handle.
 * Return: Return true means peer device type headset, false means others .
 *******************************************************************************/
bool bth_isHeadset(uint16 aclHandle);

/******************************************************************************
 * Function: bth_devClassToDevType
 * Descript: This interface be used to reflect from devClass to device type.
 * Params: @devClass[IN]--The device class.
 * Return: The Device type.
 *******************************************************************************/
uint08 bth_devClassToDevType(uint32 devClass);

/******************************************************************************
 * Function: bth_acl_setInitRole
 * Descript: This interface be used to set the role of acl link.
 * Params: @btaddr[IN]--The device bt address.
 *         @initRole[IN]--The role of acl link.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_acl_setInitRole(uint08 btaddr[6], uint08 initRole);

/******************************************************************************
 * Function: bth_acl_connect
 * Descript: This interface be used to start the acl link set up.
 * Params: @btaddr[IN]--The device bt address.
 *         @devClass[IN]--The device class.
 *         @initRole[IN]--The role of acl link.
 *         @timeout[IN]--The acl setup timeout value. Range:3000~3600000.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_acl_connect(uint08 btaddr[6], uint32 devClass, uint08 initRole, uint32 timeout);

/******************************************************************************
 * Function: bth_acl_disconn
 * Descript: This interface be used to tear up the acl link which specify
 *           by aclhandle.
 * Params: @aclHandle[IN]--The acl link handle.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_acl_disconn(uint16 aclHandle, uint08 reason);

/******************************************************************************
 * Function: bth_acl_destroy
 * Descript: This interface be used to release the acl link resource which
 *           specify by aclhandle.
 * Params: @aclHandle[IN]--The acl link handle.
 * Return: None.
 *******************************************************************************/
extern void bth_acl_destroy(uint16 aclHandle);

/******************************************************************************
 * Function: bth_acl_connectCancel
 * Descript: This interface be used to cancel the acl link connect which
 *           specify by bt address.
 * Params: @btaddr[IN]--The peer device address.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_acl_connectCancel(uint08 btaddr[6]);

/******************************************************************************
 * Function: bth_acl_disconnByAddr
 * Descript: This interface be used to disconnect the acl link which
 *           specify by bt address.
 * Params: @btaddr[IN]--The peer device address.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_acl_disconnByAddr(uint08 btaddr[6]);


/******************************************************************************
 * Function: bth_sco_connect
 * Descript: This interface be used to connect the SCO link which
 *           specify by aclHandle.
 * Params: @aclHandle[IN]--The acl link handle.
 *         @linkType[IN]--it is a scl or sco link type.
 *         @airMode[IN]--The air mode(cvsd or else).
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_sco_connect(uint16 aclHandle, uint16 linkType, uint08 airMode);

/******************************************************************************
 * Function: bth_sco_disconn
 * Descript: This interface be used to disconn the SCO link which
 *           specify by scoHandle.
 * Params: @scoHandle[IN]--The sco link handle.
 *         @reason[IN]--disconnect reason.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_sco_disconn(uint16 scoHandle, uint08 reason);

/******************************************************************************
 * Function: bth_sco_destroy
 * Descript: This interface be used to destroy the SCO link resource.
 * Params: @scoHandle[IN]--The sco link handle.
 * Return: None.
 *******************************************************************************/
extern void bth_sco_destroy(uint16 scoHandle);

/******************************************************************************
 * Function: bth_sco_destroyByAclHandle
 * Descript: This interface be used to destroy the SCO link resource.
 * Params: @scoHandle[IN]--The sco link handle.
 * Return: None.
 *******************************************************************************/
extern void bth_sco_destroyByAclHandle(uint16 aclHandle);

/******************************************************************************
 * Function: bth_sco_sendMute
 * Descript: This interface be used to set the phone mute.
 * Params: @scoHandle[IN]--The sco link handle.
 *         @micSpk[IN]--is mic or speaker.
 *         @enable[IN]--enable or disable.
 * Return: TLK_ENONE is set success, others means failure.
 *******************************************************************************/
extern int bth_sco_sendMute(uint16 scoHandle, uint08 micSpk, uint08 enable);


#endif // BTH_H
