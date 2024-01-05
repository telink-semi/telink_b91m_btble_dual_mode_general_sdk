/********************************************************************************************************
 * @file	string.c
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
#ifndef		DEBUG_MEM_VIOLATION
#define		DEBUG_MEM_VIOLATION		0
#endif
#ifndef assert
#define assert(expression)
#endif

_attribute_retention_code_ int tstrlen(const char *pStr)
{
	int len = 0;
	
	if(pStr != nullptr){
		while((*pStr++) != '\0'){
			len++;
			if(len >= 0xFFFFF) break;
		}
	}
	return len;
}
_attribute_retention_code_ void *  memset(void * dest, int val, unsigned int len)
{
	register unsigned char *ptr = (unsigned char*) dest;
	while (len--)
		*ptr++ = (unsigned char)val;
	return dest;
}
_attribute_retention_code_ int tmemcmp(const void * m1, const void *m2, u32 len)
{
	u8 *st1 = (u8 *) m1;
	u8 *st2 = (u8 *) m2;

	while(len--){
		if(*st1 != *st2){
			return (*st1 - *st2);
		}
		st1++;
		st2++;
	}
	return 0;
}


_attribute_retention_code_ int tmemcmp4(void * m1, void * m2, register unsigned int len)
{
	unsigned int *st1 = (unsigned int *) m1;
	unsigned int *st2 = (unsigned int *) m2;
	unsigned int word_len = len >> 2;
	while(word_len--){
		if(*st1 != *st2){
			return 1; //return (*st1 - *st2)
		}
		st1++;
		st2++;
	}
	return 0;
}

#if DEBUG_MEM_VIOLATION
extern uint32_t _RETENTION_DATA_VMA_END,_RAMCODE_VMA_END,_RETENTION_RESET_VMA_END;
#endif

volatile uint32_t tdest_addr;
volatile uint32_t tdest_addr_end;

_attribute_retention_code_ void * tmemset(void * dest, int val, unsigned int len)
{
#if DEBUG_MEM_VIOLATION
	uint32_t retention_reset_end 	= (uint32_t)&_RETENTION_RESET_VMA_END;
	uint32_t retention_code_start 	= (uint32_t)&_RETENTION_DATA_VMA_END;
	uint32_t ram_code_end 			= (uint32_t)&_RAMCODE_VMA_END;
	uint32_t dest_addr    			= (uint32_t)dest;
	uint32_t dest_addr_end    		= (uint32_t)dest+len;
	tdest_addr = dest_addr;
	tdest_addr_end = dest_addr_end;
//	if(((dest_addr>=retention_code_start) && (dest_addr<ram_code_end))||((dest_addr_end>=retention_code_start) && (dest_addr_end<ram_code_end)))
	if(((dest_addr>=retention_code_start) && (dest_addr<ram_code_end))||((dest_addr_end>=retention_code_start) && (dest_addr_end<ram_code_end)) || (dest_addr<retention_reset_end))
	{
		__asm__ volatile("ecall");
		return 0;
	}
#endif
	register unsigned char *ptr = (unsigned char*) dest;
	while (len--)
		*ptr++ = (unsigned char)val;
	return dest;
}

_attribute_retention_code_ void tmemcpy(void * out, const void * in, unsigned int length)
{
#if DEBUG_MEM_VIOLATION
	uint32_t retention_reset_end 	= (uint32_t)&_RETENTION_RESET_VMA_END;
	uint32_t retention_code_start 	= (uint32_t)&_RETENTION_DATA_VMA_END;
	uint32_t ram_code_end 			= (uint32_t)&_RAMCODE_VMA_END;
	uint32_t dest_addr    			= (uint32_t)out;
	uint32_t dest_addr_end    		= (uint32_t)out+length;
	tdest_addr = dest_addr;
	tdest_addr_end = dest_addr_end;
//	if(((dest_addr>=retention_code_start) && (dest_addr<ram_code_end))||((dest_addr_end>=retention_code_start) && (dest_addr_end<ram_code_end)))
	if(((dest_addr>=retention_code_start) && (dest_addr<ram_code_end))||((dest_addr_end>=retention_code_start) && (dest_addr_end<ram_code_end)) || (dest_addr<retention_reset_end))
	{
		__asm__ volatile("ecall");
		return;
	}
#endif
	u8 *pi = (u8*) in;
	u8 *po = (u8*) out;
	while (length--)
	{
		*po++ = *pi++;
	}	

	//return out;
}

// for performance, assume length % 4 == 0,  and no memory overlapped
_attribute_retention_code_ void tmemcpy4(void * d, const void * s, unsigned int length)
{
#if DEBUG_MEM_VIOLATION
	uint32_t retention_reset_end 	= (uint32_t)&_RETENTION_RESET_VMA_END;
	uint32_t retention_code_start 	= (uint32_t)&_RETENTION_DATA_VMA_END;
	uint32_t ram_code_end 			= (uint32_t)&_RAMCODE_VMA_END;
	uint32_t dest_addr    			= (uint32_t)d;
	uint32_t dest_addr_end    		= (uint32_t)d+length*4;
	tdest_addr = dest_addr;
	tdest_addr_end = dest_addr_end;
//	if(((dest_addr>=retention_code_start) && (dest_addr<ram_code_end))||((dest_addr_end>=retention_code_start) && (dest_addr_end<ram_code_end)))
	if(((dest_addr>=retention_code_start) && (dest_addr<ram_code_end))||((dest_addr_end>=retention_code_start) && (dest_addr_end<ram_code_end)) || (dest_addr<retention_reset_end))
	{
		__asm__ volatile("ecall");
		return;
	}
#endif
	int* dst = (int*)d;
	int* src = (int*)s;
	assert((((int)dst) >> 2) << 2 == ((int)dst));			// address must alighn to 4
	assert((((int)src) >> 2) << 2 == ((int)src));			// address must alighn to 4
	assert((length >> 2) << 2 == length);					// length % 4 == 0
	assert(( ((char*)dst) + length <= (const char*)src) || (((const char*)src) + length <= (char*)dst));	//  no overlapped
	unsigned int len = length >> 2;
	while(len --){
		*dst++ = *src++;
	}
}


#if 0
void __assert_func(const char *mess)
{

}
#endif
