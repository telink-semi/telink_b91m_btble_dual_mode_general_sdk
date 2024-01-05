/********************************************************************************************************
 * @file	btp.h
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
#ifndef BTP_H
#define BTP_H

#if (TLK_STK_BTP_ENABLE)


int btp_init(void);
void btp_deinit(void);

void btp_handler(void);

void btp_destroy(uint16 aclHandle);

int btp_needMemLen(void);

/******************************************************************************
 * Function: btp_setConnTimeout
 * Descript: Set the time from establishment of the Profile to connection timeout.
 * Params:
 *     @timeout[IN]--The time of connection timeout. Unit-ms, Range[3000~15000].
 * Return: None.
 *******************************************************************************/
void btp_setConnTimeout(uint16 timeout);
void btp_setDiscTimeout(uint16 timeout);


#endif // #if (TLK_STK_BTP_ENABLE)

#endif // BTP_H
