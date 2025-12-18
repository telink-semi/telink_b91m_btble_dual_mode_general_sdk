/********************************************************************************************************
 * @file    config_obj.c
 *
 * @brief   This is the header file for B91m
 *
 * @author  Driver Group
 * @date    2023
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "config_obj.h"
#include <string.h>

volatile __attribute__((section(".config_data"))) unsigned int config_data_sec[CONFIG_OBJ_MAX_SIZE] = {
		CONFIG_CHECK_VAL_BEGIN_U32, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, CONFIG_CHECK_VAL_END_U32};

static unsigned char s_config_data_flag = 0;

void config_obj_init()
{
	tlk_config_obj_ptr ptr = (tlk_config_obj_ptr)config_data_sec;

	//check check_val
	if(ptr->check_val_begin != CONFIG_CHECK_VAL_BEGIN_U32 || ptr->check_val_end != CONFIG_CHECK_VAL_END_U32)
	{
		s_config_data_flag = 1;
		return;
	}

	//check bit1_cnt
	unsigned int *data = (unsigned int*)config_data_sec + 2;
	unsigned int bit1_cnt = 0;
	for(int i = 0; i < CONFIG_OBJ_MAX_SIZE-3; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			bit1_cnt += ((data[i] >> j) & 1);
		}
	}
	if(ptr->bit1_cnt != bit1_cnt || (bit1_cnt == 0))
	{
		s_config_data_flag = 2;
		return;
	}

	//check offset and size
	for(unsigned int i = 0; i < ptr->pointer_cnt; i++)
	{
		if(ptr->content.pointer[i].size + ptr->content.pointer[i].offset > ((CONFIG_OBJ_MAX_SIZE-1)*4))
		{
			s_config_data_flag = 3;
			break;
		}

		if(i)
		{
			if(ptr->content.pointer[i-1].size + ptr->content.pointer[i-1].offset > ptr->content.pointer[i].offset)
			{
				s_config_data_flag = 4;
				break;
			}
		}
	}

	return;
}

volatile int debug = 0;
unsigned char get_config_data(tlk_config_type_e type, unsigned char* buffer, unsigned short buffer_max_size)
{
	if(s_config_data_flag != 0) return 0;

	unsigned char ret = 0;
	unsigned short size;
	tlk_config_obj_ptr ptr = (tlk_config_obj_ptr)config_data_sec;
	for(unsigned int i = 0; i <  ptr->pointer_cnt; i++)
	{
		if(ptr->content.data[i] == 0)
		{
			continue;
		}

		if(ptr->content.pointer[i].type == type)
		{
			size = ((ptr->content.pointer[i].size <= buffer_max_size)? ptr->content.pointer[i].size : buffer_max_size);
			memcpy(buffer, ((unsigned char*)config_data_sec) + ptr->content.pointer[i].offset, size);
			ret = 1;
			break;
		}
	}

	return ret;
}

unsigned short get_config_offset(tlk_config_type_e type)
{
	if(s_config_data_flag != 0) return 0;

	unsigned short ret = 0;
	tlk_config_obj_ptr ptr = (tlk_config_obj_ptr)config_data_sec;
	for(unsigned int i = 0; i <  ptr->pointer_cnt; i++)
	{
		if(ptr->content.data[i] == 0)
		{
			continue;
		}

		if(ptr->content.pointer[i].type == type)
		{
			ret = ptr->content.pointer[i].offset;
			break;
		}
	}

	return ret;
}

unsigned char get_config_data_byte(unsigned short offset)
{
	unsigned char *data = (unsigned char*)config_data_sec;
	return data[offset];
}

unsigned short get_config_data_half_word(unsigned short offset)
{
	unsigned char *data = (unsigned char*)config_data_sec;
	return data[offset] | (data[offset+1] << 8);
}

unsigned int get_config_data_word(unsigned short offset)
{
	unsigned char *data = (unsigned char*)config_data_sec;
	return data[offset] | (data[offset+1] << 8) | (data[offset+2] << 16) | (data[offset+3] << 24);
}
