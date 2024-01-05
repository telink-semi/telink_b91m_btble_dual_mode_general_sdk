/********************************************************************************************************
 * @file	tlkpto_test.h
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
#ifndef TLKPTO_TEST_H
#define TLKPTO_TEST_H



typedef enum
{
	TLKPTO_TEST_TYPE_NONE = 0x00,
	TLKPTO_TEST_TYPE_PTS  = 0x01,
	TLKPTO_TEST_TYPE_FAT  = 0x02,
	TLKPTO_TEST_TYPE_RDT  = 0x03,
	TLKPTO_TEST_TYPE_EMI  = 0x04,
	TLKPTO_TEST_TYPE_USR  = 0x05,
}TLKPTO_TEST_TYPE_ENUM;






#endif //TLKPTO_TEST_H

