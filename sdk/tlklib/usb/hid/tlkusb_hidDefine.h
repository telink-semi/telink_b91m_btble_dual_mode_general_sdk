/********************************************************************************************************
 * @file	tlkusb_hidDefine.h
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
#ifndef TLKUSB_HID_DEFINE_H
#define TLKUSB_HID_DEFINE_H

#if (TLK_USB_HID_ENABLE)


typedef enum{
	#if (TLKUSB_HID_MOUSE_ENABLE)
	TLKUSB_HID_INF_MOUSE,
	#endif
	#if (TLKUSB_HID_KEYBOARD_ENABLE)
	TLKUSB_HID_INF_KEYBOARD,
	#endif
	TLKUSB_HID_INF_MAX,
}TLKUSB_HID_INF_ENUM;

typedef enum{
	TLKUSB_HID_EDP_MOUSE_IN    = 1,
	TLKUSB_HID_EDP_KEYBOARD_IN = 2,
}TLKUSB_HID_EDP_ENUM;

typedef enum{
	TLKUSB_HID_DESC_HID = 0x21,
	TLKUSB_HID_DESC_REPORT = 0x22,
	TLKUSB_HID_DESC_PHYSICAL = 0x23,
}TLKUSB_HID_DESC_ENUM;




typedef enum{
	TLKUSB_HID_CSCP_NonBootSubclass = 0x00,
	TLKUSB_HID_CSCP_NonBootProtocol = 0x00,
	TLKUSB_HID_CSCP_BootSubclass    = 0x01,
	TLKUSB_HID_CSCP_KeyboardBootProtocol = 0x01,
	TLKUSB_HID_CSCP_MouseBootProtocol,
	TLKUSB_HID_CSCP_HIDClass,
}TLKUSB_HID_CSCP_ENUM; // CSCP = ClassSubClassProtocol



#define TLKUSB_HID_RPT_DATA_SIZE_MASK                   (0x03)
#define TLKUSB_HID_RPT_TYPE_MASK                        (0x0C)
#define TLKUSB_HID_RPT_TAG_MASK                         (0xF0)

#define TLKUSB_HID_RPT_TYPE_MAIN                        (0x00)
#define TLKUSB_HID_RPT_TYPE_GLOBAL                      (0x04)
#define TLKUSB_HID_RPT_TYPE_LOCAL                       (0x08)

#define TLKUSB_HID_RPT_DATA_BITS_0                      (0x00)
#define TLKUSB_HID_RPT_DATA_BITS_8                      (0x01)
#define TLKUSB_HID_RPT_DATA_BITS_16                     (0x02)
#define TLKUSB_HID_RPT_DATA_BITS_32                     (0x03)
#define TLKUSB_HID_RPT_DATA_BITS(Data_bits)             TLKUSB_HID_RPT_DATA_BITS_##Data_bits

#define TLKUSB_HID_RPT_DATA_ENCODE_0(Data)
#define TLKUSB_HID_RPT_DATA_ENCODE_8(Data)              , (Data & 0xFF)
#define TLKUSB_HID_RPT_DATA_ENCODE_16(Data)             TLKUSB_HID_RPT_DATA_ENCODE_8(Data)  TLKUSB_HID_RPT_DATA_ENCODE_8(Data>>8)
#define TLKUSB_HID_RPT_DATA_ENCODE_32(Data)             TLKUSB_HID_RPT_DATA_ENCODE_16(Data) TLKUSB_HID_RPT_DATA_ENCODE_16(Data>>16)
#define TLKUSB_HID_RPT_DATA_ENCODE(Data_bits, ...)      TLKUSB_HID_RPT_DATA_ENCODE_##Data_bits(__VA_ARGS__)


#define TLKUSB_HID_RPT_DATA_ENTRY(Type, Tag, Data_bits, ...)  \
			(Type | Tag | TLKUSB_HID_RPT_DATA_BITS(Data_bits)) TLKUSB_HID_RPT_DATA_ENCODE(Data_bits, (__VA_ARGS__))

//data item for Input/Output/Feature
#define TLKUSB_HID_IOF_CONSTANT                          BIT(0)
#define TLKUSB_HID_IOF_DATA                              (0<<0)
#define TLKUSB_HID_IOF_VARIABLE                          BIT(1)
#define TLKUSB_HID_IOF_ARRAY                             (0<<1)
#define TLKUSB_HID_IOF_RELATIVE                          BIT(2)
#define TLKUSB_HID_IOF_ABSOLUTE                          (0<<2)
#define TLKUSB_HID_IOF_WRAP                              BIT(3)
#define TLKUSB_HID_IOF_NO_WRAP                           (0<<3)
#define TLKUSB_HID_IOF_NON_LINEAR                        BIT(4)
#define TLKUSB_HID_IOF_LINEAR                            (0<<4)
#define TLKUSB_HID_IOF_NO_PREFERRED_STATE                BIT(5)
#define TLKUSB_HID_IOF_PREFERRED_STATE                   (0<<5)
#define TLKUSB_HID_IOF_NULLSTATE                         BIT(6)
#define TLKUSB_HID_IOF_NO_NULL_POSITION                  (0<<6)
#define TLKUSB_HID_IOF_VOLATILE                          BIT(7)
#define TLKUSB_HID_IOF_NON_VOLATILE                      (0<<7)
#define TLKUSB_HID_IOF_BUFFERED_BYTES                    BIT(8)
#define TLKUSB_HID_IOF_BITFIELD                          (0<<8)

//HID report definitions
#define TLKUSB_HID_RPT_INPUT(Data_bits, ...)             TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_MAIN  , 0x80, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_OUTPUT(Data_bits, ...)            TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_MAIN  , 0x90, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_COLLECTION(Data_bits, ...)        TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_MAIN  , 0xA0, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_FEATURE(Data_bits, ...)           TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_MAIN  , 0xB0, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_END_COLLECTION(Data_bits, ...)    TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_MAIN  , 0xC0, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_USAGE_PAGE(Data_bits, ...)        TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x00, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_LOGICAL_MINIMUM(Data_bits, ...)   TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x10, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_LOGICAL_MAXIMUM(Data_bits, ...)   TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x20, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_PHYSICAL_MINIMUM(Data_bits, ...)  TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x30, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_PHYSICAL_MAXIMUM(Data_bits, ...)  TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x40, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_UNIT_EXPONENT(Data_bits, ...)     TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x50, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_UNIT(Data_bits, ...)              TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x60, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_REPORT_SIZE(Data_bits, ...)       TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x70, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_REPORT_ID(Data_bits, ...)         TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x80, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_REPORT_COUNT(Data_bits, ...)      TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0x90, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_PUSH(Data_bits, ...)              TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0xA0, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_POP(Data_bits, ...)               TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_GLOBAL, 0xB0, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_USAGE(Data_bits, ...)             TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_LOCAL , 0x00, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_USAGE_MINIMUM(Data_bits, ...)     TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_LOCAL , 0x10, Data_bits, __VA_ARGS__)
#define TLKUSB_HID_RPT_USAGE_MAXIMUM(Data_bits, ...)     TLKUSB_HID_RPT_DATA_ENTRY(TLKUSB_HID_RPT_TYPE_LOCAL , 0x20, Data_bits, __VA_ARGS__)

//country code
typedef enum{
	TLKUSB_HID_COUNTRY_NONE = 0,
	TLKUSB_HID_COUNTRY_ARABIC,
	TLKUSB_HID_COUNTRY_BELGIAN,
	TLKUSB_HID_COUNTRY_CANADA_BI,
	TLKUSB_HID_COUNTRY_CANADA_FR,
	TLKUSB_HID_COUNTRY_CZECH_REPUBLIC,
	TLKUSB_HID_COUNTRY_DANISH,
	TLKUSB_HID_COUNTRY_FINNISH,
	TLKUSB_HID_COUNTRY_FRENCH,
	TLKUSB_HID_COUNTRY_GERMAN,
	TLKUSB_HID_COUNTRY_GREEK,
	TLKUSB_HID_COUNTRY_HEBREW,
	TLKUSB_HID_COUNTRY_HUNGARY,
	TLKUSB_HID_COUNTRY_INTERNATIONAL_ISO,
	TLKUSB_HID_COUNTRY_ITALIAN,
	TLKUSB_HID_COUNTRY_JAPAN_KATAKANA,
	TLKUSB_HID_COUNTRY_KOREAN,
	TLKUSB_HID_COUNTRY_LATIN_AMERICAN,
	TLKUSB_HID_COUNTRY_NETHERLANDS,
	TLKUSB_HID_COUNTRY_NORWEGIAN,
	TLKUSB_HID_COUNTRY_PERSIAN,
	TLKUSB_HID_COUNTRY_POLAND,
	TLKUSB_HID_COUNTRY_PORTUGUESE,
	TLKUSB_HID_COUNTRY_RUSSIA,
	TLKUSB_HID_COUNTRY_SLOVAKIA,
	TLKUSB_HID_COUNTRY_SPANISH,
	TLKUSB_HID_COUNTRY_SWEDISH,
	TLKUSB_HID_COUNTRY_SWISS_FRENCH,
	TLKUSB_HID_COUNTRY_SWISS_GERMAN,
	TLKUSB_HID_COUNTRY_SWITZERLAND,
	TLKUSB_HID_COUNTRY_TAIWAN,
	TLKUSB_HID_COUNTRY_TURKISH_Q,
	TLKUSB_HID_COUNTRY_UK,
	TLKUSB_HID_COUNTRY_US,
	TLKUSB_HID_COUNTRY_YUGOSLAVIA,
	TLKUSB_HID_COUNTRY_TURKISH_F,
}TLKUSB_HID_COUNTRY_ENUM;



#endif //#if (TLK_USB_HID_ENABLE)

#endif //TLKUSB_HID_DEFINE_H

