/*
 * example_vorbis.c - Decode an Ogg Vorbis file to WAV
 *
 * Build:
 *   gcc -o example_vorbis example_vorbis.c -lm
 *
 * Usage:
 *   ./example_vorbis input.ogg output.wav
 */

#define FF_VORBIS_IMPLEMENTATION
#include "ff_vorbis.h"

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

    int16_t* samples = ff_vorbis_open_file_and_read_pcm_frames_s16(
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

    ff_vorbis_free(samples, NULL);
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Example 2: Streaming API - decode in s16 chunks with seeking               */
/* -------------------------------------------------------------------------- */
static int decode_streaming(const char* inputPath, const char* outputPath)
{
    ff_vorbis vorbis;
    ff_vorbis_result result;

    printf("Decoding (streaming): %s\n", inputPath);

    result = ff_vorbis_init_file(inputPath, NULL, &vorbis);
    if (result != FF_VORBIS_SUCCESS) {
        fprintf(stderr, "Error: Failed to open %s (error %d)\n", inputPath, result);
        return 1;
    }

    printf("  Channels:     %u\n", vorbis.channels);
    printf("  Sample rate:  %u Hz\n", vorbis.sampleRate);

    uint64_t totalLength = ff_vorbis_get_length_in_pcm_frames(&vorbis);
    if (totalLength > 0) {
        printf("  Length:       %llu frames (%.2f seconds)\n",
               (unsigned long long)totalLength, (double)totalLength / vorbis.sampleRate);
    }

    #define CHUNK_FRAMES 4096
    int16_t chunkBuf[CHUNK_FRAMES * 8]; /* up to 8 channels */

    size_t capacity = 65536;
    size_t totalSamples = 0;
    int16_t* allSamples = (int16_t*)malloc(capacity * sizeof(int16_t));

    uint64_t framesRead;
    while ((framesRead = ff_vorbis_read_pcm_frames_s16(&vorbis, CHUNK_FRAMES, chunkBuf)) > 0) {
        size_t samplesThisChunk = (size_t)(framesRead * vorbis.channels);

        while (totalSamples + samplesThisChunk > capacity) {
            capacity *= 2;
            allSamples = (int16_t*)realloc(allSamples, capacity * sizeof(int16_t));
        }

        memcpy(allSamples + totalSamples, chunkBuf, samplesThisChunk * sizeof(int16_t));
        totalSamples += samplesThisChunk;
    }

    uint64_t totalFrames = totalSamples / vorbis.channels;
    printf("  Decoded:      %llu frames\n", (unsigned long long)totalFrames);

    write_wav(outputPath, allSamples, totalFrames, vorbis.channels, vorbis.sampleRate);
    printf("  Written to:   %s\n", outputPath);

    free(allSamples);
    ff_vorbis_uninit(&vorbis);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        printf("Usage: %s <input.ogg> <output.wav> [--stream]\n", argv[0]);
        printf("  --stream    Use streaming API\n");
        return 1;
    }

    int streaming = (argc > 3 && strcmp(argv[3], "--stream") == 0);

    if (streaming)
        return decode_streaming(argv[1], argv[2]);
    else
        return decode_oneshot(argv[1], argv[2]);
}
