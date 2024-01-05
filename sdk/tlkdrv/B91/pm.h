/********************************************************************************************************
 * @file     pm.h
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

#pragma once
#include "reg_include/register.h"
#include "compiler.h"
#include "gpio.h"
#include "clock.h"
#include "app/app_config.h"

#define PM_LONG_SUSPEND_EN					0

#ifndef PM_TIM_RECOVER_MODE
#define PM_TIM_RECOVER_MODE			    	0 //TODO: no need this now, eagle have 32k RC clock align design
#endif

#ifndef	SYS_TIMER_AUTO_MODE
#define SYS_TIMER_AUTO_MODE					1
#endif

/**
 * @brief analog register 0x7f will not be cleared when watch dog reboot,
 * The software must ensure that the bit(0) of the ana_0x7f register cannot be 0
 * before reboot(Avoid being misjudged as retention back).
 */
#ifndef WDT_REBOOT_RESET_ANA7F_WORK_AROUND
#define WDT_REBOOT_RESET_ANA7F_WORK_AROUND	1  //TODO: SiHui & LiBiao
#endif

#ifndef	PM_32k_RC_CALIBRATION_ALGORITHM_EN
#define PM_32k_RC_CALIBRATION_ALGORITHM_EN	1
#endif


#define PM_LONG_SLEEP_WAKEUP_EN			    0 //if user need to make MCU sleep for a long time that is more than 268s, this macro need to be enabled and use "pm_long_sleep_wakeup" function

/**
 * @brief these analog register can store data in deepsleep mode or deepsleep with SRAM retention mode.
 * 	      Reset these analog registers by watchdog, chip reset, RESET Pin, power cycle
 */
#define PM_ANA_REG_WD_CLR_BUF0 			0x38 // initial value 0xff. [Bit0] is already occupied. The customer cannot change!

/**
 * @brief analog register below can store information when MCU in deepsleep mode or deepsleep with SRAM retention mode.
 * 	      Reset these analog registers only by power cycle
 */
#define PM_ANA_REG_POWER_ON_CLR_BUF0 	0x39 // initial value 0x00. [Bit0][Bit1] is already occupied. The customer cannot change!
#define PM_ANA_REG_POWER_ON_CLR_BUF1 	0x3a // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF2 	0x3b // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF3 	0x3c // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF4 	0x3d // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF5 	0x3e // initial value 0x00
#define PM_ANA_REG_POWER_ON_CLR_BUF6	0x3f // initial value 0x0f
#define PM_ANA_REG_POWER_ON_CLR_BUF7    0x38 //initial value =0xff

/**
 * @brief	gpio wakeup level definition
 */
typedef enum{
	WAKEUP_LEVEL_LOW		= 0,
	WAKEUP_LEVEL_HIGH		= 1,
}pm_gpio_wakeup_level_e;


#define WAKEUP_STATUS_TIMER_CORE     	    ( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_CORE)
#define WAKEUP_STATUS_TIMER_PAD		        ( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_PAD)

/**
 * @brief these analog register can store data in deepsleep mode or deepsleep with SRAM retention mode.
 * 	      Reset these analog registers by watchdog, chip reset, RESET Pin, power cycle
 */





#define SYS_NEED_REINIT_EXT32K			    BIT(1)

//ana3e system used, user can not use
#define SYS_DEEP_ANA_REG 					PM_ANA_REG_POWER_ON_CLR_BUF0

#define	ZB_POWER_DOWN						1 //whether to power down the RF before suspend
#define	AUDIO_POWER_DOWN					1 //whether to power down the AUDIO before suspend
#define	USB_POWER_DOWN						1 //whether to power down the USB before suspend  //PA5/PA6 pad low wakeup need USB power on


/**
 * @brief	wakeup tick type definition
 */
typedef enum {
	 PM_TICK_STIMER_16M		= 0,
	 PM_TICK_32K			= 1,
}pm_wakeup_tick_type_e;

/**
 * @brief	suspend power whether to power down definition
 */
typedef enum {
	 PM_POWERON_BASEBAND  	= BIT(0),	//whether to power on the BASEBAND before suspend.
	 PM_POWERON_USB  		= BIT(1),	//whether to power on the USB before suspend.
	 PM_POWERON_NPE 		= BIT(2),	//whether to power on the NPE before suspend.
}pm_suspend_power_cfg_e;

/**
 * @brief	sleep mode.
 */
typedef enum {
	//available mode for customer
	SUSPEND_MODE						= 0x00, //The A0 version of the suspend execution process is abnormal and the program restarts.
	DEEPSLEEP_MODE						= 0x30,	//when use deep mode pad wakeup(low or high level), if the high(low) level always in
												//the pad, system will not enter sleep and go to below of pm API, will reboot by core_6f = 0x20
												//deep retention also had this issue, but not to reboot.
	DEEPSLEEP_MODE_RET_SRAM_LOW32K  	= 0x21, //for boot from sram
	DEEPSLEEP_MODE_RET_SRAM_LOW64K  	= 0x03, //for boot from sram
	//not available mode
	DEEPSLEEP_RETENTION_FLAG			= 0x0F,
}pm_sleep_mode_e;

/**
 * @brief	available wake-up source for customer
 */
typedef enum {
	 //available wake-up source for customer
	 PM_WAKEUP_PAD   		= BIT(3),
	 PM_WAKEUP_CORE  		= BIT(4),
	 PM_WAKEUP_TIMER 		= BIT(5),
	 PM_WAKEUP_COMPARATOR 	= BIT(6),
	 PM_WAKEUP_MDEC		 	= BIT(7),
	 //not available wake-up source for customer
	 PM_TIM_RECOVER_START   = BIT(14),
	 PM_TIM_RECOVER_END     = BIT(15),
}pm_sleep_wakeup_src_e;

/**
 * @brief	wakeup status
 */
typedef enum {
	 WAKEUP_STATUS_COMPARATOR  		= BIT(0),
	 WAKEUP_STATUS_TIMER  			= BIT(1),
	 WAKEUP_STATUS_CORE 			= BIT(2),
	 WAKEUP_STATUS_PAD    			= BIT(3),
	 WAKEUP_STATUS_MDEC    			= BIT(4),

	 STATUS_GPIO_ERR_NO_ENTER_PM	= BIT(7),
	 STATUS_ENTER_SUSPEND  			= BIT(30),
}pm_wakeup_status_e;

/**
 * @brief	mcu status
 * 			In order to fix the problem that reboot returns to occasional crash when hclk = 1/2cclk, after each reboot,
 * 			it will immediately enter deep. Therefore, the user will not see the reboot status. Increase the REBOOT_DEEP
 * 			state to indicate this process.(add by weihua.zhang, confirmed by libiao and yangbin 20201211)
 */
typedef enum{
	MCU_STATUS_POWER_ON  		= BIT(0),
	MCU_STATUS_REBOOT_BACK    	= BIT(2),	//the user will not see the reboot status.
	MCU_STATUS_DEEPRET_BACK  	= BIT(3),
	MCU_STATUS_DEEP_BACK		= BIT(4),
	MCU_STATUS_REBOOT_DEEP_BACK	= BIT(5),	//reboot + deep
}pm_mcu_status;

/**
 * @brief	early wakeup time
 */
typedef struct {
	unsigned short  suspend_early_wakeup_time_us;	/**< suspend_early_wakeup_time_us = deep_ret_r_delay_us + xtal_stable_time + early_time*/
	unsigned short  deep_ret_early_wakeup_time_us;  /**< deep_ret_early_wakeup_time_us = deep_ret_r_delay_us + early_time*/
	unsigned short  deep_early_wakeup_time_us;		/**< deep_early_wakeup_time_us = suspend_ret_r_delay_us*/
	unsigned short  sleep_min_time_us;				/**< sleep_min_time_us = suspend_early_wakeup_time_us + 200*/
}pm_early_wakeup_time_us_s;

/**
 * @brief	hardware delay time
 */
typedef struct {
	unsigned short  deep_r_delay_cycle ;			/**< hardware delay time ,deep_ret_r_delay_us = deep_r_delay_cycle * 1/16k */
	unsigned short  suspend_ret_r_delay_cycle ;		/**< hardware delay time ,suspend_ret_r_delay_us = suspend_ret_r_delay_cycle * 1/16k */

}pm_r_delay_cycle_s;

/**
 * @brief   deepsleep wakeup status
 */
typedef struct{
	unsigned char is_pad_wakeup;
	unsigned char wakeup_src;	//The pad pin occasionally wakes up abnormally in A0. The core wakeup flag will be incorrectly set in A0.
	unsigned char mcu_status;
	unsigned char rsvd;
}pm_status_info_s;

/**
 * @brief   pm 32k rc calibration algorithm.
 */
typedef struct  pm_clock_drift
{
	unsigned int	ref_tick;
	unsigned int	ref_tick_32k;
	int				offset;
//	int				offset_dc;   //not used now
//	int				offset_cur;  //not used now
	unsigned int	offset_cal_tick;
	int				tc;
	int				rc32;
	int				rc32_wakeup;
	int				rc32_rt;
	int				s0;
	unsigned char	calib;
	unsigned char	ref_no;

} pm_clock_drift_t;


extern pm_clock_drift_t	pmcd;

/**
 * @brief   deepsleep wakeup by external xtal
 */
typedef struct{
	unsigned char ext_cap_en;    //24xtal  cap
	unsigned char pad32k_en;
	unsigned char pm_enter_en;
	unsigned char rsvd;
}misc_para_t;

extern  _attribute_aligned_(4) misc_para_t 				blt_miscParam;

/**
 * @brief   deepsleep wakeup status
 */
typedef struct{
	unsigned char is_deepretn_back;
	unsigned char is_pad_wakeup;
	unsigned char wakeup_src;
	unsigned char mcu_status;
}pm_para_t;

void pm_32k_rc_offset_init(void);

void pm_cal_32k_rc_offset (int offset_tick);

void pm_ble_cal_32k_rc_offset (int, int);

unsigned int pm_32k_rc_get_calib_tick (int t);

static inline void pm_32k_rc_init () {pmcd.tc = 0; pmcd.offset = 0; pmcd.ref_tick = 0;};

unsigned int pm_32k_rc_get_calib ();

extern _attribute_aligned_(4) pm_para_t	pmParam;

#if (PM_TIM_RECOVER_MODE)

typedef struct{
	unsigned int   tick_sysClk;
	unsigned int   tick_32k;
	unsigned int   recover_flag;
}pm_tim_recover_t;

extern _attribute_aligned_(4) pm_tim_recover_t			pm_timRecover;
#endif

extern _attribute_aligned_(4) pm_status_info_s g_pm_status_info;
extern _attribute_data_retention_sec_ unsigned char g_pm_suspend_power_cfg;
extern _attribute_data_retention_sec_ unsigned char g_pm_vbat_v;

typedef int (*suspend_handler_t)(void);
extern  suspend_handler_t 		 func_before_suspend;

typedef void (*check_32k_clk_handler_t)(void);
extern  check_32k_clk_handler_t  pm_check_32k_clk_stable;
typedef unsigned int (*pm_get_32k_clk_handler_t)(void);
extern  pm_get_32k_clk_handler_t	pm_get_32k_tick;

void bls_pm_registerFuncBeforeSuspend (suspend_handler_t func );


/**
 * @brief		This function servers to set the match value for MDEC wakeup.
 * @param[in]	value - the MDEC match value for wakeup.
 * @return		none.
 */
static inline void pm_set_mdec_value_wakeup(unsigned char value)
{
	analog_write_reg8(mdec_ctrl,((analog_read_reg8(mdec_ctrl) & (~0x0f)) | value));
}

/**
 * @brief		This function serves to set baseband/usb/npe power on.
 * @param[in]	value - weather to power on the baseband/usb/npe.
 * @return		none.
 */
static inline void pm_set_suspend_power_cfg(pm_suspend_power_cfg_e value)
{
	g_pm_suspend_power_cfg &= (~value);
}

/**
 * @brief		This function serves to get deep retention flag.
 * @return		1 deep retention, 0 deep.
 */
static inline unsigned char pm_get_deep_retention_flag(void)
{
	return !(analog_read_reg8(0x7f) & BIT(0));
}

/**
 * @brief		This function serves to get wakeup source.
 * @return		wakeup source.
 */
static inline pm_wakeup_status_e pm_get_wakeup_src(void)
{
	return analog_read_reg8(0x64);
}

/**
 * @brief		This function configures a GPIO pin as the wakeup pin.
 * @param[in]	pin	- the pin needs to be configured as wakeup pin.
 * @param[in]	pol - the wakeup polarity of the pad pin(0: low-level wakeup, 1: high-level wakeup).
 * @param[in]	en  - enable or disable the wakeup function for the pan pin(1: enable, 0: disable).
 * @return		none.
 */
void pm_set_gpio_wakeup (gpio_pin_e pin, pm_gpio_wakeup_level_e pol, int en);
#define cpu_set_gpio_wakeup				pm_set_gpio_wakeup

/**
 * @brief   This function serves to reboot chip.
 * @param   none.
 * @return  none.
 */

void start_reboot(void);

/**
 * @brief		This function configures pm wakeup time parameter.
 * @param[in]	param - pm wakeup time parameter.
 * @return		none.
 */
void pm_set_wakeup_time_param(pm_r_delay_cycle_s param);

/**
 * @brief		this function servers to wait bbpll clock lock.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void pm_wait_bbpll_done(void);

/**
 * @brief		This function serves to recover system timer.
 * 				The code is placed in the ram code section, in order to shorten the time.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ void pm_stimer_recover(void);

/**
 * @brief   This function serves to recover system timer from tick of internal 32k RC.
 * @param   none.
 * @return  none.
 */
unsigned int pm_tim_recover_32k_rc(unsigned int now_tick_32k);

/**
 * @brief   This function serves to recover system timer from tick of external 32k crystal.
 * @param   none.
 * @return  none.
 */
unsigned int pm_tim_recover_32k_xtal(unsigned int now_tick_32k);


typedef unsigned int (*pm_tim_recover_handler_t)(unsigned int);

extern  pm_tim_recover_handler_t pm_tim_recover;

/**
 * @brief   This function serves to get the 32k tick.
 * @param   none
 * @return  tick of 32k .
 */

extern unsigned int get_32k_tick(void);

/**
 * @brief      This function serves to set the working mode of MCU based on 32k crystal,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return     indicate whether the cpu is wake up successful.
 */
int  cpu_sleep_wakeup_32k_rc(pm_sleep_mode_e sleep_mode,  pm_sleep_wakeup_src_e wakeup_src, unsigned int  wakeup_tick);

/**
 * @brief      This function serves to set the working mode of MCU based on 32k crystal,e.g. suspend mode, deepsleep mode, deepsleep with SRAM retention mode and shutdown mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of short sleep, which means MCU can sleep for less than 5 minutes.
 * @return     indicate whether the cpu is wake up successful.
 */
int  cpu_sleep_wakeup_32k_xtal(pm_sleep_mode_e sleep_mode,  pm_sleep_wakeup_src_e wakeup_src, unsigned int  wakeup_tick);

typedef int (*cpu_pm_handler_t)(pm_sleep_mode_e sleep_mode,  pm_sleep_wakeup_src_e wakeup_src, unsigned int  wakeup_tick);

extern 	cpu_pm_handler_t  		 cpu_sleep_wakeup;

/**
 * @brief      This function serves to determine whether wake up source is internal 32k RC.
 * @param[in]  none.
 * @return     none.
 */
static inline void blc_pm_select_internal_32k_crystal(void)
{
	cpu_sleep_wakeup 	 	= cpu_sleep_wakeup_32k_rc;
	pm_tim_recover  	 	= pm_tim_recover_32k_rc;

	blt_miscParam.pm_enter_en 	= 1; // allow enter pm, 32k rc does not need to wait for 32k clk to be stable
}

extern void check_32k_clk_stable(void);

/**
 * @brief      This function serves to determine whether wake up source is external 32k RC.
 * @param[in]  none.
 * @return     none.
 */
static inline void blc_pm_select_external_32k_crystal(void)
{
	cpu_sleep_wakeup 	 	= cpu_sleep_wakeup_32k_xtal;
	pm_check_32k_clk_stable = check_32k_clk_stable;
	pm_tim_recover		 	= pm_tim_recover_32k_xtal;
	pm_get_32k_tick 		= get_32k_tick;
	blt_miscParam.pad32k_en 	= 1; // set '1': 32k clk src use external 32k crystal
}

/**********************************  Internal APIs (not for user)***************************************************/
extern  unsigned char 		    tl_24mrc_cal;
extern 	unsigned int 			g_pm_tick_32k_calib;
extern  unsigned int 			g_pm_tick_cur;
extern  unsigned int 			g_pm_tick_32k_cur;
extern  unsigned char       	g_pm_long_suspend;
extern  unsigned int 			g_pm_multi_addr;

void pm_sleep_start(void);

#if PM_LONG_SLEEP_WAKEUP_EN
/**
 * @brief      This function servers to wake up the cpu from sleep mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  SleepDurationUs - the time of sleep.
 * @return     indicate whether the cpu is wake up successful.
 */
int pm_long_sleep_wakeup (pm_sleep_mode_e sleep_mode, pm_sleep_wakeup_src_e wakeup_src, unsigned int  SleepDurationUs);
#endif
/**
 * @brief      This function servers to wake up the cpu from sleep mode.
 * @param[in]  sleep_mode - sleep mode type select.
 * @param[in]  wakeup_src - wake up source select.
 * @param[in]  wakeup_tick - the time of sleep,unit is 31.25us,1ms = 32.
 * @return     indicate whether the cpu is wake up successful.
 */
int cpu_long_sleep_wakeup_32k_rc(pm_sleep_mode_e sleep_mode,  pm_sleep_wakeup_src_e wakeup_src, unsigned int  wakeup_tick);



extern unsigned char pm_low_power_reg_saved;


void pm_update_status_info(void);
void pm_switch_ext32kpad_to_int32krc(void);
void pm_low_power_en (void);
void pm_low_power_record(void);
void pm_low_power_recover(void);
