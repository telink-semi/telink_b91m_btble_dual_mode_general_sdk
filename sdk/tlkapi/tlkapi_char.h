/********************************************************************************************************
 * @file	tlkapi_char.h
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
#ifndef TLKAPI_CHAR_H
#define TLKAPI_CHAR_H




/******************************************************************************
 * Function: tlkapi_utf8ToUnicode
 * Descript: Convert UTF8 encoding to Unicode encoding.
 * Params:
 *     @pDst[IN]--The string of UTF8 encoding.
 *     @pSrc[OUT]--The string of unicode encoding.
 *     @dstLen[IN]--The length of the input UTF-8 string.
 *     @srcLen[IN]--The length of the output UNICODE string.
 * Return: The number of actual UNICODE characters encoded.
 * Notice:
 *     1.Each UNICODE character takes up 2 bytes of space.
 *****************************************************************************/
int tlkapi_utf8ToUnicode(uint16 *pDst, uint16 dstLen, uint08 *pSrc, uint16 srcLen);





#endif //TLKAPI_CHAR_H

