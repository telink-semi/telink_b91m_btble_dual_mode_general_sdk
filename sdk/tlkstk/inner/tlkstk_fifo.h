/********************************************************************************************************
 * @file	tlkstk_fifo.h
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
#ifndef TLKSTK_FIFO_H
#define TLKSTK_FIFO_H

#include "types.h"
#include "compiler.h"

typedef	struct {
	uint32_t		size;
	uint16_t		num;
	uint8_t		    wptr;
	uint8_t		    rptr;
	uint8_t*		p;
}	__attribute__ ((aligned (4))) my_fifo_t;

void my_fifo_init (my_fifo_t *f, int s, uint8_t n, uint8_t *p);

uint8_t* my_fifo_wptr (my_fifo_t *f);





#define		MYFIFO_INIT(name,size,n)		/*__attribute__ ((aligned (4)))*/ uint8_t name##_b[size * n]__attribute__((aligned(4)))/*={0}*/;my_fifo_t name = {size,n,0,0, name##_b}

#define		MYFIFO_INIT_IRAM(name,size,n)		/*__attribute__ ((aligned (4)))*/ _attribute_iram_data_ uint8_t name##_b[size * n]__attribute__((aligned(4)))/*={0}*/;\
							_attribute_data_retention_sec_ my_fifo_t name = {size,n,0,0, name##_b}


#endif /* TLKSTK_FIFO_H */
