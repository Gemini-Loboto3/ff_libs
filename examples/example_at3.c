/*
 * example_at3.c - Decode an ATRAC3/ATRAC3+ file to WAV
 *
 * Build:
 *   gcc -o example_at3 example_at3.c -lm
 *
 * Usage:
 *   ./example_at3 input.at3 output.wav
 *
 * Supported input formats:
 *   - ATRAC3 in RIFF WAV container (.at3, .wav)
 *   - ATRAC3+ in RIFF WAV container (.at3, .wav)
 *   - ATRAC3/3+ in OMA container (.oma, .aa3)
 */

#define FF_AT3_IMPLEMENTATION
#include "ff_at3.h"

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

static const char* codec_name(uint32_t codec)
{
    switch (codec) {
        case FF_AT3_CODEC_ATRAC3:  return "ATRAC3";
        case FF_AT3_CODEC_ATRAC3P: return "ATRAC3+";
        default:                   return "Unknown";
    }
}

/* -------------------------------------------------------------------------- */
/* Example 1: One-shot API - decode entire file to s16 directly               */
/* -------------------------------------------------------------------------- */
static int decode_oneshot(const char* inputPath, const char* outputPath)
{
    uint32_t channels, sampleRate;
    uint64_t totalFrames;

    printf("Decoding (one-shot): %s\n", inputPath);

    int16_t* samples = ff_at3_open_file_and_read_pcm_frames_s16(
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

    ff_at3_free(samples, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Example 2: Streaming API with codec info                                   */
/* -------------------------------------------------------------------------- */
static int decode_streaming(const char* inputPath, const char* outputPath)
{
    ff_at3 at3;
    ff_at3_result result;

    printf("Decoding (streaming): %s\n", inputPath);

    result = ff_at3_init_file(inputPath, NULL, &at3);
    if (result != FF_AT3_SUCCESS) {
        fprintf(stderr, "Error: Failed to open %s (error %d)\n", inputPath, result);
        return 1;
    }

    printf("  Codec:        %s\n", codec_name(at3.codecType));
    printf("  Channels:     %u\n", at3.channels);
    printf("  Sample rate:  %u Hz\n", at3.sampleRate);
    printf("  Total frames: %llu\n", (unsigned long long)at3.totalPCMFrameCount);

    if (at3.totalPCMFrameCount > 0) {
        printf("  Duration:     %.2f seconds\n",
               (double)at3.totalPCMFrameCount / at3.sampleRate);
    }

    #define CHUNK_FRAMES 2048
    int16_t chunkBuf[CHUNK_FRAMES * 2]; /* max stereo */

    size_t capacity = 65536;
    size_t totalSamples = 0;
    int16_t* allSamples = (int16_t*)malloc(capacity * sizeof(int16_t));

    uint64_t framesRead;
    while ((framesRead = ff_at3_read_pcm_frames_s16(&at3, CHUNK_FRAMES, chunkBuf)) > 0) {
        size_t samplesThisChunk = (size_t)(framesRead * at3.channels);

        while (totalSamples + samplesThisChunk > capacity) {
            capacity *= 2;
            allSamples = (int16_t*)realloc(allSamples, capacity * sizeof(int16_t));
        }

        memcpy(allSamples + totalSamples, chunkBuf, samplesThisChunk * sizeof(int16_t));
        totalSamples += samplesThisChunk;
    }

    uint64_t totalFrames = totalSamples / at3.channels;
    printf("  Decoded:      %llu frames\n", (unsigned long long)totalFrames);

    write_wav(outputPath, allSamples, totalFrames, at3.channels, at3.sampleRate);
    printf("  Written to:   %s\n", outputPath);

    free(allSamples);
    ff_at3_uninit(&at3);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <input.at3|.oma> <output.wav> [--stream]\n", argv[0]);
        printf("  --stream    Use streaming API\n");
        return 1;
    }

    int streaming = (argc > 3 && strcmp(argv[3], "--stream") == 0);

    if (streaming)
        return decode_streaming(argv[1], argv[2]);
    else
        return decode_oneshot(argv[1], argv[2]);
}
