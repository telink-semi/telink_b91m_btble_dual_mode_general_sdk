/********************************************************************************************************
 * @file	tlkapi_string.h
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
#ifndef TLKAPI_STRING_H
#define TLKAPI_STRING_H


#define tlkapi_uint08ToStr(str,value,isDropZero)     tlkapi_hexToStr(str, value, 2, isDropZero)
#define tlkapi_uint16ToStr(str,value,isDropZero)     tlkapi_hexToStr(str, value, 4, isDropZero)
#define tlkapi_uint32ToStr(str,value,isDropZero)     tlkapi_hexToStr(str, value, 8, isDropZero)


int tlkapi_wcharStrlen(uint08 *pStr, uint16 maxLen);

char tlkapi_ascii_char2val(const  char c);
int tlkapi_ascii_str2val(const char str[], char base);



int tlkapi_decToStr(char *str, uint32 dec, uint num, bool isDropZero);
int tlkapi_hexToStr(char *str, uint32 hex, uint num, bool isDropZero);
int tlkapi_strToInt32(char *str, int strLen, sint32 *pValue);
int tlkapi_strToInt08(char *str, int strLen, sint08 *pValue);
int tlkapi_strToInt16(char *str, int strLen, sint16 *pValue);
int tlkapi_strToUint32(char *str, int strLen, uint32 *pValue);
int tlkapi_strToUint08(char *str, int strLen, uint08 *pValue);
int tlkapi_strToUint16(char *str, int strLen, uint16 *pValue);
int tlkapi_strToArray(char *str, int strLen, uint08 *pHex, int hexLen);
int tlkapi_arrayToStr(uint08 *pHex, int hexLen, char *str, int strLen, char split);


char *tlkapi_str_findChar(char *pSrc, uint16 srcLen, char dst, uint16 *pOffset);
char *tlkapi_str_findStr(char *pSrc, uint16 srcLen, char *pDst, uint16 dstLen, uint16 *pOffset);



#endif //TLKAPI_STRING_H

