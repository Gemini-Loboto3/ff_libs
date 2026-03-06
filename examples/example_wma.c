/*
 * example_wma.c - Decode a WMA file to WAV
 *
 * Build:
 *   gcc -o example_wma example_wma.c -lm
 *
 * Usage:
 *   ./example_wma input.wma output.wav
 */

#define FF_WMA_IMPLEMENTATION
#include "ff_wma.h"

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

    int16_t* samples = ff_wma_open_file_and_read_pcm_frames_s16(
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

    ff_wma_free(samples, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Example 2: Streaming API with seek demonstration                           */
/* -------------------------------------------------------------------------- */
static int decode_streaming(const char* inputPath, const char* outputPath)
{
    ff_wma wma;
    ff_wma_result result;

    printf("Decoding (streaming): %s\n", inputPath);

    result = ff_wma_init_file(inputPath, NULL, &wma);
    if (result != FF_WMA_SUCCESS) {
        fprintf(stderr, "Error: Failed to open %s (error %d)\n", inputPath, result);
        return 1;
    }

    printf("  Channels:     %u\n", wma.channels);
    printf("  Sample rate:  %u Hz\n", wma.sampleRate);

    uint64_t totalLength = ff_wma_get_length_in_pcm_frames(&wma);
    if (totalLength > 0) {
        printf("  Length:       %llu frames (%.2f seconds)\n",
               (unsigned long long)totalLength, (double)totalLength / wma.sampleRate);
    }

    /* Demonstrate seeking: skip first 1 second if file is long enough */
    if (totalLength > wma.sampleRate * 2) {
        result = ff_wma_seek_to_pcm_frame(&wma, wma.sampleRate);
        if (result == FF_WMA_SUCCESS) {
            printf("  Seeked to:    frame %u (1 second in)\n", wma.sampleRate);
        }
    }

    #define CHUNK_FRAMES 2048
    int16_t chunkBuf[CHUNK_FRAMES * 2]; /* max stereo */

    size_t capacity = 65536;
    size_t totalSamples = 0;
    int16_t* allSamples = (int16_t*)malloc(capacity * sizeof(int16_t));

    uint64_t framesRead;
    while ((framesRead = ff_wma_read_pcm_frames_s16(&wma, CHUNK_FRAMES, chunkBuf)) > 0) {
        size_t samplesThisChunk = (size_t)(framesRead * wma.channels);

        while (totalSamples + samplesThisChunk > capacity) {
            capacity *= 2;
            allSamples = (int16_t*)realloc(allSamples, capacity * sizeof(int16_t));
        }

        memcpy(allSamples + totalSamples, chunkBuf, samplesThisChunk * sizeof(int16_t));
        totalSamples += samplesThisChunk;
    }

    uint64_t totalFrames = totalSamples / wma.channels;
    printf("  Decoded:      %llu frames\n", (unsigned long long)totalFrames);

    write_wav(outputPath, allSamples, totalFrames, wma.channels, wma.sampleRate);
    printf("  Written to:   %s\n", outputPath);

    free(allSamples);
    ff_wma_uninit(&wma);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <input.wma> <output.wav> [--stream]\n", argv[0]);
        printf("  --stream    Use streaming API (also demonstrates seeking)\n");
        return 1;
    }

    int streaming = (argc > 3 && strcmp(argv[3], "--stream") == 0);

    if (streaming)
        return decode_streaming(argv[1], argv[2]);
    else
        return decode_oneshot(argv[1], argv[2]);
}
