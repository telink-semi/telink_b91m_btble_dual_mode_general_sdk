/********************************************************************************************************
 * @file	tlkdrv_led.h
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
#ifndef TLKDRV_LED_H
#define TLKDRV_LED_H

#if (TLK_DEV_LED_ENABLE)



#define TLKDRV_LED_TIMEOUT          20000 //100ms

#define TLKDRV_LED_NOR_MAX_NUMB     2 //Normal LED
#define TLKDRV_LED_PWM_MAX_NUMB     2 //PWM Control LED

#if (TLKDRV_LED_PWM_MAX_NUMB != 0)
#define TLKDRV_LED_PWM_PCLK_SPEED   1000000 //pwm clock 1M.
enum TLKDRV_LED_PWM_CLOCK_ENUM
{
	TLKDRV_LED_PWM_CLOCK_1S    = TLKDRV_LED_PWM_PCLK_SPEED,
	TLKDRV_LED_PWM_CLOCK_1MS   = (TLKDRV_LED_PWM_CLOCK_1S / 1000),
	TLKDRV_LED_PWM_CLOCK_1US   = (TLKDRV_LED_PWM_CLOCK_1S / 1000000),
	TLKDRV_LED_PWM_CLOCK_10MS  = (TLKDRV_LED_PWM_CLOCK_1S / 100),
	TLKDRV_LED_PWM_CLOCK_100MS = (TLKDRV_LED_PWM_CLOCK_1S / 10),
	TLKDRV_LED_PWM_CLOCK_10US  = (TLKDRV_LED_PWM_CLOCK_1S / 100000),
	TLKDRV_LED_PWM_CLOCK_100US = (TLKDRV_LED_PWM_CLOCK_1S / 10000),
};
#endif //(TLKDRV_LED_PWM_MAX_NUMB != 0)


typedef struct{
	uint08 ledID;
	uint08 level;
	uint16 count;
	uint08 upDown;
	uint08 isCtrl;
	uint08 isOpen;
	uint08 isKeep; //keep on when ctrl is over
	uint32 ioPort;
	uint32 onTimer;
	uint32 offTimer;
	uint32 runTimer;
}tlkdrv_led_nor_unit_t; //Normal LED
typedef struct{
	uint08 ledID;
	uint08 pwmID;
	uint16 count;
	uint08 isInvt; //invert control
	uint08 isCtrl;
	uint08 isOpen;
	uint08 isKeep; //keep on when ctrl is over
	uint32 ioPort;
	uint16 dutyMax;
	uint16 curDuty;
	uint16 stepChg;
	uint16 flushTmo; //Flush Timeout
	uint32 onTimer;
	uint32 offTimer;
	uint32 runTimer;
	uint32 flushTmr; //Flush Timer
}tlkdrv_led_pwm_unit_t; //PWM Control LED
typedef struct{
	#if (TLKDRV_LED_NOR_MAX_NUMB != 0)
	tlkapi_timer_t norTimer;
	tlkdrv_led_nor_unit_t norUnit[TLKDRV_LED_NOR_MAX_NUMB];
	#endif
	#if (TLKDRV_LED_PWM_MAX_NUMB != 0)
	tlkapi_timer_t pwmTimer;
	tlkdrv_led_pwm_unit_t pwmUnit[TLKDRV_LED_PWM_MAX_NUMB];
	#endif
	#if ((TLKDRV_LED_NOR_MAX_NUMB == 0) && (TLKDRV_LED_PWM_MAX_NUMB == 0))
	uint32 reserve;
	#endif
}tlkdrv_led_ctrl_t;

/******************************************************************************
 * Function: tlkdrv_led_init
 * Descript: Initial I/O and PWM timers of the led
 * Params: None.
 * Return: TLK_ENONE is success.
*******************************************************************************/
int tlkdrv_led_init(void);

bool tlkdrv_led_isBusy(void);
void tlkdrv_led_enterSleep(uint mcuMode);
void tlkdrv_led_leaveSleep(uint wakeSrc);


#if (TLKDRV_LED_NOR_MAX_NUMB != 0)

/******************************************************************************
 * Function: tlkdrv_led_insert
 * Descript: Insert a led and initial the GPIO for the led.
 * Params: @ledID[IN]--The ledID, for instance 0x01, 0x02.
 * 		   @ioPort[IN]--ioPort
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @onLevel[IN]--LED effective level.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int  tlkdrv_led_insert(uint08 ledID, uint32 ioPort, uint08 upDown, uint08 onLevel);

/******************************************************************************
 * Function: tlkdrv_led_remove
 * Descript: Disable the gpio for the led and remove a timer.
 * Params: @ledID[IN]--The ledID.
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @enOutput[IN]--enOutput, true enable output, false disable output.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int  tlkdrv_led_remove(uint08 ledID, uint08 upDown, bool enOutput);

/******************************************************************************
 * Function: tlkdrv_led_isOn
 * Descript: Get the led switch status.
 * Params: @ledID[IN]--The ledID.
 * Return: True is on, false is off.
*******************************************************************************/
bool tlkdrv_led_isOn(uint08 ledID);

/******************************************************************************
 * Function: tlkdrv_led_isAuto
 * Descript: Get the led control status.
 * Params: @ledID[IN]--The ledID.
 * Return: True is controlled, false is not controlled.
*******************************************************************************/
bool tlkdrv_led_isAuto(uint08 ledID);

/******************************************************************************
 * Function: tlkdrv_led_on
 * Descript: Turn on a led.
 * Params: @keledIDyID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdrv_led_on(uint08 ledID);

/******************************************************************************
 * Function: tlkdrv_led_off
 * Descript: Turn off a led.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdrv_led_off(uint08 ledID);

/******************************************************************************
 * Function: tlkdrv_led_auto
 * Descript: Control led flash,the led flash frequency can be controlled by
 * 			 'onTimerMs' and 'offTimerMs', can also choose to keep the led on
 * 			 or off after the flashing is over.
 * Params: @ledID[IN]--The ledID.
 * 		   @firstOn[IN]--The state of the first work, true is on, false is off.
 * 		   @count[IN]--led flash count.
 * 		   @onTimerMs[IN]--on time.
 * 		   @offTimerMs[IN]--off time.
 * 		   @isKeepOn[IN]--The status after the flash is over, true is on, false is off.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdrv_led_auto(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs, bool isKeepOn);

tlkdrv_led_nor_unit_t *tlkdrv_led_getUsedUnit(uint08 ledID);

#endif // #if (TLKDRV_LED_NOR_MAX_NUMB != 0)

#if (TLKDRV_LED_PWM_MAX_NUMB != 0)

/******************************************************************************
 * Function: tlkdev_pwmled_insert
 * Descript: Insert an led with a PWM function, enable the PWM function of the LED.
 * Params: @ledID[IN]--The ledID.
 * 		   @ioPort[IN]--ioPort
 * 		   @pwmID[IN]--refer to 'pwm_id_e'.
 * 		   @isInvert[IN]--default true.
 * Return: TLK_ENONE is success, other value is failure.
*******************************************************************************/
int  tlkdev_pwmled_insert(uint08 ledID, uint32 ioPort, uint08 pwmID, bool isInvert);

/******************************************************************************
 * Function: tlkdev_pwmled_remove
 * Descript: Disable the gpio for the led, remove an led with a PWM function.
 * Params: @ledID[IN]--The ledID.
 * 		   @upDown[IN]--refer to 'gpio_pull_type_e'.
 * 		   @enOutput[IN]--enOutput, true enable output, false disable output.
 * Return: TLK_ENONE is success,other value is failure.
*******************************************************************************/
int  tlkdev_pwmled_remove(uint08 ledID, uint08 upDown, bool enOutput);

/******************************************************************************
 * Function: tlkdev_pwmled_isOn
 * Descript: Get the led switch status.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_pwmled_isOn(uint08 ledID);

/******************************************************************************
 * Function: tlkdev_pwmled_isAuto
 * Descript: Get the led control status.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_pwmled_isAuto(uint08 ledID);

/******************************************************************************
 * Function: tlkdev_pwmled_on
 * Descript: Turn on an led with a PWM function.
 * Params: @keledIDyID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_pwmled_on(uint08 ledID, uint16 pwmPeriodUs, uint16 pwmDutyUs);

/******************************************************************************
 * Function: tlkdev_pwmled_off
 * Descript: Turn off a led with a PWM function.
 * Params: @ledID[IN]--The ledID.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_pwmled_off(uint08 ledID);

/******************************************************************************
 * Function: tlkdev_pwmled_auto
 * Descript: Use PWM to control led breathing, can choose to keep the led on
 * 			 or off after the breathing is over.
 * Params: @ledID[IN]--The ledID.
 * 		   @firstOn[IN]--The state of the first work, true is on, false is off.
 * 		   @count[IN]--breath count.
 * 		   @onTimerMs[IN]--on time.
 * 		   @offTimerMs[IN]--off time.
 * 		   @pwmPeriodUs[IN]--One PWM cycle.
 * 		   @pwmDutyUs[IN]--Duty cycle.
 * 		   @dutyFlushUs[IN]--Duty cycle(pwmDutyUs) changes time.
 * 		   @stepUs[IN]--breath step.
 * 		   @isKeepOn[IN]--The status after the breath is over, true is on,
 * 		   				false is off.
 * Return: True is success, false is failure.
*******************************************************************************/
bool tlkdev_pwmled_auto(uint08 ledID, uint08 firstOn, uint16 count, uint16 onTimerMs, uint16 offTimerMs,
	 uint16 pwmPeriodUs, uint16 pwmDutyUs, uint16 dutyFlushUs, uint08 stepUs, uint08 isKeepOn);
#endif


#endif //#if (TLK_DEV_LED_ENABLE)

#endif //#ifndef TLKDRV_LED_H

