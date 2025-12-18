/********************************************************************************************************
 * @file	tlkstk_utility.h
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
#ifndef TLKTK_UTILITY_H
#define TLKTK_UTILITY_H

#include "types.h"




#define STATIC_ASSERT(X)    	{if(!(X)) while(1);}


#define OFFSETOF(s, m) 			((unsigned int) &((s *)0)->m)

//  direct memory access
#define U8_GET(addr)			(*(volatile uint8_t  *)(addr))
#define U16_GET(addr)			(*(volatile unsigned short *)(addr))
#define U32_GET(addr)			(*(volatile unsigned int  *)(addr))

#define U8_SET(addr, v)			(*(volatile uint8_t  *)(addr) = (uint8_t)(v))
#define U16_SET(addr, v)		(*(volatile unsigned short *)(addr) = (unsigned short)(v))
#define U32_SET(addr, v)		(*(volatile unsigned int  *)(addr) = (v))


#define MAKE_U16(h,l) 			((unsigned short)(((h) << 8) | (l)))
#define MAKE_U32(a,b,c,d)		((unsigned int)(((a) << 24) | ((b) << 16) | ((c) << 8) | (d)))


#define IS_POWER_OF_2(x)		(!(x & (x-1)))
#define IS_4BYTE_ALIGN(x) 		(!(x & 3))


// better than xor swap:  http://stackoverflow.com/questions/3912699/why-swap-with-xor-works-fine-in-c-but-in-java-doesnt-some-puzzle
#define SWAP(x, y, T) 			do { T tmp = (x); (x) = (y); (y) = tmp; } while(0)
#define SORT2(a, b, T) 			do { if ((a) > (b)) SWAP((a), (b), T); } while (0)

#define foreach(i, n) 			for(int i = 0; i < (n); ++i)

#define ARRAY_SIZE(a) 			(sizeof(a) / sizeof(*a))

#define everyN(i, n) 			++(i); (i)=((i) < N ? (i) : 0); if(0 == (i))

#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

#define U32_BYTE0(a) ((a) & 0xFF)
#define U32_BYTE1(a) (((a) >> 8) & 0xFF)
#define U32_BYTE2(a) (((a) >> 16) & 0xFF)
#define U32_BYTE3(a) (((a) >> 24) & 0xFF)

#define U32_MAX ((uint32_t)0xffffffff)
#define U16_MAX ((uint16_t)0xffff)
#define U8_MAX ((uint8_t)0xff)


void swapN (uint8_t *p, int n);
void swapX(const uint8_t *src, uint8_t *dst, int len);




static inline int abs_ram(signed int i)
{
	return i >= 0 ? i:-i;
}



#endif /* TLKTK_UTILITY_H */
