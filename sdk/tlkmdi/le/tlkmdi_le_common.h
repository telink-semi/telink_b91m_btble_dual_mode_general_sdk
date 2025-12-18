/********************************************************************************************************
 * @file	tlkmdi_le_common.h
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
#ifndef BLT_COMMON_H_
#define BLT_COMMON_H_


#include "app/app_config.h"
#include "tlk_config.h"
#include "drivers.h"





/** Calibration Information FLash Address Offset of  CFG_ADR_CALIBRATION_xx_FLASH ***/
#define		CALIB_OFFSET_CAP_INFO								0x00
#define		CALIB_OFFSET_TP_INFO								0x40

#define		CALIB_OFFSET_ADC_VREF								0xC0

#define		CALIB_OFFSET_FIRMWARE_SIGNKEY						0x180




extern	const u8 	telink_adv_trigger_pairing[6];
extern	const u8 	telink_adv_trigger_unpair[6];



extern u32 flash_sector_mac_address;
extern u32 flash_sector_calibration;

extern const u8 vendor_OtaUUID[];
extern unsigned int tlkcfg_getFlashAddr(unsigned int offset);

/**
 * @brief		This function is used to enable the external crystal capacitor
 * @param[in]	en - enable the external crystal capacitor
 * @return      none
 */
static inline void blc_app_setExternalCrystalCapEnable(u8  en)
{
//	blt_miscParam.ext_cap_en = en;
	analog_write(0x8a, analog_read(0x8a) | 0x80);//disable internal cap
}


/**
 * @brief		This function is used to load customized parameters from flash sector for application
 * @param[in]	none
 * @return      none
 */
static inline void blc_app_loadCustomizedParameters(void)
{
//	 if(!blt_miscParam.ext_cap_en)
	 {
		 //customize freq_offset adjust cap value, if not customized, default ana_8A is 0x60
		 //for 1M  Flash, flash_sector_calibration equals to 0xFE000
		 if(flash_sector_calibration){
			 u8 cap_frqoft;
			 unsigned int caliAddr;
		 	 caliAddr = tlkcfg_getFlashAddr(flash_sector_calibration);
			 if(caliAddr == 0) return;
			 flash_read_page(caliAddr + CALIB_OFFSET_CAP_INFO, 1, &cap_frqoft);
			 if( cap_frqoft != 0xff ){
				 analog_write_reg8(0x8A, (analog_read_reg8(0x8A) & 0xc0)|(cap_frqoft & 0x3f));
			 }
		 }
	 }
}


/**
 * @brief		This function is used to initialize the MAC address
 * @param[in]	flash_addr - flash address for MAC address
 * @param[in]	mac_public - public address
 * @param[in]	mac_random_static - random static MAC address
 * @return      none
 */
void blc_initMacAddress(int flash_addr, u8 *mac_public, u8 *mac_random_static);




#endif /* BLT_COMMON_H_ */
