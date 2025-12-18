/********************************************************************************************************
 * @file	tlkmmi_rdt_leAtt.h
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
#ifndef TLKMMI_RDT_LEATT_H
#define TLKMMI_RDT_LEATT_H

#if (TLK_TEST_RDT_ENABLE)


////////////////////////////////////////// slave-role ATT service concerned ///////////////////////////////////////////////
typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  uint16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  uint16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  uint16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  uint16 timeout;
}tlkmmi_rdt_leConnParam_t;



///////////////////////////////////// slave-role ATT service HANDLER define ///////////////////////////////////////
typedef enum
{
	RDT_LEATT_START = 0,
	
	//// Gap ////
	/**********************************************************************************************/
	RDT_LEATT_GAP_START, 					//UUID: 2800, 	VALUE: uuid 1800
	RDT_LEATT_GAP_DEV_NAME_CTRL,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	RDT_LEATT_GAP_DEV_NAME_DATA,			//UUID: 2A00,   VALUE: device name
	RDT_LEATT_GAP_APPEARANCE_CTRL,			//UUID: 2803, 	VALUE:  			Prop: Read
	RDT_LEATT_GAP_APPEARANCE_DATA,			//UUID: 2A01,	VALUE: appearance
	RDT_LEATT_GAP_CONN_PARAM_CTRL,						//UUID: 2803, 	VALUE:  			Prop: Read
	RDT_LEATT_GAP_CONN_PARAM_DATA,						//UUID: 2A04,   VALUE: connParameter

	//// gatt ////
	/**********************************************************************************************/
	RDT_LEATT_GATT_START,					//UUID: 2800, 	VALUE: uuid 1801
	RDT_LEATT_GATT_SERVICE_CHANGED_CTRL,	//UUID: 2803, 	VALUE:  			Prop: Indicate
	RDT_LEATT_GATT_SERVICE_CHANGED_DATA,   //UUID:	2A05,	VALUE: service change
	RDT_LEATT_GATT_SERVICE_CHANGED_CCC,	//UUID: 2902,	VALUE: serviceChangeCCC
	
	//// device information ////
	/**********************************************************************************************/
	RDT_LEATT_DINFO_START,					//UUID: 2800, 	VALUE: uuid 180A
	RDT_LEATT_DINFO_PNPID_CTRL,			//UUID: 2803, 	VALUE:  			Prop: Read
	RDT_LEATT_DINFO_PNPID_DATA,			//UUID: 2A50,	VALUE: PnPtrs
	
	//// HID ////
	/**********************************************************************************************/
	RDT_LEATT_HID_START, 								//UUID: 2800, 	VALUE: uuid 1812
	//include
	RDT_LEATT_HID_INCLUDE,							//UUID: 2802, 	VALUE: include
	//protocol
	RDT_LEATT_HID_PROTOCOL_MODE_CTRL,					//UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	RDT_LEATT_HID_PROTOCOL_MODE_DATA,					//UUID: 2A4E,	VALUE: protocolMode
	//boot keyboard input report
	RDT_LEATT_HID_BOOT_KB_REPORT_INPUT_CTRL,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	RDT_LEATT_HID_BOOT_KB_REPORT_INPUT_DATA,			//UUID: 2A22, 	VALUE: bootKeyInReport
	RDT_LEATT_HID_BOOT_KB_REPORT_INPUT_CCC,			//UUID: 2902, 	VALUE: bootKeyInReportCCC
	//boot keyboard output report
	RDT_LEATT_HID_BOOT_KB_REPORT_OUTPUT_CTRL,			//UUID: 2803, 	VALUE:  			Prop: Read | write| write_without_rsp
	RDT_LEATT_HID_BOOT_KB_REPORT_OUTPUT_DATA,		    //UUID: 2A32, 	VALUE: bootKeyOutReport
	//consume report in
	RDT_LEATT_HID_CONSUME_REPORT_INPUT_CTRL,			//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	RDT_LEATT_HID_CONSUME_REPORT_INPUT_DATA,			//UUID: 2A4D, 	VALUE: reportConsumerIn
	RDT_LEATT_HID_CONSUME_REPORT_INPUT_CCC,			//UUID: 2902, 	VALUE: reportConsumerInCCC
	RDT_LEATT_HID_CONSUME_REPORT_INPUT_REF, 		//UUID: 2908    VALUE: REPORT_ID_CONSUMER, TYPE_INPUT
	//keyboard report in
	RDT_LEATT_HID_NORMAL_KB_REPORT_INPUT_CTRL,		//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	RDT_LEATT_HID_NORMAL_KB_REPORT_INPUT_DATA,		//UUID: 2A4D, 	VALUE: reportKeyIn
	RDT_LEATT_HID_NORMAL_KB_REPORT_INPUT_CCC,		//UUID: 2902, 	VALUE: reportKeyInInCCC
	RDT_LEATT_HID_NORMAL_KB_REPORT_INPUT_REF, 		//UUID: 2908    VALUE: REPORT_ID_KEYBOARD, TYPE_INPUT
	//keyboard report out
	RDT_LEATT_HID_NORMAL_KB_REPORT_OUTPUT_CTRL,		//UUID: 2803, 	VALUE:  			Prop: Read | write| write_without_rsp
	RDT_LEATT_HID_NORMAL_KB_REPORT_OUTPUT_DATA,  		//UUID: 2A4D, 	VALUE: reportKeyOut
	RDT_LEATT_HID_NORMAL_KB_REPORT_OUTPUT_REF, 		//UUID: 2908    VALUE: REPORT_ID_KEYBOARD, TYPE_OUTPUT
	// report map
	RDT_LEATT_HID_REPORT_MAP_CTRL,					//UUID: 2803, 	VALUE:  			Prop: Read
	RDT_LEATT_HID_REPORT_MAP_DATA,					//UUID: 2A4B, 	VALUE: reportKeyIn
	RDT_LEATT_HID_REPORT_MAP_EXT_REF,				//UUID: 2907 	VALUE: extService
	//hid information
	RDT_LEATT_HID_INFO_CTRL,					//UUID: 2803, 	VALUE:  			Prop: read
	RDT_LEATT_HID_INFO_DATA,					//UUID: 2A4A 	VALUE: hidInformation
	//control point
	RDT_LEATT_HID_CONTROL_POINT_CTRL,					//UUID: 2803, 	VALUE:  			Prop: write_without_rsp
	RDT_LEATT_HID_CONTROL_POINT_DATA,					//UUID: 2A4C 	VALUE: controlPoint
	
	//// battery service ////
	/**********************************************************************************************/
	RDT_LEATT_BATT_START, 								//UUID: 2800, 	VALUE: uuid 180f
	RDT_LEATT_BATT_LEVEL_INPUT_CTRL,					//UUID: 2803, 	VALUE:  			Prop: Read | Notify
	RDT_LEATT_BATT_LEVEL_INPUT_DATA,					//UUID: 2A19 	VALUE: batVal
	RDT_LEATT_BATT_LEVEL_INPUT_CCC,					//UUID: 2902, 	VALUE: batValCCC

	//// SPP ////
	/**********************************************************************************************/
	RDT_LEATT_SPP_START, 							 //UUID: 2800, 	VALUE: telink spp service uuid
	//server to client
	RDT_LEATT_SPP_SERVER_TO_CLIENT_CTRL,		     //UUID: 2803, 	VALUE:  			Prop: read | Notify
	RDT_LEATT_SPP_SERVER_TO_CLIENT_DATA,			 //UUID: telink spp s2c uuid,  VALUE: SppDataServer2ClientData
	RDT_LEATT_SPP_SERVER_TO_CLIENT_CCC,			 //UUID: 2902, 	VALUE: SppDataServer2ClientDataCCC
	RDT_LEATT_SPP_SERVER_TO_CLIENT_DESC,		 //UUID: 2901, 	VALUE: TelinkSPPS2CDescriptor
	//client to server
	RDT_LEATT_SPP_CLIENT_TO_SERVER_CTRL,		     //UUID: 2803, 	VALUE:  			Prop: read | write_without_rsp
	RDT_LEATT_SPP_CLIENT_TO_SERVER_DATA,			 //UUID: telink spp c2s uuid,  VALUE: SppDataClient2ServerData
	RDT_LEATT_SPP_CLIENT_TO_SERVER_DESC,		 //UUID: 2901, 	VALUE: TelinkSPPC2SDescriptor

	RDT_LEATT_END,
}RDT_LEATT_HANDLE;




int tlkmmi_rdt_leAttInit(void);




#endif //#if (TLK_TEST_RDT_ENABLE)

#endif //TLKMMI_RDT_LEATT_H

