/********************************************************************************************************
 * @file	tlkmmi_rdt_leAtt.c
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
#if (TLKMMI_TEST_ENABLE)
#include "../../tlkmmi_testStdio.h"
#if (TLK_TEST_RDT_ENABLE)
#include "../tlkmmi_rdtStd.h"
#include "tlkmmi_rdt_leAtt.h"
#include "tlkstk/ble/ble.h"



static const uint16 sTlkmmiRdtLeCccUUID            = GATT_UUID_CLIENT_CHAR_CFG;
static const uint16 sTlkmmiRdtLeExtReportRefUUID   = GATT_UUID_EXT_REPORT_REF;
static const uint16 sTlkmmiRdtLeReportRefUUID      = GATT_UUID_REPORT_REF;
static const uint16 sTlkmmiRdtLeUserDescUUID	   = GATT_UUID_CHAR_USER_DESC;
static const uint16 sTlkmmiRdtLeServiceChangeUUID  = GATT_UUID_SERVICE_CHANGE;
static const uint16 sTlkmmiRdtLePrimaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;
static const uint16 sTlkmmiRdtLeCharacterUUID      = GATT_UUID_CHARACTER;
static const uint16 sTlkmmiRdtLeDevServiceUUID     = SERVICE_UUID_DEVICE_INFORMATION;
static const uint16 sTlkmmiRdtLePnpUUID            = CHARACTERISTIC_UUID_PNP_ID;
static const uint16 sTlkmmiRdtLeDevNameUUID        = GATT_UUID_DEVICE_NAME;
static const uint16 sTlkmmiRdtLeGapServiceUUID     = SERVICE_UUID_GENERIC_ACCESS;
static const uint16 sTlkmmiRdtLeAppearanceUUID     = GATT_UUID_APPEARANCE;
static const uint16 sTlkmmiRdtLePeriConnParamUUID  = GATT_UUID_PERI_CONN_PARAM;
static const uint16 sTlkmmiRdtLeGattServiceUUID    = SERVICE_UUID_GENERIC_ATTRIBUTE;
static const uint16 sTlkmmiRdtLeHidServiceUUID     = SERVICE_UUID_HUMAN_INTERFACE_DEVICE;

static const uint16 sTlkmmiRdtLeHidProtocolModeUUID      = CHARACTERISTIC_UUID_HID_PROTOCOL_MODE;
static const uint16 sTlkmmiRdtLeHidReportUUID            = CHARACTERISTIC_UUID_HID_REPORT;
static const uint16 sTlkmmiRdtLeHidReportMapUUID         = CHARACTERISTIC_UUID_HID_REPORT_MAP;
static const uint16 sTlkmmiRdtLeHidBootKeyInReportUUID   = CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT;
static const uint16 sTlkmmiRdtLeHidBootKeyOutReportUUID  = CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT;
static const uint16 sTlkmmiRdtLeHidInformationUUID       = CHARACTERISTIC_UUID_HID_INFORMATION;
static const uint16 sTlkmmiRdtLeHidCtrlPointUUID         = CHARACTERISTIC_UUID_HID_CONTROL_POINT;
static const uint16 sTlkmmiRdtLeHidIncludeUUID           = GATT_UUID_INCLUDE;
////////////////////// SPP ////////////////////////////////////
static const uint08 sTlkmmiRdtLeSppTelinkServiceUUID[16]        = WRAPPING_BRACES(TELINK_SPP_UUID_SERVICE);
static const uint08 sTlkmmiRdtLeSppTelinkService2ClientUUID[16] = WRAPPING_BRACES(TELINK_SPP_DATA_SERVER2CLIENT);
static const uint08 sTlkmmiRdtLeSppTelinkClient2ServerUUID[16]  = WRAPPING_BRACES(TELINK_SPP_DATA_CLIENT2SERVER);


// HID Report Map characteristic
// Keyboard report descriptor (using format for Boot interface descriptor)
static const uint08 sTlkmmiRdtLeHidReportMap[] =
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


//// GAP attribute values
static const uint08 sTlkmmiRdtLeDevName[] = { 'T','L','K','W','A','T','C','H'};
static const uint08 sTlkmmiRdtLeDevNameChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(RDT_LEATT_GAP_DEV_NAME_DATA), U16_HI(RDT_LEATT_GAP_DEV_NAME_DATA),
	U16_LO(GATT_UUID_DEVICE_NAME), U16_HI(GATT_UUID_DEVICE_NAME)
};
static const uint16 sTlkmmiRdtLeAppearance = GAP_APPEARE_UNKNOWN;
static const uint08 sTlkmmiRdtLeAppearanceChar[5] = {
	CHAR_PROP_READ,
	U16_LO(RDT_LEATT_GAP_APPEARANCE_DATA), U16_HI(RDT_LEATT_GAP_APPEARANCE_DATA),
	U16_LO(GATT_UUID_APPEARANCE), U16_HI(GATT_UUID_APPEARANCE)
};
static const tlkmmi_rdt_leConnParam_t sTlkmmiRdtLeConnParamVal = {20, 40, 0, 1000};
static const uint08 sTlkmmiRdtLeConnParamChar[5] = {
	CHAR_PROP_READ,
	U16_LO(RDT_LEATT_GAP_CONN_PARAM_DATA), U16_HI(RDT_LEATT_GAP_CONN_PARAM_DATA),
	U16_LO(GATT_UUID_PERI_CONN_PARAM), U16_HI(GATT_UUID_PERI_CONN_PARAM)
};
	
//// GATT attribute values
static uint16 sTlkmmiRdtLeServiceChangeVal[2] = {0};
static uint08 sTlkmmiRdtLeServiceChangeCCC[2] = {0,0};
static const uint08 sTlkmmiRdtLeServiceChangeChar[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(RDT_LEATT_GATT_SERVICE_CHANGED_DATA), U16_HI(RDT_LEATT_GATT_SERVICE_CHANGED_DATA),
	U16_LO(GATT_UUID_SERVICE_CHANGE), U16_HI(GATT_UUID_SERVICE_CHANGE)
};

//// device Information  attribute values
static const uint08 sTlkmmiRdtLeDinfoPnpVal[] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};
static const uint08 sTlkmmiRdtLeDinfoPnpChar[5] = {
	CHAR_PROP_READ,
	U16_LO(RDT_LEATT_DINFO_PNPID_DATA), U16_HI(RDT_LEATT_DINFO_PNPID_DATA),
	U16_LO(CHARACTERISTIC_UUID_PNP_ID), U16_HI(CHARACTERISTIC_UUID_PNP_ID)
};

//// HID attribute values
static const uint16 sTlkmmiRdtLeHidInclude[3] = {RDT_LEATT_BATT_START, RDT_LEATT_BATT_LEVEL_INPUT_CCC, SERVICE_UUID_BATTERY};
static uint08 sTlkmmiRdtLeHidProtocolMode = DFLT_HID_PROTOCOL_MODE;
static const uint08 sTlkmmiRdtLeHidProtocolModeChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RDT_LEATT_HID_PROTOCOL_MODE_DATA), U16_HI(RDT_LEATT_HID_PROTOCOL_MODE_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE), U16_HI(CHARACTERISTIC_UUID_HID_PROTOCOL_MODE)
};
static uint08 sTlkmmiRdtLeHidBootKeyInReport;
static uint08 sTlkmmiRdtLeHidBootKeyInReportCCC[2] = {0,0};
static const uint08 sTlkmmiRdtLeHidBootKeyInChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(RDT_LEATT_HID_BOOT_KB_REPORT_INPUT_DATA), U16_HI(RDT_LEATT_HID_BOOT_KB_REPORT_INPUT_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_INPUT)
};
static uint08 sTlkmmiRdtLeHidBootKeyOutReport;
static const uint08 sTlkmmiRdtLeHidBootKeyOutChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RDT_LEATT_HID_BOOT_KB_REPORT_OUTPUT_DATA), U16_HI(RDT_LEATT_HID_BOOT_KB_REPORT_OUTPUT_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT), U16_HI(CHARACTERISTIC_UUID_HID_BOOT_KEY_OUTPUT)
};
static uint08 sTlkmmiRdtLeHidConsumerKeyInVal[2];
static uint08 sTlkmmiRdtLeHidConsumerKeyInCcc[2] = {0,0};
static uint08 sTlkmmiRdtLeHidConsumerKeyInRef[2] = { HID_REPORT_ID_CONSUME_CONTROL_INPUT, HID_REPORT_TYPE_INPUT };
static const uint08 sTlkmmiRdtLeHidConsumeKeyInChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(RDT_LEATT_HID_CONSUME_REPORT_INPUT_DATA), U16_HI(RDT_LEATT_HID_CONSUME_REPORT_INPUT_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
static uint08 sTlkmmiRdtLeHidNormalKeyInVal[8];
static uint08 sTlkmmiRdtLeHidNormalKeyInCcc[2] = {0,0};
static uint08 sTlkmmiRdtLeHidNormalKeyInRef[2] = { HID_REPORT_ID_KEYBOARD_INPUT, HID_REPORT_TYPE_INPUT };
static const uint08 sTlkmmiRdtLeHidNormalKeyInChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(RDT_LEATT_HID_NORMAL_KB_REPORT_INPUT_DATA), U16_HI(RDT_LEATT_HID_NORMAL_KB_REPORT_INPUT_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
static uint08 sTlkmmiRdtLeHidNormalKeyOutVal[1];
static uint08 sTlkmmiRdtLeHidNormalKeyOutRef[2] = { HID_REPORT_ID_KEYBOARD_INPUT, HID_REPORT_TYPE_OUTPUT };
static const uint08 sTlkmmiRdtLeHidNormalKeyOutChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RDT_LEATT_HID_NORMAL_KB_REPORT_OUTPUT_DATA), U16_HI(RDT_LEATT_HID_NORMAL_KB_REPORT_OUTPUT_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT), U16_HI(CHARACTERISTIC_UUID_HID_REPORT)
};
static uint16 sTlkmmiRdtLeHidExtReportMapVal;
static const uint08 sTlkmmiRdtLeHidReportMapChar[5] = {
	CHAR_PROP_READ,
	U16_LO(RDT_LEATT_HID_REPORT_MAP_DATA), U16_HI(RDT_LEATT_HID_REPORT_MAP_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_REPORT_MAP), U16_HI(CHARACTERISTIC_UUID_HID_REPORT_MAP)
};
static const uint08 sTlkmmiRdtLeHidInformation[] = {
	U16_LO(0x0111), U16_HI(0x0111), // bcdHID (USB HID version)
	0x00, // bCountryCode
	0x01  // Flags
};
static const uint08 sTlkmmiRdtLeHidInformationChar[5] = {
	CHAR_PROP_READ,
	U16_LO(RDT_LEATT_HID_INFO_DATA), U16_HI(RDT_LEATT_HID_INFO_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_INFORMATION), U16_HI(CHARACTERISTIC_UUID_HID_INFORMATION)
};
static uint08 sTlkmmiRdtLeHidControlPoint;
static const uint08 sTlkmmiRdtLeHidCtrlPointChar[5] = {
	CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RDT_LEATT_HID_CONTROL_POINT_DATA), U16_HI(RDT_LEATT_HID_CONTROL_POINT_DATA),
	U16_LO(CHARACTERISTIC_UUID_HID_CONTROL_POINT), U16_HI(CHARACTERISTIC_UUID_HID_CONTROL_POINT)
};

//// Battery attribute values
static const uint16 sTlkmmiRdtLeBatServiceUUID        = SERVICE_UUID_BATTERY;
static const uint16 sTlkmmiRdtLeBatCharUUID       	  = CHARACTERISTIC_UUID_BATTERY_LEVEL;
static uint08 sTlkmmiRdtLeBatteryValueInCCC[2] = {0,0};
static uint08 sTlkmmiRdtLeBatteryValue[1] 	= {99};
static const uint08 sTlkmmiRdtLeBatteryChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(RDT_LEATT_BATT_LEVEL_INPUT_DATA), U16_HI(RDT_LEATT_BATT_LEVEL_INPUT_DATA),
	U16_LO(CHARACTERISTIC_UUID_BATTERY_LEVEL), U16_HI(CHARACTERISTIC_UUID_BATTERY_LEVEL)
};

//// Telink spp  attribute values
static uint08 sTlkmmiRdtLeSppS2cCCC[2] = {0};
static uint08 sTlkmmiRdtLeSppC2sCCC[1] = {0};  //sTlkmmiRdtLeSppC2sCCC[20]
//this array will not used for receiving data(data processed by Attribute Write CallBack function), so cut array length from 20 to 1, saving some SRAM
static uint08 sTlkmmiRdtLeSppC2sVal[1] = {0};  //sTlkmmiRdtLeSppC2sVal[20]
static const uint08 sTlkmmiRdtLeSppS2cDesc[] = "Telink SPP: Module->Phone";
static const uint08 sTlkmmiRdtLeSppC2sDesc[] = "Telink SPP: Phone->Module";
static const uint08 sTlkmmiRdtLeSppS2cChar[19] = {
	CHAR_PROP_READ | CHAR_PROP_NOTIFY,
	U16_LO(RDT_LEATT_SPP_SERVER_TO_CLIENT_DATA), U16_HI(RDT_LEATT_SPP_SERVER_TO_CLIENT_DATA),
	TELINK_SPP_DATA_SERVER2CLIENT
};
static const uint08 sTlkmmiRdtLeSppC2sChar[19] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP,
	U16_LO(RDT_LEATT_SPP_CLIENT_TO_SERVER_DATA), U16_HI(RDT_LEATT_SPP_CLIENT_TO_SERVER_DATA),
	TELINK_SPP_DATA_CLIENT2SERVER
};

int tlkmmi_rdt_leSppRecvDeal(uint16 connHandle, rf_packet_att_write_t *p)
{
//	uint08 len = p->l2capLen - 3;
//	if(len > 0)
//	{
//		//blc_gatt_pushHandleValueNotify(connHandle, RDT_LEATT_SPP_SERVER_TO_CLIENT_DATA, (void *)(&(p->value)), p->l2capLen - 3);
//	}

	return 0;
}




// TM : to modify
static const attribute_t sTlkmmiRdtLeAttributes[] =
{
	{RDT_LEATT_END - 1, 0,0,0,0,0},	// total num of attribute
	
	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID),(uint08*)(&sTlkmmiRdtLeGapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeDevNameChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeDevNameChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeDevName),(uint08*)(&sTlkmmiRdtLeDevNameUUID),(uint08*)(sTlkmmiRdtLeDevName), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeAppearanceChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeAppearanceChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeAppearance),(uint08*)(&sTlkmmiRdtLeAppearanceUUID),(uint08*)(&sTlkmmiRdtLeAppearance), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeConnParamChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeConnParamVal),(uint08*)(&sTlkmmiRdtLePeriConnParamUUID),(uint08*)(&sTlkmmiRdtLeConnParamVal), 0},
	
	// 0008 - 000b gatt
	{4,ATT_PERMISSIONS_READ,2,2,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID),(uint08*)(&sTlkmmiRdtLeGattServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeServiceChangeChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeServiceChangeChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (sTlkmmiRdtLeServiceChangeVal),(uint08*)(&sTlkmmiRdtLeServiceChangeUUID),(uint08*)(&sTlkmmiRdtLeServiceChangeVal), 0},
	{0,ATT_PERMISSIONS_RDWR,2,sizeof (sTlkmmiRdtLeServiceChangeCCC),(uint08*)(&sTlkmmiRdtLeCccUUID),(uint08*)(sTlkmmiRdtLeServiceChangeCCC), 0},

	// 000c - 000e  device Information Service
	{3,ATT_PERMISSIONS_READ,2,2,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID),(uint08*)(&sTlkmmiRdtLeDevServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeDinfoPnpChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeDinfoPnpChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (sTlkmmiRdtLeDinfoPnpVal),(uint08*)(&sTlkmmiRdtLePnpUUID), (uint08*)(sTlkmmiRdtLeDinfoPnpVal), 0},

	/////////////////////////////////// 4. HID Service /////////////////////////////////////////////////////////
	// 000f
	//{27, ATT_PERMISSIONS_READ,2,2,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID), 	(uint08*)(&sTlkmmiRdtLeHidServiceUUID), 0},
	{RDT_LEATT_HID_CONTROL_POINT_DATA-RDT_LEATT_HID_START+1, ATT_PERMISSIONS_READ,2,2,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID),(uint08*)(&sTlkmmiRdtLeHidServiceUUID), 0},
	// 0010  include battery service
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidInclude),(uint08*)(&sTlkmmiRdtLeHidIncludeUUID),(uint08*)(sTlkmmiRdtLeHidInclude), 0},
	// 0011 - 0012  protocol mode
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidProtocolModeChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidProtocolModeChar), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,2, sizeof(sTlkmmiRdtLeHidProtocolMode),(uint08*)(&sTlkmmiRdtLeHidProtocolModeUUID),(uint08*)(&sTlkmmiRdtLeHidProtocolMode), 0},	//value
	// 0013 - 0015  boot keyboard input report (char-val-client)
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidBootKeyInChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidBootKeyInChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidBootKeyInReport),(uint08*)(&sTlkmmiRdtLeHidBootKeyInReportUUID),(uint08*)(&sTlkmmiRdtLeHidBootKeyInReport), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidBootKeyInReportCCC),(uint08*)(&sTlkmmiRdtLeCccUUID),(uint08*)(sTlkmmiRdtLeHidBootKeyInReportCCC), 0},	//value
	// 0016 - 0017   boot keyboard output report (char-val)
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidBootKeyOutChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidBootKeyOutChar), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,2, sizeof(sTlkmmiRdtLeHidBootKeyOutReport), (uint08*)(&sTlkmmiRdtLeHidBootKeyOutReportUUID),(uint08*)(&sTlkmmiRdtLeHidBootKeyOutReport), 0},	//value
	// 0018 - 001b. consume report in: 4 (char-val-client-ref)
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidConsumeKeyInChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidConsumeKeyInChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2, sizeof(sTlkmmiRdtLeHidConsumerKeyInVal),(uint08*)(&sTlkmmiRdtLeHidReportUUID),(uint08*)(sTlkmmiRdtLeHidConsumerKeyInVal), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidConsumerKeyInCcc),(uint08*)(&sTlkmmiRdtLeCccUUID),(uint08*)(sTlkmmiRdtLeHidConsumerKeyInCcc), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidConsumerKeyInRef),(uint08*)(&sTlkmmiRdtLeReportRefUUID),(uint08*)(sTlkmmiRdtLeHidConsumerKeyInRef), 0},	//value
	// 001c - 001f . keyboard report in : 4 (char-val-client-ref)
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidNormalKeyInChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyInChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2, sizeof(sTlkmmiRdtLeHidNormalKeyInVal),(uint08*)(&sTlkmmiRdtLeHidReportUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyInVal), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidNormalKeyInCcc),(uint08*)(&sTlkmmiRdtLeCccUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyInCcc), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidNormalKeyInRef),(uint08*)(&sTlkmmiRdtLeReportRefUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyInRef), 0},	//value
	// 0020 - 0022 . keyboard report out: 3 (char-val-ref)
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidNormalKeyOutChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyOutChar), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidNormalKeyOutVal),(uint08*)(&sTlkmmiRdtLeHidReportUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyOutVal), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidNormalKeyOutRef),(uint08*)(&sTlkmmiRdtLeReportRefUUID),(uint08*)(sTlkmmiRdtLeHidNormalKeyOutRef), 0},	//value
	// 0023 - 0025 . report map: 3
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidReportMapChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidReportMapChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidReportMap),(uint08*)(&sTlkmmiRdtLeHidReportMapUUID),(uint08*)(sTlkmmiRdtLeHidReportMap), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeHidExtReportMapVal),(uint08*)(&sTlkmmiRdtLeExtReportRefUUID),(uint08*)(&sTlkmmiRdtLeHidExtReportMapVal), 0},	//value
	// 0026 - 0027 . hid information: 2
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidInformationChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidInformationChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2, sizeof(sTlkmmiRdtLeHidInformation),(uint08*)(&sTlkmmiRdtLeHidInformationUUID),(uint08*)(sTlkmmiRdtLeHidInformation), 0},	//value
	// 0028 - 0029 . control point: 2
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeHidCtrlPointChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeHidCtrlPointChar), 0},				//prop
	{0,ATT_PERMISSIONS_WRITE,2, sizeof(sTlkmmiRdtLeHidControlPoint),(uint08*)(&sTlkmmiRdtLeHidCtrlPointUUID),(uint08*)(&sTlkmmiRdtLeHidControlPoint), 0},	//value

	////////////////////////////////////// Battery Service /////////////////////////////////////////////////////
	// 002a - 002d
	{4,ATT_PERMISSIONS_READ,2,2,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID),(uint08*)(&sTlkmmiRdtLeBatServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeBatteryChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeBatteryChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeBatteryValue),(uint08*)(&sTlkmmiRdtLeBatCharUUID),(uint08*)(sTlkmmiRdtLeBatteryValue), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(sTlkmmiRdtLeBatteryValueInCCC),(uint08*)(&sTlkmmiRdtLeCccUUID),(uint08*)(sTlkmmiRdtLeBatteryValueInCCC), 0},	//value

	// 002e - 0035 SPP for data test
	{8,ATT_PERMISSIONS_READ,2,16,(uint08*)(&sTlkmmiRdtLePrimaryServiceUUID),(uint08*)(&sTlkmmiRdtLeSppTelinkServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeSppS2cChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeSppS2cChar), 0},				//prop
	{0,ATT_PERMISSIONS_READ,16,sizeof(sTlkmmiRdtLeSppC2sCCC),(uint08*)(&sTlkmmiRdtLeSppTelinkService2ClientUUID),(uint08*)(sTlkmmiRdtLeSppC2sCCC), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,2,(uint08*)&sTlkmmiRdtLeCccUUID,(uint08*)(&sTlkmmiRdtLeSppS2cCCC)},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeSppS2cDesc),(uint08*)&sTlkmmiRdtLeUserDescUUID,(uint08*)(&sTlkmmiRdtLeSppS2cDesc)},
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeSppC2sChar),(uint08*)(&sTlkmmiRdtLeCharacterUUID),(uint08*)(sTlkmmiRdtLeSppC2sChar), 0},				//prop
	{0,ATT_PERMISSIONS_RDWR,16,sizeof(sTlkmmiRdtLeSppC2sVal),(uint08*)(&sTlkmmiRdtLeSppTelinkClient2ServerUUID),(uint08*)(sTlkmmiRdtLeSppC2sVal), (att_readwrite_callback_t)&tlkmmi_rdt_leSppRecvDeal},	//value
	{0,ATT_PERMISSIONS_READ,2,sizeof(sTlkmmiRdtLeSppC2sDesc),(uint08*)&sTlkmmiRdtLeUserDescUUID,(uint08*)(&sTlkmmiRdtLeSppC2sDesc)},
};




int tlkmmi_rdt_leAttInit(void)
{
	//Note: this function is used to register ATT table to BLE Stack.
	bls_att_setAttributeTable((uint08 *)sTlkmmiRdtLeAttributes);
	return TLK_ENONE;
}








#endif //#if (TLK_TEST_RDT_ENABLE)
#endif //#if (TLKMMI_TEST_ENABLE)

