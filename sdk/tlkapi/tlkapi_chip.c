/********************************************************************************************************
 * @file	tlkapi_chip.c
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
#include "stdbool.h"
#include "string.h"
#include "drivers.h"
#include "tlkapi/tlkapi_type.h"
#include "tlkapi/tlkapi_error.h"
#include "tlkapi/tlkapi_chip.h"


extern unsigned int trng_rand(void);

/******************************************************************************
 * Object: _STACK_TOP, _BSS_VMA_END
 * Brief : The location of the D-RAM segment in the system that is not 
 *         currently being used.
 *****************************************************************************/
extern unsigned long _STACK_TOP, _BSS_VMA_END;


static uint32 sTlkApiChipStackStart = 0;
static uint32 sTlkApiChipStackCurLen = 0;
static uint32 sTlkApiChipStackLength = 0;



/******************************************************************************
 * Function: tlkapi_chip_stackInit
 * Descript: Initialize the stack that is not used in the chip.
 * Params: None.
 * Return: None.
 * Notice:
 *     1.The position of calling the interface should be as close as possible 
 *       to the beginning position of system initialization; otherwise, the 
 *       value initialized by other variables will be tampered with, resulting 
 *       in abnormal system operation. 
 *     2.If the unused stack in the system is already smaller than 
 *       "TLKAPI_CHIP_STACK_CHACK_OFFSET", the detection function will fail.
 *     3.This interface is only used to detect D-RAM usage.
 *     4.Because the detection function consumes MCU bandwidth, do not invoke 
 *       it unless necessary.
 *****************************************************************************/
void tlkapi_chip_stackInit(void)
{
	uint32 index;
	uint32 saddr;
	uint32 eaddr;
	uint32 length;
	volatile uint32 *pStart;

	eaddr = (unsigned long)&_STACK_TOP;
	saddr = (unsigned long)&_BSS_VMA_END;

	if(eaddr < 0x1000 || saddr >= eaddr) return;
	eaddr = (eaddr - 0) & 0xFFFFFFFC;
	saddr = (saddr + 4) & 0xFFFFFFFC;
	
	sTlkApiChipStackStart = saddr;
	if(saddr+TLKAPI_CHIP_STACK_CHACK_OFFSET >= eaddr){
		sTlkApiChipStackCurLen = 0;
		sTlkApiChipStackLength = 0;
	}else{
		sTlkApiChipStackLength = eaddr-saddr;
		sTlkApiChipStackCurLen = TLKAPI_CHIP_STACK_CHACK_OFFSET;
	}
	
	length = (sTlkApiChipStackLength-sTlkApiChipStackCurLen)>>2;
	pStart = (volatile uint32*)sTlkApiChipStackStart;
	for(index=0; index<length; index++){
		pStart[index] = TLKAPI_CHIP_STACK_CHACK_SIGN;
	}
}

/******************************************************************************
 * Function: tlkapi_chip_stackCheck
 * Descript: Detects the current stack usage.
 * Params: None.
 * Return: The usage length of the current stack.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackInit()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *****************************************************************************/
uint tlkapi_chip_stackCheck(void)
{
	uint32 length;
	volatile uint32 *pStart;
	
	if(sTlkApiChipStackCurLen >= sTlkApiChipStackLength) return sTlkApiChipStackCurLen;
	length = (sTlkApiChipStackLength-sTlkApiChipStackCurLen)>>2;
	if(length <= 2){
		sTlkApiChipStackCurLen = sTlkApiChipStackLength;
	}else{
		uint32 index;
		pStart = (volatile uint32*)(sTlkApiChipStackStart);
		for(index=length-1; index>1; index--){
			if(pStart[index] == TLKAPI_CHIP_STACK_CHACK_SIGN && pStart[index-1] == TLKAPI_CHIP_STACK_CHACK_SIGN){
				break;
			}
		}
		if(index <= 1) sTlkApiChipStackCurLen = sTlkApiChipStackLength;
		else sTlkApiChipStackCurLen += (length-index-1)<<2;
	}
	return sTlkApiChipStackCurLen;
}

/******************************************************************************
 * Function: tlkapi_chip_stackUsed
 * Descript: Obtains the usage length of the current stack.
 * Params: None.
 * Return: The usage length of the current stack.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackCheck()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *****************************************************************************/
uint tlkapi_chip_stackUsed(void)
{
	return sTlkApiChipStackCurLen;
}

/******************************************************************************
 * Function: tlkapi_chip_stackDepth
 * Descript: Obtains the depth of the system stack.
 * Params: None.
 * Return: The depth of the current stack.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackInit()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *     3.For generated programs, the depth of the stack is fixed, that is, it 
 *       does not change as the system runs.
 *****************************************************************************/
uint tlkapi_chip_stackDepth(void)
{
	return sTlkApiChipStackLength;
}

/******************************************************************************
 * Function: tlkapi_chip_stackOverflow
 * Descript: Detect if the system is running out of bounds (the remaining stack
 *           of the system is 0).
 * Params: None.
 * Return: True indicates that the system runs out of bounds.
 * Notice:
 *     1.The interface should be called after "tlkapi_chip_stackInit()".
 *     2.The actual use depth of the stack requires the return value of the 
 *       interface based on the addition of "TLKAPI_CHIP_STACK_CHACK_OFFSET".
 *     3.For generated programs, the depth of the stack is fixed, that is, it 
 *       does not change as the system runs.
 *     4.If the system has crossed the line, the user needs to pay attention to
 *       the use of the system stack, otherwise it may crash at any time.
 *****************************************************************************/
bool tlkapi_chip_stackOverflow(void)
{
	if(sTlkApiChipStackCurLen == sTlkApiChipStackLength) return true;
	else return false;
}


/******************************************************************************
 * Function: tlkapi_random
 * Descript: Generate a sequence of random numbers.
 * Params: 
 *     @pBuff[OUT]--Cache space for storing random sequences.
 *     @buffLen[IN]--The length of the cache space.
 * Return: None.
 *****************************************************************************/
void tlkapi_random(uint08 *pBuff, uint16 buffLen)
{
	int i;
	unsigned int randNums = 0;
    /* if len is odd */
	for(i=0; i<buffLen; i++){
		if((i & 3) == 0) randNums = trng_rand();
		pBuff[i] = randNums & 0xff;
		randNums >>=8;
	}
}


