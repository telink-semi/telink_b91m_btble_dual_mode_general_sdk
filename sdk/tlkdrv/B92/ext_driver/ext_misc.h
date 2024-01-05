/********************************************************************************************************
 * @file     ext_misc.h
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

#ifndef DRIVERS_B91_EXT_MISC_H_
#define DRIVERS_B91_EXT_MISC_H_

#include "analog.h"
#include "clock.h"
#include "compiler.h"
#include "dma.h"
#include "flash.h"
#include "gpio.h"
#include "mdec.h"
#include "stimer.h"
#include "timer.h"
#include "uart.h"
#include "nds_intrinsic.h"

#include "types.h"
#include "analog.h"
#include "lib/include/pm.h"
#include "lib/include/trng.h"
#include "lib/include/sys.h"
#include "lib/include/plic.h"


/* for debug */
#define	DBG_SRAM_ADDR					0x00014

#define write_dbg32(addr, value)   		write_sram32(addr, value)

#define write_log32(err_code)   		write_sram32(0x00014, err_code)




/******************************* stimer_start ******************************************************************/
//#define	SYSTICK_NUM_PER_US				16
#define reg_system_tick_irq				reg_system_irq_level

typedef enum {
	STIMER_IRQ_MASK     		=   BIT(0),
	STIMER_32K_CAL_IRQ_MASK     =   BIT(1),
}stimer_irq_mask_e;

typedef enum {
	FLD_IRQ_SYSTEM_TIMER     		=   BIT(0),
}system_timer_irq_mask_e;


typedef enum {
	STIMER_IRQ_CLR	     		=   BIT(0),
	STIMER_32K_CAL_IRQ_CLR     	=   BIT(1),
}stimer_irq_clr_e;


/**
 * @brief    This function serves to enable system timer interrupt.
 * @return  none
 */
static inline void systimer_irq_enable(void)
{
	reg_irq_src0 |= BIT(IRQ1_SYSTIMER);
	//plic_interrupt_enable(IRQ1_SYSTIMER);
}

/**
 * @brief    This function serves to disable system timer interrupt.
 * @return  none
 */
static inline void systimer_irq_disable(void)
{
	reg_irq_src0 &= ~BIT(IRQ1_SYSTIMER);
	//plic_interrupt_disable(IRQ1_SYSTIMER);
}

static inline void systimer_set_irq_mask(void)
{
	reg_system_irq_mask |= STIMER_IRQ_MASK;
}

static inline void systimer_clr_irq_mask(void)
{
	reg_system_irq_mask &= (~STIMER_IRQ_MASK);
}

static inline unsigned char systimer_get_irq_status(void)
{
	return reg_system_cal_irq & FLD_IRQ_SYSTEM_TIMER;
}

static inline void systimer_clr_irq_status(void)
{
	reg_system_cal_irq = STIMER_IRQ_CLR;
}

static inline void systimer_set_irq_capture(unsigned int tick)
{
	reg_system_irq_level = tick;
}

static inline unsigned int systimer_get_irq_capture(void)
{
	return reg_system_irq_level;
}

static inline int tick1_exceed_tick2(unsigned int tick1, unsigned int tick2)
{
	return (unsigned int)(tick1 - tick2) < 0xc0000000; // 3/4 of max, about 201 second
}

static inline int tick1_exceed_tick2_half(unsigned int tick1, unsigned int tick2)//tick2 if too far of tick1,1/4 of max is not enough
{
	return (unsigned int)(tick1 - tick2) < 0x7fffffff; // 1/2 of max, about 89 second
}


/******************************* aes_start ******************************************************************/
extern unsigned int aes_data_buff[8];
/******************************* aes_end ********************************************************************/


unsigned int get_u32_safe (unsigned int v);

static inline int	clock_time_exceed_safe (unsigned int t, unsigned int us)
{
	unsigned int ts = get_u32_safe(t);
	return ts && ((unsigned int)(stimer_get_tick() - ts) > us * SYSTEM_TIMER_TICK_1US);
}

#define sys_timeout				clock_time_exceed
/******************************* stimer_end ********************************************************************/





/******************************* timer_start ******************************************************************/
/**
 * @brief	application hardware timer
 */
typedef struct {
	unsigned char en;
	unsigned char type;
}hw_timer_t;


extern	hw_timer_t			hw_timer_ctl;

/*
 * @brief   This function refer to get timer0/timer1 tick.
 * @param[in] type - timer0/timer1.
 * @return  none
 */
static inline unsigned int timer_get_tick(timer_type_e type)
{
	return reg_tmr_tick(type);
}

/**
 * @brief    This function serves to enable timer interrupt.
 * @return  none
 */
static inline void hw_timer_irq_enable(int  timer_src)
{
	reg_irq_src0 |= BIT(timer_src);
}

/**
 * @brief    This function serves to disable timer interrupt.
 * @return  none
 */
static inline void hw_timer_irq_disable(int  timer_src)
{
	reg_irq_src0 &= ~BIT(timer_src);
}
/******************************* timer_end ******************************************************************/





/******************************* core_start ******************************************************************/
#define  irq_disable		core_interrupt_disable
#define	 irq_enable			core_interrupt_enable
#define  irq_restore(en)	core_restore_interrupt(en)
/******************************* core_end ********************************************************************/



/******************************* analog_start ******************************************************************/
#define analog_write	analog_write_reg8
#define analog_read		analog_read_reg8

/******************************* analog_end ********************************************************************/



/******************************* trng_start ******************************************************************/
//#define rand						trng_rand
#define random_generator_init  		trng_init


/**
 * @brief     This function performs to generate a series of random numbers
 * @param[in]  len - data length
 * @param[out] data - data pointer
 * @return    none
 **/
void generateRandomNum(int len, unsigned char *data);

/******************************* trng_end ********************************************************************/



/******************************* sys_start ******************************************************************/
#define sleep_us(x)					delay_us(x)
#define sleep_ms(x)					delay_ms(x)


/******************************* sys_end ********************************************************************/






/******************************* dma_start ***************************************************************/



/**
 * @brief	ACL RX Data buffer length = maxRxOct + 21, then 16 Byte align
 *			maxRxOct + 21 = 4(DMA_len) + 2(BLE header) + maxRxOct + 4(MIC) + 3(CRC) + 8(ExtraInfo)
			RX buffer size must be be 16*n, due to MCU design
 */
#define 	CAL_LL_ACL_RX_FIFO_SIZE(maxRxOct)	(((maxRxOct+21) + 15) / 16 *16)


/**
 * @brief	ACL TX Data buffer length = maxTxOct + 10, then 16 Byte align
 *			maxTxOct + 10 = 4(DMA_len) + 2(BLE header) + maxTxOct + 4(MIC)
			TX buffer size must be be 16*n, due to MCU design
 */
#define 	CAL_LL_ACL_TX_FIFO_SIZE(maxTxOct)	(((maxTxOct+10) + 15) / 16 *16)


/*HCI TX RX buffer len = uart_fifo+ dma 4byte */
#define 	HCI_FIFO_SIZE(n)					(((n+2+4) + 15) / 16 *16)


/*
 * @brief	ISO RX Data buffer length = ISORxOct + 21, then 16 Byte align
 *			ISORxOct + 21 = 4(DMA_len) + 2(BLE header) + ISORxOct + 4(MIC) + 3(CRC) + 8(ExtraInfo)
 *			RX buffer size must be be 16*n, due to MCU design
 */
#define		CAL_LL_ISO_RX_FIFO_SIZE(n)			(((n + 21) + 15) / 16 * 16)


/*
 * @brief	ISO TX Data buffer length = ISOTxOct + 10, then 16 Byte align
 * 			ISORxOct + 10 = 4(DMA_len) + 2(BLE header) + ISOTxOct + 4(MIC)
 *			TX buffer size must be be 16*n, due to MCU design
 */
#define		CAL_LL_ISO_TX_FIFO_SIZE(n)			(((n + 10) + 15) / 16 * 16)


/*
* DMA_LEN(4B)+Hdr(2B)+PLD(251B)+MIC(4B)+CRC(3B)+TLK_PKT_INFO(12B)
*             **use 2B enough**
*/
#define		ISO_BIS_RX_PDU_SIZE_ALIGN16(n)			(((n + 25) + 15) / 16 * 16) //4+2+4+2+4+3+12

/******************************* dma_end ********************************************************************/



/**
 * @brief	6 = header(2)+l2cap_len(2)+CID(2)
 */
#define		CAL_L2CAP_BUFF_SIZE(n)				(((n + 6) + 3)/4 * 4)





/******************************* flash_start *****************************************************************/
_attribute_text_code_ unsigned int flash_get_jedec_id(void);
/**
 * @brief     flash capacity definition
 * Call flash_read_mid function to get the size of flash capacity.
 * Example is as follows:
 * unsigned char temp_buf[4];
 * flash_read_mid(temp_buf);
 * The value of temp_buf[2] reflects flash capacity.
 */
typedef enum {
	FLASH_CAPACITY_64K     = 0x10,
	FLASH_CAPACITY_128K    = 0x11,
	FLASH_CAPACITY_256K    = 0x12,
	FLASH_CAPACITY_512K    = 0x13,
	FLASH_CAPACITY_1M      = 0x14,
	FLASH_CAPACITY_2M      = 0x15,
	FLASH_CAPACITY_4M      = 0x16,
	FLASH_CAPACITY_8M      = 0x17,
} Flash_CapacityDef;
void flash_set_capacity(Flash_CapacityDef flash_cap);
Flash_CapacityDef flash_get_capacity(void);

/******************************* flash_end *******************************************************************/


/******************************* uart_start ******************************************************************/
/**
 * @brief     	uart0 receive data function, this  function tell the DMA to get data from the uart0 data fifo and start
 *            	the DMA transmission
 * @param[in]  	uart_num - UART0 or UART1
 * @param[in] 	Addr - pointer to the buffer  receive data
 * @param[in] 	len  - The length of receiver buffer.
 * @return     	none
 */
extern void uart_receive_dma_buffer_set(uart_num_e uart_num, unsigned char * Addr, unsigned int len);

void uart0_init(unsigned int baudrate);
/******************************* uart_end ********************************************************************/

extern void trng_init(void);
extern unsigned int trng_rand(void);


#endif /* DRIVERS_B91_EXT_MISC_H_ */
