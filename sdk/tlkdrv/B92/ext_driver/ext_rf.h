/********************************************************************************************************
 * @file     ext_rf.h
 *
 * @brief    This is the header file for BTBLE SDK
 *
 * @author	 BTBLE GROUP
 * @date         2,2022
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#ifndef DRIVERS_B91_DRIVER_EXT_EXT_RF_H_
#define DRIVERS_B91_DRIVER_EXT_EXT_RF_H_

#include "compiler.h"
#include "types.h"
#include "lib/include/plic.h"
#include "lib/include/rf.h"


enum{
	//BLE mode
	FLD_RF_BRX_SN_INIT	 		=   BIT(4),
	FLD_RF_BRX_NESN_INIT	 	=   BIT(5),
	FLD_RF_BTX_SN_INIT	 		=   BIT(6),
	FLD_RF_BTX_NESN_INIT	 	=   BIT(7),
};


#define DMA_RFRX_LEN_HW_INFO				0	// 826x: 8
#define DMA_RFRX_OFFSET_HEADER				4	// 826x: 12
#define DMA_RFRX_OFFSET_RFLEN				5   // 826x: 13
#define DMA_RFRX_OFFSET_DATA				6	// 826x: 14

#define DMA_RFRX_OFFSET_CRC24(p)			(p[DMA_RFRX_OFFSET_RFLEN]+6)  //data len:3
#define DMA_RFRX_OFFSET_TIME_STAMP(p)		(p[DMA_RFRX_OFFSET_RFLEN]+9)  //data len:4
#define DMA_RFRX_OFFSET_FREQ_OFFSET(p)		(p[DMA_RFRX_OFFSET_RFLEN]+13) //data len:2
#define DMA_RFRX_OFFSET_RSSI(p)				(p[DMA_RFRX_OFFSET_RFLEN]+15) //data len:1, signed

#define	 FIX_RF_DMA_REWRITE				    0    /*B91 needs to 1*/

#define		RF_BLE_PACKET_LENGTH_OK(p)				( *((unsigned int*)p) == p[5]+13)    			//dma_len must 4 byte aligned
#define		RF_BLE_PACKET_CRC_OK(p)					((p[*((unsigned int*)p) + 3] & 0x01) == 0x0)


#define	RF_BLE_PACKET_VALIDITY_CHECK(p)					(RF_BLE_RF_PAYLOAD_LENGTH_OK(p) && RF_BLE_RF_PACKET_CRC_OK(p))



typedef enum {
	 RF_ACC_CODE_TRIGGER_AUTO   =    BIT(0),	/**< auto trigger */
	 RF_ACC_CODE_TRIGGER_MANU   =    BIT(1),	/**< manual trigger */
} rf_acc_trigger_mode;




extern signed char ble_txPowerLevel;

void ble_rf_set_rx_dma(unsigned char *buff, unsigned char size_div_16);

void ble_rf_set_tx_dma(unsigned char fifo_dep, unsigned char size_div_16);

void ble_tx_dma_config(void);

void ble_rx_dma_config(void);

void rf_drv_ble_init(void);

/**
 * @brief   This function serves to settle adjust for RF Tx.This function for adjust the differ time
 * 			when rx_dly enable.
 * @param   txstl_us - adjust TX settle time.
 * @return  none
 */
static inline void 	rf_tx_settle_adjust(unsigned short txstl_us)
{
	REG_ADDR16(0x80170204) = txstl_us;
}

/**
*	@brief     This function serves to reset RF BaseBand
*	@param[in] none.
*	@return	   none.
*/
static inline void rf_reset_baseband(void)
{
	//todo  The relevant register is not found in the register table
	//REG_ADDR8(0x801404e3) = 0;		//rf_reset_baseband,rf reg need re-setting
	//	REG_ADDR8(0x801404e3) = BIT(0);			//release reset signal
}

/**
 * @brief   This function serves to set RF access code value.
 * @param[in]   ac - the address value.
 * @return  none
 */
static inline void rf_set_ble_access_code_value (unsigned int ac)
{
	write_reg32 (0x80170008, ac);
}

/**
 * @brief   This function serves to set RF access code.
 * @param[in]   p - the address to access.
 * @return  none
 */
static inline void rf_set_ble_access_code (unsigned char *p)
{
	write_reg32 (0x80170008, p[3] | (p[2]<<8) | (p[1]<<16) | (p[0]<<24));
}

/**
 * @brief   This function serves to reset function for RF.
 * @param   none
 * @return  none
 *******************need driver change
 */
static inline void reset_sn_nesn(void)
{
	REG_ADDR8(0x80170201) =  0x01;
}

/**
 * @brief   This function serves to set RF access code advantage.
 * @param   none.
 * @return  none.
 */
static inline void rf_set_ble_access_code_adv (void)
{
	write_reg32 (0x170008, 0xd6be898e);
}


/**
 * @brief   This function serves to trigger accesscode in coded Phy mode.
 * @param   none.
 * @return  none.
 */
static inline void rf_trigle_codedPhy_accesscode(void)
{
	write_reg8(0x170425,read_reg8(0x170425)|0x01);
}

/**
 * @brief     This function performs to enable RF Tx.
 * @param[in] none.
 * @return    none.
 */
static inline void rf_ble_tx_on ()
{
	write_reg8  (0x80170202, 0x45 | BIT(4));	// TX enable
}

/**
 * @brief     This function performs to done RF Tx.
 * @param[in] none.
 * @return    none.
 */
static inline void rf_ble_tx_done ()
{
	write_reg8  (0x80170202, 0x45);
}

/**
 * @brief   This function serves to set RX first timeout value.
 * @param[in]   tm - timeout, unit: uS.
 * @return  none.
 */
static inline void rf_set_1st_rx_timeout(unsigned int tm)
{
	reg_rf_ll_rx_fst_timeout = tm;
}


/**
 * @brief   This function serves to set RX timeout value.
 * @param[in]   tm - timeout, unit: uS, range: 0 ~ 0xfff
 * @return  none.
 */
static inline void rf_ble_set_rx_timeout(u16 tm)
{
	reg_rf_rx_timeout = tm;
}

/**
 * @brief	This function serve to set the length of preamble for BLE packet.
 * @param[in]	len		-The value of preamble length.Set the register bit<0>~bit<4>.
 * @return		none
 */
static inline void rf_ble_set_preamble_len(u8 len)
{
	write_reg8(0x170002,(read_reg8(0x170002)&0xe0)|(len&0x1f));
}

static inline int rf_ble_get_preamble_len(void)
{
	return (read_reg8(0x170002)&0x1f); //[4:0]: ble/N_Europe preamble length
}

static inline void rf_set_dma_tx_addr(unsigned int src_addr)//Todo:need check by sunwei
{
	reg_dma_src_addr(DMA0)=convert_ram_addr_cpu2bus(src_addr);
}

typedef enum{
	FSM_BTX 	= 0x81,
	FSM_BRX 	= 0x82,
	FSM_PTX		= 0x83,
	FSM_PRX     = 0x84,
	FSM_STX 	= 0x85,
	FSM_SRX 	= 0x86,
	FSM_TX2RX	= 0x87,
	FSM_RX2TX	= 0x88,
}fsm_mode_e;


/**
 * @brief     	This function serves to RF trigger RF state machine.
 * @param[in] 	mode  - FSM mode.
 * @param[in] 	tx_addr  - DMA TX buffer, if not TX, must be "NULL"
 * @param[in] 	tick  - FAM Trigger tick.
 * @return	   	none.
 */
void rf_start_fsm(fsm_mode_e mode, void* tx_addr, unsigned int tick);


#define	rf_set_ble_channel	rf_set_ble_chn


/**
 * @brief     This function performs to switch PHY test mode.
 * @param[in] mode - PHY mode
 * @return    none.
 */
//void rf_switchPhyTestMode(rf_mode_e mode);










//TODO: merge into driver
#define	STOP_RF_STATE_MACHINE				( REG_ADDR8(0x80170200) = 0x80 )
enum{
	FLD_RF_SN                     =	BIT(0),
};



/**
 * @brief    This function serves to enable zb_rt interrupt source.
 * @return  none
 */
static inline void zb_rt_irq_enable(void)
{
	plic_interrupt_enable(IRQ15_ZB_RT);
}


#define PRMBL_LENGTH_1M						6	//preamble length for 1M PHY
#define PRMBL_LENGTH_2M						6   //preamble length for 2M PHY
#define PRMBL_LENGTH_Coded					10  //preamble length for Coded PHY, never change this value !!!

#define PRMBL_EXTRA_1M						(PRMBL_LENGTH_1M - 1)	// 1 byte for 1M PHY
#define PRMBL_EXTRA_2M						(PRMBL_LENGTH_2M - 2)	// 2 byte for 2M
#define PRMBL_EXTRA_Coded					0     					// 10byte for Coded, 80uS, no extra byte



#if RF_RX_SHORT_MODE_EN//open rx dly
//TX settle time

	#define 		LL_SCAN_TX_SETTLE								80
	#define 		LL_SCANRSP_TX_SETTLE							78  //63+15=78


	//TODO: need debug
	#define 		LL_TX_STL_TIFS_1M								63
	#define 		LL_TX_STL_TIFS_2M								(LL_TX_STL_TIFS_1M + 24)
	#define 		LL_TX_STL_TIFS_CODED							(LL_TX_STL_TIFS_1M + 40)

	#define 		TX_STL_LEGADV_SCANRSP							75

	#define			LL_TX_STL_ADV_1M								84
	#define			LL_TX_STL_ADV_2M								115
	#define			LL_TX_STL_ADV_CODED								124

	#define 		LL_SLAVE_TX_SETTLE								86
	#define 		LL_SLAVE_TX_STL_2M								117
	#define 		LL_SLAVE_TX_STL_CODED							125

	#define 		LL_MASTER_TX_SETTLE								87
	#define			LL_MASTER_TX_STL_2M								117
	#define			LL_MASTER_TX_STL_CODED							125

#else// close rx dly
//	#error "add code here"
#endif


/* AD convert delay : timing cost on RF analog to digital convert signal process:
 * 					Eagle	1M: 20uS	   2M: 10uS;      500K(S2): 14uS    125K(S8):  14uS
 *					Jaguar	1M: 20uS	   2M: 10uS;      500K(S2): 14uS    125K(S8):  14uS
 */
#define AD_CONVERT_DLY_1M											20
#define AD_CONVERT_DLY_2M											10
#define AD_CONVERT_DLY_CODED										14


static inline void rf_ble_set_1m_phy(void)
{
	write_reg8(0x17063d,0x61);
	write_reg32(0x170620,0x23200a16);
	write_reg8(0x170420,0x8c);// script cc.BIT[3]continue mode.After syncing to the preamble, it will immediately enter
							  //the sync state again, reducing the probability of mis-syncing.modified by zhiwei,confirmed
							  //by qiangkai and xuqiang.20221205
	write_reg8(0x170422,0x00);
	write_reg8(0x17044d,0x01);
	write_reg8(0x17044e,0x1e);
	write_reg16(0x170436,0x0eb7);
	write_reg16(0x170438,0x71c4);
	write_reg8(0x170473,0x01);

	write_reg8(0x17049a,0x00);//tx_tp_align.
	write_reg16(0x1704c2,0x4b3a);
	write_reg32(0x1704c4,0x7a6e6356);
	write_reg8(0x1704c8,0x39);//bit[0:5]grx_fix
#if 1
	write_reg32(0x170000,0x4440081f | PRMBL_LENGTH_1M<<16);
#else
	write_reg32(0x170000,0x4446081f);
#endif

	write_reg16(0x170004,0x04f5);

	write_reg8(0x1704bb,0x50);//BIT[5]:rxc_chf_sel_ble;1M:0(default) 2M:1 open two stage filter to improve
							  //the sensitivity.modified by zhiwei,confirmed by wenfeng and xuqiang,20230106.
}


static inline void rf_ble_set_2m_phy(void)
{
	write_reg8(0x17063d,0x41);
	write_reg32(0x170620,0x26432a06);
	write_reg8(0x170420,0x8c);// script cc.BIT[3]continue mode.After syncing to the preamble, it will immediately enter
							  //the sync state again, reducing the probability of mis-syncing.modified by zhiwei,confirmed
							  //by qiangkai and xuqiang.20221205
	write_reg8(0x170422,0x01);
	write_reg8(0x17044d,0x01);
	write_reg8(0x17044e,0x1e);
	write_reg16(0x170436,0x0eb7);
	write_reg16(0x170438,0x71c4);
	write_reg8(0x170473,0x01);

	write_reg8(0x17049a,0x00);//tx_tp_align.

	write_reg16(0x1704c2,0x4c3b);
	write_reg32(0x1704c4,0x7b726359);
	write_reg8(0x1704c8,0x39);//bit[0:5]grx_fix
	#if 1
		write_reg32(0x170000,0x4440081f | PRMBL_LENGTH_2M<<16);
	#else
		write_reg32(0x170000,0x4446081f);
	#endif

	write_reg16(0x170004,0x04e5);

	write_reg8(0x1704bb,0x70);//BIT[5]:rxc_chf_sel_ble;1M:0(default) 2M:1 open two stage filter to improve
								  //the sensitivity.modified by zhiwei,confirmed by wenfeng and xuqiang,20230106.
}




static inline void rf_ble_set_coded_phy_common(void)
{
	write_reg8(0x17063d,0x61);
	write_reg32(0x170620,0x23200a16);
	write_reg8(0x170420,0xcd);// script cc.BIT[3]continue mode.After syncing to the preamble, it will immediately enter
							  //the sync state again, reducing the probability of mis-syncing.modified by zhiwei,confirmed
							  //by qiangkai and xuqiang.20221205
	write_reg8(0x170422,0x00);
	write_reg8(0x17044d,0x01);
	write_reg8(0x17044e,0xf0);
	write_reg16(0x170438,0x7dc8);
	write_reg8(0x170473,0xa1);

	write_reg8(0x17049a,0x00);//tx_tp_align.

	write_reg16(0x1704c2,0x4b3a);
	write_reg32(0x1704c4,0x7a6e6356);
	write_reg8(0x1704c8,0x39);//bit[0:5]grx_fix
	#if 1
		write_reg32(0x170000,0x4440081f | PRMBL_LENGTH_Coded<<16);
	#else
		write_reg32(0x170000,0x444a081f);
	#endif

	write_reg8(0x1704bb,0x50);//BIT[5]:rxc_chf_sel_ble;1M:0(default) 2M:1 open two stage filter to improve
							  //the sensitivity.modified by zhiwei,confirmed by wenfeng and xuqiang,20230106.
}


static inline void rf_ble_set_coded_phy_s2(void)
{
	write_reg16(0x170436,0x0cee);
	write_reg16(0x170004,0xa4f5);
}


static inline void rf_ble_set_coded_phy_s8(void)
{
	write_reg16(0x170436,0x0cf6);
	write_reg16(0x170004,0xb4f5);
}


#endif /* DRIVERS_B91_DRIVER_EXT_EXT_RF_H_ */
