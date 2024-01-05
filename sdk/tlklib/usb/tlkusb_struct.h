/********************************************************************************************************
 * @file	tlkusb_struct.h
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
#ifndef TLKUSB_STRUCT_H
#define TLKUSB_STRUCT_H




typedef struct
{
	uint08 bReqType; /**< Type of the request. */
	uint08 bRequest; /**< Request command code. */
	uint16 wValue; /**< wValue parameter of the request. */
	uint16 wIndex; /**< wIndex parameter of the request. */
	uint16 wLength; /**< Length of the data to transfer in bytes. */
}tlkusb_setup_req_t;


/** \brief Standard USB String Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard descriptors, the length
 *  of the descriptor for placement in the descriptor header must be determined by the \ref USB_STRING_LEN()
 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the supported language IDs for
 *  the device as an array.
 *
 *  This structure uses the relevant standard's given element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_String_t for the version of this type with with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint16 bString[]; /**< String data, as unicode characters (alternatively, string language IDs).
	                     *   If normal ASCII characters are to be used, they must be added as an array
	                     *   of characters rather than a normal C string so that they are widened to
	                     *   Unicode size.
	                     *
	                     *   Under GCC, strings prefixed with the "L" character (before the opening string
	                     *   quotation mark) are considered to be Unicode strings, and may be used instead
	                     *   of an explicit array of ASCII characters.
	                     */
}__attribute__((packed)) tlkusb_stdStringDesc_t;


/** \brief Standard USB Device Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength;
	uint08 bDescriptorType;
	uint16 bcdUSB; /**< BCD of the supported USB specification. */
	uint08 bDeviceClass; /**< USB device class. */
	uint08 bDeviceSubClass; /**< USB device subclass. */
	uint08 bDeviceProtocol; /**< USB device protocol. */
	uint08 bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	uint16 idVendor; /**< Vendor ID for the USB product. */
	uint16 idProduct; /**< Unique product ID for the USB product. */
	uint16 bcdDevice; /**< Product release (version) number. */
	uint08 iManufacturer; /**< String index for the manufacturer's name.*/
	uint08 iProduct; /**< String index for the product name/details.*/
	uint08 iSerialNumber; /**< String index for the product's globally unique hexadecimal serial number, in uppercase Unicode ASCII.*/
	uint08 bNumConfigurations; /**< Total number of configurations supported by the device. */
}__attribute__((packed)) tlkusb_stdDeviceDesc_t;

/** \brief Standard USB Device Qualifier Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Qualifier Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_DeviceQualifier_t for the version of this type with non-standard LUFA specific element names.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint16 bcdUSB; /**< BCD of the supported USB specification. */
	uint08 bDeviceClass; /**< USB device class. */
	uint08 bDeviceSubClass; /**< USB device subclass. */
	uint08 bDeviceProtocol; /**< USB device protocol. */
	uint08 bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	uint08 bNumConfigurations; /**< Total number of configurations supported by the device.*/
	uint08 bReserved; /**< Reserved for future use, must be 0. */
}__attribute__((packed)) tlkusb_stdQualifierDesc_t;

/** \brief Standard USB Configuration Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint16 wTotalLength; /**< Size of the configuration descriptor header, and all sub descriptors inside the configuration.*/
	uint08 bNumInterfaces; /**< Total number of interfaces in the configuration. */
	uint08 bConfigurationValue; /**< Configuration index of the current configuration. */
	uint08 iConfiguration; /**< Index of a string descriptor describing the configuration. */
	uint08 bmAttributes; /**< Configuration attributes, comprised of a mask of \c USB_CONFIG_ATTR_* masks.*/
	uint08 bMaxPower; /**< Maximum power consumption of the device while in the current configuration, calculated by the \ref USB_CONFIG_POWER_MA() macro.*/
}__attribute__((packed)) tlkusb_stdConfigureDesc_t;

/** \brief Standard USB Interface Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref tlkusb_stdInterfaceDesc_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint08 bInterfaceNumber; /**< Index of the interface in the current configuration. */
	uint08 bAlternateSetting; /**< Alternate setting for the interface number.*/
	uint08 bNumEndpoints; /**< Total number of endpoints in the interface. */
	uint08 bInterfaceClass; /**< Interface class ID. */
	uint08 bInterfaceSubClass; /**< Interface subclass ID. */
	uint08 bInterfaceProtocol; /**< Interface protocol ID. */
	uint08 iInterface; /**< Index of the string descriptor describing the interface.*/
}__attribute__((packed)) tlkusb_stdInterfaceDesc_t;

/** \brief Standard USB Endpoint Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  \see \ref tlkusb_stdEndpointDesc_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class.*/
	uint08 bEndpointAddress; /**< Logical address of the endpoint within the device for the current configuration, including direction mask.*/
	uint08 bmAttributes; /**< Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*) and attributes (ENDPOINT_ATTR_*) masks. */
	uint16 wMaxPacketSize; /**< Size of the endpoint bank, in bytes. This indicates the maximum packet size that the endpoint can receive at a time.*/
	uint08 bInterval; /**< Polling interval in milliseconds for the endpoint if it is an INTERRUPT or ISOCHRONOUS type.*/
}__attribute__((packed)) tlkusb_stdEndpointDesc_t;

/** \brief Standard USB Interface Association Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
 *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
 *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_Descriptor_Interface_Association_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	uint08 bLength; /**< Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 bFirstInterface; /**< Index of the first associated interface. */
	uint08 bInterfaceCount; /**< Total number of associated interfaces. */
	uint08 bFunctionClass; /**< Interface class ID. */
	uint08 bFunctionSubClass; /**< Interface subclass ID. */
	uint08 bFunctionProtocol; /**< Interface protocol ID. */
	uint08 iFunction; /**< Index of the string descriptor describing the interface association. */
}__attribute__((packed)) tlkusb_stdAssociateDesc_t;

typedef struct
{
	uint08 Size; /**< Size of the descriptor, in bytes. */
    uint08 Type; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint16 HIDSpec; /*BCD of the supported HID specification.*/
	uint08 CountryCode; /*The country code of the country where the hardware device is located*/
	uint08 TotalReportDescriptors;	/*The number of the accessory descriptor*/
	uint08 HIDReportType;	/*The type of the accessory descriptor*/
	uint16 HIDReportLength;	/*The length of the accessory descriptor*/
}tlkusb_HidEndpointDesc_t;


typedef struct
{
	uint08 bLength; /* Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /**< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 Subtype; /*Subtype of the descriptors.*/
	uint08 ACSpecification[2]; /*Revision of class specification*/
	uint08 TotalLength[2];	/*Total size of class specific descriptors.*/
	uint08 InCollection; /*Number of streaming interfaces.*/
	uint08 InterfaceNumber;	/*AudioStreaming interface belongs to AudioControl interface.Multiple connections follow in turn*/
}__attribute__((packed)) tlkusb_uacInterfaceAcDesc_t;
typedef struct
{
	uint08 bLength; /* Size of the descriptor, in bytes. */
	uint08 bDescriptorType; /*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 Subtype;	/*Subtype of the descriptor.*/
	uint08 ACSpecification[2]; /*Revision of class specification - 1.0*/
	uint08 TotalLength[2];	/*Total size of class specific descriptors.*/
	uint08 InCollection;	/*Number of streaming interfaces.*/
	uint08 InterfaceNumber_spk;	/*AudioStreaming interface belongs to AudioControl interface.Multiple connections follow in turn*/
	uint08 InterfaceNumber_mic;	/*AudioStreaming interface belongs to AudioControl interface.Multiple connections follow in turn*/
}__attribute__((packed)) tlkusb_uacInterfaceAcTLDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype; /*Subtype of the descriptor.*/
	uint08 TerminalLink; /*The Terminal ID of the Terminal to which the endpoint of this interface is connected.*/
	uint08 Delay; /*Delay introduced by the data path. Expressed in number of frames.*/
	uint08 AudioFormat[2]; /*The Audio Data Format that has to be used to communicate with this interface.*/
}__attribute__((packed)) tlkusb_uacInterfaceAsDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype;	/*Subtype of the descriptor.*/
	uint08 TerminalID;	/*Terminal ID,either a value in \ref TLKUSB_AUDID_ENUM,	Constant uniquely identifying theTerminal within the audio function. */
	uint16 TerminalType; /*Constant characterizing the type of Terminal. */
	uint08 AssocTerminal; /*ID of the Output Terminal to which this Input Terminal is associated.*/
	uint08 TotalChannels; /*Number of logical output channels in the Terminals output audio channel cluster.*/
	uint16 ChannelConfig; /*Describes the spatial location of the logical channels.*/
	uint08 ChannelStrIndex; /*Index of a string descriptor, describing the name of the first logical channel.*/
	uint08 TerminalStrIndex; /*Index of a string descriptor, describing the Input Terminal.*/
}__attribute__((packed)) tlkusb_uacInputDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype;	/*Subtype of the descriptor.*/
	uint08 TerminalID; /*Terminal ID,either a value in \ref TLKUSB_AUDID_ENUM,	Constant uniquely identifying theTerminal within the audio function. */
	uint16 TerminalType; /*Constant characterizing the type of Terminal. */
	uint08 AssocTerminal; /*ID of the Iutput Terminal to which this Output Terminal is associated.*/
	uint08 SourceID; /*ID of the Unit or Terminal to which this Terminal is connected.*/
	uint08 Terminal; /*Index of a string descriptor, describing the Output Terminal.*/
}__attribute__((packed)) tlkusb_uacOutputDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype;	/*Subtype of the descriptor.*/
	uint08 UnitID; /*Constant uniquely identifying the Unit within the audio function. This value is used in all requests to address this Unit.*/
	uint08 SourceID;/*ID of the Unit or Terminal to which this Feature Unit is connected.*/
	uint08 ControlSize; /*Size in bytes of an element of the MAControls */
	uint08 MAControls[2]; /*An array of bit-maps, each indicating the availability of certain audio Controls for a specific logical channel or for the master channel 0*/
	uint08 FeatureUnitStrIndex; /*Index of a string descriptor, describing this Feature Unit.*/
}__attribute__((packed)) tlkusb_uacSingleFeatureDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype;	/*Subtype of the descriptor.*/
	uint08 UnitID; /*Constant uniquely identifying the Unit within the audio function. This value is used in all requests to address this Unit.*/
	uint08 SourceID; /*ID of the Unit or Terminal to which this Feature Unit is connected.*/
	uint08 ControlSize; /*Size in bytes of an element of the MAControls */
	uint08 MAControls[3];/*An array of bit-maps, each indicating the availability of certain audio Controls for a specific logical channel or for the master channel 0*/
	uint08 FeatureUnitStrIndex; /*Index of a string descriptor, describing this Feature Unit.*/
}__attribute__((packed)) tlkusb_uacDoubleFeatureDesc_t;

typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype;	/*Subtype of the descriptor.*/
	uint08 FormatType; /*Audio format type.*/
	uint08 NrChannels; /*Number of physical channels supported by the interface*/
	uint08 SubFrameSize; /*Bytes of data per channel.*/
	uint08 BitResolution; /*The number of significant bits in SubFrameSize.*/
	uint08 SampleFrequencyType; /*Sample type. 0:Continuous sampling rate, other:The number of supported discrete sampling frequencies*/
	uint08 tSamFreq[3]; /*Sample rate*/
}__attribute__((packed)) tlkusb_uacFormatDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 EndpointAddress; /*The address of the endpoint. bit[7]: Direction,bit[3-0]: The endpoint number,bit[6-4]: Reserved.*/
	uint08 MAttributes; /*Synchronization type.bit[1-0]:Transfer type. bit[3-2]:Synchronisation Type,bit[5-4]:Usage type.*/
	uint16 MaxPacketSize; /*The maximum packet length of the endpoint's data transfer*/
	uint08 Interval; /*Interval for polling endpoint for data transfers expressed in ms.Must be set to 1.*/
	uint08 Refresh; /*Reset to 0.*/
	uint08 SynchAddress; /*The address of the endpoint used to communicate synchronization information if required by this endpoint. Reset to zero if no synchronization pipe is used*/
}__attribute__((packed)) tlkusb_uacStdEndpointDesc_t;
typedef struct
{
	uint08 Length;	/* Size of the descriptor, in bytes. */
	uint08 DescriptorType;	/*< Type of the descriptor, either a value in \ref TLKUSB_TYPE_ENUM or a value given by the specific class. */
	uint08 DescriptorSubtype;	/*Subtype of the descriptor.*/
	uint08 MAttributes;/*A bit in the range bit[6-0] set to 1 indicates that the mentioned Control is supported by this endpoint.bit[0]: Sampling Frequency bit[1]: Pitch bit[6-2]: Reserved bit[7] MaxPacketsOnly.*/
	uint08 LockDelayUnits;/*Indicates the units used for the LockDelay field:0: Undefined  1: Milliseconds 2: Decoded PCM samples 3..255: Reserved*/
	uint08 LockDelay[2]; /*Indicates the time it takes this endpoint to reliably lock its internal clock recovery circuitry. Units used depend on the value of the bLockDelayUnits field.*/
}__attribute__((packed)) tlkusb_uacSpcEndpointDesc_t;





#endif //TLKUSB_STRUCT_H

