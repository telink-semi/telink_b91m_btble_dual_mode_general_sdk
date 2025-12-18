/********************************************************************************************************
 * @file	tlkmdi_auduac.h
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
#ifndef TLKMDI_UAC_H
#define TLKMDI_UAC_H



#if (TLK_MDI_AUDUAC_ENABLE)


/******************************************************************************
 * Function: tlkmdi_auduac_init
 * Descript: Initial the A2DP sink block, register the data callback. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_auduac_init(void);

int tlkmdi_auduac_start(uint16 handle, uint32 param);
int tlkmdi_auduac_close(uint16 handle);

void tlkmdi_auduac_timer(void);

/******************************************************************************
 * Function: tlkmdi_auduac_switch
 * Descript: Change UAC status. 
 * Params: @handle[IN]--The handle.
 *         @status[IN]--The status.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_auduac_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_auduac_isBusy
 * Descript: 
 * Params: None.
 * Return: true or false.
*******************************************************************************/
bool tlkmdi_auduac_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_auduac_intval
 * Descript: Get UAC Interval which will determine the time 
 *           when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_auduac_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_auduac_irqProc
 * Descript: Call UAC spk handler. 
 * Params: None.
 * Return: Return true or false.
*******************************************************************************/
bool tlkmdi_auduac_irqProc(void);




#endif //#if (TLK_MDI_AUDUAC_ENABLE)

#endif //TLKMDI_UAC_H

