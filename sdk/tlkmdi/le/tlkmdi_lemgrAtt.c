/********************************************************************************************************
 * @file	tlkmdi_lemgrAtt.c
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
#if (TLK_MDI_LEGATT_ENABLE)
#include "tlkstk/ble/ble.h"
#include "tlkmmi/lemgr/tlkmmi_lemgr.h"
//#include "tlkmmi/lemgr/tlkmmi_lemgrComm.h"
#include "tlkmdi/le/tlkmdi_lemgrAtt.h"

#if BLE_IOS_ANCS_ENABLE
	#include "tlklib/ios_service/ancs/ancs.h"
	#include "tlklib/ios_service/ancs/ancsDemo.h"
#endif

#if BLE_IOS_AMS_ENABLE
	#include "tlklib/ios_service/ams/ams.h"
	#include "tlklib/ios_service/ams/amsDemo.h"
#endif


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
} gap_periConnectParams_t;

static const uint16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

static const uint16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

static const uint16 reportRefUUID = GATT_UUID_REPORT_REF;

static const uint16 userdesc_UUID	= GATT_UUID_CHAR_USER_DESC;

static const uint16 serviceChangeUUID = GATT_UUID_SERVICE_CHANGE;

static const uint16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const uint16 my_characterUUID = GATT_UUID_CHARACTER;

static const uint16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

static const uint16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

static const uint16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

static const uint16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;

static const uint16 my_appearanceUIID = GATT_UUID_APPEARANCE;

static const uint16 my_periConnParamUUID = GATT_UUID_PERI_CONN_PARAM;

static const uint16 my_appearance = GAP_APPEARE_UNKNOWN;

static const uint16 my_gattServiceUUID = SERVICE_UUID_GENERIC_ATTRIBUTE;

static const gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

_attribute_ble_data_retention_	static uint16 serviceChangeVal[2] = {0};

_attribute_ble_data_retention_	static uint08 serviceChangeCCC[2] = {0,0};



static const uint08 my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

//////////////////////// Battery /////////////////////////////////////////////////
static const uint16 my_batServiceUUID        = SERVICE_UUID_BATTERY;
static const uint16 my_batCharUUID       	  = CHARACTERISTIC_UUID_BATTERY_LEVEL;
_attribute_ble_data_retention_	static uint08 batteryValueInCCC[2] = {0,0};
_attribute_ble_data_retention_	static uint08 my_batVal[1] 	= {99};

//////////////////////// HID /////////////////////////////////////////////////////

static const uint16 my_hidServiceUUID        = SERVICE_UUID_HUMAN_INTERFACE_DEVICE;

static const uint16 hidProtocolModeUUID      = CHARACTERISTIC_UUID_HID_PROTOCOL_MODE;
static const uint16 hidReportUUID            = CHARACTERISTIC_UUID_HID_REPORT;
static const uint16 hidReportMapUUID         = CHARACTERISTIC_UUID_HID_REPORT_MAP;
static const uint16 hidbootKeyInReportUUID   = CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT;
static const uint16 hidbootKeyOutReportUUID  = CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT;
static const uint16 hidinformationUUID       = CHARACTERISTIC_UUID_HID_INFORMATION;
static const uint16 hidCtrlPointUUID         = CHARACTERISTIC_UUID_HID_CONTROL_POINT;
static const uint16 hidIncludeUUID           = GATT_UUID_INCLUDE;

_attribute_ble_data_retention_	static uint08 protocolMode 			  = DFLT_HID_PROTOCOL_MODE;

// Key in Report characteristic variables
_attribute_ble_data_retention_	static uint08 reportKeyIn[8];
_attribute_ble_data_retention_	static uint08 reportKeyInCCC[2] = {0,0};
// HID Report Reference characteristic descriptor, key input
_attribute_ble_data_retention_	static uint08 reportRefKeyIn[2] =
             { HID_REPORT_ID_KEYBOARD_INPUT, HID_REPORT_TYPE_INPUT };

// Key out Report characteristic variables
_attribute_ble_data_retention_	static uint08 reportKeyOut[1];
_attribute_ble_data_retention_	static uint08 reportRefKeyOut[2] =
             { HID_REPORT_ID_KEYBOARD_INPUT, HID_REPORT_TYPE_OUTPUT };

// Consumer Control input Report
_attribute_ble_data_retention_	static uint08 reportConsumerControlIn[2];
_attribute_ble_data_retention_	static uint08 reportConsumerControlInCCC[2] = {0,0};
_attribute_ble_data_retention_	static uint08 reportRefConsumerControlIn[2] =
			 { HID_REPORT_ID_CONSUME_CONTROL_INPUT, HID_REPORT_TYPE_INPUT };

// Boot Keyboard Input Report
_attribute_ble_data_retention_	static uint08 bootKeyInReport;
_attribute_ble_data_retention_	static uint08 bootKeyInReportCCC[2] = {0,0};

// Boot Keyboard Output Report
_attribute_ble_data_retention_	static uint08 bootKeyOutReport;

// HID Information characteristic
static const uint08 hidInformation[] =
{
  U16_LO(0x0111), U16_HI(0x0111),             // bcdHID (USB HID version)
  0x00,                                       // bCountryCode
  0x01                                        // Flags
};

// HID Control Point characteristic
_attribute_ble_data_retention_	static uint08 controlPoint;

// HID Report Map characteristic
// Keyboard report descriptor (using format for Boot interface descriptor)

static const uint08 reportMap[] =
{
	//keyboard report in
	0x05, 0x01,     // Usage Pg (Generic Desktop)
	0x09, 0x06,     // Usage (Keyboard)
	0xA1, 0x01,     // Collection: (Application)
	0x85, HID_REPORT_ID_KEYBOARD_INPUT,     // Report Id (keyboard)
				  //
	0x05, 0x07,     // Usage Pg (Key Codes)
	0x19, 0xE0,     // Usage Min (224)  VK_CTRL:0xe0
	0x29, 0xE7,     // Usage Max (231)  VK_RWIN:0xe7
	0x15, 0x00,     // Log Min (0)
	0x25, 0x01,     // Log Max (1)
				  //
				  // Modifier byte
	0x75, 0x01,     // Report Size (1)   1 bit * 8
	0x95, 0x08,     // Report Count (8)
	0x81, 0x02,     // Input: (Data, Variable, Absolute)
				  //
				  // Reserved byte
	0x95, 0x01,     // Report Count (1)
	0x75, 0x08,     // Report Size (8)
	0x81, 0x01,     // Input: (static constant)

	//keyboard output
	//5 bit led ctrl: NumLock CapsLock ScrollLock Compose kana
	0x95, 0x05,    //Report Count (5)
	0x75, 0x01,    //Report Size (1)
	0x05, 0x08,    //Usage Pg (LEDs )
	0x19, 0x01,    //Usage Min
	0x29, 0x05,    //Usage Max
	0x91, 0x02,    //Output (Data, Variable, Absolute)
	//3 bit reserved
	0x95, 0x01,    //Report Count (1)
	0x75, 0x03,    //Report Size (3)
	0x91, 0x01,    //Output (static constant)

	// Key arrays (6 bytes)
	0x95, 0x06,     // Report Count (6)
	0x75, 0x08,     // Report Size (8)
	0x15, 0x00,     // Log Min (0)
	0x25, 0xF1,     // Log Max (241)
	0x05, 0x07,     // Usage Pg (Key Codes)
	0x19, 0x00,     // Usage Min (0)
	0x29, 0xf1,     // Usage Max (241)
	0x81, 0x00,     // Input: (Data, Array)

	0xC0,            // End Collection

	//consumer report in
	0x05, 0x0C,   // Usage Page (Consumer)
	0x09, 0x01,   // Usage (Consumer Control)
	0xA1, 0x01,   // Collection (Application)
	0x85, HID_REPORT_ID_CONSUME_CONTROL_INPUT,   //     Report Id
	0x75,0x10,     //global, report size 16 bits
	0x95,0x01,     //global, report count 1
	0x15,0x01,     //global, min  0x01
	0x26,0x8c,0x02,  //global, max  0x28c
	0x19,0x01,     //local, min   0x01
	0x2a,0x8c,0x02,  //local, max    0x28c
	0x81,0x00,     //main,  input data variable, absolute
	0xc0,        //main, end collection

};

// HID External Report Reference Descriptor for report map
_attribute_ble_data_retention_	static uint16 extServiceUUID;

#if  TLKMDI_LEOTA_ENABLE
//////////////////////// OTA //////////////////////////////////
static const uint08 my_OtaServiceUUID[16]				= WRAPPING_BRACES(TELINK_OTA_UUID_SERVICE);
static const uint08 my_OtaUUID[16]						= WRAPPING_BRACES(TELINK_SPP_DATA_OTA);
_attribute_ble_data_retention_	static 		  uint08 my_OtaData 						= 0x00;
static const uint08 my_OtaName[] = {'O', 'T', 'A'};
#endif

// Include attribute (Battery service)
static const uint16 include[3] = {BATT_PS_H, BATT_LEVEL_INPUT_CCB_H, SERVICE_UUID_BATTERY};



////////////////////// SPP ////////////////////////////////////
static const uint08 TelinkSppServiceUUID[16]	      	    = WRAPPING_BRACES(TELINK_SPP_UUID_SERVICE);
static const uint08 TelinkSppDataServer2ClientUUID[16]      = WRAPPING_BRACES(TELINK_SPP_DATA_SERVER2CLIENT);
static const uint08 TelinkSppDataClient2ServerUUID[16]      = WRAPPING_BRACES(TELINK_SPP_DATA_CLIENT2SERVER);


// Spp data from Server to Client characteristic variables
_attribute_ble_data_retention_	static uint08 SppDataServer2ClientDataCCC[2]  				= {0};
//this array will not used for sending data(directly calling HandleValueNotify API), so cut array length from 20 to 1, saving some SRAM
_attribute_ble_data_retention_	static uint08 SppDataServer2ClientData[1] 					= {0};  //SppDataServer2ClientData[20]
// Spp data from Client to Server characteristic variables
//this array will not used for receiving data(data processed by Attribute Write CallBack function), so cut array length from 20 to 1, saving some SRAM
_attribute_ble_data_retention_	static uint08 SppDataClient2ServerData[1] 					= {0};  //SppDataClient2ServerData[20]


//SPP data descriptor
static const uint08 TelinkSPPS2CDescriptor[] 		 		= "Telink SPP: Module->Phone";
static const uint08 TelinkSPPC2SDescriptor[]        		= "Telink SPP: Phone->Module";


//// GAP attribute values
static const uint08 my_devNameCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(GenericAccess_DeviceName_DP_H), U16_HI(GenericAccess_DeviceName_DP_H),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const uint08 my_appearanceCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(GenericAccess_Appearance_DP_H), U16_HI(GenericAccess_Appearance_DP_H),
	U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
static const uint08 my_periConnParamCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(CONN_PARAM_DP_H), U16_HI(CONN_PARAM_DP_H),
	U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};


//// GATT attribute values
static const uint08 my_serviceChangeCharVal[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(GenericAttribute_ServiceChanged_DP_H), U16_HI(GenericAttribute_ServiceChanged_DP_H),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};


//// device Information  attribute values
static const uint08 my_PnCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(DeviceInformation_pnpID_DP_H), U16_HI(DeviceInformation_pnpID_DP_H),
	U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
};


//// HID attribute values
static const uint08 my_hidProtocolModeCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(HID_PROTOCOL_MODE_DP_H), U16_HI(HID_PROTOCOL_MODE_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE), U16_HI(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE)
};
static const uint08 my_hidbootKeyInReporCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(HID_BOOT_KB_REPORT_INPUT_DP_H), U16_HI(HID_BOOT_KB_REPORT_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT)
};
static const uint08 my_hidbootKeyOutReporCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(HID_BOOT_KB_REPORT_OUTPUT_DP_H), U16_HI(HID_BOOT_KB_REPORT_OUTPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT)
};
static const uint08 my_hidReportCCinCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(HID_CONSUME_REPORT_INPUT_DP_H), U16_HI(HID_CONSUME_REPORT_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
static const uint08 my_hidReportKEYinCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(HID_NORMAL_KB_REPORT_INPUT_DP_H), U16_HI(HID_NORMAL_KB_REPORT_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
static const uint08 my_hidReportKEYoutCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(HID_NORMAL_KB_REPORT_OUTPUT_DP_H), U16_HI(HID_NORMAL_KB_REPORT_OUTPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
static const uint08 my_hidReportMapCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(HID_REPORT_MAP_DP_H), U16_HI(HID_REPORT_MAP_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT_MAP), U16_HI(CHARACTERISTIC_UUID_HID_REPORT_MAP)
};
static const uint08 my_hidinformationCharVal[5] = {
	CHAR_PROP_READ,
	U16_LO(HID_INFORMATION_DP_H), U16_HI(HID_INFORMATION_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_INFORMATION), U16_HI(CHARACTERISTIC_UUID_HID_INFORMATION)
};
static const uint08 my_hidCtrlPointCharVal[5] = {
	CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(HID_CONTROL_POINT_DP_H), U16_HI(HID_CONTROL_POINT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_HID_CONTROL_POINT), U16_HI(CHARACTERISTIC_UUID_HID_CONTROL_POINT)
};


//// Battery attribute values
static const uint08 my_batCharVal[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(BATT_LEVEL_INPUT_DP_H), U16_HI(BATT_LEVEL_INPUT_DP_H),
	U16_LO(CHARACTERISTIC_UUID_BATTERY_LEVEL), U16_HI(CHARACTERISTIC_UUID_BATTERY_LEVEL)
};


//// Telink spp  attribute values
static const uint08 TelinkSppDataServer2ClientCharVal[19] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(SPP_SERVER_TO_CLIENT_DP_H), U16_HI(SPP_SERVER_TO_CLIENT_DP_H),
	TELINK_SPP_DATA_SERVER2CLIENT
};
static const uint08 TelinkSppDataClient2ServerCharVal[19] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(SPP_CLIENT_TO_SERVER_DP_H), U16_HI(SPP_CLIENT_TO_SERVER_DP_H),
	TELINK_SPP_DATA_CLIENT2SERVER
};
#if TLKMDI_LEOTA_ENABLE
//// OTA attribute values
static const uint08 my_OtaCharVal[19] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(OTA_CMD_OUT_DP_H), U16_HI(OTA_CMD_OUT_DP_H),
	TELINK_SPP_DATA_OTA,
};
#endif



void ancsAndAmsFuncRebuild(){
	#if BLE_IOS_ANCS_ENABLE
	ancs_initial(ANCS_ATT_TYPE_ALL_INCLUDE);///clear parameters
	ancs_setNewsReceivedStatus(ANCS_SET_NO_NEW_NOTICE);///clear flag

	extern u32 tick_encryptionFinish;///enable ancs and ams function
	tick_encryptionFinish = clock_time() | 1;
	#elif (BLE_IOS_AMS_ENABLE && (!BLE_IOS_ANCS_ENABLE))
	amsInit(1);///enable ams function
	amsFuncSetEnable(1);///enable ams flag
	#endif
}

int myBattValHandle(u16 connHandle, void *pData){
	_attribute_data_retention_ static u32 tick_state = 0;
	rf_packet_att_write_t *p = (rf_packet_att_write_t*)pData;
	if(
			(tick_state == 0)
			&& (p->value == 0)
#if BLE_IOS_ANCS_ENABLE
			&& (ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED)
#endif

#if BLE_IOS_AMS_ENABLE
			&& (amsGetConnState() == AMS_CONNECTION_ESTABLISHED)
#endif
			){
		tick_state = clock_time() | 1;
	}else if (tick_state & 1){
		if(!clock_time_exceed(tick_state, 4000000)){
			if(
					(p->value == 1)
#if BLE_IOS_ANCS_ENABLE
					&& (ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED)
#endif

#if BLE_IOS_AMS_ENABLE
					&& (amsGetConnState() == AMS_CONNECTION_ESTABLISHED)
#endif
				){
					tick_state = 0;
					ancsAndAmsFuncRebuild();
			}
		}else{
			tick_state = 0;
		}
	}else{
		if(
				(tick_state == 0)
				&& (p->value == 1)
	#if BLE_IOS_ANCS_ENABLE
				&& (ancsGetConnState() == ANCS_CONNECTION_ESTABLISHED)
	#endif

	#if BLE_IOS_AMS_ENABLE
				&& (amsGetConnState() == AMS_CONNECTION_ESTABLISHED)
	#endif
				){
			ancsAndAmsFuncRebuild();
		}
	}
	return 0;
}



#if (APP_BLE_TEST_ENABLE)
extern u32 gAppBleTestRecvCount;
extern u32 gAppBleTestRecvLens;
#endif
int spp_onReceiveData(uint16 connHandle, rf_packet_att_write_t *p)
{
	uint08 len = p->l2capLen - 3;
	if(len > 0)
	{
		//blc_gatt_pushHandleValueNotify(connHandle, SPP_SERVER_TO_CLIENT_DP_H, (void *)(&(p->value)), p->l2capLen - 3);
	}
	#if (APP_BLE_TEST_ENABLE)
	gAppBleTestRecvCount ++;
	gAppBleTestRecvLens = len;
	#endif

	return 0;
}

static uint08 my_devNameLen = 9;
static uint08 my_devName[18] = {'T','E','L','I','N','K','-','L','E',};
int tlkmdi_lemgr_attSetDevName(uint08 *pName, uint08 nameLen)
{
	if(pName == nullptr || nameLen == 0) return -TLK_EPARAM;
	if(nameLen > 18) nameLen = 18;
	my_devNameLen = nameLen;
	tmemcpy(my_devName, pName, nameLen);
	return TLK_ENONE;
}
static int tlkmdi_lemgr_attReadDevNameDeal(u16 connHandle, void* p)
{
	blc_gatt_pushReadResponse(connHandle, my_devName, my_devNameLen);
	return 1;
}


// TM : to modify
static const attribute_t my_Attributes[] = {

	{ATT_END_H - 1, 0,0,0,0,0},	// total num of attribute


	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_devNameCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_devNameCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_devName), (uint08*)(&my_devNameUUID), (uint08*)(my_devName), 0, tlkmdi_lemgr_attReadDevNameDeal},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_appearanceCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_appearanceCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (uint08*)(&my_appearanceUIID), 	(uint08*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_periConnParamCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_periConnParamCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(uint08*)(&my_periConnParamUUID), 	(uint08*)(&my_periConnParameters), 0},


	// 0008 - 000b gatt
	{4,ATT_PERMISSIONS_READ,2,2,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_gattServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_serviceChangeCharVal),(uint08*)(&my_characterUUID), 		(uint08*)(my_serviceChangeCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (serviceChangeVal), (uint08*)(&serviceChangeUUID), 	(uint08*)(&serviceChangeVal), 0},
	{0,ATT_PERMISSIONS_RDWR,2,sizeof (serviceChangeCCC),(uint08*)(&clientCharacterCfgUUID), (uint08*)(serviceChangeCCC), 0},


	// 000c - 000e  device Information Service
	{3,ATT_PERMISSIONS_READ,2,2,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_devServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_PnCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_PnCharVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_PnPtrs),(uint08*)(&my_PnPUUID), (uint08*)(my_PnPtrs), 0},


	/////////////////////////////////// 4. HID Service /////////////////////////////////////////////////////////
	// 000f
	//{27, ATT_PERMISSIONS_READ,2,2,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_hidServiceUUID), 0},
	{HID_CONTROL_POINT_DP_H - HID_PS_H + 1, ATT_PERMISSIONS_READ,2,2,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_hidServiceUUID), 0},

	// 0010  include battery service
	{0,ATT_PERMISSIONS_READ,2,sizeof(include),(uint08*)(&hidIncludeUUID), 	(uint08*)(include), 0},

	// 0011 - 0012  protocol mode
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidProtocolModeCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidProtocolModeCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,2, sizeof(protocolMode),(uint08*)(&hidProtocolModeUUID), 	(uint08*)(&protocolMode), 0},	//value

	// 0013 - 0015  boot keyboard input report (char-val-client)
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidbootKeyInReporCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidbootKeyInReporCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(bootKeyInReport),(uint08*)(&hidbootKeyInReportUUID), 	(uint08*)(&bootKeyInReport), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(bootKeyInReportCCC),(uint08*)(&clientCharacterCfgUUID), 	(uint08*)(bootKeyInReportCCC), 0},	//value

	// 0016 - 0017   boot keyboard output report (char-val)
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidbootKeyOutReporCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidbootKeyOutReporCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,2, sizeof(bootKeyOutReport), (uint08*)(&hidbootKeyOutReportUUID), 	(uint08*)(&bootKeyOutReport), 0},	//value


	// 0018 - 001b. consume report in: 4 (char-val-client-ref)
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidReportCCinCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidReportCCinCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2, sizeof(reportConsumerControlIn),(uint08*)(&hidReportUUID), 	(uint08*)(reportConsumerControlIn), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(reportConsumerControlInCCC),(uint08*)(&clientCharacterCfgUUID), 	(uint08*)(reportConsumerControlInCCC), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(reportRefConsumerControlIn),(uint08*)(&reportRefUUID), 	(uint08*)(reportRefConsumerControlIn), 0},	//value

	// 001c - 001f . keyboard report in : 4 (char-val-client-ref)
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidReportKEYinCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidReportKEYinCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2, sizeof(reportKeyIn),(uint08*)(&hidReportUUID), 	(uint08*)(reportKeyIn), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(reportKeyInCCC),(uint08*)(&clientCharacterCfgUUID), 	(uint08*)(reportKeyInCCC), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(reportRefKeyIn),(uint08*)(&reportRefUUID), 	(uint08*)(reportRefKeyIn), 0},	//value

	// 0020 - 0022 . keyboard report out: 3 (char-val-ref)
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidReportKEYoutCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidReportKEYoutCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(reportKeyOut),(uint08*)(&hidReportUUID), 	(uint08*)(reportKeyOut), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(reportRefKeyOut),(uint08*)(&reportRefUUID), 	(uint08*)(reportRefKeyOut), 0},	//value


	// 0023 - 0025 . report map: 3
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidReportMapCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidReportMapCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(reportMap),(uint08*)(&hidReportMapUUID), 	(uint08*)(reportMap), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(extServiceUUID),(uint08*)(&extReportRefUUID), 	(uint08*)(&extServiceUUID), 0},	//value

	// 0026 - 0027 . hid information: 2
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidinformationCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidinformationCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2, sizeof(hidInformation),(uint08*)(&hidinformationUUID), 	(uint08*)(hidInformation), 0},	//value

	// 0028 - 0029 . control point: 2
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_hidCtrlPointCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_hidCtrlPointCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_WRITE,2, sizeof(controlPoint),(uint08*)(&hidCtrlPointUUID), 	(uint08*)(&controlPoint), 0},	//value

	////////////////////////////////////// Battery Service /////////////////////////////////////////////////////
	// 002a - 002d
	{4,ATT_PERMISSIONS_READ,2,2,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_batServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_batCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_batCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(my_batVal),(uint08*)(&my_batCharUUID), 	(uint08*)(my_batVal), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(batteryValueInCCC),(uint08*)(&clientCharacterCfgUUID), 	(uint08*)(batteryValueInCCC), &myBattValHandle, 0},	//value


	// 002e - 0035 SPP for data test
	{8,ATT_PERMISSIONS_READ,2,16,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&TelinkSppServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSppDataServer2ClientCharVal),(uint08*)(&my_characterUUID), 		(uint08*)(TelinkSppDataServer2ClientCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_READ,16,sizeof(SppDataServer2ClientData),(uint08*)(&TelinkSppDataServer2ClientUUID), (uint08*)(SppDataServer2ClientData), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,2,(uint08*)&clientCharacterCfgUUID,(uint08*)(&SppDataServer2ClientDataCCC)},
	{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSPPS2CDescriptor),(uint08*)&userdesc_UUID,(uint08*)(&TelinkSPPS2CDescriptor)},
	{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSppDataClient2ServerCharVal),(uint08*)(&my_characterUUID), 		(uint08*)(TelinkSppDataClient2ServerCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,16,sizeof(SppDataClient2ServerData),(uint08*)(&TelinkSppDataClient2ServerUUID), (uint08*)(SppDataClient2ServerData), (att_readwrite_callback_t)&spp_onReceiveData},	//value
	{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSPPC2SDescriptor),(uint08*)&userdesc_UUID,(uint08*)(&TelinkSPPC2SDescriptor)},

#if  TLK_MDI_LEOTA_ENABLE
	////////////////////////////////////// OTA /////////////////////////////////////////////////////
	// 0036 - 0039
	{4,ATT_PERMISSIONS_READ, 2,16,(uint08*)(&my_primaryServiceUUID), 	(uint08*)(&my_OtaServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ, 2, sizeof(my_OtaCharVal),(uint08*)(&my_characterUUID), (uint08*)(my_OtaCharVal), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,16,sizeof(my_OtaData),(uint08*)(&my_OtaUUID),	(&my_OtaData), &otaWrite, NULL},			//value
	{0,ATT_PERMISSIONS_READ, 2,sizeof (my_OtaName),(uint08*)(&userdesc_UUID), (uint08*)(my_OtaName), 0},
#endif
};




int tlkmdi_lemgr_attInit(void)
{
	//Note: this function is used to register ATT table to BLE Stack.
	bls_att_setAttributeTable((uint08 *)my_Attributes);
	return TLK_ENONE;
}










#endif

