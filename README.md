<h4 align="center">Single-file audio decoding libraries for C and C++, based on FFmpeg.</h4>

<p align="center">
    <a href="#license"><img src="https://img.shields.io/badge/license-LGPL--2.1-blue?style=flat-square" alt="license"></a>
</p>

ff_libs is a collection of portable, single-header audio decoders ported primarily from [FFmpeg](https://ffmpeg.org/). The API design is inspired by [dr_libs](https://github.com/mackron/dr_libs) by David Reid, but ff_libs is a **separate, independently maintained project**.

Each library compiles cleanly on both GCC and MSVC with no external dependencies.

## Libraries

Library                                         | Description                                                        | Based On
----------------------------------------------- | ------------------------------------------------------------------ | --------
[ff_opus](ff_opus.h)                            | Opus audio decoder with Ogg container support.                     | [libopus](https://opus-codec.org/) 1.4
[ff_vorbis](ff_vorbis.h)                        | Ogg Vorbis audio decoder.                                          | [stb_vorbis](https://github.com/nothings/stb) v1.22
[ff_wma](ff_wma.h)                              | WMAv1 & WMAv2 decoder with ASF container support.                  | [FFmpeg](https://ffmpeg.org/) wmadec
[ff_at3](ff_at3.h)                              | ATRAC3 & ATRAC3+ decoder with RIFF/OMA container support.          | [FFmpeg](https://ffmpeg.org/) via [PPSSPP](https://www.ppsspp.org/)
[ff_aac](ff_aac.h)                              | AAC-LC decoder with ADTS container support.                        | [FFmpeg](https://ffmpeg.org/) aacdec
[ff_adx](ff_adx.h)                              | CRI ADX (v1–v4) & AIX decoder with encryption support.             | [vgmstream](https://github.com/vgmstream/vgmstream)

## Usage

These are single-file libraries. To use one, define the implementation macro in **one** `.c` file before including:

```c
#define FF_WMA_IMPLEMENTATION
#include "ff_wma.h"
```

Each library provides both a **streaming API** and a **one-shot API**:

```c
/* One-shot: decode an entire file */
ff_wma_uint32 channels, sampleRate;
ff_wma_uint64 totalFrames;
float* samples = ff_wma_open_file_and_read_pcm_frames_f32(
    "file.wma", &channels, &sampleRate, &totalFrames, NULL);
/* ... */
ff_wma_free(samples, NULL);

/* Streaming: decode frame by frame */
ff_wma wma;
if (ff_wma_init_file("file.wma", NULL, &wma) == FF_WMA_SUCCESS) {
    float buf[4096];
    ff_wma_uint64 read;
    while ((read = ff_wma_read_pcm_frames_f32(&wma, 1024, buf)) > 0) {
        /* process samples */
    }
    ff_wma_uninit(&wma);
}
```

All libraries follow the same pattern — just swap the prefix (`ff_wma`, `ff_opus`, `ff_vorbis`, `ff_at3`, `ff_adx`).

## Options

Each library supports disabling file I/O APIs for use in environments without `stdio.h`:

```c
#define FF_WMA_NO_STDIO
#define FF_OPUS_IMPLEMENTATION
/* etc. */
```

## License

The decoder code in each library retains its original upstream license:

Library      | Decoder License                          | Wrapper License
------------ | ---------------------------------------- | ---------------
ff_opus      | BSD 3-Clause (libopus)                   | LGPL-2.1-or-later
ff_vorbis    | Public Domain (stb_vorbis)               | LGPL-2.1-or-later
ff_wma       | LGPL-2.1-or-later (FFmpeg)               | LGPL-2.1-or-later
ff_at3       | LGPL-2.1-or-later (FFmpeg / PPSSPP)      | LGPL-2.1-or-later
ff_aac       | LGPL-2.1-or-later (FFmpeg)               | LGPL-2.1-or-later
ff_adx       | LGPL-2.1-or-later (vgmstream)            | LGPL-2.1-or-later

The combined work for each library is distributed under the **LGPL v2.1 or later**. See the header of each file for full license text.

## Acknowledgements

- [FFmpeg](https://ffmpeg.org/) — WMA and ATRAC3/3+ decoder algorithms
- [PPSSPP](https://www.ppsspp.org/) — standalone ATRAC3/3+ extraction
- [libopus](https://opus-codec.org/) — Opus reference decoder
- [stb_vorbis](https://github.com/nothings/stb) by Sean Barrett — Vorbis decoder
- [vgmstream](https://github.com/vgmstream/vgmstream) — CRI ADX/AIX decoder reference
- [dr_libs](https://github.com/mackron/dr_libs) by David Reid — API design inspiration
