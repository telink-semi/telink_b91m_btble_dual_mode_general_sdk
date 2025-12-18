/*
 * reg_stack_adapt.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Admin
 */

#ifndef TLKDRV_B91_REG_INCLUDE_REG_STACK_ADAPT_H_
#define TLKDRV_B91_REG_INCLUDE_REG_STACK_ADAPT_H_

#define reg_zb_mst_mod							REG_ADDR8(0x1401d2)
#define	reg_tx_midx_bt_l						REG_ADDR8(0x140c9c)
#define	reg_tx_midx_bt_h						REG_ADDR8(0x140c9d)


#define	LDOT_DBG2_1								(0x140ee5)
#define	LDOT_DBG2_0								(0x140ee4)
#define	MODEM_TX_CTRL_0							(0x140c80)
#define	MODE_CFG_TX2_0							(0x140e24)
#define	LL_RXSTL_L								(0x140a0c)
#define	LL_TXSTL_L								(0x140a04)
#define	LL_ARD_L								(0x140a10)
#define	LL_MAX_RETRY_CNT						(0x140a14)
#define	RST0									(0x1401e0)
#define	MODEM_TX_SCALE							(0x140c91)
#define	TIM_ALIGN_1								(0x140c79)
#define	LL_TXWAIT_L								(0x140a0e)
#define	MODEM_RX_CTRL_0							(0x140c4c)
#define MODEM_RX_CTRL_1							(0x140c4d)
#define	LL_CTRL2								(0x140a15)
#define	FORMAT									(0x140804)
#define	PREAMBLE_TRAIL							(0x140802)
#define	LL_CMD									(0x140a00)
#define	LL_CTRL3								(0x140a16)
#define	RXMODE									(0x140828)
#define	LL_CTRL0								(0x140a02)
#define	LL_CTRL1								(0x140a03)
#define	LL_RESET_PID							(0x140a01)
#define	SFT_RST									(0x140f3a)
#define	MODE_CFG_RX2_1							(0x140e3f)
#define	TXRX_CB_CAL_CTRL						(0x140e84)
#define	IDLE_RX_SS1_SS2_STRT_CB_0				(0x140e9c)
#define	IDLE_RX_SS5_SS6_STRT_CB_0				(0x140ea0)
#define	IDLE_TXFSK_SS1_SS2_STRT_CB_0			(0x140e96)
#define	IDLE_TXFSK_SS3_SS4_STRT_CB0				(0x140e98)
#define	GUARD_ENABLE_0							(0x140c84)
#define	ALIGN_DELAY								(0x140c85)
#define	CBPF_ADC_1								(0x140f4f)
#define	AGC_THRSHLD1_2M_1						(0x140e55)
#define	RSVD0									(0x1401f4)
#define	EFUSE_ADDR								(0x1401c8)
#define	EFUSE_B0								(0x1401cc)
#define	MODEM_MODE_CFG_RX1_0					(0x140c20)
#define	ACCLEN									(0x140805)
#define	LDOT_RDBK2_1							(0x140eed)
#define	LDOT_RDBK2_0							(0x140eec)
#define	RXTCRCPKT								(0x140830)
#define	BB_PDZB									(0x80140b00)
#define TL_MODEM								(0x80140c00)
#define TL_RADIO								(0x80140e00)

#define BTCORE_BASE_ADDR                        0x80160400
#define	RADIOCNTL00								(0x160070)
#define	RADIOCNTL10								(0x160074)
#define RADIOCNTL20								(0x160078)
#define RADIOCNTL30								(0x16007c)
#define	EDRCNTL0								(0x160428)
#define	TIMGENCNTL0								(0x1604e0)

#define afe1v_reg11								(0x8b)
///////////////used in btc_rf.c////////////////
/* default value 0x00
 * for BLE: 0x00, for BT: 0xff */
#define reg_pdzb_ntl_cv         REG_ADDR8(0x140b00)

/* core_140c20 default value 0xc4, BIT(2) is 1, BIT(4) is 0
 * for BLE: 0xc4, for BT: 0xd0
 * core_140c21 default value 0x00 */
#define reg_modem_mode_rx_mz    REG_ADDR16(0x140c20)

/* default value 0x0026
 * for BLE: 0x0126, for BT: 0x0026 */
#define reg_burst_cfg_txrx_mz	REG_ADDR16(0x80140e28)

/* default value 0x80
 * for BLE: 0x80, for BT: 0x00 */
#define reg_ct_dint_care_mz		REG_ADDR8(0x80140e79)


/////////////used in btc_dbgport.h//////////////
#define reg_bt_dbg_sel      REG_ADDR16(0x140354)

////////////used in tpll.c////////////////
#define REG_DMA_RX_FIFO_WPTR (0x1004f4)
#define REG_DMA_RX_FIFO_RPTR (0x1004f5)

#define ACCESS_CODE_BASE_PIPE0 (0x80140808)
#define ACCESS_CODE_BASE_PIPE2 (0x80140818)

#define REG_LINKLAYER_PTX_RX_TIME (0x140a0a)

#define REG_BASEBAND_RX_PIPE_EN (0x140c4d)
#define REG_BASEBAND_ACCESS_CODE_LEN (0x140805)
#define REG_BASEBAND_ACCESS_CODE_SYNC_THRESHOLD (0x140c4e)
#define REG_LINKLAYER_STATE_MACHINE_STATUS (0x140a24)

#define REG_DMA_TX_FIFO_WPTR_PIPE0 (0x100500)
#define REG_DMA_TX_FIFO_RPTR_PIPE0 (0x100501)
#define reg_rf_ll_ctrl_2 						REG_ADDR8(0x140a15)




#endif /* TLKDRV_B91_REG_INCLUDE_REG_STACK_ADAPT_H_ */
