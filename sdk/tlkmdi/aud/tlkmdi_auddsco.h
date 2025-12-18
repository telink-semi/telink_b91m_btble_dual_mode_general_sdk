/********************************************************************************************************
 * @file     tlkmdi_auddsco.h
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

#ifndef TLKMDI_AUDDSCO_H
#define TLKMDI_AUDDSCO_H

#if (TLK_MDI_AUDDSCO_ENABLE)


#define TLKMDI_DSCO_SPK_ENC_BUFF_NUMB        16
#define TLKMDI_DSCO_SPK_ENC_BUFF_SIZE        64

#define TLKMDI_DSCO_MIC_ENC_BUFF_NUMB        16
#define TLKMDI_DSCO_MIC_ENC_BUFF_SIZE        64

#define TLKMDI_DSCO_MAX_NUMB                 2

#define TLKMDI_DSCO_TEMP_BUFFER_SIZE         256

#define TLKMDI_DSCO_CVSD_PARAM_SIZE          TLKALG_CVSD_PARAM_SIZE
#define TLKMDI_DSCO_CVSD_CACHE_SIZE          TLKALG_CVSD_CACHE_SIZE
#define TLKMDI_DSCO_CVSD_BUFFER_SIZE         (TLKMDI_DSCO_CVSD_PARAM_SIZE+TLKMDI_DSCO_CVSD_CACHE_SIZE)

#define TLKMDI_DSCO_SBC_ENC_BUFF_SIZE        3368
#define TLKMDI_DSCO_SBC_ENC_BUFF_OFFS        0
#define TLKMDI_DSCO_SBC_DEC_BUFF_SIZE        1248
#define TLKMDI_DSCO_SBC_DEC_BUFF_OFFS        TLKMDI_DSCO_SBC_ENC_BUFF_SIZE
#define TLKMDI_DSCO_SBC_BUFFER_SIZE          (TLKMDI_DSCO_SBC_ENC_BUFF_SIZE+TLKMDI_DSCO_SBC_DEC_BUFF_SIZE)

#define TLKMDI_DSCO_ENCODE_BUFFER_SIZE       (TLKMDI_DSCO_SBC_BUFFER_SIZE) //max(TLKMDI_DSCO_SBC_BUFFER_SIZE,TLKMDI_DSCO_CVSD_BUFFER_SIZE)
#define TLKMDI_DSCO_CACHE_BUFFER_SIZE        (TLKMDI_DSCO_SPK_ENC_BUFF_SIZE*TLKMDI_DSCO_SPK_ENC_BUFF_NUMB\
	+TLKMDI_DSCO_MIC_ENC_BUFF_SIZE*TLKMDI_DSCO_MIC_ENC_BUFF_NUMB)

//Total buffer = CACHE(64*16+64*16)+TEMP(256)+CVSD(1664)+SBC(3368+1248)
//             = 2048+256+1664+4616 = 8564+3*16 = 8612

#define TLKMDI_DSCO_CODEC_ID_CVSD          1
#define TLKMDI_DSCO_CODEC_ID_MSBC          2


typedef void(*TlkMdiDscoConnCB)(uint16 aclHandle, uint16 scoHandle, bool isConn);


typedef int(*TlkMdiDscoDecFunc)(uint08 *pSrc, uint16 srcLen, uint08 *pOut);
typedef int(*TlkMdiDscoEncFunc)(uint08 index,uint08 *pSrc, uint16 srcLen, uint08 *pOut);
typedef int(*TlkMdiDscoPlcFunc)(uint08 *pSrc, uint16 srcLen, uint08 *pOut);


typedef struct{
	uint08 *pTempBuffer;
	uint08 *pCacheBuffer;
	uint08 *pEncodeBuffer;
}tlkmdi_dsco_buff_t;



typedef struct{
	uint08 numb;
	uint08 enable;
	uint08 dropSpkNumb;
	uint08 dropMicNumb;
	uint16 aclHandle;
	uint16 scoHandle;
	uint16 spkBuffLen;
	uint16 micBuffLen;
	uint16 spkReadOffs;
	uint16 micReadOffs;
	uint08 codec;
	uint08 resv;
	TlkMdiDscoDecFunc dec_func;
	TlkMdiDscoEncFunc enc_func;
	TlkMdiDscoPlcFunc plc_func;
}tlkmdi_dsco_ctrl_t;

typedef struct{
	uint08 role; //0-hf 1-ag
	uint16 aclHandle;
	uint16 scoHandle;
}tlkmdi_dsco_handle_t;

/******************************************************************************
 * Function: tlkmdi_auddsco_init
 * Descript: Initial the Sco block, register the Sco data callback. and register 
 *           connect and disconnect or codec callback to listen the sco status. 
 * Params: None.
 * Return: TLK_ENONE is success,other value is false.
*******************************************************************************/
int tlkmdi_auddsco_init(void);

/******************************************************************************
 * Function: tlkmdi_auddsco_enable
 * Descript:  Enable auddsco.
 * Params: 
 *        @pData[IN]--Include the aclhandle,scohandle and codec type.
 *        @dataLen[IN]--The dataLen.
 * Return: Whether the auddsco is enable.
*******************************************************************************/
int tlkmdi_auddsco_enable(uint08 *pData,uint16 dataLen);

/******************************************************************************
 * Function: tlkmdi_auddsco_disable
 * Descript: Disable the audsco. 
 * Params: None.
 * Return: Whether the auddsco is disable.
*******************************************************************************/
int tlkmdi_auddsco_disable();

/******************************************************************************
 * Function: tlkmdi_auddsco_regCB
 * Descript: Register SCO connection status switch callback. 
 * Params: None.
 * Return: None.
*******************************************************************************/
void tlkmdi_auddsco_regCB(TlkMdiDscoConnCB connCB);

/******************************************************************************
 * Function: tlkmdi_auddsco_init
 * Descript: verify the sco is enable. 
 * Params: None.
 * Return: true is success,other value is false.
*******************************************************************************/
bool tlkmdi_auddsco_isBusy(void);

/******************************************************************************
 * Function: tlkmdi_auddsco_intval
 * Descript: Get the sco interval which will determine when to start the next timer. 
 * Params: None.
 * Return: The interval value.
*******************************************************************************/
uint tlkmdi_auddsco_intval(void); //Interval

/******************************************************************************
 * Function: tlkmdi_auddsco_switch
 * Descript: Change the sco status. 
 * Params:
 *        @handle[IN]--The handle
 *        @status[IN]--The status.
 * Return: The interval value.
*******************************************************************************/
bool tlkmdi_auddsco_switch(uint16 handle, uint08 status);

/******************************************************************************
 * Function: tlkmdi_auddsco_irqProc
 * Descript: call the mic handler and spkhandler. 
 * Params: None.
 * Return: The true/false.
*******************************************************************************/
bool tlkmdi_auddsco_irqProc(void);



#endif //#if (TLK_MDI_AUDDSCO_ENABLE)


#endif //TLKMDI_AUDDSCO_H

