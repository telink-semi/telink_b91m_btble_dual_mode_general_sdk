/********************************************************************************************************
 * @file    bqb.h
 *
 * @brief   This is the header file for B91m
 *
 * @author  Driver Group
 * @date    2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef BQB_H_
#define BQB_H_

//#include "../app_config.h"

//#if(TEST_DEMO==BQB_DEMO)

/**
 *  @brief  command type for BQB Test
 */
typedef enum{
	CMD_SETUP=0,
	CMD_RX_TEST=1,
	CMD_TX_TEST=2,
	CMD_END=3,
}Test_Command_e;

/**
 *  @brief  command status for BQB Test
 */
typedef enum {
	SETUP_STATE=0x10,
	RX_STATE,
	TX_STATE,
	END_STATE
}Test_Status_e;


/**
 *  @brief  union structure for configuration
 */
typedef struct
{
	unsigned int access_code;
	unsigned short uart_tx;
	unsigned short uart_rx;
	unsigned short pa_addr;
	unsigned char flash;
	unsigned char cal_pos;
	unsigned char power_mode;
	unsigned char br_power;
	unsigned char edr_power;
	unsigned char cap;
	unsigned char swire_through_usb_en;
	unsigned char pa_en:1;
	unsigned char pa_bypass_en:1;
	unsigned char power_control_en:1;
}bqb_setting_t;

/* global value for configuration */
extern bqb_setting_t g_bqb_setting;

/* global value for uart */
//extern uart_num_redef_e uart_using;
void rd_usr_definition(unsigned char _s);

void bqb_pa_init();

//#endif

#endif /* BQB_H_ */
