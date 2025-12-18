/*
 * emi_general.c
 *
 *  Created on: 2023Äê4ÔÂ23ÈÕ
 *      Author: Admin
 */


#include "../BT/rf_bt.h"
#include "./bt_emi.h"
#include "emi_general.h"
#include "../PA/pa.h"
#include "drivers.h"
#include "tlkstk/tlkstk_stdio.h"
#include "../calibration/calibration.h"
/**
 * @brief   Init the emi reference parameter
 */
unsigned char  g_power_level = 4;  // 0
unsigned char  g_chn = 2;          // 2
unsigned char  g_cmd_now = 1;      // 1
unsigned char  g_run = 1;          // 1
unsigned char  g_hop = 0;          // 0
unsigned char  g_tx_cnt = 0;       // 0
//unsigned short g_pa_setting = 0;
emi_RF_MODE_BT_e  g_mode = BR_DH3; //1
unsigned char  g_manual_setting = 0;
unsigned char  g_scale = 8;
unsigned char  g_slice = 29;
extern unsigned char mode_set_nslot;

#if EMI_SUPPORT_SETTING
/**
* @brief global variable to save EMI initial setting
*/
volatile emi_setting_t g_emi_setting;

#endif
void emi_setting_init(void);
void emi_init(void);
void emicarrieronly(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_prbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emirx(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitxprbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emitx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_tx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
void emi_con_tx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn);
/**
 * @brief   Init the structure of the emi test command and function
 */
test_list_t  ate_list[] = {
		{0x01,emicarrieronly},
		{0x02,emi_con_prbs9},
		{0x03,emirx},
		{0x04,emitxprbs9},
		{0x05,emitx55},
		{0x06,emitx0f},
		{0x07,emi_con_tx55},
		{0x08,emi_con_tx0f}
};
void emi_start(){
#if EMI_SUPPORT_SETTING
	emi_setting_init();
	if(g_emi_setting.general_setting.power_mode == 1)
	{
		analog_write_reg8(0x0a, DCDC_1P4_LDO_1P8);
//		gpio_set_level(GPIO_PD1,1);
	}
	else if(g_emi_setting.general_setting.power_mode == 2)
	{
		analog_write_reg8(0x0a, DCDC_1P4_DCDC_1P8);
//		gpio_set_level(GPIO_PD0,1);
	}
	else
	{
		analog_write_reg8(0x0a, LDO_1P4_LDO_1P8);
//		gpio_set_level(GPIO_PE7,1);
	}
#endif
#if EMI_SUPPORT_SETTING
	pa_setting_init(g_emi_setting.pa_setting_pos, g_emi_setting.general_setting.pa_bypass_en);
	if(g_emi_setting.general_setting.swire_through_usb_en)
	{
		usb_set_pin_en();
		gpio_set_up_down_res(GPIO_DM, GPIO_PIN_PULLUP_10K);
	}
#else
//</remove when release sdk>
#if SWIRE_THROUGH_USB_EN
	usb_set_pin_en();
	gpio_set_up_down_res(GPIO_DM, GPIO_PIN_PULLUP_10K);
#endif
//<remove when release sdk>
#endif

	emi_init();
	bt_emi_init();
}
void read_calibration_flash()
{
//#if (MCU_CORE_B91||MCU_CORE_B92) //Wait for the B92 calibration function to be added before changing here
	unsigned char flash_type = (flash_read_mid() >> 16) & 0xff;
	switch (flash_type)
	{
		case FLASH_SIZE_64K:
			user_calib_freq_offset(USER_CALIB_FROM_FLASH, FLASH_CAP_VALUE_ADDR_64K);
			break;
		case FLASH_SIZE_128K:
			user_calib_freq_offset(USER_CALIB_FROM_FLASH, FLASH_CAP_VALUE_ADDR_128K);
			break;
		case FLASH_SIZE_512K:
			user_calib_freq_offset(USER_CALIB_FROM_FLASH, FLASH_CAP_VALUE_ADDR_512K);
			break;
		case FLASH_SIZE_1M:
			user_calib_freq_offset(USER_CALIB_FROM_FLASH, FLASH_CAP_VALUE_ADDR_1M);
			break;
		case FLASH_SIZE_2M:
			user_calib_freq_offset(USER_CALIB_FROM_FLASH, FLASH_CAP_VALUE_ADDR_2M);
			break;
		case FLASH_SIZE_4M:
			user_calib_freq_offset(USER_CALIB_FROM_FLASH, FLASH_CAP_VALUE_ADDR_4M);
			break;
		default:
			break;
	}
//#endif
}
/////////////////////////////////////////////////////

/**
 * @brief		This function serves to EMI Init
 * @return 		none
 */

void emi_init(void)
{

	read_calibration_flash();
//<remove when release sdk>
#if EMI_SUPPORT_SETTING
	if(g_emi_setting.general_setting.cap)
	{
		rf_turn_off_internal_cap();
	}
#else
//</remove when release sdk>
#if CLOSE_INTERNAL_CAP_EN
	rf_turn_off_internal_cap();
#endif
//<remove when release sdk>
#endif
//</remove when release sdk>
//	rf_turn_off_internal_cap();
//	rf_access_code_comm(EMI_ACCESS_CODE);             // access code

    write_sram8(TX_PACKET_MODE_ADDR,g_tx_cnt);        // tx_cnt
    write_sram8(RUN_STATUE_ADDR,g_run);               // run
    write_sram8(TEST_COMMAND_ADDR,g_cmd_now);         // cmd
    write_sram8(POWER_ADDR,g_power_level);            // power
    write_sram8(CHANNEL_ADDR,g_chn);                  // chn
    write_sram8(RF_MODE_ADDR,g_mode);                 // mode
    write_sram8(CD_MODE_HOPPING_CHN,g_hop);           // hop
//    write_sram16(PA_SETTING_ADDR, g_pa_setting);
    write_sram8(RSSI_ADDR,0);                         // rssi
    write_sram32(RX_PACKET_NUM_ADDR,0);               // rx_packet_num
    write_sram8(SCALE_SETTING_ADDR, g_scale);
    write_sram8(SLICE_SETTING_ADDR,g_slice);
    write_sram8(MANUALE_EN_ADDR,g_manual_setting);

}
/**
 * @brief		This function serves to EMI CarryOnly
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emicarrieronly(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_TX);
	/* core_140c20 default value 0xc4, BIT(2) is 1, BIT(4) is 0, same as tbl_rf_1m
	 * for BLE: 0xc4, for BT: 0xd0
	 * core_140c21 default value 0x00 */
	REG_ADDR16(MODEM_MODE_CFG_RX1_0) |= BIT(2);
	/* default value 0x00
	 * for BLE: 0x00, for BT: 0xff */
	REG_ADDR32(BB_PDZB) = 0;

	rf_emi_stop();
	rf_set_tx_rx_off();
	dma_reset();
	rf_emi_reset_baseband();
	(void)(rf_mode);
	bt_emi_single_tone(rf_chn, pwr);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting));
	rf_emi_stop();
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI ConPrbs9
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr 	- power level of RF.
 * @param[in]   rf_chn 	- channel of RF.
 * @return 		none
 */
//volatile unsigned int debug;
volatile int BT_error_0 = 0;
void emi_con_prbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
#if 1
	pa_operation(PA_SETTING_STATE_TX);
	unsigned int tx_cnt = 0;
//	unsigned int t0 = reg_system_tick, chnidx = 1;
//	if(g_hop)
//	{
		bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_Prbs9);
//	}
//	else
//	{
//		bt_emi_tx_continue_mode_run(rf_mode,rf_chn,pwr,BT_PKT_Data_Prbs9);
//	}
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting))
	{
//		if(g_hop)
			bt_emi_tx_burst_mode_loop_hoop(&tx_cnt,mode_set_nslot);
	}
	bt_emi_stop();
#else
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_Prbs9);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting))
	{
		if(g_hop)
			bt_emi_tx_burst_mode_loop_hoop(&tx_cnt,mode_set_nslot);
	}
	bt_emi_stop();
#endif
}

/**
 * @brief		This function serves to EMI Rx
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr	    - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emirx(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_RX);
	(void)(pwr);
	unsigned int rx_cnt = 0;
	bt_emi_rx_mode_init(rf_mode, rf_chn);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) \
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting))
	{
		bt_emi_rx_mode_loop(&rx_cnt);
		if(rx_cnt != read_sram32(RX_PACKET_NUM_ADDR))
		{
		    write_sram8(RSSI_ADDR,(unsigned char)bt_get_rssi(0));
		    write_sram32(RX_PACKET_NUM_ADDR,rx_cnt);
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI TxPrbs
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emitxprbs9(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_TX);
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_Prbs9);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting))
	{
		bt_emi_tx_burst_mode_loop(&tx_cnt, mode_set_nslot);
		if(g_tx_cnt)
		{
			if(tx_cnt >= 1000)
				break;
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI Tx55
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */

void emitx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_TX);
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_0x55);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting))
	{
		bt_emi_tx_burst_mode_loop(&tx_cnt, mode_set_nslot);
		if(g_tx_cnt)
		{
			if(tx_cnt >= 1000)
				break;
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI Tx0f
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emitx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_TX);
	unsigned int tx_cnt = 0;
	bt_emi_tx_burst_mode_init(rf_mode, rf_chn, pwr, BT_PKT_Data_0x0f);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting))
	{
		bt_emi_tx_burst_mode_loop(&tx_cnt, mode_set_nslot);
		if(g_tx_cnt)
		{

			if(tx_cnt >= 1000)
				break;
		}
	}
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI_CON_TX55
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emi_con_tx55(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_TX);
	bt_emi_tx_continue_mode_run(rf_mode,rf_chn,pwr,BT_PKT_Data_0x55);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode)\
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting));
	bt_emi_stop();
}

/**
 * @brief		This function serves to EMI_CON_TX0f
 * @param[in]   rf_mode - mode of RF.
 * @param[in]   pwr     - power level of RF.
 * @param[in]   rf_chn  - channel of RF.
 * @return 		none
 */
void emi_con_tx0f(RF_MODE_BT_e rf_mode,unsigned char pwr,signed char rf_chn)
{
	pa_operation(PA_SETTING_STATE_TX);
	bt_emi_tx_continue_mode_run(rf_mode,rf_chn,pwr,BT_PKT_Data_0x0f);
	while( ((read_sram8(RUN_STATUE_ADDR)) == g_run ) &&  ((read_sram8(TEST_COMMAND_ADDR)) == g_cmd_now )\
			&& ((read_sram8(POWER_ADDR)) == g_power_level ) &&  ((read_sram8(CHANNEL_ADDR)) == g_chn )\
			&& ((read_sram8(RF_MODE_ADDR)) == g_mode) \
			&& (read_sram8(SCALE_SETTING_ADDR) == g_scale) && ((read_sram8(SLICE_SETTING_ADDR)) == g_slice)\
			&& ((read_sram8(MANUALE_EN_ADDR)) == g_manual_setting));
	bt_emi_stop();
}
void emi_loop_server(){
	   g_run = read_sram8(RUN_STATUE_ADDR);  // get the run state!
	   if(g_run != 0)
	   {
		   g_power_level = read_sram8(POWER_ADDR);
		   g_chn = read_sram8(CHANNEL_ADDR);
		   g_mode = read_sram8(RF_MODE_ADDR);
		   g_cmd_now = read_sram8(TEST_COMMAND_ADDR);  // get the command!
		   g_tx_cnt = read_sram8(TX_PACKET_MODE_ADDR);
		   g_hop = read_sram8(CD_MODE_HOPPING_CHN);
		   g_scale = read_sram8(SCALE_SETTING_ADDR);
		   g_slice = read_sram8(SLICE_SETTING_ADDR);
		   g_manual_setting = read_sram8(MANUALE_EN_ADDR);
		   for (int i = 0;i < 8;i++)
		   {
				if(g_cmd_now == ate_list[i].cmd_id)
				{
					switch(g_mode)
					{
					  case BR_DH1:
						  ate_list[i].func(BT_MODE_BR_DH1,g_power_level,g_chn);
						  break;
					  case EDR2_DH1:
						  ate_list[i].func(BT_MODE_EDR2_DH1,g_power_level,g_chn);
						  break;
					  case EDR3_DH1:
						  ate_list[i].func(BT_MODE_EDR3_DH1,g_power_level,g_chn);
						  break;
					  case BR_DH3:
						  ate_list[i].func(BT_MODE_BR_DH3,g_power_level,g_chn);
						  break;
					  case EDR2_DH3:
						  ate_list[i].func(BT_MODE_EDR2_DH3,g_power_level,g_chn);
						  break;
					  case EDR3_DH3:
						  ate_list[i].func(BT_MODE_EDR3_DH3,g_power_level,g_chn);
						  break;
					  case BR_DH5:
						  ate_list[i].func(BT_MODE_BR_DH5,g_power_level,g_chn);
						  break;
					  case EDR2_DH5:
						  ate_list[i].func(BT_MODE_EDR2_DH5,g_power_level,g_chn);
						  break;
					  case EDR3_DH5:
						  ate_list[i].func(BT_MODE_EDR3_DH5,g_power_level,g_chn);
						  break;

					  default:break;
					}
				}
		   }
		   g_run = 0;
		   write_sram8(RUN_STATUE_ADDR,g_run);
	   }
}
#if EMI_SUPPORT_SETTING

/**
 * @brief		This function serves to read EMI initial setting.
 * @return 		none
 */

void emi_setting_init(void)
{
	g_emi_setting.general_setting.ptr[0] = read_data8(GENERAL_SETTING_ADDR);
	g_emi_setting.pa_setting_pos = read_data32(PA_SETTING_ADDR);
}

#endif
