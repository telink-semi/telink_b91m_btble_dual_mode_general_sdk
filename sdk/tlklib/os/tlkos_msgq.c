/********************************************************************************************************
 * @file	tlkos_msgq.c
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
#if (TLK_CFG_OS_ENABLE)
#include "tlklib/os/tlkos_stdio.h"
#include "tlklib/os/tlkos_msgq.h"
#if (TLK_OS_FREERTOS_ENABLE)
#include "tlklib/os/freertos/include/FreeRTOS.h"
#include "tlklib/os/freertos/include/task.h"
#include "tlklib/os/freertos/include/queue.h"
#endif


/******************************************************************************
 * Function: tlkos_msgq_create
 * Descript: Create a message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 *     @numb[IN]--Number of items in the queue.
 *     @size[IN]--The size of a single item.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: pMsgq must not be nullptr.
*******************************************************************************/
int tlkos_msgq_create(tlkos_msgq_t *pMsgq, uint numb, uint size)
{ 
	if(pMsgq == nullptr || numb == 0 || size == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	pMsgq->size = size;
	pMsgq->handle = (ulong)xQueueCreate(numb, size);
	if(pMsgq->handle == 0) return -TLK_EFAIL;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_msgq_destroy
 * Descript: Destroy message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 * Return: Operating results, TLK_ENONE means success and others mean failure.
 * Others: None.
*******************************************************************************/
int tlkos_msgq_destroy(tlkos_msgq_t *pMsgq)
{
	if(pMsgq == nullptr) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pMsgq->handle == 0) return -TLK_EPARAM;
	vQueueDelete((QueueHandle_t)(pMsgq->handle));
	pMsgq->size = 0;
	pMsgq->handle = 0;
	return TLK_ENONE;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_msgq_send
 * Descript: Sends a chunk of data to a message queue.
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 *     @pData[IN]--Data to be sent.
 *     @dataLen[IN]--The Length of data to be sent.
 *     @timeout[IN]--The maximum amount of time the task should block waiting
 *      for space to become available on the queue, should it already be full.
 *      The call will return immediately if this is set to 0 and the
 *      queue is full. Unit:us.
 * Return: Returns the length of the sent data. A negative value means failure.
 * Others: None.
*******************************************************************************/
int tlkos_msgq_send(tlkos_msgq_t *pMsgq, uint08 *pData, uint16 dataLen, uint32 timeout)
{
	if(pMsgq == nullptr || pData == nullptr || dataLen == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(dataLen > pMsgq->size) return -TLK_EOVERFLOW;
	if(timeout != 0 && timeout != portMAX_DELAY){
		if(timeout < 1000000/configTICK_RATE_HZ) timeout = 1;
		else timeout = (timeout*configTICK_RATE_HZ)/1000000;
	}
	BaseType_t ret = xQueueExtSend((QueueHandle_t)(pMsgq->handle), pData, dataLen,
		nullptr, 0, nullptr, 0, timeout);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}
int tlkos_msgq_extSend(tlkos_msgq_t *pMsgq, uint08 *pData0, uint16 data0Len, 
	uint08 *pData1, uint16 data1Len, uint08 *pData2, uint16 data2Len, uint32 timeout)
{
	if(pMsgq == nullptr || (data0Len+data1Len+data2Len) == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if((data0Len+data1Len+data2Len) > pMsgq->size) return -TLK_EOVERFLOW;
	if(timeout != 0 && timeout != portMAX_DELAY){
		if(timeout < 1000000/configTICK_RATE_HZ) timeout = 1;
		else timeout = (timeout*configTICK_RATE_HZ)/1000000;
	}
	BaseType_t ret = xQueueExtSend((QueueHandle_t)(pMsgq->handle), pData0, data0Len,
		pData1, data1Len, pData2, data2Len, timeout);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}
int tlkos_msgq_sendFromIrq(tlkos_msgq_t *pMsgq, uint08 *pData, uint16 dataLen)
{
	if(pMsgq == nullptr || pData == nullptr || dataLen == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(dataLen > pMsgq->size) return -TLK_EOVERFLOW;
	BaseType_t ret = xQueueExtSendFromISR((QueueHandle_t)(pMsgq->handle), pData, dataLen,
			nullptr, 0, nullptr, 0);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}
int tlkos_msgq_extSendFromIrq(tlkos_msgq_t *pMsgq, uint08 *pData0, uint16 data0Len, 
	uint08 *pData1, uint16 data1Len, uint08 *pData2, uint16 data2Len)
{
	if(pMsgq == nullptr || (data0Len+data1Len+data2Len) == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if((data0Len+data1Len+data2Len) > pMsgq->size) return -TLK_EOVERFLOW;
	BaseType_t ret = xQueueExtSendFromISR((QueueHandle_t)(pMsgq->handle), pData0, data0Len,
		pData1, data1Len, pData2, data2Len);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_msgq_wait
 * Descript: Read data from the message queue. 
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 *     @pBuff[OUT]--Data to be sent.
 *     @buffLen[IN]--The Length of data to be sent.
 *     @timeout[IN]--The maximum amount of time the task should block waiting
 *      for space to become available on the queue, should it already be full.
 *      The call will return immediately if this is set to 0 and the queue is 
 *      full. Unit:us.
 * Return: Returns the length of the read data. A negative value means failure.
 * Others: When the item is read, it will be discarded regardless of whether 
 *      the data has been read.
*******************************************************************************/
int tlkos_msgq_wait(tlkos_msgq_t *pMsgq, uint08 *pBuff, uint16 buffLen, uint32 timeout)
{
	if(pMsgq == nullptr || pBuff == nullptr || buffLen == 0) return -TLK_EPARAM;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pMsgq->handle == 0 || pMsgq->size == 0) return -TLK_ENOREADY;
	if(buffLen > pMsgq->size) buffLen = pMsgq->size;
	if(timeout != 0 && timeout != portMAX_DELAY){
		if(timeout < 1000000/configTICK_RATE_HZ) timeout = 1;
		else timeout = (timeout*configTICK_RATE_HZ)/1000000;
	}
	BaseType_t ret = xQueueExtReceive((QueueHandle_t)(pMsgq->handle), pBuff, buffLen, timeout);
	if(ret == pdTRUE) return TLK_ENONE;
	return -TLK_EFAIL;
#else
	return -TLK_ENOSUPPORT;
#endif
}

/******************************************************************************
 * Function: tlkos_msgq_clear
 * Descript: Reset a queue back to its original empty state. 
 * Params:
 *     @pMsgq[IN]--Message queue control structure.
 * Return: None.
*******************************************************************************/
void tlkos_msgq_clear(tlkos_msgq_t *pMsgq)
{
	if(pMsgq == nullptr) return;
#if (TLK_OS_FREERTOS_ENABLE)
	if(pMsgq->handle == 0 || pMsgq->size == 0) return;
	xQueueReset((QueueHandle_t)(pMsgq->handle));
#else
	
#endif
}




#endif //#if (TLK_CFG_OS_ENABLE)

