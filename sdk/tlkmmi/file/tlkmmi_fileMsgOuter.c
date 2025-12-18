/********************************************************************************************************
 * @file	tlkmmi_fileMsgOuter.c
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
#if (TLKMMI_FILE_ENABLE)
#include "tlksys/tlksys_stdio.h"
#include "tlkmmi_file.h"
#include "tlkmmi_fileMsgOuter.h"
#include "tlkmdi/misc/tlkmdi_file.h"


int tlkmmi_file_outerMsgHandler(uint16 msgID, uint08 *pData, uint08 dataLen)
{
	uint08 headLen;
	uint08 header[8];
	
	if(dataLen < 2){
		tlkapi_error(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: error length[%d]", dataLen);
		return -TLK_EPARAM;
	}
	
	headLen = 0;
	header[headLen++] = TLKPRT_COMM_PTYPE_CMD; //Cmd Packet
	header[headLen++] = TLKPRT_COMM_MTYPE_FILE;
	header[headLen++] = msgID;
	header[headLen++] = 0x00;
	header[headLen++] = dataLen;
	header[headLen++] = pData[0];
	header[headLen++] = pData[1];
	if(header[4] < 2 || header[4] > dataLen){
		tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: error length");
		return -TLK_EPARAM;
	}
	
	tlkapi_trace(TLKMMI_FILE_DBG_FLAG, TLKMMI_FILE_DBG_SIGN, "tlkmmi_file_recvCmdHandler: msgID-%d", msgID);
	#if (TLK_MDI_FILE_ENABLE)
	tlkmdi_file_recvHandler(TLKMDI_FILE_OPTCHN_SERIAL, 0xFFFF, header, headLen, pData+2, dataLen-2);
	#endif
	return TLK_ENONE;
}





#endif //#if (TLKMMI_FILE_ENABLE)

