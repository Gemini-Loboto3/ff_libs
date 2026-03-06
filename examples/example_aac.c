/*
 * example_aac.c - Decode an AAC file to WAV
 *
 * Build:
 *   gcc -o example_aac example_aac.c -lm
 *
 * Usage:
 *   ./example_aac input.aac output.wav [--stream]
 *
 *   --stream    Use streaming/chunked API instead of one-shot API
 *
 * Supported input formats:
 *   - AAC-LC in ADTS container (.aac)
 *   - AAC-LC in M4A/MP4 container (.m4a, .mp4)
 *
 * Supported AAC profiles:
 *   - AAC-LC (Low Complexity)
 *   - HE-AAC / AAC+ (High Efficiency, v1 and v2)
 *
 * API modes demonstrated:
 *   One-shot:  ff_aac_open_file_and_read_pcm_frames_s16() - decode entire file at once
 *   Streaming: ff_aac_init_file() + ff_aac_read_pcm_frames_s16() - decode in chunks
 */

#define FF_AAC_IMPLEMENTATION
#include "ff_aac.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Minimal WAV writer (16-bit PCM) */
static void write_wav(const char* path, const int16_t* samples, uint64_t totalFrames, uint32_t channels, uint32_t sampleRate)
{
    FILE* f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Error: Could not open %s for writing\n", path);
        return;
    }

    unsigned int dataSize = (unsigned int)(totalFrames * channels * sizeof(int16_t));
    unsigned int fileSize = 36 + dataSize;
    unsigned short audioFormat = 1;
    unsigned short ch = (unsigned short)channels;
    unsigned int sr = sampleRate;
    unsigned int byteRate = sr * ch * 2;
    unsigned short blockAlign = ch * 2;
    unsigned short bitsPerSample = 16;
    unsigned int fmtSize = 16;

    fwrite("RIFF", 1, 4, f);
    fwrite(&fileSize, 4, 1, f);
    fwrite("WAVE", 1, 4, f);
    fwrite("fmt ", 1, 4, f);
    fwrite(&fmtSize, 4, 1, f);
    fwrite(&audioFormat, 2, 1, f);
    fwrite(&ch, 2, 1, f);
    fwrite(&sr, 4, 1, f);
    fwrite(&byteRate, 4, 1, f);
    fwrite(&blockAlign, 2, 1, f);
    fwrite(&bitsPerSample, 2, 1, f);
    fwrite("data", 1, 4, f);
    fwrite(&dataSize, 4, 1, f);
    fwrite(samples, 1, dataSize, f);
    fclose(f);
}

/* -------------------------------------------------------------------------- */
/* Example 1: One-shot API - decode entire file to s16 directly               */
/* -------------------------------------------------------------------------- */
static int decode_oneshot(const char* inputPath, const char* outputPath)
{
    uint32_t channels, sampleRate;
    uint64_t totalFrames;

    printf("Decoding (one-shot): %s\n", inputPath);

    int16_t* samples = ff_aac_open_file_and_read_pcm_frames_s16(
        inputPath, &channels, &sampleRate, &totalFrames, NULL);

    if (!samples) {
        fprintf(stderr, "Error: Failed to decode %s\n", inputPath);
        return 1;
    }

    printf("  Channels:     %u\n", channels);
    printf("  Sample rate:  %u Hz\n", sampleRate);
    printf("  Total frames: %llu\n", (unsigned long long)totalFrames);
    printf("  Duration:     %.2f seconds\n", (double)totalFrames / sampleRate);

    write_wav(outputPath, samples, totalFrames, channels, sampleRate);
    printf("  Written to:   %s\n", outputPath);

    ff_aac_free(samples, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Example 2: Streaming API - decode in chunks                                */
/* -------------------------------------------------------------------------- */
static int decode_streaming(const char* inputPath, const char* outputPath)
{
    ff_aac aac;
    ff_aac_result result;

    printf("Decoding (streaming): %s\n", inputPath);

    result = ff_aac_init_file(inputPath, NULL, &aac);
    if (result != FF_AAC_SUCCESS) {
        fprintf(stderr, "Error: Failed to open %s (error %d)\n", inputPath, result);
        return 1;
    }

    printf("  Channels:     %u\n", aac.channels);
    printf("  Sample rate:  %u Hz\n", aac.sampleRate);
    printf("  Total frames: %llu\n", (unsigned long long)aac.totalPCMFrameCount);

    if (aac.totalPCMFrameCount > 0) {
        printf("  Duration:     %.2f seconds\n",
               (double)aac.totalPCMFrameCount / aac.sampleRate);
    }

    #define CHUNK_FRAMES 2048
    int16_t chunkBuf[CHUNK_FRAMES * 8]; /* up to 8 channels */

    size_t capacity = 65536;
    size_t totalSamples = 0;
    int16_t* allSamples = (int16_t*)malloc(capacity * sizeof(int16_t));

    uint64_t framesRead;
    while ((framesRead = ff_aac_read_pcm_frames_s16(&aac, CHUNK_FRAMES, chunkBuf)) > 0) {
        size_t samplesThisChunk = (size_t)(framesRead * aac.channels);

        while (totalSamples + samplesThisChunk > capacity) {
            capacity *= 2;
            allSamples = (int16_t*)realloc(allSamples, capacity * sizeof(int16_t));
        }

        memcpy(allSamples + totalSamples, chunkBuf, samplesThisChunk * sizeof(int16_t));
        totalSamples += samplesThisChunk;
    }

    uint64_t totalFrames = totalSamples / aac.channels;
    printf("  Decoded:      %llu frames\n", (unsigned long long)totalFrames);

    write_wav(outputPath, allSamples, totalFrames, aac.channels, aac.sampleRate);
    printf("  Written to:   %s\n", outputPath);

    free(allSamples);
    ff_aac_uninit(&aac);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <input.aac> <output.wav> [--stream]\n", argv[0]);
        printf("  --stream    Use streaming API\n");
        return 1;
    }

    int streaming = (argc > 3 && strcmp(argv[3], "--stream") == 0);

    if (streaming)
        return decode_streaming(argv[1], argv[2]);
    else
        return decode_oneshot(argv[1], argv[2]);
}
