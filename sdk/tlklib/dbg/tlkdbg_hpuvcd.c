/********************************************************************************************************
 * @file	tlkdbg_hpuvcd.c
 *
 * @brief	This is the source file for BTBLE SDK
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
#include "tlk_config.h"
#if (TLK_DBG_VCD_ENABLE)
#include <stdio.h>
#include <stdarg.h>
#include "tlkapi/tlkapi_stdio.h"
#include "tlkdbg.h"
#include "tlkdbg_config.h"
#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#include "tlkdbg_define.h"
#include "tlkdbg_hpuvcd.h"
#include "tlksys/prt/tlkpto_stdio.h"
#include "drivers.h"

//HPU - Hardware Protocol UART

//static bool tlkdbg_hpuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg);
//static tlkapi_timer_t sTlkDbgHpuVcdTimer;

static tlkdbg_hpuvcd_t sTlkDbgHpuVCD;


//uint32 AAAA_test_tick01 = 0;
//uint32 AAAA_test_tick02 = 0;
//uint32 AAAA_test_tick03 = 0;
//uint32 AAAA_test_tick04 = 0;

void tlkdbg_hpuvcd_init(void)
{
	tmemset(&sTlkDbgHpuVCD, 0, sizeof(tlkdbg_hpuvcd_t));
	sTlkDbgHpuVCD.isFirst = true;
//	tlkapi_timer_initNode(&sTlkDbgHpuVcdTimer, tlkdbg_hpuvcd_timer, nullptr, 5000);
}

void tlkdbg_hpuvcd_handler(void)
{
	int ret;
	uint32 rptr;
	uint32 index;
	uint32 buffer[TLKDBG_HPUVCD_CACHE_POINT];

	if(!sTlkDbgHpuVCD.isOpen || sTlkDbgHpuVCD.sendCB == nullptr){
		return;
	}
	
	if(sTlkDbgHpuVCD.syncTicks == 0 || clock_time_exceed(sTlkDbgHpuVCD.syncTicks, 100000)){
		sTlkDbgHpuVCD.syncTicks = clock_time() | 1;
		tlkdbg_hpuvcd_sync();
	}
//	if(AAAA_test_tick01 == 0 || clock_time_exceed(AAAA_test_tick01, 750)){
//		AAAA_test_tick01 = clock_time() | 1;
//		tlkdbg_hpuvcd_tick(1);
//	}
//	if(AAAA_test_tick02 == 0 || clock_time_exceed(AAAA_test_tick02, 800)){
//		AAAA_test_tick02 = clock_time() | 1;
//		tlkdbg_hpuvcd_tick(2);
//	}
//	if(AAAA_test_tick03 == 0 || clock_time_exceed(AAAA_test_tick03, 900)){
//		AAAA_test_tick03 = clock_time() | 1;
//		tlkdbg_hpuvcd_tick(3);
//	}
//	if(AAAA_test_tick04 == 0 || clock_time_exceed(AAAA_test_tick04, 600)){
//		AAAA_test_tick04 = clock_time() | 1;
//		tlkdbg_hpuvcd_tick(4);
//	}
	
#if (TLKDBG_HPUVCD_FLOW_CTRL_EN)
	if(sTlkDbgHpuVCD.cOffset == sTlkDbgHpuVCD.wOffset) return;
#else
	if(sTlkDbgHpuVCD.rOffset == sTlkDbgHpuVCD.wOffset) return;
#endif
	while(true){
#if (TLKDBG_HPUVCD_FLOW_CTRL_EN)
		rptr = sTlkDbgHpuVCD.cOffset;
#else
		rptr = sTlkDbgHpuVCD.rOffset;
#endif
		buffer[0] = sTlkDbgHpuVCD.currentNumb;
		for(index=1; index<sTlkDbgHpuVCD.sendSize; index++){
			if(rptr == sTlkDbgHpuVCD.wOffset) break;
			buffer[index] = sTlkDbgHpuVCD.buffer[rptr++];
			if(rptr == TLKDBG_HPUVCD_POINTS_NUMB) rptr = 0;
		}
		if(index == 1) break;
		ret = sTlkDbgHpuVCD.sendCB((char*)buffer, index << 2);
		if(ret >= 0 || ret == -TLK_ENOSUPPORT){
			sTlkDbgHpuVCD.currentNumb += index-1;
#if (TLKDBG_HPUVCD_FLOW_CTRL_EN)
			sTlkDbgHpuVCD.cOffset = rptr;
			if(sTlkDbgHpuVCD.cOffset == sTlkDbgHpuVCD.wOffset) break;
#else
			sTlkDbgHpuVCD.rOffset = rptr;
			if(sTlkDbgHpuVCD.rOffset == sTlkDbgHpuVCD.wOffset) break;
#endif
		}else{
			break;
		}
	}
}

//bool tlkdbg_hpuvcd_timer(tlkapi_timer_t *pTimer, uint32 userArg)
//{
//	tlkdbg_hpuvcd_handler();
//	return true;
//}
bool tlkdbg_hpuvcd_isUpdate(void)
{
	if(sTlkDbgHpuVCD.currentNumb-sTlkDbgHpuVCD.comfirmNumb > TLKDBG_HPUVCD_FCS_POINTS){
		return true;
	}
	return false;
}

bool tlkdbg_hpuvcd_getParam(uint08 *isForce, uint32 *currentNumb, uint32 *totalSignal, uint32 *stallSignal)
{
	if(sTlkDbgHpuVCD.isFirst || sTlkDbgHpuVCD.currentNumb-sTlkDbgHpuVCD.comfirmNumb 
		> TLKDBG_HPUVCD_POINTS_NUMB-64){
		*isForce = true;
	}else{
		*isForce = false;
	}
	*currentNumb = sTlkDbgHpuVCD.currentNumb;
	*totalSignal = sTlkDbgHpuVCD.totalSignal;
	*stallSignal = sTlkDbgHpuVCD.stallSignal; 
	return true;
}
void tlkdbg_hpuvcd_syncParam(uint08 isStall, uint32 confirmNumb)
{
#if (TLKDBG_HPUVCD_FLOW_CTRL_EN)
	uint32 offset;
	if(confirmNumb > sTlkDbgHpuVCD.currentNumb || confirmNumb < sTlkDbgHpuVCD.comfirmNumb){
		sTlkDbgHpuVCD.isFirst = true;
		return;
	}
	if(confirmNumb == sTlkDbgHpuVCD.comfirmNumb) return;
	offset = confirmNumb-sTlkDbgHpuVCD.comfirmNumb;
	offset = sTlkDbgHpuVCD.rOffset+offset;
	if(offset >= TLKDBG_HPUVCD_POINTS_NUMB){
		offset -= TLKDBG_HPUVCD_POINTS_NUMB;
	}
	sTlkDbgHpuVCD.comfirmNumb = confirmNumb;
	sTlkDbgHpuVCD.rOffset = offset;
	if(!isStall) return;
	if(confirmNumb > sTlkDbgHpuVCD.currentNumb){
		sTlkDbgHpuVCD.isFirst = true;
		return;
	}
	offset = sTlkDbgHpuVCD.currentNumb-confirmNumb;
	if(sTlkDbgHpuVCD.cOffset >= offset){
		sTlkDbgHpuVCD.cOffset -= offset;
	}else{
		sTlkDbgHpuVCD.cOffset = TLKDBG_HPUVCD_POINTS_NUMB-offset+sTlkDbgHpuVCD.cOffset;
	}
#endif
}

void tlkdbg_hpuvcd_regSendCB(TlkDbgHpuVcdSendCB cb, uint16 maxLen)
{
	sTlkDbgHpuVCD.sendCB = cb;
	maxLen = maxLen >> 2;
	if(maxLen == 0 || maxLen >= TLKDBG_HPUVCD_CACHE_POINT){
		sTlkDbgHpuVCD.sendSize = TLKDBG_HPUVCD_CACHE_POINT;
	}else{
		sTlkDbgHpuVCD.sendSize = maxLen;
	}
}
void tlkdbg_hpuvcd_enable(bool enable)
{
	if(enable && sTlkDbgHpuVCD.sendCB == nullptr){
		return;
	}
	if(sTlkDbgHpuVCD.isOpen == enable) return;

	if(enable){
		tlkdbg_hpuvcd_reset();
	}
	sTlkDbgHpuVCD.isOpen = enable;
}


void tlkdbg_hpuvcd_reset(void)
{
	sTlkDbgHpuVCD.isFirst = true;
	sTlkDbgHpuVCD.wOffset = 0;
	sTlkDbgHpuVCD.rOffset = 0;
#if (TLKDBG_HPUVCD_FLOW_CTRL_EN)
	sTlkDbgHpuVCD.cOffset = 0;
#endif
	sTlkDbgHpuVCD.syncTicks = 0;
	sTlkDbgHpuVCD.stallSignal = 0;
	sTlkDbgHpuVCD.totalSignal = 0;
	sTlkDbgHpuVCD.currentNumb = 0;
	sTlkDbgHpuVCD.comfirmNumb = 0;
}
bool tlkdbg_hpuvcd_isBusy(void)
{
	if(!sTlkDbgHpuVCD.isOpen || sTlkDbgHpuVCD.rOffset == sTlkDbgHpuVCD.wOffset) return false;
	else return true;
}



_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_sync(void)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = (clock_time() & 0x00FFFFF8) 
				| (TLKDBG_HPU_VCD_TYPE_EVENT & 0x07);
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_tick(uint08 id)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = (clock_time() & 0x00FFFFF8) 
				| (TLKDBG_HPU_VCD_TYPE_TICK & 0x07) | ((uint32)id<<24);
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_level(uint08 id, uint08 level)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			if (level == 0) {
				sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = (clock_time() & 0x00FFFFF8) 
					| (TLKDBG_HPU_VCD_TYPE_LEVEL0 & 0x07) | ((uint32)id<<24);
			} else {
				sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = (clock_time() & 0x00FFFFF8) 
					| (TLKDBG_HPU_VCD_TYPE_LEVEL1 & 0x07) | ((uint32)id<<24);
			}
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_event(uint08 id)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = (clock_time() & 0x00FFFFF8) 
				| (TLKDBG_HPU_VCD_TYPE_EVENT & 0x07) | ((uint32)id<<24);
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_byte(uint08 id, uint08 value)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = ((uint32)value<<8)
				| (TLKDBG_HPU_VCD_TYPE_BYTE & 0x07) | ((uint32)id<<24);
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}
_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_word(uint08 id, uint16 value)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = ((uint32)value<<8)
				| (TLKDBG_HPU_VCD_TYPE_WORD & 0x07) | ((uint32)id<<24);
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}

_attribute_ram_code_sec_noinline_
void tlkdbg_hpuvcd_byte3(uint08 id, uint32 value)
{
	if (sTlkDbgHpuVCD.isOpen) {
		tlksys_disable_interrupt();
		sTlkDbgHpuVCD.totalSignal ++;
		if (sTlkDbgHpuVCD.wOffset+1 == sTlkDbgHpuVCD.rOffset) {
			sTlkDbgHpuVCD.stallSignal ++;
		} else {
			sTlkDbgHpuVCD.buffer[sTlkDbgHpuVCD.wOffset++] = ((uint32)value<<8)
				| (TLKDBG_HPU_VCD_TYPE_WORD & 0x07) | ((uint32)id<<24);
			if(sTlkDbgHpuVCD.wOffset == TLKDBG_HPUVCD_POINTS_NUMB) sTlkDbgHpuVCD.wOffset = 0;
		}
		tlksys_restore_interrupt();
	}
}



#endif //#if (TLKDBG_CFG_HPU_VCD_ENABLE)
#endif //#if (TLK_DBG_VCD_ENABLE)

