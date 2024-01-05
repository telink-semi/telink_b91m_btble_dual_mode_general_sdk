/*
 * bt_emi.h
 *
 *  Created on: 2023Äê4ÔÂ4ÈÕ
 *      Author: Admin
 */

#ifndef TLKDRV_B92_EXT_DRIVER_EMI_BT_EMI_H_
#define TLKDRV_B92_EXT_DRIVER_EMI_BT_EMI_H_
/**
 * @brief Generate EMI binary that support setting
 * @note  When compiling sram bin, you need to delete (NOLOAD) after the sbss and bss segments in ram_boot.link.
 *        At the same time, delete the AT instruction content after sdk_version segment.
 */
#define EMI_SUPPORT_SETTING			1
#define MCU_STARTUP_FLASH			1
#if EMI_SUPPORT_SETTING
/**
 * @brief Structure to save general setting.
 */
typedef struct{
	unsigned char ptr[0];
	unsigned char calib_pos:2;
	unsigned char pa_bypass_en:1;
	unsigned char :1;
	unsigned char swire_through_usb_en:1;
	unsigned char power_mode:2;
    unsigned char cap:1;
}general_setting_t, *general_setting_t_ptr;

/**
 * @brief Structure to save emi configuration.
 */
typedef struct{
	unsigned int pa_setting_pos;
	general_setting_t general_setting;
}emi_setting_t, *emi_setting_t_ptr;
#endif


/**
 * @brief Define where the configuration data will be saved in the binary file.
 */
#define VERSION_ADDR				(0x02)
#define PA_SETTING_ADDR				(0x04)
#define GENERAL_SETTING_ADDR		(0x16)
//</remove when release sdk>
/*
 * @brief 	this macro definition serve to open the setting to deal with problem of zigbee mode 2480Mhz
 * 			band edge can't pass the spec.only use it at the time of certification.
 * */
//#define	  FIX_ZIGBEE_BANDEAGE_EN	0
#define TX_PACKET_MODE_ADDR 		     0x00000005
#define RUN_STATUE_ADDR 			     0x00000006
#define TEST_COMMAND_ADDR			     0x00000007
#define POWER_ADDR 					     0x00000008
#define CHANNEL_ADDR				     0x00000009
#define RF_MODE_ADDR				     0x0000000a
#define CD_MODE_HOPPING_CHN			     0x0000000b
#define RSSI_ADDR                        0x00000004
#define RX_PACKET_NUM_ADDR               0x0000000c
//#define PA_SETTING_ADDR				 	 0x00000015 //2bytes

#define SCALE_SETTING_ADDR				 0x00000018
#define SLICE_SETTING_ADDR				 0x00000019
#define MANUALE_EN_ADDR			 		 0x0000001a //1byte
/**
 * @brief   The emi test mode.
 */
typedef enum{
	BR_DH1 = 0,
	BR_DH3 = 1,
	BR_DH5 = 2,
	EDR2_DH1 = 3,
	EDR2_DH3 = 4,
	EDR2_DH5 = 5,
	EDR3_DH1 = 6,
	EDR3_DH3 = 7,
	EDR3_DH5 = 8,
}emi_RF_MODE_BT_e;
typedef enum{
	BT_MODE_BR = 0,
	BT_MODE_EDR2 = 2,
	BT_MODE_EDR3 = 3
}BT_MODE_e;
typedef enum{
	BT_MODE_BR_DH1 		= 	0 | (1 << 2),
	BT_MODE_BR_DH3 		= 	0 | (3 << 2),
	BT_MODE_BR_DH5 		= 	0 | (5 << 2),
	BT_MODE_EDR2_DH1 	= 	2 | (1 << 2),
	BT_MODE_EDR2_DH3 	= 	2 | (3 << 2),
	BT_MODE_EDR2_DH5 	= 	2 | (5 << 2),
	BT_MODE_EDR3_DH1 	= 	3 | (1 << 2),
	BT_MODE_EDR3_DH3 	= 	3 | (3 << 2),
	BT_MODE_EDR3_DH5 	= 	3 | (5 << 2),
}RF_MODE_BT_e;
typedef enum{
	BT_PKT_Data_Prbs9 = 0,
	BT_PKT_Data_0x55 = 1,
	BT_PKT_Data_0x0f = 2,
	BT_PKT_Data_0x00 = 3,
	BT_PKT_Data_0xff
}BT_PKT_Data_type_e;

/**
 * @brief   The emi test command and function
 */
typedef struct {
	unsigned char  cmd_id; 										/**< emi command id */
	void	 (*func)(RF_MODE_BT_e, unsigned char , signed char);   /**< emi function */
}test_list_t;
#define BT_RXCHASS_ADDR				(RD0_ADDR + 0x08)
#define BT_RXDESC_SIZE 				14

#define BT_PWR_LIST_MAX_LENGTH 		30

extern void bt_emi_tx_continue_mode_run(RF_MODE_BT_e mode, signed short chn, unsigned char pwr_index, BT_PKT_Data_type_e type);

extern void bt_emi_tx_burst_mode_init(RF_MODE_BT_e mode, signed short chn, unsigned char pwr_index, BT_PKT_Data_type_e type);

extern void bt_emi_tx_burst_mode_loop(unsigned int *finish_times, unsigned char nslot);

extern void bt_emi_tx_burst_mode_loop_hoop(unsigned int *finish_times, unsigned char nslot);

extern void bt_emi_rx_mode_init(RF_MODE_BT_e mode, signed short chn);

extern void bt_emi_rx_mode_loop(unsigned int *finish_times);

extern void bt_emi_stop();

extern void bt_emi_init();

extern void bt_emi_reset_cv();

extern signed char bt_get_rssi(unsigned char index);

extern void bt_emi_single_tone(signed char rf_chn, unsigned char pwr);
#endif /* TLKDRV_B92_EXT_DRIVER_EMI_BT_EMI_H_ */
