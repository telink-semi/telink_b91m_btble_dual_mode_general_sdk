/********************************************************************************************************
 * @file    tlkalg_aac.h
 *
 * @brief   This is the header file for BTBLE SDK
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

/*! Construct version number from major/minor/micro values. */
#define AACDEC_VERSION_INT(major, minor, micro) \
  (((major) << 16) | ((minor) << 8) | (micro))

/*! Version number to ensure header and binary are matching. */
#define AACDEC_VERSION AACDEC_VERSION_INT(0, 3, 3)


typedef struct AAC_DECODER_INSTANCE
    *HANDLE_AACDECODER; /*!< Pointer to a AAC decoder instance. */

typedef enum {
    AAC_CHANNEL_DEFAULT = 0,  /* the output channels is matched with input bitstream */
    AAC_CHANNEL_LEFT = 1,     /* only output left channel */
    AAC_CHANNEL_RIGHT = 2,    /* only output right channel */
} AAC_CHANNEL_IDX;

/* error code */
typedef enum
{
    TLKA_AAC_DEC_OK = 0,
	TLKA_AAC_DEC_FILL_ERR,
	TLKA_AAC_DEC_DECODE_ERR,
    TLKA_AAC_DEC_PARA_END
} TLKA_AAC_DEC_ERROR;

typedef struct {
    AAC_CHANNEL_IDX chIdx;
}TLKA_AAC_DEC_CFG_PARAS;

/**
 * \brief This structure gives information about the currently decoded audio
 * data. All fields are read-only.
 */
typedef struct {
  /* These five members are the only really relevant ones for the user. */
  int sampleRate; /*!< The sample rate in Hz of the decoded PCM audio signal. */
  int frameSize;  /*!< The frame size of the decoded PCM audio signal. */
  int channels;   /*!< The number of output audio channels */
} TLKA_AAC_DEC_INFO;

/*! Return library version number. It should match AACDEC_VERSION. */
int tlka_aac_dec_get_version(void);

/*
 * return buffer size (bytes) needed by AAC decoder
 */
int tlka_aac_dec_get_size();

/*
 * return scratch buffer size (bytes) needed by AAC decoder
 */
int tlka_aac_dec_get_scratch_size();

/*
 *  \param[in]  pBuf  pointer to a buffer which will be needed by AAC decoder
 *  \param[in]  pScratchBuf  pointer to a scratch buffer which will be needed by AAC decoder
 *  \return handle of AAC decoder
 */
HANDLE_AACDECODER tlka_aac_dec_init(TLKA_AAC_DEC_CFG_PARAS *cfgs, void *pBuf, void *pScratchBuf);

/*
 *  \param[in]  hAacDec  handle of AAC decoder
 *  \param[in]  inBuf    pointer to input buffer
 *  \param[in]  inSize   input buffer size (bytes)
 *  \param[m]   bValid   valid data in input buffer, it will be modified inside tlka_aac_dec_process_frame
 *  \param[out] outBuf   pointer to output buffer
 *  \param[in]  outSize  output buffer size
 *  \return handle of AAC decoder
 */
TLKA_AAC_DEC_ERROR tlka_aac_dec_process_frame(HANDLE_AACDECODER hAacDec,
                     unsigned char*    inBuf,
					 int               inSize,
					 int*              bValid,
					 short*            outBuf,
					 int               outSize,
					 TLKA_AAC_DEC_INFO *pInfo);

/*
 *  \param[in]  hAacDec  handle of AAC decoder
 */
void tlka_aac_dec_free(HANDLE_AACDECODER hAacDec);

#ifndef CODEC_AAC_TLK_AAC_INTERFACE_API_H_
#define CODEC_AAC_TLK_AAC_INTERFACE_API_H_

extern void *g_aac_dec_env_buf_ptr;
extern void *g_aac_scratch_buf_ptr;
extern TLKA_AAC_DEC_CFG_PARAS aacParas;
extern HANDLE_AACDECODER aacDecoder_handle;

/**
 * @brief  Set up the AAC required for the decoding and scratch buffer
 *
 * @param[in]  The address the system  for AAC decodes
 * @param[in]  The AAC module's scratch buffer system pointer
 *
 * @returns Size required for the AAC decoding
*/
extern int aac_set_dec_buffer_scratch (void *p, void *ps);

/**
 * @brief  The AAC decodes the left channel
 *
 * @param[in]  input the undecoded data pointer
 * @param[in]  input the data length
 * @param[out] Outputs the decoded data pointer
 *
 * @returns Decoding state
*/
extern int tlkalg_aac_dec_chn_left (const unsigned char *ps, int len, unsigned char *pd);

/**
 * @brief  The AAC decodes the right channel
 *
 * @param[in]  input the undecoded data pointer
 * @param[in]  input the data length
 * @param[out] Outputs the decoded data pointer
 *
 * @returns Decoding state
*/
extern int tlkalg_aac_dec_chn_right (const unsigned char *ps, int len, unsigned char *pd);

/**
 * @brief  The AAC decodes the stereo
 *
 * @param[in]  input the undecoded data pointer
 * @param[in]  input the data length
 * @param[out] Outputs the decoded data pointer
 *
 * @returns Decoding state
*/
extern int tlkalg_aac_dec_stereo (const unsigned char *ps, int len, unsigned char *pd);

#endif /* CODEC_AAC_TLK_AAC_INTERFACE_API_H_ */
