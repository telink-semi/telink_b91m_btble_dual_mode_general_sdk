/********************************************************************************************************
 * @file    tlk_aac_interface_api.c
 *
 * @brief   This is the source file for BTBLE SDK
 *
 * @author  BT Audio Group
 * @date    2023
 *
 * @par     Copyright (c) 2023, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "drivers.h"
#include "tlkapi/tlkapi_stdio.h"
#include "tlkalg/audio/aac/tlkalg_aac.h"

#define TLKMDI_ALGAAC_DBG_FLAG       ((TLK_MAJOR_DBGID_MDI_AUDIO << 24) | (TLK_MINOR_DBGID_MDI_AUD_SNK << 16) | TLK_DEBUG_DBG_FLAG_ALL)
#define TLKMDI_ALGAAC_DBG_SIGN       "[MDI]"

void *g_aac_dec_env_buf_ptr = NULL;
void *g_aac_scratch_buf_ptr = NULL;
HANDLE_AACDECODER aacDecoder_handle;


TLKA_AAC_DEC_CFG_PARAS aacParas = {
		.chIdx = AAC_CHANNEL_DEFAULT,   /**< the output channels is matched with input bitstream */
};

/**
 * @brief  Set up the AAC required for the decoding and scratch buffer
 *
 * @param[in]  The address the system  for AAC decodes
 * @param[in]  The AAC module's scratch buffer system pointer
 *
 * @returns Size required for the AAC decoding
*/
int aac_set_dec_buffer_scratch (void *aac_dec_env_buf_ptr, void *aac_scratch_buf_ptr)
{
	int aac_dec_env_size = tlka_aac_dec_get_size();

	aac_dec_env_size = (aac_dec_env_size+3)/4*4;
	if(NULL != aac_dec_env_buf_ptr) {
		g_aac_dec_env_buf_ptr = aac_dec_env_buf_ptr;
	} else {
		tlkapi_trace(TLKMDI_ALGAAC_DBG_FLAG, TLKMDI_ALGAAC_DBG_SIGN, "aac_dec_env_buf_ptr NULL");
		return 0;
	}

	if(NULL != aac_scratch_buf_ptr) {
		g_aac_scratch_buf_ptr = aac_scratch_buf_ptr;
	}  else {
		tlkapi_trace(TLKMDI_ALGAAC_DBG_FLAG, TLKMDI_ALGAAC_DBG_SIGN, "aac_scratch_buf_ptr NULL");
		return 0;
	}

	aacParas.chIdx    = AAC_CHANNEL_DEFAULT;//AAC_CHANNEL_LEFT	AAC_CHANNEL_DEFAULT
	aacDecoder_handle = tlka_aac_dec_init(&aacParas, g_aac_dec_env_buf_ptr, g_aac_scratch_buf_ptr);
	tlkapi_trace(TLKMDI_ALGAAC_DBG_FLAG, TLKMDI_ALGAAC_DBG_SIGN, "dec scr %x", g_aac_dec_env_buf_ptr, g_aac_scratch_buf_ptr);
	return (aac_dec_env_size);
}

/**
 * @brief  The AAC decodes the left channel
 *
 * @param[in]  input the undecoded data pointer
 * @param[in]  input the data length
 * @param[out] Outputs the decoded data pointer
 *
 * @returns Decoding state
*/
int tlkalg_aac_dec_chn_left (const unsigned char *ps, int len, unsigned char *pd)
{
	int ret;
	TLKA_AAC_DEC_INFO aacInfo;

	ret = tlka_aac_dec_process_frame(aacDecoder_handle,					///handle
                                    (unsigned char*)ps, len,			///input addr and size
                                    &len,								///valid packet lenth
                                    (short*)pd,1024*2,					///output addr and size
                                    &aacInfo);							///aac dec information

	return ret;
}

/**
 * @brief  The AAC decodes the right channel
 *
 * @param[in]  input the undecoded data pointer
 * @param[in]  input the data length
 * @param[out] Outputs the decoded data pointer
 *
 * @returns Decoding state
*/
int tlkalg_aac_dec_chn_right (const unsigned char *ps, int len, unsigned char *pd)
{
	uint32_t ret = -1;
	TLKA_AAC_DEC_INFO aacInfo;

	if (ps == NULL || pd == NULL) {
		return ret;
	}

	ret = tlka_aac_dec_process_frame(aacDecoder_handle,
                                    (unsigned char*)ps, len,
                                    &len,
                                    (short*)pd,1024*2,
                                    &aacInfo);

	return ret;
}

/**
 * @brief  The AAC decodes the stereo
 *
 * @param[in]  input the undecoded data pointer
 * @param[in]  input the data length
 * @param[out] Outputs the decoded data pointer
 *
 * @returns Decoding state
*/
int tlkalg_aac_dec_stereo (const unsigned char *ps, int len, unsigned char *pd)
{
	uint32_t ret = -1;
	TLKA_AAC_DEC_INFO aacInfo;
	int valid = len;

	if (ps == NULL || pd == NULL) {
		tlkapi_trace(TLKMDI_ALGAAC_DBG_FLAG, TLKMDI_ALGAAC_DBG_SIGN, "PS or PD null");
		return ret;
	}

	ret = tlka_aac_dec_process_frame(aacDecoder_handle,
									(unsigned char*)ps, len,
									&valid,
									(short*)pd,1024*2,
									&aacInfo);
	if (TLKA_AAC_DEC_OK != ret) {
		tlkapi_trace(TLKMDI_ALGAAC_DBG_FLAG, TLKMDI_ALGAAC_DBG_SIGN, "dec error %d %d %d %x", ret, len, valid, ps);
	}

	return ret;
}

