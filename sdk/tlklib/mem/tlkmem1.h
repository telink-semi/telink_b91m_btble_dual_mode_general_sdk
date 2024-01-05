/********************************************************************************************************
 * @file	tlkapi_mem1.h
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
#ifndef TLKMEM_MEM1_H
#define TLKMEM_MEM1_H



typedef ulong tlkmem_mem1_t;

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
tlkmem_mem1_t tlkmem1_init(bool enIrqMask, bool enSecMask, uint08 *pBuffer, uint32 buffLen);

/******************************************************************************
 * Function: tlkmem1_deinit
 * Descript: Uninitialization the memory manager.
 * Params:
 *     @mem[IN]--Memory management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmem1_deinit(tlkmem_mem1_t mem);

/******************************************************************************
 * Function: tlkmem1_clean
 * Descript: Reset the memory manager.
 * Params:
 *     @mem[IN]--Memory management handle.
 * Return: None.
 * Others: None.
*******************************************************************************/
void tlkmem1_clean(tlkmem_mem1_t mem);

void tlkmem1_print(tlkmem_mem1_t mem);

/******************************************************************************
 * Function: tlkmem1_malloc
 * Descript: Allocates a segment of memory from the specified memory pool.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @size[IN]--The size to allocate
 * Return: The first address of memory allocated, nullptr means failure.
 * Others: Cannot be invoked across threads or processes.
*******************************************************************************/
void *tlkmem1_malloc(tlkmem_mem1_t mem, uint32 size);

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
void *tlkmem1_calloc(tlkmem_mem1_t mem, uint32 size);

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
void *tlkmem1_realloc(tlkmem_mem1_t mem, void *ptr, uint32 size);

/******************************************************************************
 * Function: tlkmem1_free
 * Descript: Return the occupied memory to the memory manager.
 * Params:
 *     @mem[IN]--Memory management handle.
 *     @ptr[IN]--The first memory address that needs to be freed.
 * Return: None.
 * Others: Cannot be invoked across threads or processes.
*******************************************************************************/
int tlkmem1_free(tlkmem_mem1_t mem, void *ptr);



#endif //TLKMEM_MEM1_H

