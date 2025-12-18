/********************************************************************************************************
 * @file	tlkapi_char.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "string.h" 
#include "tlk_config.h"
#include "tlkapi/tlkapi_type.h"
#include "tlkapi/tlkapi_error.h"
#include "tlkapi/tlkapi_char.h"



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
int tlkapi_utf8ToUnicode(uint16 *pDst, uint16 dstLen, uint08 *pSrc, uint16 srcLen)
{
    int ii, iii;
	uint16 unicode = 0;
    uint16 codeLen = 0;
    //bool chinese_flag = FALSE;

	uint08 count = 0;

	if(pDst == NULL || dstLen == 0 || pSrc == NULL || srcLen == 0){
		return 0;
	}
	
    while(srcLen != 0){
        //1. UTF-8 ---> Unicode
        if(0 == (pSrc[0] & 0x80)){
            // one byte
            codeLen = 1;
            unicode = pSrc[0];
        }else if(0xC0 == (pSrc[0] & 0xE0) && 0x80 == (pSrc[1] & 0xC0)){// two bytes
        	codeLen = 2;
        	if(srcLen < codeLen) break;
            unicode = (int)((((int)pSrc[0] & 0x001F) << 6) | ((int)pSrc[1] & 0x003F));
        }else if(0xE0 == (pSrc[0] & 0xF0) && 0x80 == (pSrc[1] & 0xC0) && 0x80 == (pSrc[2] & 0xC0)){// three  bytes
            codeLen = 3;
			if(srcLen < codeLen) break;
            //chinese_flag = TRUE;
            ii = (((int)pSrc[0] & 0x000F) << 12);
            iii = (((int)pSrc[1] & 0x003F) << 6);
            unicode = ii|iii|((int)pSrc[2] & 0x003F);
            unicode = (int)((((int)pSrc[0] & 0x000F) << 12) | (((int)pSrc[1] & 0x003F) << 6) | ((int)pSrc[2] & 0x003F));
        }else if(0xF0 == (pSrc[0] & 0xF0) && 0x80 == (pSrc[1] & 0xC0) && 0x80 == (pSrc[2] & 0xC0) && 0x80 == (pSrc[3] & 0xC0)){// four bytes
            codeLen = 4;
			if(srcLen < codeLen) break;
            unicode = (((int)(pSrc[0] & 0x07)) << 18) | (((int)(pSrc[1] & 0x3F)) << 12) | (((int)(pSrc[2] & 0x3F)) << 6) | (pSrc[3] & 0x3F);
        }else{
            break;
        }

		srcLen -= codeLen;
        pSrc += codeLen;
        if(unicode < 0x80){
            if (count == 0 && unicode == 0x20) continue;
        }
		pDst[count++] = unicode;
        if(count == dstLen) break;
	} // end while
	
    return count;
}







