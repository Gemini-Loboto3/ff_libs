/*
 * example_opus.c - Decode an Opus file to WAV
 *
 * Build:
 *   gcc -o example_opus example_opus.c -lm
 *
 * Usage:
 *   ./example_opus input.opus output.wav
 */

#define FF_OPUS_IMPLEMENTATION
#include "ff_opus.h"

#include <stdio.h>
#include <string.h>

/* Minimal WAV writer (16-bit PCM) */
static void write_wav(const char* path, const int16_t* samples, uint64_t totalFrames, int channels, int sampleRate)
{
    FILE* f = fopen(path, "wb");
    if (!f) {
        fprintf(stderr, "Error: Could not open %s for writing\n", path);
        return;
    }

    unsigned int dataSize = (unsigned int)(totalFrames * channels * sizeof(int16_t));
    unsigned int fileSize = 36 + dataSize;
    unsigned short audioFormat = 1; /* PCM */
    unsigned short ch = (unsigned short)channels;
    unsigned int sr = (unsigned int)sampleRate;
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
/* Example 1: One-shot API - load entire file as s16 directly                 */
/* -------------------------------------------------------------------------- */
static int decode_oneshot(const char* inputPath, const char* outputPath)
{
    int channels, sampleRate;
    uint64_t totalFrames;

    printf("Decoding (one-shot): %s\n", inputPath);

    int16_t* samples = ff_opus_open_file_and_read_pcm_frames_s16(
        inputPath, &channels, &sampleRate, &totalFrames, NULL);

    if (!samples) {
        fprintf(stderr, "Error: Failed to decode %s\n", inputPath);
        return 1;
    }

    printf("  Channels:     %d\n", channels);
    printf("  Sample rate:  %d Hz\n", sampleRate);
    printf("  Total frames: %llu\n", (unsigned long long)totalFrames);
    printf("  Duration:     %.2f seconds\n", (double)totalFrames / sampleRate);

    write_wav(outputPath, samples, totalFrames, channels, sampleRate);
    printf("  Written to:   %s\n", outputPath);

    ff_opus_free(samples, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Example 2: Streaming API - decode in chunks as s16 directly                */
/* -------------------------------------------------------------------------- */
static int decode_streaming(const char* inputPath, const char* outputPath)
{
    ff_opus opus;
    ff_opus_result result;

    printf("Decoding (streaming): %s\n", inputPath);

    result = ff_opus_init_file(inputPath, NULL, &opus);
    if (result != FF_OPUS_SUCCESS) {
        fprintf(stderr, "Error: Failed to open %s (error %d)\n", inputPath, result);
        return 1;
    }

    printf("  Channels:     %u\n", opus.channels);
    printf("  Sample rate:  %u Hz\n", opus.sampleRate);

    #define CHUNK_FRAMES 1024
    int16_t chunkBuf[CHUNK_FRAMES * 2]; /* max 2 channels */

    size_t capacity = 4096;
    size_t totalSamples = 0;
    int16_t* allSamples = (int16_t*)malloc(capacity * sizeof(int16_t));

    uint64_t framesRead;
    while ((framesRead = ff_opus_read_pcm_frames_s16(&opus, CHUNK_FRAMES, chunkBuf)) > 0) {
        size_t samplesThisChunk = (size_t)(framesRead * opus.channels);

        while (totalSamples + samplesThisChunk > capacity) {
            capacity *= 2;
            allSamples = (int16_t*)realloc(allSamples, capacity * sizeof(int16_t));
        }

        memcpy(allSamples + totalSamples, chunkBuf, samplesThisChunk * sizeof(int16_t));
        totalSamples += samplesThisChunk;
    }

    uint64_t totalFrames = totalSamples / opus.channels;
    printf("  Total frames: %llu\n", (unsigned long long)totalFrames);
    printf("  Duration:     %.2f seconds\n", (double)totalFrames / opus.sampleRate);

    write_wav(outputPath, allSamples, totalFrames, opus.channels, opus.sampleRate);
    printf("  Written to:   %s\n", outputPath);

    free(allSamples);
    ff_opus_uninit(&opus);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <input.opus> <output.wav> [--stream]\n", argv[0]);
        printf("  --stream    Use streaming API instead of one-shot\n");
        return 1;
    }

    int streaming = (argc > 3 && strcmp(argv[3], "--stream") == 0);

    if (streaming)
        return decode_streaming(argv[1], argv[2]);
    else
        return decode_oneshot(argv[1], argv[2]);
}
