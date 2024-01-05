/********************************************************************************************************
 * @file	acl_slave.h
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
#ifndef ACL_SLAVE_H_
#define ACL_SLAVE_H_





/**
 * @brief      for user to initialize ACL connection slave role.
 * @param	   none
 * @return     none
 */
void 		blc_ll_initAclSlaveRole_module(void);



/**
 * @brief      for user to set the number to ignore interval.
 * @param	   the value of ignoring.
 * @return     0:success; -1:fail.
 */
int 		blc_ll_setIgnoreIntvlCnt(unsigned char ignoreCnt);

/**
 * @brief      for used to configure how many consecutive intervals the task will be abandoned
 * @param	   the number of consecutive intervals.
 * @return     0:success; -1:fail.
 */
int			blc_ll_setConsecIntvlCnt(unsigned char consecCnt);


/**
 * @brief      for slave, received peer packet but the packet transmitting to peer master is stopped.
 *             for master,transmitted packet to peer slave, but not receive the peer slave packet.
 *             I name these situation notAck. For other tasks to run, BLE can accept a certain number of such situations,
 *             but the number can not be too much and need to make sure that BLE connection cannot be affected
 * @param	   the value of ignoreNotAckNum.
 * @return     0:success; -1:fail.
 */
int			blc_ll_setIgnore_notAckNum(unsigned char ignoreNotAckNum);


#endif /* ACL_SLAVE_H_ */
