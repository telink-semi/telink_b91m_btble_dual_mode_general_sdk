/********************************************************************************************************
 * @file	btp_a2dp.h
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
#ifndef BTP_A2DP_H
#define BTP_A2DP_H

#if (TLK_STK_BTP_ENABLE)


typedef void (*BtpA2dpRecvDataCallback)(uint16 aclHandle, uint08 *pData, uint16 dataLen);


typedef enum {
    BTP_A2DP_MODE_AUTO = 0x01, // By default, both Sink and Source are supported
    BTP_A2DP_MODE_SNK,         // Only support Sink
    BTP_A2DP_MODE_SRC,         // Only support Source
} BTP_A2DP_MODE_ENUM;
typedef enum {
    BTP_A2DP_STATUS_CLOSED = 0,
    BTP_A2DP_STATUS_OPENED,
    BTP_A2DP_STATUS_PAUSED,
    BTP_A2DP_STATUS_STREAM,
} BTP_A2DP_STATUS_ENUM;

typedef enum {
    BTP_A2DP_CODEC_SBC    = 0x00,
    BTP_A2DP_CODEC_MPEG12 = 0x01,
    BTP_A2DP_CODEC_MPEG24 = 0x02,
    BTP_A2DP_CODEC_ATRAC  = 0x04,
    BTP_A2DP_CODEC_VENDOR = 0xff,
} BTP_A2DP_CODEC_ENUM;
typedef enum {
    BTP_A2DP_CHN_MODE_MONO         = 0x08,
    BTP_A2DP_CHN_MODE_DUAL         = 0x04,
    BTP_A2DP_CHN_MODE_STEREO       = 0x02,
    BTP_A2DP_CHN_MODE_JOINT_STEREO = 0x01,
} BTP_A2DP_CHN_MODE_ENUM;


typedef enum { // A2DP SPEC 5.1.3 Error Codes
    BTP_A2DP_ERRID_INVALID_CODEC_TYPE        = 0xC1,
    BTP_A2DP_ERRID_UNSUPPORTED_CODEC_TYPE    = 0xC2,
    BTP_A2DP_ERRID_INVALID_SAMPLING_FREQ     = 0xC3,
    BTP_A2DP_ERRID_UNSUPPORTED_SAMPLING_FREQ = 0xC4,
    BTP_A2DP_ERRID_INVALID_CHANNEL_MODE      = 0xC5,
    BTP_A2DP_ERRID_UNSUPPORTED_CHANNEL_MODE    = 0xC6,
    BTP_A2DP_ERRID_INVALID_SUBBANDS          = 0xC7,
    BTP_A2DP_ERRID_UNSUPPORTED_SUBBANDS        = 0xC8,
    BTP_A2DP_ERRID_INVALID_ALLOC_METHOD      = 0xC9,
    BTP_A2DP_ERRID_UNSUPPORTED_ALLOC_METHOD    = 0xCA,
    BTP_A2DP_ERRID_INVALID_MINIMUM_BITPOOL   = 0xCB,
    BTP_A2DP_ERRID_UNSUPPORTED_MINIMUM_BITPOOL = 0xCC,
    BTP_A2DP_ERRID_INVALID_MAXIMUM_BITPOOL   = 0xCD,
    BTP_A2DP_ERRID_UNSUPPORTED_MAXIMUM_BITPOOL = 0xCE,
    BTP_A2DP_ERRID_INVALID_LAYER             = 0xCF,
    BTP_A2DP_ERRID_UNSUPPORTED_LAYER           = 0xD0,
    BTP_A2DP_ERRID_UNSUPPORTED_CRC             = 0xD1,
    BTP_A2DP_ERRID_UNSUPPORTED_MPF             = 0xD2,
    BTP_A2DP_ERRID_UNSUPPORTED_VBR             = 0xD3,
    BTP_A2DP_ERRID_INVALID_BIT_RATE          = 0xD4,
    BTP_A2DP_ERRID_UNSUPPORTED_BIT_RATE        = 0xD5,
    BTP_A2DP_ERRID_INVALID_OBJECT_TYPE       = 0xD6,
    BTP_A2DP_ERRID_UNSUPPORTED_OBJECT_TYPE     = 0xD7,
    BTP_A2DP_ERRID_INVALID_CHANNELS          = 0xD8,
    BTP_A2DP_ERRID_UNSUPPORTED_CHANNELS        = 0xD9,
    BTP_A2DP_ERRID_INVALID_VERSION           = 0xDA,
    BTP_A2DP_ERRID_UNSUPPORTED_VERSION         = 0xDB,
    BTP_A2DP_ERRID_UNSUPPORTED_MAXIMUM_SUL     = 0xDC,
    BTP_A2DP_ERRID_INVALID_BLOCK_LENGTH      = 0xDD,
    // 0xDE-0xDF RFD
    BTP_A2DP_ERRID_INVALID_CP_TYPE           = 0xE0,
    BTP_A2DP_ERRID_INVALID_CP_FORMAT         = 0xE1,
    BTP_A2DP_ERRID_INVALID_CODEC_PARAMETER   = 0xE2,
    BTP_A2DP_ERRID_UNSUPPORTED_CODEC_PARAMETER = 0xE3,
    // 0xE4-0xFF RFD
} BTP_A2DTP_ERRID_ENUM;


#define A2DP_BLOCK_LENGTH_4      (1 << 3)
#define A2DP_BLOCK_LENGTH_8      (1 << 2)
#define A2DP_BLOCK_LENGTH_12     (1 << 1)
#define A2DP_BLOCK_LENGTH_16     1

#define A2DP_SUBBANDS_4          (1 << 1)
#define A2DP_SUBBANDS_8          1

#define A2DP_ALLOCATION_SNR      (1 << 1)
#define A2DP_ALLOCATION_LOUDNESS 1


/******************************************************************************
 * Function: A2DP initial interface
 * Descript: Defines trigger the initial flow of A2DP sink and A2DP source
 * Params:
 * 		@count[IN]--the A2DP number, refer to TLK_BT_A2DP_MAX_NUMB
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value(-TLK_ENOMEM) is returned means the initial process fail.
 *******************************************************************************/
extern int btp_a2dp_init(uint08 count);

/******************************************************************************
 * Function: A2DP deinitial interface
 * Descript: Defines trigger the initial flow of A2DP sink and A2DP source
 * Params:
 * 		@count[IN]--the A2DP number, refer to TLK_BT_A2DP_MAX_NUMB
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *******************************************************************************/
extern int btp_a2dp_deinit(void);

/******************************************************************************
 * Function: btp_a2dp_getMemLen
 * Descript: A2DP get the size of the required number of nodes interface.
 * Params:
 * 		@count[IN]--the A2DP number, refer to TLK_BT_A2DP_MAX_NUMB
 * Return: Actual node memory size required(unit: byte).
 *******************************************************************************/
extern int btp_a2dp_getMemLen(uint08 count);

/******************************************************************************
 * Function: btp_a2dp_setMode
 * Descript: Example Set the working mode of A2DP
 * Params:
 *     @aclHandle[IN]--The ACL link's handle. If this value is 0, the default
 *         working mode of A2DP is set.
 *     @mode--Work Mode. Refer BTP_A2DP_MODE_ENUM
 *         BTP_A2DP_MODE_AUTO: Both Sink and Source are supported;
 *         BTP_A2DP_MODE_SNK: Only support Sink
 *         BTP_A2DP_MODE_SRC: Only support Source
 * Return: Returning TLK_ENONE(0x00) means the initial process success.
 *         If others value is returned means the initial process fail.
 * Notice:
 *     1. If "aclHandle" is not 0, you need to set the mode immediately after
 *        the A2DP is connected. Otherwise, a connection exception may be raised.
 *     2. If roles have already been identified during the connection process,
 *        you are not allowed to change the current mode again.
 *     3. If the working mode has already been assigned, another assignment
 *        will not be allowed.
 *******************************************************************************/
extern int btp_a2dp_setMode(uint16 aclHandle, uint08 mode);

/******************************************************************************
 * Function: A2DP connect interface
 * Descript: Defines the format of the user trigger connect interface.
 * Params:
 *     @aclHandle[IN]--The ACL link's handle.
 *     @usrID[IN]--The user id use to verify whether user is client or server
 *                  to trigger the connect process
 * Return: Returning TLK_ENONE(0x00) means the connect process success.
 *         If others value is returned means the connect process fail.
 *******************************************************************************/
extern int btp_a2dp_connect(uint16 aclHandle, uint08 usrID);

/******************************************************************************
 * Function: A2DP disconnect interface
 * Descript: Defines the format of the user trigger disconnect interface.
 * Params:
 *     @aclHandle[IN]--The ACL link's handle.
 *     @usrID[IN]--The user id use to verify whether user is client or server
 *                  to trigger the disconnect process
 * Return: Returning TLK_ENONE(0x00) means the disconnect process success.
 *         If others value is returned means the disconnect process fail.
 *******************************************************************************/
extern int btp_a2dp_disconn(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP destroy interface
 * Descript: Defines the format of the user trigger destroy interface.
 * Params:
 *     @aclHandle[IN]--The ACL link's handle.
 * Return: null
 *******************************************************************************/
extern void btp_a2dp_destroy(uint16 aclHandle);

extern bool btp_a2dp_isSnk(uint16 aclHandle);
extern bool btp_a2dp_isSrc(uint16 aclHandle);

extern void btp_a2dp_srcEnable(bool enable);
extern void btp_a2dp_snkEnable(bool enable);


extern bool btp_a2dp_isStart(uint16 aclHandle);

extern uint16 btp_a2dp_getSrcHandle(void);
extern uint16 btp_a2dp_getSnkHandle(void);


/******************************************************************************
 * The a2dp source interface
 ******************************************************************************/

#if (TLKBTP_CFG_A2DPSRC_ENABLE)
/******************************************************************************
 * Function: A2DP source send media data interface
 * Descript: Defines the format of the A2dp source send media data to sink device.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 *      @packet[IN]--the stream data from audio part.
 *      @size[IN]--the stream data size.
 *      @marker[IN]--the marker.
 * Return: Returning TLK_ENONE(0x00) means the send process success.
 * 		If others value is returned means the send process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_sendMediaData(uint16 aclHandle,
                                     uint16 seqNumber,
                                     uint32 timestamp,
                                     uint08 *pPayload,
                                     uint16 payloadSize);

/******************************************************************************
 * Function: A2DP source start interface
 * Descript: Defines the format of the A2dp source trigger start stream.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the start process success.
 * 		If others value is returned means the start process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_start(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source suspend interface
 * Descript: Defines the format of the A2dp source trigger suspend stream.
 * Params:
 *   	@aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the suspend process success.
 * 		If others value is returned means the suspend process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_suspend(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source stop interface
 * Descript: Defines the format of the A2dp source trigger stop stream.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the stop process success.
 * 		If others value is returned means the stop process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_close(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source abort interface
 * Descript: Defines the format of the A2dp source trigger abort the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the abort process success.
 * 		If others value is returned means the abort process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_abort(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source reconfig interface
 * Descript: Defines the format of the A2dp source trigger reconfig
 *           the codec's parameter.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 *      @sampleRate[IN]--16K, 32K, 44.1K, 48K
 * Return: Returning TLK_ENONE(0x00) means the reconfig process success.
 * 		If others value is returned means the reconfig process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_reconfig(uint16 aclHandle, uint32 sampleRate);

/******************************************************************************
 * Function: btp_a2dpsrc_restart
 * Descript: Defines the format of the A2dp source trigger restart
 *           the codec's parameter.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the restart process success.
 * 		If others value is returned means the restart process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_restart(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source get A2DP connection status
 * Descript: Defines the format of the A2dp source trigger get status
 *           of the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the get process success.
 * 		If others value is returned means the get process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_getStatus(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source verify a2dp stream status
 * Descript: Defines the format of the A2dp source trigger to get whether
 *           the a2dp is stream or not.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning true(0x00) means the a2dp in streaming.
 * 		If false is returned means the a2dp in others status.
 *******************************************************************************/
extern bool btp_a2dpsrc_isInStream(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP source set a2dp samplerate
 * Descript: Defines the format of the A2dp source trigger to set the samplerate
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * 	    @sampleRate[IN]--The codec's samplerate
 * Return: Returning TLK_ENONE(0x00) means set success.
 * 		If other's value is returned means set fail.
 *******************************************************************************/
extern int btp_a2dpsrc_setSampleRate(uint16 aclHandle, uint32 sampleRate);

/******************************************************************************
 * Function: btp_a2dpsnk_getSampleRate
 * Descript: Get Sample Rate for Codec.
 * Params:
 * 	    @pA2dp[IN]--The A2dp control bolck which convey the parameter.
 * Return: SampleRate.
 *******************************************************************************/
extern uint btp_a2dpsrc_getSampleRate(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP Source get capability interface
 * Descript: Defines trigger the AVDTP send get capability command to peer AVDTP entity.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) or -TLK_EBUSY means the close process success.
 * 		If others value is returned means the abort process fail.
 *******************************************************************************/
int btp_a2dpsrc_getAllCapability(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP Source set config interface
 * Descript: Defines trigger the AVDTP set config command to peer AVDTP entity.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) or -TLK_EBUSY means the close process success.
 * 		If others value is returned means the abort process fail.
 *******************************************************************************/
int btp_a2dpsrc_setConfiguration(uint16 aclHandle);
extern uint btp_a2dpsrc_getBitpool(uint16 aclHandle);
extern uint btp_a2dpsrc_getBlockSize(uint16 aclHandle);
extern int btp_a2dpsrc_sendDiscoveryCmd(uint16 aclHandle);

#endif // #if (TLKBTP_CFG_A2DPSRC_ENABLE)

#if (TLKBTP_CFG_A2DPSNK_ENABLE)

/******************************************************************************
 * Function: A2DP sink suspend interface
 * Descript: Defines the format of the A2dp sink trigger suspend the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) or -TLK_EBUSY means the suspend process success.
 * 		If others value is returned means the abort process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_suspend(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP sink start interface
 * Descript: Defines the format of the A2dp source trigger start stream.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the start process success.
 * 		If others value is returned means the start process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_start(uint16 aclHandle);

/********************************************************************************************
 * The a2dp Sink Interface
 *******************************************************************************************/

/******************************************************************************
 * Function: btp_a2dpsnk_getSampleRate
 * Descript: Get Sample Rate for Codec.
 * Params:
 * 	    @pA2dp[IN]--The A2dp control bolck which convey the parameter.
 * Return: SampleRate.
 *******************************************************************************/
extern uint btp_a2dpsnk_getSampleRate(uint16 aclHandle);

/******************************************************************************
 * Function: A2DP sink send reconfig service's parameters interface
 * Descript: Defines the format of the A2dp sink trigger to reconfig
 *           a2dp codec frequency interface.
 * Params:
 * 	@aclHandle[IN]--The ACL link's handle.
 *  @type[IN]--The codec's frequency's type;
 *             0 is 16K, 1 is 32K, 2 is 44.1K, default is 48K
 * Return: Returning TLK_ENONE(0x00) means the reconfig process success.
 * 		   If others value is returned means the reconfig process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_reconfigCodec(uint16 aclHandle, uint08 type);

/******************************************************************************
 * Function: A2DP sink data callback
 * Descript: Defines the format of the A2dp sink application register data
 *           channel's callback to upper layer.
 * Params:
 * 	    @datCallback[IN]--The a2dp sink data callback function.
 * Return:null.
 *
 *******************************************************************************/
extern void btp_a2dpsnk_regRecvDataCB(BtpA2dpRecvDataCallback datCallback);

/******************************************************************************
 * Function: A2DP sink send delay report
 * Descript: Defines the delay report function for audio control
 *           the A2DP stream delay to peer device, because there exists
 *           some time delay including decoding, buffer or else some other reason
 * Params:
 * 	    @aclHandle[IN]--The a2dp sink handle.
 *      @delay[IN]--The delay time.
 * Return:null.
 *******************************************************************************/
extern int btp_a2dpsnk_delayreport(uint16 aclHandle, uint16 delay);

/******************************************************************************
 * Function: A2DP source get A2DP connection status
 * Descript: Defines the format of the A2dp source trigger get status
 *           of the connection.
 * Params:
 * 	    @aclHandle[IN]--The ACL link's handle.
 * Return: Returning TLK_ENONE(0x00) means the get process success.
 * 		If others value is returned means the get process fail.
 *******************************************************************************/
extern int btp_a2dpsnk_getStatus(uint16 aclHandle);
/******************************************************************************
 * Function: btp_a2dpsrc_setCodecCapacity
 * Descript: Set the capabilities supported by codec.
 * Params:
 * 	    @chnMode[IN]-- refer to "BTP_A2DP_CHN_MODE_ENUM".
 * 	    @frequency[IN]-- 16k->0x08, 32k->0x04, 44.1k->0x02, 48k->0x01.
 * 	    @allocaMethod[IN]-- SNR->0x02, LOUDNESS->0x01.
 * 	    @subbands[IN]-- SUBBANDS_4->0x02, SUBBANDS_8->0x01.
 * 	    @blockLength[IN]-- LENGTH_4->0x08, LENGTH_8->0x04, LENGTH_12->0x02, LENGTH_16->0x01.
 * 	    @minBitpool[IN]-- refer to A2DP_v1.3.2 Spec Page 19 "SBC".
 * 	    @maxBitpool[IN]-- refer to A2DP_v1.3.2 Spec Page 19 "SBC".
 * Return: Returning TLK_ENONE(0x00) means the get process success.
 * 		If others value is returned means the get process fail.
 *******************************************************************************/
extern int btp_a2dpsrc_setCodecCapacity(uint08 chnMode,
                                        uint08 frequency,
                                        uint08 allocaMethod,
                                        uint08 subbands,
                                        uint08 blockLength,
                                        uint08 minBitpool,
                                        uint08 maxBitpool);

#endif // #if (TLKBTP_CFG_A2DPSNK_ENABLE)


#endif // #if (TLK_STK_BTP_ENABLE)

#endif /* BTP_A2DP_H */
