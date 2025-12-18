/********************************************************************************************************
 * @file	tlkstk_logdef.h
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
#ifndef	TLKSTK_LOG_DEF_H
#define TLKSTK_LOG_DEF_H


//Note: TLK_VCD_xxx_START is very important for identifying xxx's type ids.

//	event: 0 for time stamp; 1 reserved; eid2 - eid31
#define TLK_VCD_EVENT_START
#define SLEV_timestamp					0
#define SLEV_reserved					1
#define SLEV_RX_SYNC					2
#define SLEV_lmp_rx   					3
#define SLEV_lmp_tx						4
#define SLEV_acl_rx   					5
#define SLEV_acl_tx						6
#define SLEV_acl_rxbit					7

#define SLEV_acl_newconnect_timeout		8
#define SLEV_page_1st_id_error			9
#define SLEV_acl_link_sup_to			10
#define SLEV_acl_start	 			    11
#define SLEV_acl_skip	 				12
#define SLEV_acl_done	 				13
#define SLEV_acl_continue 				14
#define SLEV_ble_acl_expire 			15

#define SLEV_bt_core_reset			    16
#define SLEV_pagescan_retry             17
#define SLEV_pagescan_timeout           18

#define SLEV_sch_sel_pick   			19
#define SLEV_sniff_flush_acc   			20
#define SLEV_h2c_acl_data_pending		21
#define SLEV_acl_expire					22
#define SLEV_acl_duration_done			23
#define SLEV_acl_lmp_full    			24
// event with tick: 0 - 31
#define TLK_VCD_TICK_START
#define SLET_timestamp					0
#define SLET_RX_SYNC					1
#define SLET_acl_reset					2
#define SLET_acl_rx_chg_fcnt	   		3
#define SLET_rxdesc_fresh				4
#define SLET_btc_hw_prog				5
#define SLET_bt_rsw						6
#define SLET_bt_rsw_tdd_timeout			7

#define SLET_bt_rsw_piconet_timeout		8
#define SLET_bt_irq_skip				9
#define SLET_bt_irq_error				10
#define SLET_bt_acl_isr					11
#define SLET_bt_seqn_err		        12
#define SLET_btc_hw_prog_skip		    13
#define SLET_bt_rxirq_rxdone		    14
#define SLET_bt_txirq_txdone		    15

#define SLET_fresh_tx_pcnt	            16
#define SLET_fresh_rx_pcnt	            17
#define SLET_fresh_tx_pcnt_log	        18
#define SLET_fresh_rx_pcnt_log	        19
#define SLET_esco_irq_rcvd				20
#define SLET_esco_speaker_pkt_err		21
#define SLET_esco_set_et		        22
#define SLET_esco_slot_done	            23

#define SLET_bt_hw_seqn_err			    25
#define SLET_bt_sync_err			    26
#define SLET_instant_pass		        27
#define SLET_bt_sniff_prog              28
#define SLET_sniff_chg_fcnt             29
#define SLET_bt_crc_err					30
#define SLET_aes_rxstat_micerr   		31


// 1-bit data: 0/1 for hardware signal: PA4/PB1; bid2 - bid31
#define TLK_VCD_LEVEL_START
#define SL01_TX_EN                  	0
#define SL01_RX_EN                 	 	1
#define SL01_ac_inq						2
#define SL01_ac_inq_scan				3
#define SL01_ac_page					4
#define SL01_ac_page_scan				5
#define SL01_bt_frame_end  		   	 	6
#define SL01_task_system_tick   		7

#define SL01_task_system_access			8
#define SL01_bt_detach_timeout			9
#define SL01_schedule_add				10
#define SL01_schedule_sel				11
#define SL01_schedule_keep				12
#define SL01_schedule_end				13
#define SL01_task_pa_rx					14
#define SL01_task_pa_tx					15

#define SL01_task_ble_stimer   			16
#define SL01_task_ble_brx				17
#define SL01_reserved_1				    18
#define SL01_reserved_2  				19
#define SL01_reserved_3	                20
#define SL01_reserved_4				    21
#define SL01_reserved_5				    22
#define SL01_reserved_6				    23

#define SL01_reserved_7				    24
#define SL01_reserved_8				    25
#define SL01_reserved_9				    26
#define SL01_reserved_10			    27
#define SL01_reserved_11				28
#define SL01_reserved_12			    29
#define SL01_reserved_13         		30
#define SL01_btble_pm_state         	31

// 8-bit data: cid0 - cid63
#define TLK_VCD_BYTE_START
#define SL08_bt_inq_rx_state		    0
#define SL08_bt_page_train			    1
#define SL08_bt_reset_state				2
#define SL08_bt_detach_state			3
#define SL08_bt_link_dis				4
#define SL08_bt_acl_link_id				5
#define SL08_bt_afh_state               6
#define SL08_bt_afh_cls_state           7

#define SL08_bt_acl_rx_nb				8
#define SL08_bt_rsw_state				9
#define SL08_bt_pair_state				10
#define SL08_bt_authen_state			11
#define SL08_bt_encrypt_state			12
#define SL08_bt_test_mode_state			13
#define SL08_bt_sniff_state				14
#define SL08_bt_esco_link_state		    15

#define SL08_bt_esco_frm_no             16
#define SL08_bt_esco_tog                17
#define SL08_bt_esco_rx_err             18
#define SL08_tx_path_delay				19
#define SL08_bt_acl_flow				20
#define SL08_schedule_cur_priority		21
#define SL08_schedule_seq				22
#define SL08_schedule_last_id			23

#define SL08_sch_update_next_id         24
#define SL08_sch_change_next_id         25
#define SL08_bt_rxchannel               26
#define SL08_btc_authen_step     	    27
#define SL08_btc_enc_aes_puse_stat      28
#define SL08_reserved_state_1           29
#define SL08_cur_rxdesc_hw_index           30
#define SL08_cur_rxdesc_sw_index           31






// 16-bit data: sid0 - sid63
#define TLK_VCD_WORD_START
#define SL16_bt_CLKN_HS					0
#define SL16_bt_FCNT					1
#define SL16_bt_RXBIT					2
#define SL16_bt_RXCLKN					3
#define SL16_bt_RXBIT_offset			4

#define SL16_bt_RxStat					5
#define SL16_bt_RxHeader				6
#define SL16_bt_rx_l2caplen				7
#define SL16_schedule_dbg				8
#define SL16_schedule_cur_latency		9

#define SL16_page_latency				10
#define SL16_page_end_ts				11
#define SL16_pagescan_MCLK				12
#define SL16_pagescan_MCLKP				13
#define SL16_bt_inq_latency				14
#define SL16_bt_inq_end_ts				15
#define SL16_bt_rsw_instant				16
#define SL16_bt_rawstep					17
#define SL16_freq         				18

#define SL16_esco_slot					19
#define SL16_esco_st					20
#define SL16_bt_sniff_sub_instant		21
#define SL16_bt_sniff_anchor_point		22

#define SL16_reserved_state16_1         26
#define SL16_reserved_state16_2         27
#define SL16_reserved_state16_3         28
#define SL16_reserved_state16_4         29
#define SL16_reserved_state16_5         30
#define SL16_lp_ctlr_state_busy         31


#define SL16_ec_rptr					31
#define SL16_bt_rsw_slotoffset			33
#define SL16_bt_sniff_hslot             34
#define SL16_bt_sniff_prefetch          35
#define SL16_bt_et_start	            36
#define SL16_acl_rx_stat                38
#define SL16_acl_tx_stat                39
#define SL16_lmp_tx_opcode	            40
#define SL16_lmp_rx_code				41
#define SL16_hci_opcode                 42

#define SL16_et_rxwinsz                 43
#define SL16_sniff_half_rxwnd_slot      44



#define SL16_sec_rx_pcnt	            47
#define SL16_sec_tx_pcnt	            48
#define SL16_bt_last_enc_aes_rx_pcnt0   49
#define SL16_bt_last_enc_aes_tx_pcnt0   50



#endif //TLKSTK_LOG_DEF_H
