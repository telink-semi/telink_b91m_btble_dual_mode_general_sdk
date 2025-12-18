/********************************************************************************************************
 * @file	tlkapi_mem1.c
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
#include "tlkapi/tlkapi_mem.h"
#include "drivers.h"
#include "tlklib/mem/tlkmem1.h"
#include "tlklib/mem/tlkmem_config.h"


#define TLKMEM1_COMM_MIN_SIZE          512 //Bytes

#ifndef TLKMEM1_COMM_ALIGN_SIZE
#define TLKMEM1_COMM_ALIGN_SIZE        4
#endif

#define TLKMEM1_COMM_HEAD_SIZE         12

typedef struct{
	uint32 prev;
	uint32 next;
	uint32 size:31;
	uint32 used:1;
}tlkmem1_unit_t;

typedef struct{
	uint08 protect;
	uint08 reserve0;
	uint16 reserve1;
	uint32 totalSize;//Memory capacity
	uint32 unuseSize;//Current remaining memory size
	uint08 *pBaseAddr; //Memory base address
}tlkmem1_ctrl_t;


static tlkmem1_unit_t *tlkmem1_get_unit(tlkmem1_ctrl_t *pCtrl, void *ptr);


/******************************************************************************
 * Function: tlkmem1_init
 * Descript: Initializes the memory manager.
 * Params:
 *     @enIrqMask[IN]--Whether to enable interrupt protection. After this
 *         function is enabled, interrupts at key points are masked.
 *     @enSecMask[IN]--Whether to enable the critical section protection. After
 *         the critical section protection is enabled, the critical section
 *         function is called at key points.
 *     @pBuffer[IN]--Memory to be managed.
 *     @buffLen[IN]--The size of memory to be managed.
 * Return: Memory management handle, and zero means failure.
 * Others: None.
*******************************************************************************/
tlkmem_mem1_t tlkmem1_init(bool enIrqMask, bool enSecMask, uint08 *pBuffer, uint32 buffLen)
{
	ulong sAddr;
	ulong eAddr;
	tlkmem1_ctrl_t *pCtrl;

	if(pBuffer == nullptr || buffLen<TLKMEM1_COMM_MIN_SIZE){
		return 0;
	}

	sAddr = ((ulong)pBuffer+(TLKMEM1_COMM_ALIGN_SIZE-1))&(~(TLKMEM1_COMM_ALIGN_SIZE-1));
	eAddr = ((ulong)pBuffer+buffLen)&(~(TLKMEM1_COMM_ALIGN_SIZE-1));

	pCtrl = (tlkmem1_ctrl_t*)(sAddr);
	tmemset(pCtrl, 0, TLKMEM1_COMM_HEAD_SIZE);

	if(enIrqMask) pCtrl->protect = 1;
	else if(enSecMask) pCtrl->protect = 2;
	else pCtrl->protect = 0;
	pCtrl->pBaseAddr = (uint08*)(pCtrl+1);
	pCtrl->totalSize = (eAddr-sAddr)-sizeof(tlkmem1_ctrl_t);

	tlkmem1_clean((tlkmem_mem1_t)pCtrl);

	return (tlkmem_mem1_t)pCtrl;
}

/******************************************************************************
 * Function: tlkmem1_deinit
 * Descript: Uninitialization the memory manager.
 * Params:
 *     @mem[IN]--Memory management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmem1_deinit(tlkmem_mem1_t mem)
{
	if(mem == 0) return;
	tlkmem1_clean(mem);
}

/******************************************************************************
 * Function: tlkmem1_clean
 * Descript: Reset the memory manager.
 * Params:
 *     @mem[IN]--Memory management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmem1_clean(tlkmem_mem1_t mem)
{
	tlkmem1_unit_t *pUnit;
	tlkmem1_ctrl_t *pCtrl;

	if(mem == 0) return;
	pCtrl = (tlkmem1_ctrl_t*)mem;
	if(pCtrl->protect == 1) tlksys_disable_interrupt();
	else if(pCtrl->protect == 2) tlksys_enter_critical();
	pUnit = (tlkmem1_unit_t*)pCtrl->pBaseAddr;
	pUnit->prev = 0;
	pUnit->next = 0;
	pUnit->used = 0;
	pUnit->size = pCtrl->totalSize-TLKMEM1_COMM_HEAD_SIZE;
	pCtrl->unuseSize = pUnit->size;
	if(pCtrl->protect == 1) tlksys_restore_interrupt();
	else if(pCtrl->protect == 2) tlksys_leave_critical();
}

void tlkmem1_print(tlkmem_mem1_t mem)
{
#if (TLK_DBG_LOG_ENABLE)
	uint32 caddr = 0;
	uint16 index = 0;
	tlkmem1_unit_t *pUnit;
	tlkmem1_ctrl_t *pCtrl;

	if(mem == 0) return;
	pCtrl = (tlkmem1_ctrl_t*)mem;
	tlkapi_trace(TLKMEM_DBG_FLAG, TLKMEM_DBG_SIGN, "tlkmem1_print:{addr-0x%x, total-%d, unuse-%d}",
		pCtrl->pBaseAddr, pCtrl->totalSize, pCtrl->unuseSize);
	pUnit = (tlkmem1_unit_t*)pCtrl->pBaseAddr;
	while(pUnit != nullptr){
//		tlkapi_trace(TLKMEM_DBG_FLAG, TLKMEM_DBG_SIGN, "  index:{index-%d, addr-0x%x, caddr-0x%x}", index++, pUnit, caddr, 0);
//		tlkapi_trace(TLKMEM_DBG_FLAG, TLKMEM_DBG_SIGN, "    {used-%d, size-%d, prev-0x%x, next-0x%x}",
//				pUnit->used, pUnit->size, pUnit->prev, pUnit->next);

		tlkapi_trace(TLKMEM_DBG_FLAG, TLKMEM_DBG_SIGN, "  index:{index-%d, addr-0x%x, prev-0x%x, next-0x%x}", index++, caddr, pUnit->prev, pUnit->next);

		caddr = pUnit->next;
		if(pUnit->next == 0) pUnit = nullptr;
		else pUnit = (tlkmem1_unit_t*)(pCtrl->pBaseAddr+pUnit->next);
	}
#endif
}

/******************************************************************************
 * Function: tlkmem1_malloc
 * Descript: Allocates a segment of memory from the specified memory pool.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @size[IN]--The size to allocate
 * Return: The first address of memory allocated, nullptr means failure.
 * Others: Cannot be invoked across threads or processes.
*******************************************************************************/
void *tlkmem1_malloc(tlkmem_mem1_t mem, uint32 size)
{
	uint32 curAddr;
	uint08 *pBuffer;
	tlkmem1_unit_t *pTemp;
	tlkmem1_unit_t *pUnit;
	tlkmem1_ctrl_t *pCtrl;

	if(mem == 0 || size == 0) return nullptr;

	curAddr = 0;
	pBuffer = nullptr;
	pCtrl = (tlkmem1_ctrl_t*)mem;
	if(pCtrl->protect == 1) tlksys_disable_interrupt();
	else if(pCtrl->protect == 2) tlksys_enter_critical();
	size = ((size+3)&(~0x03));
	pUnit = (tlkmem1_unit_t*)(pCtrl->pBaseAddr);
	while(pUnit != nullptr){
		if(pUnit->used == 0 && pUnit->size >= size+TLKMEM1_COMM_HEAD_SIZE) break;
		curAddr = pUnit->next;
		if(pUnit->next == 0) pUnit = nullptr;
		else pUnit = (tlkmem1_unit_t*)(pCtrl->pBaseAddr+pUnit->next);
	}
	if(pUnit != nullptr){
		uint08 *ptr = ((uint08*)pUnit);
		pTemp = (tlkmem1_unit_t*)(ptr+(size+TLKMEM1_COMM_HEAD_SIZE));
		pTemp->used = 0;
		pTemp->size = pUnit->size-(TLKMEM1_COMM_HEAD_SIZE+size);
		pTemp->next = pUnit->next;
		pTemp->prev = curAddr;
		pUnit->used = 1;
		pUnit->size = size;
		pUnit->next = curAddr+TLKMEM1_COMM_HEAD_SIZE+size;
		pBuffer = ((uint08*)pUnit)+TLKMEM1_COMM_HEAD_SIZE;
		pCtrl->unuseSize -= size+TLKMEM1_COMM_HEAD_SIZE;
	}
	if(pCtrl->protect == 1) tlksys_restore_interrupt();
	else if(pCtrl->protect == 2) tlksys_leave_critical();

	return pBuffer;
}

/******************************************************************************
 * Function: tlkmem1_calloc
 * Descript: Allocates a segment of memory from the specified memory pool.
 *           And reset all variables in memory to zero.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @size[IN]--The size to allocate.
 * Return: The first address of memory allocated, nullptr means failure.
 * Others: Cannot be invoked across threads or processes.
*******************************************************************************/
void *tlkmem1_calloc(tlkmem_mem1_t mem, uint32 size)
{
	uint08 *pTemp;

	pTemp = (uint08 *)tlkmem1_malloc(mem, size);
	if(nullptr == pTemp) return nullptr;

	tmemset(pTemp, 0, size);

	return (void *)pTemp;
}

/******************************************************************************
 * Function: tlkmem1_realloc
 * Descript: A segment of memory that is used to store the contents of existing memory.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @ptr[IN]--The first memory address that needs to be reallocated.
 *     @size[IN]--The size to allocate.
 * Return: The first address of memory allocated, nullptr means failure.
 * Others:
 *     1.Cannot be invoked across threads or processes.
 *     2.If allocation fails, the original memory address remains unchanged.
 *     3.If the allocation is successful, the existing memory is freed.
 *     4.If the allocation is successful, the contents of existing memory are
 *       copied into the new memory.
*******************************************************************************/
void *tlkmem1_realloc(tlkmem_mem1_t mem, void *ptr, uint32 size)
{
	uint08 *pTemp;

	if(mem == 0 || ptr == nullptr || size == 0) return nullptr;

	pTemp = (uint08*)tlkmem1_malloc(mem, size);
	if(pTemp != nullptr) tlkmem1_free(mem, ptr);
	return pTemp;
}

/******************************************************************************
 * Function: tlkmem1_free
 * Descript: Return the occupied memory to the memory manager.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @ptr[IN]--The first memory address that needs to be freed.
 * Return: None.
 * Others: Cannot be invoked across threads or processes.
*******************************************************************************/
int tlkmem1_free(tlkmem_mem1_t mem, void *ptr)
{
	tlkmem1_ctrl_t *pCtrl;
	tlkmem1_unit_t *pUnit;
	tlkmem1_unit_t *pPrev;
	tlkmem1_unit_t *pNext;

	if(mem == 0 || ptr == nullptr) return -TLK_EPARAM;

	pCtrl = (tlkmem1_ctrl_t*)mem;
	if(pCtrl->protect == 1) tlksys_disable_interrupt();
	else if(pCtrl->protect == 2) tlksys_enter_critical();
	pUnit = tlkmem1_get_unit(pCtrl, ptr);
	pUnit->used = 0;
	//Check Next
	if(pUnit->next == 0) pNext = nullptr;
	else pNext = (tlkmem1_unit_t*)(pCtrl->pBaseAddr+pUnit->next);
	pCtrl->unuseSize += pUnit->size;
	if(pNext != nullptr && pNext->used == 0){
		pUnit->next = pNext->next;
		pUnit->size += TLKMEM1_COMM_HEAD_SIZE+pNext->size;
		pCtrl->unuseSize += TLKMEM1_COMM_HEAD_SIZE;
		if(pUnit->next != 0){
			pNext = (tlkmem1_unit_t*)(pCtrl->pBaseAddr+pUnit->next);
			pNext->prev = (uint32)((ulong)pUnit-(ulong)pCtrl->pBaseAddr);
		}
	}
	//Check Prev
	pPrev = (tlkmem1_unit_t*)(pCtrl->pBaseAddr+pUnit->prev);
	if(pPrev->used == 0 && pPrev != pUnit){
		pPrev->next = pUnit->next;
		pPrev->size += TLKMEM1_COMM_HEAD_SIZE+pUnit->size;
		pCtrl->unuseSize += TLKMEM1_COMM_HEAD_SIZE;
		if(pPrev->next != 0){
			pNext = (tlkmem1_unit_t*)(pCtrl->pBaseAddr+pPrev->next);
			pNext->prev = (uint32)((ulong)pPrev-(ulong)pCtrl->pBaseAddr);
		}
	}
	if(pCtrl->protect == 1) tlksys_restore_interrupt();
	else if(pCtrl->protect == 2) tlksys_leave_critical();

	return TLK_ENONE;
}

/******************************************************************************
 * Function: tlkmem1_get_node
 * Descript: Gets the memory management unit for the specified memory.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @ptr[IN]--The first address of occupied memory.
 * Return: Memory management unit.
 * Others: None.
*******************************************************************************/
static tlkmem1_unit_t *tlkmem1_get_unit(tlkmem1_ctrl_t *pCtrl, void *ptr)
{
	ulong addr0;
	ulong addr1;
	tlkmem1_unit_t *pUnit;

	if(pCtrl == nullptr || ptr == nullptr) return nullptr;

	pUnit = (tlkmem1_unit_t*)((uint08*)ptr-sizeof(tlkmem1_unit_t));

	addr0 = (ulong)pUnit;
	addr1 = (ulong)pCtrl->pBaseAddr;
	if(addr0 < addr1 || addr0 > addr1+pCtrl->totalSize){
		return nullptr;
	}

	return pUnit;
}

