/*
 * example_adx.c - Decode a CRI ADX or AIX file to WAV
 *
 * Build:
 *   gcc -o example_adx example_adx.c -lm
 *
 * Usage:
 *   ./example_adx input.adx output.wav
 *   ./example_adx input.adx output.wav --stream
 *   ./example_adx input.aix output.wav --aix-stream 1
 *
 * Supported input formats:
 *   - ADX v1/v2/v3/v4 (.adx)
 *   - ADX with type-8 or type-9 XOR encryption (.adx) -- requires key in config
 *   - AIX multi-stream interleaved (.aix)
 */

#define FF_ADX_IMPLEMENTATION
#include "ff_adx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Minimal WAV writer (16-bit PCM) */
static void write_wav(const char* path, const int16_t* samples,
                      uint64_t totalFrames, uint32_t channels, uint32_t sampleRate)
{
    FILE* f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Error: Could not open %s for writing\n", path);
        return;
    }

    unsigned int dataSize   = (unsigned int)(totalFrames * channels * sizeof(int16_t));
    unsigned int fileSize   = 36 + dataSize;
    unsigned short audioFmt = 1;
    unsigned short ch       = (unsigned short)channels;
    unsigned int sr         = sampleRate;
    unsigned int byteRate   = sr * ch * 2;
    unsigned short blkAlign = (unsigned short)(ch * 2);
    unsigned short bps      = 16;
    unsigned int fmtSize    = 16;

    fwrite("RIFF", 1, 4, f); fwrite(&fileSize,  4, 1, f);
    fwrite("WAVE", 1, 4, f);
    fwrite("fmt ", 1, 4, f); fwrite(&fmtSize,   4, 1, f);
    fwrite(&audioFmt, 2, 1, f); fwrite(&ch,     2, 1, f);
    fwrite(&sr,       4, 1, f); fwrite(&byteRate,4, 1, f);
    fwrite(&blkAlign, 2, 1, f); fwrite(&bps,    2, 1, f);
    fwrite("data", 1, 4, f); fwrite(&dataSize,  4, 1, f);
    fwrite(samples, 1, dataSize, f);
    fclose(f);
}

/* Open, print info, decode and write WAV. Used by both one-shot and streaming
 * modes -- the only difference is chunk size. Both go through ff_adx_init_file
 * so that aix_stream_index in the config is always honoured. */
static int decode(const char* inputPath, const char* outputPath,
                  uint8_t aix_stream_index, int streaming)
{
    ff_adx        adx;
    ff_adx_config cfg;
    ff_adx_result result;

    #define CHUNK_FRAMES_STREAMING  2048
    #define CHUNK_FRAMES_ONESHOT    65536

    uint32_t chunk_frames = streaming ? CHUNK_FRAMES_STREAMING : CHUNK_FRAMES_ONESHOT;

    printf("Decoding (%s): %s\n", streaming ? "streaming" : "one-shot", inputPath);

    memset(&cfg, 0, sizeof(cfg));
    cfg.aix_stream_index = aix_stream_index;

    result = ff_adx_init_file(inputPath, &cfg, &adx);
    if (result == FF_ADX_ENCRYPTED) {
        fprintf(stderr, "Error: File is encrypted -- provide a decryption key via ff_adx_config\n");
        return 1;
    }
    if (result != FF_ADX_SUCCESS) {
        fprintf(stderr, "Error: Failed to open %s (error %d)\n", inputPath, result);
        return 1;
    }

    printf("  Container:    %s\n",
           adx.info.container_type == FF_ADX_CONTAINER_AIX ? "AIX" : "ADX");
    printf("  ADX version:  %u\n",   adx.info.adx_version);
    printf("  Channels:     %u\n",   adx.channels);
    printf("  Sample rate:  %u Hz\n", adx.sampleRate);
    printf("  Total frames: %llu\n",  (unsigned long long)adx.totalPCMFrameCount);
    printf("  Duration:     %.2f seconds\n",
           (double)adx.totalPCMFrameCount / adx.sampleRate);
    if (adx.info.has_loop) {
        printf("  Loop:         sample %llu -> %llu\n",
               (unsigned long long)adx.info.loop_start_sample,
               (unsigned long long)adx.info.loop_end_sample);
    }
    if (adx.info.container_type == FF_ADX_CONTAINER_AIX) {
        printf("  AIX streams:  %u (decoding stream %u)\n",
               adx.info.aix_stream_count, aix_stream_index);
    }

    {
        int16_t* chunkBuf   = (int16_t*)malloc(
            chunk_frames * adx.channels * sizeof(int16_t));
        size_t   capacity     = chunk_frames * 4;
        size_t   totalSamples = 0;
        int16_t* allSamples   = (int16_t*)malloc(capacity * sizeof(int16_t));
        uint64_t framesRead;

        while ((framesRead = ff_adx_read_pcm_frames_s16(
                    &adx, chunk_frames, chunkBuf)) > 0) {
            size_t samplesThisChunk = (size_t)(framesRead * adx.channels);
            while (totalSamples + samplesThisChunk > capacity) {
                capacity *= 2;
                allSamples = (int16_t*)realloc(allSamples,
                                               capacity * sizeof(int16_t));
            }
            memcpy(allSamples + totalSamples, chunkBuf,
                   samplesThisChunk * sizeof(int16_t));
            totalSamples += samplesThisChunk;
        }

        {
            uint64_t totalFrames = totalSamples / adx.channels;
            printf("  Decoded:      %llu frames\n", (unsigned long long)totalFrames);
            write_wav(outputPath, allSamples, totalFrames, adx.channels, adx.sampleRate);
            printf("  Written to:   %s\n", outputPath);
        }

        free(chunkBuf);
        free(allSamples);
    }

    ff_adx_uninit(&adx);
    return 0;
}

int main(int argc, char** argv)
{
    int     streaming        = 0;
    uint8_t aix_stream_index = 0;
    int     i;

    if (argc < 3) {
        printf("Usage: %s <input.adx|.aix> <output.wav> [--stream] [--aix-stream N]\n",
               argv[0]);
        printf("  --stream        Decode in small chunks (default: one large pass)\n");
        printf("  --aix-stream N  Select AIX sub-stream index (0-based, default: 0)\n");
        return 1;
    }

    for (i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--stream") == 0) {
            streaming = 1;
        } else if (strcmp(argv[i], "--aix-stream") == 0 && i + 1 < argc) {
            aix_stream_index = (uint8_t)atoi(argv[++i]);
        }
    }

    return decode(argv[1], argv[2], aix_stream_index, streaming);
}
