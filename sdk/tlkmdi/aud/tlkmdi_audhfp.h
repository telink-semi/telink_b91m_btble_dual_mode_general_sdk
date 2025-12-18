/********************************************************************************************************
 * @file	tlkmdi_audhfp.h
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
#ifndef TLKMDI_AUD_HFP_H
#define TLKMDI_AUD_HFP_H

#if (TLK_MDI_AUDHFP_ENABLE)


#define TLKMDI_AUDHF_TIMEOUT                100000
#define TLKMDI_AUDHF_CALL_WAIT_TIMEOUT      (3000000/TLKMDI_AUDHF_TIMEOUT)



typedef struct{
	uint08 status;
	uint16 handle;
	uint08 timeout;
	tlkapi_timer_t timer;
}tlkmdi_audhfp_ctrl_t;



/******************************************************************************
 * Function: tlkmdi_audhfp_init.
 * Descript: Trigger to Initial the HF control block and register the callback.
 * Params: None.
 * Return: Return TLK_ENONE is success, other's value is false.
 * Others: None.
*******************************************************************************/
int tlkmdi_audhfp_init(void);

int tlkmdi_audhfp_start(uint16 handle);
int tlkmdi_audhfp_close(uint16 handle);

/******************************************************************************
 * Function: tlkmdi_audhfp_isBusy.
 * Descript: Get the HF control block is in-using.
 * Params: None.
 * Return: Return true or false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_audhfp_intval.
 * Descript: Get the HF control block interval.
 * Params: None.
 * Return: Return the interval.
 * Others: None.
*******************************************************************************/
uint tlkmdi_audhfp_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_audhfp_switch.
 * Descript: Trigger to Change the HF control block status.
 * Params: None.
 * Return: Return true or false.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_audhfp_irqProc.
 * Descript: Get the HF status.
 * Params: None.
 * Return: Return HF status.
 * Others: None.
*******************************************************************************/
bool tlkmdi_audhfp_irqProc(void);


#endif //#if (TLK_MDI_AUDHFP_ENABLE)

#endif //TLKMDI_AUD_HFP_H
