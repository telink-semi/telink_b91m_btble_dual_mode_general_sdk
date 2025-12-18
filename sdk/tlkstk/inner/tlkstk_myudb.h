/********************************************************************************************************
 * @file	tlkstk_myudb.h
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
#ifndef TLKSTK_MYUDB_H
#define TLKSTK_MYUDB_H


#include "tlk_config.h"
#include "tlk_debug.h"
#include "tlkstk_logdef.h"


extern void tlkdbg_sendData(uint32 flags, char *pStr, uint08 *pData, uint16 dataLen);
extern void tlkdbg_sendU32s(uint32 flags, void *pStr, uint32 val0, uint32 val1, uint32 val2, uint32 val3);

extern void tlkdbg_vcd_sync(void);
extern void tlkdbg_vcd_tick(uint32 flags, uint08 id);
extern void tlkdbg_vcd_level(uint32 flags, uint08 id, uint08 level);
extern void tlkdbg_vcd_event(uint32 flags, uint08 id);
extern void tlkdbg_vcd_byte(uint32 flags, uint08 id, uint08 value);
extern void tlkdbg_vcd_word(uint32 flags, uint08 id, uint16 value);


#define my_dump_str_data(flags,s,p,n)           tlkdbg_sendData(flags,s,(uint08*)(p),n)
#define my_dump_str_u32s(flags,s,d0,d1,d2,d3)   tlkdbg_sendU32s(flags,s,(uint32)(d0),(uint32)(d1),(uint32)(d2),(uint32)(d3))




#ifndef			VCD_BLE_EN
#define			VCD_BLE_EN					(0)
#endif



#define SL_STACK_VCD_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
/* Timing VCD id enable */
#define SL_STACK_TIMING_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_RESET_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_RESET << 16))
/* schedule VCD id enable */
#define SL_STACK_SCH01_EN			((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_SCH << 16))
#define SL_STACK_SCH02_EN			((TLK_MAJOR_DBGID_SYS << 24) | (TLK_MINOR_DBGID_SYS_SCH << 16))
/* frame VCD id enable */
#define SL_STACK_FRAME_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_FRAME_ST_EN		((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_RADIO_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_FLOW_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
/* access link VCD id enable */
#define SL_STACK_INQ_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_INQSCAN_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_PAGE_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_PAGE << 16))
#define SL_STACK_PAGESCAN_EN		((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_PAGE << 16))

/* acl-c link VCD id enable */
#define SL_STACK_ACL_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_ACL << 16))
#define SL_STACK_LMP_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_LMP << 16))
#define SL_STACK_CON_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_ACL << 16))
#define SL_STACK_RSW_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_RSW << 16))
#define SL_STACK_SNIFF_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_SNIFF << 16))
#define SL_STACK_AFH_EN             ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_AFH << 16))
#define SL_STACK_AFH_CLS_EN         ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_CHN << 16))
#define SL_STACK_DETACH_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_DETACH << 16))

/* sco/e-sco link VCD id enable */
#define SL_STACK_SCO_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_SCO << 16))
/* authen/pair/encrypt VCD id enable */
#define SL_STACK_AUTH_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_AUTH << 16))
#define SL_STACK_PAIR_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_PAIR << 16))
#define SL_STACK_ENCRYPT_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_ENC << 16))
/* PA VCD id enable */
#define SL_STACK_PA_EN				((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_TEST_EN			((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC << 16))
#define SL_STACK_HCI_EN			    ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_HCI << 16))



//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
#define	log_tick(en,id)	tlkdbg_vcd_tick(en,id)
//1-byte (000_id-5bits)
#define	log_event(en,id) tlkdbg_vcd_event(en,id)
//1-byte (01x_id-5bits) 1-bit data: id0 & id1 reserved for hardware
#define	log_task(en,id,b)	tlkdbg_vcd_level(en,id,b)
//2-byte (10-id-6bits) 8-bit data
#define	log_b8(en,id,d)	tlkdbg_vcd_byte(en,id,d)
//3-byte (11-id-6bits) 16-bit data
#define	log_b16(en,id,d) tlkdbg_vcd_word(en,id,d)


//BLE used only //
#define	log_sync_mainloop(en)	tlkdbg_vcd_sync()
#define	log_tick_mainloop(en,id) tlkdbg_vcd_tick(en,id)
#define	log_tick_irq(en,id)		tlkdbg_vcd_tick(en,id)
#define	log_event_irq(en,id) 	tlkdbg_vcd_event(en,id)
#define	log_task_irq(en,id,b)	tlkdbg_vcd_level(en,id,b)
#define	log_b8_irq(en,id,d)		tlkdbg_vcd_byte(en,id,d)
#define	log_b16_irq(en,id,d)	tlkdbg_vcd_word(en,id,d)


#ifndef	DUMP_BLE_MSG
#define DUMP_BLE_MSG      (0)
#endif


#ifndef	DUMP_ACL_MSG
#define DUMP_ACL_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_ACL << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_SCO_MSG
#define DUMP_SCO_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_SCO << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_HCI_MSG
#define DUMP_HCI_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_HCI << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_EVT_MSG
#define DUMP_EVT_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_EVT << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_ENC_MSG
#define DUMP_ENC_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_ENC << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_AUTH_MSG
#define DUMP_AUTH_MSG     ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_AUTH << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_PAIR_MSG
#define DUMP_PAIR_MSG     ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_PAIR << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_RSW_MSG
#define DUMP_RSW_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_RSW << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_AFH_MSG
#define DUMP_AFH_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_AFH << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_LMP_MSG
#define DUMP_LMP_MSG      ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_LMP << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_TASK_MSG
#define DUMP_TASK_MSG     ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_TASK << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_NAME_MSG
#define DUMP_NAME_MSG     ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_NAME << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_PAGE_MSG
#define DUMP_PAGE_MSG     ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_PAGE << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_SNIFF_MSG
#define DUMP_SNIFF_MSG    ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_SNIFF << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_TIMER_MSG
#define DUMP_TIMER_MSG    ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_TIMER << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_RESET_MSG
#define DUMP_RESET_MSG    ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_RESET << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif
#ifndef	DUMP_DETACH_MSG
#define DUMP_DETACH_MSG   ((TLK_MAJOR_DBGID_BTC << 24) | (TLK_MINOR_DBGID_BTC_DETACH << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#endif





#endif //TLKSTK_MYUDB_H
