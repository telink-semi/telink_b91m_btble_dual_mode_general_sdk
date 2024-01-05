/********************************************************************************************************
 * @file	types.h
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
#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
#ifndef NULL
#define NULL  0
#endif

#ifndef nullptr
#define nullptr  0
#endif

#ifndef TRUE
#define TRUE     1
#endif

#ifndef FALSE
#define FALSE     0
#endif


#ifndef min
#define min(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)	((a) > (b) ? (a): (b))
#endif


#define bool	_Bool
//typedef unsigned char   uint8_t  ;
//typedef unsigned short  uint16_t ;
//typedef long unsigned int  uint32_t ;
//typedef signed char     int8_t   ;
//typedef signed short    int16_t  ;
//typedef signed  int    int32_t  ;


typedef unsigned char        u8  ;
typedef unsigned short       u16 ;
typedef long unsigned int    u32 ;
typedef signed char          s8  ;
typedef signed short         s16 ;
typedef signed int           s32 ;

typedef long long s64;
typedef unsigned long long u64;


//typedef unsigned long uint32_t;



typedef unsigned char		 uint08;
typedef unsigned short		 uint16;
typedef long unsigned int	 uint32;
typedef unsigned long long	 uint64;



#endif /* TYPES_H_ */
