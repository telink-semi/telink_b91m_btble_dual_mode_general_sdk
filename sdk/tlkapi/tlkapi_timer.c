/********************************************************************************************************
 * @file	tlkapi_timer.c
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
#include "tlkapi/tlkapi_stdio.h"
#include "tlkapi_timer.h"
#include "drivers.h"
#include "tlkstk/btble/btble.h"
#include "tlkstk/btble/btble_pm.h"


#define TLKAPI_TIMER_ID     TIMER0


extern void tlkbt_hci_h2cHandler(void);
extern void btble_pm_setAppWakeupLowPower(uint32_t wakeup_tick, uint8_t enable);
extern void btble_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);


//static tlkapi_timer_t *tlkmdi_timer_takeFirst(void);
static void tlkapi_timer_start(uint32 intervalUs);
static void tlkapi_timer_close(void);


static uint08 sTlkApiTimerEnLpWake = true;
static uint08 sTlkAppTimerIsBusy = false;
static uint08 sTlkAppTimerIsStart = false;
static uint32 sTlkAppTimerInterval = 0xFFFFFFFF;

static tlkapi_adapt_t sTlkAppTimerAdapt = {
	true, //uint08 isUpdate;
	0, //uint08 reserve0;
	0, //uint16 reserve1;
	nullptr, //tlkapi_queue_t *pQueueList; //Circulation list
	nullptr, //tlkapi_timer_t *pTimerList; //Singly linked list
};



/******************************************************************************
 * Function: tlkapi_timer_init
 * Descript: Initialize parameters related to hardware scheduled tasks.
 * Params: None.
 * Return: Operating results. TLK_ENONE means success, others means failure.
*******************************************************************************/
int tlkapi_timer_init(void)
{
	#if (TLKAPI_TIMER_ID == TIMER0)
	plic_set_priority(IRQ4_TIMER0, 1);
	#else
	plic_set_priority(IRQ3_TIMER1, 1);
	#endif
	
	reg_tmr_tick(TLKAPI_TIMER_ID) = 0;
	reg_tmr_capt(TLKAPI_TIMER_ID) = 2*1000*sys_clk.pclk;
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_sta |= FLD_TMR_STA_TMR0; //clear irq status
 	reg_tmr_ctrl0 &= (~FLD_TMR0_MODE);
 	reg_tmr_ctrl0 |= TIMER_MODE_SYSCLK;
	#else
	reg_tmr_sta |= FLD_TMR_STA_TMR1; //clear irq status
	reg_tmr_ctrl0 &= (~FLD_TMR1_MODE);
	reg_tmr_ctrl0 |= (TIMER_MODE_SYSCLK<<4);
	#endif

	#if (TLKAPI_TIMER_ID == TIMER0)
	plic_interrupt_enable(IRQ4_TIMER0);
	#else
	plic_interrupt_enable(IRQ3_TIMER1);
	#endif

	if(sTlkApiTimerEnLpWake){
		btble_pm_setAppWakeupLowPower(0, 0);
		btble_pm_registerAppWakeupLowPowerCb(tlkapi_timer_wakeup);
	}

	return TLK_ENONE;
}



/******************************************************************************
 * Function: tlkapi_timer_clear
 * Descript: Clear all timers saved in the list.
 * Params:  
 * Return: None.
 * Notice: 
 *     1.No callback is generated when a scheduled task is cleared.
*******************************************************************************/
void tlkapi_timer_clear(void)
{
	tlkapi_timer_close();
	tlksys_enter_critical();
	tlkapi_adapt_clear(&sTlkAppTimerAdapt);
	tlksys_leave_critical();
}

/******************************************************************************
 * Function: tlkapi_timer_wakeup
 * Descript: Restart the timer, which is usually called after the PM wakes up.
 * Params: 
 *     @mode[IN]--Wake-up source. Refer pm_sleep_wakeup_src_e. Not used in this
 *                interface.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkapi_timer_wakeup(int wakeSource)
{
	if(sTlkAppTimerAdapt.pTimerList == nullptr){
		return;
	}
	tlksys_enter_critical();
	tlkapi_timer_handler();
	tlksys_leave_critical();
}

/******************************************************************************
 * Function: tlkapi_timer_handler
 * Descript: The processing interface of the timer is automatically invoked by
 *           the timer.
 * Params: None.
 * Return: None.
 * Others: 
 *     1.Since this interface is called in the interrupt function 
 *       "timer0_irq_handler", users should use this interface with caution and
 *       call the tlkapi_timer_wakeup interface if necessary.
*******************************************************************************/
void tlkapi_timer_handler(void)
{
	uint32 timeIntval;

	tlkapi_timer_close();
		
	sTlkAppTimerIsBusy = true;
	tlkapi_adapt_handler(&sTlkAppTimerAdapt);

	timeIntval = tlkapi_adapt_timerInterval(&sTlkAppTimerAdapt);
	if(timeIntval < 50) timeIntval = 50;
	sTlkAppTimerIsBusy = false;
	tlkbt_hci_h2cHandler();
	if(timeIntval != 0xFFFFFFFF){
		tlkapi_timer_start(timeIntval);
	}
}

/******************************************************************************
 * Function: tlkapi_timer_isbusy
 * Descript: If there is a process or timer and less than 1ms, the module 
 *           is considered busy, it is the basis of the schedule. 
 * Params:  
 * Return: true is busy/false is idle.
 * Others: None.
*******************************************************************************/
bool tlkapi_timer_isBusy(void)
{
	//If there is a process or timer and less than 1ms, the module is considered busy
	if(tlkapi_timer_interval() < 1000){
		return true;
	}else{
		return false;
	}
}
bool tlkapi_timer_isNull(void)
{
	if(sTlkAppTimerAdapt.pTimerList == nullptr) return true;
	else return false;
}

/******************************************************************************
 * Function: tlkapi_timer_isout
 * Descript: Detects whether the current timer has timed out.
 * Params:
 *     @timer[IN]--Reference timer.
 *     @ticks[IN]--Timeout. unit: 1/16 us:
 * Return: True means the timer is timeout, false means not.
 * Others: None.
*******************************************************************************/
//_attribute_ram_code_sec_noinline_
bool tlkapi_timer_isout(uint32 timer, uint32 ticks)
{
	if((uint32)(clock_time()-timer) >= ticks) return true;
	else return false;
}


/******************************************************************************
 * Function: tlkapi_timer_initNode
 * Descript: Initializes the timing scheduler and work queue.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 *     @userArg[IN]--Parameters passed in by the user will be returned on the call.
 *     @timeout[IN]--Scheduling interval set by the user. Unit:us.
 *     @timerCB[IN]--The callback interface after the time has arrived.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkapi_timer_initNode(tlkapi_timer_t *pTimer, TlkApiTimerCB timerCB, uint32 userArg, uint32 timeout)
{
	int ret;
	tlksys_enter_critical();
	if(tlkapi_adapt_isHaveTimer(&sTlkAppTimerAdapt, pTimer)){
		tlksys_leave_critical();
		return -TLK_EEXIST;
	}
	ret = tlkapi_adapt_initTimer(pTimer, timerCB, userArg, timeout);
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkmdi_timer_isHave
 * Descript: Check whether scheduled tasks exists in the task list.
 * Params:
 *     @pTimer[IN]--Timer scheduling node.
 * Return: Return TRUE if has, FALSE otherwise.
*******************************************************************************/
bool tlkapi_timer_isHaveNode(tlkapi_timer_t *pTimer)
{
	bool ret;
	tlksys_enter_critical();
	ret = tlkapi_adapt_isHaveTimer(&sTlkAppTimerAdapt, pTimer);
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkmdi_timer_update
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pTimer[IN]--Timer.
 *     @timeout[IN]--Unit: us.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkapi_timer_updateNode(tlkapi_timer_t *pTimer, uint32 timeout, bool isInsert)
{
	int ret;
	tlksys_enter_critical();
	ret = tlkapi_adapt_updateTimer(&sTlkAppTimerAdapt, pTimer, timeout, isInsert);
	tlksys_leave_critical();
	if(!sTlkAppTimerIsBusy){
		uint interval = tlkapi_timer_interval();
		if(interval < 50) interval = 50;
		if(interval != 0xFFFFFFFF) tlkapi_timer_start(interval);
	}
	return ret;
}
/******************************************************************************
 * Function: tlkmdi_adapt_insertTimer
 * Descript: Insert a timer timer into the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkapi_timer_insertNode(tlkapi_timer_t *pTimer)
{
	int ret;
	tlksys_enter_critical();
	ret = tlkapi_adapt_insertTimer(&sTlkAppTimerAdapt, pTimer, true);
	tlksys_leave_critical();
	if(!sTlkAppTimerIsBusy){
		uint interval = tlkapi_timer_interval();
		if(interval < 50) interval = 50;
		if(interval != 0xFFFFFFFF) tlkapi_timer_start(interval);
	}
	return ret;
}
/******************************************************************************
 * Function: tlkmdi_timer_remove
 * Descript: Remove a timer timer from the Adapter.
 * Params:
 *     @pAdapt[IN]--The adapt self-manages handle.
 *     @pTimer[IN]--Timer.
 * Return: Operating results. TLK_ENONE means success, others means failure.
 * Others: None.
*******************************************************************************/
int tlkapi_timer_removeNode(tlkapi_timer_t *pTimer)
{
	int ret;
	tlksys_enter_critical();
	ret = tlkapi_adapt_removeTimer(&sTlkAppTimerAdapt, pTimer);
	tlksys_leave_critical();
	return ret;
}

/******************************************************************************
 * Function: tlkapi_timer_interval
 * Descript: Gets the time required for the last timer to arrive. Unit:us.
 * Params:
 *     @pAdapt[IN]--The adapter for a System Task .
 * Return: None.
 * Others: arrival_interval.
*******************************************************************************/
uint tlkapi_timer_interval(void)
{
	uint ret;
	tlksys_enter_critical();
	ret = tlkapi_adapt_timerInterval(&sTlkAppTimerAdapt);
	tlksys_leave_critical();
	return ret;
}


/******************************************************************************
 * Function: tlkapi_timer_take_first_timeout
 * Descript: Retrieves the first timer to arrive in the adapter.
 * Params:
 *     @pAdapt[IN]--The adapter for a System Task .
 * Return: None.
 * Others: None.
*******************************************************************************/
static void tlkapi_timer_start(uint32 intervalUs)
{
	tlksys_enter_critical();
	if(sTlkAppTimerIsStart && sTlkAppTimerInterval <= intervalUs){
		tlksys_leave_critical();
		return;
	}
	if(sTlkAppTimerIsStart){
		#if (TLKAPI_TIMER_ID == TIMER0)
		reg_tmr_ctrl0 &= (~FLD_TMR0_EN); //stop TIMER0
		timer_clr_irq_status(FLD_TMR_STA_TMR0);
		#else
		reg_tmr_ctrl0 &= (~FLD_TMR1_EN);
		timer_clr_irq_status(FLD_TMR_STA_TMR1);
		#endif
	}
	sTlkAppTimerIsStart = true;
	sTlkAppTimerInterval = intervalUs;
	reg_tmr_tick(TLKAPI_TIMER_ID) = 0;
	reg_tmr_capt(TLKAPI_TIMER_ID) = intervalUs*sys_clk.pclk;
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_sta |= FLD_TMR_STA_TMR0; //clear irq status
 	reg_tmr_ctrl0 &= (~FLD_TMR0_MODE);
 	reg_tmr_ctrl0 |= TIMER_MODE_SYSCLK;
	reg_tmr_ctrl0 |= FLD_TMR0_EN;
	#else
	reg_tmr_sta |= FLD_TMR_STA_TMR1; //clear irq status
	reg_tmr_ctrl0 &= (~FLD_TMR1_MODE);
	reg_tmr_ctrl0 |= (TIMER_MODE_SYSCLK<<4);
	reg_tmr_ctrl0 |= FLD_TMR1_EN;
	#endif
	if(sTlkApiTimerEnLpWake){
		if(intervalUs >= 2000000){
			btble_pm_setAppWakeupLowPower(0, 0);
		}else{
			uint32 nextTick = clock_time();
			if(intervalUs < 15) nextTick += 15*SYSTEM_TIMER_TICK_1US;
			else nextTick += intervalUs*SYSTEM_TIMER_TICK_1US;
			btble_pm_setAppWakeupLowPower(nextTick, 1);
		}
	}
	tlksys_leave_critical();
}
static void tlkapi_timer_close(void)
{
	tlksys_enter_critical();
	sTlkAppTimerIsStart = false;
	sTlkAppTimerInterval = 0xFFFFFFFF;
	
	#if (TLKAPI_TIMER_ID == TIMER0)
	reg_tmr_ctrl0 &= (~FLD_TMR0_EN); //stop TIMER0
	timer_clr_irq_status(FLD_TMR_STA_TMR0);
	#else
	reg_tmr_ctrl0 &= (~FLD_TMR1_EN);
	timer_clr_irq_status(FLD_TMR_STA_TMR1);
	#endif
	if(sTlkApiTimerEnLpWake){
		btble_pm_setAppWakeupLowPower(0, 0);
	}
	tlksys_leave_critical();
}


