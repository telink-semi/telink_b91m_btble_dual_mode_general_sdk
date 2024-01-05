/********************************************************************************************************
 * @file	tlkalg_crc32.h
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
#ifndef TLKALG_CRC32_H
#define TLKALG_CRC32_H



void tlkalg_crc32_init(uint32 *pCrc);
void tlkalg_crc32_update(uint32 *pCrc, uint08 *pData, uint16 dataLen);
void tlkalg_crc32_finish(uint32 *pCrc);

void tlkalg_crc32_result(uint32 *pCrc, uint08 *pData, uint16 dataLen);



#endif //TLKALG_CRC32_H

