/********************************************************************************************************
 * @file     ext_btrf.h
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
#ifndef EXT_BT_RF_H
#define EXT_BT_RF_H


/**********************************************************************************************************************
 * RF FLAG BEGIN:
 *********************************************************************************************************************/
#define EXT_BTRF_FLG_ALL_MSK                     0xFFFFFFFFUL
#define EXT_BTRF_FLG_IQ_ID_FASTSETTLE_EN         (0x01UL << 0)   // IQ fast settle en.
#define EXT_BTRF_FLG_IQ_NONID_FASTSETTLE_EN      (0x01UL << 1)
#define EXT_BTRF_FLG_TP_ID_FASTSETTLE_EN         (0x01UL << 2)   // TP fast settle en.
#define EXT_BTRF_FLG_TP_NONID_FASTSETTLE_EN      (0x01UL << 3)
#define EXT_BTRF_FLG_IQ_TP_SWITCH_EN             (0x01UL << 4)
#define EXT_BTRF_FLG_IQ_ONLY_EN                  (0x01UL << 5)
#define EXT_BTRF_FLG_IQ_PWR_MANUAL_EN            (0x01UL << 6)
#define EXT_BTRF_FLG_PA_MANUAL_EN                (0x01UL << 7)
#define EXT_BTRF_FLG_PA_AUTO_EN                  (0x01UL << 8)
#define EXT_BTRF_FLG_TXPWR_ADJUST_EN             (0x01UL << 9)
#define EXT_BTRF_FLG_TXPWR_ADJUST_APP_EN         (0x01UL << 10)

#define EXT_BTRF_TXPWR_LEVEL_MAX                 12
#define EXT_BTRF_TXPWR_LEVEL_MIN                 0
#define EXT_BTRF_TXPWR_LEVEL_APP_MAX             6

#define EXT_BTRF_FLAG_CT_FE_CTRIM_SEL               BIT(7)
#define EXT_BTRF_FLAG_BURST_CFG_CHANNEL_NUM_LL_SEL  BIT(8)
#define	EXT_BTRF_FLAG_RF_MODEM_BLE                  BIT(2)
#define	EXT_BTRF_FLAG_RF_CONT_MODE                  BIT(3)
#define	EXT_BTRF_FLAG_MODEM_NTL_CV                  BIT(4)

#define PA_TX_RX_SWITCH_US          10
#define PA_TX_RX_SWITCH_US          10
/**********************************************************************************************************************
 * RF AUTO MODE IN PWR ADJUST SETTING BEGIN:
 *********************************************************************************************************************/

#ifndef	BT_AUTO_PWR_DFT
#define BT_AUTO_PWR_DFT    0x28        // slice
#endif


/**********************************************************************************************************************
 * RF PATH/DELAY/POWERUP/POWERDOWN SETTING BEGIN:
 *********************************************************************************************************************/

#define	BT_RADIO_RX_EIR_ADJ_EN		1// for inquiry eir

#define	BT_RADIO_SYNC_OFFSET		0

#define	BT_RADIO_RX_PATH_DELAY		14
#define	BT_RADIO_IQ_TX_PATH_DELAY	6
#define	BT_RADIO_TP_TX_PATH_DELAY	6

#define	BT_RADIO_RX_POWER_UP		(100 - BT_RADIO_RX_PATH_DELAY)		//CEVA bug

#define BT_RADIO_TP_TX_POWER_UP     110 //>108
#define	BT_RADIO_IQ_TX_POWER_UP		100
#define	BT_RADIO_IQ_POWER_DOWN		4
#define	BT_RADIO_TP_POWER_DOWN		8

/*  IQ modulation : VABT mode
 *
 * */
typedef enum {
	EXT_BTRF_IQ_POWER_INDEX_N10P0_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N9P01_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N8P00_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N7P04_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N6P03_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N5P00_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N4P09_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N3P09_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N2P08_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N1P05_dBm,
	EXT_BTRF_IQ_POWER_INDEX_N0P01_dBm,
	EXT_BTRF_IQ_POWER_INDEX_P1P00_dBm,
	EXT_BTRF_IQ_POWER_INDEX_P1P50_dBm,
	EXT_BTRF_IQ_POWER_INDEX_MAX,
}EXT_BTRF_IQ_POWER_INDEX_ENUM;

/*  TP modulation : VABT mode -> VANT mode
 *
 * */
typedef enum{
	EXT_BTRF_TP_POWER_INDEX_N8p78dBm,
	EXT_BTRF_TP_POWER_INDEX_N6p54dBm,
	EXT_BTRF_TP_POWER_INDEX_N4p77dBm,
	EXT_BTRF_TP_POWER_INDEX_N3p37dBm,
	EXT_BTRF_TP_POWER_INDEX_N2p01dBm,
	EXT_BTRF_TP_POWER_INDEX_N1p37dBm,
	EXT_BTRF_TP_POWER_INDEX_N0p53dBm,
	EXT_BTRF_TP_POWER_INDEX_P0p01dBm,
	EXT_BTRF_TP_POWER_INDEX_P0p80dBm,
	EXT_BTRF_TP_POWER_INDEX_P1p72dBm,
	EXT_BTRF_TP_POWER_INDEX_P2p32dBm,
	EXT_BTRF_TP_POWER_INDEX_P2p79dBm,
	EXT_BTRF_TP_POWER_INDEX_MAX,
}EXT_BTRF_TP_POWER_INDEX_ENUM;


typedef enum{
    EXT_BTRF_RADIO_TX_MODE_BLE = 1,
	EXT_BTRF_RADIO_TX_MODE_IQ  = 2,
	EXT_BTRF_RADIO_TX_MODE_TP  =3
}EXT_BTRF_RADIO_TX_MODE_ENUM;


/**********************************************************************************************************************
 * RF RADIO/MODEM SETTING BEGIN:
 *********************************************************************************************************************/

/***********************************
* add power ramp up time  ||from rf.c
*  0x140e24[1:0]
*  b01: 250ns/step, 8*250ns = 2us
*  b10: 500ns/step, 8*500ns = 4us
*  b11: 1us/step , 8*1us = 8us
***********************************/
static inline void rf_bt_add_power_ramp_up_time(u8 val)
{
	write_reg8(MODE_CFG_TX2_0,((read_reg8(MODE_CFG_TX2_0) & 0xfc) | (val & 0x03)) );
}

static inline void rf_init_ntl_cv(void)
{
    reg_pdzb_cv = 0xffffffff;
}

static inline void rf_set_em_base_address(uint32_t addr )
{
    reg_pdzb_em_base_address = addr;
}


static inline void rf_set_modem_mode(uint8_t mode)
{
    reg_modem_mode_rx &= (uint8_t)(~FLD_MODEM_BLE_BT_SEL);
    reg_modem_mode_rx |= (mode<<2);
}

static inline void rf_set_modem_ntl_cv_en(void)
{
    reg_modem_mode_rx|=FLD_MODEM_NTL_CV;
}

static inline void rf_set_freq_chn_auto(void)
{
    reg_burst_cfg_txrx &= ~FLD_BURST_CFG_CHANNEL_NUM_LL_SEL;
    reg_ct_dint_care   &= ~FLD_CT_FE_CTRIM_SEL;
    reg_chnl_freq_direct &= ~FLD_TXRX_DBG3_0_CHNL_FREQ_DIRECT;
}

static inline void rf_set_power_auto(void)
{
    // set power by power index in BR
    reg_mode_cfg_tx3 |= FLD_MODE_CFG_TX3_TXC_PWR_SRL;
}

static inline void rf_set_auto_mode(int v)
{
    if(v){
        reg_modem_mode_rx |= FLD_MODEM_BLE_BT_AUTO_SEL_EN;
    }
    else{
        reg_modem_mode_rx &= ~FLD_MODEM_BLE_BT_AUTO_SEL_EN;
    }
}



typedef struct{
	unsigned char slice;
	unsigned char scale;
} __attribute__ ((__packed__)) ext_btrf_expwr_t;


void ext_btrf_init(void);

bool ext_btrf_txpwr_inc(uint8_t link_id);
bool ext_btrf_txpwr_dec(uint8_t link_id);
void ext_btrf_txpwr_max(uint8_t link_id);


void ext_btrf_setTxIqPower(EXT_BTRF_IQ_POWER_INDEX_ENUM index);
void ext_btrf_setTxTpPower(EXT_BTRF_TP_POWER_INDEX_ENUM index);

void ext_btrf_setFlag(unsigned int flag, unsigned int en);
bool ext_btrf_flagIsSet(u32 flag);


void ext_btrf_setBtTxTpSlice(u8 slice);
void ext_btrf_setLeTxTpSlice(u8 slice);
void ext_btrf_setBtTxIqScaleSlice(u8 scale, u8 slice);


void ext_btrf_changeRadioMode(EXT_BTRF_RADIO_TX_MODE_ENUM mode);

void ext_btrf_rxCalibtration(void);
void ext_btrf_setBtTailDelay(int v);

void ext_btrf_brManualInit(void);



#endif //EXT_BT_RF_H

