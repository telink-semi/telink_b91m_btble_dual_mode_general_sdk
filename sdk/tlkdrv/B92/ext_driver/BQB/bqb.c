/********************************************************************************************************
 * @file    bqb.c
 *
 * @brief   This is the source file for B91m
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
#include "bqb.h"
#include "../setting/config_obj.h"
#include "../PA/pa.h"
#include "../../reg_include/register.h"
//#include "../ext_btrf.h"
#include "driver.h"
//#if(TEST_DEMO==BQB_DEMO)

/* Set to 1, the program includes a non-standard BQB protocol part, which adds new functions for factory testing (single tone, rssi, sending fixed packets)*/
#define BQB_PRIVATE_AGREEMENT		0
#define BQB_TX_POWER		0
/*
 * @brief This macro is defined to turn on the fastsettle function
 * */
#define RF_FAST_SETTLE      0
bqb_setting_t g_bqb_setting = {
//	.access_code = ACCESS_CODE,
//	.uart_tx = BQB_UART_TX_PORT,
//	.uart_rx = BQB_UART_RX_PORT,
	.pa_addr = 0,
//	.flash = FLASH_SIZE_1M,
	.cal_pos = 1,
	.power_mode = LDO_1P4_LDO_2P0,
//	.power = BQB_TX_POWER,
	.cap = 1,
	.swire_through_usb_en = 0,
	.pa_en = 0,
	.pa_bypass_en = 0,
	.power_control_en = 0,
};
//uart_num_redef_e uart_using = UART_NONE;
//rf_fast_settle_t fast_settle;

//static unsigned short pkt_cnt =0,cmd_pkt,l, h;
//static unsigned char chn, pkt_type,freq,uart_tx_index,uart_rx_index,para, ctrl;
//static unsigned char bb_mode = 1;
//static unsigned int pkt_interval;
//static unsigned int tick_rx = 0;
volatile unsigned int t0,tick_tx;
Test_Status_e test_state;
//static unsigned char rxpara_flag  = 1;

unsigned char	bqbtest_buffer[272] __attribute__ ((aligned (4)));
unsigned char __attribute__ ((aligned (4))) bqbtest_pkt [264] = {
	37, 0, 0, 0,
	0, 37,
	0, 1, 2, 3, 4, 5, 6, 7
};

//static union pkt_length_u
//{
//	unsigned char len;
//	struct len_t
//	{
//		unsigned char low:6;
//		unsigned char upper:2;
//	}l;
//}pkt_length;

#if BQB_PRIVATE_AGREEMENT
unsigned char private_agreement_rssi = 0;
static union private_status_u
{
	unsigned int u32Status;
	struct
	{
		unsigned int parameter:16;
		unsigned int		  :6;
		unsigned int test_mode:1;
		unsigned int test_flag:1;
	}tStatus;
}uPrivateStatus;
typedef enum
{
	PRIVATE_TX_CARRIER_MODE = 0x00,
	PRIVATE_TX_BURST_MODE = 0x01,
}private_status_e;
#endif
void rd_usr_definition(unsigned char _s)
{
	if(_s)
	{
		static bqb_config_data_t bqb_cfg_data;
		if(get_config_data(TLK_CONFIG_TYPE_BQB, (unsigned char*)&bqb_cfg_data, sizeof(bqb_cfg_data)))
		{
			g_bqb_setting.uart_tx = get_pin(bqb_cfg_data.uart_tx);
			g_bqb_setting.uart_rx = get_pin(bqb_cfg_data.uart_rx);

			g_bqb_setting.cap = bqb_cfg_data.general_cfg.definition.cap;
			g_bqb_setting.cal_pos = bqb_cfg_data.general_cfg.definition.cal_pos;

			switch(bqb_cfg_data.power_mode)
			{
			case 0:
				g_bqb_setting.power_mode = LDO_1P4_LDO_2P0;
				break;
			case 1:
				g_bqb_setting.power_mode = DCDC_1P4_LDO_2P0;
				break;
			case 2:
				g_bqb_setting.power_mode = DCDC_1P4_DCDC_2P0;
				break;
			default:
				break;
			}

			g_bqb_setting.br_power = (bqb_cfg_data.power_index_br==0)?BQB_TX_POWER:bqb_cfg_data.power_index_br-1;
			g_bqb_setting.edr_power = (bqb_cfg_data.power_index_br==0)?BQB_TX_POWER:bqb_cfg_data.power_index_edr-1;
			g_bqb_setting.swire_through_usb_en = bqb_cfg_data.general_cfg.definition.swire_through_usb_en;
			g_bqb_setting.pa_en = bqb_cfg_data.general_cfg.definition.pa_en;
			g_bqb_setting.pa_bypass_en = bqb_cfg_data.general_cfg.definition.pa_bypass_en;
			g_bqb_setting.power_control_en = bqb_cfg_data.general_cfg.definition.power_control_en;
			g_bqb_setting.access_code = bqb_cfg_data.access_code;
		}
	}
	else
	{
		g_bqb_setting.flash = (flash_read_mid() >> 16) & 0xff;
	}
	if(g_bqb_setting.cap)
	{
		rf_turn_off_internal_cap();
	}
}

//static unsigned char a_paaddr=0;
void bqb_pa_init()
{
	if(g_bqb_setting.pa_en)
	{
		g_bqb_setting.pa_addr = get_config_offset(TLK_CONFIG_TYPE_PA);
		if(g_bqb_setting.pa_addr)
		{
			pa_setting_init(g_bqb_setting.pa_addr, g_bqb_setting.pa_bypass_en);
		}
		else
		{
			g_bqb_setting.pa_en = 0;
		}
	}

}


//#endif
