/********************************************************************************************************
 * @file	btble_scheduler.h
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
#ifndef _BT_LL_SCHEDULER_H_
#define _BT_LL_SCHEDULER_H_

#include "tlkstk/inner/tlkstk_inner.h"



#ifndef	MAX_BLE_LINK
#define	MAX_BLE_LINK				3
#endif

enum {

	ID_LINK_ACCESS					= 	MAX_BT_ACL_LINK,
	ID_LINK_BLE						=	(MAX_BT_ACL_LINK + 1),				ID_LINK_BLE_ADV	= ID_LINK_BLE,
	ID_LINK_BLE_CONN				=	(MAX_BT_ACL_LINK + 2),
	MAX_SCHEDULE_LINK				=	(ID_LINK_BLE + MAX_BLE_LINK),

	FLAG_SCHEDULE_LINK_ID_NULL		=	0x80,
	FLAG_SCHEDULE_LINK_ID_CHANGE	=	0x40,
	SCHEDULER_LATENCY_DEFAULT		=	2500,		// 1250 + 625 + 25

	FLAG_SCHEDULE_START				=	0,
	FLAG_SCHEDULE_DONE				=	0x80,
	FLAG_SCHEDULE_CHANGE			=	0x81,
	FLAG_SCHEDULE_EXPIRE			=	0xC0,  //add by SiHui, now only used for standard BLE
	FLAG_SCHEDULE_RF_END			=	0xE0,  //add by SiHui, now only used for standard BLE

	FLAG_SCHEDULE_TASK_DISABLE		=	1,

	SCHEDULER_TIME_FLEXIBLE			=	0x00,
	SCHEDULER_TIME_FIX				=	0x01,

	SCHEDULER_INTERVAL_7MS5_0		=	0,
	SCHEDULER_INTERVAL_7MS5_1		=	1,
	SCHEDULER_INTERVAL_15MS_0		=	2,
	SCHEDULER_INTERVAL_15MS_1		=	3,
	//sniff is currently executed frequently, resulting in BLE create & update failures. After synchronizing with "Qipeng", lower the priority. if any question, contact ronglu 20230421
	SCHEDULER_PRIORITY_SNIFF		=	0x09,
	SCHEDULER_PRIORITY_PAGETOACL	=	0x10,
	//if the role switch priority lower sniff, it will cause skip irq.(schedule preemption)
	SCHEDULER_PRIORITY_ROLESWITCH	=	0x08,
	SCHEDULER_PRIORITY_SCO			=	0x20,
	
	SCHEDULER_PRIORITY_BLE			=	0x30,

	SCHEDULER_PRIORITY_BLE_SYNC		=	0x08,  //when BLE connection create & update, need high priority
	SCHEDULER_PRIORITY_BLE_EXTEND	=	0x48,  //when BLE connection extend timing, priority between BT ACL normal and BT ACL voice task
	SCHEDULER_PRIORITY_ADV			=	0x40,

	SCHEDULER_PRIORITY_INQUIRY		=	0x50,
	SCHEDULER_PRIORITY_PAGE			=	0x50,
	SCHEDULER_PRIORITY_INQUIRYSCAN	=	0x50,
	SCHEDULER_PRIORITY_PAGESCAN		=	0x50,
	SCHEDULER_PRIORITY_ACL			=	0x50,
	SCHEDULER_PRIORITY_ACL_URGENT   =	0x40,  //add by ZhangJian for SiHui's request: ACL with voice task set by BT application
	SCHEDULER_PRIORITY_SCAN_LOW		=	0x60,

	SCHEDULER_PRIORITY_LOWEST		=	0xff,

	SCHEDULER_MODE_STANDARD			=	0x00,
	SCHEDULER_MODE_TWS				=	0x01,

	SCHEDULER_LOWPOWER_IDLE				= 	0x0,
	SCHEDULER_LOWPOWER_ENABLE			= 	0x80,
	SCHEDULER_LOWPOWER_ENABLE_BT		= 	0x40,
	SCHEDULER_LOWPOWER_ENABLE_BLE		= 	0x20,
	SCHEDULER_LOWPOWER_BUSY				= 	0x01,
	SCHEDULER_LOWPOWER_EXIT				= 	0x04,
	SCHEDULER_LOWPOWER_MASK				= 	0x70,
};

/* For example, ID_LINK_BLE is 4, MAX_SCHEDULE_LINK is 7, ble_task_mask should be BIT(4~6)
 * (2<<7) - 1: BIT(0~6)
 * (2<<4) - 1: BIT(0~3)
 * BIT(4~6) = BIT(0~6) -  BIT(0~3) = (2<<7) - (2<<4)
 */
#define SCHEDULER_TASK_BLE_MASK		((2<<MAX_SCHEDULE_LINK) - (2<<ID_LINK_BLE))

typedef	uint32_t (*func_system_tick_isr_t) (uint8_t sequence, uint8_t link);
extern func_system_tick_isr_t	sys_tick_isr_ble_access_cb;

typedef struct link_scheduler_item
{
	uint8_t			en;
	uint8_t			skip;
	uint8_t			priority;
	uint8_t			type;

	uint8_t			slot12_end;
	uint8_t			duration;
	uint8_t			interval;
	uint8_t			win1m25;

	uint32_t			tick;
	uint32_t			latency;
	uint8_t			bandwidth_adjust;
	func_system_tick_isr_t func;
} link_scheduler_t;

typedef struct env_scheduler
{
//0x00
	uint8_t			cur_id;
	uint8_t			cur_priority;
	uint8_t			next_id;
	uint8_t			last_id;			//id of last flexible link

	uint8_t			sequence;
	uint8_t			keep;
	uint16_t		cur_latency;

	uint32_t		cur_tick;
	uint32_t		next_tick;

//0x10
	uint8_t			next_duration;
	uint8_t			cur_duration;
	uint8_t			inq_page_priority;
	uint8_t			access;

	uint8_t			scan_priority;
	uint8_t			last_link;
	uint8_t			slot12_offset;
	uint8_t			lp_bt_last;

	uint8_t			max_bt;
	uint8_t			nbt;				//nbt < max_bt : bt access enable
	uint8_t			max_ble;
	uint8_t			nble;				//nble < max_ble: ble access enable

	uint8_t			nflex;				// number of flexible link
	uint8_t			mode;
	uint8_t			bt_access;			// number of access: switch between BT/BLE
	uint8_t			hslot24;

//0x20
	uint8_t			lp_bt;
	uint8_t			lp_ble;
	uint8_t			lp_en;
	uint8_t			lp_link;


	uint32_t		lp_tick;
	uint32_t		lp_sniff_interval;
	uint32_t		hslot;

//0x30
	uint32_t		fno24;
	uint32_t		no;
	uint32_t		irq0;
	uint32_t		irq1;

//0x40
	uint32_t    	 	deepRet_earlyWakeupTick;

	link_scheduler_t	link_scheduler[MAX_SCHEDULE_LINK];

	uint8_t         max_bt_bkup;
	uint8_t         sch_sel_go_on;
	uint8_t         sco_link_pending;
	uint8_t			standard_ble;

	uint32_t        next_sco_tick;
	uint32_t		system_irq_tick;
} env_scheduler_t ;


extern env_scheduler_t	env_sch;
extern uint8_t bt_ll_bandwidth_req;

static inline void bt_ll_schedule_disable (uint8_t id) {if (id < MAX_SCHEDULE_LINK) {env_sch.link_scheduler[id].en = 0;}}

void bt_ll_schedule_keep (uint8_t en);


static inline int	bt_ll_schedule_end_pending ()
{return (env_sch.mode) && (!(FLAG_SCHEDULE_LINK_ID_NULL & env_sch.next_id) || (FLAG_SCHEDULE_LINK_ID_NULL & env_sch.cur_id));}

static inline void bt_ll_schedule_set_slot12_pre_end(uint8_t id, uint8_t slot) {if (id<MAX_SCHEDULE_LINK) env_sch.link_scheduler[id].slot12_end = slot;}

static inline uint8_t bt_ll_schedule_slot12_pre_end (uint8_t id, uint32_t slot, uint8_t sco)
{
	slot += env_sch.slot12_offset;
	slot &= (BIT(27) - 1);
	slot %= 12;
	uint8_t ref = env_sch.link_scheduler[id].slot12_end + (sco ? 2 : 0);
	return slot > ref && slot < 10 ? slot : 0;
}
static inline uint16_t bt_ll_schedule_cur_next_id_get (void)
{
	return env_sch.cur_id | (env_sch.next_id << 8);
}
static inline int bt_ll_schedule_get_ble_acl (uint8_t sch_link) {return sch_link >= ID_LINK_BLE && sch_link < MAX_SCHEDULE_LINK ? sch_link - ID_LINK_BLE : FLAG_SCHEDULE_LINK_ID_NULL;}

static inline int bt_ll_schedule_get_bt_acl (uint8_t sch_link) {return sch_link < ID_LINK_ACCESS ? sch_link : FLAG_SCHEDULE_LINK_ID_NULL;}

static inline uint8_t bt_ll_schedule_get_ble_link (uint8_t acl) {return acl + ID_LINK_BLE;}

void	bt_ll_schedule_set_mode (uint8_t m, uint8_t max_bt, uint8_t max_ble);

void	bt_ll_schedule_set_ble_callback (void *p);

void	bt_ll_schedule_set_bt_callback (void *p);

void	bt_ll_schedule_set_enable (uint8_t id, uint8_t priority, uint8_t type, uint32_t tick, uint32_t win1m25, uint32_t latency);

void 	bt_ll_schedule_set_duration_interval (uint8_t id, uint8_t duration, uint8_t interval);

void 	bt_ll_schedule_acl_bandwidth_policy_enter (uint16_t con_handle);

void 	bt_ll_schedule_acl_bandwidth_policy_exit (void);

void 	bt_ll_schedule_set_inquiry_page_mode (uint8_t priority, uint8_t priority_scan, uint8_t duration, uint8_t interval);

uint8_t	bt_ll_schedule_set_current_priority (uint8_t id, uint8_t priority);

int		bt_ll_schedule_next (uint32_t slot);

int		bt_ll_schedule_end (uint8_t link, uint8_t disable);

int		bt_ll_schedule_select (uint8_t id, uint32_t interval);

int		bt_ll_schedule_link_idle (uint8_t link, uint8_t local_idle, uint8_t peer_idle);

uint32_t bt_ll_schedule_get_next_available_tick (uint32_t next_tick);

uint32_t bt_ll_schedule_get_next_7ms5_tick (uint32_t * out_nframe);

uint8_t	bt_ll_schedule_get_slot12_offset (void);

int		bt_ll_schedule_update_hslot24 ();

void	bt_ll_schedule_access_scan_pause (void);
void	bt_ll_schedule_access_scan_restore (void);


void   bt_ll_schedule_set_priority (uint8_t id, uint8_t priority);
uint32_t    bt_ll_set_next_irq(uint32_t tick);

extern void btble_schedule_init(void);

#endif // _BT_LL_SCHEDULER_H_
