/********************************************************************************************************
 * @file	tlkusb_udbDefine.h
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
#ifndef TLKUSB_UDB_DEFINE_H
#define TLKUSB_UDB_DEFINE_H

#if (TLK_USB_UDB_ENABLE)



#define TLKUSB_UDB_VCD_ENABLE        1


typedef enum{
	TLKUSB_UDB_INF_DBG,
	#if (TLKUSB_UDB_VCD_ENABLE)
	TLKUSB_UDB_INF_VCD,
	#endif
	TLKUSB_UDB_INF_MAX,
}TLKUSB_UDB_INF_ENUM;

typedef enum{
	TLKUSB_UDB_EDP_DBG_IN  = 3,
	TLKUSB_UDB_EDP_DBG_OUT = 5,
	TLKUSB_UDB_EDP_VCD_IN  = 8,
	TLKUSB_UDB_EDP_VCD_OUT = 6,
}TLKUSB_UDB_EDP_ENUM;


typedef enum{
	TLKUSB_PRNT_CSCP_PrinterClass = 0x07,
	TLKUSB_PRNT_CSCP_PrinterSubclass = 0x01,
	TLKUSB_PRNT_CSCP_BidirectionalProtocol = 0x02,
}TLKUSB_PRINT_CSCP_ENUM; //CSCP-ClassSubClassProtocol



#endif //#if (TLK_USB_UDB_ENABLE)

#endif //TLKUSB_UDB_DEFINE_H

