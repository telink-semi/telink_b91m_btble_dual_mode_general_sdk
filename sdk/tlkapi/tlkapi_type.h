/********************************************************************************************************
 * @file	tlkapi_type.h
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
#ifndef TLKAPI_TYPE_H
#define TLKAPI_TYPE_H


#ifndef BIT
#define BIT(n)          (1<<(n))
#endif

#ifndef true
#define true  1
#endif

#ifndef false
#define false  0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef bool
#define bool  unsigned char
#endif
#ifndef BOOL
#define BOOL  unsigned char
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef nullptr
#define nullptr  0
#endif

#define tlkapi_min(a,b)   ((a) > (b) ? (b) : (a))
#define tlkapi_max(a,b)   ((a) > (b) ? (a) : (b))


typedef unsigned int         uint;
typedef unsigned char        uchar;
typedef unsigned long        ulong;

typedef signed char          sint08;
typedef signed short         sint16;
typedef signed int           sint32;
typedef signed long long     sint64;


typedef unsigned char		 uint08;
typedef unsigned short		 uint16;
typedef long unsigned int    uint32;
typedef unsigned long long	 uint64;



#define SPLIT_WORD(word)     (word)&0xFF, ((word)&0xFF00)>>8


#define ARRAY_TO_UINT16L(array, offset, value)         \
	    (value)  = (array)[(offset)+1]; (value) <<= 8; \
	    (value) |= (array)[(offset)+0];
#define ARRAY_TO_UINT16H(array, offset, value)         \
	    (value)  = (array)[(offset)+0]; (value) <<= 8; \
	    (value) |= (array)[(offset)+1];

#define ARRAY_TO_UINT24L(array, offset, value)         \
		(value) |= (array)[(offset)+2]; (value) <<= 8; \
		(value) |= (array)[(offset)+1]; (value) <<= 8; \
		(value) |= (array)[(offset)+0];
#define ARRAY_TO_UINT24B(array, offset, value)         \
		(value) |= (array)[(offset)+0]; (value) <<= 8; \
		(value) |= (array)[(offset)+1]; (value) <<= 8; \
		(value) |= (array)[(offset)+2];

#define ARRAY_TO_UINT32L(array, offset, value)         \
	    (value)  = (array)[(offset)+3]; (value) <<= 8; \
	    (value) |= (array)[(offset)+2]; (value) <<= 8; \
	    (value) |= (array)[(offset)+1]; (value) <<= 8; \
	    (value) |= (array)[(offset)+0];
#define ARRAY_TO_UINT32H(array, offset, value)         \
	    (value)  = (array)[(offset)+0]; (value) <<= 8; \
	    (value) |= (array)[(offset)+1]; (value) <<= 8; \
	    (value) |= (array)[(offset)+2]; (value) <<= 8; \
	    (value) |= (array)[(offset)+3];

#define ARRAY_TO_UINT64L(array, offset, value)         \
		(value)  = (array)[(offset)+7]; (value) <<= 8; \
		(value) |= (array)[(offset)+6]; (value) <<= 8; \
		(value) |= (array)[(offset)+5]; (value) <<= 8; \
		(value) |= (array)[(offset)+4]; (value) <<= 8; \
		(value)  = (array)[(offset)+3]; (value) <<= 8; \
		(value) |= (array)[(offset)+2]; (value) <<= 8; \
		(value) |= (array)[(offset)+1]; (value) <<= 8; \
		(value) |= (array)[(offset)+0];
#define ARRAY_TO_UINT64H(array, offset, value)         \
		(value)  = (array)[(offset)+0]; (value) <<= 8; \
		(value) |= (array)[(offset)+1]; (value) <<= 8; \
		(value) |= (array)[(offset)+2]; (value) <<= 8; \
		(value) |= (array)[(offset)+3]; (value) <<= 8; \
		(value) |= (array)[(offset)+4]; (value) <<= 8; \
		(value) |= (array)[(offset)+5]; (value) <<= 8; \
		(value) |= (array)[(offset)+6]; (value) <<= 8; \
		(value) |= (array)[(offset)+7];


#define UINT16L_TO_ARRAY(value, array, offset)       \
		(array)[(offset)+0] = ((value) & 0xFF);      \
		(array)[(offset)+1] = ((value) & 0xFF00)>>8;
#define UINT16H_TO_ARRAY(value, array, offset)       \
		(array)[(offset)+1] = ((value) & 0xFF);      \
		(array)[(offset)+0] = ((value) & 0xFF00)>>8;

#define UINT24L_TO_ARRAY(value, array, offset)       \
		(array)[(offset)+0] = ((value) & 0xFF); 	 \
		(array)[(offset)+1] = ((value) & 0xFF00)>>8; \
		(array)[(offset)+2] = ((value) & 0xFF0000)>>16;
#define UINT24B_TO_ARRAY(value, array, offset)       \
		(array)[(offset)+3] = ((value) & 0xFF); 	 \
		(array)[(offset)+2] = ((value) & 0xFF00)>>8; \
		(array)[(offset)+1] = ((value) & 0xFF0000)>>16;

#define UINT32L_TO_ARRAY(value, array, offset)           \
		(array)[(offset)+0] = ((value) & 0xFF); 	     \
		(array)[(offset)+1] = ((value) & 0xFF00)>>8;     \
		(array)[(offset)+2] = ((value) & 0xFF0000)>>16;  \
		(array)[(offset)+3] = ((value) & 0xFF000000)>>24; 
#define UINT32H_TO_ARRAY(value, array, offset)            \
		(array)[(offset)+3] = ((value) & 0xFF); 	      \
		(array)[(offset)+2] = ((value) & 0xFF00)>>8;      \
		(array)[(offset)+1] = ((value) & 0xFF0000)>>16;   \
		(array)[(offset)+0] = ((value) & 0xFF000000)>>24; 

#define UINT64L_TO_ARRAY(value, array, offset)                   \
		(array)[(offset)+0] = ((value) & 0xFF); 	             \
		(array)[(offset)+1] = ((value) & 0xFF00)>>8;             \
		(array)[(offset)+2] = ((value) & 0xFF0000)>>16;          \
		(array)[(offset)+3] = ((value) & 0xFF000000)>>24;        \
		(array)[(offset)+4] = ((value) & 0xFF00000000)>>32;      \
		(array)[(offset)+5] = ((value) & 0xFF0000000000)>>40;    \
		(array)[(offset)+6] = ((value) & 0xFF000000000000)>>48;  \
		(array)[(offset)+7] = ((value) & 0xFF00000000000000)>>56;
#define UINT64H_TO_ARRAY(value, array, offset)                   \
		(array)[(offset)+7] = ((value) & 0xFF); 	             \
		(array)[(offset)+6] = ((value) & 0xFF00)>>8;             \
		(array)[(offset)+5] = ((value) & 0xFF0000)>>16;          \
		(array)[(offset)+4] = ((value) & 0xFF000000)>>24;        \
		(array)[(offset)+3] = ((value) & 0xFF00000000)>>32;      \
		(array)[(offset)+2] = ((value) & 0xFF0000000000)>>40;    \
		(array)[(offset)+1] = ((value) & 0xFF000000000000)>>48;  \
		(array)[(offset)+0] = ((value) & 0xFF00000000000000)>>56; 






#endif //TLKAPI_TYPE_H

