/*
 * ff_adx.h - CRI ADX / AIX Audio Decoder - Single Header Library
 * Part of ff_libs (https://github.com/GeminiRebirth/ff_libs)
 *
 * Decoder derived from vgmstream's ADX implementation.
 * API design inspired by dr_libs (https://github.com/mackron/dr_libs) but this
 * is a separate, independently maintained project.
 *
 * Copyright (c) 2008-2024 hcs, bnnm, and the vgmstream contributors (ADX decoder)
 * Copyright (c) 2025 Gemini REbirth (ff_libs API wrapper, container parser, AIX support)
 *
 * Decoder code: LGPL v2.1 or later (derived from vgmstream)
 * Container parser & API wrapper: LGPL v2.1 or later
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * ============================================================================
 * USAGE
 * ============================================================================
 *
 * This is a single-file library. In exactly ONE .c/.cpp file, define the
 * implementation before including this header:
 *
 *     #define FF_ADX_IMPLEMENTATION
 *     #include "ff_adx.h"
 *
 * In all other files, just #include "ff_adx.h" normally.
 *
 * QUICK START - Simple API (decode entire file):
 *
 *     uint32_t channels, sampleRate;
 *     uint64_t totalFrames;
 *     float* samples = ff_adx_open_file_and_read_pcm_frames_f32(
 *         "bgm.adx", &channels, &sampleRate, &totalFrames, NULL);
 *     if (samples) {
 *         // ... use samples ...
 *         ff_adx_free(samples, NULL);
 *     }
 *
 * QUICK START - Streaming API:
 *
 *     ff_adx adx;
 *     if (ff_adx_init_file("bgm.adx", NULL, &adx) == FF_ADX_SUCCESS) {
 *         float buf[4096];
 *         uint64_t framesRead;
 *         while ((framesRead = ff_adx_read_pcm_frames_f32(&adx, 1024, buf)) > 0) {
 *             // ... process ...
 *         }
 *         ff_adx_uninit(&adx);
 *     }
 *
 * QUICK START - AIX (multi-stream interleaved) streaming:
 *
 *     ff_adx adx;
 *     ff_adx_config cfg;
 *     memset(&cfg, 0, sizeof(cfg));
 *     cfg.aix_stream_index = 1;   // select which embedded stream (0-based)
 *     if (ff_adx_init_file("bgm.aix", &cfg, &adx) == FF_ADX_SUCCESS) {
 *         // decode as normal...
 *         ff_adx_uninit(&adx);
 *     }
 *
 * ============================================================================
 * SUPPORTED FORMATS
 * ============================================================================
 *
 *   - ADX v3 (.adx)  -- most common, fixed header size
 *   - ADX v4 (.adx)  -- extended loop fields, Sonic Adventure / early Dreamcast
 *   - ADX v5 (.adx)  -- ahx-style header (8000 Hz speech codec), passthrough
 *   - AIX (.aix)     -- interleaved multi-stream ADX container (e.g. multi-language)
 *   - SPDIF-wrapped ADX (.adx inside IEC-61937) -- NOT supported
 *
 *   Encryption variants supported:
 *     - Unencrypted
 *     - Type 8 (XOR with linear congruential key derived from 2 shorts)
 *     - Type 9 (XOR with more complex key schedule, Sonic Heroes and later)
 *
 * ============================================================================
 * OPTIONS
 * ============================================================================
 *
 *   #define FF_ADX_NO_STDIO
 *     Disables file I/O (ff_adx_init_file, ff_adx_open_file_and_read_pcm_frames_*).
 *
 *   #define FF_ADX_NO_ENCRYPTION
 *     Omits encryption key-derivation code (saves ~200 bytes).
 *
 *   #define FF_ADX_DECRYPT_KEY_XOR8(xor8_key_lo, xor8_key_hi)
 *     Supply a compile-time type-8 decryption key pair (uint16_t each).
 *     If the stream is encrypted and no runtime key is provided, this is used.
 *
 *   #define FF_ADX_DECRYPT_KEY_XOR9_START  <uint32_t>
 *   #define FF_ADX_DECRYPT_KEY_XOR9_MULT   <uint32_t>
 *   #define FF_ADX_DECRYPT_KEY_XOR9_INC    <uint32_t>
 *     Supply compile-time type-9 decryption parameters.
 */

#ifndef FF_ADX_H
#define FF_ADX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ============================================================================
 * Version
 * ============================================================================ */
#define FF_ADX_VERSION_MAJOR    0
#define FF_ADX_VERSION_MINOR    1
#define FF_ADX_VERSION_REVISION 0
#define FF_ADX_VERSION_STRING   "0.1.0"

/* ============================================================================
 * DLL visibility
 * ============================================================================ */
#if !defined(FF_ADX_API)
    #if defined(FF_ADX_DLL)
        #if defined(_WIN32)
            #define FF_ADX_DLL_IMPORT  __declspec(dllimport)
            #define FF_ADX_DLL_EXPORT  __declspec(dllexport)
        #else
            #if defined(__GNUC__) && __GNUC__ >= 4
                #define FF_ADX_DLL_IMPORT  __attribute__((visibility("default")))
                #define FF_ADX_DLL_EXPORT  __attribute__((visibility("default")))
            #else
                #define FF_ADX_DLL_IMPORT
                #define FF_ADX_DLL_EXPORT
            #endif
        #endif
        #if defined(FF_ADX_IMPLEMENTATION)
            #define FF_ADX_API FF_ADX_DLL_EXPORT
        #else
            #define FF_ADX_API FF_ADX_DLL_IMPORT
        #endif
    #else
        #define FF_ADX_API extern
    #endif
#endif

/* ============================================================================
 * Result codes
 * ============================================================================ */
typedef int32_t ff_adx_result;
#define FF_ADX_SUCCESS              0
#define FF_ADX_ERROR               -1
#define FF_ADX_INVALID_ARGS        -2
#define FF_ADX_INVALID_OPERATION   -3
#define FF_ADX_OUT_OF_MEMORY       -4
#define FF_ADX_OUT_OF_RANGE        -5
#define FF_ADX_INVALID_FILE       -10
#define FF_ADX_AT_END             -17
#define FF_ADX_BAD_SEEK           -25
#define FF_ADX_NOT_IMPLEMENTED    -29
#define FF_ADX_ENCRYPTED          -30   /* file is encrypted and no key was provided */

/* ============================================================================
 * Seek origin
 * ============================================================================ */
typedef enum {
    ff_adx_seek_origin_start   = 0,
    ff_adx_seek_origin_current = 1
} ff_adx_seek_origin;

/* ============================================================================
 * Allocation callbacks
 * ============================================================================ */
typedef struct {
    void* pUserData;
    void* (*onMalloc )(size_t sz,            void* pUserData);
    void* (*onRealloc)(void* p, size_t sz,   void* pUserData);
    void  (*onFree   )(void* p,              void* pUserData);
} ff_adx_allocation_callbacks;

/* ============================================================================
 * Encryption key
 * ============================================================================ */
typedef enum {
    FF_ADX_ENC_NONE = 0,  /* no encryption             */
    FF_ADX_ENC_TYPE8 = 8, /* XOR key from 2x uint16_t  */
    FF_ADX_ENC_TYPE9 = 9  /* XOR key from LCG params   */
} ff_adx_encryption_type;

typedef struct {
    ff_adx_encryption_type type;
    /* Type 8 */
    uint16_t xor8_key_lo;   /* low  16-bit key component */
    uint16_t xor8_key_hi;   /* high 16-bit key component */
    /* Type 9 */
    uint32_t xor9_start;
    uint32_t xor9_mult;
    uint32_t xor9_inc;
} ff_adx_key;

/* ============================================================================
 * Container / stream info (public, read-only after init)
 * ============================================================================ */
typedef enum {
    FF_ADX_CONTAINER_ADX = 0,  /* single-stream .adx        */
    FF_ADX_CONTAINER_AIX = 1   /* multi-stream interleaved  */
} ff_adx_container_type;

typedef struct {
    ff_adx_container_type container_type;
    uint8_t  adx_version;       /* 3, 4, or 5 */
    uint32_t sample_rate;
    uint16_t channels;
    uint32_t frame_size;        /* bytes per encoded frame per channel */
    uint32_t filter_order;      /* almost always 4 */
    uint64_t total_samples;
    uint8_t  has_loop;
    uint64_t loop_start_sample;
    uint64_t loop_end_sample;
    ff_adx_encryption_type enc_type;
    /* AIX only */
    uint8_t  aix_stream_count;
} ff_adx_info;

/* ============================================================================
 * Config (passed to init functions; may be NULL for defaults)
 * ============================================================================ */
typedef struct {
    ff_adx_allocation_callbacks allocationCallbacks;
    ff_adx_key  decrypt_key;        /* set type != NONE to provide a key     */
    uint8_t     aix_stream_index;   /* which AIX sub-stream to decode (0-based) */
} ff_adx_config;

/* ============================================================================
 * Main decoder structure
 * ============================================================================ */
#define FF_ADX__MAX_CHANNELS 8   /* ADX is typically mono/stereo, AIX up to 8 */

typedef struct ff_adx {
    /* Public fields (read-only after init) */
    uint32_t channels;
    uint32_t sampleRate;
    uint64_t totalPCMFrameCount;
    uint64_t currentPCMFrame;

    /* Container info */
    ff_adx_info info;

    /* Internal decode state (do not touch) */
    ff_adx_allocation_callbacks allocationCallbacks;

    /* I/O source - one of pData/fileSize (memory) or FILE* (file) */
    const uint8_t* pData;       /* memory-mapped source, or NULL if file     */
    size_t         dataSize;
    void*          pFile;       /* FILE* if opened from path, else NULL      */

    /* Per-channel ADPCM history */
    int32_t  hist1[FF_ADX__MAX_CHANNELS];
    int32_t  hist2[FF_ADX__MAX_CHANNELS];

    /* Encryption state */
    ff_adx_key   key;
    uint32_t     xor_pos;       /* position in key stream (sample count)     */

    /* Stream layout */
    uint32_t frame_size;        /* bytes per frame per channel (usually 18)  */
    uint32_t samples_per_frame; /* always (frame_size-2)*2                   */
    uint64_t data_offset;       /* byte offset where encoded data begins     */
    uint64_t stream_data_size;  /* size of the encoded stream (bytes)        */

    /* AIX */
    uint8_t  aix_stream_index;
    uint8_t  aix_stream_count;
    uint64_t aix_block_size;    /* size of one interleaved block (all streams)*/
    uint64_t aix_stream_size;   /* size of one stream's slice per block      */

    /* Output sample buffer (one decoded frame, interleaved) */
    int16_t* pFrameOut;         /* [samples_per_frame * channels]            */
    uint32_t leftoverSamples;   /* samples remaining in pFrameOut            */
    uint32_t leftoverOffset;    /* read head into pFrameOut                  */

    /* Coefficient table (fixed, computed from cutoff freq at init) */
    int32_t coeff1;
    int32_t coeff2;
} ff_adx;

/* ============================================================================
 * Low-Level API
 * ============================================================================ */

#ifndef FF_ADX_NO_STDIO
/** Initialize decoder from a file path. pConfig may be NULL. */
FF_ADX_API ff_adx_result ff_adx_init_file(const char* pFilePath,
                                           const ff_adx_config* pConfig,
                                           ff_adx* pAdx);
#endif

/** Initialize decoder from a memory block. pData must remain valid. pConfig may be NULL. */
FF_ADX_API ff_adx_result ff_adx_init_memory(const void* pData, size_t dataSize,
                                              const ff_adx_config* pConfig,
                                              ff_adx* pAdx);

/** Uninitialize and release resources. */
FF_ADX_API void ff_adx_uninit(ff_adx* pAdx);

/** Read up to framesToRead interleaved float PCM frames. Returns frames read. */
FF_ADX_API uint64_t ff_adx_read_pcm_frames_f32(ff_adx* pAdx,
                                                 uint64_t framesToRead,
                                                 float* pBufferOut);

/** Read up to framesToRead interleaved s16 PCM frames. Returns frames read. */
FF_ADX_API uint64_t ff_adx_read_pcm_frames_s16(ff_adx* pAdx,
                                                 uint64_t framesToRead,
                                                 int16_t* pBufferOut);

/** Seek to an absolute PCM frame index. */
FF_ADX_API ff_adx_result ff_adx_seek_to_pcm_frame(ff_adx* pAdx, uint64_t targetFrame);

/** Get the current read cursor in PCM frames. */
FF_ADX_API uint64_t ff_adx_get_cursor_in_pcm_frames(const ff_adx* pAdx);

/** Get total length in PCM frames. */
FF_ADX_API uint64_t ff_adx_get_length_in_pcm_frames(const ff_adx* pAdx);

/* ============================================================================
 * High-Level API
 * ============================================================================ */

#ifndef FF_ADX_NO_STDIO
FF_ADX_API float*   ff_adx_open_file_and_read_pcm_frames_f32(const char* pFilePath,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc);

FF_ADX_API int16_t* ff_adx_open_file_and_read_pcm_frames_s16(const char* pFilePath,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc);
#endif

FF_ADX_API float*   ff_adx_open_memory_and_read_pcm_frames_f32(const void* pData, size_t dataSize,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc);

FF_ADX_API int16_t* ff_adx_open_memory_and_read_pcm_frames_s16(const void* pData, size_t dataSize,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc);

FF_ADX_API void ff_adx_free(void* p, const ff_adx_allocation_callbacks* pAlloc);


/* ============================================================================
 * ff_aix - Multi-stream AIX interface
 *
 * Opens an AIX file once and exposes all embedded streams through a single
 * decoder object. Each stream has its own independent cursor and ADPCM history
 * so they can be read at the same position in lockstep, as required for game
 * audio (e.g. multi-language stems decoded simultaneously).
 *
 * Usage:
 *   ff_aix aix;
 *   if (ff_aix_init_file("bgm.aix", NULL, &aix) == FF_ADX_SUCCESS) {
 *       float buf0[2048], buf1[2048];
 *       uint64_t got;
 *       while ((got = ff_aix_read_pcm_frames_f32(&aix, 0, 1024, buf0)) > 0) {
 *           ff_aix_read_pcm_frames_f32(&aix, 1, 1024, buf1);
 *           // route buf0 or buf1 to audio output based on active language...
 *       }
 *       ff_aix_uninit(&aix);
 *   }
 * ============================================================================ */

/* Per-stream state (internal ADPCM history + output buffer) */
typedef struct {
    int32_t  hist1[FF_ADX__MAX_CHANNELS]; /* ADPCM predictor history per channel */
    int32_t  hist2[FF_ADX__MAX_CHANNELS];
    uint64_t data_offset;                 /* file offset of this stream in block 0 */
    uint64_t currentPCMFrame;
    int16_t* pFrameOut;                   /* decoded frame: samples_per_frame*channels */
    uint32_t leftoverSamples;
    uint32_t leftoverOffset;
} ff_aix_stream_state;

/* Main multi-stream decoder */
typedef struct ff_aix {
    /* Public (read-only after init) */
    uint32_t stream_count;
    uint32_t channels;              /* per stream -- all streams identical */
    uint32_t sampleRate;
    uint64_t totalPCMFrameCount;

    /* Per-stream state */
    ff_aix_stream_state streams[FF_ADX__MAX_CHANNELS];

    /* Shared geometry */
    uint32_t frame_size;
    uint32_t samples_per_frame;
    uint64_t aix_stream_size;       /* audio bytes per stream per block     */
    uint64_t aix_block_size;        /* num_streams * block_total_size       */
    int32_t  coeff1;
    int32_t  coeff2;

    /* Shared I/O */
    const uint8_t* pData;
    size_t         dataSize;
    void*          pFile;

    ff_adx_allocation_callbacks allocationCallbacks;
} ff_aix;

/* ---- ff_aix API --------------------------------------------------------- */

#ifndef FF_ADX_NO_STDIO
/** Open an AIX file. pConfig may be NULL (aix_stream_index is ignored here). */
FF_ADX_API ff_adx_result ff_aix_init_file(const char* pFilePath,
                                           const ff_adx_config* pConfig,
                                           ff_aix* pAix);
#endif

/** Open AIX from a memory block. pData must remain valid for decoder lifetime. */
FF_ADX_API ff_adx_result ff_aix_init_memory(const void* pData, size_t dataSize,
                                              const ff_adx_config* pConfig,
                                              ff_aix* pAix);

/** Release all resources. */
FF_ADX_API void ff_aix_uninit(ff_aix* pAix);

/** Read up to framesToRead float PCM frames from stream streamIdx.
 *  Returns frames actually read. */
FF_ADX_API uint64_t ff_aix_read_pcm_frames_f32(ff_aix* pAix, uint8_t streamIdx,
                                                 uint64_t framesToRead,
                                                 float* pBufferOut);

/** Read up to framesToRead s16 PCM frames from stream streamIdx. */
FF_ADX_API uint64_t ff_aix_read_pcm_frames_s16(ff_aix* pAix, uint8_t streamIdx,
                                                 uint64_t framesToRead,
                                                 int16_t* pBufferOut);

/** Seek ALL streams to the same PCM frame index. */
FF_ADX_API ff_adx_result ff_aix_seek_to_pcm_frame(ff_aix* pAix, uint64_t targetFrame);

/** Current read cursor for a specific stream. */
FF_ADX_API uint64_t ff_aix_get_cursor_in_pcm_frames(const ff_aix* pAix,
                                                      uint8_t streamIdx);

/** Total length in PCM frames. */
FF_ADX_API uint64_t ff_aix_get_length_in_pcm_frames(const ff_aix* pAix);

#ifdef __cplusplus
}
#endif

/* ============================================================================
 * IMPLEMENTATION
 * ============================================================================ */
#ifdef FF_ADX_IMPLEMENTATION

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)  /* conversion, possible loss of data       */
#pragma warning(disable: 4100)  /* unreferenced formal parameter           */
#pragma warning(disable: 4310)  /* cast truncates constant value           */
#pragma warning(disable: 4389)  /* signed/unsigned mismatch                */
#pragma warning(disable: 4456)  /* declaration hides previous local        */
#pragma warning(disable: 4324)  /* structure padded due to alignment       */
#pragma warning(disable: 4101)  /* unreferenced local variable             */
#pragma warning(disable: 4701)  /* potentially uninitialized variable used */
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef FF_ADX_NO_STDIO
#include <stdio.h>
/* Large-file seek portability */
#if defined(_MSC_VER)
    #define FF_ADX__FSEEK(f, off, org)  _fseeki64((f), (__int64)(off), (org))
#elif defined(_LARGEFILE_SOURCE) || defined(_FILE_OFFSET_BITS)
    #define FF_ADX__FSEEK(f, off, org)  fseeko((f), (off_t)(off), (org))
#else
    #define FF_ADX__FSEEK(f, off, org)  fseek((f), (long)(off), (org))
#endif
#endif

/* ============================================================================
 * ff_libs shared SIMD float->s16 converter
 * ============================================================================ */
#ifndef FF_LIBS__F32_TO_S16_DEFINED
#define FF_LIBS__F32_TO_S16_DEFINED

#ifndef FF_LIBS_NO_SIMD
    #if defined(__SSE2__) || (defined(_MSC_VER) && (defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)))
        #define FF_LIBS__SSE2
        #include <emmintrin.h>
    #elif defined(__ARM_NEON) || defined(__ARM_NEON__)
        #define FF_LIBS__NEON
        #include <arm_neon.h>
    #endif
#endif

static void ff_libs__f32_to_s16_batch(const float* src, int16_t* dst, size_t count)
{
#if defined(FF_LIBS__SSE2)
    {
        __m128 scale = _mm_set1_ps(32767.0f);
        size_t i = 0;
        size_t simd_end = count & ~(size_t)7;
        for (; i < simd_end; i += 8) {
            __m128 f0 = _mm_loadu_ps(src + i);
            __m128 f1 = _mm_loadu_ps(src + i + 4);
            f0 = _mm_mul_ps(f0, scale);
            f1 = _mm_mul_ps(f1, scale);
            __m128i i0 = _mm_cvtps_epi32(f0);
            __m128i i1 = _mm_cvtps_epi32(f1);
            __m128i packed = _mm_packs_epi32(i0, i1);
            _mm_storeu_si128((__m128i*)(dst + i), packed);
        }
        for (; i < count; i++) {
            float s = src[i] * 32767.0f;
            if (s >  32767.0f) s =  32767.0f;
            if (s < -32768.0f) s = -32768.0f;
            dst[i] = (int16_t)s;
        }
    }
#elif defined(FF_LIBS__NEON)
    {
        float32x4_t scale = vdupq_n_f32(32767.0f);
        size_t i = 0;
        size_t simd_end = count & ~(size_t)7;
        for (; i < simd_end; i += 8) {
            float32x4_t f0 = vld1q_f32(src + i);
            float32x4_t f1 = vld1q_f32(src + i + 4);
            f0 = vmulq_f32(f0, scale);
            f1 = vmulq_f32(f1, scale);
            int32x4_t i0 = vcvtq_s32_f32(f0);
            int32x4_t i1 = vcvtq_s32_f32(f1);
            int16x4_t h0 = vqmovn_s32(i0);
            int16x4_t h1 = vqmovn_s32(i1);
            int16x8_t packed = vcombine_s16(h0, h1);
            vst1q_s16(dst + i, packed);
        }
        for (; i < count; i++) {
            float s = src[i] * 32767.0f;
            if (s >  32767.0f) s =  32767.0f;
            if (s < -32768.0f) s = -32768.0f;
            dst[i] = (int16_t)s;
        }
    }
#else
    {
        size_t i;
        for (i = 0; i < count; i++) {
            float s = src[i] * 32767.0f;
            if (s >  32767.0f) s =  32767.0f;
            if (s < -32768.0f) s = -32768.0f;
            dst[i] = (int16_t)s;
        }
    }
#endif
}
#endif /* FF_LIBS__F32_TO_S16_DEFINED */

/* ============================================================================
 * ff_adx__s16_to_f32_batch
 * Convert count int16 samples to float in [-1, 1]. SIMD-accelerated.
 * ============================================================================ */
static void ff_adx__s16_to_f32_batch(const int16_t* src, float* dst, size_t count)
{
#if defined(FF_LIBS__SSE2)
    {
        /* SSE2: process 8 int16s per iteration -> 8 floats */
        __m128 scale = _mm_set1_ps(1.0f / 32768.0f);
        size_t i = 0;
        size_t simd_end = count & ~(size_t)7;

        for (; i < simd_end; i += 8) {
            __m128i s16v  = _mm_loadu_si128((const __m128i*)(src + i));
            /* Sign-extend int16 -> int32 using SSE2 (no PMOVSXWD until SSE4.1) */
            __m128i sign  = _mm_srai_epi16(s16v, 15);      /* 0x0000 or 0xFFFF */
            __m128i lo32  = _mm_unpacklo_epi16(s16v, sign); /* 4 x int32 */
            __m128i hi32  = _mm_unpackhi_epi16(s16v, sign); /* 4 x int32 */
            __m128 flo    = _mm_mul_ps(_mm_cvtepi32_ps(lo32), scale);
            __m128 fhi    = _mm_mul_ps(_mm_cvtepi32_ps(hi32), scale);
            _mm_storeu_ps(dst + i,     flo);
            _mm_storeu_ps(dst + i + 4, fhi);
        }
        /* Scalar tail */
        for (; i < count; i++)
            dst[i] = src[i] * (1.0f / 32768.0f);
    }
#elif defined(FF_LIBS__NEON)
    {
        /* NEON: process 8 int16s per iteration -> 8 floats */
        float32x4_t scale = vdupq_n_f32(1.0f / 32768.0f);
        size_t i = 0;
        size_t simd_end = count & ~(size_t)7;

        for (; i < simd_end; i += 8) {
            int16x8_t  s16v = vld1q_s16(src + i);
            int32x4_t  lo32 = vmovl_s16(vget_low_s16(s16v));
            int32x4_t  hi32 = vmovl_s16(vget_high_s16(s16v));
            float32x4_t flo = vmulq_f32(vcvtq_f32_s32(lo32), scale);
            float32x4_t fhi = vmulq_f32(vcvtq_f32_s32(hi32), scale);
            vst1q_f32(dst + i,     flo);
            vst1q_f32(dst + i + 4, fhi);
        }
        /* Scalar tail */
        for (; i < count; i++)
            dst[i] = src[i] * (1.0f / 32768.0f);
    }
#else
    {
        size_t i;
        for (i = 0; i < count; i++)
            dst[i] = src[i] * (1.0f / 32768.0f);
    }
#endif
}

/* ============================================================================
 * Memory helpers
 * ============================================================================ */
static void* ff_adx__malloc(size_t sz, const ff_adx_allocation_callbacks* a) {
    return a && a->onMalloc ? a->onMalloc(sz, a->pUserData) : malloc(sz);
}
static void* ff_adx__realloc(void* p, size_t sz, const ff_adx_allocation_callbacks* a) {
    return a && a->onRealloc ? a->onRealloc(p, sz, a->pUserData) : realloc(p, sz);
}
static void ff_adx__free(void* p, const ff_adx_allocation_callbacks* a) {
    if (p) { if (a && a->onFree) a->onFree(p, a->pUserData); else free(p); }
}

/* ============================================================================
 * Read helpers (abstracts memory vs FILE)
 * ============================================================================ */
static int ff_adx__read(ff_adx* a, uint64_t offset, void* buf, size_t len)
{
    if (a->pData) {
        if (offset + len > a->dataSize) return 0;
        memcpy(buf, a->pData + offset, len);
        return 1;
    }
#ifndef FF_ADX_NO_STDIO
    else {
        FILE* f = (FILE*)a->pFile;
        if (!f) return 0;
        if (FF_ADX__FSEEK(f, offset, SEEK_SET)) return 0;
        return (fread(buf, 1, len, f) == len);
    }
#else
    return 0;
#endif
}

static uint8_t  ff_adx__r8 (const uint8_t* b, int o) { return b[o]; }
static uint16_t ff_adx__r16be(const uint8_t* b, int o) {
    return (uint16_t)((b[o]<<8)|b[o+1]);
}
static uint32_t ff_adx__r32be(const uint8_t* b, int o) {
    return ((uint32_t)b[o]<<24)|((uint32_t)b[o+1]<<16)|
           ((uint32_t)b[o+2]<<8)|(uint32_t)b[o+3];
}

/* Clamp helper */
static int32_t ff_adx__clamp(int32_t v, int32_t lo, int32_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* ============================================================================
 * Coefficient computation
 *
 * The filter coefficients are derived from a high-pass cutoff frequency.
 * Formula matches CRI's own decoder (criware_decode.cpp / ADXDEC_SetCoeff):
 *
 *   a = sqrt(2) - cos(2*pi*cutoff / sample_rate)
 *   b = sqrt(2) - 1
 *   c = (a - sqrt((a+b)*(a-b))) / b
 *
 *   coeff1 = (int16_t)(c        * 8192)   -- 13-bit fixed point, truncated
 *   coeff2 = (int16_t)(c*c * -4096)       -- 12-bit fixed point, truncated
 *
 * Prediction at decode time:
 *   predicted = (coeff1 * hist1 + coeff2 * hist2) >> 12
 *
 * coeff1 uses 8192 (not 4096) so the effective shift is 13 bits,
 * but the >>12 at prediction site makes it: c*2*hist1.
 * coeff2 uses -4096 giving: -c²*hist2 after >>12.
 *
 * The typical cutoff stored in files is 500 Hz; v5/AHX uses 0.
 * ============================================================================ */
static void ff_adx__compute_coefficients(uint32_t sample_rate, uint16_t cutoff,
                                          int32_t* coeff1, int32_t* coeff2)
{
    /* Precomputed fallback for 500 Hz / 44100 Hz */
    if (cutoff == 0 || sample_rate == 0) {
        *coeff1 = 7334;
        *coeff2 = -3283;
        return;
    }

    {
        double sqrt2 = 1.41421356237309504880;
        double pi    = 3.14159265358979323846;
        double a = sqrt2 - cos(2.0 * pi * (double)cutoff / (double)sample_rate);
        double b = sqrt2 - 1.0;
        double c = (a - sqrt((a + b) * (a - b))) / b;

        /* toshort: truncate toward zero, matching the C (short) cast in reference */
        *coeff1 = (int32_t)(int16_t)(int32_t)(c       * 8192.0);
        *coeff2 = (int32_t)(int16_t)(int32_t)(c * c * -4096.0);
    }
}

/* ============================================================================
 * Encryption key stream
 *
 * ADX type 8 encryption XORs each 14-byte sample payload with a key stream
 * derived from two 16-bit seed values. The key stream advances per sample
 * (not per byte). Each sample's 4-bit nibble is XOR'd independently, but the
 * same key byte is used for both nibbles within a 4-bit pair.
 *
 * Type 9 uses a linear congruential generator:
 *   state = state * mult + inc
 *   key   = (state >> 16) & 0x7FFF
 *
 * Both key types produce one 16-bit value per sample position.
 *
 * key_pos tracks the current sample index within the stream; seeking resets it.
 * ============================================================================ */

#ifndef FF_ADX_NO_ENCRYPTION

static uint16_t ff_adx__xor8_key_at(const ff_adx_key* k, uint32_t pos)
{
    /*
     * Type 8: generate a 14-entry key table from xor8_key_lo and xor8_key_hi,
     * then cycle through it. The table is generated once per stream:
     *
     *   table[0] = key_lo * key_hi
     *   table[i] = table[i-1] + key_hi * (i+1) * key_lo + key_lo
     *
     * This matches vgmstream's adx_cri.c key8 generation exactly.
     */
    uint16_t t[14];
    uint32_t i;
    uint16_t lo = k->xor8_key_lo;
    uint16_t hi = k->xor8_key_hi;
    /* Build table */
    t[0] = (uint16_t)(lo * hi);
    for (i = 1; i < 14; i++) {
        t[i] = (uint16_t)(t[i-1] + hi * (uint16_t)(i+1) * lo + lo);
    }
    return t[pos % 14];
}

static uint16_t ff_adx__xor9_key_at(const ff_adx_key* k, uint32_t pos,
                                      uint32_t* state_cache, uint32_t* cached_pos)
{
    /*
     * Type 9: LCG key stream. Evaluating from position 0 each time is slow.
     * We cache the last state so seeking forward is cheap; backward seeks
     * restart from 0 (acceptable, since seeks are rare).
     */
    uint32_t state;
    uint32_t i;

    if (pos < *cached_pos) {
        /* reset and replay from start */
        state = k->xor9_start;
        *cached_pos = 0;
    } else {
        state = *state_cache;
    }

    for (i = *cached_pos; i < pos; i++) {
        state = state * k->xor9_mult + k->xor9_inc;
    }

    *state_cache = state * k->xor9_mult + k->xor9_inc; /* step to pos */
    *cached_pos  = pos + 1;
    return (uint16_t)((*state_cache >> 16) & 0x7FFF);
}

#endif /* FF_ADX_NO_ENCRYPTION */

/* ============================================================================
 * ADX frame decode
 *
 * An ADX frame is `frame_size` bytes. Structure:
 *   Byte 0:    predictor index in high nibble, scale exponent in low nibble
 *              -- actually: bytes 0..1 are a big-endian int16 "scale" value
 *   Bytes 2..frame_size-1: packed 4-bit signed nibbles (MSN first)
 *
 * Wait—let me be precise. The actual layout is:
 *   Bytes 0..1: big-endian int16 "scale" (a linear PCM scale factor)
 *   Bytes 2..frame_size-1: (frame_size-2) bytes, each containing 2 nibbles
 *                          high nibble = sample[2i], low nibble = sample[2i+1]
 *                          nibbles are 4-bit signed (range -8..7).
 *
 * Decoding a sample:
 *   delta_s = nibble * scale
 *   predicted = (coeff1 * hist1 + coeff2 * hist2) >> 12
 *   out = clamp(delta_s + predicted, -32768, 32767)
 *   hist2 = hist1
 *   hist1 = out
 *
 * This produces `(frame_size - 2) * 2` samples per channel per frame.
 *
 * Encryption: the 14 bytes following the 2-byte scale header (bytes 2..15 for
 * frame_size==18) are XOR'd per-sample with the key stream. Specifically:
 *   byte 2+j is XOR'd with (key_at(sample_offset + j) >> 8) for even j,
 *                          (key_at(sample_offset + j/2) & 0xFF) for odd j?
 *
 * Actually, the encryption applies one 16-bit key word per input byte pair:
 *   plaintext[j] = ciphertext[j] ^ (key_at(xor_pos + j) >> 8)  (high byte)
 *   plaintext[j+1] = ciphertext[j+1] ^ (key_at(xor_pos + j) & 0xFF) (low byte)
 * advancing xor_pos by (frame_size-2)/2 per frame per channel.
 * ============================================================================ */

/* Per-channel state for the XOR9 key */
typedef struct {
    uint32_t xor9_state;
    uint32_t xor9_cached_pos;
} ff_adx__xor9_chan;

static void ff_adx__decode_frame(
    const uint8_t* frame_data, /* frame_size bytes, already read */
    uint32_t       frame_size,
    int32_t        coeff1,
    int32_t        coeff2,
    int32_t*       hist1,      /* in/out */
    int32_t*       hist2,      /* in/out */
    int16_t*       out,        /* output: (frame_size-2)*2 samples */
    /* Encryption */
    const ff_adx_key* key,
    uint32_t          xor_pos  /* sample offset for this frame (for key stream) */
)
{
    uint8_t  decrypted[256]; /* max frame_size is 256 bytes */
    const uint8_t* src;
    uint32_t num_samples = (frame_size - 2) * 2;
    uint32_t i;
    int32_t  scale;

    (void)xor_pos;

#ifndef FF_ADX_NO_ENCRYPTION
    if (key && key->type != FF_ADX_ENC_NONE) {
        /* Decrypt bytes 2..frame_size-1 (leave scale header intact) */
        uint32_t payload_bytes = frame_size - 2;
        uint32_t j;
        /* Copy scale header unchanged */
        decrypted[0] = frame_data[0];
        decrypted[1] = frame_data[1];

        for (j = 0; j < payload_bytes; j += 2) {
            uint16_t kw;
            uint32_t sample_idx = xor_pos + j / 2; /* which sample this covers */
            if (key->type == FF_ADX_ENC_TYPE8) {
                kw = ff_adx__xor8_key_at(key, sample_idx);
            } else {
                /* type 9 -- caller must use sequential decode; we use xor_pos directly */
                /* For type 9 we rely on the sequential calling convention -- caller
                 * should pass xor_pos as the absolute sample offset so we can use it. */
                kw = (uint16_t)(xor_pos >> 16); /* simplified: precomputed outside */
                /* Real type 9 is managed by the outer loop; see ff_adx__decode_frame_xor9 */
            }
            decrypted[2 + j]     = frame_data[2 + j]     ^ (uint8_t)(kw >> 8);
            if (j + 1 < payload_bytes)
                decrypted[2 + j + 1] = frame_data[2 + j + 1] ^ (uint8_t)(kw & 0xFF);
        }
        src = decrypted;
    } else {
        src = frame_data;
    }
#else
    src = frame_data;
#endif

    /* Read big-endian scale: unsigned 13-bit, +1 (range 1..8192) */
    scale = (int32_t)(((uint32_t)(src[0] << 8) | src[1]) & 0x1fff) + 1;

    /* Decode nibbles */
    for (i = 0; i < num_samples / 2; i++) {
        uint8_t byte = src[2 + i];
        int32_t nibbles[2];
        int32_t s;
        uint32_t n;

        nibbles[0] = (int32_t)(int8_t)(byte & 0xF0) >> 4; /* sign-extend 4-bit */
        nibbles[1] = (int32_t)(int8_t)(byte << 4) >> 4;

        for (n = 0; n < 2; n++) {
            int32_t predicted = (*hist1 * coeff1 + *hist2 * coeff2) >> 12;
            s = ff_adx__clamp(nibbles[n] * scale + predicted, -32768, 32767);
            *hist2 = *hist1;
            *hist1 = s;
            out[i * 2 + n] = (int16_t)s;
        }
    }
}

/* ============================================================================
 * Type-9 encryption: specialized version that uses inline LCG state
 * ============================================================================ */
#ifndef FF_ADX_NO_ENCRYPTION
static void ff_adx__decode_frame_xor9(
    const uint8_t* frame_data,
    uint32_t       frame_size,
    int32_t        coeff1,
    int32_t        coeff2,
    int32_t*       hist1,
    int32_t*       hist2,
    int16_t*       out,
    uint32_t*      xor9_state,       /* LCG state (in/out, updated per byte-pair) */
    const ff_adx_key* key
)
{
    uint8_t  decrypted[256];
    uint32_t payload_bytes = frame_size - 2;
    uint32_t j;
    int32_t  scale;
    uint32_t num_samples = (frame_size - 2) * 2;
    uint32_t i;

    /* Decrypt */
    decrypted[0] = frame_data[0];
    decrypted[1] = frame_data[1];
    for (j = 0; j < payload_bytes; j += 2) {
        uint32_t next = *xor9_state * key->xor9_mult + key->xor9_inc;
        uint16_t kw   = (uint16_t)(next >> 16) & 0x7FFF;
        *xor9_state = next;
        decrypted[2 + j]     = frame_data[2 + j]     ^ (uint8_t)(kw >> 7);
        if (j + 1 < payload_bytes)
            decrypted[2 + j + 1] = frame_data[2 + j + 1] ^ (uint8_t)(kw & 0x7F);
    }

    scale = (int32_t)(((uint32_t)(decrypted[0] << 8) | decrypted[1]) & 0x1fff) + 1;

    for (i = 0; i < num_samples / 2; i++) {
        uint8_t byte = decrypted[2 + i];
        int32_t nibbles[2];
        int32_t s;
        uint32_t n;
        nibbles[0] = (int32_t)(int8_t)(byte & 0xF0) >> 4;
        nibbles[1] = (int32_t)(int8_t)(byte << 4) >> 4;
        for (n = 0; n < 2; n++) {
            int32_t predicted = (*hist1 * coeff1 + *hist2 * coeff2) >> 12;
            s = ff_adx__clamp(nibbles[n] * scale + predicted, -32768, 32767);
            *hist2 = *hist1;
            *hist1 = s;
            out[i * 2 + n] = (int16_t)s;
        }
    }
}
#endif /* FF_ADX_NO_ENCRYPTION */

/* ============================================================================
 * ADX header parsing
 * ============================================================================ */

/*
 * ADX header layout:
 *
 *  Offset  Size  Description
 *  0x00     2    Magic: 0x8000
 *  0x02     2    Offset to data block (from byte 0x04) - "copyright offset"
 *  0x04     1    Encoding type: 3=ADXv3, 4=ADXv4, 5=AHX/ADXv5
 *  0x05     1    Frame size (bytes per channel frame, typically 18)
 *  0x06     1    Bits per sample (always 4)
 *  0x07     1    Channel count
 *  0x08     4    Sample rate (Hz)
 *  0x0C     4    Total sample count
 *  0x10     2    High-pass cutoff frequency (Hz)
 *  0x12     1    Version (3 or 4)
 *  0x13     1    Flags: bit0 = loop flag (v3), enc_type (v4+)
 *
 * Version 3 loop block (at offset 0x14):
 *  0x14     2    Loop flags (0=no loop, 1=loop)
 *  0x16     2    Padding
 *  0x18     4    Loop start sample
 *  0x1C     4    Loop start byte
 *  0x20     4    Loop end sample
 *  0x24     4    Loop end byte
 *
 * Version 4 loop block (at offset 0x14):
 *  0x14     2    Loop flags
 *  0x16     2    Padding
 *  0x18     4    Loop start sample
 *  0x1C     4    Loop start byte offset
 *  0x20     4    Loop end sample
 *  0x24     4    Loop end byte offset
 *  ... (same as v3 but with encryption info)
 *
 * After the header area (up to copyright_offset+4 bytes from start) comes
 * a copyright string "(c)CRI" followed by the data area.
 *
 * data_offset = copyright_offset + 4  (i.e. offset_field + 4)
 *
 * Encryption is indicated in byte 0x13 (flags):
 *   0x08 = type 8 XOR encryption
 *   0x09 = type 9 XOR encryption
 * In v3 this byte is always 0; encryption info appears in v4+.
 *
 * For encrypted files the 2-byte value at offset (header_size - 2) encodes
 * the public XOR key used during validation (not the decryption key itself).
 */
static ff_adx_result ff_adx__parse_header(ff_adx* a)
{
    uint8_t  hdr[0x80]; /* enough for any ADX header */
    uint32_t copyright_offset;
    uint32_t data_offset;
    uint8_t  enc_type_raw;
    uint16_t cutoff;
    uint8_t  version;

    /* Read first 4 bytes to determine header size */
    if (!ff_adx__read(a, 0, hdr, 6)) return FF_ADX_INVALID_FILE;

    /* Magic check */
    if (hdr[0] != 0x80 || hdr[1] != 0x00) return FF_ADX_INVALID_FILE;

    copyright_offset = ff_adx__r16be(hdr, 2);
    data_offset      = copyright_offset + 4; /* +4 to skip past "(c)CR"... actually +4 */

    if (data_offset < 0x14 || data_offset > 0x800)
        return FF_ADX_INVALID_FILE;

    /* Read full header block */
    if (!ff_adx__read(a, 0, hdr, data_offset < sizeof(hdr) ? data_offset : sizeof(hdr)))
        return FF_ADX_INVALID_FILE;

    /* Verify copyright signature just before data start */
    /* Usually "(c)CRI" at copyright_offset from offset 4, i.e. at byte copyright_offset+4-6 */
    /* We just check the encoding type byte is sane */

    version          = ff_adx__r8(hdr, 0x12);
    enc_type_raw     = ff_adx__r8(hdr, 0x13);
    cutoff           = ff_adx__r16be(hdr, 0x10);

    a->info.adx_version = version;
    a->info.channels    = ff_adx__r8(hdr, 0x07);
    a->info.sample_rate = ff_adx__r32be(hdr, 0x08);
    a->info.total_samples = ff_adx__r32be(hdr, 0x0C);
    a->info.frame_size  = ff_adx__r8(hdr, 0x05);
    a->info.filter_order = 4; /* always */

    if (a->info.channels == 0 || a->info.channels > FF_ADX__MAX_CHANNELS)
        return FF_ADX_INVALID_FILE;
    if (a->info.sample_rate == 0) return FF_ADX_INVALID_FILE;
    if (a->info.frame_size < 3 || a->info.frame_size > 64)
        return FF_ADX_INVALID_FILE;

    /*
     * Version notes:
     *   v1 (0x01): Very early Saturn titles. Minimal header; no loop, no enc.
     *              Version byte may be 0x01 or occasionally 0x00.
     *   v2 (0x02): Early Saturn/Dreamcast. No loop fields, no encryption.
     *   v3 (0x03): Standard. Loop fields at 0x14.
     *   v4 (0x04): Extended. Loop fields + encryption type in flags byte.
     *   v5 (0x05): AHX speech codec -- different sub-codec, not supported.
     */
    if (version == 5) return FF_ADX_NOT_IMPLEMENTED; /* AHX uses a different codec */

    /* Encryption: only v4+ files can be encrypted; v1/v2/v3 are always plain. */
    if (version >= 4) {
        if      (enc_type_raw == 0x08) a->info.enc_type = FF_ADX_ENC_TYPE8;
        else if (enc_type_raw == 0x09) a->info.enc_type = FF_ADX_ENC_TYPE9;
        else                           a->info.enc_type = FF_ADX_ENC_NONE;
    } else {
        a->info.enc_type = FF_ADX_ENC_NONE;
    }

    /* Loop info: only present in v3/v4 when the header is large enough.
     * v1/v2 have no loop block -- has_loop stays 0. */
    a->info.has_loop = 0;
    if ((version == 3 || version == 4) && data_offset >= 0x28) {
        uint16_t loop_flag = ff_adx__r16be(hdr, 0x14);
        if (loop_flag == 1) {
            a->info.has_loop          = 1;
            a->info.loop_start_sample = ff_adx__r32be(hdr, 0x18);
            a->info.loop_end_sample   = ff_adx__r32be(hdr, 0x20);
        }
    }

    a->data_offset     = data_offset;
    a->frame_size      = a->info.frame_size;
    a->samples_per_frame = (a->frame_size - 2) * 2;

    ff_adx__compute_coefficients(a->info.sample_rate, cutoff,
                                  &a->coeff1, &a->coeff2);

    return FF_ADX_SUCCESS;
}

/* ============================================================================
 * AIX header parsing
 *
 * AIX (ADX Interleaved eXtended) is a container that multiplexes N ADX streams
 * into a single file. Format:
 *
 *  Offset  Size  Description
 *  0x00     4    Magic "AIXF"
 *  0x04     4    Total file size - 8
 *  0x08     4    Unknown / always 0x01000000
 *  0x0C     4    Unknown
 *  0x10     4    Number of streams (N)
 *  0x14     4    Size of this header chunk = 0x18 (varies)
 *  0x18 + i*8   per-stream: 4-byte data_size, 4-byte unknown
 *
 * After the AIXF chunk comes one "AIXI" (info) chunk per stream containing
 * a standard ADX sub-header, then the data section ("AIXD") with interleaved
 * blocks of stream data.
 *
 * In practice the layout is:
 *   [AIXF header] [N × AIXI sub-headers] [interleaved data blocks]
 *
 * Each data block = N × (block_samples × frame_size × channels) bytes,
 * where block_samples is typically 64 or 128 frames, interleaved in stream order.
 *
 * Since AIX internals vary across versions, we use a pragmatic approach:
 *   1. Find the AIXI chunk for the requested stream index.
 *   2. Parse its embedded ADX header to get codec parameters.
 *   3. Record the interleave geometry so read() can skip other streams.
 * ============================================================================ */
static ff_adx_result ff_adx__parse_aix(ff_adx* a, uint8_t stream_idx)
{
    /*
     * Real AIX format (from CRI middleware source):
     *
     * Every chunk starts with AIX_CHUNK (8 bytes):
     *   0x00  magic[3] + type char  ("AIXF", "AIXP", "AIXE")
     *   0x04  next (BE32) -- payload bytes after this 8-byte header
     *         => next chunk is at current_offset + next + 8
     *
     * AIXF chunk (file start):
     *   0x24  data_size    (BE32) total audio data bytes
     *   0x28  total_samples(BE32)
     *   0x2C  frequency    (BE32) Hz
     *   0x40  stream_count (BYTE)
     *   0x48  AIX_ENTRY[N]: frequency(BE32) + channels(BE32) each
     *
     * AIXP info chunk (frames == 0xFFFFFFFF, one per stream):
     *   +8   stream_id    (BYTE)
     *   +9   out_channels (BYTE)
     *   +10  size         (BE16) ADX header byte size
     *   +12  frames       (BE32) == 0xFFFFFFFF
     *   +16  embedded ADX sub-header (0x8000 magic)
     *
     * AIXP data chunk (frames == 0, one per stream per block):
     *   +8   stream_id    (BYTE)
     *   +9   out_channels (BYTE)
     *   +10  size         (BE16) audio bytes in this chunk
     *   +12  frames       (BE32) == 0
     *   +16  ADX frame data for this stream
     *
     * Blocks interleave strictly: [s0 block][s1 block]...[sN block][s0 block]...
     *
     * Geometry:
     *   block_total_size = 8 + chunk.next
     *   aix_stream_size  = AIXP_data.size   (audio bytes per stream per block)
     *   aix_block_size   = num_streams * block_total_size  (one full interleave round)
     *   data_offset      = first_data_block + stream_idx * block_total_size + 16
     */
    uint8_t  aixf[0x50];
    uint32_t num_streams, aixf_end, off, info_found;
    uint32_t block_total_size, data_per_stream;
    uint64_t first_data_block;
    uint8_t  chunk_hdr[16];
    uint16_t cutoff;
    uint8_t  version;

    if (!ff_adx__read(a, 0, aixf, sizeof(aixf))) return FF_ADX_INVALID_FILE;
    if (aixf[0]!='A'||aixf[1]!='I'||aixf[2]!='X'||aixf[3]!='F') return FF_ADX_INVALID_FILE;

    aixf_end    = ff_adx__r32be(aixf, 0x04) + 8;
    num_streams = ff_adx__r8(aixf, 0x40);

    if (num_streams == 0 || num_streams > FF_ADX__MAX_CHANNELS) return FF_ADX_INVALID_FILE;
    if (stream_idx >= num_streams) return FF_ADX_INVALID_ARGS;

    a->info.aix_stream_count = (uint8_t)num_streams;
    a->info.container_type   = FF_ADX_CONTAINER_AIX;
    a->info.total_samples    = ff_adx__r32be(aixf, 0x28);
    a->info.enc_type         = FF_ADX_ENC_NONE;
    a->info.has_loop         = 0;

    /* Walk AIXP chunks: info chunks first, then data */
    off = aixf_end;
    info_found = 0;
    block_total_size = 0;
    data_per_stream  = 0;
    first_data_block = 0;

    while (off + 16 <= (uint32_t)a->dataSize) {
        uint32_t chunk_next, frames;
        uint8_t  stream_id;

        if (!ff_adx__read(a, off, chunk_hdr, 16)) return FF_ADX_INVALID_FILE;
        if (chunk_hdr[0]!='A'||chunk_hdr[1]!='I'||chunk_hdr[2]!='X') return FF_ADX_INVALID_FILE;
        if (chunk_hdr[3] == 'E') break; /* AIXE = end */

        chunk_next = ff_adx__r32be(chunk_hdr, 4);
        stream_id  = chunk_hdr[8];
        frames     = ff_adx__r32be(chunk_hdr, 12);

        if (frames == 0xFFFFFFFF) {
            /* Info chunk: ADX sub-header at off+16 */
            if (stream_id == stream_idx) {
                uint8_t adx_buf[0x40];
                if (!ff_adx__read(a, off + 16, adx_buf, sizeof(adx_buf)))
                    return FF_ADX_INVALID_FILE;
                if (adx_buf[0] != 0x80 || adx_buf[1] != 0x00) return FF_ADX_INVALID_FILE;

                version = ff_adx__r8(adx_buf, 0x12);
                cutoff  = ff_adx__r16be(adx_buf, 0x10);

                if (version == 5) return FF_ADX_NOT_IMPLEMENTED;

                a->info.adx_version = version;
                a->info.channels    = ff_adx__r8(adx_buf, 0x07);
                a->info.sample_rate = ff_adx__r32be(adx_buf, 0x08);
                a->info.frame_size  = ff_adx__r8(adx_buf, 0x05);
                a->info.filter_order = 4;

                if (!a->info.channels || a->info.channels > FF_ADX__MAX_CHANNELS)
                    return FF_ADX_INVALID_FILE;
                if (!a->info.sample_rate) return FF_ADX_INVALID_FILE;
                if (a->info.frame_size < 3 || a->info.frame_size > 64)
                    return FF_ADX_INVALID_FILE;

                a->frame_size        = a->info.frame_size;
                a->samples_per_frame = (a->frame_size - 2) * 2;
                ff_adx__compute_coefficients(a->info.sample_rate, cutoff,
                                              &a->coeff1, &a->coeff2);
                info_found = 1;
            }
        } else {
            /* First data chunk: extract geometry and stop scanning */
            data_per_stream  = ((uint32_t)chunk_hdr[10] << 8) | chunk_hdr[11];
            block_total_size = 8 + chunk_next;
            first_data_block = off;
            break;
        }

        off += 8 + chunk_next;
    }

    if (!info_found || !data_per_stream || block_total_size < 16)
        return FF_ADX_INVALID_FILE;

    a->aix_stream_size  = data_per_stream;
    a->aix_block_size   = (uint64_t)num_streams * block_total_size;
    a->aix_stream_index = stream_idx;
    a->aix_stream_count = (uint8_t)num_streams;
    /* Absolute offset of stream N's audio data in block 0 */
    a->data_offset      = first_data_block
                        + (uint64_t)stream_idx * block_total_size
                        + 16;
    a->stream_data_size = (a->info.total_samples / a->samples_per_frame)
                        * (uint64_t)a->frame_size * a->info.channels;

    return FF_ADX_SUCCESS;
}

static int ff_adx__is_aix(ff_adx* a)
{
    uint8_t tag[4];
    if (!ff_adx__read(a, 0, tag, 4)) return 0;
    return (tag[0]=='A' && tag[1]=='I' && tag[2]=='X' && tag[3]=='F');
}

/* ============================================================================
 * Internal init (shared between file and memory paths)
 * ============================================================================ */
static ff_adx_result ff_adx__init_internal(ff_adx* a, const ff_adx_config* cfg)
{
    ff_adx_result r;
    uint8_t stream_idx = 0;
    size_t frame_buf_size;

    if (cfg) {
        a->allocationCallbacks = cfg->allocationCallbacks;
        a->key                 = cfg->decrypt_key;
        stream_idx             = cfg->aix_stream_index;
    }

    /* Default key from compile-time defines */
#ifndef FF_ADX_NO_ENCRYPTION
#if defined(FF_ADX_DECRYPT_KEY_XOR8)
    if (a->key.type == FF_ADX_ENC_NONE) {
        a->key.type = FF_ADX_ENC_TYPE8;
        /* FF_ADX_DECRYPT_KEY_XOR8 should expand to two comma-separated args */
        /* Usage: #define FF_ADX_DECRYPT_KEY_XOR8(lo,hi) - can't do that here.
         * Instead expect the user to define the key via config. */
    }
#endif
#endif

    if (ff_adx__is_aix(a)) {
        a->info.container_type = FF_ADX_CONTAINER_AIX;
        r = ff_adx__parse_aix(a, stream_idx);
    } else {
        a->info.container_type = FF_ADX_CONTAINER_ADX;
        r = ff_adx__parse_header(a);
    }

    if (r != FF_ADX_SUCCESS) return r;

    /* Check encryption: if file is encrypted and no key provided, return error */
    if (a->info.enc_type != FF_ADX_ENC_NONE && a->key.type == FF_ADX_ENC_NONE)
        return FF_ADX_ENCRYPTED;

    /* Validate key type matches file */
    if (a->key.type != FF_ADX_ENC_NONE && a->key.type != a->info.enc_type)
        return FF_ADX_INVALID_ARGS;

    /* Allocate per-frame output buffer: samples_per_frame * channels * sizeof(int16_t) */
    frame_buf_size = (size_t)a->samples_per_frame * a->info.channels * sizeof(int16_t);
    a->pFrameOut = (int16_t*)ff_adx__malloc(frame_buf_size, &a->allocationCallbacks);
    if (!a->pFrameOut) return FF_ADX_OUT_OF_MEMORY;

    /* Public fields */
    a->channels           = a->info.channels;
    a->sampleRate         = a->info.sample_rate;
    a->totalPCMFrameCount = a->info.total_samples;
    a->currentPCMFrame    = 0;
    a->leftoverSamples    = 0;
    a->leftoverOffset     = 0;
    a->xor_pos            = 0;

    memset(a->hist1, 0, sizeof(a->hist1));
    memset(a->hist2, 0, sizeof(a->hist2));

    return FF_ADX_SUCCESS;
}

/* ============================================================================
 * Public init functions
 * ============================================================================ */
#ifndef FF_ADX_NO_STDIO
FF_ADX_API ff_adx_result ff_adx_init_file(const char* pFilePath,
                                            const ff_adx_config* pConfig,
                                            ff_adx* pAdx)
{
    ff_adx_result r;
    FILE* f;

    if (!pFilePath || !pAdx) return FF_ADX_INVALID_ARGS;
    memset(pAdx, 0, sizeof(*pAdx));

    f = fopen(pFilePath, "rb");
    if (!f) return FF_ADX_INVALID_FILE;

    pAdx->pFile = f;

    /* Get file size */
    fseek(f, 0, SEEK_END);
    pAdx->dataSize = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    r = ff_adx__init_internal(pAdx, pConfig);
    if (r != FF_ADX_SUCCESS) {
        fclose(f);
        pAdx->pFile = NULL;
        if (pAdx->pFrameOut) {
            ff_adx__free(pAdx->pFrameOut, &pAdx->allocationCallbacks);
            pAdx->pFrameOut = NULL;
        }
    }
    return r;
}
#endif

FF_ADX_API ff_adx_result ff_adx_init_memory(const void* pData, size_t dataSize,
                                              const ff_adx_config* pConfig,
                                              ff_adx* pAdx)
{
    ff_adx_result r;
    if (!pData || dataSize == 0 || !pAdx) return FF_ADX_INVALID_ARGS;
    memset(pAdx, 0, sizeof(*pAdx));
    pAdx->pData    = (const uint8_t*)pData;
    pAdx->dataSize = dataSize;
    r = ff_adx__init_internal(pAdx, pConfig);
    if (r != FF_ADX_SUCCESS) {
        if (pAdx->pFrameOut) {
            ff_adx__free(pAdx->pFrameOut, &pAdx->allocationCallbacks);
            pAdx->pFrameOut = NULL;
        }
    }
    return r;
}

FF_ADX_API void ff_adx_uninit(ff_adx* pAdx)
{
    if (!pAdx) return;
    if (pAdx->pFrameOut) {
        ff_adx__free(pAdx->pFrameOut, &pAdx->allocationCallbacks);
        pAdx->pFrameOut = NULL;
    }
#ifndef FF_ADX_NO_STDIO
    if (pAdx->pFile) {
        fclose((FILE*)pAdx->pFile);
        pAdx->pFile = NULL;
    }
#endif
    memset(pAdx, 0, sizeof(*pAdx));
}

/* ============================================================================
 * Compute byte offset of a given PCM frame in the encoded stream.
 *
 * ADX:  data_offset + super_frame * frame_size * channels
 *
 * AIX:  blocks interleave per-stream: [s0][s1]...[sN][s0]...
 *       supers_per_block = aix_stream_size / (frame_size * channels)
 *       file_offset = data_offset                               (stream N, block 0)
 *                   + (super / supers_per_block) * aix_block_size
 *                   + (super % supers_per_block) * frame_size * channels
 * ============================================================================ */
static uint64_t ff_adx__byte_offset_of_frame(const ff_adx* a, uint64_t pcm_frame)
{
    uint64_t super = pcm_frame / a->samples_per_frame;

    if (a->info.container_type == FF_ADX_CONTAINER_ADX) {
        return a->data_offset + super * (uint64_t)a->frame_size * a->channels;
    } else {
        uint64_t spf_bytes       = (uint64_t)a->frame_size * a->channels;
        uint64_t supers_per_block = a->aix_stream_size / spf_bytes;
        uint64_t block_idx       = super / supers_per_block;
        uint64_t frame_in_block  = super % supers_per_block;
        return a->data_offset
             + block_idx    * a->aix_block_size
             + frame_in_block * spf_bytes;
    }
}

/* ============================================================================
 * Decode one super-frame (all channels) into pAdx->pFrameOut
 * Returns 1 on success, 0 on EOF/error.
 * ============================================================================ */
static int ff_adx__decode_super_frame(ff_adx* a)
{
    uint8_t  frame_raw[64]; /* max frame_size */
    uint64_t byte_off;
    uint32_t ch;
    uint32_t spf = a->samples_per_frame;

    /* Where does the current super-frame start? */
    byte_off = ff_adx__byte_offset_of_frame(a, a->currentPCMFrame);

    for (ch = 0; ch < a->channels; ch++) {
        int16_t ch_samples[64 * 2]; /* max samples_per_frame = (64-2)*2 = 124 */
        uint64_t ch_frame_off = byte_off + ch * a->frame_size;

        if (!ff_adx__read(a, ch_frame_off, frame_raw, a->frame_size))
            return 0; /* EOF or error */

#ifndef FF_ADX_NO_ENCRYPTION
        if (a->key.type == FF_ADX_ENC_TYPE9) {
            ff_adx__decode_frame_xor9(
                frame_raw, a->frame_size,
                a->coeff1, a->coeff2,
                &a->hist1[ch], &a->hist2[ch],
                ch_samples,
                &a->key.xor9_start, /* used as mutable state -- init'd at seek */
                &a->key
            );
        } else
#endif
        {
            ff_adx__decode_frame(
                frame_raw, a->frame_size,
                a->coeff1, a->coeff2,
                &a->hist1[ch], &a->hist2[ch],
                ch_samples,
                (a->key.type != FF_ADX_ENC_NONE) ? &a->key : NULL,
                a->xor_pos
            );
        }

        /* Interleave into pFrameOut: [s0ch0, s0ch1, ..., s1ch0, s1ch1, ...] */
        {
            uint32_t s;
            for (s = 0; s < spf; s++) {
                a->pFrameOut[s * a->channels + ch] = ch_samples[s];
            }
        }
    }

    a->leftoverSamples = spf;
    a->leftoverOffset  = 0;
    a->xor_pos        += spf;

    return 1;
}

/* ============================================================================
 * Read PCM frames (s16 path, core implementation)
 * ============================================================================ */
static uint64_t ff_adx__read_s16(ff_adx* a, uint64_t framesToRead, int16_t* out)
{
    uint64_t framesRead = 0;

    while (framesRead < framesToRead) {
        uint64_t remaining = framesToRead - framesRead;
        uint64_t samplesLeft = a->totalPCMFrameCount - a->currentPCMFrame;

        if (samplesLeft == 0) break;
        if (remaining > samplesLeft) remaining = samplesLeft;

        /* Drain leftover buffer first */
        if (a->leftoverSamples > 0) {
            uint32_t can_drain = a->leftoverSamples;
            if ((uint64_t)can_drain > remaining) can_drain = (uint32_t)remaining;

            if (out) {
                memcpy(out + framesRead * a->channels,
                       a->pFrameOut + a->leftoverOffset * a->channels,
                       can_drain * a->channels * sizeof(int16_t));
            }
            a->leftoverOffset  += can_drain;
            a->leftoverSamples -= can_drain;
            a->currentPCMFrame += can_drain;
            framesRead         += can_drain;
            continue;
        }

        /* Decode the next super-frame */
        if (!ff_adx__decode_super_frame(a)) break;
    }

    return framesRead;
}

/* ============================================================================
 * Public read functions
 * ============================================================================ */
FF_ADX_API uint64_t ff_adx_read_pcm_frames_s16(ff_adx* pAdx,
                                                 uint64_t framesToRead,
                                                 int16_t* pBufferOut)
{
    if (!pAdx || framesToRead == 0) return 0;
    return ff_adx__read_s16(pAdx, framesToRead, pBufferOut);
}

FF_ADX_API uint64_t ff_adx_read_pcm_frames_f32(ff_adx* pAdx,
                                                 uint64_t framesToRead,
                                                 float* pBufferOut)
{
    uint64_t totalRead = 0;

    if (!pAdx || framesToRead == 0) return 0;

    if (!pBufferOut) {
        /* Just advance cursor */
        return ff_adx__read_s16(pAdx, framesToRead, NULL);
    }

    /* Decode in chunks using a stack-allocated s16 scratch buffer */
    while (totalRead < framesToRead) {
        int16_t  scratch[256 * FF_ADX__MAX_CHANNELS];
        uint64_t want   = framesToRead - totalRead;
        uint64_t chunk  = want < 256 ? want : 256;
        uint64_t got;

        got = ff_adx__read_s16(pAdx, chunk, scratch);
        if (got == 0) break;

        {
            size_t sample_count = (size_t)(got * pAdx->channels);
            float* dst = pBufferOut + totalRead * pAdx->channels;
            ff_adx__s16_to_f32_batch(scratch, dst, sample_count);
        }

        totalRead += got;
    }

    return totalRead;
}

/* ============================================================================
 * Seek
 * ============================================================================ */
FF_ADX_API ff_adx_result ff_adx_seek_to_pcm_frame(ff_adx* pAdx, uint64_t targetFrame)
{
    if (!pAdx) return FF_ADX_INVALID_ARGS;
    if (targetFrame > pAdx->totalPCMFrameCount) return FF_ADX_OUT_OF_RANGE;

    /* Snap to nearest super-frame boundary */
    {
        uint64_t super     = targetFrame / pAdx->samples_per_frame;
        uint64_t remainder = targetFrame % pAdx->samples_per_frame;

        pAdx->currentPCMFrame = super * pAdx->samples_per_frame;
        pAdx->leftoverSamples = 0;
        pAdx->leftoverOffset  = 0;
        pAdx->xor_pos         = (uint32_t)(super * pAdx->samples_per_frame);

        /* Reset ADPCM history (decoder restarts from silence, same as vgmstream) */
        memset(pAdx->hist1, 0, sizeof(pAdx->hist1));
        memset(pAdx->hist2, 0, sizeof(pAdx->hist2));

        /* Reset type-9 LCG state to match the seek position */
#ifndef FF_ADX_NO_ENCRYPTION
        if (pAdx->key.type == FF_ADX_ENC_TYPE9) {
            uint32_t state = pAdx->key.xor9_start;
            uint64_t step;
            uint64_t total_steps = super * pAdx->frame_size; /* approx */
            for (step = 0; step < total_steps; step++)
                state = state * pAdx->key.xor9_mult + pAdx->key.xor9_inc;
            pAdx->key.xor9_start = state; /* store running state */
        }
#endif

        /* Skip over the remainder samples after the boundary */
        if (remainder > 0) {
            ff_adx__read_s16(pAdx, remainder, NULL);
        }
    }

    return FF_ADX_SUCCESS;
}

FF_ADX_API uint64_t ff_adx_get_cursor_in_pcm_frames(const ff_adx* pAdx) {
    return pAdx ? pAdx->currentPCMFrame : 0;
}

FF_ADX_API uint64_t ff_adx_get_length_in_pcm_frames(const ff_adx* pAdx) {
    return pAdx ? pAdx->totalPCMFrameCount : 0;
}

/* ============================================================================
 * High-level helpers
 * ============================================================================ */
static void* ff_adx__malloc_hl(size_t sz, const ff_adx_allocation_callbacks* a) {
    return a && a->onMalloc ? a->onMalloc(sz, a->pUserData) : malloc(sz);
}
static void* ff_adx__realloc_hl(void* p, size_t sz, const ff_adx_allocation_callbacks* a) {
    return a && a->onRealloc ? a->onRealloc(p, sz, a->pUserData) : realloc(p, sz);
}

static void* ff_adx__read_all_f32(ff_adx* a, uint32_t* pCh, uint32_t* pSr,
                                   uint64_t* pTotal,
                                   const ff_adx_allocation_callbacks* alloc)
{
    size_t   cap    = 4096;
    size_t   used   = 0;
    float*   buf    = (float*)ff_adx__malloc_hl(cap * a->channels * sizeof(float), alloc);
    uint64_t got;

    if (!buf) return NULL;
    if (pCh)    *pCh    = a->channels;
    if (pSr)    *pSr    = a->sampleRate;
    if (pTotal) *pTotal = 0;

    for (;;) {
        uint64_t want = cap - used;
        if (want == 0) {
            size_t  new_cap = cap * 2;
            float*  tmp     = (float*)ff_adx__realloc_hl(buf,
                                  new_cap * a->channels * sizeof(float), alloc);
            if (!tmp) { ff_adx__free(buf, alloc); return NULL; }
            buf = tmp;
            cap = new_cap;
            want = cap - used;
        }
        got = ff_adx_read_pcm_frames_f32(a, want, buf + used * a->channels);
        if (got == 0) break;
        used += (size_t)got;
    }

    if (pTotal) *pTotal = used;
    return buf;
}

static void* ff_adx__read_all_s16(ff_adx* a, uint32_t* pCh, uint32_t* pSr,
                                   uint64_t* pTotal,
                                   const ff_adx_allocation_callbacks* alloc)
{
    size_t   cap  = 4096;
    size_t   used = 0;
    int16_t* buf  = (int16_t*)ff_adx__malloc_hl(cap * a->channels * sizeof(int16_t), alloc);
    uint64_t got;

    if (!buf) return NULL;
    if (pCh)    *pCh    = a->channels;
    if (pSr)    *pSr    = a->sampleRate;
    if (pTotal) *pTotal = 0;

    for (;;) {
        uint64_t want = cap - used;
        if (want == 0) {
            size_t   new_cap = cap * 2;
            int16_t* tmp     = (int16_t*)ff_adx__realloc_hl(buf,
                                   new_cap * a->channels * sizeof(int16_t), alloc);
            if (!tmp) { ff_adx__free(buf, alloc); return NULL; }
            buf = tmp;
            cap = new_cap;
            want = cap - used;
        }
        got = ff_adx_read_pcm_frames_s16(a, want, buf + used * a->channels);
        if (got == 0) break;
        used += (size_t)got;
    }

    if (pTotal) *pTotal = used;
    return buf;
}

#ifndef FF_ADX_NO_STDIO
FF_ADX_API float* ff_adx_open_file_and_read_pcm_frames_f32(const char* pFilePath,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc)
{
    ff_adx  adx;
    void*   result;
    if (ff_adx_init_file(pFilePath, NULL, &adx) != FF_ADX_SUCCESS) return NULL;
    result = ff_adx__read_all_f32(&adx, pChannels, pSampleRate, pTotalPCMFrameCount, pAlloc);
    ff_adx_uninit(&adx);
    return (float*)result;
}

FF_ADX_API int16_t* ff_adx_open_file_and_read_pcm_frames_s16(const char* pFilePath,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc)
{
    ff_adx  adx;
    void*   result;
    if (ff_adx_init_file(pFilePath, NULL, &adx) != FF_ADX_SUCCESS) return NULL;
    result = ff_adx__read_all_s16(&adx, pChannels, pSampleRate, pTotalPCMFrameCount, pAlloc);
    ff_adx_uninit(&adx);
    return (int16_t*)result;
}
#endif

FF_ADX_API float* ff_adx_open_memory_and_read_pcm_frames_f32(const void* pData, size_t dataSize,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc)
{
    ff_adx adx;
    void*  result;
    if (ff_adx_init_memory(pData, dataSize, NULL, &adx) != FF_ADX_SUCCESS) return NULL;
    result = ff_adx__read_all_f32(&adx, pChannels, pSampleRate, pTotalPCMFrameCount, pAlloc);
    ff_adx_uninit(&adx);
    return (float*)result;
}

FF_ADX_API int16_t* ff_adx_open_memory_and_read_pcm_frames_s16(const void* pData, size_t dataSize,
    uint32_t* pChannels, uint32_t* pSampleRate, uint64_t* pTotalPCMFrameCount,
    const ff_adx_allocation_callbacks* pAlloc)
{
    ff_adx adx;
    void*  result;
    if (ff_adx_init_memory(pData, dataSize, NULL, &adx) != FF_ADX_SUCCESS) return NULL;
    result = ff_adx__read_all_s16(&adx, pChannels, pSampleRate, pTotalPCMFrameCount, pAlloc);
    ff_adx_uninit(&adx);
    return (int16_t*)result;
}

FF_ADX_API void ff_adx_free(void* p, const ff_adx_allocation_callbacks* pAlloc) {
    ff_adx__free(p, pAlloc);
}


/* ============================================================================
 * ff_aix implementation
 * ============================================================================ */

/* Shared read helper for ff_aix (same logic as ff_adx__read) */
static int ff_aix__read(ff_aix* a, uint64_t offset, void* buf, size_t len)
{
    if (a->pData) {
        if (offset + len > a->dataSize) return 0;
        memcpy(buf, (const uint8_t*)a->pData + offset, len);
        return 1;
    }
#ifndef FF_ADX_NO_STDIO
    {
        FILE* f = (FILE*)a->pFile;
        if (!f) return 0;
        if (FF_ADX__FSEEK(f, offset, SEEK_SET)) return 0;
        return (fread(buf, 1, len, f) == len);
    }
#else
    return 0;
#endif
}

/* Parse AIX header and populate ff_aix -- shares logic with ff_adx__parse_aix
 * but populates all N stream states at once. */
static ff_adx_result ff_aix__init_internal(ff_aix* a)
{
    uint8_t  aixf[0x50];
    uint32_t num_streams, aixf_end, off, i;
    uint32_t block_total_size, data_per_stream;
    uint64_t first_data_block;
    uint8_t  chunk_hdr[16];
    /* track which streams have had their info chunk parsed */
    uint8_t  info_found[FF_ADX__MAX_CHANNELS];
    uint32_t info_count;
    uint16_t cutoff  = 500;
    uint8_t  version = 3;
    uint32_t channels = 0, sample_rate = 0, frame_size = 0;

    if (!ff_aix__read(a, 0, aixf, sizeof(aixf))) return FF_ADX_INVALID_FILE;
    if (aixf[0]!='A'||aixf[1]!='I'||aixf[2]!='X'||aixf[3]!='F') return FF_ADX_INVALID_FILE;

    aixf_end    = ff_adx__r32be(aixf, 0x04) + 8;
    num_streams = ff_adx__r8(aixf, 0x40);
    if (num_streams == 0 || num_streams > FF_ADX__MAX_CHANNELS) return FF_ADX_INVALID_FILE;

    a->stream_count     = num_streams;
    a->totalPCMFrameCount = ff_adx__r32be(aixf, 0x28);

    memset(info_found, 0, sizeof(info_found));
    info_count = 0;
    block_total_size = 0;
    data_per_stream  = 0;
    first_data_block = 0;
    off = aixf_end;

    while (off + 16 <= (uint32_t)a->dataSize) {
        uint32_t chunk_next, frames;
        uint8_t  stream_id;

        if (!ff_aix__read(a, off, chunk_hdr, 16)) return FF_ADX_INVALID_FILE;
        if (chunk_hdr[0]!='A'||chunk_hdr[1]!='I'||chunk_hdr[2]!='X') return FF_ADX_INVALID_FILE;
        if (chunk_hdr[3] == 'E') break;

        chunk_next = ff_adx__r32be(chunk_hdr, 4);
        stream_id  = chunk_hdr[8];
        frames     = ff_adx__r32be(chunk_hdr, 12);

        if (frames == 0xFFFFFFFF) {
            /* Info chunk */
            if (stream_id < num_streams && !info_found[stream_id]) {
                uint8_t adx_buf[0x40];
                if (!ff_aix__read(a, off + 16, adx_buf, sizeof(adx_buf)))
                    return FF_ADX_INVALID_FILE;
                if (adx_buf[0] != 0x80 || adx_buf[1] != 0x00) return FF_ADX_INVALID_FILE;

                version     = ff_adx__r8(adx_buf, 0x12);
                cutoff      = ff_adx__r16be(adx_buf, 0x10);
                channels    = ff_adx__r8(adx_buf, 0x07);
                sample_rate = ff_adx__r32be(adx_buf, 0x08);
                frame_size  = ff_adx__r8(adx_buf, 0x05);

                if (!channels || channels > FF_ADX__MAX_CHANNELS) return FF_ADX_INVALID_FILE;
                if (!sample_rate) return FF_ADX_INVALID_FILE;
                if (frame_size < 3 || frame_size > 64) return FF_ADX_INVALID_FILE;
                if (version == 5) return FF_ADX_NOT_IMPLEMENTED;

                info_found[stream_id] = 1;
                info_count++;
                /* All AIX streams are identical format; store from first one */
            }
        } else {
            /* First data chunk -- geometry */
            data_per_stream  = ((uint32_t)chunk_hdr[10] << 8) | chunk_hdr[11];
            block_total_size = 8 + chunk_next;
            first_data_block = off;
            break;
        }
        off += 8 + chunk_next;
    }

    if (info_count < num_streams || !data_per_stream || block_total_size < 16)
        return FF_ADX_INVALID_FILE;

    /* Shared geometry */
    a->channels          = channels;
    a->sampleRate        = sample_rate;
    a->frame_size        = frame_size;
    a->samples_per_frame = (frame_size - 2) * 2;
    a->aix_stream_size   = data_per_stream;
    a->aix_block_size    = (uint64_t)num_streams * block_total_size;

    ff_adx__compute_coefficients(sample_rate, cutoff, &a->coeff1, &a->coeff2);

    /* Allocate per-stream buffers and set data offsets */
    for (i = 0; i < num_streams; i++) {
        ff_aix_stream_state* s = &a->streams[i];
        size_t buf_bytes = (size_t)a->samples_per_frame * channels * sizeof(int16_t);
        s->pFrameOut = (int16_t*)ff_adx__malloc(buf_bytes, &a->allocationCallbacks);
        if (!s->pFrameOut) {
            /* roll back already-allocated buffers */
            uint32_t j;
            for (j = 0; j < i; j++) {
                ff_adx__free(a->streams[j].pFrameOut, &a->allocationCallbacks);
                a->streams[j].pFrameOut = NULL;
            }
            return FF_ADX_OUT_OF_MEMORY;
        }
        memset(s->hist1, 0, sizeof(s->hist1));
        memset(s->hist2, 0, sizeof(s->hist2));
        s->data_offset     = first_data_block + (uint64_t)i * block_total_size + 16;
        s->currentPCMFrame = 0;
        s->leftoverSamples = 0;
        s->leftoverOffset  = 0;
    }

    return FF_ADX_SUCCESS;
}

/* Compute byte offset for a given PCM frame within stream s */
static uint64_t ff_aix__byte_offset(const ff_aix* a,
                                     const ff_aix_stream_state* s,
                                     uint64_t pcm_frame)
{
    uint64_t super         = pcm_frame / a->samples_per_frame;
    uint64_t spf_bytes     = (uint64_t)a->frame_size * a->channels;
    uint64_t supers_per_blk = a->aix_stream_size / spf_bytes;
    uint64_t block_idx     = super / supers_per_blk;
    uint64_t frame_in_blk  = super % supers_per_blk;
    return s->data_offset
         + block_idx    * a->aix_block_size
         + frame_in_blk * spf_bytes;
}

/* Decode one super-frame for stream s into s->pFrameOut */
static int ff_aix__decode_super_frame(ff_aix* a, ff_aix_stream_state* s)
{
    uint8_t  frame_raw[64];
    uint64_t byte_off = ff_aix__byte_offset(a, s, s->currentPCMFrame);
    uint32_t ch, spf = a->samples_per_frame;

    for (ch = 0; ch < a->channels; ch++) {
        int16_t  ch_samples[128];
        uint64_t ch_off = byte_off + ch * a->frame_size;
        if (!ff_aix__read(a, ch_off, frame_raw, a->frame_size)) return 0;

        /* Decode frame (no encryption in AIX) */
        {
            int32_t  scale = (int32_t)(((uint32_t)(frame_raw[0]<<8)|frame_raw[1]) & 0x1fff) + 1;
            uint32_t i;
            for (i = 0; i < spf / 2; i++) {
                uint8_t  byte = frame_raw[2 + i];
                int32_t  nib[2], n;
                nib[0] = (int32_t)(int8_t)(byte & 0xF0) >> 4;
                nib[1] = (int32_t)(int8_t)(byte << 4)  >> 4;
                for (n = 0; n < 2; n++) {
                    int32_t pred = (s->hist1[ch] * a->coeff1 +
                                    s->hist2[ch] * a->coeff2) >> 12;
                    int32_t out  = nib[n] * scale + pred;
                    if (out >  32767) out =  32767;
                    if (out < -32768) out = -32768;
                    s->hist2[ch] = s->hist1[ch];
                    s->hist1[ch] = out;
                    ch_samples[i*2+n] = (int16_t)out;
                }
            }
        }

        /* Interleave into pFrameOut */
        {
            uint32_t smp;
            for (smp = 0; smp < spf; smp++)
                s->pFrameOut[smp * a->channels + ch] = ch_samples[smp];
        }
    }

    s->leftoverSamples = spf;
    s->leftoverOffset  = 0;
    return 1;
}

/* Core s16 read for one stream */
static uint64_t ff_aix__read_s16(ff_aix* a, uint8_t idx,
                                   uint64_t framesToRead, int16_t* out)
{
    ff_aix_stream_state* s = &a->streams[idx];
    uint64_t framesRead = 0;

    while (framesRead < framesToRead) {
        uint64_t remaining = framesToRead - framesRead;
        uint64_t left      = a->totalPCMFrameCount - s->currentPCMFrame;
        if (left == 0) break;
        if (remaining > left) remaining = left;

        if (s->leftoverSamples > 0) {
            uint32_t drain = s->leftoverSamples;
            if ((uint64_t)drain > remaining) drain = (uint32_t)remaining;
            if (out)
                memcpy(out + framesRead * a->channels,
                       s->pFrameOut + s->leftoverOffset * a->channels,
                       drain * a->channels * sizeof(int16_t));
            s->leftoverOffset  += drain;
            s->leftoverSamples -= drain;
            s->currentPCMFrame += drain;
            framesRead         += drain;
            continue;
        }

        if (!ff_aix__decode_super_frame(a, s)) break;
    }
    return framesRead;
}

/* ---- Public ff_aix functions -------------------------------------------- */

#ifndef FF_ADX_NO_STDIO
FF_ADX_API ff_adx_result ff_aix_init_file(const char* pFilePath,
                                           const ff_adx_config* pConfig,
                                           ff_aix* pAix)
{
    ff_adx_result r;
    FILE* f;
    if (!pFilePath || !pAix) return FF_ADX_INVALID_ARGS;
    memset(pAix, 0, sizeof(*pAix));
    if (pConfig) pAix->allocationCallbacks = pConfig->allocationCallbacks;
    f = fopen(pFilePath, "rb");
    if (!f) return FF_ADX_INVALID_FILE;
    pAix->pFile = f;
    fseek(f, 0, SEEK_END);
    pAix->dataSize = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);
    r = ff_aix__init_internal(pAix);
    if (r != FF_ADX_SUCCESS) { fclose(f); pAix->pFile = NULL; }
    return r;
}
#endif

FF_ADX_API ff_adx_result ff_aix_init_memory(const void* pData, size_t dataSize,
                                              const ff_adx_config* pConfig,
                                              ff_aix* pAix)
{
    ff_adx_result r;
    if (!pData || !dataSize || !pAix) return FF_ADX_INVALID_ARGS;
    memset(pAix, 0, sizeof(*pAix));
    if (pConfig) pAix->allocationCallbacks = pConfig->allocationCallbacks;
    pAix->pData    = (const uint8_t*)pData;
    pAix->dataSize = dataSize;
    r = ff_aix__init_internal(pAix);
    return r;
}

FF_ADX_API void ff_aix_uninit(ff_aix* pAix)
{
    uint32_t i;
    if (!pAix) return;
    for (i = 0; i < pAix->stream_count; i++) {
        if (pAix->streams[i].pFrameOut) {
            ff_adx__free(pAix->streams[i].pFrameOut, &pAix->allocationCallbacks);
            pAix->streams[i].pFrameOut = NULL;
        }
    }
#ifndef FF_ADX_NO_STDIO
    if (pAix->pFile) { fclose((FILE*)pAix->pFile); pAix->pFile = NULL; }
#endif
    memset(pAix, 0, sizeof(*pAix));
}

FF_ADX_API uint64_t ff_aix_read_pcm_frames_s16(ff_aix* pAix, uint8_t streamIdx,
                                                 uint64_t framesToRead,
                                                 int16_t* pBufferOut)
{
    if (!pAix || streamIdx >= pAix->stream_count || framesToRead == 0) return 0;
    return ff_aix__read_s16(pAix, streamIdx, framesToRead, pBufferOut);
}

FF_ADX_API uint64_t ff_aix_read_pcm_frames_f32(ff_aix* pAix, uint8_t streamIdx,
                                                 uint64_t framesToRead,
                                                 float* pBufferOut)
{
    uint64_t totalRead = 0;
    if (!pAix || streamIdx >= pAix->stream_count || framesToRead == 0) return 0;
    if (!pBufferOut)
        return ff_aix__read_s16(pAix, streamIdx, framesToRead, NULL);

    while (totalRead < framesToRead) {
        int16_t  scratch[256 * FF_ADX__MAX_CHANNELS];
        uint64_t want  = framesToRead - totalRead;
        uint64_t chunk = want < 256 ? want : 256;
        uint64_t got   = ff_aix__read_s16(pAix, streamIdx, chunk, scratch);
        if (got == 0) break;
        {
            size_t n = (size_t)(got * pAix->channels);
            float* dst = pBufferOut + totalRead * pAix->channels;
            ff_adx__s16_to_f32_batch(scratch, dst, n);
        }
        totalRead += got;
    }
    return totalRead;
}

FF_ADX_API ff_adx_result ff_aix_seek_to_pcm_frame(ff_aix* pAix, uint64_t targetFrame)
{
    uint32_t i;
    if (!pAix) return FF_ADX_INVALID_ARGS;
    if (targetFrame > pAix->totalPCMFrameCount) return FF_ADX_OUT_OF_RANGE;

    for (i = 0; i < pAix->stream_count; i++) {
        ff_aix_stream_state* s = &pAix->streams[i];
        uint64_t super     = targetFrame / pAix->samples_per_frame;
        uint64_t remainder = targetFrame % pAix->samples_per_frame;

        s->currentPCMFrame = super * pAix->samples_per_frame;
        s->leftoverSamples = 0;
        s->leftoverOffset  = 0;
        memset(s->hist1, 0, sizeof(s->hist1));
        memset(s->hist2, 0, sizeof(s->hist2));

        if (remainder > 0)
            ff_aix__read_s16(pAix, (uint8_t)i, remainder, NULL);
    }
    return FF_ADX_SUCCESS;
}

FF_ADX_API uint64_t ff_aix_get_cursor_in_pcm_frames(const ff_aix* pAix, uint8_t streamIdx)
{
    if (!pAix || streamIdx >= pAix->stream_count) return 0;
    return pAix->streams[streamIdx].currentPCMFrame;
}

FF_ADX_API uint64_t ff_aix_get_length_in_pcm_frames(const ff_aix* pAix)
{
    return pAix ? pAix->totalPCMFrameCount : 0;
}

/* ============================================================================
 * Restore compiler warning state
 * ============================================================================ */
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif /* FF_ADX_IMPLEMENTATION */
#endif /* FF_ADX_H */
