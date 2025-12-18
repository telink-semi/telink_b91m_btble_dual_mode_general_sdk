/********************************************************************************************************
 * @file	btp_attStdio.h
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
#ifndef BTP_ATT_STDIO_H
#define BTP_ATT_STDIO_H

#if (TLKBTP_CFG_ATT_ENABLE)


// GATT Attribute Types    16 bit UUID definition
#define BTP_GATT_UUID_PRIMARY_SERVICE   0x2800 // Primary Service
#define BTP_GATT_UUID_SECONDARY_SERVICE 0x2801 // Secondary Service
#define BTP_GATT_UUID_INCLUDE           0x2802 // Include
#define BTP_GATT_UUID_CHARACT           0x2803 // Characteristic
// GATT Characteristic Descriptors    16 bit UUID definition
#define BTP_GATT_UUID_CHAR_EXT_PROPS      0x2900 // Characteristic Extended Properties
#define BTP_GATT_UUID_CHAR_USER_DESC      0x2901 // Characteristic User Description
#define BTP_GATT_UUID_CLIENT_CHAR_CFG     0x2902 // Client Characteristic Configuration
#define BTP_GATT_UUID_SERVER_CHAR_CFG     0x2903 // Server Characteristic Configuration
#define BTP_GATT_UUID_CHAR_PRESENT_FORMAT 0x2904 // Characteristic Present Format
#define BTP_GATT_UUID_CHAR_AGG_FORMAT     0x2905 // Characteristic Aggregate Format
#define BTP_GATT_UUID_VALID_RANGE         0x2906 // Valid Range
#define BTP_GATT_UUID_EXT_REPORT_REF      0x2907 // External Report Reference Descriptor
#define BTP_GATT_UUID_REPORT_REF          0x2908 // Report Reference Descriptor
// GATT Characteristic service
#define BTP_SERVICE_UUID_ALERT_NOTIFICATION            0x1811
#define BTP_SERVICE_UUID_BATTERY                       0x180F
#define BTP_SERVICE_UUID_BLOOD_PRESSURE                0x1810
#define BTP_SERVICE_UUID_CURRENT_TIME                  0x1805
#define BTP_SERVICE_UUID_CYCLING_POWER                 0x1818
#define BTP_SERVICE_UUID_CYCLING_SPEED_AND_CADENCE     0x1816
#define BTP_SERVICE_UUID_DEVICE_INFORMATION            0x180A
#define BTP_SERVICE_UUID_GENERIC_ACCESS                0x1800
#define BTP_SERVICE_UUID_GENERIC_ATTRIBUTE             0x1801
#define BTP_SERVICE_UUID_GLUCOSE                       0x1808
#define BTP_SERVICE_UUID_HEALTH_THERMOMETER            0x1809
#define BTP_SERVICE_UUID_HEART_RATE                    0x180D
#define BTP_SERVICE_UUID_HUMAN_INTERFACE_DEVICE        0x1812
#define BTP_SERVICE_UUID_IMMEDIATE_ALERT               0x1802
#define BTP_SERVICE_UUID_LINK_LOSS                     0x1803
#define BTP_SERVICE_UUID_LOCATION_AND_NAVIGATION       0x1819
#define BTP_SERVICE_UUID_NEXT_DST_CHANGE               0x1807
#define BTP_SERVICE_UUID_PHONE_ALERT_STATUS            0x180E
#define BTP_SERVICE_UUID_REFERENCE_TIME_UPDATE         0x1806
#define BTP_SERVICE_UUID_RUNNING_SPEED_AND_CADENCE     0x1814
#define BTP_SERVICE_UUID_SCAN_PARAMETER                0x1813
#define BTP_SERVICE_UUID_TX_POWER                      0x1804
#define BTP_SERVICE_UUID_USER_DATA                     0x181C
#define BTP_SERVICE_UUID_CONTINUOUS_GLUCOSE_MONITORING 0x181F
#define BTP_SERVICE_UUID_WEIGHT_SCALE                  0x181D
// GATT Characteristic Types
#define BTP_CHARACT_UUID_DEVICE_NAME        0x2A00 // Device Name
#define BTP_CHARACT_UUID_APPEARANCE         0x2A01 // Appearance
#define BTP_CHARACT_UUID_PERIPHERAL_PRIVACY 0x2A02 // Peripheral Privacy Flag
#define BTP_CHARACT_UUID_RECONNRCT_ADDRESS  0x2A03 // Reconnection Address
#define BTP_CHARACT_UUID_PERI_CONN_PARAM    0x2A04 // Peripheral Preferred Connection Parameters
#define BTP_CHARACT_UUID_SERVICE_CHANGE     0x2A05 // Service Changed
#define BTP_CHARACT_UUID_BATTERY_LEVEL      0x2A19
// Device Characteristic Types
#define BTP_CHARACT_UUID_MANU_NAME_STRING     0x2A29
#define BTP_CHARACT_UUID_MODEL_NUM_STRING     0x2A24
#define BTP_CHARACT_UUID_SERIAL_NUM_STRING    0x2A25
#define BTP_CHARACT_UUID_HW_REVISION_STRING   0x2A27
#define BTP_CHARACT_UUID_FW_REVISION_STRING   0x2A26
#define BTP_CHARACT_UUID_SW_REVISION_STRING   0x2A28
#define BTP_CHARACT_UUID_SYSTEM_ID            0x2A23
#define BTP_CHARACT_UUID_IEEE_11073_CERT_LIST 0x2A2A
#define BTP_CHARACT_UUID_PNP_ID               0x2A50
// HID Characteristic Types
#define BTP_CHARACT_UUID_HID_BOOT_KEY_INPUT   0x2A22 //!< HID Boot Keyboard Input Report
#define BTP_CHARACT_UUID_HID_BOOT_KEY_OUTPUT  0x2A32 //!< HID Boot Keyboard Output Report
#define BTP_CHARACT_UUID_HID_BOOT_MOUSE_INPUT 0x2A33 //!< HID Boot Mouse Input Report
#define BTP_CHARACT_UUID_HID_INFORMATION      0x2A4A //!< HID Information
#define BTP_CHARACT_UUID_HID_REPORT_MAP       0x2A4B //!< HID Report Map
#define BTP_CHARACT_UUID_HID_CONTROL_POINT    0x2A4C //!< HID Control Point
#define BTP_CHARACT_UUID_HID_REPORT           0x2A4D //!< HID Report
#define BTP_CHARACT_UUID_HID_PROTOCOL_MODE    0x2A4E //!< HID Protocol Mode


extern const uint08 cBtpUUID16Primary[];
extern const uint08 cBtpUUID16SecondS[];
extern const uint08 cBtpUUID16Charact[];
extern const uint08 cBtpUUID16Include[];
extern const uint08 cBtpUUID16ExtProp[];

extern const uint08 cBtpUUID16CharUserDesc[];
extern const uint08 cBtpUUID16ClientCharCfg[];
extern const uint08 cBtpUUID16ServerCharCfg[];
extern const uint08 cBtpUUID16CharFormat[];
extern const uint08 cBtpUUID16CharAggFormat[];
extern const uint08 cBtpUUID16ValidRange[];
extern const uint08 cBtpUUID16ExtReportRef[];
extern const uint08 cBtpUUID16ReportRef[];
// Service
extern const uint08 cBtpUUID16GapService[];
extern const uint08 cBtpUUID16DevService[];
extern const uint08 cBtpUUID16HidService[];
extern const uint08 cBtpUUID16GattService[];
// Gap Chars
extern const uint08 cBtpUUID16DeviceName[];
extern const uint08 cBtpUUID16Appearance[];
extern const uint08 cBtpUUID16PrivacyFlag[];
extern const uint08 cBtpUUID16ReconnectAddr[];
extern const uint08 cBtpUUID16PeriConnParam[];
// Gatt Chars
extern const uint08 cBtpUUID16ServiceChanged[];
// Battery Chars
extern const uint08 cBtpUUID16BatteryLevel[];
// Dev Chars
extern const uint08 cBtpUUID16PnpID[];
extern const uint08 cBtpUUID16SystemID[];
extern const uint08 cBtpUUID16ManuName[];
extern const uint08 cBtpUUID16ModelNum[];
extern const uint08 cBtpUUID16SerialNum[];
extern const uint08 cBtpUUID16HWVersion[];
extern const uint08 cBtpUUID16FWVersion[];
extern const uint08 cBtpUUID16SWVersion[];
// HID Chars
extern const uint08 cBtpUUID16HidBootKeyInput[];
extern const uint08 cBtpUUID16HidBootKeyOutput[];
extern const uint08 cBtpUUID16HidBootMouseInput[];
extern const uint08 cBtpUUID16HidInformation[];
extern const uint08 cBtpUUID16HidReportMap[];
extern const uint08 cBtpUUID16HidControlPoint[];
extern const uint08 cBtpUUID16HidReport[];
extern const uint08 cBtpUUID16HidProtocolMode[];

#define BTP_GATT_PROPERTY_BROADCAST         0x01 // permit broadcasts of the Characteristic Value
#define BTP_GATT_PROPERTY_READ              0x02 // permit reads of the Characteristic Value
#define BTP_GATT_PROPERTY_WRITE_WITHOUT_RSP 0x04 // Permit writes of the Characteristic Value without response
#define BTP_GATT_PROPERTY_WRITE             0x08 // Permit writes of the Characteristic Value with response
#define BTP_GATT_PROPERTY_NOTIFY            0x10 // Permit notifications of a Characteristic Value without acknowledgement
#define BTP_GATT_PROPERTY_INDICATE          0x20 // Permit indications of a Characteristic Value with acknowledgement
#define BTP_GATT_PROPERTY_AUTHEN            0x40 // permit signed writes to the Characteristic Value
#define BTP_GATT_PROPERTY_EXTENDED          0x80 // additional characteristic properties are defined
#define BTP_GATT_PROPERTY_RDWR              (BTP_GATT_PROPERTY_READ | BTP_GATT_PROPERTY_WRITE)
#define BTP_GATT_PROPERTY_RDWR_NOTY         (BTP_GATT_PROPERTY_READ | BTP_GATT_PROPERTY_WRITE | BTP_GATT_PROPERTY_NOTIFY)


typedef enum {
    BTP_ATT_PERMIT_READ              = 0x01, //!< Attribute is Readable
    BTP_ATT_PERMIT_WRITE             = 0x02, //!< Attribute is Writable
    BTP_ATT_PERMIT_WRITE_WITHOUT_RSP = 0x04,
    BTP_ATT_PERMIT_RDWR              = (BTP_ATT_PERMIT_READ | BTP_ATT_PERMIT_WRITE),

    BTP_ATT_PERMIT_AUTHOR   = 0x10, // Attribute access(Read & Write) requires Authorization
    BTP_ATT_PERMIT_ENCRYPT  = 0x20, // Attribute access(Read & Write) requires Encryption
    BTP_ATT_PERMIT_AUTHEN   = 0x40, // Attribute access(Read & Write) requires Authentication(MITM protection)
    BTP_ATT_PERMIT_SECURE   = 0x80, // Attribute access(Read & Write) requires Secure_Connection
    BTP_ATT_PERMIT_SECURITY = 0xF0,

    BTP_ATT_PERMIT_ENCRYPT_READ  = (BTP_ATT_PERMIT_READ | BTP_ATT_PERMIT_ENCRYPT),  //!< Read requires Encryption
    BTP_ATT_PERMIT_ENCRYPT_WRITE = (BTP_ATT_PERMIT_WRITE | BTP_ATT_PERMIT_ENCRYPT), //!< Write requires Encryption
    BTP_ATT_PERMIT_ENCRYPT_RDWR  = (BTP_ATT_PERMIT_RDWR | BTP_ATT_PERMIT_ENCRYPT), //!< Read & Write requires Encryption

    BTP_ATT_PERMIT_AUTHOR_READ  = (BTP_ATT_PERMIT_AUTHOR | BTP_ATT_PERMIT_READ),   //!< Read requires Authorization
    BTP_ATT_PERMIT_AUTHOR_WRITE = (BTP_ATT_PERMIT_AUTHOR | BTP_ATT_PERMIT_WRITE),  //!< Write requires Authorization
    BTP_ATT_PERMIT_AUTHOR_RDWR = (BTP_ATT_PERMIT_AUTHOR | BTP_ATT_PERMIT_RDWR), //!< Read & Write requires Authorization

    BTP_ATT_PERMIT_AUTHEN_READ =
        (BTP_ATT_PERMIT_AUTHEN | BTP_ATT_PERMIT_ENCRYPT | BTP_ATT_PERMIT_READ),  //!< Read requires Authentication
    BTP_ATT_PERMIT_AUTHEN_WRITE =
        (BTP_ATT_PERMIT_AUTHEN | BTP_ATT_PERMIT_ENCRYPT | BTP_ATT_PERMIT_WRITE), //!< Write requires Authentication
    BTP_ATT_PERMIT_AUTHEN_RDWR = (BTP_ATT_PERMIT_AUTHEN | BTP_ATT_PERMIT_ENCRYPT |
                                  BTP_ATT_PERMIT_RDWR), //!< Read & Write requires Authentication

    BTP_ATT_PERMIT_SECURE_READ  = (BTP_ATT_PERMIT_SECURE | BTP_ATT_PERMIT_ENCRYPT | BTP_ATT_PERMIT_AUTHEN |
                                  BTP_ATT_PERMIT_READ),  //!< Read requires Secure_Connection
    BTP_ATT_PERMIT_SECURE_WRITE = (BTP_ATT_PERMIT_SECURE | BTP_ATT_PERMIT_ENCRYPT | BTP_ATT_PERMIT_AUTHEN |
                                   BTP_ATT_PERMIT_WRITE), //!< Write requires Secure_Connection
    BTP_ATT_PERMIT_SECURE_RDWR  = (BTP_ATT_PERMIT_SECURE | BTP_ATT_PERMIT_ENCRYPT | BTP_ATT_PERMIT_AUTHEN |
                                  BTP_ATT_PERMIT_RDWR),  //!< Read & Write requires Secure_Connection
} BTP_ATT_PERMIT_ENUM;
typedef enum {
    BTP_ATT_OPCODE_NONE                   = 0x00, //
    BTP_ATT_OPCODE_ERROR_RSP              = 0x01, // ATT Error Response
    BTP_ATT_OPCODE_EXCHG_MTU_REQ          = 0x02, // ATT Exchange MTU Request
    BTP_ATT_OPCODE_EXCHG_MTU_RSP          = 0x03, // ATT Exchange MTU Response
    BTP_ATT_OPCODE_FIND_INFO_REQ          = 0x04, // ATT Find Information Request
    BTP_ATT_OPCODE_FIND_INFO_RSP          = 0x05, // ATT Find Information Response
    BTP_ATT_OPCODE_FIND_TYPE_REQ          = 0x06, // ATT Find By Type Value Request
    BTP_ATT_OPCODE_FIND_TYPE_RSP          = 0x07, // ATT Find By Type Value Response
    BTP_ATT_OPCODE_READ_TYPE_REQ          = 0x08, // ATT Read By Type Request
    BTP_ATT_OPCODE_READ_TYPE_RSP          = 0x09, // ATT Read By Type Response
    BTP_ATT_OPCODE_READ_REQ               = 0x0A, // ATT Read Request
    BTP_ATT_OPCODE_READ_RSP               = 0x0B, // ATT Read Response
    BTP_ATT_OPCODE_READ_BLOB_REQ          = 0x0C, // ATT Read Blob Request
    BTP_ATT_OPCODE_READ_BLOB_RSP          = 0x0D, // ATT Read Blob Response
    BTP_ATT_OPCODE_READ_MULTI_REQ         = 0x0E, // ATT Read Multiple Request
    BTP_ATT_OPCODE_READ_MULTI_RSP         = 0x0F, // ATT Read Multiple Response
    BTP_ATT_OPCODE_READ_GROUP_REQ         = 0x10, // ATT Read By Group Type Request
    BTP_ATT_OPCODE_READ_GROUP_RSP         = 0x11, // ATT Read By Group Type Response
    BTP_ATT_OPCODE_WRITE_REQ              = 0x12, // ATT Write Request
    BTP_ATT_OPCODE_WRITE_RSP              = 0x13, // ATT Write Response
    BTP_ATT_OPCODE_PREPARE_WRITE_REQ      = 0x16, // ATT Prepare Write Request
    BTP_ATT_OPCODE_PREPARE_WRITE_RSP      = 0x17, // ATT Prepare Write Response
    BTP_ATT_OPCODE_EXECUTE_WRITE_REQ      = 0x18, // ATT Execute Write Request
    BTP_ATT_OPCODE_EXECUTE_WRITE_RSP      = 0x19, // ATT Execute Write Response
    BTP_ATT_OPCODE_HANDLE_VALUE_NTF       = 0x1B, // ATT Handle Value Notification
    BTP_ATT_OPCODE_HANDLE_VALUE_IND       = 0x1D, // ATT Handle Value Indication
    BTP_ATT_OPCODE_HANDLE_VALUE_CFM       = 0x1E, // ATT Handle Value Confirmation
    BTP_ATT_OPCODE_READ_MULTI_VAR_REQ     = 0x20, // core_5.2
    BTP_ATT_OPCODE_READ_MULTI_VAR_RSP     = 0x21, // core_5.2
    BTP_ATT_OPCODE_MULTI_HANDLE_VALUE_NTF = 0x23, // core_5.2
    BTP_ATT_OPCODE_WRITE_CMD              = 0x52, // ATT Write Command
    BTP_ATT_OPCODE_SINGED_WRITE_CMD       = 0xD2, // ATT Signed Write Command
} BTP_ATT_OPCODE_H;

typedef enum {
    BTP_ATT_ECODE_NONE                = 0x00, // success
    BTP_ATT_ECODE_INVALID_HANDLE      = 0x01, // Attribute handle value given was not valid on this attribute server
    BTP_ATT_ECODE_READ_NOT_PERMITTED  = 0x02, // Attribute cannot be read
    BTP_ATT_ECODE_WRITE_NOT_PERMITTED = 0x03, // Attribute cannot be written
    BTP_ATT_ECODE_INVALID_PDU         = 0x04, // The attribute PDU was invalid
    BTP_ATT_ECODE_INSUFFICIENT_AUTHEN = 0x05, // The attribute requires authentication before it can be read or written
    BTP_ATT_ECODE_UNSUPPORTED_REQ =
        0x06, // Attribute server doesn't support the request received from the attribute client
    BTP_ATT_ECODE_INVALID_OFFSET = 0x07,     // Offset specified was past the end of the attribute
    BTP_ATT_ECODE_INSUFFICIENT_AUTHOR =
        0x08,                                // The attribute requires an authorization before it can be read or written
    BTP_ATT_ECODE_PREPARE_QUEUE_FULL = 0x09, // Too many prepare writes have been queued
    BTP_ATT_ECODE_ATTR_NOT_FOUND     = 0x0a, // No attribute found within the given attribute handle range
    BTP_ATT_ECODE_ATTR_NOT_LONG =
        0x0b, // Attribute cannot be read or written using the Read Blob Request or Prepare Write Request
    BTP_ATT_ECODE_INSUFFICIENT_KEY_SIZE = 0x0c, // The Encryption Key Size used for encrypting this link is insufficient
    BTP_ATT_ECODE_INVALID_VALUE_LENGTH = 0x0d,  // The attribute value length is invalid for the operation
    BTP_ATT_ECODE_UNLIKELY_ERROR = 0x0e, // The attribute request that was requested has encountered an error that was
                                         // very unlikely, and therefore could not be completed as requested
    BTP_ATT_ECODE_INSUFFICIENT_ENCRYPT = 0x0f, // The attribute requires encryption before it can be read or written
    BTP_ATT_ECODE_UNSUPPORTED_GRP_TYPE =
        0x10, // The attribute type is not a supported grouping attribute as defined by a higher layer specification
    BTP_ATT_ECODE_INSUFFICIENT_RESOURCES = 0x11, // Insufficient Resources to complete the request
    BTP_ATT_ECODE_DATABASE_OUT_OF_SYNC   = 0x12, // The server requests the client to rediscover the database
    BTP_ATT_ECODE_VALUE_NOT_ALLOWED      = 0x13, // The attribute parameter value was not allowed
    BTP_ATT_ECODE_APPLICATION_ERROR =
        0x80, // 0x80 – 0x9F Application error code defined by a higher layer specification
    BTP_ATT_ECODE_PROFILE_SERVICE_ERROR =
        0xE0, // 0xE0 – 0xFF Common profile and service error codes defined in Core Specification Supplement, Part B
} BTP_ATT_ECODE_ENUM;


int btp_att_sendErrorRsp(uint16 handle, uint16 chnID, uint08 errOpcode, uint16 errHandle, uint08 errReason);


int btp_att_sendExchangeMtuReq(uint16 handle, uint16 chnID, uint16 mtuSize);
int btp_att_sendExchangeMtuRsp(uint16 handle, uint16 chnID, uint16 mtuSize);

int btp_att_sendFindInfoReq(uint16 handle, uint16 chnID, uint16 startAttHandle, uint16 endAttHandle);
int btp_att_sendFindInfoRsp(uint16 handle, uint16 chnID, uint08 format, uint08 *pData, uint16 dataLen);

int btp_att_sendFindTypeReq(uint16 handle,
                            uint16 chnID,
                            uint16 startAttHandle,
                            uint16 endAttHandle,
                            uint08 *pUuid,
                            uint08 *pData,
                            int valueLen);
int btp_att_sendFindTypeRsp(uint16 handle, uint16 chnID, uint08 *pData, uint16 dataLen);
int btp_att_sendReadTypeReq(uint16 handle,
                            uint16 chnID,
                            uint16 startAttHandle,
                            uint16 endAttHandle,
                            uint08 *uuid,
                            int uuidLen);
int btp_att_sendReadTypeRsp(uint16 handle, uint16 chnID, uint08 typeLen, uint08 *pData, uint08 dataLen);
int btp_att_sendReadGroupReq(uint16 handle,
                             uint16 chnID,
                             uint16 startAttHandle,
                             uint16 endAttHandle,
                             uint08 *uuid,
                             int uuidLen);
int btp_att_sendReadGroupRsp(uint16 handle, uint16 chnID, uint08 typeLen, uint08 *pData, uint16 datalen);

int btp_att_sendReadReq(uint16 handle, uint16 chnID, uint16 attHandle);
int btp_att_sendReadRsp(uint16 handle, uint16 chnID, uint08 *pData, uint08 dataLen);

int btp_att_sendReadBlobReq(uint16 handle, uint16 chnID, uint16 attHandle, uint16 offset);
int btp_att_sendReadBlobRsp(uint16 handle, uint16 chnID, uint08 *pData, uint08 dataLen);

int btp_att_sendWriteReq(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen);
int btp_att_sendWriteRsp(uint16 handle, uint16 chnID);

int btp_att_sendWriteCmd(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen);
int btp_att_sendHandleValueNtf(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen);
int btp_att_sendHandleValueInd(uint16 handle, uint16 chnID, uint16 attHandle, uint08 *pData, uint16 dataLen);
int btp_att_sendHandleValueCfm(uint16 handle, uint16 chnID);

int btp_att_sendHandleValueNtf1(uint16 handle,
                                uint16 chnID,
                                uint16 attHandle,
                                uint08 *pHead,
                                uint08 headLen,
                                uint08 *pData,
                                uint16 dataLen);


#endif // #if (TLKBTP_CFG_ATT_ENABLE)

#endif /* BTP_ATT_STDIO_H */
