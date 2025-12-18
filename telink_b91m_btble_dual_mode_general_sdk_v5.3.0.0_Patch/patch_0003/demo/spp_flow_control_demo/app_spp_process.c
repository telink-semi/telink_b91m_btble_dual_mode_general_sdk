#include "btble.h"
#include "bp_uart.h"
#include "spp_serial.h"
#include "tlkdrv/B91/gpio.h"
#include "tlkdrv/B91/uart.h"
#include "string.h"
#include "tlkstk/bt/btc/btc_hci.h"
#include "tlkdrv/B91/compatibility_pack/cmpt.h"
#include "tlkstk/bt/bth/bth_event.h"
#include "tlkstk/bt/bth/bth_acl.h"
#include <stdio.h>
#include "tlkstk/bt/bth/bth_hcicmd.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkstk/bt/btp/btp_stdio.h"
#include "tlkstk/inner/tlkstk_inner.h"

uint32_t app_spp_rx_total_length = 0,app_uart_tx_total_length = 0;


MYFIFO_INIT_IRAM(sBtSPPRxFifo, TLKBT_APP_SPP_FIFO_SIZE, TLKBT_APP_SPP_FIFO_NUMB);

extern uint16 app_spp_handle;

bool tlk_app_spp_RxFifoIsFull(void)
{
	uint8_t n = sBtSPPRxFifo.wptr - sBtSPPRxFifo.rptr;
	return n >= sBtSPPRxFifo.num;
}
bool tlk_app_spp_RxFifoIsEmpty(void)
{
	return sBtSPPRxFifo.wptr == sBtSPPRxFifo.rptr;
}
bool tlk_app_spp_RxFifoIsHalfFull(void)
{
	uint8_t n = sBtSPPRxFifo.wptr - sBtSPPRxFifo.rptr;
	return n >= (sBtSPPRxFifo.num/2);
}
uint32_t tlk_app_spp_RxFifoSize(void)
{
	if(sBtSPPRxFifo.size <= 4) return 0;
	else return (sBtSPPRxFifo.size-4);
}
uint32_t tlk_app_spp_RxFifoNumb(void)
{
	return sBtSPPRxFifo.num;
}
uint32_t tlk_app_spp_RxFifoUsed(void)
{
	return (uint8_t)(sBtSPPRxFifo.wptr - sBtSPPRxFifo.rptr);
}
uint32_t tlk_app_spp_RxFifoUnused(void)
{
	return sBtSPPRxFifo.num-tlk_app_spp_RxFifoUsed();
}
uint32_t tlk_app_spp_RxDatFifoUsed(void)
{
	return (uint8_t)(sBtSPPRxFifo.wptr - sBtSPPRxFifo.rptr);
}

void spp_rx_callback_2(uint16 aclHandle, uint08 rfcHandle, uint08 *pData, uint16 dataLen)
{
	uint8_t *pBuffer;
	if (tlk_app_spp_RxFifoIsFull()) {

		 tlkapi_trace(0xffffffff, "app_spp", "spp_rx_callback_2 error overflow");
		 return;
	}
	pBuffer = sBtSPPRxFifo.p + (sBtSPPRxFifo.wptr  & (sBtSPPRxFifo.num-1)) * sBtSPPRxFifo.size;
    pBuffer[0] = dataLen&0xff;
    pBuffer[1] = (dataLen>>8)&0xff;



    tmemcpy(pBuffer+2,pData,dataLen);
    app_spp_rx_total_length += dataLen;

    tlkapi_trace(0xffffffff, "RX##", "spp_rx_callback_2 rl %d,len %d,fifoUsed %d,buffer:0x%x,wp:%d,rp:%d ,sBtSPPRxFifo.num %d, wr %d %d end",
    		dataLen,((uint16_t)pBuffer[1] << 8) | pBuffer[0],
			tlk_app_spp_RxFifoUsed(),
			pBuffer,
			sBtSPPRxFifo.wptr & (sBtSPPRxFifo.num-1),
			sBtSPPRxFifo.rptr & (sBtSPPRxFifo.num-1),
			sBtSPPRxFifo.num,sBtSPPRxFifo.wptr,sBtSPPRxFifo.wptr);
    sBtSPPRxFifo.wptr++;
}
#if 0
void print_spp_data2(void)
{

	uint8_t *pBuffer;
	uint16 dataLen;
	if(sBtSPPRxFifo.rptr == sBtSPPRxFifo.wptr) return;

	pBuffer = sBtSPPRxFifo.p + (sBtSPPRxFifo.rptr  & (sBtSPPRxFifo.num-1)) * sBtSPPRxFifo.size;
	dataLen = ((uint16_t)pBuffer[1] << 8) | pBuffer[0];
    for(int i = 0; i < dataLen; i++)
    {
    	uart_send_byte(UART1, pBuffer[i+2]);
    }
    app_uart_tx_total_length += dataLen;

    sBtSPPRxFifo.rptr ++;
    btp_spp_update_rx_credits_consumed(app_spp_handle, 1);
}
#else
static uint8_t current_send_buffer[TLKBT_APP_SPP_FIFO_SIZE+2];
static volatile uint16 index = 0;
static volatile uint16 dataLen = 0;
uint8_t new_fifo = 1;
void print_spp_data2(void)
{
    if (new_fifo) {
    	uint8_t *pBuffer;
    	if(sBtSPPRxFifo.rptr == sBtSPPRxFifo.wptr) {
    		return;
    	}

    	pBuffer = sBtSPPRxFifo.p + (sBtSPPRxFifo.rptr  & (sBtSPPRxFifo.num-1)) * sBtSPPRxFifo.size;
    	dataLen = ((uint16_t)pBuffer[1] << 8) | pBuffer[0];
    	index = 0;
    	new_fifo = 0;
    	tmemcpy(current_send_buffer, pBuffer+2, dataLen);


        tlkapi_trace(0xffffffff, "TX##", "rx_len %d,fifoUsed %d,buffer 0x%x,wp %d,rp %d sBtSPPRxFifo.num %d,wr %d,%d end",
        		dataLen, tlk_app_spp_RxFifoUsed(), pBuffer,
				sBtSPPRxFifo.wptr  & (sBtSPPRxFifo.num-1),
				sBtSPPRxFifo.wptr  & (sBtSPPRxFifo.num-1),
				sBtSPPRxFifo.num,sBtSPPRxFifo.wptr,sBtSPPRxFifo.wptr);

        sBtSPPRxFifo.rptr ++;
    	btp_spp_update_rx_credits_consumed(app_spp_handle, 1);

    } else {
    	uart_send_byte(UART1, current_send_buffer[index]);
    	current_send_buffer[index] = 0x41;
    	index++;
    	if (dataLen == index) {
    		new_fifo = 1;
    	}
    	app_uart_tx_total_length++;
    }


}
#endif

