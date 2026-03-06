/*
 * ff_aac.h - AAC Decoder - Single Header Library
 * Part of ff_libs (https://github.com/user/ff_libs)
 *
 * Decoder ported from FFmpeg n4.4 (libavcodec/aacdec.c and dependencies).
 * API design inspired by dr_libs (https://github.com/mackron/dr_libs) but this
 * is a separate, independently maintained project.
 *
 * Copyright (c) 2005-2006 Oded Shimon (ods15 ods15 dyndns org)
 * Copyright (c) 2006-2007 Maxim Gavrilov (maxim.gavrilov gmail com)
 * Copyright (c) 2008-2013 Alex Converse <alex.converse@gmail.com>
 *
 * Decoder code: LGPL v2.1 or later (from FFmpeg)
 * Container parser & API wrapper: LGPL v2.1 or later
 */
#ifndef FF_AAC_H
#define FF_AAC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stddef.h>
typedef enum {
    FF_AAC_SUCCESS=0,FF_AAC_ERROR=-1,FF_AAC_ERROR_INVALID_ARGS=-2,
    FF_AAC_ERROR_OUT_OF_MEMORY=-3,FF_AAC_ERROR_OPEN_FAILED=-4,
    FF_AAC_ERROR_INVALID_FILE=-5,FF_AAC_AT_END=1
} ff_aac_result;
typedef struct {
    void*(*onMalloc)(size_t,void*); void*(*onRealloc)(void*,size_t,void*);
    void(*onFree)(void*,void*); void* pUserData;
} ff_aac_allocation_callbacks;
typedef struct ff_aac_config { int _reserved; } ff_aac_config;
typedef struct ff_aac__ctx ff_aac__ctx;
typedef struct { uint32_t channels; uint32_t sampleRate; uint64_t totalPCMFrameCount; ff_aac__ctx* pInternal; } ff_aac;
float*   ff_aac_open_file_and_read_pcm_frames_f32  (const char*,uint32_t*,uint32_t*,uint64_t*,const ff_aac_allocation_callbacks*);
int16_t* ff_aac_open_file_and_read_pcm_frames_s16  (const char*,uint32_t*,uint32_t*,uint64_t*,const ff_aac_allocation_callbacks*);
float*   ff_aac_open_memory_and_read_pcm_frames_f32(const void*,size_t,uint32_t*,uint32_t*,uint64_t*,const ff_aac_allocation_callbacks*);
int16_t* ff_aac_open_memory_and_read_pcm_frames_s16(const void*,size_t,uint32_t*,uint32_t*,uint64_t*,const ff_aac_allocation_callbacks*);
void     ff_aac_free(void*,const ff_aac_allocation_callbacks*);
ff_aac_result ff_aac_init_file  (const char*, const ff_aac_config*, ff_aac*);
ff_aac_result ff_aac_init_memory(const void*,size_t, const ff_aac_config*, ff_aac*);
uint64_t      ff_aac_read_pcm_frames_f32(ff_aac*,uint64_t,float*);
uint64_t      ff_aac_read_pcm_frames_s16(ff_aac*,uint64_t,int16_t*);
ff_aac_result ff_aac_seek_to_pcm_frame(ff_aac*,uint64_t);
void          ff_aac_uninit(ff_aac*);
#ifdef __cplusplus
}
#endif
#endif /* FF_AAC_H */
#ifdef FF_AAC_IMPLEMENTATION
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244 4100 4310 4389 4456 4324 4101 4701 4702 4132 4996 4554 4305 4127 4204 4334 4057 4267 4018 4013 4052)
#endif
#if defined(__GNUC__)||defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <sys/types.h>
#ifndef _WIN32
#include <pthread.h>
#else
/* pthread_once compat for Windows using interlocked ops; scalar-init compatible */
#include <windows.h>
typedef volatile LONG pthread_once_t;
#define PTHREAD_ONCE_INIT 0
static __inline int pthread_once(pthread_once_t* o, void(*f)(void)) {
    if(InterlockedCompareExchange(o,1,0)==0){f();InterlockedExchange(o,2);}
    else{while(*o!=2){Sleep(0);}} return 0; }
#endif
#include <errno.h>
#ifndef AV_NUM_DATA_POINTERS
#define AV_NUM_DATA_POINTERS 8
#endif
#ifndef FF_PROFILE_AAC_MAIN
#define FF_PROFILE_AAC_MAIN  0
#define FF_PROFILE_AAC_LOW   1
#define FF_PROFILE_AAC_SSR   2
#define FF_PROFILE_AAC_LTP   3
#define FF_PROFILE_AAC_HE    4
#define FF_PROFILE_UNKNOWN  -99
#endif
#ifndef AVERROR
#define AVERROR(e) (-(e))
#endif
#define FF_DISABLE_DEPRECATION_WARNINGS
#define FF_ENABLE_DEPRECATION_WARNINGS

#ifdef _MSC_VER
/* ---- MSVC compatibility: suppress / replace GCC-only constructs ---- */
#define __attribute__(x)
#define __extension__
#define __builtin_inff()       INFINITY
#define __builtin_offsetof(T,M) offsetof(T,M)
/* __builtin_add/sub_overflow: MSVC lacks these; implement via safe arithmetic */
#include <limits.h>
static __inline int ff_aac__add_overflow_i64(
        __int64 a, __int64 b, __int64* r) {
    *r = (__int64)((unsigned __int64)a + (unsigned __int64)b);
    return (b >= 0) ? (*r < a) : (*r > a);
}
static __inline int ff_aac__sub_overflow_i64(
        __int64 a, __int64 b, __int64* r) {
    *r = (__int64)((unsigned __int64)a - (unsigned __int64)b);
    return (b >= 0) ? (*r > a) : (*r < a);
}
#define __builtin_add_overflow(a,b,r) ff_aac__add_overflow_i64((a),(b),(r))
#define __builtin_sub_overflow(a,b,r) ff_aac__sub_overflow_i64((a),(b),(r))
/* also silence __PRETTY_FUNCTION__ in av_assert macros */
#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
#endif /* _MSC_VER */
/* Forward declarations for stub functions (prevents MSVC C4013 / C2371) */
void ff_dlog(void*, const char*, ...);
void avpriv_report_missing_feature(void*, const char*, ...);
void avpriv_request_sample(void*, const char*, ...);
void av_log(void*, int, const char*, ...);
const char* av_default_item_name(void*);
void* av_mallocz(size_t);
void* av_malloc(size_t);
void* av_malloc_array(size_t, size_t);
void  av_free(void*);
void  av_freep(void*);
void* av_realloc_f(void*, size_t, size_t);
int   av_log2(unsigned);
/* ff_get_buffer and av_frame_unref forward-declared after core types */
#ifndef _MSC_VER
typedef __builtin_va_list va_list;
#endif
typedef float INTFLOAT;
typedef float UINTFLOAT;
typedef float INT64FLOAT;
typedef float SHORTFLOAT;
typedef float AAC_FLOAT;
typedef unsigned AAC_SIGNE;

typedef struct AVFloatDSPContext {
    void (*vector_fmul)(float *dst, const float *src0, const float *src1,
                        int len);
    void (*vector_fmac_scalar)(float *dst, const float *src, float mul,
                               int len);
    void (*vector_dmac_scalar)(double *dst, const double *src, double mul,
                               int len);
    void (*vector_fmul_scalar)(float *dst, const float *src, float mul,
                               int len);
    void (*vector_dmul_scalar)(double *dst, const double *src, double mul,
                               int len);
    void (*vector_fmul_window)(float *dst, const float *src0,
                               const float *src1, const float *win, int len);
    void (*vector_fmul_add)(float *dst, const float *src0, const float *src1,
                            const float *src2, int len);
    void (*vector_fmul_reverse)(float *dst, const float *src0,
                                const float *src1, int len);
    void (*butterflies_float)(float *restrict v1, float *restrict v2, int len);
    float (*scalarproduct_float)(const float *v1, const float *v2, int len);
    void (*vector_dmul)(double *dst, const double *src0, const double *src1,
                        int len);
} AVFloatDSPContext;
float avpriv_scalarproduct_float_c(const float *v1, const float *v2, int len);

void ff_float_dsp_init_aarch64(AVFloatDSPContext *fdsp);
void ff_float_dsp_init_arm(AVFloatDSPContext *fdsp);
void ff_float_dsp_init_ppc(AVFloatDSPContext *fdsp, int strict);
void ff_float_dsp_init_x86(AVFloatDSPContext *fdsp);
void ff_float_dsp_init_mips(AVFloatDSPContext *fdsp);






AVFloatDSPContext *avpriv_float_dsp_alloc(int strict);



int av_strerror(int errnum, char *errbuf, size_t errbuf_size);
static inline char *av_make_error_string(char *errbuf, size_t errbuf_size, int errnum)
{
    av_strerror(errnum, errbuf, errbuf_size);
    return errbuf;
}
unsigned avutil_version(void);






const char *av_version_info(void);




const char *avutil_configuration(void);




const char *avutil_license(void);
enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,
    AVMEDIA_TYPE_NB
};





const char *av_get_media_type_string(enum AVMediaType media_type);
enum AVPictureType {
    AV_PICTURE_TYPE_NONE = 0,
    AV_PICTURE_TYPE_I,
    AV_PICTURE_TYPE_P,
    AV_PICTURE_TYPE_B,
    AV_PICTURE_TYPE_S,
    AV_PICTURE_TYPE_SI,
    AV_PICTURE_TYPE_SP,
    AV_PICTURE_TYPE_BI,
};
char av_get_picture_type_char(enum AVPictureType pict_type);









__attribute__((const)) int av_log2(unsigned v);



__attribute__((const)) int av_log2_16bit(unsigned v);
static __attribute__((always_inline)) inline __attribute__((const)) int av_clip_c(int a, int amin, int amax)
{



    if (a < amin) return amin;
    else if (a > amax) return amax;
    else return a;
}
static __attribute__((always_inline)) inline __attribute__((const)) int64_t av_clip64_c(int64_t a, int64_t amin, int64_t amax)
{



    if (a < amin) return amin;
    else if (a > amax) return amax;
    else return a;
}






static __attribute__((always_inline)) inline __attribute__((const)) uint8_t av_clip_uint8_c(int a)
{
    if (a&(~0xFF)) return (~a)>>31;
    else return a;
}






static __attribute__((always_inline)) inline __attribute__((const)) int8_t av_clip_int8_c(int a)
{
    if ((a+0x80U) & ~0xFF) return (a>>31) ^ 0x7F;
    else return a;
}






static __attribute__((always_inline)) inline __attribute__((const)) uint16_t av_clip_uint16_c(int a)
{
    if (a&(~0xFFFF)) return (~a)>>31;
    else return a;
}






static __attribute__((always_inline)) inline __attribute__((const)) int16_t av_clip_int16_c(int a)
{
    if ((a+0x8000U) & ~0xFFFF) return (a>>31) ^ 0x7FFF;
    else return a;
}






static __attribute__((always_inline)) inline __attribute__((const)) int32_t av_clipl_int32_c(int64_t a)
{
    if ((a+0x80000000u) & ~0xFFFFFFFFUL) return (int32_t)((a>>63) ^ 0x7FFFFFFF);
    else return (int32_t)a;
}







static __attribute__((always_inline)) inline __attribute__((const)) int av_clip_intp2_c(int a, int p)
{
    if (((unsigned)a + (1 << p)) & ~((2 << p) - 1))
        return (a >> 31) ^ ((1 << p) - 1);
    else
        return a;
}







static __attribute__((always_inline)) inline __attribute__((const)) unsigned av_clip_uintp2_c(int a, int p)
{
    if (a & ~((1<<p) - 1)) return (~a) >> 31 & ((1<<p) - 1);
    else return a;
}







static __attribute__((always_inline)) inline __attribute__((const)) unsigned av_mod_uintp2_c(unsigned a, unsigned p)
{
    return a & ((1U << p) - 1);
}
static __attribute__((always_inline)) inline int av_sat_add32_c(int a, int b)
{
    return av_clipl_int32_c((int64_t)a + b);
}
static __attribute__((always_inline)) inline int av_sat_dadd32_c(int a, int b)
{
    return av_sat_add32_c(a, av_sat_add32_c(b, b));
}
static __attribute__((always_inline)) inline int av_sat_sub32_c(int a, int b)
{
    return av_clipl_int32_c((int64_t)a - b);
}
static __attribute__((always_inline)) inline int av_sat_dsub32_c(int a, int b)
{
    return av_sat_sub32_c(a, av_sat_add32_c(b, b));
}
static __attribute__((always_inline)) inline int64_t av_sat_add64_c(int64_t a, int64_t b) {

    int64_t tmp;
    return !__builtin_add_overflow(a, b, &tmp) ? tmp : (tmp < 0 ? 
                                                                 (9223372036854775807L) 
                                                                           : 
                                                                             (-9223372036854775807L -1)
                                                                                      );






}
static __attribute__((always_inline)) inline int64_t av_sat_sub64_c(int64_t a, int64_t b) {

    int64_t tmp;
    return !__builtin_sub_overflow(a, b, &tmp) ? tmp : (tmp < 0 ? 
                                                                 (9223372036854775807L) 
                                                                           : 
                                                                             (-9223372036854775807L -1)
                                                                                      );







}
static __attribute__((always_inline)) inline __attribute__((const)) float av_clipf_c(float a, float amin, float amax)
{



    if (a < amin) return amin;
    else if (a > amax) return amax;
    else return a;
}
static __attribute__((always_inline)) inline __attribute__((const)) double av_clipd_c(double a, double amin, double amax)
{



    if (a < amin) return amin;
    else if (a > amax) return amax;
    else return a;
}





static __attribute__((always_inline)) inline __attribute__((const)) int av_ceil_log2_c(int x)
{
    return av_log2((x - 1U) << 1);
}






static __attribute__((always_inline)) inline __attribute__((const)) int av_popcount_c(uint32_t x)
{
    x -= (x >> 1) & 0x55555555;
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x += x >> 8;
    return (x + (x >> 16)) & 0x3F;
}






static __attribute__((always_inline)) inline __attribute__((const)) int av_popcount64_c(uint64_t x)
{
    return av_popcount_c((uint32_t)x) + av_popcount_c((uint32_t)(x >> 32));
}

static __attribute__((always_inline)) inline __attribute__((const)) int av_parity_c(uint32_t v)
{
    return av_popcount_c(v) & 1;
}

typedef struct AVRational{
    int num;
    int den;
} AVRational;
static inline AVRational av_make_q(int num, int den)
{
    AVRational r = { num, den };
    return r;
}
static inline int av_cmp_q(AVRational a, AVRational b){
    const int64_t tmp= a.num * (int64_t)b.den - b.num * (int64_t)a.den;

    if(tmp) return (int)((tmp ^ a.den ^ b.den)>>63)|1;
    else if(b.den && a.den) return 0;
    else if(a.num && b.num) return (a.num>>31) - (b.num>>31);
    else return 
                                  (-0x7fffffff - 1)
                                         ;
}







static inline double av_q2d(AVRational a){
    return a.num / (double) a.den;
}
int av_reduce(int *dst_num, int *dst_den, int64_t num, int64_t den, int64_t max);







AVRational av_mul_q(AVRational b, AVRational c) __attribute__((const));







AVRational av_div_q(AVRational b, AVRational c) __attribute__((const));







AVRational av_add_q(AVRational b, AVRational c) __attribute__((const));







AVRational av_sub_q(AVRational b, AVRational c) __attribute__((const));






static __attribute__((always_inline)) inline AVRational av_inv_q(AVRational q)
{
    AVRational r = { q.den, q.num };
    return r;
}
AVRational av_d2q(double d, int max) __attribute__((const));
int av_nearer_q(AVRational q, AVRational q1, AVRational q2);
int av_find_nearest_q_idx(AVRational q, const AVRational* q_list);
uint32_t av_q2intfloat(AVRational q);





AVRational av_gcd_q(AVRational a, AVRational b, int max_den, AVRational def);


union av_intfloat32 {
    uint32_t i;
    float f;
};

union av_intfloat64 {
    uint64_t i;
    double f;
};




static __attribute__((always_inline)) inline float av_int2float(uint32_t i)
{
    union av_intfloat32 v;
    v.i = i;
    return v.f;
}




static __attribute__((always_inline)) inline uint32_t av_float2int(float f)
{
    union av_intfloat32 v;
    v.f = f;
    return v.i;
}




static __attribute__((always_inline)) inline double av_int2double(uint64_t i)
{
    union av_intfloat64 v;
    v.i = i;
    return v.f;
}




static __attribute__((always_inline)) inline uint64_t av_double2int(double f)
{
    union av_intfloat64 v;
    v.f = f;
    return v.i;
}
enum AVRounding {
    AV_ROUND_ZERO = 0,
    AV_ROUND_INF = 1,
    AV_ROUND_DOWN = 2,
    AV_ROUND_UP = 3,
    AV_ROUND_NEAR_INF = 5,
    AV_ROUND_PASS_MINMAX = 8192,
};
int64_t __attribute__((const)) av_gcd(int64_t a, int64_t b);
int64_t av_rescale(int64_t a, int64_t b, int64_t c) __attribute__((const));
int64_t av_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd) __attribute__((const));
int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) __attribute__((const));
int64_t av_rescale_q_rnd(int64_t a, AVRational bq, AVRational cq,
                         enum AVRounding rnd) __attribute__((const));
int av_compare_ts(int64_t ts_a, AVRational tb_a, int64_t ts_b, AVRational tb_b);
int64_t av_compare_mod(uint64_t a, uint64_t b, uint64_t mod);
int64_t av_rescale_delta(AVRational in_tb, int64_t in_ts, AVRational fs_tb, int duration, int64_t *last, AVRational out_tb);
int64_t av_add_stable(AVRational ts_tb, int64_t ts, AVRational inc_tb, int64_t inc);



typedef enum {
    AV_CLASS_CATEGORY_NA = 0,
    AV_CLASS_CATEGORY_INPUT,
    AV_CLASS_CATEGORY_OUTPUT,
    AV_CLASS_CATEGORY_MUXER,
    AV_CLASS_CATEGORY_DEMUXER,
    AV_CLASS_CATEGORY_ENCODER,
    AV_CLASS_CATEGORY_DECODER,
    AV_CLASS_CATEGORY_FILTER,
    AV_CLASS_CATEGORY_BITSTREAM_FILTER,
    AV_CLASS_CATEGORY_SWSCALER,
    AV_CLASS_CATEGORY_SWRESAMPLER,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT = 40,
    AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_AUDIO_INPUT,
    AV_CLASS_CATEGORY_DEVICE_OUTPUT,
    AV_CLASS_CATEGORY_DEVICE_INPUT,
    AV_CLASS_CATEGORY_NB
}AVClassCategory;
struct AVOptionRanges;






typedef struct AVClass {




    const char* class_name;





    const char* (*item_name)(void* ctx);






    const struct AVOption *option;







    int version;





    int log_level_offset_offset;
    int parent_log_context_offset;




    void* (*child_next)(void *obj, void *prev);
    __attribute__((deprecated))
    const struct AVClass* (*child_class_next)(const struct AVClass *prev);







    AVClassCategory category;





    AVClassCategory (*get_category)(void* ctx);





    int (*query_ranges)(struct AVOptionRanges **, void *obj, const char *key, int flags);
    const struct AVClass* (*child_class_iterate)(void **iter);
} AVClass;
void av_log(void *avcl, int level, const char *fmt, ...) __attribute__((__format__(__printf__, 3, 4)));
void av_log_once(void* avcl, int initial_level, int subsequent_level, int *state, const char *fmt, ...) __attribute__((__format__(__printf__, 5, 6)));
void av_vlog(void *avcl, int level, const char *fmt, va_list vl);
int av_log_get_level(void);
void av_log_set_level(int level);
void av_log_set_callback(void (*callback)(void*, int, const char*, va_list));
void av_log_default_callback(void *avcl, int level, const char *fmt,
                             va_list vl);
const char* av_default_item_name(void* ctx);
AVClassCategory av_default_get_category(void *ptr);
void av_log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);
int av_log_format_line2(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix);
void av_log_set_flags(int arg);
int av_log_get_flags(void);
enum AVPixelFormat {
    AV_PIX_FMT_NONE = -1,
    AV_PIX_FMT_YUV420P,
    AV_PIX_FMT_YUYV422,
    AV_PIX_FMT_RGB24,
    AV_PIX_FMT_BGR24,
    AV_PIX_FMT_YUV422P,
    AV_PIX_FMT_YUV444P,
    AV_PIX_FMT_YUV410P,
    AV_PIX_FMT_YUV411P,
    AV_PIX_FMT_GRAY8,
    AV_PIX_FMT_MONOWHITE,
    AV_PIX_FMT_MONOBLACK,
    AV_PIX_FMT_PAL8,
    AV_PIX_FMT_YUVJ420P,
    AV_PIX_FMT_YUVJ422P,
    AV_PIX_FMT_YUVJ444P,
    AV_PIX_FMT_UYVY422,
    AV_PIX_FMT_UYYVYY411,
    AV_PIX_FMT_BGR8,
    AV_PIX_FMT_BGR4,
    AV_PIX_FMT_BGR4_BYTE,
    AV_PIX_FMT_RGB8,
    AV_PIX_FMT_RGB4,
    AV_PIX_FMT_RGB4_BYTE,
    AV_PIX_FMT_NV12,
    AV_PIX_FMT_NV21,

    AV_PIX_FMT_ARGB,
    AV_PIX_FMT_RGBA,
    AV_PIX_FMT_ABGR,
    AV_PIX_FMT_BGRA,

    AV_PIX_FMT_GRAY16BE,
    AV_PIX_FMT_GRAY16LE,
    AV_PIX_FMT_YUV440P,
    AV_PIX_FMT_YUVJ440P,
    AV_PIX_FMT_YUVA420P,
    AV_PIX_FMT_RGB48BE,
    AV_PIX_FMT_RGB48LE,

    AV_PIX_FMT_RGB565BE,
    AV_PIX_FMT_RGB565LE,
    AV_PIX_FMT_RGB555BE,
    AV_PIX_FMT_RGB555LE,

    AV_PIX_FMT_BGR565BE,
    AV_PIX_FMT_BGR565LE,
    AV_PIX_FMT_BGR555BE,
    AV_PIX_FMT_BGR555LE,




    AV_PIX_FMT_VAAPI_MOCO,
    AV_PIX_FMT_VAAPI_IDCT,
    AV_PIX_FMT_VAAPI_VLD,

    AV_PIX_FMT_VAAPI = AV_PIX_FMT_VAAPI_VLD,
    AV_PIX_FMT_YUV420P16LE,
    AV_PIX_FMT_YUV420P16BE,
    AV_PIX_FMT_YUV422P16LE,
    AV_PIX_FMT_YUV422P16BE,
    AV_PIX_FMT_YUV444P16LE,
    AV_PIX_FMT_YUV444P16BE,
    AV_PIX_FMT_DXVA2_VLD,

    AV_PIX_FMT_RGB444LE,
    AV_PIX_FMT_RGB444BE,
    AV_PIX_FMT_BGR444LE,
    AV_PIX_FMT_BGR444BE,
    AV_PIX_FMT_YA8,

    AV_PIX_FMT_Y400A = AV_PIX_FMT_YA8,
    AV_PIX_FMT_GRAY8A= AV_PIX_FMT_YA8,

    AV_PIX_FMT_BGR48BE,
    AV_PIX_FMT_BGR48LE,






    AV_PIX_FMT_YUV420P9BE,
    AV_PIX_FMT_YUV420P9LE,
    AV_PIX_FMT_YUV420P10BE,
    AV_PIX_FMT_YUV420P10LE,
    AV_PIX_FMT_YUV422P10BE,
    AV_PIX_FMT_YUV422P10LE,
    AV_PIX_FMT_YUV444P9BE,
    AV_PIX_FMT_YUV444P9LE,
    AV_PIX_FMT_YUV444P10BE,
    AV_PIX_FMT_YUV444P10LE,
    AV_PIX_FMT_YUV422P9BE,
    AV_PIX_FMT_YUV422P9LE,
    AV_PIX_FMT_GBRP,
    AV_PIX_FMT_GBR24P = AV_PIX_FMT_GBRP,
    AV_PIX_FMT_GBRP9BE,
    AV_PIX_FMT_GBRP9LE,
    AV_PIX_FMT_GBRP10BE,
    AV_PIX_FMT_GBRP10LE,
    AV_PIX_FMT_GBRP16BE,
    AV_PIX_FMT_GBRP16LE,
    AV_PIX_FMT_YUVA422P,
    AV_PIX_FMT_YUVA444P,
    AV_PIX_FMT_YUVA420P9BE,
    AV_PIX_FMT_YUVA420P9LE,
    AV_PIX_FMT_YUVA422P9BE,
    AV_PIX_FMT_YUVA422P9LE,
    AV_PIX_FMT_YUVA444P9BE,
    AV_PIX_FMT_YUVA444P9LE,
    AV_PIX_FMT_YUVA420P10BE,
    AV_PIX_FMT_YUVA420P10LE,
    AV_PIX_FMT_YUVA422P10BE,
    AV_PIX_FMT_YUVA422P10LE,
    AV_PIX_FMT_YUVA444P10BE,
    AV_PIX_FMT_YUVA444P10LE,
    AV_PIX_FMT_YUVA420P16BE,
    AV_PIX_FMT_YUVA420P16LE,
    AV_PIX_FMT_YUVA422P16BE,
    AV_PIX_FMT_YUVA422P16LE,
    AV_PIX_FMT_YUVA444P16BE,
    AV_PIX_FMT_YUVA444P16LE,

    AV_PIX_FMT_VDPAU,

    AV_PIX_FMT_XYZ12LE,
    AV_PIX_FMT_XYZ12BE,
    AV_PIX_FMT_NV16,
    AV_PIX_FMT_NV20LE,
    AV_PIX_FMT_NV20BE,

    AV_PIX_FMT_RGBA64BE,
    AV_PIX_FMT_RGBA64LE,
    AV_PIX_FMT_BGRA64BE,
    AV_PIX_FMT_BGRA64LE,

    AV_PIX_FMT_YVYU422,

    AV_PIX_FMT_YA16BE,
    AV_PIX_FMT_YA16LE,

    AV_PIX_FMT_GBRAP,
    AV_PIX_FMT_GBRAP16BE,
    AV_PIX_FMT_GBRAP16LE,




    AV_PIX_FMT_QSV,




    AV_PIX_FMT_MMAL,

    AV_PIX_FMT_D3D11VA_VLD,





    AV_PIX_FMT_CUDA,

    AV_PIX_FMT_0RGB,
    AV_PIX_FMT_RGB0,
    AV_PIX_FMT_0BGR,
    AV_PIX_FMT_BGR0,

    AV_PIX_FMT_YUV420P12BE,
    AV_PIX_FMT_YUV420P12LE,
    AV_PIX_FMT_YUV420P14BE,
    AV_PIX_FMT_YUV420P14LE,
    AV_PIX_FMT_YUV422P12BE,
    AV_PIX_FMT_YUV422P12LE,
    AV_PIX_FMT_YUV422P14BE,
    AV_PIX_FMT_YUV422P14LE,
    AV_PIX_FMT_YUV444P12BE,
    AV_PIX_FMT_YUV444P12LE,
    AV_PIX_FMT_YUV444P14BE,
    AV_PIX_FMT_YUV444P14LE,
    AV_PIX_FMT_GBRP12BE,
    AV_PIX_FMT_GBRP12LE,
    AV_PIX_FMT_GBRP14BE,
    AV_PIX_FMT_GBRP14LE,
    AV_PIX_FMT_YUVJ411P,

    AV_PIX_FMT_BAYER_BGGR8,
    AV_PIX_FMT_BAYER_RGGB8,
    AV_PIX_FMT_BAYER_GBRG8,
    AV_PIX_FMT_BAYER_GRBG8,
    AV_PIX_FMT_BAYER_BGGR16LE,
    AV_PIX_FMT_BAYER_BGGR16BE,
    AV_PIX_FMT_BAYER_RGGB16LE,
    AV_PIX_FMT_BAYER_RGGB16BE,
    AV_PIX_FMT_BAYER_GBRG16LE,
    AV_PIX_FMT_BAYER_GBRG16BE,
    AV_PIX_FMT_BAYER_GRBG16LE,
    AV_PIX_FMT_BAYER_GRBG16BE,

    AV_PIX_FMT_XVMC,

    AV_PIX_FMT_YUV440P10LE,
    AV_PIX_FMT_YUV440P10BE,
    AV_PIX_FMT_YUV440P12LE,
    AV_PIX_FMT_YUV440P12BE,
    AV_PIX_FMT_AYUV64LE,
    AV_PIX_FMT_AYUV64BE,

    AV_PIX_FMT_VIDEOTOOLBOX,

    AV_PIX_FMT_P010LE,
    AV_PIX_FMT_P010BE,

    AV_PIX_FMT_GBRAP12BE,
    AV_PIX_FMT_GBRAP12LE,

    AV_PIX_FMT_GBRAP10BE,
    AV_PIX_FMT_GBRAP10LE,

    AV_PIX_FMT_MEDIACODEC,

    AV_PIX_FMT_GRAY12BE,
    AV_PIX_FMT_GRAY12LE,
    AV_PIX_FMT_GRAY10BE,
    AV_PIX_FMT_GRAY10LE,

    AV_PIX_FMT_P016LE,
    AV_PIX_FMT_P016BE,
    AV_PIX_FMT_D3D11,

    AV_PIX_FMT_GRAY9BE,
    AV_PIX_FMT_GRAY9LE,

    AV_PIX_FMT_GBRPF32BE,
    AV_PIX_FMT_GBRPF32LE,
    AV_PIX_FMT_GBRAPF32BE,
    AV_PIX_FMT_GBRAPF32LE,






    AV_PIX_FMT_DRM_PRIME,






    AV_PIX_FMT_OPENCL,

    AV_PIX_FMT_GRAY14BE,
    AV_PIX_FMT_GRAY14LE,

    AV_PIX_FMT_GRAYF32BE,
    AV_PIX_FMT_GRAYF32LE,

    AV_PIX_FMT_YUVA422P12BE,
    AV_PIX_FMT_YUVA422P12LE,
    AV_PIX_FMT_YUVA444P12BE,
    AV_PIX_FMT_YUVA444P12LE,

    AV_PIX_FMT_NV24,
    AV_PIX_FMT_NV42,






    AV_PIX_FMT_VULKAN,

    AV_PIX_FMT_Y210BE,
    AV_PIX_FMT_Y210LE,

    AV_PIX_FMT_X2RGB10LE,
    AV_PIX_FMT_X2RGB10BE,
    AV_PIX_FMT_NB
};
enum AVColorPrimaries {
    AVCOL_PRI_RESERVED0 = 0,
    AVCOL_PRI_BT709 = 1,
    AVCOL_PRI_UNSPECIFIED = 2,
    AVCOL_PRI_RESERVED = 3,
    AVCOL_PRI_BT470M = 4,

    AVCOL_PRI_BT470BG = 5,
    AVCOL_PRI_SMPTE170M = 6,
    AVCOL_PRI_SMPTE240M = 7,
    AVCOL_PRI_FILM = 8,
    AVCOL_PRI_BT2020 = 9,
    AVCOL_PRI_SMPTE428 = 10,
    AVCOL_PRI_SMPTEST428_1 = AVCOL_PRI_SMPTE428,
    AVCOL_PRI_SMPTE431 = 11,
    AVCOL_PRI_SMPTE432 = 12,
    AVCOL_PRI_EBU3213 = 22,
    AVCOL_PRI_JEDEC_P22 = AVCOL_PRI_EBU3213,
    AVCOL_PRI_NB
};





enum AVColorTransferCharacteristic {
    AVCOL_TRC_RESERVED0 = 0,
    AVCOL_TRC_BT709 = 1,
    AVCOL_TRC_UNSPECIFIED = 2,
    AVCOL_TRC_RESERVED = 3,
    AVCOL_TRC_GAMMA22 = 4,
    AVCOL_TRC_GAMMA28 = 5,
    AVCOL_TRC_SMPTE170M = 6,
    AVCOL_TRC_SMPTE240M = 7,
    AVCOL_TRC_LINEAR = 8,
    AVCOL_TRC_LOG = 9,
    AVCOL_TRC_LOG_SQRT = 10,
    AVCOL_TRC_IEC61966_2_4 = 11,
    AVCOL_TRC_BT1361_ECG = 12,
    AVCOL_TRC_IEC61966_2_1 = 13,
    AVCOL_TRC_BT2020_10 = 14,
    AVCOL_TRC_BT2020_12 = 15,
    AVCOL_TRC_SMPTE2084 = 16,
    AVCOL_TRC_SMPTEST2084 = AVCOL_TRC_SMPTE2084,
    AVCOL_TRC_SMPTE428 = 17,
    AVCOL_TRC_SMPTEST428_1 = AVCOL_TRC_SMPTE428,
    AVCOL_TRC_ARIB_STD_B67 = 18,
    AVCOL_TRC_NB
};





enum AVColorSpace {
    AVCOL_SPC_RGB = 0,
    AVCOL_SPC_BT709 = 1,
    AVCOL_SPC_UNSPECIFIED = 2,
    AVCOL_SPC_RESERVED = 3,
    AVCOL_SPC_FCC = 4,
    AVCOL_SPC_BT470BG = 5,
    AVCOL_SPC_SMPTE170M = 6,
    AVCOL_SPC_SMPTE240M = 7,
    AVCOL_SPC_YCGCO = 8,
    AVCOL_SPC_YCOCG = AVCOL_SPC_YCGCO,
    AVCOL_SPC_BT2020_NCL = 9,
    AVCOL_SPC_BT2020_CL = 10,
    AVCOL_SPC_SMPTE2085 = 11,
    AVCOL_SPC_CHROMA_DERIVED_NCL = 12,
    AVCOL_SPC_CHROMA_DERIVED_CL = 13,
    AVCOL_SPC_ICTCP = 14,
    AVCOL_SPC_NB
};
enum AVColorRange {
    AVCOL_RANGE_UNSPECIFIED = 0,
    AVCOL_RANGE_MPEG = 1,
    AVCOL_RANGE_JPEG = 2,
    AVCOL_RANGE_NB
};
enum AVChromaLocation {
    AVCHROMA_LOC_UNSPECIFIED = 0,
    AVCHROMA_LOC_LEFT = 1,
    AVCHROMA_LOC_CENTER = 2,
    AVCHROMA_LOC_TOPLEFT = 3,
    AVCHROMA_LOC_TOP = 4,
    AVCHROMA_LOC_BOTTOMLEFT = 5,
    AVCHROMA_LOC_BOTTOM = 6,
    AVCHROMA_LOC_NB
};




static inline void *av_x_if_null(const void *p, const void *x)
{
    return (void *)(intptr_t)(p ? p : x);
}
unsigned av_int_list_length_for_size(unsigned elsize,
                                     const void *list, uint64_t term) __attribute__((pure));
FILE *av_fopen_utf8(const char *path, const char *mode);




AVRational av_get_time_base_q(void);
char *av_fourcc_make_string(char *buf, uint32_t fourcc);
void *av_malloc(size_t size) __attribute__((__malloc__)) __attribute__((alloc_size(1)));
void *av_mallocz(size_t size) __attribute__((__malloc__)) __attribute__((alloc_size(1)));
__attribute__((alloc_size(1, 2))) void *av_malloc_array(size_t nmemb, size_t size);
__attribute__((alloc_size(1, 2))) void *av_mallocz_array(size_t nmemb, size_t size);






void *av_calloc(size_t nmemb, size_t size) __attribute__((__malloc__));
void *av_realloc(void *ptr, size_t size) __attribute__((alloc_size(2)));
__attribute__((warn_unused_result))
int av_reallocp(void *ptr, size_t size);
void *av_realloc_f(void *ptr, size_t nelem, size_t elsize);
__attribute__((alloc_size(2, 3))) void *av_realloc_array(void *ptr, size_t nmemb, size_t size);
int av_reallocp_array(void *ptr, size_t nmemb, size_t size);
void *av_fast_realloc(void *ptr, unsigned int *size, size_t min_size);
void av_fast_malloc(void *ptr, unsigned int *size, size_t min_size);
void av_fast_mallocz(void *ptr, unsigned int *size, size_t min_size);
void av_free(void *ptr);
void av_freep(void *ptr);
char *av_strdup(const char *s) __attribute__((__malloc__));
char *av_strndup(const char *s, size_t len) __attribute__((__malloc__));
void *av_memdup(const void *p, size_t size);
void av_memcpy_backptr(uint8_t *dst, int back, int cnt);
void av_dynarray_add(void *tab_ptr, int *nb_ptr, void *elem);
__attribute__((warn_unused_result))
int av_dynarray_add_nofree(void *tab_ptr, int *nb_ptr, void *elem);
void *av_dynarray2_add(void **tab_ptr, int *nb_ptr, size_t elem_size,
                       const uint8_t *elem_data);
static inline int av_size_mult(size_t a, size_t b, size_t *r)
{
    size_t t = a * b;


    if ((a | b) >= ((size_t)1 << (sizeof(size_t) * 4)) && a && t / a != b)
        return (-(
              22
              ));
    *r = t;
    return 0;
}
void av_max_alloc(size_t max);

static void vector_fmul_c(float *dst, const float *src0, const float *src1,
                          int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src0[i] * src1[i];
}

static void vector_dmul_c(double *dst, const double *src0, const double *src1,
                          int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src0[i] * src1[i];
}

static void vector_fmac_scalar_c(float *dst, const float *src, float mul,
                                 int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] += src[i] * mul;
}

static void vector_dmac_scalar_c(double *dst, const double *src, double mul,
                                 int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] += src[i] * mul;
}

static void vector_fmul_scalar_c(float *dst, const float *src, float mul,
                                 int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src[i] * mul;
}

static void vector_dmul_scalar_c(double *dst, const double *src, double mul,
                                 int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[i] = src[i] * mul;
}

static void vector_fmul_window_c(float *dst, const float *src0,
                                 const float *src1, const float *win, int len)
{
    int i, j;

    dst += len;
    win += len;
    src0 += len;

    for (i = -len, j = len - 1; i < 0; i++, j--) {
        float s0 = src0[i];
        float s1 = src1[j];
        float wi = win[i];
        float wj = win[j];
        dst[i] = s0 * wj - s1 * wi;
        dst[j] = s0 * wi + s1 * wj;
    }
}

static void vector_fmul_add_c(float *dst, const float *src0, const float *src1,
                              const float *src2, int len){
    int i;

    for (i = 0; i < len; i++)
        dst[i] = src0[i] * src1[i] + src2[i];
}

static void vector_fmul_reverse_c(float *dst, const float *src0,
                                  const float *src1, int len)
{
    int i;

    src1 += len-1;
    for (i = 0; i < len; i++)
        dst[i] = src0[i] * src1[-i];
}

static void butterflies_float_c(float *restrict v1, float *restrict v2,
                                int len)
{
    int i;

    for (i = 0; i < len; i++) {
        float t = v1[i] - v2[i];
        v1[i] += v2[i];
        v2[i] = t;
    }
}

float avpriv_scalarproduct_float_c(const float *v1, const float *v2, int len)
{
    float p = 0.0;
    int i;

    for (i = 0; i < len; i++)
        p += v1[i] * v2[i];

    return p;
}

__attribute__((cold)) AVFloatDSPContext *avpriv_float_dsp_alloc(int bit_exact)
{
    AVFloatDSPContext *fdsp = av_mallocz(sizeof(AVFloatDSPContext));
    if (!fdsp)
        return 
              ((void *)0)
                  ;

    fdsp->vector_fmul = vector_fmul_c;
    fdsp->vector_dmul = vector_dmul_c;
    fdsp->vector_fmac_scalar = vector_fmac_scalar_c;
    fdsp->vector_fmul_scalar = vector_fmul_scalar_c;
    fdsp->vector_dmac_scalar = vector_dmac_scalar_c;
    fdsp->vector_dmul_scalar = vector_dmul_scalar_c;
    fdsp->vector_fmul_window = vector_fmul_window_c;
    fdsp->vector_fmul_add = vector_fmul_add_c;
    fdsp->vector_fmul_reverse = vector_fmul_reverse_c;
    fdsp->butterflies_float = butterflies_float_c;
    fdsp->scalarproduct_float = avpriv_scalarproduct_float_c;

    if (0)
        ff_float_dsp_init_aarch64(fdsp);
    if (0)
        ff_float_dsp_init_arm(fdsp);
    if (0)
        ff_float_dsp_init_ppc(fdsp, bit_exact);
    if (0)
        ff_float_dsp_init_x86(fdsp);
    if (0)
        ff_float_dsp_init_mips(fdsp);
    return fdsp;
}
const uint8_t ff_log2_tab[256]={
        0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
        7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};
void av_assert0_fpu(void);
enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,
    AV_SAMPLE_FMT_S16,
    AV_SAMPLE_FMT_S32,
    AV_SAMPLE_FMT_FLT,
    AV_SAMPLE_FMT_DBL,

    AV_SAMPLE_FMT_U8P,
    AV_SAMPLE_FMT_S16P,
    AV_SAMPLE_FMT_S32P,
    AV_SAMPLE_FMT_FLTP,
    AV_SAMPLE_FMT_DBLP,
    AV_SAMPLE_FMT_S64,
    AV_SAMPLE_FMT_S64P,

    AV_SAMPLE_FMT_NB
};





const char *av_get_sample_fmt_name(enum AVSampleFormat sample_fmt);





enum AVSampleFormat av_get_sample_fmt(const char *name);







enum AVSampleFormat av_get_alt_sample_fmt(enum AVSampleFormat sample_fmt, int planar);
enum AVSampleFormat av_get_packed_sample_fmt(enum AVSampleFormat sample_fmt);
enum AVSampleFormat av_get_planar_sample_fmt(enum AVSampleFormat sample_fmt);
char *av_get_sample_fmt_string(char *buf, int buf_size, enum AVSampleFormat sample_fmt);
int av_get_bytes_per_sample(enum AVSampleFormat sample_fmt);







int av_sample_fmt_is_planar(enum AVSampleFormat sample_fmt);
int av_samples_get_buffer_size(int *linesize, int nb_channels, int nb_samples,
                               enum AVSampleFormat sample_fmt, int align);
int av_samples_fill_arrays(uint8_t **audio_data, int *linesize,
                           const uint8_t *buf,
                           int nb_channels, int nb_samples,
                           enum AVSampleFormat sample_fmt, int align);
int av_samples_alloc(uint8_t **audio_data, int *linesize, int nb_channels,
                     int nb_samples, enum AVSampleFormat sample_fmt, int align);
int av_samples_alloc_array_and_samples(uint8_t ***audio_data, int *linesize, int nb_channels,
                                       int nb_samples, enum AVSampleFormat sample_fmt, int align);
int av_samples_copy(uint8_t **dst, uint8_t * const *src, int dst_offset,
                    int src_offset, int nb_samples, int nb_channels,
                    enum AVSampleFormat sample_fmt);
int av_samples_set_silence(uint8_t **audio_data, int offset, int nb_samples,
                           int nb_channels, enum AVSampleFormat sample_fmt);


typedef struct AVBuffer AVBuffer;







typedef struct AVBufferRef {
    AVBuffer *buffer;






    uint8_t *data;




    int size;



} AVBufferRef;







AVBufferRef *av_buffer_alloc(int size);
AVBufferRef *av_buffer_allocz(int size);
AVBufferRef *av_buffer_create(uint8_t *data, int size,



                              void (*free)(void *opaque, uint8_t *data),
                              void *opaque, int flags);






void av_buffer_default_free(void *opaque, uint8_t *data);







AVBufferRef *av_buffer_ref(AVBufferRef *buf);







void av_buffer_unref(AVBufferRef **buf);







int av_buffer_is_writable(const AVBufferRef *buf);




void *av_buffer_get_opaque(const AVBufferRef *buf);

int av_buffer_get_ref_count(const AVBufferRef *buf);
int av_buffer_make_writable(AVBufferRef **buf);
int av_buffer_realloc(AVBufferRef **buf, int size);
int av_buffer_replace(AVBufferRef **dst, AVBufferRef *src);
typedef struct AVBufferPool AVBufferPool;
AVBufferPool *av_buffer_pool_init(int size, AVBufferRef* (*alloc)(int size));
AVBufferPool *av_buffer_pool_init2(int size, void *opaque,
                                   AVBufferRef* (*alloc)(void *opaque, int size),




                                   void (*pool_free)(void *opaque));
void av_buffer_pool_uninit(AVBufferPool **pool);







AVBufferRef *av_buffer_pool_get(AVBufferPool *pool);
void *av_buffer_pool_buffer_get_opaque(AVBufferRef *ref);
int av_get_cpu_flags(void);





void av_force_cpu_flags(int flags);






__attribute__((deprecated)) void av_set_cpu_flags_mask(int mask);
__attribute__((deprecated))
int av_parse_cpu_flags(const char *s);






int av_parse_cpu_caps(unsigned *flags, const char *s);




int av_cpu_count(void);
size_t av_cpu_max_align(void);
enum AVMatrixEncoding {
    AV_MATRIX_ENCODING_NONE,
    AV_MATRIX_ENCODING_DOLBY,
    AV_MATRIX_ENCODING_DPLII,
    AV_MATRIX_ENCODING_DPLIIX,
    AV_MATRIX_ENCODING_DPLIIZ,
    AV_MATRIX_ENCODING_DOLBYEX,
    AV_MATRIX_ENCODING_DOLBYHEADPHONE,
    AV_MATRIX_ENCODING_NB
};
uint64_t av_get_channel_layout(const char *name);
int av_get_extended_channel_layout(const char *name, uint64_t* channel_layout, int* nb_channels);
void av_get_channel_layout_string(char *buf, int buf_size, int nb_channels, uint64_t channel_layout);

struct AVBPrint;



void av_bprint_channel_layout(struct AVBPrint *bp, int nb_channels, uint64_t channel_layout);




int av_get_channel_layout_nb_channels(uint64_t channel_layout);




int64_t av_get_default_channel_layout(int nb_channels);
int av_get_channel_layout_channel_index(uint64_t channel_layout,
                                        uint64_t channel);




uint64_t av_channel_layout_extract_channel(uint64_t channel_layout, int index);






const char *av_get_channel_name(uint64_t channel);







const char *av_get_channel_description(uint64_t channel);
int av_get_standard_channel_layout(unsigned index, uint64_t *layout,
                                   const char **name);
typedef struct AVDictionaryEntry {
    char *key;
    char *value;
} AVDictionaryEntry;

typedef struct AVDictionary AVDictionary;
AVDictionaryEntry *av_dict_get(const AVDictionary *m, const char *key,
                               const AVDictionaryEntry *prev, int flags);







int av_dict_count(const AVDictionary *m);
int av_dict_set(AVDictionary **pm, const char *key, const char *value, int flags);







int av_dict_set_int(AVDictionary **pm, const char *key, int64_t value, int flags);
int av_dict_parse_string(AVDictionary **pm, const char *str,
                         const char *key_val_sep, const char *pairs_sep,
                         int flags);
int av_dict_copy(AVDictionary **dst, const AVDictionary *src, int flags);





void av_dict_free(AVDictionary **m);
int av_dict_get_string(const AVDictionary *m, char **buffer,
                       const char key_val_sep, const char pairs_sep);



enum AVFrameSideDataType {



    AV_FRAME_DATA_PANSCAN,





    AV_FRAME_DATA_A53_CC,




    AV_FRAME_DATA_STEREO3D,



    AV_FRAME_DATA_MATRIXENCODING,




    AV_FRAME_DATA_DOWNMIX_INFO,



    AV_FRAME_DATA_REPLAYGAIN,







    AV_FRAME_DATA_DISPLAYMATRIX,




    AV_FRAME_DATA_AFD,






    AV_FRAME_DATA_MOTION_VECTORS,
    AV_FRAME_DATA_SKIP_SAMPLES,




    AV_FRAME_DATA_AUDIO_SERVICE_TYPE,





    AV_FRAME_DATA_MASTERING_DISPLAY_METADATA,




    AV_FRAME_DATA_GOP_TIMECODE,





    AV_FRAME_DATA_SPHERICAL,





    AV_FRAME_DATA_CONTENT_LIGHT_LEVEL,






    AV_FRAME_DATA_ICC_PROFILE,
    AV_FRAME_DATA_QP_TABLE_PROPERTIES,






    AV_FRAME_DATA_QP_TABLE_DATA,
    AV_FRAME_DATA_S12M_TIMECODE,






    AV_FRAME_DATA_DYNAMIC_HDR_PLUS,





    AV_FRAME_DATA_REGIONS_OF_INTEREST,




    AV_FRAME_DATA_VIDEO_ENC_PARAMS,







    AV_FRAME_DATA_SEI_UNREGISTERED,





    AV_FRAME_DATA_FILM_GRAIN_PARAMS,
};

enum AVActiveFormatDescription {
    AV_AFD_SAME = 8,
    AV_AFD_4_3 = 9,
    AV_AFD_16_9 = 10,
    AV_AFD_14_9 = 11,
    AV_AFD_4_3_SP_14_9 = 13,
    AV_AFD_16_9_SP_14_9 = 14,
    AV_AFD_SP_4_3 = 15,
};
typedef struct AVFrameSideData {
    enum AVFrameSideDataType type;
    uint8_t *data;

    int size;



    AVDictionary *metadata;
    AVBufferRef *buf;
} AVFrameSideData;
typedef struct AVRegionOfInterest {




    uint32_t self_size;
    int top;
    int bottom;
    int left;
    int right;
    AVRational qoffset;
} AVRegionOfInterest;
typedef struct AVFrame {
    uint8_t *data[8];
    int linesize[8];
    uint8_t **extended_data;
    int width, height;







    int nb_samples;






    int format;




    int key_frame;




    enum AVPictureType pict_type;




    AVRational sample_aspect_ratio;




    int64_t pts;






    __attribute__((deprecated))
    int64_t pkt_pts;







    int64_t pkt_dts;




    int coded_picture_number;



    int display_picture_number;




    int quality;




    void *opaque;





    __attribute__((deprecated))
    uint64_t error[8];






    int repeat_pict;




    int interlaced_frame;




    int top_field_first;




    int palette_has_changed;
    int64_t reordered_opaque;




    int sample_rate;




    uint64_t channel_layout;
    AVBufferRef *buf[8];
    AVBufferRef **extended_buf;



    int nb_extended_buf;

    AVFrameSideData **side_data;
    int nb_side_data;
    int flags;






    enum AVColorRange color_range;

    enum AVColorPrimaries color_primaries;

    enum AVColorTransferCharacteristic color_trc;






    enum AVColorSpace colorspace;

    enum AVChromaLocation chroma_location;






    int64_t best_effort_timestamp;






    int64_t pkt_pos;







    int64_t pkt_duration;






    AVDictionary *metadata;
    int decode_error_flags;
    int channels;
    int pkt_size;





    __attribute__((deprecated))
    int8_t *qscale_table;



    __attribute__((deprecated))
    int qstride;

    __attribute__((deprecated))
    int qscale_type;

    __attribute__((deprecated))
    AVBufferRef *qp_table_buf;





    AVBufferRef *hw_frames_ctx;
    AVBufferRef *opaque_ref;
    size_t crop_top;
    size_t crop_bottom;
    size_t crop_left;
    size_t crop_right;
    AVBufferRef *private_ref;
} AVFrame;






__attribute__((deprecated))
int64_t av_frame_get_best_effort_timestamp(const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_best_effort_timestamp(AVFrame *frame, int64_t val);
__attribute__((deprecated))
int64_t av_frame_get_pkt_duration (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_pkt_duration (AVFrame *frame, int64_t val);
__attribute__((deprecated))
int64_t av_frame_get_pkt_pos (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_pkt_pos (AVFrame *frame, int64_t val);
__attribute__((deprecated))
int64_t av_frame_get_channel_layout (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_channel_layout (AVFrame *frame, int64_t val);
__attribute__((deprecated))
int av_frame_get_channels (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_channels (AVFrame *frame, int val);
__attribute__((deprecated))
int av_frame_get_sample_rate (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_sample_rate (AVFrame *frame, int val);
__attribute__((deprecated))
AVDictionary *av_frame_get_metadata (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_metadata (AVFrame *frame, AVDictionary *val);
__attribute__((deprecated))
int av_frame_get_decode_error_flags (const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_decode_error_flags (AVFrame *frame, int val);
__attribute__((deprecated))
int av_frame_get_pkt_size(const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_pkt_size(AVFrame *frame, int val);

__attribute__((deprecated))
int8_t *av_frame_get_qp_table(AVFrame *f, int *stride, int *type);
__attribute__((deprecated))
int av_frame_set_qp_table(AVFrame *f, AVBufferRef *buf, int stride, int type);

__attribute__((deprecated))
enum AVColorSpace av_frame_get_colorspace(const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_colorspace(AVFrame *frame, enum AVColorSpace val);
__attribute__((deprecated))
enum AVColorRange av_frame_get_color_range(const AVFrame *frame);
__attribute__((deprecated))
void av_frame_set_color_range(AVFrame *frame, enum AVColorRange val);






const char *av_get_colorspace_name(enum AVColorSpace val);
AVFrame *av_frame_alloc(void);
void av_frame_free(AVFrame **frame);
int av_frame_ref(AVFrame *dst, const AVFrame *src);
AVFrame *av_frame_clone(const AVFrame *src);




void av_frame_unref(AVFrame *frame);
void av_frame_move_ref(AVFrame *dst, AVFrame *src);
int av_frame_get_buffer(AVFrame *frame, int align);
int av_frame_is_writable(AVFrame *frame);
int av_frame_make_writable(AVFrame *frame);
int av_frame_copy(AVFrame *dst, const AVFrame *src);
int av_frame_copy_props(AVFrame *dst, const AVFrame *src);
AVBufferRef *av_frame_get_plane_buffer(AVFrame *frame, int plane);
AVFrameSideData *av_frame_new_side_data(AVFrame *frame,
                                        enum AVFrameSideDataType type,

                                        int size);
AVFrameSideData *av_frame_new_side_data_from_buf(AVFrame *frame,
                                                 enum AVFrameSideDataType type,
                                                 AVBufferRef *buf);





AVFrameSideData *av_frame_get_side_data(const AVFrame *frame,
                                        enum AVFrameSideDataType type);




void av_frame_remove_side_data(AVFrame *frame, enum AVFrameSideDataType type);





enum {
    AV_FRAME_CROP_UNALIGNED = 1 << 0,
};
int av_frame_apply_cropping(AVFrame *frame, int flags);




const char *av_frame_side_data_name(enum AVFrameSideDataType type);



enum AVHWDeviceType {
    AV_HWDEVICE_TYPE_NONE,
    AV_HWDEVICE_TYPE_VDPAU,
    AV_HWDEVICE_TYPE_CUDA,
    AV_HWDEVICE_TYPE_VAAPI,
    AV_HWDEVICE_TYPE_DXVA2,
    AV_HWDEVICE_TYPE_QSV,
    AV_HWDEVICE_TYPE_VIDEOTOOLBOX,
    AV_HWDEVICE_TYPE_D3D11VA,
    AV_HWDEVICE_TYPE_DRM,
    AV_HWDEVICE_TYPE_OPENCL,
    AV_HWDEVICE_TYPE_MEDIACODEC,
    AV_HWDEVICE_TYPE_VULKAN,
};

typedef struct AVHWDeviceInternal AVHWDeviceInternal;
typedef struct AVHWDeviceContext {



    const AVClass *av_class;





    AVHWDeviceInternal *internal;







    enum AVHWDeviceType type;
    void *hwctx;
    void (*free)(struct AVHWDeviceContext *ctx);




    void *user_opaque;
} AVHWDeviceContext;

typedef struct AVHWFramesInternal AVHWFramesInternal;
typedef struct AVHWFramesContext {



    const AVClass *av_class;





    AVHWFramesInternal *internal;






    AVBufferRef *device_ref;







    AVHWDeviceContext *device_ctx;
    void *hwctx;







    void (*free)(struct AVHWFramesContext *ctx);




    void *user_opaque;
    AVBufferPool *pool;
    int initial_pool_size;
    enum AVPixelFormat format;
    enum AVPixelFormat sw_format;






    int width, height;
} AVHWFramesContext;
enum AVHWDeviceType av_hwdevice_find_type_by_name(const char *name);







const char *av_hwdevice_get_type_name(enum AVHWDeviceType type);
enum AVHWDeviceType av_hwdevice_iterate_types(enum AVHWDeviceType prev);
AVBufferRef *av_hwdevice_ctx_alloc(enum AVHWDeviceType type);
int av_hwdevice_ctx_init(AVBufferRef *ref);
int av_hwdevice_ctx_create(AVBufferRef **device_ctx, enum AVHWDeviceType type,
                           const char *device, AVDictionary *opts, int flags);
int av_hwdevice_ctx_create_derived(AVBufferRef **dst_ctx,
                                   enum AVHWDeviceType type,
                                   AVBufferRef *src_ctx, int flags);
int av_hwdevice_ctx_create_derived_opts(AVBufferRef **dst_ctx,
                                        enum AVHWDeviceType type,
                                        AVBufferRef *src_ctx,
                                        AVDictionary *options, int flags);
AVBufferRef *av_hwframe_ctx_alloc(AVBufferRef *device_ctx);
int av_hwframe_ctx_init(AVBufferRef *ref);
int av_hwframe_get_buffer(AVBufferRef *hwframe_ctx, AVFrame *frame, int flags);
int av_hwframe_transfer_data(AVFrame *dst, const AVFrame *src, int flags);

enum AVHWFrameTransferDirection {



    AV_HWFRAME_TRANSFER_DIRECTION_FROM,




    AV_HWFRAME_TRANSFER_DIRECTION_TO,
};
int av_hwframe_transfer_get_formats(AVBufferRef *hwframe_ctx,
                                    enum AVHWFrameTransferDirection dir,
                                    enum AVPixelFormat **formats, int flags);
typedef struct AVHWFramesConstraints {




    enum AVPixelFormat *valid_hw_formats;






    enum AVPixelFormat *valid_sw_formats;





    int min_width;
    int min_height;





    int max_width;
    int max_height;
} AVHWFramesConstraints;
void *av_hwdevice_hwconfig_alloc(AVBufferRef *device_ctx);
AVHWFramesConstraints *av_hwdevice_get_hwframe_constraints(AVBufferRef *ref,
                                                           const void *hwconfig);






void av_hwframe_constraints_free(AVHWFramesConstraints **constraints);





enum {



    AV_HWFRAME_MAP_READ = 1 << 0,



    AV_HWFRAME_MAP_WRITE = 1 << 1,





    AV_HWFRAME_MAP_OVERWRITE = 1 << 2,





    AV_HWFRAME_MAP_DIRECT = 1 << 3,
};
int av_hwframe_map(AVFrame *dst, const AVFrame *src, int flags);
int av_hwframe_ctx_create_derived(AVBufferRef **derived_frame_ctx,
                                  enum AVPixelFormat format,
                                  AVBufferRef *derived_device_ctx,
                                  AVBufferRef *source_frame_ctx,
                                  int flags);

enum AVCodecID {
    AV_CODEC_ID_NONE,


    AV_CODEC_ID_MPEG1VIDEO,
    AV_CODEC_ID_MPEG2VIDEO,
    AV_CODEC_ID_H261,
    AV_CODEC_ID_H263,
    AV_CODEC_ID_RV10,
    AV_CODEC_ID_RV20,
    AV_CODEC_ID_MJPEG,
    AV_CODEC_ID_MJPEGB,
    AV_CODEC_ID_LJPEG,
    AV_CODEC_ID_SP5X,
    AV_CODEC_ID_JPEGLS,
    AV_CODEC_ID_MPEG4,
    AV_CODEC_ID_RAWVIDEO,
    AV_CODEC_ID_MSMPEG4V1,
    AV_CODEC_ID_MSMPEG4V2,
    AV_CODEC_ID_MSMPEG4V3,
    AV_CODEC_ID_WMV1,
    AV_CODEC_ID_WMV2,
    AV_CODEC_ID_H263P,
    AV_CODEC_ID_H263I,
    AV_CODEC_ID_FLV1,
    AV_CODEC_ID_SVQ1,
    AV_CODEC_ID_SVQ3,
    AV_CODEC_ID_DVVIDEO,
    AV_CODEC_ID_HUFFYUV,
    AV_CODEC_ID_CYUV,
    AV_CODEC_ID_H264,
    AV_CODEC_ID_INDEO3,
    AV_CODEC_ID_VP3,
    AV_CODEC_ID_THEORA,
    AV_CODEC_ID_ASV1,
    AV_CODEC_ID_ASV2,
    AV_CODEC_ID_FFV1,
    AV_CODEC_ID_4XM,
    AV_CODEC_ID_VCR1,
    AV_CODEC_ID_CLJR,
    AV_CODEC_ID_MDEC,
    AV_CODEC_ID_ROQ,
    AV_CODEC_ID_INTERPLAY_VIDEO,
    AV_CODEC_ID_XAN_WC3,
    AV_CODEC_ID_XAN_WC4,
    AV_CODEC_ID_RPZA,
    AV_CODEC_ID_CINEPAK,
    AV_CODEC_ID_WS_VQA,
    AV_CODEC_ID_MSRLE,
    AV_CODEC_ID_MSVIDEO1,
    AV_CODEC_ID_IDCIN,
    AV_CODEC_ID_8BPS,
    AV_CODEC_ID_SMC,
    AV_CODEC_ID_FLIC,
    AV_CODEC_ID_TRUEMOTION1,
    AV_CODEC_ID_VMDVIDEO,
    AV_CODEC_ID_MSZH,
    AV_CODEC_ID_ZLIB,
    AV_CODEC_ID_QTRLE,
    AV_CODEC_ID_TSCC,
    AV_CODEC_ID_ULTI,
    AV_CODEC_ID_QDRAW,
    AV_CODEC_ID_VIXL,
    AV_CODEC_ID_QPEG,
    AV_CODEC_ID_PNG,
    AV_CODEC_ID_PPM,
    AV_CODEC_ID_PBM,
    AV_CODEC_ID_PGM,
    AV_CODEC_ID_PGMYUV,
    AV_CODEC_ID_PAM,
    AV_CODEC_ID_FFVHUFF,
    AV_CODEC_ID_RV30,
    AV_CODEC_ID_RV40,
    AV_CODEC_ID_VC1,
    AV_CODEC_ID_WMV3,
    AV_CODEC_ID_LOCO,
    AV_CODEC_ID_WNV1,
    AV_CODEC_ID_AASC,
    AV_CODEC_ID_INDEO2,
    AV_CODEC_ID_FRAPS,
    AV_CODEC_ID_TRUEMOTION2,
    AV_CODEC_ID_BMP,
    AV_CODEC_ID_CSCD,
    AV_CODEC_ID_MMVIDEO,
    AV_CODEC_ID_ZMBV,
    AV_CODEC_ID_AVS,
    AV_CODEC_ID_SMACKVIDEO,
    AV_CODEC_ID_NUV,
    AV_CODEC_ID_KMVC,
    AV_CODEC_ID_FLASHSV,
    AV_CODEC_ID_CAVS,
    AV_CODEC_ID_JPEG2000,
    AV_CODEC_ID_VMNC,
    AV_CODEC_ID_VP5,
    AV_CODEC_ID_VP6,
    AV_CODEC_ID_VP6F,
    AV_CODEC_ID_TARGA,
    AV_CODEC_ID_DSICINVIDEO,
    AV_CODEC_ID_TIERTEXSEQVIDEO,
    AV_CODEC_ID_TIFF,
    AV_CODEC_ID_GIF,
    AV_CODEC_ID_DXA,
    AV_CODEC_ID_DNXHD,
    AV_CODEC_ID_THP,
    AV_CODEC_ID_SGI,
    AV_CODEC_ID_C93,
    AV_CODEC_ID_BETHSOFTVID,
    AV_CODEC_ID_PTX,
    AV_CODEC_ID_TXD,
    AV_CODEC_ID_VP6A,
    AV_CODEC_ID_AMV,
    AV_CODEC_ID_VB,
    AV_CODEC_ID_PCX,
    AV_CODEC_ID_SUNRAST,
    AV_CODEC_ID_INDEO4,
    AV_CODEC_ID_INDEO5,
    AV_CODEC_ID_MIMIC,
    AV_CODEC_ID_RL2,
    AV_CODEC_ID_ESCAPE124,
    AV_CODEC_ID_DIRAC,
    AV_CODEC_ID_BFI,
    AV_CODEC_ID_CMV,
    AV_CODEC_ID_MOTIONPIXELS,
    AV_CODEC_ID_TGV,
    AV_CODEC_ID_TGQ,
    AV_CODEC_ID_TQI,
    AV_CODEC_ID_AURA,
    AV_CODEC_ID_AURA2,
    AV_CODEC_ID_V210X,
    AV_CODEC_ID_TMV,
    AV_CODEC_ID_V210,
    AV_CODEC_ID_DPX,
    AV_CODEC_ID_MAD,
    AV_CODEC_ID_FRWU,
    AV_CODEC_ID_FLASHSV2,
    AV_CODEC_ID_CDGRAPHICS,
    AV_CODEC_ID_R210,
    AV_CODEC_ID_ANM,
    AV_CODEC_ID_BINKVIDEO,
    AV_CODEC_ID_IFF_ILBM,

    AV_CODEC_ID_KGV1,
    AV_CODEC_ID_YOP,
    AV_CODEC_ID_VP8,
    AV_CODEC_ID_PICTOR,
    AV_CODEC_ID_ANSI,
    AV_CODEC_ID_A64_MULTI,
    AV_CODEC_ID_A64_MULTI5,
    AV_CODEC_ID_R10K,
    AV_CODEC_ID_MXPEG,
    AV_CODEC_ID_LAGARITH,
    AV_CODEC_ID_PRORES,
    AV_CODEC_ID_JV,
    AV_CODEC_ID_DFA,
    AV_CODEC_ID_WMV3IMAGE,
    AV_CODEC_ID_VC1IMAGE,
    AV_CODEC_ID_UTVIDEO,
    AV_CODEC_ID_BMV_VIDEO,
    AV_CODEC_ID_VBLE,
    AV_CODEC_ID_DXTORY,
    AV_CODEC_ID_V410,
    AV_CODEC_ID_XWD,
    AV_CODEC_ID_CDXL,
    AV_CODEC_ID_XBM,
    AV_CODEC_ID_ZEROCODEC,
    AV_CODEC_ID_MSS1,
    AV_CODEC_ID_MSA1,
    AV_CODEC_ID_TSCC2,
    AV_CODEC_ID_MTS2,
    AV_CODEC_ID_CLLC,
    AV_CODEC_ID_MSS2,
    AV_CODEC_ID_VP9,
    AV_CODEC_ID_AIC,
    AV_CODEC_ID_ESCAPE130,
    AV_CODEC_ID_G2M,
    AV_CODEC_ID_WEBP,
    AV_CODEC_ID_HNM4_VIDEO,
    AV_CODEC_ID_HEVC,

    AV_CODEC_ID_FIC,
    AV_CODEC_ID_ALIAS_PIX,
    AV_CODEC_ID_BRENDER_PIX,
    AV_CODEC_ID_PAF_VIDEO,
    AV_CODEC_ID_EXR,
    AV_CODEC_ID_VP7,
    AV_CODEC_ID_SANM,
    AV_CODEC_ID_SGIRLE,
    AV_CODEC_ID_MVC1,
    AV_CODEC_ID_MVC2,
    AV_CODEC_ID_HQX,
    AV_CODEC_ID_TDSC,
    AV_CODEC_ID_HQ_HQA,
    AV_CODEC_ID_HAP,
    AV_CODEC_ID_DDS,
    AV_CODEC_ID_DXV,
    AV_CODEC_ID_SCREENPRESSO,
    AV_CODEC_ID_RSCC,
    AV_CODEC_ID_AVS2,
    AV_CODEC_ID_PGX,
    AV_CODEC_ID_AVS3,
    AV_CODEC_ID_MSP2,
    AV_CODEC_ID_VVC,


    AV_CODEC_ID_Y41P = 0x8000,
    AV_CODEC_ID_AVRP,
    AV_CODEC_ID_012V,
    AV_CODEC_ID_AVUI,
    AV_CODEC_ID_AYUV,
    AV_CODEC_ID_TARGA_Y216,
    AV_CODEC_ID_V308,
    AV_CODEC_ID_V408,
    AV_CODEC_ID_YUV4,
    AV_CODEC_ID_AVRN,
    AV_CODEC_ID_CPIA,
    AV_CODEC_ID_XFACE,
    AV_CODEC_ID_SNOW,
    AV_CODEC_ID_SMVJPEG,
    AV_CODEC_ID_APNG,
    AV_CODEC_ID_DAALA,
    AV_CODEC_ID_CFHD,
    AV_CODEC_ID_TRUEMOTION2RT,
    AV_CODEC_ID_M101,
    AV_CODEC_ID_MAGICYUV,
    AV_CODEC_ID_SHEERVIDEO,
    AV_CODEC_ID_YLC,
    AV_CODEC_ID_PSD,
    AV_CODEC_ID_PIXLET,
    AV_CODEC_ID_SPEEDHQ,
    AV_CODEC_ID_FMVC,
    AV_CODEC_ID_SCPR,
    AV_CODEC_ID_CLEARVIDEO,
    AV_CODEC_ID_XPM,
    AV_CODEC_ID_AV1,
    AV_CODEC_ID_BITPACKED,
    AV_CODEC_ID_MSCC,
    AV_CODEC_ID_SRGC,
    AV_CODEC_ID_SVG,
    AV_CODEC_ID_GDV,
    AV_CODEC_ID_FITS,
    AV_CODEC_ID_IMM4,
    AV_CODEC_ID_PROSUMER,
    AV_CODEC_ID_MWSC,
    AV_CODEC_ID_WCMV,
    AV_CODEC_ID_RASC,
    AV_CODEC_ID_HYMT,
    AV_CODEC_ID_ARBC,
    AV_CODEC_ID_AGM,
    AV_CODEC_ID_LSCR,
    AV_CODEC_ID_VP4,
    AV_CODEC_ID_IMM5,
    AV_CODEC_ID_MVDV,
    AV_CODEC_ID_MVHA,
    AV_CODEC_ID_CDTOONS,
    AV_CODEC_ID_MV30,
    AV_CODEC_ID_NOTCHLC,
    AV_CODEC_ID_PFM,
    AV_CODEC_ID_MOBICLIP,
    AV_CODEC_ID_PHOTOCD,
    AV_CODEC_ID_IPU,
    AV_CODEC_ID_ARGO,
    AV_CODEC_ID_CRI,
    AV_CODEC_ID_SIMBIOSIS_IMX,
    AV_CODEC_ID_SGA_VIDEO,


    AV_CODEC_ID_FIRST_AUDIO = 0x10000,
    AV_CODEC_ID_PCM_S16LE = 0x10000,
    AV_CODEC_ID_PCM_S16BE,
    AV_CODEC_ID_PCM_U16LE,
    AV_CODEC_ID_PCM_U16BE,
    AV_CODEC_ID_PCM_S8,
    AV_CODEC_ID_PCM_U8,
    AV_CODEC_ID_PCM_MULAW,
    AV_CODEC_ID_PCM_ALAW,
    AV_CODEC_ID_PCM_S32LE,
    AV_CODEC_ID_PCM_S32BE,
    AV_CODEC_ID_PCM_U32LE,
    AV_CODEC_ID_PCM_U32BE,
    AV_CODEC_ID_PCM_S24LE,
    AV_CODEC_ID_PCM_S24BE,
    AV_CODEC_ID_PCM_U24LE,
    AV_CODEC_ID_PCM_U24BE,
    AV_CODEC_ID_PCM_S24DAUD,
    AV_CODEC_ID_PCM_ZORK,
    AV_CODEC_ID_PCM_S16LE_PLANAR,
    AV_CODEC_ID_PCM_DVD,
    AV_CODEC_ID_PCM_F32BE,
    AV_CODEC_ID_PCM_F32LE,
    AV_CODEC_ID_PCM_F64BE,
    AV_CODEC_ID_PCM_F64LE,
    AV_CODEC_ID_PCM_BLURAY,
    AV_CODEC_ID_PCM_LXF,
    AV_CODEC_ID_S302M,
    AV_CODEC_ID_PCM_S8_PLANAR,
    AV_CODEC_ID_PCM_S24LE_PLANAR,
    AV_CODEC_ID_PCM_S32LE_PLANAR,
    AV_CODEC_ID_PCM_S16BE_PLANAR,

    AV_CODEC_ID_PCM_S64LE = 0x10800,
    AV_CODEC_ID_PCM_S64BE,
    AV_CODEC_ID_PCM_F16LE,
    AV_CODEC_ID_PCM_F24LE,
    AV_CODEC_ID_PCM_VIDC,
    AV_CODEC_ID_PCM_SGA,


    AV_CODEC_ID_ADPCM_IMA_QT = 0x11000,
    AV_CODEC_ID_ADPCM_IMA_WAV,
    AV_CODEC_ID_ADPCM_IMA_DK3,
    AV_CODEC_ID_ADPCM_IMA_DK4,
    AV_CODEC_ID_ADPCM_IMA_WS,
    AV_CODEC_ID_ADPCM_IMA_SMJPEG,
    AV_CODEC_ID_ADPCM_MS,
    AV_CODEC_ID_ADPCM_4XM,
    AV_CODEC_ID_ADPCM_XA,
    AV_CODEC_ID_ADPCM_ADX,
    AV_CODEC_ID_ADPCM_EA,
    AV_CODEC_ID_ADPCM_G726,
    AV_CODEC_ID_ADPCM_CT,
    AV_CODEC_ID_ADPCM_SWF,
    AV_CODEC_ID_ADPCM_YAMAHA,
    AV_CODEC_ID_ADPCM_SBPRO_4,
    AV_CODEC_ID_ADPCM_SBPRO_3,
    AV_CODEC_ID_ADPCM_SBPRO_2,
    AV_CODEC_ID_ADPCM_THP,
    AV_CODEC_ID_ADPCM_IMA_AMV,
    AV_CODEC_ID_ADPCM_EA_R1,
    AV_CODEC_ID_ADPCM_EA_R3,
    AV_CODEC_ID_ADPCM_EA_R2,
    AV_CODEC_ID_ADPCM_IMA_EA_SEAD,
    AV_CODEC_ID_ADPCM_IMA_EA_EACS,
    AV_CODEC_ID_ADPCM_EA_XAS,
    AV_CODEC_ID_ADPCM_EA_MAXIS_XA,
    AV_CODEC_ID_ADPCM_IMA_ISS,
    AV_CODEC_ID_ADPCM_G722,
    AV_CODEC_ID_ADPCM_IMA_APC,
    AV_CODEC_ID_ADPCM_VIMA,

    AV_CODEC_ID_ADPCM_AFC = 0x11800,
    AV_CODEC_ID_ADPCM_IMA_OKI,
    AV_CODEC_ID_ADPCM_DTK,
    AV_CODEC_ID_ADPCM_IMA_RAD,
    AV_CODEC_ID_ADPCM_G726LE,
    AV_CODEC_ID_ADPCM_THP_LE,
    AV_CODEC_ID_ADPCM_PSX,
    AV_CODEC_ID_ADPCM_AICA,
    AV_CODEC_ID_ADPCM_IMA_DAT4,
    AV_CODEC_ID_ADPCM_MTAF,
    AV_CODEC_ID_ADPCM_AGM,
    AV_CODEC_ID_ADPCM_ARGO,
    AV_CODEC_ID_ADPCM_IMA_SSI,
    AV_CODEC_ID_ADPCM_ZORK,
    AV_CODEC_ID_ADPCM_IMA_APM,
    AV_CODEC_ID_ADPCM_IMA_ALP,
    AV_CODEC_ID_ADPCM_IMA_MTF,
    AV_CODEC_ID_ADPCM_IMA_CUNNING,
    AV_CODEC_ID_ADPCM_IMA_MOFLEX,


    AV_CODEC_ID_AMR_NB = 0x12000,
    AV_CODEC_ID_AMR_WB,


    AV_CODEC_ID_RA_144 = 0x13000,
    AV_CODEC_ID_RA_288,


    AV_CODEC_ID_ROQ_DPCM = 0x14000,
    AV_CODEC_ID_INTERPLAY_DPCM,
    AV_CODEC_ID_XAN_DPCM,
    AV_CODEC_ID_SOL_DPCM,

    AV_CODEC_ID_SDX2_DPCM = 0x14800,
    AV_CODEC_ID_GREMLIN_DPCM,
    AV_CODEC_ID_DERF_DPCM,


    AV_CODEC_ID_MP2 = 0x15000,
    AV_CODEC_ID_MP3,
    AV_CODEC_ID_AAC,
    AV_CODEC_ID_AC3,
    AV_CODEC_ID_DTS,
    AV_CODEC_ID_VORBIS,
    AV_CODEC_ID_DVAUDIO,
    AV_CODEC_ID_WMAV1,
    AV_CODEC_ID_WMAV2,
    AV_CODEC_ID_MACE3,
    AV_CODEC_ID_MACE6,
    AV_CODEC_ID_VMDAUDIO,
    AV_CODEC_ID_FLAC,
    AV_CODEC_ID_MP3ADU,
    AV_CODEC_ID_MP3ON4,
    AV_CODEC_ID_SHORTEN,
    AV_CODEC_ID_ALAC,
    AV_CODEC_ID_WESTWOOD_SND1,
    AV_CODEC_ID_GSM,
    AV_CODEC_ID_QDM2,
    AV_CODEC_ID_COOK,
    AV_CODEC_ID_TRUESPEECH,
    AV_CODEC_ID_TTA,
    AV_CODEC_ID_SMACKAUDIO,
    AV_CODEC_ID_QCELP,
    AV_CODEC_ID_WAVPACK,
    AV_CODEC_ID_DSICINAUDIO,
    AV_CODEC_ID_IMC,
    AV_CODEC_ID_MUSEPACK7,
    AV_CODEC_ID_MLP,
    AV_CODEC_ID_GSM_MS,
    AV_CODEC_ID_ATRAC3,
    AV_CODEC_ID_APE,
    AV_CODEC_ID_NELLYMOSER,
    AV_CODEC_ID_MUSEPACK8,
    AV_CODEC_ID_SPEEX,
    AV_CODEC_ID_WMAVOICE,
    AV_CODEC_ID_WMAPRO,
    AV_CODEC_ID_WMALOSSLESS,
    AV_CODEC_ID_ATRAC3P,
    AV_CODEC_ID_EAC3,
    AV_CODEC_ID_SIPR,
    AV_CODEC_ID_MP1,
    AV_CODEC_ID_TWINVQ,
    AV_CODEC_ID_TRUEHD,
    AV_CODEC_ID_MP4ALS,
    AV_CODEC_ID_ATRAC1,
    AV_CODEC_ID_BINKAUDIO_RDFT,
    AV_CODEC_ID_BINKAUDIO_DCT,
    AV_CODEC_ID_AAC_LATM,
    AV_CODEC_ID_QDMC,
    AV_CODEC_ID_CELT,
    AV_CODEC_ID_G723_1,
    AV_CODEC_ID_G729,
    AV_CODEC_ID_8SVX_EXP,
    AV_CODEC_ID_8SVX_FIB,
    AV_CODEC_ID_BMV_AUDIO,
    AV_CODEC_ID_RALF,
    AV_CODEC_ID_IAC,
    AV_CODEC_ID_ILBC,
    AV_CODEC_ID_OPUS,
    AV_CODEC_ID_COMFORT_NOISE,
    AV_CODEC_ID_TAK,
    AV_CODEC_ID_METASOUND,
    AV_CODEC_ID_PAF_AUDIO,
    AV_CODEC_ID_ON2AVC,
    AV_CODEC_ID_DSS_SP,
    AV_CODEC_ID_CODEC2,

    AV_CODEC_ID_FFWAVESYNTH = 0x15800,
    AV_CODEC_ID_SONIC,
    AV_CODEC_ID_SONIC_LS,
    AV_CODEC_ID_EVRC,
    AV_CODEC_ID_SMV,
    AV_CODEC_ID_DSD_LSBF,
    AV_CODEC_ID_DSD_MSBF,
    AV_CODEC_ID_DSD_LSBF_PLANAR,
    AV_CODEC_ID_DSD_MSBF_PLANAR,
    AV_CODEC_ID_4GV,
    AV_CODEC_ID_INTERPLAY_ACM,
    AV_CODEC_ID_XMA1,
    AV_CODEC_ID_XMA2,
    AV_CODEC_ID_DST,
    AV_CODEC_ID_ATRAC3AL,
    AV_CODEC_ID_ATRAC3PAL,
    AV_CODEC_ID_DOLBY_E,
    AV_CODEC_ID_APTX,
    AV_CODEC_ID_APTX_HD,
    AV_CODEC_ID_SBC,
    AV_CODEC_ID_ATRAC9,
    AV_CODEC_ID_HCOM,
    AV_CODEC_ID_ACELP_KELVIN,
    AV_CODEC_ID_MPEGH_3D_AUDIO,
    AV_CODEC_ID_SIREN,
    AV_CODEC_ID_HCA,
    AV_CODEC_ID_FASTAUDIO,


    AV_CODEC_ID_FIRST_SUBTITLE = 0x17000,
    AV_CODEC_ID_DVD_SUBTITLE = 0x17000,
    AV_CODEC_ID_DVB_SUBTITLE,
    AV_CODEC_ID_TEXT,
    AV_CODEC_ID_XSUB,
    AV_CODEC_ID_SSA,
    AV_CODEC_ID_MOV_TEXT,
    AV_CODEC_ID_HDMV_PGS_SUBTITLE,
    AV_CODEC_ID_DVB_TELETEXT,
    AV_CODEC_ID_SRT,

    AV_CODEC_ID_MICRODVD = 0x17800,
    AV_CODEC_ID_EIA_608,
    AV_CODEC_ID_JACOSUB,
    AV_CODEC_ID_SAMI,
    AV_CODEC_ID_REALTEXT,
    AV_CODEC_ID_STL,
    AV_CODEC_ID_SUBVIEWER1,
    AV_CODEC_ID_SUBVIEWER,
    AV_CODEC_ID_SUBRIP,
    AV_CODEC_ID_WEBVTT,
    AV_CODEC_ID_MPL2,
    AV_CODEC_ID_VPLAYER,
    AV_CODEC_ID_PJS,
    AV_CODEC_ID_ASS,
    AV_CODEC_ID_HDMV_TEXT_SUBTITLE,
    AV_CODEC_ID_TTML,
    AV_CODEC_ID_ARIB_CAPTION,


    AV_CODEC_ID_FIRST_UNKNOWN = 0x18000,
    AV_CODEC_ID_TTF = 0x18000,

    AV_CODEC_ID_SCTE_35,
    AV_CODEC_ID_EPG,
    AV_CODEC_ID_BINTEXT = 0x18800,
    AV_CODEC_ID_XBIN,
    AV_CODEC_ID_IDF,
    AV_CODEC_ID_OTF,
    AV_CODEC_ID_SMPTE_KLV,
    AV_CODEC_ID_DVD_NAV,
    AV_CODEC_ID_TIMED_ID3,
    AV_CODEC_ID_BIN_DATA,


    AV_CODEC_ID_PROBE = 0x19000,

    AV_CODEC_ID_MPEG2TS = 0x20000,

    AV_CODEC_ID_MPEG4SYSTEMS = 0x20001,

    AV_CODEC_ID_FFMETADATA = 0x21000,
    AV_CODEC_ID_WRAPPED_AVFRAME = 0x21001,
};




enum AVMediaType avcodec_get_type(enum AVCodecID codec_id);





const char *avcodec_get_name(enum AVCodecID id);
enum AVFieldOrder {
    AV_FIELD_UNKNOWN,
    AV_FIELD_PROGRESSIVE,
    AV_FIELD_TT,
    AV_FIELD_BB,
    AV_FIELD_TB,
    AV_FIELD_BT,
};
typedef struct AVCodecParameters {



    enum AVMediaType codec_type;



    enum AVCodecID codec_id;



    uint32_t codec_tag;
    uint8_t *extradata;



    int extradata_size;





    int format;




    int64_t bit_rate;
    int bits_per_coded_sample;
    int bits_per_raw_sample;




    int profile;
    int level;




    int width;
    int height;
    AVRational sample_aspect_ratio;




    enum AVFieldOrder field_order;




    enum AVColorRange color_range;
    enum AVColorPrimaries color_primaries;
    enum AVColorTransferCharacteristic color_trc;
    enum AVColorSpace color_space;
    enum AVChromaLocation chroma_location;




    int video_delay;






    uint64_t channel_layout;



    int channels;



    int sample_rate;






    int block_align;



    int frame_size;







    int initial_padding;






    int trailing_padding;



    int seek_preroll;
} AVCodecParameters;






AVCodecParameters *avcodec_parameters_alloc(void);





void avcodec_parameters_free(AVCodecParameters **par);







int avcodec_parameters_copy(AVCodecParameters *dst, const AVCodecParameters *src);














enum AVPacketSideDataType {





    AV_PKT_DATA_PALETTE,
    AV_PKT_DATA_NEW_EXTRADATA,
    AV_PKT_DATA_PARAM_CHANGE,
    AV_PKT_DATA_H263_MB_INFO,





    AV_PKT_DATA_REPLAYGAIN,
    AV_PKT_DATA_DISPLAYMATRIX,





    AV_PKT_DATA_STEREO3D,





    AV_PKT_DATA_AUDIO_SERVICE_TYPE,
    AV_PKT_DATA_QUALITY_STATS,







    AV_PKT_DATA_FALLBACK_TRACK,




    AV_PKT_DATA_CPB_PROPERTIES,
    AV_PKT_DATA_SKIP_SAMPLES,
    AV_PKT_DATA_JP_DUALMONO,





    AV_PKT_DATA_STRINGS_METADATA,
    AV_PKT_DATA_SUBTITLE_POSITION,







    AV_PKT_DATA_MATROSKA_BLOCKADDITIONAL,




    AV_PKT_DATA_WEBVTT_IDENTIFIER,





    AV_PKT_DATA_WEBVTT_SETTINGS,






    AV_PKT_DATA_METADATA_UPDATE,





    AV_PKT_DATA_MPEGTS_STREAM_ID,






    AV_PKT_DATA_MASTERING_DISPLAY_METADATA,





    AV_PKT_DATA_SPHERICAL,






    AV_PKT_DATA_CONTENT_LIGHT_LEVEL,






    AV_PKT_DATA_A53_CC,






    AV_PKT_DATA_ENCRYPTION_INIT_INFO,





    AV_PKT_DATA_ENCRYPTION_INFO,





    AV_PKT_DATA_AFD,






    AV_PKT_DATA_PRFT,





    AV_PKT_DATA_ICC_PROFILE,
    AV_PKT_DATA_DOVI_CONF,







    AV_PKT_DATA_S12M_TIMECODE,
    AV_PKT_DATA_NB
};



typedef struct AVPacketSideData {
    uint8_t *data;

    int size;



    enum AVPacketSideDataType type;
} AVPacketSideData;
typedef struct AVPacket {





    AVBufferRef *buf;
    int64_t pts;





    int64_t dts;
    uint8_t *data;
    int size;
    int stream_index;



    int flags;




    AVPacketSideData *side_data;
    int side_data_elems;





    int64_t duration;

    int64_t pos;







    __attribute__((deprecated))
    int64_t convergence_duration;

} AVPacket;


__attribute__((deprecated))
typedef struct AVPacketList {
    AVPacket pkt;
    struct AVPacketList *next;
} AVPacketList;
enum AVSideDataParamChangeFlags {
    AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_COUNT = 0x0001,
    AV_SIDE_DATA_PARAM_CHANGE_CHANNEL_LAYOUT = 0x0002,
    AV_SIDE_DATA_PARAM_CHANGE_SAMPLE_RATE = 0x0004,
    AV_SIDE_DATA_PARAM_CHANGE_DIMENSIONS = 0x0008,
};
AVPacket *av_packet_alloc(void);
AVPacket *av_packet_clone(const AVPacket *src);
void av_packet_free(AVPacket **pkt);
__attribute__((deprecated))
void av_init_packet(AVPacket *pkt);
int av_new_packet(AVPacket *pkt, int size);







void av_shrink_packet(AVPacket *pkt, int size);







int av_grow_packet(AVPacket *pkt, int grow_by);
int av_packet_from_data(AVPacket *pkt, uint8_t *data, int size);
__attribute__((deprecated))
int av_dup_packet(AVPacket *pkt);







__attribute__((deprecated))
int av_copy_packet(AVPacket *dst, const AVPacket *src);
__attribute__((deprecated))
int av_copy_packet_side_data(AVPacket *dst, const AVPacket *src);
__attribute__((deprecated))
void av_free_packet(AVPacket *pkt);
uint8_t* av_packet_new_side_data(AVPacket *pkt, enum AVPacketSideDataType type,

                                 int size);
int av_packet_add_side_data(AVPacket *pkt, enum AVPacketSideDataType type,
                            uint8_t *data, size_t size);
int av_packet_shrink_side_data(AVPacket *pkt, enum AVPacketSideDataType type,

                               int size);
uint8_t* av_packet_get_side_data(const AVPacket *pkt, enum AVPacketSideDataType type,

                                 int *size);





__attribute__((deprecated))
int av_packet_merge_side_data(AVPacket *pkt);

__attribute__((deprecated))
int av_packet_split_side_data(AVPacket *pkt);


const char *av_packet_side_data_name(enum AVPacketSideDataType type);
uint8_t *av_packet_pack_dictionary(AVDictionary *dict, int *size);
int av_packet_unpack_dictionary(const uint8_t *data, int size, AVDictionary **dict);
void av_packet_free_side_data(AVPacket *pkt);
int av_packet_ref(AVPacket *dst, const AVPacket *src);
void av_packet_unref(AVPacket *pkt);
void av_packet_move_ref(AVPacket *dst, AVPacket *src);
int av_packet_copy_props(AVPacket *dst, const AVPacket *src);
int av_packet_make_refcounted(AVPacket *pkt);
int av_packet_make_writable(AVPacket *pkt);
void av_packet_rescale_ts(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);






typedef struct AVBSFInternal AVBSFInternal;
typedef struct AVBSFContext {



    const AVClass *av_class;




    const struct AVBitStreamFilter *filter;





    AVBSFInternal *internal;





    void *priv_data;






    AVCodecParameters *par_in;





    AVCodecParameters *par_out;





    AVRational time_base_in;





    AVRational time_base_out;
} AVBSFContext;

typedef struct AVBitStreamFilter {
    const char *name;






    const enum AVCodecID *codec_ids;
    const AVClass *priv_class;
    int priv_data_size;
    int (*init)(AVBSFContext *ctx);
    int (*filter)(AVBSFContext *ctx, AVPacket *pkt);
    void (*close)(AVBSFContext *ctx);
    void (*flush)(AVBSFContext *ctx);
} AVBitStreamFilter;





const AVBitStreamFilter *av_bsf_get_by_name(const char *name);
const AVBitStreamFilter *av_bsf_iterate(void **opaque);
int av_bsf_alloc(const AVBitStreamFilter *filter, AVBSFContext **ctx);





int av_bsf_init(AVBSFContext *ctx);
int av_bsf_send_packet(AVBSFContext *ctx, AVPacket *pkt);
int av_bsf_receive_packet(AVBSFContext *ctx, AVPacket *pkt);




void av_bsf_flush(AVBSFContext *ctx);





void av_bsf_free(AVBSFContext **ctx);







const AVClass *av_bsf_get_class(void);





typedef struct AVBSFList AVBSFList;
AVBSFList *av_bsf_list_alloc(void);






void av_bsf_list_free(AVBSFList **lst);
int av_bsf_list_append(AVBSFList *lst, AVBSFContext *bsf);
int av_bsf_list_append2(AVBSFList *lst, const char * bsf_name, AVDictionary **options);
int av_bsf_list_finalize(AVBSFList **lst, AVBSFContext **bsf);
int av_bsf_list_parse_str(const char *str, AVBSFContext **bsf);
int av_bsf_get_null_filter(AVBSFContext **bsf);
typedef struct AVProfile {
    int profile;
    const char *name;
} AVProfile;

typedef struct AVCodecDefault AVCodecDefault;

struct AVCodecContext;
struct AVSubtitle;
struct AVPacket;




typedef struct AVCodec {






    const char *name;




    const char *long_name;
    enum AVMediaType type;
    enum AVCodecID id;




    int capabilities;
    const AVRational *supported_framerates;
    const enum AVPixelFormat *pix_fmts;
    const int *supported_samplerates;
    const enum AVSampleFormat *sample_fmts;
    const uint64_t *channel_layouts;
    uint8_t max_lowres;
    const AVClass *priv_class;
    const AVProfile *profiles;
    const char *wrapper_name;
    int priv_data_size;

    struct AVCodec *next;
    int (*update_thread_context)(struct AVCodecContext *dst, const struct AVCodecContext *src);





    const AVCodecDefault *defaults;







    void (*init_static_data)(struct AVCodec *codec);

    int (*init)(struct AVCodecContext *);
    int (*encode_sub)(struct AVCodecContext *, uint8_t *buf, int buf_size,
                      const struct AVSubtitle *sub);
    int (*encode2)(struct AVCodecContext *avctx, struct AVPacket *avpkt,
                   const struct AVFrame *frame, int *got_packet_ptr);
    int (*decode)(struct AVCodecContext *avctx, void *outdata,
                  int *got_frame_ptr, struct AVPacket *avpkt);
    int (*close)(struct AVCodecContext *);





    int (*receive_packet)(struct AVCodecContext *avctx, struct AVPacket *avpkt);






    int (*receive_frame)(struct AVCodecContext *avctx, struct AVFrame *frame);




    void (*flush)(struct AVCodecContext *);




    int caps_internal;





    const char *bsfs;
    const struct AVCodecHWConfigInternal *const *hw_configs;




    const uint32_t *codec_tags;
} AVCodec;
const AVCodec *av_codec_iterate(void **opaque);







AVCodec *avcodec_find_decoder(enum AVCodecID id);







AVCodec *avcodec_find_decoder_by_name(const char *name);







AVCodec *avcodec_find_encoder(enum AVCodecID id);







AVCodec *avcodec_find_encoder_by_name(const char *name);



int av_codec_is_encoder(const AVCodec *codec);




int av_codec_is_decoder(const AVCodec *codec);

enum {







    AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX = 0x01,
    AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX = 0x02,






    AV_CODEC_HW_CONFIG_METHOD_INTERNAL = 0x04,
    AV_CODEC_HW_CONFIG_METHOD_AD_HOC = 0x08,
};

typedef struct AVCodecHWConfig {
    enum AVPixelFormat pix_fmt;




    int methods;






    enum AVHWDeviceType device_type;
} AVCodecHWConfig;
const AVCodecHWConfig *avcodec_get_hw_config(const AVCodec *codec, int index);
typedef struct AVCodecDescriptor {
    enum AVCodecID id;
    enum AVMediaType type;





    const char *name;



    const char *long_name;



    int props;





    const char *const *mime_types;




    const struct AVProfile *profiles;
} AVCodecDescriptor;
const AVCodecDescriptor *avcodec_descriptor_get(enum AVCodecID id);
const AVCodecDescriptor *avcodec_descriptor_next(const AVCodecDescriptor *prev);





const AVCodecDescriptor *avcodec_descriptor_get_by_name(const char *name);



enum AVDiscard{


    AVDISCARD_NONE =-16,
    AVDISCARD_DEFAULT = 0,
    AVDISCARD_NONREF = 8,
    AVDISCARD_BIDIR = 16,
    AVDISCARD_NONINTRA= 24,
    AVDISCARD_NONKEY = 32,
    AVDISCARD_ALL = 48,
};

enum AVAudioServiceType {
    AV_AUDIO_SERVICE_TYPE_MAIN = 0,
    AV_AUDIO_SERVICE_TYPE_EFFECTS = 1,
    AV_AUDIO_SERVICE_TYPE_VISUALLY_IMPAIRED = 2,
    AV_AUDIO_SERVICE_TYPE_HEARING_IMPAIRED = 3,
    AV_AUDIO_SERVICE_TYPE_DIALOGUE = 4,
    AV_AUDIO_SERVICE_TYPE_COMMENTARY = 5,
    AV_AUDIO_SERVICE_TYPE_EMERGENCY = 6,
    AV_AUDIO_SERVICE_TYPE_VOICE_OVER = 7,
    AV_AUDIO_SERVICE_TYPE_KARAOKE = 8,
    AV_AUDIO_SERVICE_TYPE_NB ,
};




typedef struct RcOverride{
    int start_frame;
    int end_frame;
    int qscale;
    float quality_factor;
} RcOverride;
typedef struct AVPanScan {





    int id;






    int width;
    int height;






    int16_t position[3][2];
} AVPanScan;






typedef struct AVCPBProperties {





    int max_bitrate;
    int min_bitrate;
    int avg_bitrate;
    int buffer_size;
    uint64_t vbv_delay;
} AVCPBProperties;






typedef struct AVProducerReferenceTime {



    int64_t wallclock;
    int flags;
} AVProducerReferenceTime;
struct AVCodecInternal;
typedef struct AVCodecContext {




    const AVClass *av_class;
    int log_level_offset;

    enum AVMediaType codec_type;
    const struct AVCodec *codec;
    enum AVCodecID codec_id;
    unsigned int codec_tag;

    void *priv_data;







    struct AVCodecInternal *internal;






    void *opaque;







    int64_t bit_rate;







    int bit_rate_tolerance;







    int global_quality;





    int compression_level;







    int flags;






    int flags2;
    uint8_t *extradata;
    int extradata_size;
    AVRational time_base;
    int ticks_per_frame;
    int delay;
    int width, height;
    int coded_width, coded_height;






    int gop_size;
    enum AVPixelFormat pix_fmt;
    void (*draw_horiz_band)(struct AVCodecContext *s,
                            const AVFrame *src, int offset[8],
                            int y, int type, int height);
    enum AVPixelFormat (*get_format)(struct AVCodecContext *s, const enum AVPixelFormat * fmt);







    int max_b_frames;
    float b_quant_factor;



    __attribute__((deprecated))
    int b_frame_strategy;







    float b_quant_offset;







    int has_b_frames;



    __attribute__((deprecated))
    int mpeg_quant;
    float i_quant_factor;






    float i_quant_offset;






    float lumi_masking;






    float temporal_cplx_masking;






    float spatial_cplx_masking;






    float p_masking;






    float dark_masking;






    int slice_count;



    __attribute__((deprecated))
     int prediction_method;
    int *slice_offset;
    AVRational sample_aspect_ratio;






    int me_cmp;





    int me_sub_cmp;





    int mb_cmp;





    int ildct_cmp;
    int dia_size;






    int last_predictor_count;



    __attribute__((deprecated))
    int pre_me;







    int me_pre_cmp;






    int pre_dia_size;






    int me_subpel_quality;
    int me_range;






    int slice_flags;
    int mb_decision;
    uint16_t *intra_matrix;
    uint16_t *inter_matrix;



    __attribute__((deprecated))
    int scenechange_threshold;


    __attribute__((deprecated))
    int noise_reduction;







    int intra_dc_precision;






    int skip_top;






    int skip_bottom;






    int mb_lmin;






    int mb_lmax;





    __attribute__((deprecated))
    int me_penalty_compensation;






    int bidir_refine;



    __attribute__((deprecated))
    int brd_scale;







    int keyint_min;






    int refs;



    __attribute__((deprecated))
    int chromaoffset;







    int mv0_threshold;



    __attribute__((deprecated))
    int b_sensitivity;







    enum AVColorPrimaries color_primaries;






    enum AVColorTransferCharacteristic color_trc;






    enum AVColorSpace colorspace;






    enum AVColorRange color_range;






    enum AVChromaLocation chroma_sample_location;
    int slices;





    enum AVFieldOrder field_order;


    int sample_rate;
    int channels;






    enum AVSampleFormat sample_fmt;
    int frame_size;
    int frame_number;





    int block_align;






    int cutoff;






    uint64_t channel_layout;






    uint64_t request_channel_layout;






    enum AVAudioServiceType audio_service_type;







    enum AVSampleFormat request_sample_fmt;
    int (*get_buffer2)(struct AVCodecContext *s, AVFrame *frame, int flags);
    __attribute__((deprecated))
    int refcounted_frames;



    float qcompress;
    float qblur;






    int qmin;






    int qmax;






    int max_qdiff;






    int rc_buffer_size;






    int rc_override_count;
    RcOverride *rc_override;






    int64_t rc_max_rate;






    int64_t rc_min_rate;






    float rc_max_available_vbv_use;






    float rc_min_vbv_overflow_use;






    int rc_initial_buffer_occupancy;
    __attribute__((deprecated))
    int coder_type;




    __attribute__((deprecated))
    int context_model;




    __attribute__((deprecated))
    int frame_skip_threshold;


    __attribute__((deprecated))
    int frame_skip_factor;


    __attribute__((deprecated))
    int frame_skip_exp;


    __attribute__((deprecated))
    int frame_skip_cmp;







    int trellis;



    __attribute__((deprecated))
    int min_prediction_order;


    __attribute__((deprecated))
    int max_prediction_order;


    __attribute__((deprecated))
    int64_t timecode_frame_start;
    __attribute__((deprecated))
    void (*rtp_callback)(struct AVCodecContext *avctx, void *data, int size, int mb_nb);




    __attribute__((deprecated))
    int rtp_payload_size;
    __attribute__((deprecated))
    int mv_bits;
    __attribute__((deprecated))
    int header_bits;
    __attribute__((deprecated))
    int i_tex_bits;
    __attribute__((deprecated))
    int p_tex_bits;
    __attribute__((deprecated))
    int i_count;
    __attribute__((deprecated))
    int p_count;
    __attribute__((deprecated))
    int skip_count;
    __attribute__((deprecated))
    int misc_bits;


    __attribute__((deprecated))
    int frame_bits;







    char *stats_out;







    char *stats_in;






    int workaround_bugs;
    int strict_std_compliance;
    int error_concealment;
    int debug;
    int err_recognition;
    int64_t reordered_opaque;






    const struct AVHWAccel *hwaccel;
    void *hwaccel_context;






    uint64_t error[8];






    int dct_algo;
    int idct_algo;
     int bits_per_coded_sample;






    int bits_per_raw_sample;






     int lowres;
    __attribute__((deprecated)) AVFrame *coded_frame;
    int thread_count;
    int thread_type;
    int active_thread_type;
    __attribute__((deprecated))
    int thread_safe_callbacks;
    int (*execute)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg), void *arg2, int *ret, int count, int size);
    int (*execute2)(struct AVCodecContext *c, int (*func)(struct AVCodecContext *c2, void *arg, int jobnr, int threadnr), void *arg2, int *ret, int count);






     int nsse_weight;






     int profile;
     int level;







    enum AVDiscard skip_loop_filter;






    enum AVDiscard skip_idct;






    enum AVDiscard skip_frame;
    uint8_t *subtitle_header;
    int subtitle_header_size;
    __attribute__((deprecated))
    uint64_t vbv_delay;
    __attribute__((deprecated))
    int side_data_only_packets;
    int initial_padding;
    AVRational framerate;






    enum AVPixelFormat sw_pix_fmt;






    AVRational pkt_timebase;






    const AVCodecDescriptor *codec_descriptor;






    int64_t pts_correction_num_faulty_pts;
    int64_t pts_correction_num_faulty_dts;
    int64_t pts_correction_last_pts;
    int64_t pts_correction_last_dts;






    char *sub_charenc;







    int sub_charenc_mode;
    int skip_alpha;






    int seek_preroll;





    __attribute__((deprecated))
    int debug_mv;
    uint16_t *chroma_intra_matrix;







    uint8_t *dump_separator;







    char *codec_whitelist;






    unsigned properties;
    AVPacketSideData *coded_side_data;
    int nb_coded_side_data;
    AVBufferRef *hw_frames_ctx;






    int sub_text_format;
    int trailing_padding;







    int64_t max_pixels;
    AVBufferRef *hw_device_ctx;
    int hwaccel_flags;
    int apply_cropping;
    int extra_hw_frames;







    int discard_damaged_percentage;







    int64_t max_samples;
    int export_side_data;
    int (*get_encode_buffer)(struct AVCodecContext *s, AVPacket *pkt, int flags);
} AVCodecContext;






__attribute__((deprecated))
AVRational av_codec_get_pkt_timebase (const AVCodecContext *avctx);
__attribute__((deprecated))
void av_codec_set_pkt_timebase (AVCodecContext *avctx, AVRational val);

__attribute__((deprecated))
const AVCodecDescriptor *av_codec_get_codec_descriptor(const AVCodecContext *avctx);
__attribute__((deprecated))
void av_codec_set_codec_descriptor(AVCodecContext *avctx, const AVCodecDescriptor *desc);

__attribute__((deprecated))
unsigned av_codec_get_codec_properties(const AVCodecContext *avctx);

__attribute__((deprecated))
int av_codec_get_lowres(const AVCodecContext *avctx);
__attribute__((deprecated))
void av_codec_set_lowres(AVCodecContext *avctx, int val);

__attribute__((deprecated))
int av_codec_get_seek_preroll(const AVCodecContext *avctx);
__attribute__((deprecated))
void av_codec_set_seek_preroll(AVCodecContext *avctx, int val);

__attribute__((deprecated))
uint16_t *av_codec_get_chroma_intra_matrix(const AVCodecContext *avctx);
__attribute__((deprecated))
void av_codec_set_chroma_intra_matrix(AVCodecContext *avctx, uint16_t *val);


struct AVSubtitle;


__attribute__((deprecated))
int av_codec_get_max_lowres(const AVCodec *codec);


struct MpegEncContext;
typedef struct AVHWAccel {





    const char *name;






    enum AVMediaType type;






    enum AVCodecID id;






    enum AVPixelFormat pix_fmt;





    int capabilities;
    int (*alloc_frame)(AVCodecContext *avctx, AVFrame *frame);
    int (*start_frame)(AVCodecContext *avctx, const uint8_t *buf, uint32_t buf_size);
    int (*decode_params)(AVCodecContext *avctx, int type, const uint8_t *buf, uint32_t buf_size);
    int (*decode_slice)(AVCodecContext *avctx, const uint8_t *buf, uint32_t buf_size);
    int (*end_frame)(AVCodecContext *avctx);
    int frame_priv_data_size;
    void (*decode_mb)(struct MpegEncContext *s);
    int (*init)(AVCodecContext *avctx);







    int (*uninit)(AVCodecContext *avctx);





    int priv_data_size;




    int caps_internal;
    int (*frame_params)(AVCodecContext *avctx, AVBufferRef *hw_frames_ctx);
} AVHWAccel;
typedef struct AVPicture {
    __attribute__((deprecated))
    uint8_t *data[8];
    __attribute__((deprecated))
    int linesize[8];
} AVPicture;






enum AVSubtitleType {
    SUBTITLE_NONE,

    SUBTITLE_BITMAP,





    SUBTITLE_TEXT,





    SUBTITLE_ASS,
};



typedef struct AVSubtitleRect {
    int x;
    int y;
    int w;
    int h;
    int nb_colors;





    __attribute__((deprecated))
    AVPicture pict;





    uint8_t *data[4];
    int linesize[4];

    enum AVSubtitleType type;

    char *text;






    char *ass;

    int flags;
} AVSubtitleRect;

typedef struct AVSubtitle {
    uint16_t format;
    uint32_t start_display_time;
    uint32_t end_display_time;
    unsigned num_rects;
    AVSubtitleRect **rects;
    int64_t pts;
} AVSubtitle;







__attribute__((deprecated))
AVCodec *av_codec_next(const AVCodec *c);





unsigned avcodec_version(void);




const char *avcodec_configuration(void);




const char *avcodec_license(void);





__attribute__((deprecated))
void avcodec_register(AVCodec *codec);




__attribute__((deprecated))
void avcodec_register_all(void);
AVCodecContext *avcodec_alloc_context3(const AVCodec *codec);





void avcodec_free_context(AVCodecContext **avctx);







int avcodec_get_context_defaults3(AVCodecContext *s, const AVCodec *codec);
const AVClass *avcodec_get_class(void);





__attribute__((deprecated))
const AVClass *avcodec_get_frame_class(void);
const AVClass *avcodec_get_subtitle_rect_class(void);
__attribute__((deprecated))
int avcodec_copy_context(AVCodecContext *dest, const AVCodecContext *src);
int avcodec_parameters_from_context(AVCodecParameters *par,
                                    const AVCodecContext *codec);
int avcodec_parameters_to_context(AVCodecContext *codec,
                                  const AVCodecParameters *par);
int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
int avcodec_close(AVCodecContext *avctx);






void avsubtitle_free(AVSubtitle *sub);
int avcodec_default_get_buffer2(AVCodecContext *s, AVFrame *frame, int flags);






int avcodec_default_get_encode_buffer(AVCodecContext *s, AVPacket *pkt, int flags);
void avcodec_align_dimensions(AVCodecContext *s, int *width, int *height);
void avcodec_align_dimensions2(AVCodecContext *s, int *width, int *height,
                               int linesize_align[8]);
int avcodec_enum_to_chroma_pos(int *xpos, int *ypos, enum AVChromaLocation pos);
enum AVChromaLocation avcodec_chroma_pos_to_enum(int xpos, int ypos);
__attribute__((deprecated))
int avcodec_decode_audio4(AVCodecContext *avctx, AVFrame *frame,
                          int *got_frame_ptr, const AVPacket *avpkt);
__attribute__((deprecated))
int avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture,
                         int *got_picture_ptr,
                         const AVPacket *avpkt);
int avcodec_decode_subtitle2(AVCodecContext *avctx, AVSubtitle *sub,
                            int *got_sub_ptr,
                            AVPacket *avpkt);
int avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt);
int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame);
int avcodec_send_frame(AVCodecContext *avctx, const AVFrame *frame);
int avcodec_receive_packet(AVCodecContext *avctx, AVPacket *avpkt);
int avcodec_get_hw_frames_parameters(AVCodecContext *avctx,
                                     AVBufferRef *device_ref,
                                     enum AVPixelFormat hw_pix_fmt,
                                     AVBufferRef **out_frames_ref);
enum AVPictureStructure {
    AV_PICTURE_STRUCTURE_UNKNOWN,
    AV_PICTURE_STRUCTURE_TOP_FIELD,
    AV_PICTURE_STRUCTURE_BOTTOM_FIELD,
    AV_PICTURE_STRUCTURE_FRAME,
};

typedef struct AVCodecParserContext {
    void *priv_data;
    struct AVCodecParser *parser;
    int64_t frame_offset;
    int64_t cur_offset;

    int64_t next_frame_offset;

    int pict_type;
    int repeat_pict;
    int64_t pts;
    int64_t dts;


    int64_t last_pts;
    int64_t last_dts;
    int fetch_timestamp;


    int cur_frame_start_index;
    int64_t cur_frame_offset[4];
    int64_t cur_frame_pts[4];
    int64_t cur_frame_dts[4];

    int flags;






    int64_t offset;
    int64_t cur_frame_end[4];







    int key_frame;





    __attribute__((deprecated))
    int64_t convergence_duration;
    int dts_sync_point;
    int dts_ref_dts_delta;
    int pts_dts_delta;






    int64_t cur_frame_pos[4];




    int64_t pos;




    int64_t last_pos;






    int duration;

    enum AVFieldOrder field_order;
    enum AVPictureStructure picture_structure;







    int output_picture_number;




    int width;
    int height;




    int coded_width;
    int coded_height;
    int format;
} AVCodecParserContext;

typedef struct AVCodecParser {
    int codec_ids[5];
    int priv_data_size;
    int (*parser_init)(AVCodecParserContext *s);


    int (*parser_parse)(AVCodecParserContext *s,
                        AVCodecContext *avctx,
                        const uint8_t **poutbuf, int *poutbuf_size,
                        const uint8_t *buf, int buf_size);
    void (*parser_close)(AVCodecParserContext *s);
    int (*split)(AVCodecContext *avctx, const uint8_t *buf, int buf_size);

    __attribute__((deprecated))
    struct AVCodecParser *next;

} AVCodecParser;
const AVCodecParser *av_parser_iterate(void **opaque);


__attribute__((deprecated))
AVCodecParser *av_parser_next(const AVCodecParser *c);

__attribute__((deprecated))
void av_register_codec_parser(AVCodecParser *parser);

AVCodecParserContext *av_parser_init(int codec_id);
int av_parser_parse2(AVCodecParserContext *s,
                     AVCodecContext *avctx,
                     uint8_t **poutbuf, int *poutbuf_size,
                     const uint8_t *buf, int buf_size,
                     int64_t pts, int64_t dts,
                     int64_t pos);







__attribute__((deprecated))
int av_parser_change(AVCodecParserContext *s,
                     AVCodecContext *avctx,
                     uint8_t **poutbuf, int *poutbuf_size,
                     const uint8_t *buf, int buf_size, int keyframe);

void av_parser_close(AVCodecParserContext *s);
__attribute__((deprecated))
int avcodec_encode_audio2(AVCodecContext *avctx, AVPacket *avpkt,
                          const AVFrame *frame, int *got_packet_ptr);
__attribute__((deprecated))
int avcodec_encode_video2(AVCodecContext *avctx, AVPacket *avpkt,
                          const AVFrame *frame, int *got_packet_ptr);


int avcodec_encode_subtitle(AVCodecContext *avctx, uint8_t *buf, int buf_size,
                            const AVSubtitle *sub);
__attribute__((deprecated))
int avpicture_alloc(AVPicture *picture, enum AVPixelFormat pix_fmt, int width, int height);




__attribute__((deprecated))
void avpicture_free(AVPicture *picture);




__attribute__((deprecated))
int avpicture_fill(AVPicture *picture, const uint8_t *ptr,
                   enum AVPixelFormat pix_fmt, int width, int height);




__attribute__((deprecated))
int avpicture_layout(const AVPicture *src, enum AVPixelFormat pix_fmt,
                     int width, int height,
                     unsigned char *dest, int dest_size);




__attribute__((deprecated))
int avpicture_get_size(enum AVPixelFormat pix_fmt, int width, int height);




__attribute__((deprecated))
void av_picture_copy(AVPicture *dst, const AVPicture *src,
                     enum AVPixelFormat pix_fmt, int width, int height);




__attribute__((deprecated))
int av_picture_crop(AVPicture *dst, const AVPicture *src,
                    enum AVPixelFormat pix_fmt, int top_band, int left_band);




__attribute__((deprecated))
int av_picture_pad(AVPicture *dst, const AVPicture *src, int height, int width, enum AVPixelFormat pix_fmt,
            int padtop, int padbottom, int padleft, int padright, int *color);
__attribute__((deprecated))
void avcodec_get_chroma_sub_sample(enum AVPixelFormat pix_fmt, int *h_shift, int *v_shift);







unsigned int avcodec_pix_fmt_to_codec_tag(enum AVPixelFormat pix_fmt);
enum AVPixelFormat avcodec_find_best_pix_fmt_of_list(const enum AVPixelFormat *pix_fmt_list,
                                            enum AVPixelFormat src_pix_fmt,
                                            int has_alpha, int *loss_ptr);





__attribute__((deprecated))
int avcodec_get_pix_fmt_loss(enum AVPixelFormat dst_pix_fmt, enum AVPixelFormat src_pix_fmt,
                             int has_alpha);



__attribute__((deprecated))
enum AVPixelFormat avcodec_find_best_pix_fmt_of_2(enum AVPixelFormat dst_pix_fmt1, enum AVPixelFormat dst_pix_fmt2,
                                            enum AVPixelFormat src_pix_fmt, int has_alpha, int *loss_ptr);

__attribute__((deprecated))
enum AVPixelFormat avcodec_find_best_pix_fmt2(enum AVPixelFormat dst_pix_fmt1, enum AVPixelFormat dst_pix_fmt2,
                                            enum AVPixelFormat src_pix_fmt, int has_alpha, int *loss_ptr);


enum AVPixelFormat avcodec_default_get_format(struct AVCodecContext *s, const enum AVPixelFormat * fmt);
__attribute__((deprecated))
size_t av_get_codec_tag_string(char *buf, size_t buf_size, unsigned int codec_tag);


void avcodec_string(char *buf, int buf_size, AVCodecContext *enc, int encode);
const char *av_get_profile_name(const AVCodec *codec, int profile);
const char *avcodec_profile_name(enum AVCodecID codec_id, int profile);

int avcodec_default_execute(AVCodecContext *c, int (*func)(AVCodecContext *c2, void *arg2),void *arg, int *ret, int count, int size);
int avcodec_default_execute2(AVCodecContext *c, int (*func)(AVCodecContext *c2, void *arg2, int, int),void *arg, int *ret, int count);
int avcodec_fill_audio_frame(AVFrame *frame, int nb_channels,
                             enum AVSampleFormat sample_fmt, const uint8_t *buf,
                             int buf_size, int align);
void avcodec_flush_buffers(AVCodecContext *avctx);







int av_get_bits_per_sample(enum AVCodecID codec_id);







enum AVCodecID av_get_pcm_codec(enum AVSampleFormat fmt, int be);
int av_get_exact_bits_per_sample(enum AVCodecID codec_id);
int av_get_audio_frame_duration(AVCodecContext *avctx, int frame_bytes);





int av_get_audio_frame_duration2(AVCodecParameters *par, int frame_bytes);


typedef struct AVBitStreamFilterContext {
    void *priv_data;
    const struct AVBitStreamFilter *filter;
    AVCodecParserContext *parser;
    struct AVBitStreamFilterContext *next;




    char *args;
} AVBitStreamFilterContext;





__attribute__((deprecated))
void av_register_bitstream_filter(AVBitStreamFilter *bsf);





__attribute__((deprecated))
AVBitStreamFilterContext *av_bitstream_filter_init(const char *name);





__attribute__((deprecated))
int av_bitstream_filter_filter(AVBitStreamFilterContext *bsfc,
                               AVCodecContext *avctx, const char *args,
                               uint8_t **poutbuf, int *poutbuf_size,
                               const uint8_t *buf, int buf_size, int keyframe);





__attribute__((deprecated))
void av_bitstream_filter_close(AVBitStreamFilterContext *bsf);





__attribute__((deprecated))
const AVBitStreamFilter *av_bitstream_filter_next(const AVBitStreamFilter *f);



__attribute__((deprecated))
const AVBitStreamFilter *av_bsf_next(void **opaque);
void av_fast_padded_malloc(void *ptr, unsigned int *size, size_t min_size);





void av_fast_padded_mallocz(void *ptr, unsigned int *size, size_t min_size);
unsigned int av_xiphlacing(unsigned char *s, unsigned int v);







__attribute__((deprecated))
void av_register_hwaccel(AVHWAccel *hwaccel);
__attribute__((deprecated))
AVHWAccel *av_hwaccel_next(const AVHWAccel *hwaccel);
enum AVLockOp {
  AV_LOCK_CREATE,
  AV_LOCK_OBTAIN,
  AV_LOCK_RELEASE,
  AV_LOCK_DESTROY,
};
__attribute__((deprecated))
int av_lockmgr_register(int (*cb)(void **mutex, enum AVLockOp op));






int avcodec_is_open(AVCodecContext *s);
AVCPBProperties *av_cpb_properties_alloc(size_t *size);
typedef struct AVFifoBuffer {
    uint8_t *buffer;
    uint8_t *rptr, *wptr, *end;
    uint32_t rndx, wndx;
} AVFifoBuffer;






AVFifoBuffer *av_fifo_alloc(unsigned int size);







AVFifoBuffer *av_fifo_alloc_array(size_t nmemb, size_t size);





void av_fifo_free(AVFifoBuffer *f);





void av_fifo_freep(AVFifoBuffer **f);





void av_fifo_reset(AVFifoBuffer *f);







int av_fifo_size(const AVFifoBuffer *f);







int av_fifo_space(const AVFifoBuffer *f);
int av_fifo_generic_peek_at(AVFifoBuffer *f, void *dest, int offset, int buf_size, void (*func)(void*, void*, int));
int av_fifo_generic_peek(AVFifoBuffer *f, void *dest, int buf_size, void (*func)(void*, void*, int));
int av_fifo_generic_read(AVFifoBuffer *f, void *dest, int buf_size, void (*func)(void*, void*, int));
int av_fifo_generic_write(AVFifoBuffer *f, void *src, int size, int (*func)(void*, void*, int));
int av_fifo_realloc2(AVFifoBuffer *f, unsigned int size);
int av_fifo_grow(AVFifoBuffer *f, unsigned int additional_space);






void av_fifo_drain(AVFifoBuffer *f, int size);
static inline uint8_t *av_fifo_peek2(const AVFifoBuffer *f, int offs)
{
    uint8_t *ptr = f->rptr + offs;
    if (ptr >= f->end)
        ptr = f->buffer + (ptr - f->end);
    else if (ptr < f->buffer)
        ptr = f->end - (f->buffer - ptr);
    return ptr;
}
typedef struct DecodeSimpleContext {
    AVPacket *in_pkt;
} DecodeSimpleContext;

typedef struct EncodeSimpleContext {
    AVFrame *in_frame;
} EncodeSimpleContext;

typedef struct AVCodecInternal {






    int is_copy;





    int last_audio_frame;


    AVFrame *to_free;


    AVBufferRef *pool;

    void *thread_ctx;

    DecodeSimpleContext ds;
    AVBSFContext *bsf;





    AVPacket *last_pkt_props;
    AVFifoBuffer *pkt_props;




    uint8_t *byte_buffer;
    unsigned int byte_buffer_size;

    void *frame_thread_encoder;

    EncodeSimpleContext es;




    int skip_samples;




    void *hwaccel_priv_data;




    int draining;




    AVPacket *buffer_pkt;
    AVFrame *buffer_frame;
    int draining_done;


    int compat_decode_warned;


    size_t compat_decode_consumed;


    size_t compat_decode_partial_size;
    AVFrame *compat_decode_frame;
    AVPacket *compat_encode_packet;


    int showed_multi_packet_warning;

    int skip_samples_multiplier;


    int nb_draining_errors;


    int changed_frames_dropped;
    int initial_format;
    int initial_width, initial_height;
    int initial_sample_rate;
    int initial_channels;
    uint64_t initial_channel_layout;
} AVCodecInternal;

struct AVCodecDefault {
    const uint8_t *key;
    const uint8_t *value;
};

extern const uint8_t ff_log2_run[41];





int ff_match_2uint16(const uint16_t (*tab)[2], int size, int a, int b);

unsigned int avpriv_toupper4(unsigned int x);

void ff_color_frame(AVFrame *frame, const int color[4]);
int ff_alloc_packet2(AVCodecContext *avctx, AVPacket *avpkt, int64_t size, int64_t min_size);




static __attribute__((always_inline)) inline int64_t ff_samples_to_time_base(AVCodecContext *avctx,
                                                        int64_t samples)
{
    if(samples == ((int64_t)0x8000000000000000UL))
        return ((int64_t)0x8000000000000000UL);
    return av_rescale_q(samples, (AVRational){ 1, avctx->sample_rate },
                        avctx->time_base);
}





static __attribute__((always_inline)) inline float ff_exp2fi(int x) {

    if (-126 <= x && x <= 128)
        return av_int2float((x+127) << 23);

    else if (x > 128)
        return 
              (__builtin_inff ())
                      ;

    else if (x > -150)
        return av_int2float(1 << (x+149));

    else
        return 0;
}






int ff_get_buffer(AVCodecContext *avctx, AVFrame *frame, int flags);






int ff_reget_buffer(AVCodecContext *avctx, AVFrame *frame, int flags);

int ff_thread_can_start_frame(AVCodecContext *avctx);

int avpriv_h264_has_num_reorder_frames(AVCodecContext *avctx);

const uint8_t *avpriv_find_start_code(const uint8_t *p,
                                      const uint8_t *end,
                                      uint32_t *state);

int avpriv_codec_get_cap_skip_frame_fill_param(const AVCodec *codec);





int ff_set_dimensions(AVCodecContext *s, int width, int height);





int ff_set_sar(AVCodecContext *avctx, AVRational sar);




int ff_side_data_update_matrix_encoding(AVFrame *frame,
                                        enum AVMatrixEncoding matrix_encoding);
int ff_get_format(AVCodecContext *avctx, const enum AVPixelFormat *fmt);




AVCPBProperties *ff_add_cpb_side_data(AVCodecContext *avctx);
int ff_alloc_timecode_sei(const AVFrame *frame, AVRational rate, size_t prefix_len,
                     void **data, size_t *sei_size);





int64_t ff_guess_coded_bitrate(AVCodecContext *avctx);
int ff_int_from_list_or_default(void *ctx, const char * val_name, int val,
                                const int * array_valid_values, int default_value);

void ff_dvdsub_parse_palette(uint32_t *palette, const char *p);
extern const uint8_t ff_reverse[256];




extern const uint32_t ff_inverse[257];
extern const uint8_t ff_sqrt_tab[256];
extern const uint8_t ff_crop_tab[256 + 2 * 1024];
extern const uint8_t ff_zigzag_direct[64];
extern const uint8_t ff_zigzag_scan[16+1];
static __attribute__((always_inline)) inline int MULH(int a, int b){
    return ((int64_t)(a) * (int64_t)(b)) >> 32;
}



static __attribute__((always_inline)) inline unsigned UMULH(unsigned a, unsigned b){
    return ((uint64_t)(a) * (uint64_t)(b))>>32;
}
static inline __attribute__((const)) int mid_pred(int a, int b, int c)
{
    if(a>b){
        if(c>b){
            if(c>a) b=a;
            else b=c;
        }
    }else{
        if(b>c){
            if(c>a) b=c;
            else b=a;
        }
    }
    return b;
}




static inline __attribute__((const)) int median4(int a, int b, int c, int d)
{
    if (a < b) {
        if (c < d) return (((b) > (d) ? (d) : (b)) + ((a) > (c) ? (a) : (c))) / 2;
        else return (((b) > (c) ? (c) : (b)) + ((a) > (d) ? (a) : (d))) / 2;
    } else {
        if (c < d) return (((a) > (d) ? (d) : (a)) + ((b) > (c) ? (b) : (c))) / 2;
        else return (((a) > (c) ? (c) : (a)) + ((b) > (d) ? (b) : (d))) / 2;
    }
}



static inline __attribute__((const)) int sign_extend(int val, unsigned bits)
{
    unsigned shift = 8 * sizeof(int) - bits;
    union { unsigned u; int s; } v = { (unsigned) val << shift };
    return v.s >> shift;
}



static inline __attribute__((const)) unsigned zero_extend(unsigned val, unsigned bits)
{
    return (val << ((8 * sizeof(int)) - bits)) >> ((8 * sizeof(int)) - bits);
}
static inline __attribute__((const)) unsigned int ff_sqrt(unsigned int a)
{
    unsigned int b;

    if (a < 255) return (ff_sqrt_tab[a + 1] - 1) >> 4;
    else if (a < (1 << 12)) b = ff_sqrt_tab[a >> 4] >> 2;

    else if (a < (1 << 14)) b = ff_sqrt_tab[a >> 6] >> 1;
    else if (a < (1 << 16)) b = ff_sqrt_tab[a >> 8] ;

    else {
        int s = av_log2_16bit(a >> 16) >> 1;
        unsigned int c = a >> (s + 2);
        b = ff_sqrt_tab[c >> (s + 8)];
        b = ((uint32_t)((((uint64_t)c) * ff_inverse[b]) >> 32)) + (b << s);
    }

    return b - (a < b * b);
}


static inline __attribute__((const)) float ff_sqrf(float a)
{
    return a*a;
}

static inline int8_t ff_u8_to_s8(uint8_t a)
{
    union {
        uint8_t u8;
        int8_t s8;
    } b;
    b.u8 = a;
    return b.s8;
}

static __attribute__((always_inline)) inline uint32_t bitswap_32(uint32_t x)
{
    return (uint32_t)ff_reverse[ x & 0xFF] << 24 |
           (uint32_t)ff_reverse[(x >> 8) & 0xFF] << 16 |
           (uint32_t)ff_reverse[(x >> 16) & 0xFF] << 8 |
           (uint32_t)ff_reverse[ x >> 24];
}


static __attribute__((always_inline)) inline __attribute__((const)) uint16_t av_bswap16(uint16_t x)
{
    x= (x>>8) | (x<<8);
    return x;
}



static __attribute__((always_inline)) inline __attribute__((const)) uint32_t av_bswap32(uint32_t x)
{
    return ((((x) << 8 & 0xff00) | ((x) >> 8 & 0x00ff)) << 16 | ((((x) >> 16) << 8 & 0xff00) | (((x) >> 16) >> 8 & 0x00ff)));
}



static inline uint64_t __attribute__((const)) av_bswap64(uint64_t x)
{
    return (uint64_t)av_bswap32(x) << 32 | av_bswap32(x >> 32);
}

typedef union {
    uint64_t u64;
    uint32_t u32[2];
    uint16_t u16[4];
    uint8_t u8 [8];
    double f64;
    float f32[2];
} __attribute__((may_alias)) av_alias64;

typedef union {
    uint32_t u32;
    uint16_t u16[2];
    uint8_t u8 [4];
    float f32;
} __attribute__((may_alias)) av_alias32;

typedef union {
    uint16_t u16;
    uint8_t u8 [2];
} __attribute__((may_alias)) av_alias16;
union unaligned_64 { uint64_t l; } __attribute__((packed)) __attribute__((may_alias));
union unaligned_32 { uint32_t l; } __attribute__((packed)) __attribute__((may_alias));
union unaligned_16 { uint16_t l; } __attribute__((packed)) __attribute__((may_alias));




typedef uint32_t BitBuf;



static const int BUF_BITS = 8 * sizeof(BitBuf);

typedef struct PutBitContext {
    BitBuf bit_buf;
    int bit_left;
    uint8_t *buf, *buf_ptr, *buf_end;
    int size_in_bits;
} PutBitContext;







static inline void init_put_bits(PutBitContext *s, uint8_t *buffer,
                                 int buffer_size)
{
    if (buffer_size < 0) {
        buffer_size = 0;
        buffer = 
                     ((void *)0)
                         ;
    }

    s->size_in_bits = 8 * buffer_size;
    s->buf = buffer;
    s->buf_end = s->buf + buffer_size;
    s->buf_ptr = s->buf;
    s->bit_left = BUF_BITS;
    s->bit_buf = 0;
}




static inline int put_bits_count(PutBitContext *s)
{
    return (s->buf_ptr - s->buf) * 8 + BUF_BITS - s->bit_left;
}
static inline void rebase_put_bits(PutBitContext *s, uint8_t *buffer,
                                   int buffer_size)
{
    do { if (!(8*buffer_size >= put_bits_count(s))) { av_log(
   ((void *)0)
   , 0, "Assertion %s failed at %s:%d\n", "8*buffer_size >= put_bits_count(s)", "libavcodec/put_bits.h", 91); abort(); } } while (0);

    s->buf_end = buffer + buffer_size;
    s->buf_ptr = buffer + (s->buf_ptr - s->buf);
    s->buf = buffer;
    s->size_in_bits = 8 * buffer_size;
}




static inline int put_bits_left(PutBitContext* s)
{
    return (s->buf_end - s->buf_ptr) * 8 - BUF_BITS + s->bit_left;
}




static inline void flush_put_bits(PutBitContext *s)
{

    if (s->bit_left < BUF_BITS)
        s->bit_buf <<= s->bit_left;

    while (s->bit_left < BUF_BITS) {
        do { if (!(s->buf_ptr < s->buf_end)) { av_log(
       ((void *)0)
       , 0, "Assertion %s failed at %s:%d\n", "s->buf_ptr < s->buf_end", "libavcodec/put_bits.h", 117); abort(); } } while (0);




        *s->buf_ptr++ = s->bit_buf >> (BUF_BITS - 8);
        s->bit_buf <<= 8;

        s->bit_left += 8;
    }
    s->bit_left = BUF_BITS;
    s->bit_buf = 0;
}

static inline void flush_put_bits_le(PutBitContext *s)
{
    while (s->bit_left < BUF_BITS) {
        do { if (!(s->buf_ptr < s->buf_end)) { av_log(
       ((void *)0)
       , 0, "Assertion %s failed at %s:%d\n", "s->buf_ptr < s->buf_end", "libavcodec/put_bits.h", 134); abort(); } } while (0);
        *s->buf_ptr++ = s->bit_buf;
        s->bit_buf >>= 8;
        s->bit_left += 8;
    }
    s->bit_left = BUF_BITS;
    s->bit_buf = 0;
}


void avpriv_align_put_bits(PutBitContext *s);
void avpriv_copy_bits(PutBitContext *pb, const uint8_t *src, int length);
void ff_put_string(PutBitContext *pb, const char *string,
                       int terminate_string);






void ff_copy_bits(PutBitContext *pb, const uint8_t *src, int length);


static inline void put_bits_no_assert(PutBitContext *s, int n, BitBuf value)
{
    BitBuf bit_buf;
    int bit_left;

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;
    if (n < bit_left) {
        bit_buf = (bit_buf << n) | value;
        bit_left -= n;
    } else {
        bit_buf <<= bit_left;
        bit_buf |= value >> (n - bit_left);
        if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) {
            ((((union unaligned_32 *) (s->buf_ptr))->l) = (av_bswap32(bit_buf)));
            s->buf_ptr += sizeof(BitBuf);
        } else {
            av_log(
                  ((void *)0)
                      , 16, "Internal error, put_bits buffer too small\n");
            ((void)0);
        }
        bit_left += BUF_BITS - n;
        bit_buf = value;
    }


    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}





static inline void put_bits(PutBitContext *s, int n, BitBuf value)
{
    ((void)0);
    put_bits_no_assert(s, n, value);
}

static inline void put_bits_le(PutBitContext *s, int n, BitBuf value)
{
    BitBuf bit_buf;
    int bit_left;

    ((void)0);

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

    bit_buf |= value << (BUF_BITS - bit_left);
    if (n >= bit_left) {
        if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) {
            ((((union unaligned_32 *) (s->buf_ptr))->l) = (bit_buf));
            s->buf_ptr += sizeof(BitBuf);
        } else {
            av_log(
                  ((void *)0)
                      , 16, "Internal error, put_bits buffer too small\n");
            ((void)0);
        }
        bit_buf = value >> bit_left;
        bit_left += BUF_BITS;
    }
    bit_left -= n;

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}

static inline void put_sbits(PutBitContext *pb, int n, int32_t value)
{
    ((void)0);

    put_bits(pb, n, av_mod_uintp2_c(value, n));
}




static void __attribute__((unused)) put_bits32(PutBitContext *s, uint32_t value)
{
    BitBuf bit_buf;
    int bit_left;

    if (BUF_BITS > 32) {
        put_bits_no_assert(s, 32, value);
        return;
    }

    bit_buf = s->bit_buf;
    bit_left = s->bit_left;
    bit_buf = (uint64_t)bit_buf << bit_left;
    bit_buf |= (BitBuf)value >> (BUF_BITS - bit_left);
    if (s->buf_end - s->buf_ptr >= sizeof(BitBuf)) {
        ((((union unaligned_32 *) (s->buf_ptr))->l) = (av_bswap32(bit_buf)));
        s->buf_ptr += sizeof(BitBuf);
    } else {
        av_log(
              ((void *)0)
                  , 16, "Internal error, put_bits buffer too small\n");
        ((void)0);
    }
    bit_buf = value;


    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}




static inline void put_bits64(PutBitContext *s, int n, uint64_t value)
{
    ((void)0);

    if (n < 32)
        put_bits(s, n, value);
    else if (n == 32)
        put_bits32(s, value);
    else if (n < 64) {
        uint32_t lo = value & 0xffffffff;
        uint32_t hi = value >> 32;




        put_bits(s, n - 32, hi);
        put_bits32(s, lo);

    } else {
        uint32_t lo = value & 0xffffffff;
        uint32_t hi = value >> 32;




        put_bits32(s, hi);
        put_bits32(s, lo);


    }
}





static inline uint8_t *put_bits_ptr(PutBitContext *s)
{
    return s->buf_ptr;
}





static inline void skip_put_bytes(PutBitContext *s, int n)
{
    ((void)0);
    ((void)0);
    do { if (!(n <= s->buf_end - s->buf_ptr)) { av_log(
   ((void *)0)
   , 0, "Assertion %s failed at %s:%d\n", "n <= s->buf_end - s->buf_ptr", "libavcodec/put_bits.h", 355); abort(); } } while (0);
    s->buf_ptr += n;
}






static inline void skip_put_bits(PutBitContext *s, int n)
{
    unsigned bits = BUF_BITS - s->bit_left + n;
    s->buf_ptr += sizeof(BitBuf) * (bits / BUF_BITS);
    s->bit_left = BUF_BITS - (bits & (BUF_BITS - 1));
}






static inline void set_put_bits_buffer_size(PutBitContext *s, int size)
{
    do { if (!(size <= 0x7fffffff
   /8 - BUF_BITS)) { av_log(
   ((void *)0)
   , 0, "Assertion %s failed at %s:%d\n", "size <= 0x7fffffff/8 - BUF_BITS", "libavcodec/put_bits.h", 378); abort(); } } while (0);
    s->buf_end = s->buf + size;
    s->size_in_bits = 8*size;
}




static inline void align_put_bits(PutBitContext *s)
{
    put_bits(s, s->bit_left & 7, 0);
}
typedef struct VLC {
    int bits;
    int16_t (*table)[2];
    int table_size, table_allocated;
} VLC;

typedef struct RL_VLC_ELEM {
    int16_t level;
    int8_t len;
    uint8_t run;
} RL_VLC_ELEM;
int ff_init_vlc_sparse(VLC *vlc, int nb_bits, int nb_codes,
                       const void *bits, int bits_wrap, int bits_size,
                       const void *codes, int codes_wrap, int codes_size,
                       const void *symbols, int symbols_wrap, int symbols_size,
                       int flags);
int ff_init_vlc_from_lengths(VLC *vlc, int nb_bits, int nb_codes,
                             const int8_t *lens, int lens_wrap,
                             const void *symbols, int symbols_wrap, int symbols_size,
                             int offset, int flags, void *logctx);

void ff_free_vlc(VLC *vlc);

const uint8_t ff_log2_run[41]={
 0, 0, 0, 0, 1, 1, 1, 1,
 2, 2, 2, 2, 3, 3, 3, 3,
 4, 4, 5, 5, 6, 6, 7, 7,
 8, 9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,
};


void avpriv_align_put_bits(PutBitContext *s)
{
    align_put_bits(s);
}
void avpriv_copy_bits(PutBitContext *pb, const uint8_t *src, int length)
{
    ff_copy_bits(pb, src, length);
}


void ff_put_string(PutBitContext *pb, const char *string, int terminate_string)
{
    while (*string) {
        put_bits(pb, 8, *string);
        string++;
    }
    if (terminate_string)
        put_bits(pb, 8, 0);
}

void ff_copy_bits(PutBitContext *pb, const uint8_t *src, int length)
{
    int words = length >> 4;
    int bits = length & 15;
    int i;

    if (length == 0)
        return;

    do { if (!(length <= put_bits_left(pb))) { av_log(
   ((void *)0)
   , 0, "Assertion %s failed at %s:%d\n", "length <= put_bits_left(pb)", "libavcodec/bitstream.c", 78); abort(); } } while (0);

    if (0 || words < 16 || put_bits_count(pb) & 7) {
        for (i = 0; i < words; i++)
            put_bits(pb, 16, av_bswap16((((const union unaligned_16 *) (src + 2 * i))->l)));
    } else {
        for (i = 0; put_bits_count(pb) & 31; i++)
            put_bits(pb, 8, src[i]);
        flush_put_bits(pb);
        memcpy(put_bits_ptr(pb), src + i, 2 * words - i);
        skip_put_bytes(pb, 2 * words - i);
    }

    put_bits(pb, bits, av_bswap16((((const union unaligned_16 *) (src + 2 * words))->l)) >> (16 - bits));
}
static int alloc_table(VLC *vlc, int size, int use_static)
{
    int index = vlc->table_size;

    vlc->table_size += size;
    if (vlc->table_size > vlc->table_allocated) {
        if (use_static)
            abort();
        vlc->table_allocated += (1 << vlc->bits);
        vlc->table = av_realloc_f(vlc->table, vlc->table_allocated, sizeof(int16_t) * 2);
        if (!vlc->table) {
            vlc->table_allocated = 0;
            vlc->table_size = 0;
            return (-(
                  12
                  ));
        }
        memset(vlc->table + vlc->table_allocated - (1 << vlc->bits), 0, sizeof(int16_t) * 2 << vlc->bits);
    }
    return index;
}



typedef struct VLCcode {
    uint8_t bits;
    int16_t symbol;


    uint32_t code;
} VLCcode;

static int vlc_common_init(VLC *vlc_arg, int nb_bits, int nb_codes,
                           VLC **vlc, VLC *localvlc, VLCcode **buf,
                           int flags)
{
    *vlc = vlc_arg;
    (*vlc)->bits = nb_bits;
    if (flags & 4) {
        do { if (!(nb_codes <= 1500)) { av_log(
       ((void *)0)
       , 0, "Assertion %s failed at %s:%d\n", "nb_codes <= 1500", "libavcodec/bitstream.c", 152); abort(); } } while (0);
        *localvlc = *vlc_arg;
        *vlc = localvlc;
        (*vlc)->table_size = 0;
    } else {
        (*vlc)->table = 
                                 ((void *)0)
                                     ;
        (*vlc)->table_allocated = 0;
        (*vlc)->table_size = 0;
    }
    if (nb_codes > 1500) {
        *buf = av_malloc_array(nb_codes, sizeof(VLCcode));
        if (!*buf)
            return (-(
                  12
                  ));
    }

    return 0;
}

static int compare_vlcspec(const void *a, const void *b)
{
    const VLCcode *sa = a, *sb = b;
    return (sa->code >> 1) - (sb->code >> 1);
}
static int build_table(VLC *vlc, int table_nb_bits, int nb_codes,
                       VLCcode *codes, int flags)
{
    int table_size, table_index, index, code_prefix, symbol, subtable_bits;
    int i, j, k, n, nb, inc;
    uint32_t code;
    volatile int16_t (* volatile table)[2];

    if (table_nb_bits > 30)
       return (-(
             22
             ));
    table_size = 1 << table_nb_bits;
    table_index = alloc_table(vlc, table_size, flags & 4);
    ff_dlog(
           ((void *)0)
               , "new table index=%d size=%d\n", table_index, table_size);
    if (table_index < 0)
        return table_index;
    table = (volatile int16_t (*)[2])&vlc->table[table_index];


    for (i = 0; i < nb_codes; i++) {
        n = codes[i].bits;
        code = codes[i].code;
        symbol = codes[i].symbol;
        ff_dlog(
               ((void *)0)
                   , "i=%d n=%d code=0x%"
                                         "x"
                                               "\n", i, n, code);
        if (n <= table_nb_bits) {

            j = code >> (32 - table_nb_bits);
            nb = 1 << (table_nb_bits - n);
            inc = 1;
            if (flags & 8) {
                j = bitswap_32(code);
                inc = 1 << n;
            }
            for (k = 0; k < nb; k++) {
                int bits = table[j][1];
                int oldsym = table[j][0];
                ff_dlog(
                       ((void *)0)
                           , "%4x: code=%d n=%d\n", j, i, n);
                if ((bits || oldsym) && (bits != n || oldsym != symbol)) {
                    av_log(
                          ((void *)0)
                              , 16, "incorrect codes\n");
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
                table[j][1] = n;
                table[j][0] = symbol;
                j += inc;
            }
        } else {

            n -= table_nb_bits;
            code_prefix = code >> (32 - table_nb_bits);
            subtable_bits = n;
            codes[i].bits = n;
            codes[i].code = code << table_nb_bits;
            for (k = i+1; k < nb_codes; k++) {
                n = codes[k].bits - table_nb_bits;
                if (n <= 0)
                    break;
                code = codes[k].code;
                if (code >> (32 - table_nb_bits) != code_prefix)
                    break;
                codes[k].bits = n;
                codes[k].code = code << table_nb_bits;
                subtable_bits = ((subtable_bits) > (n) ? (subtable_bits) : (n));
            }
            subtable_bits = ((subtable_bits) > (table_nb_bits) ? (table_nb_bits) : (subtable_bits));
            j = (flags & 8) ? bitswap_32(code_prefix) >> (32 - table_nb_bits) : code_prefix;
            table[j][1] = -subtable_bits;
            ff_dlog(
                   ((void *)0)
                       , "%4x: n=%d (subtable)\n",
                    j, codes[i].bits + table_nb_bits);
            index = build_table(vlc, subtable_bits, k-i, codes+i, flags);
            if (index < 0)
                return index;

            table = (volatile int16_t (*)[2])&vlc->table[table_index];
            table[j][0] = index;
            if (table[j][0] != index) {
                avpriv_request_sample(
                                     ((void *)0)
                                         , "strange codes");
                return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
            }
            i = k-1;
        }
    }

    for (i = 0; i < table_size; i++) {
        if (table[i][1] == 0)
            table[i][0] = -1;
    }

    return table_index;
}

static int vlc_common_end(VLC *vlc, int nb_bits, int nb_codes, VLCcode *codes,
                          int flags, VLC *vlc_arg, VLCcode localbuf[1500])
{
    int ret = build_table(vlc, nb_bits, nb_codes, codes, flags);

    if (flags & 4) {
        if (vlc->table_size != vlc->table_allocated &&
            !(flags & ((1 | 4) & ~4)))
            av_log(
                  ((void *)0)
                      , 16, "needed %d had %d\n", vlc->table_size, vlc->table_allocated);
        do { if (!(ret >= 0)) { av_log(
       ((void *)0)
       , 0, "Assertion %s failed at %s:%d\n", "ret >= 0", "libavcodec/bitstream.c", 287); abort(); } } while (0);
        *vlc_arg = *vlc;
    } else {
        if (codes != localbuf)
            av_free(codes);
        if (ret < 0) {
            av_freep(&vlc->table);
            return ret;
        }
    }
    return 0;
}
int ff_init_vlc_sparse(VLC *vlc_arg, int nb_bits, int nb_codes,
                       const void *bits, int bits_wrap, int bits_size,
                       const void *codes, int codes_wrap, int codes_size,
                       const void *symbols, int symbols_wrap, int symbols_size,
                       int flags)
{
    VLCcode localbuf[1500], *buf = localbuf;
    int i, j, ret;
    VLC localvlc, *vlc;

    ret = vlc_common_init(vlc_arg, nb_bits, nb_codes, &vlc, &localvlc,
                          &buf, flags);
    if (ret < 0)
        return ret;

    do { if (!(symbols_size <= 2 || !symbols)) { av_log(
   ((void *)0)
   , 0, "Assertion %s failed at %s:%d\n", "symbols_size <= 2 || !symbols", "libavcodec/bitstream.c", 338); abort(); } } while (0);
    j = 0;
    for (i = 0; i < nb_codes; i++) { unsigned len; { const uint8_t *ptr = (const uint8_t *)bits + i * bits_wrap; switch(bits_size) { case 1: len = *(const uint8_t *)ptr; break; case 2: len = *(const uint16_t *)ptr; break; case 4: default: ((void)0); len = *(const uint32_t *)ptr; break; } }; if (!(len > nb_bits)) continue; if (len > 3*nb_bits || len > 32) { av_log(
   ((void *)0)
   , 16, "Too long VLC (%u) in init_vlc\n", len); if (buf != localbuf) av_free(buf); return (-(
   22
   )); } buf[j].bits = len; { const uint8_t *ptr = (const uint8_t *)codes + i * codes_wrap; switch(codes_size) { case 1: buf[j].code = *(const uint8_t *)ptr; break; case 2: buf[j].code = *(const uint16_t *)ptr; break; case 4: default: ((void)0); buf[j].code = *(const uint32_t *)ptr; break; } }; if (buf[j].code >= (1LL<<buf[j].bits)) { av_log(
   ((void *)0)
   , 16, "Invalid code %"
   "x"
   " for %d in " "init_vlc\n", buf[j].code, i); if (buf != localbuf) av_free(buf); return (-(
   22
   )); } if (flags & 2) buf[j].code = bitswap_32(buf[j].code); else buf[j].code <<= 32 - buf[j].bits; if (symbols) { const uint8_t *ptr = (const uint8_t *)symbols + i * symbols_wrap; switch(symbols_size) { case 1: buf[j].symbol = *(const uint8_t *)ptr; break; case 2: buf[j].symbol = *(const uint16_t *)ptr; break; case 4: default: ((void)0); buf[j].symbol = *(const uint32_t *)ptr; break; } } else buf[j].symbol = i; j++; };

    do { void *stack[64][2]; int sp= 1; stack[0][0] = buf; stack[0][1] = (buf)+(j)-1; while(sp){ struct VLCcode *start= stack[--sp][0]; struct VLCcode *end = stack[ sp][1]; while(start < end){ if(start < end-1) { int checksort=0; struct VLCcode *right = end-2; struct VLCcode *left = start+1; struct VLCcode *mid = start + ((end-start)>>1); if(compare_vlcspec(start, end) > 0) { if(compare_vlcspec( end, mid) > 0) do{struct VLCcode SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else do{struct VLCcode SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); }else{ if(compare_vlcspec(start, mid) > 0) do{struct VLCcode SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else checksort= 1; } if(compare_vlcspec(mid, end) > 0){ do{struct VLCcode SWAP_tmp= *end; *end= *mid; *mid= SWAP_tmp;}while(0); checksort=0; } if(start == end-2) break; do{struct VLCcode SWAP_tmp= *mid; *mid= end[-1]; end[-1]= SWAP_tmp;}while(0); while(left <= right){ while(left<=right && compare_vlcspec(left, end-1) < 0) left++; while(left<=right && compare_vlcspec(right, end-1) > 0) right--; if(left <= right){ do{struct VLCcode SWAP_tmp= *right; *right= *left; *left= SWAP_tmp;}while(0); left++; right--; } } do{struct VLCcode SWAP_tmp= *left; *left= end[-1]; end[-1]= SWAP_tmp;}while(0); if(checksort && (mid == left-1 || mid == left)){ mid= start; while(mid<end && compare_vlcspec(mid, mid+1) <= 0) mid++; if(mid==end) break; } if(end-left < left-start){ stack[sp ][0]= start; stack[sp++][1]= right; start = left+1; }else{ stack[sp ][0]= left+1; stack[sp++][1]= end; end = right; } }else{ if(compare_vlcspec(start, end) > 0) do{struct VLCcode SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); break; } } }} while (0);
    for (i = 0; i < nb_codes; i++) { unsigned len; { const uint8_t *ptr = (const uint8_t *)bits + i * bits_wrap; switch(bits_size) { case 1: len = *(const uint8_t *)ptr; break; case 2: len = *(const uint16_t *)ptr; break; case 4: default: ((void)0); len = *(const uint32_t *)ptr; break; } }; if (!(len && len <= nb_bits)) continue; if (len > 3*nb_bits || len > 32) { av_log(
   ((void *)0)
   , 16, "Too long VLC (%u) in init_vlc\n", len); if (buf != localbuf) av_free(buf); return (-(
   22
   )); } buf[j].bits = len; { const uint8_t *ptr = (const uint8_t *)codes + i * codes_wrap; switch(codes_size) { case 1: buf[j].code = *(const uint8_t *)ptr; break; case 2: buf[j].code = *(const uint16_t *)ptr; break; case 4: default: ((void)0); buf[j].code = *(const uint32_t *)ptr; break; } }; if (buf[j].code >= (1LL<<buf[j].bits)) { av_log(
   ((void *)0)
   , 16, "Invalid code %"
   "x"
   " for %d in " "init_vlc\n", buf[j].code, i); if (buf != localbuf) av_free(buf); return (-(
   22
   )); } if (flags & 2) buf[j].code = bitswap_32(buf[j].code); else buf[j].code <<= 32 - buf[j].bits; if (symbols) { const uint8_t *ptr = (const uint8_t *)symbols + i * symbols_wrap; switch(symbols_size) { case 1: buf[j].symbol = *(const uint8_t *)ptr; break; case 2: buf[j].symbol = *(const uint16_t *)ptr; break; case 4: default: ((void)0); buf[j].symbol = *(const uint32_t *)ptr; break; } } else buf[j].symbol = i; j++; };
    nb_codes = j;

    return vlc_common_end(vlc, nb_bits, nb_codes, buf,
                          flags, vlc_arg, localbuf);
}

int ff_init_vlc_from_lengths(VLC *vlc_arg, int nb_bits, int nb_codes,
                             const int8_t *lens, int lens_wrap,
                             const void *symbols, int symbols_wrap, int symbols_size,
                             int offset, int flags, void *logctx)
{
    VLCcode localbuf[1500], *buf = localbuf;
    VLC localvlc, *vlc;
    uint64_t code;
    int ret, j, len_max = ((32) > (3 * nb_bits) ? (3 * nb_bits) : (32));

    ret = vlc_common_init(vlc_arg, nb_bits, nb_codes, &vlc, &localvlc,
                          &buf, flags);
    if (ret < 0)
        return ret;

    j = code = 0;
    for (int i = 0; i < nb_codes; i++, lens += lens_wrap) {
        int len = *lens;
        if (len > 0) {
            unsigned sym;

            buf[j].bits = len;
            if (symbols)
                { const uint8_t *ptr = (const uint8_t *)symbols + i * symbols_wrap; switch(symbols_size) { case 1: sym = *(const uint8_t *)ptr; break; case 2: sym = *(const uint16_t *)ptr; break; case 4: default: ((void)0); sym = *(const uint32_t *)ptr; break; } }
            else
                sym = i;
            buf[j].symbol = sym + offset;
            buf[j++].code = code;
        } else if (len < 0) {
            len = -len;
        } else
            continue;
        if (len > len_max || code & ((1U << (32 - len)) - 1)) {
            av_log(logctx, 16, "Invalid VLC (length %u)\n", len);
            goto fail;
        }
        code += 1U << (32 - len);
        if (code > 
                  (4294967295U) 
                             + 1ULL) {
            av_log(logctx, 16, "Overdetermined VLC tree\n");
            goto fail;
        }
    }
    return vlc_common_end(vlc, nb_bits, j, buf,
                          flags, vlc_arg, localbuf);
fail:
    if (buf != localbuf)
        av_free(buf);
    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
}

void ff_free_vlc(VLC *vlc)
{
    av_freep(&vlc->table);
}

static inline int ff_fast_malloc(void *ptr, unsigned int *size, size_t min_size, int zero_realloc)
{
    void *val;

    memcpy(&val, ptr, sizeof(val));
    if (min_size <= *size) {
        do { if (!(val || !min_size)) { av_log(
       ((void *)0)
       , 0, "Assertion %s failed at %s:%d\n", "val || !min_size", "./libavutil/mem_internal.h", 145); abort(); } } while (0);
        return 0;
    }
    min_size = ((min_size + min_size / 16 + 32) > (min_size) ? (min_size + min_size / 16 + 32) : (min_size));
    av_freep(ptr);
    val = zero_realloc ? av_mallocz(min_size) : av_malloc(min_size);
    memcpy(ptr, &val, sizeof(val));
    if (!val)
        min_size = 0;
    *size = min_size;
    return 1;
}



typedef float FFTSample;

typedef struct FFTComplex {
    FFTSample re, im;
} FFTComplex;

typedef struct FFTContext FFTContext;






FFTContext *av_fft_init(int nbits, int inverse);




void av_fft_permute(FFTContext *s, FFTComplex *z);





void av_fft_calc(FFTContext *s, FFTComplex *z);

void av_fft_end(FFTContext *s);

FFTContext *av_mdct_init(int nbits, int inverse, double scale);
void av_imdct_calc(FFTContext *s, FFTSample *output, const FFTSample *input);
void av_imdct_half(FFTContext *s, FFTSample *output, const FFTSample *input);
void av_mdct_calc(FFTContext *s, FFTSample *output, const FFTSample *input);
void av_mdct_end(FFTContext *s);



enum RDFTransformType {
    DFT_R2C,
    IDFT_C2R,
    IDFT_R2C,
    DFT_C2R,
};

typedef struct RDFTContext RDFTContext;






RDFTContext *av_rdft_init(int nbits, enum RDFTransformType trans);
void av_rdft_calc(RDFTContext *s, FFTSample *data);
void av_rdft_end(RDFTContext *s);



typedef struct DCTContext DCTContext;

enum DCTTransformType {
    DCT_II = 0,
    DCT_III,
    DCT_I,
    DST_I,
};
DCTContext *av_dct_init(int nbits, enum DCTTransformType type);
void av_dct_calc(DCTContext *s, FFTSample *data);
void av_dct_end (DCTContext *s);



typedef float FFTDouble;
typedef struct FFTDComplex {
    FFTDouble re, im;
} FFTDComplex;



enum fft_permutation_type {
    FF_FFT_PERM_DEFAULT,
    FF_FFT_PERM_SWAP_LSBS,
    FF_FFT_PERM_AVX,
};

enum mdct_permutation_type {
    FF_MDCT_PERM_NONE,
    FF_MDCT_PERM_INTERLEAVE,
};

struct FFTContext {
    int nbits;
    int inverse;
    uint16_t *revtab;
    FFTComplex *tmp_buf;
    int mdct_size;
    int mdct_bits;

    FFTSample *tcos;
    FFTSample *tsin;



    void (*fft_permute)(struct FFTContext *s, FFTComplex *z);




    void (*fft_calc)(struct FFTContext *s, FFTComplex *z);
    void (*imdct_calc)(struct FFTContext *s, FFTSample *output, const FFTSample *input);
    void (*imdct_half)(struct FFTContext *s, FFTSample *output, const FFTSample *input);
    void (*mdct_calc)(struct FFTContext *s, FFTSample *output, const FFTSample *input);
    enum fft_permutation_type fft_permutation;
    enum mdct_permutation_type mdct_permutation;
    uint32_t *revtab32;
};
void ff_init_ff_cos_tabs(int index);





extern FFTSample __attribute__ ((aligned (32))) ff_cos_16[16/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_32[32/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_64[64/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_128[128/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_256[256/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_512[512/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_1024[1024/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_2048[2048/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_4096[4096/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_8192[8192/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_16384[16384/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_32768[32768/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_65536[65536/2];
extern FFTSample __attribute__ ((aligned (32))) ff_cos_131072[131072/2];
extern FFTSample* const ff_cos_tabs[18];
int ff_fft_init(FFTContext *s, int nbits, int inverse);

void ff_fft_init_aarch64(FFTContext *s);
void ff_fft_init_x86(FFTContext *s);
void ff_fft_init_arm(FFTContext *s);
void ff_fft_init_mips(FFTContext *s);
void ff_fft_init_ppc(FFTContext *s);

void ff_fft_end(FFTContext *s);




int ff_mdct_init(FFTContext *s, int nbits, int inverse, double scale);
void ff_mdct_end(FFTContext *s);
void ff_imdct_calc_c(FFTContext *s, FFTSample *output, const FFTSample *input);
void ff_imdct_half_c(FFTContext *s, FFTSample *output, const FFTSample *input);
void ff_mdct_calc_c(FFTContext *s, FFTSample *output, const FFTSample *input);
__attribute__((cold)) int ff_mdct_init(FFTContext *s, int nbits, int inverse, double scale)
{
    int n, n4, i;
    double alpha, theta;
    int tstep;

    memset(s, 0, sizeof(*s));
    n = 1 << nbits;
    s->mdct_bits = nbits;
    s->mdct_size = n;
    n4 = n >> 2;
    s->mdct_permutation = FF_MDCT_PERM_NONE;

    if (ff_fft_init(s, s->mdct_bits - 2, inverse) < 0)
        goto fail;

    s->tcos = av_malloc_array(n/2, sizeof(FFTSample));
    if (!s->tcos)
        goto fail;

    switch (s->mdct_permutation) {
    case FF_MDCT_PERM_NONE:
        s->tsin = s->tcos + n4;
        tstep = 1;
        break;
    case FF_MDCT_PERM_INTERLEAVE:
        s->tsin = s->tcos + 1;
        tstep = 2;
        break;
    default:
        goto fail;
    }

    theta = 1.0 / 8.0 + (scale < 0 ? n4 : 0);
    scale = sqrt(fabs(scale));
    for(i=0;i<n4;i++) {
        alpha = 2 * 
                   3.14159265358979323846 
                        * (i + theta) / n;




        s->tcos[i*tstep] = (-cos(alpha) * scale);
        s->tsin[i*tstep] = (-sin(alpha) * scale);

    }
    return 0;
 fail:
    ff_mdct_end(s);
    return -1;
}







void ff_imdct_half_c(FFTContext *s, FFTSample *output, const FFTSample *input)
{
    int k, n8, n4, n2, n, j;
    const uint16_t *revtab = s->revtab;
    const FFTSample *tcos = s->tcos;
    const FFTSample *tsin = s->tsin;
    const FFTSample *in1, *in2;
    FFTComplex *z = (FFTComplex *)output;

    n = 1 << s->mdct_bits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;


    in1 = input;
    in2 = input + n2 - 1;
    for(k = 0; k < n4; k++) {
        j=revtab[k];
        do { (z[j].re) = (*in2) * (tcos[k]) - (*in1) * (tsin[k]); (z[j].im) = (*in2) * (tsin[k]) + (*in1) * (tcos[k]); } while (0);
        in1 += 2;
        in2 -= 2;
    }
    s->fft_calc(s, z);


    for(k = 0; k < n8; k++) {
        FFTSample r0, i0, r1, i1;
        do { (r0) = (z[n8-k-1].im) * (tsin[n8-k-1]) - (z[n8-k-1].re) * (tcos[n8-k-1]); (i1) = (z[n8-k-1].im) * (tcos[n8-k-1]) + (z[n8-k-1].re) * (tsin[n8-k-1]); } while (0);
        do { (r1) = (z[n8+k ].im) * (tsin[n8+k ]) - (z[n8+k ].re) * (tcos[n8+k ]); (i0) = (z[n8+k ].im) * (tcos[n8+k ]) + (z[n8+k ].re) * (tsin[n8+k ]); } while (0);
        z[n8-k-1].re = r0;
        z[n8-k-1].im = i0;
        z[n8+k ].re = r1;
        z[n8+k ].im = i1;
    }
}






void ff_imdct_calc_c(FFTContext *s, FFTSample *output, const FFTSample *input)
{
    int k;
    int n = 1 << s->mdct_bits;
    int n2 = n >> 1;
    int n4 = n >> 2;

    ff_imdct_half_c(s, output+n4, input);

    for(k = 0; k < n4; k++) {
        output[k] = -output[n2-k-1];
        output[n-k-1] = output[n2+k];
    }
}






void ff_mdct_calc_c(FFTContext *s, FFTSample *out, const FFTSample *input)
{
    int i, j, n, n8, n4, n2, n3;
    FFTDouble re, im;
    const uint16_t *revtab = s->revtab;
    const FFTSample *tcos = s->tcos;
    const FFTSample *tsin = s->tsin;
    FFTComplex *x = (FFTComplex *)out;

    n = 1 << s->mdct_bits;
    n2 = n >> 1;
    n4 = n >> 2;
    n8 = n >> 3;
    n3 = 3 * n4;


    for(i=0;i<n8;i++) {
        re = ((-input[2*i+n3]) + (- input[n3-1-2*i]));
        im = ((-input[n4+2*i]) + (+ input[n4-1-2*i]));
        j = revtab[i];
        do { (x[j].re) = (re) * (-tcos[i]) - (im) * (tsin[i]); (x[j].im) = (re) * (tsin[i]) + (im) * (-tcos[i]); } while (0);

        re = ((input[2*i]) + (- input[n2-1-2*i]));
        im = ((-input[n2+2*i]) + (- input[ n-1-2*i]));
        j = revtab[n8 + i];
        do { (x[j].re) = (re) * (-tcos[n8 + i]) - (im) * (tsin[n8 + i]); (x[j].im) = (re) * (tsin[n8 + i]) + (im) * (-tcos[n8 + i]); } while (0);
    }

    s->fft_calc(s, x);


    for(i=0;i<n8;i++) {
        FFTSample r0, i0, r1, i1;
        do { (i1) = (x[n8-i-1].re) * (-tsin[n8-i-1]) - (x[n8-i-1].im) * (-tcos[n8-i-1]); (r0) = (x[n8-i-1].re) * (-tcos[n8-i-1]) + (x[n8-i-1].im) * (-tsin[n8-i-1]); } while (0);
        do { (i0) = (x[n8+i ].re) * (-tsin[n8+i ]) - (x[n8+i ].im) * (-tcos[n8+i ]); (r1) = (x[n8+i ].re) * (-tcos[n8+i ]) + (x[n8+i ].im) * (-tsin[n8+i ]); } while (0);
        x[n8-i-1].re = r0;
        x[n8-i-1].im = i0;
        x[n8+i ].re = r1;
        x[n8+i ].im = i1;
    }
}

__attribute__((cold)) void ff_mdct_end(FFTContext *s)
{
    av_freep(&s->tcos);
    ff_fft_end(s);
}
int64_t av_gettime(void);
int64_t av_gettime_relative(void);





int av_gettime_relative_is_monotonic(void);
int av_usleep(unsigned usec);
 
FFTSample __attribute__ ((aligned (32))) ff_cos_16[16/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_32[32/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_64[64/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_128[128/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_256[256/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_512[512/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_1024[1024/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_2048[2048/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_4096[4096/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_8192[8192/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_16384[16384/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_32768[32768/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_65536[65536/2];
 FFTSample __attribute__ ((aligned (32))) ff_cos_131072[131072/2];

static __attribute__((cold)) void init_ff_cos_tabs(int index)
{
    int i;
    int m = 1<<index;
    double freq = 2*
                   3.14159265358979323846
                       /m;
    FFTSample *tab = ff_cos_tabs[index];
    for(i=0; i<=m/4; i++)
        tab[i] = (cos(i*freq));
    for(i=1; i<m/4; i++)
        tab[m/2-i] = tab[i];
}

typedef struct CosTabsInitOnce {
    void (*func)(void);
    pthread_once_t control;
} CosTabsInitOnce;







static __attribute__((cold)) void init_ff_cos_tabs_16 (void){ init_ff_cos_tabs(4); }
static __attribute__((cold)) void init_ff_cos_tabs_32 (void){ init_ff_cos_tabs(5); }
static __attribute__((cold)) void init_ff_cos_tabs_64 (void){ init_ff_cos_tabs(6); }
static __attribute__((cold)) void init_ff_cos_tabs_128 (void){ init_ff_cos_tabs(7); }
static __attribute__((cold)) void init_ff_cos_tabs_256 (void){ init_ff_cos_tabs(8); }
static __attribute__((cold)) void init_ff_cos_tabs_512 (void){ init_ff_cos_tabs(9); }
static __attribute__((cold)) void init_ff_cos_tabs_1024 (void){ init_ff_cos_tabs(10); }
static __attribute__((cold)) void init_ff_cos_tabs_2048 (void){ init_ff_cos_tabs(11); }
static __attribute__((cold)) void init_ff_cos_tabs_4096 (void){ init_ff_cos_tabs(12); }
static __attribute__((cold)) void init_ff_cos_tabs_8192 (void){ init_ff_cos_tabs(13); }
static __attribute__((cold)) void init_ff_cos_tabs_16384 (void){ init_ff_cos_tabs(14); }
static __attribute__((cold)) void init_ff_cos_tabs_32768 (void){ init_ff_cos_tabs(15); }
static __attribute__((cold)) void init_ff_cos_tabs_65536 (void){ init_ff_cos_tabs(16); }
static __attribute__((cold)) void init_ff_cos_tabs_131072 (void){ init_ff_cos_tabs(17); }

static CosTabsInitOnce cos_tabs_init_once[] = {
    { 
     ((void *)0) 
          },
    { 
     ((void *)0) 
          },
    { 
     ((void *)0) 
          },
    { 
     ((void *)0) 
          },
    { init_ff_cos_tabs_16, 
                          0 
                                       },
    { init_ff_cos_tabs_32, 
                          0 
                                       },
    { init_ff_cos_tabs_64, 
                          0 
                                       },
    { init_ff_cos_tabs_128, 
                           0 
                                        },
    { init_ff_cos_tabs_256, 
                           0 
                                        },
    { init_ff_cos_tabs_512, 
                           0 
                                        },
    { init_ff_cos_tabs_1024, 
                            0 
                                         },
    { init_ff_cos_tabs_2048, 
                            0 
                                         },
    { init_ff_cos_tabs_4096, 
                            0 
                                         },
    { init_ff_cos_tabs_8192, 
                            0 
                                         },
    { init_ff_cos_tabs_16384, 
                             0 
                                          },
    { init_ff_cos_tabs_32768, 
                             0 
                                          },
    { init_ff_cos_tabs_65536, 
                             0 
                                          },
    { init_ff_cos_tabs_131072, 
                              0 
                                           },
};

__attribute__((cold)) void ff_init_ff_cos_tabs(int index)
{
    pthread_once(&cos_tabs_init_once[index].control, cos_tabs_init_once[index].func);
}

 FFTSample * const ff_cos_tabs[] = {
    
   ((void *)0)
       , 
         ((void *)0)
             , 
               ((void *)0)
                   , 
                     ((void *)0)
                         ,
    ff_cos_16,
    ff_cos_32,
    ff_cos_64,
    ff_cos_128,
    ff_cos_256,
    ff_cos_512,
    ff_cos_1024,
    ff_cos_2048,
    ff_cos_4096,
    ff_cos_8192,
    ff_cos_16384,
    ff_cos_32768,
    ff_cos_65536,
    ff_cos_131072,
};



static void fft_permute_c(FFTContext *s, FFTComplex *z);
static void fft_calc_c(FFTContext *s, FFTComplex *z);

static int split_radix_permutation(int i, int n, int inverse)
{
    int m;
    if(n <= 2) return i&1;
    m = n >> 1;
    if(!(i&m)) return split_radix_permutation(i, m, inverse)*2;
    m >>= 1;
    if(inverse == !(i&m)) return split_radix_permutation(i, m, inverse)*4 + 1;
    else return split_radix_permutation(i, m, inverse)*4 - 1;
}


static const int avx_tab[] = {
    0, 4, 1, 5, 8, 12, 9, 13, 2, 6, 3, 7, 10, 14, 11, 15
};

static int is_second_half_of_fft32(int i, int n)
{
    if (n <= 32)
        return i >= 16;
    else if (i < n/2)
        return is_second_half_of_fft32(i, n/2);
    else if (i < 3*n/4)
        return is_second_half_of_fft32(i - n/2, n/4);
    else
        return is_second_half_of_fft32(i - 3*n/4, n/4);
}

static __attribute__((cold)) void fft_perm_avx(FFTContext *s)
{
    int i;
    int n = 1 << s->nbits;

    for (i = 0; i < n; i += 16) {
        int k;
        if (is_second_half_of_fft32(i, n)) {
            for (k = 0; k < 16; k++)
                s->revtab[-split_radix_permutation(i + k, n, s->inverse) & (n - 1)] =
                    i + avx_tab[k];

        } else {
            for (k = 0; k < 16; k++) {
                int j = i + k;
                j = (j & ~7) | ((j >> 1) & 3) | ((j << 2) & 4);
                s->revtab[-split_radix_permutation(i + k, n, s->inverse) & (n - 1)] = j;
            }
        }
    }
}

__attribute__((cold)) int ff_fft_init(FFTContext *s, int nbits, int inverse)
{
    int i, j, n;

    s->revtab = 
               ((void *)0)
                   ;
    s->revtab32 = 
                 ((void *)0)
                     ;

    if (nbits < 2 || nbits > 17)
        goto fail;
    s->nbits = nbits;
    n = 1 << nbits;

    if (nbits <= 16) {
        s->revtab = av_malloc(n * sizeof(uint16_t));
        if (!s->revtab)
            goto fail;
    } else {
        s->revtab32 = av_malloc(n * sizeof(uint32_t));
        if (!s->revtab32)
            goto fail;
    }
    s->tmp_buf = av_malloc(n * sizeof(FFTComplex));
    if (!s->tmp_buf)
        goto fail;
    s->inverse = inverse;
    s->fft_permutation = FF_FFT_PERM_DEFAULT;

    s->fft_permute = fft_permute_c;
    s->fft_calc = fft_calc_c;

    s->imdct_calc = ff_imdct_calc_c;
    s->imdct_half = ff_imdct_half_c;
    s->mdct_calc = ff_mdct_calc_c;






    if (0) ff_fft_init_aarch64(s);
    if (0) ff_fft_init_arm(s);
    if (0) ff_fft_init_ppc(s);
    if (0) ff_fft_init_x86(s);
    if (0) ff_fft_init_mips(s);

    for(j=4; j<=nbits; j++) {
        ff_init_ff_cos_tabs(j);
    }



    if (0 && 1 && s->fft_permutation == FF_FFT_PERM_AVX) {
        fft_perm_avx(s);
    } else {
    if (s->revtab)
        do { if (s->fft_permutation == FF_FFT_PERM_SWAP_LSBS) { do { for(i = 0; i < n; i++) { int k; j = i; j = (j & ~3) | ((j >> 1) & 1) | ((j << 1) & 2); k = -split_radix_permutation(i, n, s->inverse) & (n - 1); s->revtab[k] = j; } } while(0); } else { do { for(i = 0; i < n; i++) { int k; j = i; k = -split_radix_permutation(i, n, s->inverse) & (n - 1); s->revtab[k] = j; } } while(0); }} while(0);
    if (s->revtab32)
        do { if (s->fft_permutation == FF_FFT_PERM_SWAP_LSBS) { do { for(i = 0; i < n; i++) { int k; j = i; j = (j & ~3) | ((j >> 1) & 1) | ((j << 1) & 2); k = -split_radix_permutation(i, n, s->inverse) & (n - 1); s->revtab32[k] = j; } } while(0); } else { do { for(i = 0; i < n; i++) { int k; j = i; k = -split_radix_permutation(i, n, s->inverse) & (n - 1); s->revtab32[k] = j; } } while(0); }} while(0);




    }

    return 0;
 fail:
    av_freep(&s->revtab);
    av_freep(&s->revtab32);
    av_freep(&s->tmp_buf);
    return -1;
}

static void fft_permute_c(FFTContext *s, FFTComplex *z)
{
    int j, np;
    const uint16_t *revtab = s->revtab;
    const uint32_t *revtab32 = s->revtab32;
    np = 1 << s->nbits;

    if (revtab) {
        for(j=0;j<np;j++) s->tmp_buf[revtab[j]] = z[j];
    } else
        for(j=0;j<np;j++) s->tmp_buf[revtab32[j]] = z[j];

    memcpy(z, s->tmp_buf, np * sizeof(FFTComplex));
}

__attribute__((cold)) void ff_fft_end(FFTContext *s)
{
    av_freep(&s->revtab);
    av_freep(&s->revtab32);
    av_freep(&s->tmp_buf);
}
static void pass(FFTComplex *z, const FFTSample *wre, unsigned int n){ FFTDouble t1, t2, t3, t4, t5, t6; int o1 = 2*n; int o2 = 4*n; int o3 = 6*n; const FFTSample *wim = wre+o1; n--; { t1 = z[o2].re; t2 = z[o2].im; t5 = z[o3].re; t6 = z[o3].im; { do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2].re = z[0].re - t5; z[0].re = z[0].re + t5; } while (0); do { z[o3].im = z[o1].im - t3; z[o1].im = z[o1].im + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3].re = z[o1].re - t4; z[o1].re = z[o1].re + t4; } while (0); do { z[o2].im = z[0].im - t6; z[0].im = z[0].im + t6; } while (0);}}; { do { (t1) = (z[o2+1].re) * (wre[1]) - (z[o2+1].im) * (-wim[-1]); (t2) = (z[o2+1].re) * (-wim[-1]) + (z[o2+1].im) * (wre[1]); } while (0); do { (t5) = (z[o3+1].re) * (wre[1]) - (z[o3+1].im) * (wim[-1]); (t6) = (z[o3+1].re) * (wim[-1]) + (z[o3+1].im) * (wre[1]); } while (0); { do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2+1].re = z[1].re - t5; z[1].re = z[1].re + t5; } while (0); do { z[o3+1].im = z[o1+1].im - t3; z[o1+1].im = z[o1+1].im + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3+1].re = z[o1+1].re - t4; z[o1+1].re = z[o1+1].re + t4; } while (0); do { z[o2+1].im = z[1].im - t6; z[1].im = z[1].im + t6; } while (0);}}; do { z += 2; wre += 2; wim -= 2; { do { (t1) = (z[o2].re) * (wre[0]) - (z[o2].im) * (-wim[0]); (t2) = (z[o2].re) * (-wim[0]) + (z[o2].im) * (wre[0]); } while (0); do { (t5) = (z[o3].re) * (wre[0]) - (z[o3].im) * (wim[0]); (t6) = (z[o3].re) * (wim[0]) + (z[o3].im) * (wre[0]); } while (0); { do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2].re = z[0].re - t5; z[0].re = z[0].re + t5; } while (0); do { z[o3].im = z[o1].im - t3; z[o1].im = z[o1].im + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3].re = z[o1].re - t4; z[o1].re = z[o1].re + t4; } while (0); do { z[o2].im = z[0].im - t6; z[0].im = z[0].im + t6; } while (0);}}; { do { (t1) = (z[o2+1].re) * (wre[1]) - (z[o2+1].im) * (-wim[-1]); (t2) = (z[o2+1].re) * (-wim[-1]) + (z[o2+1].im) * (wre[1]); } while (0); do { (t5) = (z[o3+1].re) * (wre[1]) - (z[o3+1].im) * (wim[-1]); (t6) = (z[o3+1].re) * (wim[-1]) + (z[o3+1].im) * (wre[1]); } while (0); { do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2+1].re = z[1].re - t5; z[1].re = z[1].re + t5; } while (0); do { z[o3+1].im = z[o1+1].im - t3; z[o1+1].im = z[o1+1].im + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3+1].re = z[o1+1].re - t4; z[o1+1].re = z[o1+1].re + t4; } while (0); do { z[o2+1].im = z[1].im - t6; z[1].im = z[1].im + t6; } while (0);}}; } while(--n);}



static void pass_big(FFTComplex *z, const FFTSample *wre, unsigned int n){ FFTDouble t1, t2, t3, t4, t5, t6; int o1 = 2*n; int o2 = 4*n; int o3 = 6*n; const FFTSample *wim = wre+o1; n--; { t1 = z[o2].re; t2 = z[o2].im; t5 = z[o3].re; t6 = z[o3].im; { FFTSample r0=z[0].re, i0=z[0].im, r1=z[o1].re, i1=z[o1].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2].re = r0 - t5; z[0].re = r0 + t5; } while (0); do { z[o3].im = i1 - t3; z[o1].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3].re = r1 - t4; z[o1].re = r1 + t4; } while (0); do { z[o2].im = i0 - t6; z[0].im = i0 + t6; } while (0);}}; { do { (t1) = (z[o2+1].re) * (wre[1]) - (z[o2+1].im) * (-wim[-1]); (t2) = (z[o2+1].re) * (-wim[-1]) + (z[o2+1].im) * (wre[1]); } while (0); do { (t5) = (z[o3+1].re) * (wre[1]) - (z[o3+1].im) * (wim[-1]); (t6) = (z[o3+1].re) * (wim[-1]) + (z[o3+1].im) * (wre[1]); } while (0); { FFTSample r0=z[1].re, i0=z[1].im, r1=z[o1+1].re, i1=z[o1+1].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2+1].re = r0 - t5; z[1].re = r0 + t5; } while (0); do { z[o3+1].im = i1 - t3; z[o1+1].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3+1].re = r1 - t4; z[o1+1].re = r1 + t4; } while (0); do { z[o2+1].im = i0 - t6; z[1].im = i0 + t6; } while (0);}}; do { z += 2; wre += 2; wim -= 2; { do { (t1) = (z[o2].re) * (wre[0]) - (z[o2].im) * (-wim[0]); (t2) = (z[o2].re) * (-wim[0]) + (z[o2].im) * (wre[0]); } while (0); do { (t5) = (z[o3].re) * (wre[0]) - (z[o3].im) * (wim[0]); (t6) = (z[o3].re) * (wim[0]) + (z[o3].im) * (wre[0]); } while (0); { FFTSample r0=z[0].re, i0=z[0].im, r1=z[o1].re, i1=z[o1].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2].re = r0 - t5; z[0].re = r0 + t5; } while (0); do { z[o3].im = i1 - t3; z[o1].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3].re = r1 - t4; z[o1].re = r1 + t4; } while (0); do { z[o2].im = i0 - t6; z[0].im = i0 + t6; } while (0);}}; { do { (t1) = (z[o2+1].re) * (wre[1]) - (z[o2+1].im) * (-wim[-1]); (t2) = (z[o2+1].re) * (-wim[-1]) + (z[o2+1].im) * (wre[1]); } while (0); do { (t5) = (z[o3+1].re) * (wre[1]) - (z[o3+1].im) * (wim[-1]); (t6) = (z[o3+1].re) * (wim[-1]) + (z[o3+1].im) * (wre[1]); } while (0); { FFTSample r0=z[1].re, i0=z[1].im, r1=z[o1+1].re, i1=z[o1+1].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[o2+1].re = r0 - t5; z[1].re = r0 + t5; } while (0); do { z[o3+1].im = i1 - t3; z[o1+1].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[o3+1].re = r1 - t4; z[o1+1].re = r1 + t4; } while (0); do { z[o2+1].im = i0 - t6; z[1].im = i0 + t6; } while (0);}}; } while(--n);}
static void fft4(FFTComplex *z)
{
    FFTDouble t1, t2, t3, t4, t5, t6, t7, t8;

    do { t3 = z[0].re - z[1].re; t1 = z[0].re + z[1].re; } while (0);
    do { t8 = z[3].re - z[2].re; t6 = z[3].re + z[2].re; } while (0);
    do { z[2].re = t1 - t6; z[0].re = t1 + t6; } while (0);
    do { t4 = z[0].im - z[1].im; t2 = z[0].im + z[1].im; } while (0);
    do { t7 = z[2].im - z[3].im; t5 = z[2].im + z[3].im; } while (0);
    do { z[3].im = t4 - t8; z[1].im = t4 + t8; } while (0);
    do { z[3].re = t3 - t7; z[1].re = t3 + t7; } while (0);
    do { z[2].im = t2 - t5; z[0].im = t2 + t5; } while (0);
}

static void fft8(FFTComplex *z)
{
    FFTDouble t1, t2, t3, t4, t5, t6;

    fft4(z);

    do { t1 = z[4].re - -z[5].re; z[5].re = z[4].re + -z[5].re; } while (0);
    do { t2 = z[4].im - -z[5].im; z[5].im = z[4].im + -z[5].im; } while (0);
    do { t5 = z[6].re - -z[7].re; z[7].re = z[6].re + -z[7].re; } while (0);
    do { t6 = z[6].im - -z[7].im; z[7].im = z[6].im + -z[7].im; } while (0);

    { FFTSample r0=z[0].re, i0=z[0].im, r1=z[2].re, i1=z[2].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[4].re = r0 - t5; z[0].re = r0 + t5; } while (0); do { z[6].im = i1 - t3; z[2].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[6].re = r1 - t4; z[2].re = r1 + t4; } while (0); do { z[4].im = i0 - t6; z[0].im = i0 + t6; } while (0);};
    { do { (t1) = (z[5].re) * ((float)
   0.70710678118654752440
   ) - (z[5].im) * (-(float)
   0.70710678118654752440
   ); (t2) = (z[5].re) * (-(float)
   0.70710678118654752440
   ) + (z[5].im) * ((float)
   0.70710678118654752440
   ); } while (0); do { (t5) = (z[7].re) * ((float)
   0.70710678118654752440
   ) - (z[7].im) * ((float)
   0.70710678118654752440
   ); (t6) = (z[7].re) * ((float)
   0.70710678118654752440
   ) + (z[7].im) * ((float)
   0.70710678118654752440
   ); } while (0); { FFTSample r0=z[1].re, i0=z[1].im, r1=z[3].re, i1=z[3].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[5].re = r0 - t5; z[1].re = r0 + t5; } while (0); do { z[7].im = i1 - t3; z[3].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[7].re = r1 - t4; z[3].re = r1 + t4; } while (0); do { z[5].im = i0 - t6; z[1].im = i0 + t6; } while (0);}};
}


static void fft16(FFTComplex *z)
{
    FFTDouble t1, t2, t3, t4, t5, t6;
    FFTSample cos_16_1 = ff_cos_16[1];
    FFTSample cos_16_3 = ff_cos_16[3];

    fft8(z);
    fft4(z+8);
    fft4(z+12);

    { t1 = z[8].re; t2 = z[8].im; t5 = z[12].re; t6 = z[12].im; { FFTSample r0=z[0].re, i0=z[0].im, r1=z[4].re, i1=z[4].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[8].re = r0 - t5; z[0].re = r0 + t5; } while (0); do { z[12].im = i1 - t3; z[4].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[12].re = r1 - t4; z[4].re = r1 + t4; } while (0); do { z[8].im = i0 - t6; z[0].im = i0 + t6; } while (0);}};
    { do { (t1) = (z[10].re) * ((float)
   0.70710678118654752440
   ) - (z[10].im) * (-(float)
   0.70710678118654752440
   ); (t2) = (z[10].re) * (-(float)
   0.70710678118654752440
   ) + (z[10].im) * ((float)
   0.70710678118654752440
   ); } while (0); do { (t5) = (z[14].re) * ((float)
   0.70710678118654752440
   ) - (z[14].im) * ((float)
   0.70710678118654752440
   ); (t6) = (z[14].re) * ((float)
   0.70710678118654752440
   ) + (z[14].im) * ((float)
   0.70710678118654752440
   ); } while (0); { FFTSample r0=z[2].re, i0=z[2].im, r1=z[6].re, i1=z[6].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[10].re = r0 - t5; z[2].re = r0 + t5; } while (0); do { z[14].im = i1 - t3; z[6].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[14].re = r1 - t4; z[6].re = r1 + t4; } while (0); do { z[10].im = i0 - t6; z[2].im = i0 + t6; } while (0);}};
    { do { (t1) = (z[9].re) * (cos_16_1) - (z[9].im) * (-cos_16_3); (t2) = (z[9].re) * (-cos_16_3) + (z[9].im) * (cos_16_1); } while (0); do { (t5) = (z[13].re) * (cos_16_1) - (z[13].im) * (cos_16_3); (t6) = (z[13].re) * (cos_16_3) + (z[13].im) * (cos_16_1); } while (0); { FFTSample r0=z[1].re, i0=z[1].im, r1=z[5].re, i1=z[5].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[9].re = r0 - t5; z[1].re = r0 + t5; } while (0); do { z[13].im = i1 - t3; z[5].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[13].re = r1 - t4; z[5].re = r1 + t4; } while (0); do { z[9].im = i0 - t6; z[1].im = i0 + t6; } while (0);}};
    { do { (t1) = (z[11].re) * (cos_16_3) - (z[11].im) * (-cos_16_1); (t2) = (z[11].re) * (-cos_16_1) + (z[11].im) * (cos_16_3); } while (0); do { (t5) = (z[15].re) * (cos_16_3) - (z[15].im) * (cos_16_1); (t6) = (z[15].re) * (cos_16_1) + (z[15].im) * (cos_16_3); } while (0); { FFTSample r0=z[3].re, i0=z[3].im, r1=z[7].re, i1=z[7].im; do { t3 = t5 - t1; t5 = t5 + t1; } while (0); do { z[11].re = r0 - t5; z[3].re = r0 + t5; } while (0); do { z[15].im = i1 - t3; z[7].im = i1 + t3; } while (0); do { t4 = t2 - t6; t6 = t2 + t6; } while (0); do { z[15].re = r1 - t4; z[7].re = r1 + t4; } while (0); do { z[11].im = i0 - t6; z[3].im = i0 + t6; } while (0);}};
}



static void fft32(FFTComplex *z){ fft16(z); fft8(z+8*2); fft8(z+8*3); pass(z,ff_cos_32,8/2);}
static void fft64(FFTComplex *z){ fft32(z); fft16(z+16*2); fft16(z+16*3); pass(z,ff_cos_64,16/2);}
static void fft128(FFTComplex *z){ fft64(z); fft32(z+32*2); fft32(z+32*3); pass(z,ff_cos_128,32/2);}
static void fft256(FFTComplex *z){ fft128(z); fft64(z+64*2); fft64(z+64*3); pass(z,ff_cos_256,64/2);}
static void fft512(FFTComplex *z){ fft256(z); fft128(z+128*2); fft128(z+128*3); pass(z,ff_cos_512,128/2);}



static void fft1024(FFTComplex *z){ fft512(z); fft256(z+256*2); fft256(z+256*3); pass_big(z,ff_cos_1024,256/2);}
static void fft2048(FFTComplex *z){ fft1024(z); fft512(z+512*2); fft512(z+512*3); pass_big(z,ff_cos_2048,512/2);}
static void fft4096(FFTComplex *z){ fft2048(z); fft1024(z+1024*2); fft1024(z+1024*3); pass_big(z,ff_cos_4096,1024/2);}
static void fft8192(FFTComplex *z){ fft4096(z); fft2048(z+2048*2); fft2048(z+2048*3); pass_big(z,ff_cos_8192,2048/2);}
static void fft16384(FFTComplex *z){ fft8192(z); fft4096(z+4096*2); fft4096(z+4096*3); pass_big(z,ff_cos_16384,4096/2);}
static void fft32768(FFTComplex *z){ fft16384(z); fft8192(z+8192*2); fft8192(z+8192*3); pass_big(z,ff_cos_32768,8192/2);}
static void fft65536(FFTComplex *z){ fft32768(z); fft16384(z+16384*2); fft16384(z+16384*3); pass_big(z,ff_cos_65536,16384/2);}
static void fft131072(FFTComplex *z){ fft65536(z); fft32768(z+32768*2); fft32768(z+32768*3); pass_big(z,ff_cos_131072,32768/2);}

static void (* const fft_dispatch[])(FFTComplex*) = {
    fft4, fft8, fft16, fft32, fft64, fft128, fft256, fft512, fft1024,
    fft2048, fft4096, fft8192, fft16384, fft32768, fft65536, fft131072
};

static void fft_calc_c(FFTContext *s, FFTComplex *z)
{
    fft_dispatch[s->nbits-2](z);
}












typedef struct MDCT15Context {
    int fft_n;
    int len2;
    int len4;
    int inverse;
    int *pfa_prereindex;
    int *pfa_postreindex;

    FFTContext ptwo_fft;
    FFTComplex *tmp;
    FFTComplex *twiddle_exptab;

    FFTComplex __attribute__ ((aligned (32))) exptab[64];


    void (*fft15)(FFTComplex *out, FFTComplex *in, FFTComplex *exptab, ptrdiff_t stride);


    void (*postreindex)(FFTComplex *out, FFTComplex *in, FFTComplex *exp, int *lut, ptrdiff_t len8);


    void (*mdct)(struct MDCT15Context *s, float *dst, const float *src, ptrdiff_t stride);


    void (*imdct_half)(struct MDCT15Context *s, float *dst, const float *src,
                       ptrdiff_t stride);
} MDCT15Context;


int ff_mdct15_init(MDCT15Context **ps, int inverse, int N, double scale);
void ff_mdct15_uninit(MDCT15Context **ps);

void ff_mdct15_init_x86(MDCT15Context *s);






__attribute__((cold)) void ff_mdct15_uninit(MDCT15Context **ps)
{
    MDCT15Context *s = *ps;

    if (!s)
        return;

    ff_fft_end(&s->ptwo_fft);

    av_freep(&s->pfa_prereindex);
    av_freep(&s->pfa_postreindex);
    av_freep(&s->twiddle_exptab);
    av_freep(&s->tmp);

    av_freep(ps);
}

static inline int init_pfa_reindex_tabs(MDCT15Context *s)
{
    int i, j;
    const int b_ptwo = s->ptwo_fft.nbits;
    const int l_ptwo = 1 << b_ptwo;
    const int inv_1 = l_ptwo << ((4 - b_ptwo) & 3);
    const int inv_2 = 0xeeeeeeef & ((1U << b_ptwo) - 1);

    s->pfa_prereindex = av_malloc_array(15 * l_ptwo, sizeof(*s->pfa_prereindex));
    if (!s->pfa_prereindex)
        return 1;

    s->pfa_postreindex = av_malloc_array(15 * l_ptwo, sizeof(*s->pfa_postreindex));
    if (!s->pfa_postreindex)
        return 1;


    for (i = 0; i < l_ptwo; i++) {
        for (j = 0; j < 15; j++) {
            const int q_pre = ((l_ptwo * j)/15 + i) >> b_ptwo;
            const int q_post = (((j*inv_1)/15) + (i*inv_2)) >> b_ptwo;
            const int k_pre = 15*i + (j - q_pre*15)*(1 << b_ptwo);
            const int k_post = i*inv_2*15 + j*inv_1 - 15*q_post*l_ptwo;
            s->pfa_prereindex[i*15 + j] = k_pre << 1;
            s->pfa_postreindex[k_post] = l_ptwo*j + i;
        }
    }

    return 0;
}


static inline void fft5(FFTComplex *out, FFTComplex *in, FFTComplex exptab[2])
{
    FFTComplex z0[4], t[6];

    t[0].re = in[3].re + in[12].re;
    t[0].im = in[3].im + in[12].im;
    t[1].im = in[3].re - in[12].re;
    t[1].re = in[3].im - in[12].im;
    t[2].re = in[6].re + in[ 9].re;
    t[2].im = in[6].im + in[ 9].im;
    t[3].im = in[6].re - in[ 9].re;
    t[3].re = in[6].im - in[ 9].im;

    out[0].re = in[0].re + in[3].re + in[6].re + in[9].re + in[12].re;
    out[0].im = in[0].im + in[3].im + in[6].im + in[9].im + in[12].im;

    t[4].re = exptab[0].re * t[2].re - exptab[1].re * t[0].re;
    t[4].im = exptab[0].re * t[2].im - exptab[1].re * t[0].im;
    t[0].re = exptab[0].re * t[0].re - exptab[1].re * t[2].re;
    t[0].im = exptab[0].re * t[0].im - exptab[1].re * t[2].im;
    t[5].re = exptab[0].im * t[3].re - exptab[1].im * t[1].re;
    t[5].im = exptab[0].im * t[3].im - exptab[1].im * t[1].im;
    t[1].re = exptab[0].im * t[1].re + exptab[1].im * t[3].re;
    t[1].im = exptab[0].im * t[1].im + exptab[1].im * t[3].im;

    z0[0].re = t[0].re - t[1].re;
    z0[0].im = t[0].im - t[1].im;
    z0[1].re = t[4].re + t[5].re;
    z0[1].im = t[4].im + t[5].im;

    z0[2].re = t[4].re - t[5].re;
    z0[2].im = t[4].im - t[5].im;
    z0[3].re = t[0].re + t[1].re;
    z0[3].im = t[0].im + t[1].im;

    out[1].re = in[0].re + z0[3].re;
    out[1].im = in[0].im + z0[0].im;
    out[2].re = in[0].re + z0[2].re;
    out[2].im = in[0].im + z0[1].im;
    out[3].re = in[0].re + z0[1].re;
    out[3].im = in[0].im + z0[2].im;
    out[4].re = in[0].re + z0[0].re;
    out[4].im = in[0].im + z0[3].im;
}

static void fft15_c(FFTComplex *out, FFTComplex *in, FFTComplex *exptab, ptrdiff_t stride)
{
    int k;
    FFTComplex tmp1[5], tmp2[5], tmp3[5];

    fft5(tmp1, in + 0, exptab + 19);
    fft5(tmp2, in + 1, exptab + 19);
    fft5(tmp3, in + 2, exptab + 19);

    for (k = 0; k < 5; k++) {
        FFTComplex t[2];

        do { ((t[0]).re) = ((tmp2[k]).re) * ((exptab[k]).re) - ((tmp2[k]).im) * ((exptab[k]).im); ((t[0]).im) = ((tmp2[k]).re) * ((exptab[k]).im) + ((tmp2[k]).im) * ((exptab[k]).re); } while (0);
        do { ((t[1]).re) = ((tmp3[k]).re) * ((exptab[2 * k]).re) - ((tmp3[k]).im) * ((exptab[2 * k]).im); ((t[1]).im) = ((tmp3[k]).re) * ((exptab[2 * k]).im) + ((tmp3[k]).im) * ((exptab[2 * k]).re); } while (0);
        out[stride*k].re = tmp1[k].re + t[0].re + t[1].re;
        out[stride*k].im = tmp1[k].im + t[0].im + t[1].im;

        do { ((t[0]).re) = ((tmp2[k]).re) * ((exptab[k + 5]).re) - ((tmp2[k]).im) * ((exptab[k + 5]).im); ((t[0]).im) = ((tmp2[k]).re) * ((exptab[k + 5]).im) + ((tmp2[k]).im) * ((exptab[k + 5]).re); } while (0);
        do { ((t[1]).re) = ((tmp3[k]).re) * ((exptab[2 * (k + 5)]).re) - ((tmp3[k]).im) * ((exptab[2 * (k + 5)]).im); ((t[1]).im) = ((tmp3[k]).re) * ((exptab[2 * (k + 5)]).im) + ((tmp3[k]).im) * ((exptab[2 * (k + 5)]).re); } while (0);
        out[stride*(k + 5)].re = tmp1[k].re + t[0].re + t[1].re;
        out[stride*(k + 5)].im = tmp1[k].im + t[0].im + t[1].im;

        do { ((t[0]).re) = ((tmp2[k]).re) * ((exptab[k + 10]).re) - ((tmp2[k]).im) * ((exptab[k + 10]).im); ((t[0]).im) = ((tmp2[k]).re) * ((exptab[k + 10]).im) + ((tmp2[k]).im) * ((exptab[k + 10]).re); } while (0);
        do { ((t[1]).re) = ((tmp3[k]).re) * ((exptab[2 * k + 5]).re) - ((tmp3[k]).im) * ((exptab[2 * k + 5]).im); ((t[1]).im) = ((tmp3[k]).re) * ((exptab[2 * k + 5]).im) + ((tmp3[k]).im) * ((exptab[2 * k + 5]).re); } while (0);
        out[stride*(k + 10)].re = tmp1[k].re + t[0].re + t[1].re;
        out[stride*(k + 10)].im = tmp1[k].im + t[0].im + t[1].im;
    }
}

static void mdct15(MDCT15Context *s, float *dst, const float *src, ptrdiff_t stride)
{
    int i, j;
    const int len4 = s->len4, len3 = len4 * 3, len8 = len4 >> 1;
    const int l_ptwo = 1 << s->ptwo_fft.nbits;
    FFTComplex fft15in[15];


    for (i = 0; i < l_ptwo; i++) {
        for (j = 0; j < 15; j++) {
            const int k = s->pfa_prereindex[i*15 + j];
            FFTComplex tmp, exp = s->twiddle_exptab[k >> 1];
            if (k < len4) {
                tmp.re = -src[ len4 + k] + src[1*len4 - 1 - k];
                tmp.im = -src[ len3 + k] - src[1*len3 - 1 - k];
            } else {
                tmp.re = -src[ len4 + k] - src[5*len4 - 1 - k];
                tmp.im = src[-len4 + k] - src[1*len3 - 1 - k];
            }
            do { (fft15in[j].im) = (tmp.re) * (exp.re) - (tmp.im) * (exp.im); (fft15in[j].re) = (tmp.re) * (exp.im) + (tmp.im) * (exp.re); } while (0);
        }
        s->fft15(s->tmp + s->ptwo_fft.revtab[i], fft15in, s->exptab, l_ptwo);
    }


    for (i = 0; i < 15; i++)
        s->ptwo_fft.fft_calc(&s->ptwo_fft, s->tmp + l_ptwo*i);


    for (i = 0; i < len8; i++) {
        const int i0 = len8 + i, i1 = len8 - i - 1;
        const int s0 = s->pfa_postreindex[i0], s1 = s->pfa_postreindex[i1];

        do { (dst[2*i1*stride + stride]) = (s->tmp[s0].re) * (s->twiddle_exptab[i0].im) - (s->tmp[s0].im) * (s->twiddle_exptab[i0].re); (dst[2*i0*stride]) = (s->tmp[s0].re) * (s->twiddle_exptab[i0].re) + (s->tmp[s0].im) * (s->twiddle_exptab[i0].im); } while (0)
                                                                ;
        do { (dst[2*i0*stride + stride]) = (s->tmp[s1].re) * (s->twiddle_exptab[i1].im) - (s->tmp[s1].im) * (s->twiddle_exptab[i1].re); (dst[2*i1*stride]) = (s->tmp[s1].re) * (s->twiddle_exptab[i1].re) + (s->tmp[s1].im) * (s->twiddle_exptab[i1].im); } while (0)
                                                                ;
    }
}

static void imdct15_half(MDCT15Context *s, float *dst, const float *src,
                         ptrdiff_t stride)
{
    FFTComplex fft15in[15];
    FFTComplex *z = (FFTComplex *)dst;
    int i, j, len8 = s->len4 >> 1, l_ptwo = 1 << s->ptwo_fft.nbits;
    const float *in1 = src, *in2 = src + (s->len2 - 1) * stride;


    for (i = 0; i < l_ptwo; i++) {
        for (j = 0; j < 15; j++) {
            const int k = s->pfa_prereindex[i*15 + j];
            FFTComplex tmp = { in2[-k*stride], in1[k*stride] };
            do { ((fft15in[j]).re) = ((tmp).re) * ((s->twiddle_exptab[k >> 1]).re) - ((tmp).im) * ((s->twiddle_exptab[k >> 1]).im); ((fft15in[j]).im) = ((tmp).re) * ((s->twiddle_exptab[k >> 1]).im) + ((tmp).im) * ((s->twiddle_exptab[k >> 1]).re); } while (0);
        }
        s->fft15(s->tmp + s->ptwo_fft.revtab[i], fft15in, s->exptab, l_ptwo);
    }


    for (i = 0; i < 15; i++)
        s->ptwo_fft.fft_calc(&s->ptwo_fft, s->tmp + l_ptwo*i);


    s->postreindex(z, s->tmp, s->twiddle_exptab, s->pfa_postreindex, len8);
}

static void postrotate_c(FFTComplex *out, FFTComplex *in, FFTComplex *exp,
                         int *lut, ptrdiff_t len8)
{
    int i;


    for (i = 0; i < len8; i++) {
        const int i0 = len8 + i, i1 = len8 - i - 1;
        const int s0 = lut[i0], s1 = lut[i1];

        do { (out[i1].re) = (in[s1].im) * (exp[i1].im) - (in[s1].re) * (exp[i1].re); (out[i0].im) = (in[s1].im) * (exp[i1].re) + (in[s1].re) * (exp[i1].im); } while (0);
        do { (out[i0].re) = (in[s0].im) * (exp[i0].im) - (in[s0].re) * (exp[i0].re); (out[i1].im) = (in[s0].im) * (exp[i0].re) + (in[s0].re) * (exp[i0].im); } while (0);
    }
}

__attribute__((cold)) int ff_mdct15_init(MDCT15Context **ps, int inverse, int N, double scale)
{
    MDCT15Context *s;
    double alpha, theta;
    int len2 = 15 * (1 << N);
    int len = 2 * len2;
    int i;


    if ((N < 2) || (N > 13))
        return (-(
              22
              ));

    s = av_mallocz(sizeof(*s));
    if (!s)
        return (-(
              12
              ));

    s->fft_n = N - 1;
    s->len4 = len2 / 2;
    s->len2 = len2;
    s->inverse = inverse;
    s->fft15 = fft15_c;
    s->mdct = mdct15;
    s->imdct_half = imdct15_half;
    s->postreindex = postrotate_c;

    if (ff_fft_init(&s->ptwo_fft, N - 1, s->inverse) < 0)
        goto fail;

    if (init_pfa_reindex_tabs(s))
        goto fail;

    s->tmp = av_malloc_array(len, 2 * sizeof(*s->tmp));
    if (!s->tmp)
        goto fail;

    s->twiddle_exptab = av_malloc_array(s->len4, sizeof(*s->twiddle_exptab));
    if (!s->twiddle_exptab)
        goto fail;

    theta = 0.125f + (scale < 0 ? s->len4 : 0);
    scale = sqrt(fabs(scale));
    for (i = 0; i < s->len4; i++) {
        alpha = 2 * 
                   3.14159265358979323846 
                        * (i + theta) / len;
        s->twiddle_exptab[i].re = cosf(alpha) * scale;
        s->twiddle_exptab[i].im = sinf(alpha) * scale;
    }


    for (i = 0; i < 19; i++) {
        if (i < 15) {
            double theta = (2.0f * 
                                  3.14159265358979323846 
                                       * i) / 15.0f;
            if (!s->inverse)
                theta *= -1;
            s->exptab[i].re = cosf(theta);
            s->exptab[i].im = sinf(theta);
        } else {
            s->exptab[i] = s->exptab[i - 15];
        }
    }


    s->exptab[19].re = cosf(2.0f * 
                                  3.14159265358979323846 
                                       / 5.0f);
    s->exptab[19].im = sinf(2.0f * 
                                  3.14159265358979323846 
                                       / 5.0f);
    s->exptab[20].re = cosf(1.0f * 
                                  3.14159265358979323846 
                                       / 5.0f);
    s->exptab[20].im = sinf(1.0f * 
                                  3.14159265358979323846 
                                       / 5.0f);


    if (s->inverse) {
        s->exptab[19].im *= -1;
        s->exptab[20].im *= -1;
    }

    if (0)
        ff_mdct15_init_x86(s);

    *ps = s;

    return 0;

fail:
    ff_mdct15_uninit(&s);
    return (-(
          12
          ));
}
struct RDFTContext {
    int nbits;
    int inverse;
    int sign_convention;


    const FFTSample *tcos;
    const FFTSample *tsin;
    int negative_sin;
    FFTContext fft;
    void (*rdft_calc)(struct RDFTContext *s, FFTSample *z);
};






int ff_rdft_init(RDFTContext *s, int nbits, enum RDFTransformType trans);
void ff_rdft_end(RDFTContext *s);

void ff_rdft_init_arm(RDFTContext *s);
static void rdft_calc_c(RDFTContext *s, FFTSample *data)
{
    int i, i1, i2;
    FFTComplex ev, od, odsum;
    const int n = 1 << s->nbits;
    const float k1 = 0.5;
    const float k2 = 0.5 - s->inverse;
    const FFTSample *tcos = s->tcos;
    const FFTSample *tsin = s->tsin;

    if (!s->inverse) {
        s->fft.fft_permute(&s->fft, (FFTComplex*)data);
        s->fft.fft_calc(&s->fft, (FFTComplex*)data);
    }


    ev.re = data[0];
    data[0] = ev.re+data[1];
    data[1] = ev.re-data[1];
    if (s->negative_sin) {
        for (i = 1; i < (n>>2); i++) { i1 = 2*i; i2 = n-i1; ev.re = k1*(data[i1 ]+data[i2 ]); od.im = k2*(data[i2 ]-data[i1 ]); ev.im = k1*(data[i1+1]-data[i2+1]); od.re = k2*(data[i1+1]+data[i2+1]); odsum.re = od.re*tcos[i] + od.im*tsin[i]; odsum.im = od.im*tcos[i] - od.re*tsin[i]; data[i1 ] = ev.re + odsum.re; data[i1+1] = ev.im + odsum.im; data[i2 ] = ev.re - odsum.re; data[i2+1] = odsum.im - ev.im; }
    } else {
        for (i = 1; i < (n>>2); i++) { i1 = 2*i; i2 = n-i1; ev.re = k1*(data[i1 ]+data[i2 ]); od.im = k2*(data[i2 ]-data[i1 ]); ev.im = k1*(data[i1+1]-data[i2+1]); od.re = k2*(data[i1+1]+data[i2+1]); odsum.re = od.re*tcos[i] - od.im*tsin[i]; odsum.im = od.im*tcos[i] + od.re*tsin[i]; data[i1 ] = ev.re + odsum.re; data[i1+1] = ev.im + odsum.im; data[i2 ] = ev.re - odsum.re; data[i2+1] = odsum.im - ev.im; }
    }

    data[2*i+1]=s->sign_convention*data[2*i+1];
    if (s->inverse) {
        data[0] *= k1;
        data[1] *= k1;
        s->fft.fft_permute(&s->fft, (FFTComplex*)data);
        s->fft.fft_calc(&s->fft, (FFTComplex*)data);
    }
}

__attribute__((cold)) int ff_rdft_init(RDFTContext *s, int nbits, enum RDFTransformType trans)
{
    int n = 1 << nbits;
    int ret;

    s->nbits = nbits;
    s->inverse = trans == IDFT_C2R || trans == DFT_C2R;
    s->sign_convention = trans == IDFT_R2C || trans == DFT_C2R ? 1 : -1;
    s->negative_sin = trans == DFT_C2R || trans == DFT_R2C;

    if (nbits < 4 || nbits > 16)
        return (-(
              22
              ));

    if ((ret = ff_fft_init(&s->fft, nbits-1, trans == IDFT_C2R || trans == IDFT_R2C)) < 0)
        return ret;

    ff_init_ff_cos_tabs(nbits);
    s->tcos = ff_cos_tabs[nbits];
    s->tsin = ff_cos_tabs[nbits] + (n >> 2);
    s->rdft_calc = rdft_calc_c;

    if (0) ff_rdft_init_arm(s);

    return 0;
}

__attribute__((cold)) void ff_rdft_end(RDFTContext *s)
{
    ff_fft_end(&s->fft);
}
void ff_kbd_window_init(float *window, float alpha, int n);
void ff_kbd_window_init_fixed(int32_t *window, float alpha, int n);



__attribute__((cold)) void ff_kbd_window_init(float *window, float alpha, int n)
{
   int i, j;
   double sum = 0.0, bessel, tmp;
   double local_window[1024];
   double alpha2 = (alpha * 
                           3.14159265358979323846 
                                / n) * (alpha * 
                                                3.14159265358979323846 
                                                     / n);

   do { if (!(n <= 1024)) { av_log(
  ((void *)0)
  , 0, "Assertion %s failed at %s:%d\n", "n <= 1024", "libavcodec/kbdwin.c", 33); abort(); } } while (0);

   for (i = 0; i < n; i++) {
       tmp = i * (n - i) * alpha2;
       bessel = 1.0;
       for (j = 50; j > 0; j--)
           bessel = bessel * tmp / (j * j) + 1;
       sum += bessel;
       local_window[i] = sum;
   }

   sum++;
   for (i = 0; i < n; i++)
       window[i] = sqrt(local_window[i] / sum);
}

__attribute__((cold)) void ff_kbd_window_init_fixed(int32_t *window, float alpha, int n)
{
    int i;
    float local_window[1024];

    ff_kbd_window_init(local_window, alpha, n);
    for (i = 0; i < n; i++)
        window[i] = (int)floor(2147483647.0 * local_window[i] + 0.5);
}
void ff_sine_window_init(float *window, int n);




void ff_init_ff_sine_windows(int index);

extern float __attribute__ ((aligned (32))) ff_sine_32[32];
extern float __attribute__ ((aligned (32))) ff_sine_64[64];
extern float __attribute__ ((aligned (32))) ff_sine_128[128];
extern float __attribute__ ((aligned (32))) ff_sine_256[256];
extern float __attribute__ ((aligned (32))) ff_sine_512[512];
extern float __attribute__ ((aligned (32))) ff_sine_1024[1024];
extern float __attribute__ ((aligned (32))) ff_sine_2048[2048];
extern float __attribute__ ((aligned (32))) ff_sine_4096[4096];
extern float __attribute__ ((aligned (32))) ff_sine_8192[8192];

extern float *const ff_sine_windows[];
 
float __attribute__ ((aligned (32))) ff_sine_32[32];
 float __attribute__ ((aligned (32))) ff_sine_64[64];
 float __attribute__ ((aligned (32))) ff_sine_128[128];
 float __attribute__ ((aligned (32))) ff_sine_256[256];
 float __attribute__ ((aligned (32))) ff_sine_512[512];
 float __attribute__ ((aligned (32))) ff_sine_1024[1024];
 float __attribute__ ((aligned (32))) ff_sine_2048[2048];
 float __attribute__ ((aligned (32))) ff_sine_4096[4096];
 float __attribute__ ((aligned (32))) ff_sine_8192[8192];




 float *const ff_sine_windows[] = {
    
   ((void *)0)
       , 
         ((void *)0)
             , 
               ((void *)0)
                   , 
                     ((void *)0)
                         , 
                           ((void *)0)
                               ,
    ff_sine_32, ff_sine_64, ff_sine_128,
    ff_sine_256, ff_sine_512, ff_sine_1024,
    ff_sine_2048, ff_sine_4096, ff_sine_8192,
};


__attribute__((cold)) void ff_sine_window_init(float *window, int n)
{
    int i;
    for(i = 0; i < n; i++)
        window[i] = sinf((i + 0.5) * (
                                     3.14159265358979323846 
                                          / (2.0 * n)));
}
static void init_ff_sine_window_5(void) { ff_sine_window_init(ff_sine_windows[5], 1 << 5);}
static void init_ff_sine_window_6(void) { ff_sine_window_init(ff_sine_windows[6], 1 << 6);}
static void init_ff_sine_window_7(void) { ff_sine_window_init(ff_sine_windows[7], 1 << 7);}
static void init_ff_sine_window_8(void) { ff_sine_window_init(ff_sine_windows[8], 1 << 8);}
static void init_ff_sine_window_9(void) { ff_sine_window_init(ff_sine_windows[9], 1 << 9);}
static void init_ff_sine_window_10(void) { ff_sine_window_init(ff_sine_windows[10], 1 << 10);}
static void init_ff_sine_window_11(void) { ff_sine_window_init(ff_sine_windows[11], 1 << 11);}
static void init_ff_sine_window_12(void) { ff_sine_window_init(ff_sine_windows[12], 1 << 12);}
static void init_ff_sine_window_13(void) { ff_sine_window_init(ff_sine_windows[13], 1 << 13);}

static void (*const sine_window_init_func_array[])(void) = {
    init_ff_sine_window_5,
    init_ff_sine_window_6,
    init_ff_sine_window_7,
    init_ff_sine_window_8,
    init_ff_sine_window_9,
    init_ff_sine_window_10,
    init_ff_sine_window_11,
    init_ff_sine_window_12,
    init_ff_sine_window_13,
};

static pthread_once_t init_sine_window_once[9] = {
    
   0
               , 
                 0
                             , 
                               0
                                           , 
                                             0
                                                         , 
                                                           0
                                                                       ,
    
   0
               , 
                 0
                             , 
                               0
                                           , 
                                             0

};


__attribute__((cold)) void ff_init_ff_sine_windows(int index)
{
    
   ((void)0)
                                                                ;




    pthread_once(&init_sine_window_once[index - 5], sine_window_init_func_array[index - 5]);


}
void ff_cbrt_tableinit(void);
void ff_cbrt_tableinit_fixed(void);
extern uint32_t ff_cbrt_tab[1 << 13];
extern uint32_t ff_cbrt_tab_fixed[1 << 13];













uint32_t ff_cbrt_tab[1 << 13];

__attribute__((cold)) void ff_cbrt_tableinit(void)
{
    static double cbrt_tab_dbl[1 << 13];
    if (!ff_cbrt_tab[(1<<13) - 1]) {
        int i, j, k;
        double cbrt_val;

        for (i = 1; i < 1<<13; i++)
            cbrt_tab_dbl[i] = 1;


        for (i = 2; i < 90; i++) {
            if (cbrt_tab_dbl[i] == 1) {
                cbrt_val = i * cbrt(i);
                for (k = i; k < 1<<13; k *= i)
                    for (j = k; j < 1<<13; j += k)
                        cbrt_tab_dbl[j] *= cbrt_val;
            }
        }

        for (i = 91; i <= 8191; i+= 2) {
            if (cbrt_tab_dbl[i] == 1) {
                cbrt_val = i * cbrt(i);
                for (j = i; j < 1<<13; j += i)
                    cbrt_tab_dbl[j] *= cbrt_val;
            }
        }

        for (i = 0; i < 1<<13; i++)
            ff_cbrt_tab[i] = av_float2int((float)(cbrt_tab_dbl[i]));
    }
}



typedef struct AVFixedDSPContext {
    void (*vector_fmul_window_scaled)(int16_t *dst, const int32_t *src0, const int32_t *src1, const int32_t *win, int len, uint8_t bits);
    void (*vector_fmul_window)(int32_t *dst, const int32_t *src0, const int32_t *src1, const int32_t *win, int len);
    void (*vector_fmul)(int *dst, const int *src0, const int *src1,
                        int len);

    void (*vector_fmul_reverse)(int *dst, const int *src0, const int *src1, int len);
    void (*vector_fmul_add)(int *dst, const int *src0, const int *src1,
                            const int *src2, int len);
    int (*scalarproduct_fixed)(const int *v1, const int *v2, int len);
    void (*butterflies_fixed)(int *restrict v1, int *restrict v2, int len);
} AVFixedDSPContext;







AVFixedDSPContext * avpriv_alloc_fixed_dsp(int strict);

void ff_fixed_dsp_init_x86(AVFixedDSPContext *fdsp);
static __attribute__((always_inline)) inline int fixed_sqrt(int x, int bits)
{
    int retval, bit_mask, guess, square, i;
    int64_t accu;
    int shift1 = 30 - bits;
    int shift2 = bits - 15;

    if (shift1 > 0) retval = ff_sqrt(x << shift1);
    else retval = ff_sqrt(x >> -shift1);

    if (shift2 > 0) {
        retval = retval << shift2;
        bit_mask = (1 << (shift2 - 1));

        for (i=0; i<shift2; i++){
            guess = retval + bit_mask;
            accu = (int64_t)guess * guess;
            square = (int)((accu + bit_mask) >> bits);
            if (x >= square)
                retval += bit_mask;
            bit_mask >>= 1;
        }

    }
    else retval >>= (-shift2);

    return retval;
}






typedef struct GetBitContext {
    const uint8_t *buffer, *buffer_end;




    int index;
    int size_in_bits;
    int size_in_bits_plus8;
} GetBitContext;

static inline unsigned int get_bits(GetBitContext *s, int n);
static inline void skip_bits(GetBitContext *s, int n);
static inline unsigned int show_bits(GetBitContext *s, int n);
static inline int get_bits_count(const GetBitContext *s)
{



    return s->index;

}
static inline void skip_bits_long(GetBitContext *s, int n)
{






    s->index += av_clip_c(n, -s->index, s->size_in_bits_plus8 - s->index);


}
static inline int get_xbits(GetBitContext *s, int n)
{







    register int sign;
    register int32_t cache;
    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    ((void)0);
    re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
    cache = ((uint32_t) re_cache);
    sign = ~cache >> 31;
    re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8));
    (s)->index = re_index;
    return ((((uint32_t)(sign ^ cache))>>(32-(n))) ^ sign) - sign;

}


static inline int get_xbits_le(GetBitContext *s, int n)
{
    register int sign;
    register int32_t cache;
    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    ((void)0);
    re_cache = (((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l) >> (re_index & 7);
    cache = ((uint32_t) re_cache);
    sign = sign_extend(~cache, n) >> 31;
    re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8));
    (s)->index = re_index;
    return (zero_extend(sign ^ cache, n) ^ sign) - sign;
}


static inline int get_sbits(GetBitContext *s, int n)
{
    register int tmp;




    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    ((void)0);
    re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
    tmp = ((( int32_t)(re_cache))>>(32-(n)));
    re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8));
    (s)->index = re_index;

    return tmp;
}




static inline unsigned int get_bits(GetBitContext *s, int n)
{
    register unsigned int tmp;
    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    ((void)0);
    re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
    tmp = (((uint32_t)(re_cache))>>(32-(n)));
    re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8));
    (s)->index = re_index;

    ((void)0);
    return tmp;
}




static __attribute__((always_inline)) inline int get_bitsz(GetBitContext *s, int n)
{
    return n ? get_bits(s, n) : 0;
}

static inline unsigned int get_bits_le(GetBitContext *s, int n)
{
    register int tmp;
    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    ((void)0);
    re_cache = (((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l) >> (re_index & 7);
    tmp = zero_extend(re_cache, n);
    re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8));
    (s)->index = re_index;
    return tmp;

}




static inline unsigned int show_bits(GetBitContext *s, int n)
{
    register unsigned int tmp;
    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache;
    ((void)0);
    re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
    tmp = (((uint32_t)(re_cache))>>(32-(n)));

    return tmp;
}

static inline void skip_bits(GetBitContext *s, int n)
{
    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8));
    (s)->index = re_index;

}

static inline unsigned int get_bits1(GetBitContext *s)
{
    unsigned int index = s->index;
    uint8_t result = s->buffer[index >> 3];




    result <<= index & 7;
    result >>= 8 - 1;


    if (s->index < s->size_in_bits_plus8)

        index++;
    s->index = index;

    return result;

}

static inline unsigned int show_bits1(GetBitContext *s)
{
    return show_bits(s, 1);
}

static inline void skip_bits1(GetBitContext *s)
{
    skip_bits(s, 1);
}




static inline unsigned int get_bits_long(GetBitContext *s, int n)
{
    ((void)0);
    if (!n) {
        return 0;




    } else if (n <= 25) {
        return get_bits(s, n);
    } else {




        unsigned ret = get_bits(s, 16) << (n - 16);
        return ret | get_bits(s, n - 16);

    }

}




static inline uint64_t get_bits64(GetBitContext *s, int n)
{
    if (n <= 32) {
        return get_bits_long(s, n);
    } else {




        uint64_t ret = (uint64_t) get_bits_long(s, n - 32) << 32;
        return ret | get_bits_long(s, 32);

    }
}




static inline int get_sbits_long(GetBitContext *s, int n)
{

    if (!n)
        return 0;

    return sign_extend(get_bits_long(s, n), n);
}




static inline unsigned int show_bits_long(GetBitContext *s, int n)
{
    if (n <= 25) {
        return show_bits(s, n);
    } else {
        GetBitContext gb = *s;
        return get_bits_long(&gb, n);
    }
}

static inline int check_marker(void *logctx, GetBitContext *s, const char *msg)
{
    int bit = get_bits1(s);
    if (!bit)
        av_log(logctx, 32, "Marker bit missing at %d of %d %s\n",
               get_bits_count(s) - 1, s->size_in_bits, msg);

    return bit;
}

static inline int init_get_bits_xe(GetBitContext *s, const uint8_t *buffer,
                                   int bit_size, int is_le)
{
    int buffer_size;
    int ret = 0;

    if (bit_size >= 0x7fffffff 
                           - ((7) > (64*8) ? (7) : (64*8)) || bit_size < 0 || !buffer) {
        bit_size = 0;
        buffer = 
                     ((void *)0)
                         ;
        ret = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }

    buffer_size = (bit_size + 7) >> 3;

    s->buffer = buffer;
    s->size_in_bits = bit_size;
    s->size_in_bits_plus8 = bit_size + 8;
    s->buffer_end = buffer + buffer_size;
    s->index = 0;







    return ret;
}
static inline int init_get_bits(GetBitContext *s, const uint8_t *buffer,
                                int bit_size)
{



    return init_get_bits_xe(s, buffer, bit_size, 0);

}
static inline int init_get_bits8(GetBitContext *s, const uint8_t *buffer,
                                 int byte_size)
{
    if (byte_size > 0x7fffffff 
                           / 8 || byte_size < 0)
        byte_size = -1;
    return init_get_bits(s, buffer, byte_size * 8);
}

static inline int init_get_bits8_le(GetBitContext *s, const uint8_t *buffer,
                                    int byte_size)
{
    if (byte_size > 0x7fffffff 
                           / 8 || byte_size < 0)
        byte_size = -1;
    return init_get_bits_xe(s, buffer, byte_size * 8, 1);
}

static inline const uint8_t *align_get_bits(GetBitContext *s)
{
    int n = -get_bits_count(s) & 7;
    if (n)
        skip_bits(s, n);
    return s->buffer + (s->index >> 3);
}
static inline int set_idx(GetBitContext *s, int code, int *n, int *nb_bits,
                          int16_t (*table)[2])
{
    unsigned idx;

    *nb_bits = -*n;
    idx = show_bits(s, *nb_bits) + code;
    *n = table[idx][1];

    return table[idx][0];
}
static __attribute__((always_inline)) inline int get_vlc2(GetBitContext *s, int16_t (*table)[2],
                                     int bits, int max_depth)
{
    int code;

    unsigned int re_index = (s)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (s)->size_in_bits_plus8;
    re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7);

    do { int n, nb_bits; unsigned int index; index = (((uint32_t)(re_cache))>>(32-(bits))); code = table[index][0]; n = table[index][1]; if (max_depth > 1 && n < 0) { re_index = ((re_size_plus8) > (re_index + (bits)) ? (re_index + (bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = table[index][0]; n = table[index][1]; if (max_depth > 2 && n < 0) { re_index = ((re_size_plus8) > (re_index + (nb_bits)) ? (re_index + (nb_bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((s)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = table[index][0]; n = table[index][1]; } } do { re_cache <<= (n); re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8)); } while (0); } while (0);

    (s)->index = re_index;

    return code;

}

static inline int decode012(GetBitContext *gb)
{
    int n;
    n = get_bits1(gb);
    if (n == 0)
        return 0;
    else
        return get_bits1(gb) + 1;
}

static inline int decode210(GetBitContext *gb)
{
    if (get_bits1(gb))
        return 0;
    else
        return 2 - get_bits1(gb);
}

static inline int get_bits_left(GetBitContext *gb)
{
    return gb->size_in_bits - get_bits_count(gb);
}

static inline int skip_1stop_8data_bits(GetBitContext *gb)
{
    if (get_bits_left(gb) <= 0)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    while (get_bits1(gb)) {
        skip_bits(gb, 8);
        if (get_bits_left(gb) <= 0)
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }

    return 0;
}



typedef struct MPEG4AudioConfig {
    int object_type;
    int sampling_index;
    int sample_rate;
    int chan_config;
    int sbr;
    int ext_object_type;
    int ext_sampling_index;
    int ext_sample_rate;
    int ext_chan_config;
    int channels;
    int ps;
    int frame_length_short;
} MPEG4AudioConfig;

extern const int avpriv_mpeg4audio_sample_rates[16];
extern const uint8_t ff_mpeg4audio_channels[14];
int ff_mpeg4audio_get_config_gb(MPEG4AudioConfig *c, GetBitContext *gb,
                                int sync_extension, void *logctx);
int avpriv_mpeg4audio_get_config(MPEG4AudioConfig *c, const uint8_t *buf,
                                 int bit_size, int sync_extension);
int avpriv_mpeg4audio_get_config2(MPEG4AudioConfig *c, const uint8_t *buf,
                                  int size, int sync_extension, void *logctx);

enum AudioObjectType {
    AOT_NULL,

    AOT_AAC_MAIN,
    AOT_AAC_LC,
    AOT_AAC_SSR,
    AOT_AAC_LTP,
    AOT_SBR,
    AOT_AAC_SCALABLE,
    AOT_TWINVQ,
    AOT_CELP,
    AOT_HVXC,
    AOT_TTSI = 12,
    AOT_MAINSYNTH,
    AOT_WAVESYNTH,
    AOT_MIDI,
    AOT_SAFX,
    AOT_ER_AAC_LC,
    AOT_ER_AAC_LTP = 19,
    AOT_ER_AAC_SCALABLE,
    AOT_ER_TWINVQ,
    AOT_ER_BSAC,
    AOT_ER_AAC_LD,
    AOT_ER_CELP,
    AOT_ER_HVXC,
    AOT_ER_HILN,
    AOT_ER_PARAM,
    AOT_SSC,
    AOT_PS,
    AOT_SURROUND,
    AOT_ESCAPE,
    AOT_L1,
    AOT_L2,
    AOT_L3,
    AOT_DST,
    AOT_ALS,
    AOT_SLS,
    AOT_SLS_NON_CORE,
    AOT_ER_AAC_ELD,
    AOT_SMR_SIMPLE,
    AOT_SMR_MAIN,
    AOT_USAC_NOSBR,
    AOT_SAOC,
    AOT_LD_SURROUND,
    AOT_USAC,
};




static __attribute__((always_inline)) inline unsigned int ff_pce_copy_bits(PutBitContext *pb,
                                                      GetBitContext *gb,
                                                      int bits)
{
    unsigned int el = get_bits(gb, bits);
    put_bits(pb, bits, el);
    return el;
}

static inline int ff_copy_pce_data(PutBitContext *pb, GetBitContext *gb)
{
    int five_bit_ch, four_bit_ch, comment_size, bits;
    int offset = put_bits_count(pb);

    ff_pce_copy_bits(pb, gb, 10);
    five_bit_ch = ff_pce_copy_bits(pb, gb, 4);
    five_bit_ch += ff_pce_copy_bits(pb, gb, 4);
    five_bit_ch += ff_pce_copy_bits(pb, gb, 4);
    four_bit_ch = ff_pce_copy_bits(pb, gb, 2);
    four_bit_ch += ff_pce_copy_bits(pb, gb, 3);
    five_bit_ch += ff_pce_copy_bits(pb, gb, 4);
    if (ff_pce_copy_bits(pb, gb, 1))
        ff_pce_copy_bits(pb, gb, 4);
    if (ff_pce_copy_bits(pb, gb, 1))
        ff_pce_copy_bits(pb, gb, 4);
    if (ff_pce_copy_bits(pb, gb, 1))
        ff_pce_copy_bits(pb, gb, 3);
    for (bits = five_bit_ch*5+four_bit_ch*4; bits > 16; bits -= 16)
        ff_pce_copy_bits(pb, gb, 16);
    if (bits)
        ff_pce_copy_bits(pb, gb, bits);
    align_put_bits(pb);
    align_get_bits(gb);
    comment_size = ff_pce_copy_bits(pb, gb, 8);
    for (; comment_size > 0; comment_size--)
        ff_pce_copy_bits(pb, gb, 8);

    return put_bits_count(pb) - offset;
}







typedef struct PSDSPContext {
    void (*add_squares)(INTFLOAT *dst, const INTFLOAT (*src)[2], int n);
    void (*mul_pair_single)(INTFLOAT (*dst)[2], INTFLOAT (*src0)[2], INTFLOAT *src1,
                            int n);
    void (*hybrid_analysis)(INTFLOAT (*out)[2], INTFLOAT (*in)[2],
                            const INTFLOAT (*filter)[8][2],
                            ptrdiff_t stride, int n);
    void (*hybrid_analysis_ileave)(INTFLOAT (*out)[32][2], INTFLOAT L[2][38][64],
                                   int i, int len);
    void (*hybrid_synthesis_deint)(INTFLOAT out[2][38][64], INTFLOAT (*in)[32][2],
                                   int i, int len);
    void (*decorrelate)(INTFLOAT (*out)[2], INTFLOAT (*delay)[2],
                        INTFLOAT (*ap_delay)[32 +5][2],
                        const INTFLOAT phi_fract[2], const INTFLOAT (*Q_fract)[2],
                        const INTFLOAT *transient_gain,
                        INTFLOAT g_decay_slope,
                        int len);
    void (*stereo_interpolate[2])(INTFLOAT (*l)[2], INTFLOAT (*r)[2],
                                  INTFLOAT h[2][4], INTFLOAT h_step[2][4],
                                  int len);
} PSDSPContext;

void ff_psdsp_init(PSDSPContext *s);
void ff_psdsp_init_arm(PSDSPContext *s);
void ff_psdsp_init_aarch64(PSDSPContext *s);
void ff_psdsp_init_mips(PSDSPContext *s);
void ff_psdsp_init_x86(PSDSPContext *s);
typedef struct PSCommonContext {
    int start;
    int enable_iid;
    int iid_quant;
    int nr_iid_par;
    int nr_ipdopd_par;
    int enable_icc;
    int icc_mode;
    int nr_icc_par;
    int enable_ext;
    int frame_class;
    int num_env_old;
    int num_env;
    int enable_ipdopd;
    int border_position[5 +1];
    int8_t iid_par[5][34];
    int8_t icc_par[5][34];

    int8_t ipd_par[5][34];
    int8_t opd_par[5][34];
    int is34bands;
    int is34bands_old;
} PSCommonContext;

typedef struct PSContext {
    PSCommonContext common;

    INTFLOAT __attribute__ ((aligned (16))) in_buf[5][44][2];
    INTFLOAT __attribute__ ((aligned (16))) delay[91][32 + 14][2];
    INTFLOAT __attribute__ ((aligned (16))) ap_delay[50][3][32 + 5][2];
    INTFLOAT __attribute__ ((aligned (16))) peak_decay_nrg[34];
    INTFLOAT __attribute__ ((aligned (16))) power_smooth[34];
    INTFLOAT __attribute__ ((aligned (16))) peak_decay_diff_smooth[34];
    INTFLOAT __attribute__ ((aligned (16))) H11[2][5 +1][34];
    INTFLOAT __attribute__ ((aligned (16))) H12[2][5 +1][34];
    INTFLOAT __attribute__ ((aligned (16))) H21[2][5 +1][34];
    INTFLOAT __attribute__ ((aligned (16))) H22[2][5 +1][34];
    INTFLOAT __attribute__ ((aligned (16))) Lbuf[91][32][2];
    INTFLOAT __attribute__ ((aligned (16))) Rbuf[91][32][2];
    int8_t opd_hist[34];
    int8_t ipd_hist[34];
    PSDSPContext dsp;
} PSContext;

extern const int8_t ff_k_to_i_20[];
extern const int8_t ff_k_to_i_34[];

void ff_ps_init_common(void);
void ff_ps_init(void);
void ff_ps_ctx_init(PSContext *ps);
int ff_ps_read_data(AVCodecContext *avctx, GetBitContext *gb,
                     PSCommonContext *ps, int bits_left);
int ff_ps_apply(AVCodecContext *avctx, PSContext *ps, INTFLOAT L[2][38][64], INTFLOAT R[2][38][64], int top);
static const int32_t av_sqrttbl_sf[512+1] = {
   0x2d413ccd,0x2d4c8bb3,0x2d57d7c6,0x2d63210a,
   0x2d6e677f,0x2d79ab2a,0x2d84ec0b,0x2d902a23,
   0x2d9b6578,0x2da69e08,0x2db1d3d6,0x2dbd06e6,
   0x2dc83738,0x2dd364ce,0x2dde8fac,0x2de9b7d2,
   0x2df4dd43,0x2e000000,0x2e0b200c,0x2e163d68,
   0x2e215816,0x2e2c701a,0x2e378573,0x2e429824,
   0x2e4da830,0x2e58b598,0x2e63c05d,0x2e6ec883,
   0x2e79ce0a,0x2e84d0f5,0x2e8fd144,0x2e9acefb,
   0x2ea5ca1b,0x2eb0c2a7,0x2ebbb89e,0x2ec6ac04,
   0x2ed19cda,0x2edc8b23,0x2ee776df,0x2ef26012,
   0x2efd46bb,0x2f082add,0x2f130c7b,0x2f1deb95,
   0x2f28c82e,0x2f33a246,0x2f3e79e1,0x2f494eff,
   0x2f5421a3,0x2f5ef1ce,0x2f69bf81,0x2f748abe,
   0x2f7f5388,0x2f8a19e0,0x2f94ddc7,0x2f9f9f3e,
   0x2faa5e48,0x2fb51ae8,0x2fbfd51c,0x2fca8ce9,
   0x2fd5424e,0x2fdff54e,0x2feaa5eb,0x2ff55426,
   0x30000000,0x300aa97b,0x3015509a,0x301ff55c,
   0x302a97c5,0x303537d5,0x303fd58e,0x304a70f2,
   0x30550a01,0x305fa0be,0x306a352a,0x3074c747,
   0x307f5716,0x3089e499,0x30946fd2,0x309ef8c0,
   0x30a97f67,0x30b403c7,0x30be85e2,0x30c905bb,
   0x30d38351,0x30ddfea6,0x30e877bc,0x30f2ee96,
   0x30fd6332,0x3107d594,0x311245bc,0x311cb3ad,
   0x31271f67,0x313188ec,0x313bf03d,0x3146555c,
   0x3150b84a,0x315b1909,0x31657798,0x316fd3fc,
   0x317a2e34,0x31848642,0x318edc28,0x31992fe5,
   0x31a3817d,0x31add0f0,0x31b81e40,0x31c2696e,
   0x31ccb27b,0x31d6f969,0x31e13e38,0x31eb80eb,
   0x31f5c182,0x32000000,0x320a3c65,0x321476b1,
   0x321eaee8,0x3228e50a,0x32331917,0x323d4b13,
   0x32477afc,0x3251a8d6,0x325bd4a2,0x3265fe5f,
   0x32702611,0x327a4bb8,0x32846f55,0x328e90e9,
   0x3298b076,0x32a2cdfd,0x32ace97e,0x32b702fd,
   0x32c11a79,0x32cb2ff3,0x32d5436d,0x32df54e9,
   0x32e96466,0x32f371e8,0x32fd7d6d,0x330786f9,
   0x33118e8c,0x331b9426,0x332597cb,0x332f9979,
   0x33399933,0x334396fa,0x334d92cf,0x33578cb2,
   0x336184a6,0x336b7aab,0x33756ec3,0x337f60ed,
   0x3389512d,0x33933f83,0x339d2bef,0x33a71672,
   0x33b0ff10,0x33bae5c7,0x33c4ca99,0x33cead88,
   0x33d88e95,0x33e26dbf,0x33ec4b09,0x33f62673,
   0x34000000,0x3409d7af,0x3413ad82,0x341d817a,
   0x34275397,0x343123db,0x343af248,0x3444bedd,
   0x344e899d,0x34585288,0x3462199f,0x346bdee3,
   0x3475a254,0x347f63f5,0x348923c6,0x3492e1c9,
   0x349c9dfe,0x34a65865,0x34b01101,0x34b9c7d2,
   0x34c37cda,0x34cd3018,0x34d6e18f,0x34e0913f,
   0x34ea3f29,0x34f3eb4d,0x34fd95ae,0x35073e4c,
   0x3510e528,0x351a8a43,0x35242d9d,0x352dcf39,
   0x35376f16,0x35410d36,0x354aa99a,0x35544442,
   0x355ddd2f,0x35677463,0x357109df,0x357a9da2,
   0x35842fb0,0x358dc007,0x35974ea9,0x35a0db98,
   0x35aa66d3,0x35b3f05c,0x35bd7833,0x35c6fe5a,
   0x35d082d3,0x35da059c,0x35e386b7,0x35ed0626,
   0x35f683e8,0x36000000,0x36097a6e,0x3612f331,
   0x361c6a4d,0x3625dfc1,0x362f538f,0x3638c5b7,
   0x36423639,0x364ba518,0x36551252,0x365e7deb,
   0x3667e7e2,0x36715039,0x367ab6f0,0x36841c07,
   0x368d7f81,0x3696e15d,0x36a0419d,0x36a9a040,
   0x36b2fd49,0x36bc58b8,0x36c5b28e,0x36cf0acb,
   0x36d86170,0x36e1b680,0x36eb09f8,0x36f45bdc,
   0x36fdac2b,0x3706fae7,0x37104810,0x371993a7,
   0x3722ddad,0x372c2622,0x37356d08,0x373eb25f,
   0x3747f629,0x37513865,0x375a7914,0x3763b838,
   0x376cf5d0,0x377631e0,0x377f6c64,0x3788a561,
   0x3791dcd6,0x379b12c4,0x37a4472c,0x37ad7a0e,
   0x37b6ab6a,0x37bfdb44,0x37c90999,0x37d2366d,
   0x37db61be,0x37e48b8e,0x37edb3de,0x37f6daae,
   0x38000000,0x380923d3,0x3812462a,0x381b6703,
   0x38248660,0x382da442,0x3836c0aa,0x383fdb97,
   0x3848f50c,0x38520d09,0x385b238d,0x3864389b,
   0x386d4c33,0x38765e55,0x387f6f01,0x38887e3b,
   0x38918c00,0x389a9853,0x38a3a334,0x38acaca3,
   0x38b5b4a3,0x38bebb32,0x38c7c051,0x38d0c402,
   0x38d9c645,0x38e2c71b,0x38ebc685,0x38f4c482,
   0x38fdc114,0x3906bc3c,0x390fb5fa,0x3918ae4f,
   0x3921a53a,0x392a9abe,0x39338edb,0x393c8192,
   0x394572e2,0x394e62ce,0x39575155,0x39603e77,
   0x39692a36,0x39721494,0x397afd8f,0x3983e527,
   0x398ccb60,0x3995b039,0x399e93b2,0x39a775cc,
   0x39b05689,0x39b935e8,0x39c213e9,0x39caf08e,
   0x39d3cbd9,0x39dca5c7,0x39e57e5b,0x39ee5596,
   0x39f72b77,0x3a000000,0x3a08d331,0x3a11a50a,
   0x3a1a758d,0x3a2344ba,0x3a2c1291,0x3a34df13,
   0x3a3daa41,0x3a46741b,0x3a4f3ca3,0x3a5803d7,
   0x3a60c9ba,0x3a698e4b,0x3a72518b,0x3a7b137c,
   0x3a83d41d,0x3a8c936f,0x3a955173,0x3a9e0e29,
   0x3aa6c992,0x3aaf83ae,0x3ab83c7e,0x3ac0f403,
   0x3ac9aa3c,0x3ad25f2c,0x3adb12d1,0x3ae3c52d,
   0x3aec7642,0x3af5260e,0x3afdd492,0x3b0681d0,
   0x3b0f2dc6,0x3b17d878,0x3b2081e4,0x3b292a0c,
   0x3b31d0f0,0x3b3a7690,0x3b431aec,0x3b4bbe06,
   0x3b545fdf,0x3b5d0077,0x3b659fcd,0x3b6e3de4,
   0x3b76daba,0x3b7f7651,0x3b8810aa,0x3b90a9c4,
   0x3b9941a1,0x3ba1d842,0x3baa6da5,0x3bb301cd,
   0x3bbb94b9,0x3bc4266a,0x3bccb6e2,0x3bd5461f,
   0x3bddd423,0x3be660ee,0x3beeec81,0x3bf776dc,
   0x3c000000,0x3c0887ed,0x3c110ea4,0x3c199426,
   0x3c221872,0x3c2a9b8a,0x3c331d6e,0x3c3b9e1d,
   0x3c441d9a,0x3c4c9be5,0x3c5518fd,0x3c5d94e3,
   0x3c660f98,0x3c6e891d,0x3c770172,0x3c7f7898,
   0x3c87ee8e,0x3c906356,0x3c98d6ef,0x3ca1495b,
   0x3ca9ba9a,0x3cb22aac,0x3cba9992,0x3cc3074c,
   0x3ccb73dc,0x3cd3df41,0x3cdc497b,0x3ce4b28c,
   0x3ced1a73,0x3cf58132,0x3cfde6c8,0x3d064b37,
   0x3d0eae7f,0x3d17109f,0x3d1f719a,0x3d27d16e,
   0x3d30301d,0x3d388da8,0x3d40ea0d,0x3d49454f,
   0x3d519f6d,0x3d59f867,0x3d625040,0x3d6aa6f6,
   0x3d72fc8b,0x3d7b50fe,0x3d83a451,0x3d8bf683,
   0x3d944796,0x3d9c9788,0x3da4e65c,0x3dad3412,
   0x3db580a9,0x3dbdcc24,0x3dc61680,0x3dce5fc0,
   0x3dd6a7e4,0x3ddeeeed,0x3de734d9,0x3def79ab,
   0x3df7bd62,0x3e000000,0x3e084184,0x3e1081ee,
   0x3e18c140,0x3e20ff7a,0x3e293c9c,0x3e3178a7,
   0x3e39b39a,0x3e41ed77,0x3e4a263d,0x3e525def,
   0x3e5a948b,0x3e62ca12,0x3e6afe85,0x3e7331e4,
   0x3e7b642f,0x3e839567,0x3e8bc58c,0x3e93f49f,
   0x3e9c22a1,0x3ea44f91,0x3eac7b6f,0x3eb4a63e,
   0x3ebccffb,0x3ec4f8aa,0x3ecd2049,0x3ed546d9,
   0x3edd6c5a,0x3ee590cd,0x3eedb433,0x3ef5d68c,
   0x3efdf7d7,0x3f061816,0x3f0e3749,0x3f165570,
   0x3f1e728c,0x3f268e9d,0x3f2ea9a4,0x3f36c3a0,
   0x3f3edc93,0x3f46f47c,0x3f4f0b5d,0x3f572135,
   0x3f5f3606,0x3f6749cf,0x3f6f5c90,0x3f776e4a,
   0x3f7f7efe,0x3f878eab,0x3f8f9d53,0x3f97aaf6,
   0x3f9fb793,0x3fa7c32c,0x3fafcdc1,0x3fb7d752,
   0x3fbfdfe0,0x3fc7e76b,0x3fcfedf3,0x3fd7f378,
   0x3fdff7fc,0x3fe7fb7f,0x3feffe00,0x3ff7ff80,
   0x3fffffff,
};

static const int32_t av_sqr_exp_multbl_sf[2] = {
   0x20000000,0x2d413ccd,
};

static const int32_t av_costbl_1_sf[16] = {
   0x40000000,0x3ec52fa0,0x3b20d79e,0x3536cc52,
   0x2d413ccd,0x238e7673,0x187de2a7,0x0c7c5c1e,
   0x00000000,0xf383a3e3,0xe7821d5a,0xdc71898e,
   0xd2bec334,0xcac933af,0xc4df2863,0xc13ad061,
};

static const int32_t av_costbl_2_sf[32] = {
   0x40000000,0x3fffb10b,0x3ffec42d,0x3ffd3969,
   0x3ffb10c1,0x3ff84a3c,0x3ff4e5e0,0x3ff0e3b6,
   0x3fec43c7,0x3fe7061f,0x3fe12acb,0x3fdab1d9,
   0x3fd39b5a,0x3fcbe75e,0x3fc395f9,0x3fbaa740,
   0x3fb11b48,0x3fa6f228,0x3f9c2bfb,0x3f90c8da,
   0x3f84c8e2,0x3f782c30,0x3f6af2e3,0x3f5d1d1d,
   0x3f4eaafe,0x3f3f9cab,0x3f2ff24a,0x3f1fabff,
   0x3f0ec9f5,0x3efd4c54,0x3eeb3347,0x3ed87efc,
};

static const int32_t av_sintbl_2_sf[32] = {
   0x00000000,0x006487c4,0x00c90e90,0x012d936c,
   0x0192155f,0x01f69373,0x025b0caf,0x02bf801a,
   0x0323ecbe,0x038851a2,0x03ecadcf,0x0451004d,
   0x04b54825,0x0519845e,0x057db403,0x05e1d61b,
   0x0645e9af,0x06a9edc9,0x070de172,0x0771c3b3,
   0x07d59396,0x08395024,0x089cf867,0x09008b6a,
   0x09640837,0x09c76dd8,0x0a2abb59,0x0a8defc3,
   0x0af10a22,0x0b540982,0x0bb6ecef,0x0c19b374,
};

static const int32_t av_costbl_3_sf[32] = {
   0x40000000,0x3fffffec,0x3fffffb1,0x3fffff4e,
   0x3ffffec4,0x3ffffe13,0x3ffffd39,0x3ffffc39,
   0x3ffffb11,0x3ffff9c1,0x3ffff84a,0x3ffff6ac,
   0x3ffff4e6,0x3ffff2f8,0x3ffff0e3,0x3fffeea7,
   0x3fffec43,0x3fffe9b7,0x3fffe705,0x3fffe42a,
   0x3fffe128,0x3fffddff,0x3fffdaae,0x3fffd736,
   0x3fffd396,0x3fffcfcf,0x3fffcbe0,0x3fffc7ca,
   0x3fffc38c,0x3fffbf27,0x3fffba9b,0x3fffb5e7,
};

static const int32_t av_sintbl_3_sf[32] = {
   0x00000000,0x0003243f,0x0006487f,0x00096cbe,
   0x000c90fe,0x000fb53d,0x0012d97c,0x0015fdbb,
   0x001921fb,0x001c463a,0x001f6a79,0x00228eb8,
   0x0025b2f7,0x0028d736,0x002bfb74,0x002f1fb3,
   0x003243f1,0x00356830,0x00388c6e,0x003bb0ac,
   0x003ed4ea,0x0041f928,0x00451d66,0x004841a3,
   0x004b65e1,0x004e8a1e,0x0051ae5b,0x0054d297,
   0x0057f6d4,0x005b1b10,0x005e3f4c,0x00616388,
};

static const int32_t av_costbl_4_sf[33] = {
   0x40000000,0x40000000,0x40000000,0x40000000,
   0x40000000,0x40000000,0x3fffffff,0x3fffffff,
   0x3fffffff,0x3ffffffe,0x3ffffffe,0x3ffffffe,
   0x3ffffffd,0x3ffffffd,0x3ffffffc,0x3ffffffc,
   0x3ffffffb,0x3ffffffa,0x3ffffffa,0x3ffffff9,
   0x3ffffff8,0x3ffffff7,0x3ffffff7,0x3ffffff6,
   0x3ffffff5,0x3ffffff4,0x3ffffff3,0x3ffffff2,
   0x3ffffff1,0x3ffffff0,0x3fffffef,0x3fffffed,
   0x3fffffec,
};

static const int32_t av_sintbl_4_sf[33] = {
   0x00000000,0x00001922,0x00003244,0x00004b66,
   0x00006488,0x00007daa,0x000096cc,0x0000afee,
   0x0000c910,0x0000e232,0x0000fb54,0x00011476,
   0x00012d98,0x000146ba,0x00015fdc,0x000178fe,
   0x00019220,0x0001ab42,0x0001c464,0x0001dd86,
   0x0001f6a8,0x00020fca,0x000228ec,0x0002420e,
   0x00025b30,0x00027452,0x00028d74,0x0002a696,
   0x0002bfb7,0x0002d8d9,0x0002f1fb,0x00030b1d,
   0x0003243f,
};





typedef struct SoftFloat{
    int32_t mant;
    int32_t exp;
}SoftFloat;

static const SoftFloat FLOAT_0 = { 0, -149};
static const SoftFloat FLOAT_05 = { 0x20000000, 0};
static const SoftFloat FLOAT_1 = { 0x20000000, 1};
static const SoftFloat FLOAT_EPSILON = { 0x29F16B12, -16};
static const SoftFloat FLOAT_1584893192 = { 0x32B771ED, 1};
static const SoftFloat FLOAT_100000 = { 0x30D40000, 17};
static const SoftFloat FLOAT_0999999 = { 0x3FFFFBCE, 0};
static const SoftFloat FLOAT_MIN = { 0x20000000, -149};





static inline __attribute__((const)) double av_sf2double(SoftFloat v) {
    v.exp -= 29 +1;
    return ldexp(v.mant, v.exp);
}

static __attribute__((const)) SoftFloat av_normalize_sf(SoftFloat a){
    if(a.mant){

        while((a.mant + 0x1FFFFFFFU)<0x3FFFFFFFU){
            a.mant += a.mant;
            a.exp -= 1;
        }





        if(a.exp < -149){
            a.exp = -149;
            a.mant= 0;
        }
    }else{
        a.exp= -149;
    }
    return a;
}

static inline __attribute__((const)) SoftFloat av_normalize1_sf(SoftFloat a){

    if((int32_t)(a.mant + 0x40000000U) <= 0){
        a.exp++;
        a.mant>>=1;
    }
    ((void)0);
    ((void)0);
    return a;







}






static inline __attribute__((const)) SoftFloat av_mul_sf(SoftFloat a, SoftFloat b){
    a.exp += b.exp;
    ((void)0);
    a.mant = (a.mant * (int64_t)b.mant) >> 29;
    a = av_normalize1_sf((SoftFloat){a.mant, a.exp - 1});
    if (!a.mant || a.exp < -149)
        return FLOAT_0;
    return a;
}





static inline __attribute__((const)) SoftFloat av_div_sf(SoftFloat a, SoftFloat b){
    int64_t temp = (int64_t)a.mant * (1<<(29 +1));
    temp /= b.mant;
    a.exp -= b.exp;
    a.mant = temp;
    while (a.mant != temp) {
        temp /= 2;
        a.exp--;
        a.mant = temp;
    }
    a = av_normalize1_sf(a);
    if (!a.mant || a.exp < -149)
        return FLOAT_0;
    return a;
}







static inline __attribute__((const)) int av_cmp_sf(SoftFloat a, SoftFloat b){
    int t= a.exp - b.exp;
    if (t <-31) return - b.mant ;
    else if (t < 0) return (a.mant >> (-t)) - b.mant ;
    else if (t < 32) return a.mant - (b.mant >> t);
    else return a.mant ;
}





static inline __attribute__((const)) int av_gt_sf(SoftFloat a, SoftFloat b)
{
    int t= a.exp - b.exp;
    if (t <-31) return 0 > b.mant ;
    else if (t < 0) return (a.mant >> (-t)) > b.mant ;
    else if (t < 32) return a.mant > (b.mant >> t);
    else return a.mant > 0 ;
}




static inline __attribute__((const)) SoftFloat av_add_sf(SoftFloat a, SoftFloat b){
    int t= a.exp - b.exp;
    if (t <-31) return b;
    else if (t < 0) return av_normalize_sf(av_normalize1_sf((SoftFloat){ b.mant + (a.mant >> (-t)), b.exp}));
    else if (t < 32) return av_normalize_sf(av_normalize1_sf((SoftFloat){ a.mant + (b.mant >> t ), a.exp}));
    else return a;
}




static inline __attribute__((const)) SoftFloat av_sub_sf(SoftFloat a, SoftFloat b){
    return av_add_sf(a, (SoftFloat){ -b.mant, b.exp});
}
static inline __attribute__((const)) SoftFloat av_int2sf(int v, int frac_bits){
    int exp_offset = 0;
    if(v <= 
           (-0x7fffffff - 1) 
                   + 1){
        exp_offset = 1;
        v>>=1;
    }
    return av_normalize_sf(av_normalize1_sf((SoftFloat){v, 29 + 1 - frac_bits + exp_offset}));
}





static inline __attribute__((const)) int av_sf2int(SoftFloat v, int frac_bits){
    v.exp += frac_bits - (29 + 1);
    if(v.exp >= 0) return v.mant << v.exp ;
    else return v.mant >>(-v.exp);
}




static __attribute__((always_inline)) inline SoftFloat av_sqrt_sf(SoftFloat val)
{
    int tabIndex, rem;

    if (val.mant == 0)
        val.exp = -149;
    else if (val.mant < 0)
        abort();
    else
    {
        tabIndex = (val.mant - 0x20000000) >> 20;

        rem = val.mant & 0xFFFFF;
        val.mant = (int)(((int64_t)av_sqrttbl_sf[tabIndex] * (0x100000 - rem) +
                           (int64_t)av_sqrttbl_sf[tabIndex + 1] * rem +
                           0x80000) >> 20);
        val.mant = (int)(((int64_t)av_sqr_exp_multbl_sf[val.exp & 1] * val.mant +
                          0x10000000) >> 29);

        if (val.mant < 0x40000000)
            val.exp -= 2;
        else
            val.mant >>= 1;

        val.exp = (val.exp >> 1) + 1;
    }

    return val;
}




static __attribute__((unused)) void av_sincos_sf(int a, int *s, int *c)
{
    int idx, sign;
    int sv, cv;
    int st, ct;

    idx = a >> 26;
    sign = (int32_t)((unsigned)idx << 27) >> 31;
    cv = av_costbl_1_sf[idx & 0xf];
    cv = (cv ^ sign) - sign;

    idx -= 8;
    sign = (int32_t)((unsigned)idx << 27) >> 31;
    sv = av_costbl_1_sf[idx & 0xf];
    sv = (sv ^ sign) - sign;

    idx = a >> 21;
    ct = av_costbl_2_sf[idx & 0x1f];
    st = av_sintbl_2_sf[idx & 0x1f];

    idx = (int)(((int64_t)cv * ct - (int64_t)sv * st + 0x20000000) >> 30);

    sv = (int)(((int64_t)cv * st + (int64_t)sv * ct + 0x20000000) >> 30);

    cv = idx;

    idx = a >> 16;
    ct = av_costbl_3_sf[idx & 0x1f];
    st = av_sintbl_3_sf[idx & 0x1f];

    idx = (int)(((int64_t)cv * ct - (int64_t)sv * st + 0x20000000) >> 30);

    sv = (int)(((int64_t)cv * st + (int64_t)sv * ct + 0x20000000) >> 30);
    cv = idx;

    idx = a >> 11;

    ct = (int)(((int64_t)av_costbl_4_sf[idx & 0x1f] * (0x800 - (a & 0x7ff)) +
                (int64_t)av_costbl_4_sf[(idx & 0x1f)+1]*(a & 0x7ff) +
                0x400) >> 11);
    st = (int)(((int64_t)av_sintbl_4_sf[idx & 0x1f] * (0x800 - (a & 0x7ff)) +
                (int64_t)av_sintbl_4_sf[(idx & 0x1f) + 1] * (a & 0x7ff) +
                0x400) >> 11);

    *c = (int)(((int64_t)cv * ct + (int64_t)sv * st + 0x20000000) >> 30);

    *s = (int)(((int64_t)cv * st + (int64_t)sv * ct + 0x20000000) >> 30);
}

typedef struct SBRDSPContext {
    void (*sum64x5)(INTFLOAT *z);
    AAC_FLOAT (*sum_square)(INTFLOAT (*x)[2], int n);
    void (*neg_odd_64)(INTFLOAT *x);
    void (*qmf_pre_shuffle)(INTFLOAT *z);
    void (*qmf_post_shuffle)(INTFLOAT W[32][2], const INTFLOAT *z);
    void (*qmf_deint_neg)(INTFLOAT *v, const INTFLOAT *src);
    void (*qmf_deint_bfly)(INTFLOAT *v, const INTFLOAT *src0, const INTFLOAT *src1);
    void (*autocorrelate)(const INTFLOAT x[40][2], AAC_FLOAT phi[3][2][2]);
    void (*hf_gen)(INTFLOAT (*X_high)[2], const INTFLOAT (*X_low)[2],
                   const INTFLOAT alpha0[2], const INTFLOAT alpha1[2],
                   INTFLOAT bw, int start, int end);
    void (*hf_g_filt)(INTFLOAT (*Y)[2], const INTFLOAT (*X_high)[40][2],
                      const AAC_FLOAT *g_filt, int m_max, intptr_t ixh);
    void (*hf_apply_noise[4])(INTFLOAT (*Y)[2], const AAC_FLOAT *s_m,
                              const AAC_FLOAT *q_filt, int noise,
                              int kx, int m_max);
} SBRDSPContext;

extern const INTFLOAT ff_sbr_noise_table[][2];

void ff_sbrdsp_init(SBRDSPContext *s);
void ff_sbrdsp_init_arm(SBRDSPContext *s);
void ff_sbrdsp_init_aarch64(SBRDSPContext *s);
void ff_sbrdsp_init_x86(SBRDSPContext *s);
void ff_sbrdsp_init_mips(SBRDSPContext *s);

typedef struct AACContext AACContext;




typedef struct SpectrumParameters {
    uint8_t bs_start_freq;
    uint8_t bs_stop_freq;
    uint8_t bs_xover_band;





    uint8_t bs_freq_scale;
    uint8_t bs_alter_scale;
    uint8_t bs_noise_bands;

} SpectrumParameters;






typedef struct SBRData {




    unsigned bs_frame_class;
    unsigned bs_add_harmonic_flag;
    AAC_SIGNE bs_num_env;
    uint8_t bs_freq_res[7];
    AAC_SIGNE bs_num_noise;
    uint8_t bs_df_env[5];
    uint8_t bs_df_noise[2];
    uint8_t bs_invf_mode[2][5];
    uint8_t bs_add_harmonic[48];
    unsigned bs_amp_res;






    INTFLOAT __attribute__ ((aligned (32))) synthesis_filterbank_samples[((1280-128)*2)];
    INTFLOAT __attribute__ ((aligned (32))) analysis_filterbank_samples [1312];
    int synthesis_filterbank_samples_offset;

    int e_a[2];

    INTFLOAT bw_array[5];

    INTFLOAT W[2][32][32][2];

    int Ypos;
    INTFLOAT __attribute__ ((aligned (16))) Y[2][38][64][2];
    AAC_FLOAT __attribute__ ((aligned (16))) g_temp[42][48];
    AAC_FLOAT q_temp[42][48];
    uint8_t s_indexmapped[8][48];

    uint8_t env_facs_q[6][48];
    AAC_FLOAT env_facs[6][48];

    uint8_t noise_facs_q[3][5];
    AAC_FLOAT noise_facs[3][5];

    uint8_t t_env[8];

    uint8_t t_env_num_env_old;

    uint8_t t_q[3];
    unsigned f_indexnoise;
    unsigned f_indexsine;

} SBRData;

typedef struct SpectralBandReplication SpectralBandReplication;




typedef struct AACSBRContext {
    int (*sbr_lf_gen)(AACContext *ac, SpectralBandReplication *sbr,
                      INTFLOAT X_low[32][40][2], const INTFLOAT W[2][32][32][2],
                      int buf_idx);
    void (*sbr_hf_assemble)(INTFLOAT Y1[38][64][2],
                            const INTFLOAT X_high[64][40][2],
                            SpectralBandReplication *sbr, SBRData *ch_data,
                            const int e_a[2]);
    int (*sbr_x_gen)(SpectralBandReplication *sbr, INTFLOAT X[2][38][64],
                     const INTFLOAT Y0[38][64][2], const INTFLOAT Y1[38][64][2],
                     const INTFLOAT X_low[32][40][2], int ch);
    void (*sbr_hf_inverse_filter)(SBRDSPContext *dsp,
                                  INTFLOAT (*alpha0)[2], INTFLOAT (*alpha1)[2],
                                  const INTFLOAT X_low[32][40][2], int k0);
} AACSBRContext;




struct SpectralBandReplication {
    int sample_rate;
    int start;
    int ready_for_dequant;
    int id_aac;
    int reset;
    SpectrumParameters spectrum_params;
    int bs_amp_res_header;




    unsigned bs_limiter_bands;
    unsigned bs_limiter_gains;
    unsigned bs_interpol_freq;
    unsigned bs_smoothing_mode;

    unsigned bs_coupling;
    AAC_SIGNE k[5];


    AAC_SIGNE kx[2];

    AAC_SIGNE m[2];
    unsigned kx_and_m_pushed;

    AAC_SIGNE n_master;
    SBRData data[2];
    PSContext ps;

    AAC_SIGNE n[2];

    AAC_SIGNE n_q;

    AAC_SIGNE n_lim;

    uint16_t f_master[49];

    uint16_t f_tablelow[25];

    uint16_t f_tablehigh[49];

    uint16_t f_tablenoise[6];

    uint16_t f_tablelim[30];
    AAC_SIGNE num_patches;
    uint8_t patch_num_subbands[6];
    uint8_t patch_start_subband[6];

    INTFLOAT __attribute__ ((aligned (16))) X_low[32][40][2];

    INTFLOAT __attribute__ ((aligned (16))) X_high[64][40][2];

    INTFLOAT __attribute__ ((aligned (16))) X[2][2][38][64];

    INTFLOAT __attribute__ ((aligned (16))) alpha0[64][2];

    INTFLOAT __attribute__ ((aligned (16))) alpha1[64][2];

    AAC_FLOAT e_origmapped[7][48];

    AAC_FLOAT q_mapped[7][48];

    uint8_t s_mapped[7][48];

    AAC_FLOAT e_curr[7][48];

    AAC_FLOAT q_m[7][48];

    AAC_FLOAT s_m[7][48];
    AAC_FLOAT gain[7][48];
    INTFLOAT __attribute__ ((aligned (32))) qmf_filter_scratch[5][64];
    FFTContext mdct_ana;
    FFTContext mdct;
    SBRDSPContext dsp;
    AACSBRContext c;
};
enum RawDataBlockType {
    TYPE_SCE,
    TYPE_CPE,
    TYPE_CCE,
    TYPE_LFE,
    TYPE_DSE,
    TYPE_PCE,
    TYPE_FIL,
    TYPE_END,
};

enum ExtensionPayloadID {
    EXT_FILL,
    EXT_FILL_DATA,
    EXT_DATA_ELEMENT,
    EXT_DYNAMIC_RANGE = 0xb,
    EXT_SBR_DATA = 0xd,
    EXT_SBR_DATA_CRC = 0xe,
};

enum WindowSequence {
    ONLY_LONG_SEQUENCE,
    LONG_START_SEQUENCE,
    EIGHT_SHORT_SEQUENCE,
    LONG_STOP_SEQUENCE,
};

enum BandType {
    ZERO_BT = 0,
    FIRST_PAIR_BT = 5,
    ESC_BT = 11,
    RESERVED_BT = 12,
    NOISE_BT = 13,
    INTENSITY_BT2 = 14,
    INTENSITY_BT = 15,
};



enum ChannelPosition {
    AAC_CHANNEL_OFF = 0,
    AAC_CHANNEL_FRONT = 1,
    AAC_CHANNEL_SIDE = 2,
    AAC_CHANNEL_BACK = 3,
    AAC_CHANNEL_LFE = 4,
    AAC_CHANNEL_CC = 5,
};




enum CouplingPoint {
    BEFORE_TNS,
    BETWEEN_TNS_AND_IMDCT,
    AFTER_IMDCT = 3,
};




enum OCStatus {
    OC_NONE,
    OC_TRIAL_PCE,
    OC_TRIAL_FRAME,
    OC_GLOBAL_HDR,
    OC_LOCKED,
};

typedef struct OutputConfiguration {
    MPEG4AudioConfig m4ac;
    uint8_t layout_map[16*4][3];
    int layout_map_tags;
    int channels;
    uint64_t channel_layout;
    enum OCStatus status;
} OutputConfiguration;




typedef struct PredictorState {
    AAC_FLOAT cor0;
    AAC_FLOAT cor1;
    AAC_FLOAT var0;
    AAC_FLOAT var1;
    AAC_FLOAT r0;
    AAC_FLOAT r1;
    AAC_FLOAT k1;
    AAC_FLOAT x_est;
} PredictorState;
typedef struct LongTermPrediction {
    int8_t present;
    int16_t lag;
    int coef_idx;
    INTFLOAT coef;
    int8_t used[40];
} LongTermPrediction;




typedef struct IndividualChannelStream {
    uint8_t max_sfb;
    enum WindowSequence window_sequence[2];
    uint8_t use_kb_window[2];
    int num_window_groups;
    uint8_t group_len[8];
    LongTermPrediction ltp;
    const uint16_t *swb_offset;
    const uint8_t *swb_sizes;
    int num_swb;
    int num_windows;
    int tns_max_bands;
    int predictor_present;
    int predictor_initialized;
    int predictor_reset_group;
    int predictor_reset_count[31];
    uint8_t prediction_used[41];
    uint8_t window_clipping[8];
    float clip_avoidance_factor;
} IndividualChannelStream;




typedef struct TemporalNoiseShaping {
    int present;
    int n_filt[8];
    int length[8][4];
    int direction[8][4];
    int order[8][4];
    int coef_idx[8][4][20];
    INTFLOAT coef[8][4][20];
} TemporalNoiseShaping;




typedef struct DynamicRangeControl {
    int pce_instance_tag;
    int dyn_rng_sgn[17];
    int dyn_rng_ctl[17];
    int exclude_mask[64];
    int band_incr;
    int interpolation_scheme;
    int band_top[17];
    int prog_ref_level;


} DynamicRangeControl;

typedef struct Pulse {
    int num_pulse;
    int start;
    int pos[4];
    int amp[4];
} Pulse;




typedef struct ChannelCoupling {
    enum CouplingPoint coupling_point;
    int num_coupled;
    enum RawDataBlockType type[8];
    int id_select[8];
    int ch_select[8];


    INTFLOAT gain[16][120];
} ChannelCoupling;




typedef struct SingleChannelElement {
    IndividualChannelStream ics;
    TemporalNoiseShaping tns;
    Pulse pulse;
    enum BandType band_type[128];
    enum BandType band_alt[128];
    int band_type_run_end[120];
    INTFLOAT sf[120];
    int sf_idx[128];
    uint8_t zeroes[128];
    uint8_t can_pns[128];
    float is_ener[128];
    float pns_ener[128];
    INTFLOAT __attribute__ ((aligned (32))) pcoeffs[1024];
    INTFLOAT __attribute__ ((aligned (32))) coeffs[1024];
    INTFLOAT __attribute__ ((aligned (32))) saved[1536];
    INTFLOAT __attribute__ ((aligned (32))) ret_buf[2048];
    INTFLOAT __attribute__ ((aligned (16))) ltp_state[3072];
    AAC_FLOAT __attribute__ ((aligned (32))) lcoeffs[1024];
    AAC_FLOAT __attribute__ ((aligned (32))) prcoeffs[1024];
    PredictorState predictor_state[672];
    INTFLOAT *ret;
} SingleChannelElement;




typedef struct ChannelElement {
    int present;

    int common_window;
    int ms_mode;
    uint8_t is_mode;
    uint8_t ms_mask[128];
    uint8_t is_mask[128];

    SingleChannelElement ch[2];

    ChannelCoupling coup;
    SpectralBandReplication sbr;
} ChannelElement;




struct AACContext {
    AVClass *class;
    AVCodecContext *avctx;
    AVFrame *frame;

    int is_saved;
    DynamicRangeControl che_drc;





    ChannelElement *che[4][16];
    ChannelElement *tag_che_map[4][16];
    int tags_mapped;
    int warned_remapping_once;







    INTFLOAT __attribute__ ((aligned (32))) buf_mdct[1024];






    FFTContext mdct;
    FFTContext mdct_small;
    FFTContext mdct_ld;
    FFTContext mdct_ltp;



    MDCT15Context *mdct120;
    MDCT15Context *mdct480;
    MDCT15Context *mdct960;
    AVFloatDSPContext *fdsp;

    int random_state;






    SingleChannelElement *output_element[64];







    int force_dmono_mode;
    int dmono_mode;


    INTFLOAT __attribute__ ((aligned (32))) temp[128];

    OutputConfiguration oc[2];
    int warned_num_aac_frames;
    int warned_960_sbr;
    unsigned warned_71_wide;
    int warned_gain_control;


    void (*imdct_and_windowing)(AACContext *ac, SingleChannelElement *sce);
    void (*apply_ltp)(AACContext *ac, SingleChannelElement *sce);
    void (*apply_tns)(INTFLOAT coef[1024], TemporalNoiseShaping *tns,
                      IndividualChannelStream *ics, int decode);
    void (*windowing_and_mdct_ltp)(AACContext *ac, INTFLOAT *out,
                                   INTFLOAT *in, IndividualChannelStream *ics);
    void (*update_ltp)(AACContext *ac, SingleChannelElement *sce);
    void (*vector_pow43)(int *coefs, int len);
    void (*subband_scale)(int *dst, int *src, int scale, int offset, int len, void *log_context);

};

void ff_aacdec_init_mips(AACContext *c);
extern float ff_aac_pow2sf_tab[428];
extern float ff_aac_pow34sf_tab[428];

void ff_aac_tableinit(void);




static const INTFLOAT ltp_coef[8] = {
    ((float)(0.570829)), ((float)(0.696616)), ((float)(0.813004)), ((float)(0.911304)),
    ((float)(0.984900)), ((float)(1.067894)), ((float)(1.194601)), ((float)(1.369533)),
};







static const INTFLOAT tns_tmp2_map_1_3[4] = {
    ((float)(0.00000000)), ((float)(-0.43388373)), ((float)(0.64278758)), ((float)(0.34202015)),
};

static const INTFLOAT tns_tmp2_map_0_3[8] = {
    ((float)(0.00000000)), ((float)(-0.43388373)), ((float)(-0.78183150)), ((float)(-0.97492790)),
    ((float)(0.98480773)), ((float)(0.86602539)), ((float)(0.64278758)), ((float)(0.34202015)),
};

static const INTFLOAT tns_tmp2_map_1_4[8] = {
    ((float)(0.00000000)), ((float)(-0.20791170)), ((float)(-0.40673664)), ((float)(-0.58778524)),
    ((float)(0.67369562)), ((float)(0.52643216)), ((float)(0.36124167)), ((float)(0.18374951)),
};

static const INTFLOAT tns_tmp2_map_0_4[16] = {
    ((float)(0.00000000)), ((float)(-0.20791170)), ((float)(-0.40673664)), ((float)(-0.58778524)),
    ((float)(-0.74314481)), ((float)(-0.86602539)), ((float)(-0.95105654)), ((float)(-0.99452192)),
    ((float)(0.99573416)), ((float)(0.96182561)), ((float)(0.89516330)), ((float)(0.79801720)),
    ((float)(0.67369562)), ((float)(0.52643216)), ((float)(0.36124167)), ((float)(0.18374951)),
};

static const INTFLOAT * const tns_tmp2_map[4] = {
    tns_tmp2_map_0_3,
    tns_tmp2_map_0_4,
    tns_tmp2_map_1_3,
    tns_tmp2_map_1_4
};





extern float __attribute__ ((aligned (32))) ff_aac_kbd_long_1024[1024];
extern float __attribute__ ((aligned (32))) ff_aac_kbd_short_128[128];
extern const float __attribute__ ((aligned (32))) ff_aac_eld_window_512[1920];
extern const int __attribute__ ((aligned (32))) ff_aac_eld_window_512_fixed[1920];
extern const float __attribute__ ((aligned (32))) ff_aac_eld_window_480[1800];
extern const int __attribute__ ((aligned (32))) ff_aac_eld_window_480_fixed[1800];



void ff_aac_float_common_init(void);




extern const uint8_t ff_aac_num_swb_1024[];
extern const uint8_t ff_aac_num_swb_960 [];
extern const uint8_t ff_aac_num_swb_512 [];
extern const uint8_t ff_aac_num_swb_480 [];
extern const uint8_t ff_aac_num_swb_128 [];
extern const uint8_t ff_aac_num_swb_120 [];


extern const uint8_t ff_aac_pred_sfb_max [];

extern const uint32_t ff_aac_scalefactor_code[121];
extern const uint8_t ff_aac_scalefactor_bits[121];

extern const uint16_t * const ff_aac_spectral_codes[11];
extern const uint8_t * const ff_aac_spectral_bits [11];
extern const uint16_t ff_aac_spectral_sizes[11];

extern const float *const ff_aac_codebook_vectors[];
extern const float *const ff_aac_codebook_vector_vals[];
extern const uint16_t *const ff_aac_codebook_vector_idx[];

extern const uint16_t * const ff_swb_offset_1024[13];
extern const uint16_t * const ff_swb_offset_960 [13];
extern const uint16_t * const ff_swb_offset_512 [13];
extern const uint16_t * const ff_swb_offset_480 [13];
extern const uint16_t * const ff_swb_offset_128 [13];
extern const uint16_t * const ff_swb_offset_120 [13];

extern const uint8_t ff_tns_max_bands_1024[13];
extern const uint8_t ff_tns_max_bands_512 [13];
extern const uint8_t ff_tns_max_bands_480 [13];
extern const uint8_t ff_tns_max_bands_128 [13];



float ff_aac_pow2sf_tab[428];
float ff_aac_pow34sf_tab[428];





float __attribute__ ((aligned (32))) ff_aac_kbd_long_1024[1024];
float __attribute__ ((aligned (32))) ff_aac_kbd_short_128[128];

static __attribute__((cold)) void aac_float_common_init(void)
{
    ff_kbd_window_init(ff_aac_kbd_long_1024, 4.0, 1024);
    ff_kbd_window_init(ff_aac_kbd_short_128, 6.0, 128);
    ff_init_ff_sine_windows(10);
    ff_init_ff_sine_windows(7);
}

__attribute__((cold)) void ff_aac_float_common_init(void)
{
    static pthread_once_t init_static_once = 
                                    0
                                                ;
    pthread_once(&init_static_once, aac_float_common_init);
}


const uint8_t ff_aac_num_swb_1024[] = {
    41, 41, 47, 49, 49, 51, 47, 47, 43, 43, 43, 40, 40
};

const uint8_t ff_aac_num_swb_960[] = {
    40, 40, 46, 49, 49, 49, 46, 46, 42, 42, 42, 40, 40
};

const uint8_t ff_aac_num_swb_512[] = {
     0, 0, 0, 36, 36, 37, 31, 31, 0, 0, 0, 0, 0
};

const uint8_t ff_aac_num_swb_480[] = {
     0, 0, 0, 35, 35, 37, 30, 30, 0, 0, 0, 0, 0
};

const uint8_t ff_aac_num_swb_128[] = {
    12, 12, 12, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15
};

const uint8_t ff_aac_num_swb_120[] = {
    12, 12, 12, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15
};

const uint8_t ff_aac_pred_sfb_max[] = {
    33, 33, 38, 40, 40, 40, 41, 41, 37, 37, 37, 34, 34
};

const uint32_t ff_aac_scalefactor_code[121] = {
    0x3ffe8, 0x3ffe6, 0x3ffe7, 0x3ffe5, 0x7fff5, 0x7fff1, 0x7ffed, 0x7fff6,
    0x7ffee, 0x7ffef, 0x7fff0, 0x7fffc, 0x7fffd, 0x7ffff, 0x7fffe, 0x7fff7,
    0x7fff8, 0x7fffb, 0x7fff9, 0x3ffe4, 0x7fffa, 0x3ffe3, 0x1ffef, 0x1fff0,
    0x0fff5, 0x1ffee, 0x0fff2, 0x0fff3, 0x0fff4, 0x0fff1, 0x07ff6, 0x07ff7,
    0x03ff9, 0x03ff5, 0x03ff7, 0x03ff3, 0x03ff6, 0x03ff2, 0x01ff7, 0x01ff5,
    0x00ff9, 0x00ff7, 0x00ff6, 0x007f9, 0x00ff4, 0x007f8, 0x003f9, 0x003f7,
    0x003f5, 0x001f8, 0x001f7, 0x000fa, 0x000f8, 0x000f6, 0x00079, 0x0003a,
    0x00038, 0x0001a, 0x0000b, 0x00004, 0x00000, 0x0000a, 0x0000c, 0x0001b,
    0x00039, 0x0003b, 0x00078, 0x0007a, 0x000f7, 0x000f9, 0x001f6, 0x001f9,
    0x003f4, 0x003f6, 0x003f8, 0x007f5, 0x007f4, 0x007f6, 0x007f7, 0x00ff5,
    0x00ff8, 0x01ff4, 0x01ff6, 0x01ff8, 0x03ff8, 0x03ff4, 0x0fff0, 0x07ff4,
    0x0fff6, 0x07ff5, 0x3ffe2, 0x7ffd9, 0x7ffda, 0x7ffdb, 0x7ffdc, 0x7ffdd,
    0x7ffde, 0x7ffd8, 0x7ffd2, 0x7ffd3, 0x7ffd4, 0x7ffd5, 0x7ffd6, 0x7fff2,
    0x7ffdf, 0x7ffe7, 0x7ffe8, 0x7ffe9, 0x7ffea, 0x7ffeb, 0x7ffe6, 0x7ffe0,
    0x7ffe1, 0x7ffe2, 0x7ffe3, 0x7ffe4, 0x7ffe5, 0x7ffd7, 0x7ffec, 0x7fff4,
    0x7fff3,
};

const uint8_t ff_aac_scalefactor_bits[121] = {
    18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 18, 19, 18, 17, 17, 16, 17, 16, 16, 16, 16, 15, 15,
    14, 14, 14, 14, 14, 14, 13, 13, 12, 12, 12, 11, 12, 11, 10, 10,
    10, 9, 9, 8, 8, 8, 7, 6, 6, 5, 4, 3, 1, 4, 4, 5,
     6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 10, 11, 11, 11, 11, 12,
    12, 13, 13, 13, 14, 14, 16, 15, 16, 15, 18, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19,
};

static const uint16_t codes1[81] = {
    0x7f8, 0x1f1, 0x7fd, 0x3f5, 0x068, 0x3f0, 0x7f7, 0x1ec,
    0x7f5, 0x3f1, 0x072, 0x3f4, 0x074, 0x011, 0x076, 0x1eb,
    0x06c, 0x3f6, 0x7fc, 0x1e1, 0x7f1, 0x1f0, 0x061, 0x1f6,
    0x7f2, 0x1ea, 0x7fb, 0x1f2, 0x069, 0x1ed, 0x077, 0x017,
    0x06f, 0x1e6, 0x064, 0x1e5, 0x067, 0x015, 0x062, 0x012,
    0x000, 0x014, 0x065, 0x016, 0x06d, 0x1e9, 0x063, 0x1e4,
    0x06b, 0x013, 0x071, 0x1e3, 0x070, 0x1f3, 0x7fe, 0x1e7,
    0x7f3, 0x1ef, 0x060, 0x1ee, 0x7f0, 0x1e2, 0x7fa, 0x3f3,
    0x06a, 0x1e8, 0x075, 0x010, 0x073, 0x1f4, 0x06e, 0x3f7,
    0x7f6, 0x1e0, 0x7f9, 0x3f2, 0x066, 0x1f5, 0x7ff, 0x1f7,
    0x7f4,
};

static const uint8_t bits1[81] = {
    11, 9, 11, 10, 7, 10, 11, 9, 11, 10, 7, 10, 7, 5, 7, 9,
     7, 10, 11, 9, 11, 9, 7, 9, 11, 9, 11, 9, 7, 9, 7, 5,
     7, 9, 7, 9, 7, 5, 7, 5, 1, 5, 7, 5, 7, 9, 7, 9,
     7, 5, 7, 9, 7, 9, 11, 9, 11, 9, 7, 9, 11, 9, 11, 10,
     7, 9, 7, 5, 7, 9, 7, 10, 11, 9, 11, 10, 7, 9, 11, 9,
    11,
};

static const uint16_t codes2[81] = {
    0x1f3, 0x06f, 0x1fd, 0x0eb, 0x023, 0x0ea, 0x1f7, 0x0e8,
    0x1fa, 0x0f2, 0x02d, 0x070, 0x020, 0x006, 0x02b, 0x06e,
    0x028, 0x0e9, 0x1f9, 0x066, 0x0f8, 0x0e7, 0x01b, 0x0f1,
    0x1f4, 0x06b, 0x1f5, 0x0ec, 0x02a, 0x06c, 0x02c, 0x00a,
    0x027, 0x067, 0x01a, 0x0f5, 0x024, 0x008, 0x01f, 0x009,
    0x000, 0x007, 0x01d, 0x00b, 0x030, 0x0ef, 0x01c, 0x064,
    0x01e, 0x00c, 0x029, 0x0f3, 0x02f, 0x0f0, 0x1fc, 0x071,
    0x1f2, 0x0f4, 0x021, 0x0e6, 0x0f7, 0x068, 0x1f8, 0x0ee,
    0x022, 0x065, 0x031, 0x002, 0x026, 0x0ed, 0x025, 0x06a,
    0x1fb, 0x072, 0x1fe, 0x069, 0x02e, 0x0f6, 0x1ff, 0x06d,
    0x1f6,
};

static const uint8_t bits2[81] = {
    9, 7, 9, 8, 6, 8, 9, 8, 9, 8, 6, 7, 6, 5, 6, 7,
    6, 8, 9, 7, 8, 8, 6, 8, 9, 7, 9, 8, 6, 7, 6, 5,
    6, 7, 6, 8, 6, 5, 6, 5, 3, 5, 6, 5, 6, 8, 6, 7,
    6, 5, 6, 8, 6, 8, 9, 7, 9, 8, 6, 8, 8, 7, 9, 8,
    6, 7, 6, 4, 6, 8, 6, 7, 9, 7, 9, 7, 6, 8, 9, 7,
    9,
};

static const uint16_t codes3[81] = {
    0x0000, 0x0009, 0x00ef, 0x000b, 0x0019, 0x00f0, 0x01eb, 0x01e6,
    0x03f2, 0x000a, 0x0035, 0x01ef, 0x0034, 0x0037, 0x01e9, 0x01ed,
    0x01e7, 0x03f3, 0x01ee, 0x03ed, 0x1ffa, 0x01ec, 0x01f2, 0x07f9,
    0x07f8, 0x03f8, 0x0ff8, 0x0008, 0x0038, 0x03f6, 0x0036, 0x0075,
    0x03f1, 0x03eb, 0x03ec, 0x0ff4, 0x0018, 0x0076, 0x07f4, 0x0039,
    0x0074, 0x03ef, 0x01f3, 0x01f4, 0x07f6, 0x01e8, 0x03ea, 0x1ffc,
    0x00f2, 0x01f1, 0x0ffb, 0x03f5, 0x07f3, 0x0ffc, 0x00ee, 0x03f7,
    0x7ffe, 0x01f0, 0x07f5, 0x7ffd, 0x1ffb, 0x3ffa, 0xffff, 0x00f1,
    0x03f0, 0x3ffc, 0x01ea, 0x03ee, 0x3ffb, 0x0ff6, 0x0ffa, 0x7ffc,
    0x07f2, 0x0ff5, 0xfffe, 0x03f4, 0x07f7, 0x7ffb, 0x0ff7, 0x0ff9,
    0x7ffa,
};

static const uint8_t bits3[81] = {
     1, 4, 8, 4, 5, 8, 9, 9, 10, 4, 6, 9, 6, 6, 9, 9,
     9, 10, 9, 10, 13, 9, 9, 11, 11, 10, 12, 4, 6, 10, 6, 7,
    10, 10, 10, 12, 5, 7, 11, 6, 7, 10, 9, 9, 11, 9, 10, 13,
     8, 9, 12, 10, 11, 12, 8, 10, 15, 9, 11, 15, 13, 14, 16, 8,
    10, 14, 9, 10, 14, 12, 12, 15, 11, 12, 16, 10, 11, 15, 12, 12,
    15,
};

static const uint16_t codes4[81] = {
    0x007, 0x016, 0x0f6, 0x018, 0x008, 0x0ef, 0x1ef, 0x0f3,
    0x7f8, 0x019, 0x017, 0x0ed, 0x015, 0x001, 0x0e2, 0x0f0,
    0x070, 0x3f0, 0x1ee, 0x0f1, 0x7fa, 0x0ee, 0x0e4, 0x3f2,
    0x7f6, 0x3ef, 0x7fd, 0x005, 0x014, 0x0f2, 0x009, 0x004,
    0x0e5, 0x0f4, 0x0e8, 0x3f4, 0x006, 0x002, 0x0e7, 0x003,
    0x000, 0x06b, 0x0e3, 0x069, 0x1f3, 0x0eb, 0x0e6, 0x3f6,
    0x06e, 0x06a, 0x1f4, 0x3ec, 0x1f0, 0x3f9, 0x0f5, 0x0ec,
    0x7fb, 0x0ea, 0x06f, 0x3f7, 0x7f9, 0x3f3, 0xfff, 0x0e9,
    0x06d, 0x3f8, 0x06c, 0x068, 0x1f5, 0x3ee, 0x1f2, 0x7f4,
    0x7f7, 0x3f1, 0xffe, 0x3ed, 0x1f1, 0x7f5, 0x7fe, 0x3f5,
    0x7fc,
};

static const uint8_t bits4[81] = {
     4, 5, 8, 5, 4, 8, 9, 8, 11, 5, 5, 8, 5, 4, 8, 8,
     7, 10, 9, 8, 11, 8, 8, 10, 11, 10, 11, 4, 5, 8, 4, 4,
     8, 8, 8, 10, 4, 4, 8, 4, 4, 7, 8, 7, 9, 8, 8, 10,
     7, 7, 9, 10, 9, 10, 8, 8, 11, 8, 7, 10, 11, 10, 12, 8,
     7, 10, 7, 7, 9, 10, 9, 11, 11, 10, 12, 10, 9, 11, 11, 10,
    11,
};

static const uint16_t codes5[81] = {
    0x1fff, 0x0ff7, 0x07f4, 0x07e8, 0x03f1, 0x07ee, 0x07f9, 0x0ff8,
    0x1ffd, 0x0ffd, 0x07f1, 0x03e8, 0x01e8, 0x00f0, 0x01ec, 0x03ee,
    0x07f2, 0x0ffa, 0x0ff4, 0x03ef, 0x01f2, 0x00e8, 0x0070, 0x00ec,
    0x01f0, 0x03ea, 0x07f3, 0x07eb, 0x01eb, 0x00ea, 0x001a, 0x0008,
    0x0019, 0x00ee, 0x01ef, 0x07ed, 0x03f0, 0x00f2, 0x0073, 0x000b,
    0x0000, 0x000a, 0x0071, 0x00f3, 0x07e9, 0x07ef, 0x01ee, 0x00ef,
    0x0018, 0x0009, 0x001b, 0x00eb, 0x01e9, 0x07ec, 0x07f6, 0x03eb,
    0x01f3, 0x00ed, 0x0072, 0x00e9, 0x01f1, 0x03ed, 0x07f7, 0x0ff6,
    0x07f0, 0x03e9, 0x01ed, 0x00f1, 0x01ea, 0x03ec, 0x07f8, 0x0ff9,
    0x1ffc, 0x0ffc, 0x0ff5, 0x07ea, 0x03f3, 0x03f2, 0x07f5, 0x0ffb,
    0x1ffe,
};

static const uint8_t bits5[81] = {
    13, 12, 11, 11, 10, 11, 11, 12, 13, 12, 11, 10, 9, 8, 9, 10,
    11, 12, 12, 10, 9, 8, 7, 8, 9, 10, 11, 11, 9, 8, 5, 4,
     5, 8, 9, 11, 10, 8, 7, 4, 1, 4, 7, 8, 11, 11, 9, 8,
     5, 4, 5, 8, 9, 11, 11, 10, 9, 8, 7, 8, 9, 10, 11, 12,
    11, 10, 9, 8, 9, 10, 11, 12, 13, 12, 12, 11, 10, 10, 11, 12,
    13,
};

static const uint16_t codes6[81] = {
    0x7fe, 0x3fd, 0x1f1, 0x1eb, 0x1f4, 0x1ea, 0x1f0, 0x3fc,
    0x7fd, 0x3f6, 0x1e5, 0x0ea, 0x06c, 0x071, 0x068, 0x0f0,
    0x1e6, 0x3f7, 0x1f3, 0x0ef, 0x032, 0x027, 0x028, 0x026,
    0x031, 0x0eb, 0x1f7, 0x1e8, 0x06f, 0x02e, 0x008, 0x004,
    0x006, 0x029, 0x06b, 0x1ee, 0x1ef, 0x072, 0x02d, 0x002,
    0x000, 0x003, 0x02f, 0x073, 0x1fa, 0x1e7, 0x06e, 0x02b,
    0x007, 0x001, 0x005, 0x02c, 0x06d, 0x1ec, 0x1f9, 0x0ee,
    0x030, 0x024, 0x02a, 0x025, 0x033, 0x0ec, 0x1f2, 0x3f8,
    0x1e4, 0x0ed, 0x06a, 0x070, 0x069, 0x074, 0x0f1, 0x3fa,
    0x7ff, 0x3f9, 0x1f6, 0x1ed, 0x1f8, 0x1e9, 0x1f5, 0x3fb,
    0x7fc,
};

static const uint8_t bits6[81] = {
    11, 10, 9, 9, 9, 9, 9, 10, 11, 10, 9, 8, 7, 7, 7, 8,
     9, 10, 9, 8, 6, 6, 6, 6, 6, 8, 9, 9, 7, 6, 4, 4,
     4, 6, 7, 9, 9, 7, 6, 4, 4, 4, 6, 7, 9, 9, 7, 6,
     4, 4, 4, 6, 7, 9, 9, 8, 6, 6, 6, 6, 6, 8, 9, 10,
     9, 8, 7, 7, 7, 7, 8, 10, 11, 10, 9, 9, 9, 9, 9, 10,
    11,
};

static const uint16_t codes7[64] = {
    0x000, 0x005, 0x037, 0x074, 0x0f2, 0x1eb, 0x3ed, 0x7f7,
    0x004, 0x00c, 0x035, 0x071, 0x0ec, 0x0ee, 0x1ee, 0x1f5,
    0x036, 0x034, 0x072, 0x0ea, 0x0f1, 0x1e9, 0x1f3, 0x3f5,
    0x073, 0x070, 0x0eb, 0x0f0, 0x1f1, 0x1f0, 0x3ec, 0x3fa,
    0x0f3, 0x0ed, 0x1e8, 0x1ef, 0x3ef, 0x3f1, 0x3f9, 0x7fb,
    0x1ed, 0x0ef, 0x1ea, 0x1f2, 0x3f3, 0x3f8, 0x7f9, 0x7fc,
    0x3ee, 0x1ec, 0x1f4, 0x3f4, 0x3f7, 0x7f8, 0xffd, 0xffe,
    0x7f6, 0x3f0, 0x3f2, 0x3f6, 0x7fa, 0x7fd, 0xffc, 0xfff,
};

static const uint8_t bits7[64] = {
     1, 3, 6, 7, 8, 9, 10, 11, 3, 4, 6, 7, 8, 8, 9, 9,
     6, 6, 7, 8, 8, 9, 9, 10, 7, 7, 8, 8, 9, 9, 10, 10,
     8, 8, 9, 9, 10, 10, 10, 11, 9, 8, 9, 9, 10, 10, 11, 11,
    10, 9, 9, 10, 10, 11, 12, 12, 11, 10, 10, 10, 11, 11, 12, 12,
};

static const uint16_t codes8[64] = {
    0x00e, 0x005, 0x010, 0x030, 0x06f, 0x0f1, 0x1fa, 0x3fe,
    0x003, 0x000, 0x004, 0x012, 0x02c, 0x06a, 0x075, 0x0f8,
    0x00f, 0x002, 0x006, 0x014, 0x02e, 0x069, 0x072, 0x0f5,
    0x02f, 0x011, 0x013, 0x02a, 0x032, 0x06c, 0x0ec, 0x0fa,
    0x071, 0x02b, 0x02d, 0x031, 0x06d, 0x070, 0x0f2, 0x1f9,
    0x0ef, 0x068, 0x033, 0x06b, 0x06e, 0x0ee, 0x0f9, 0x3fc,
    0x1f8, 0x074, 0x073, 0x0ed, 0x0f0, 0x0f6, 0x1f6, 0x1fd,
    0x3fd, 0x0f3, 0x0f4, 0x0f7, 0x1f7, 0x1fb, 0x1fc, 0x3ff,
};

static const uint8_t bits8[64] = {
     5, 4, 5, 6, 7, 8, 9, 10, 4, 3, 4, 5, 6, 7, 7, 8,
     5, 4, 4, 5, 6, 7, 7, 8, 6, 5, 5, 6, 6, 7, 8, 8,
     7, 6, 6, 6, 7, 7, 8, 9, 8, 7, 6, 7, 7, 8, 8, 10,
     9, 7, 7, 8, 8, 8, 9, 9, 10, 8, 8, 8, 9, 9, 9, 10,
};

static const uint16_t codes9[169] = {
    0x0000, 0x0005, 0x0037, 0x00e7, 0x01de, 0x03ce, 0x03d9, 0x07c8,
    0x07cd, 0x0fc8, 0x0fdd, 0x1fe4, 0x1fec, 0x0004, 0x000c, 0x0035,
    0x0072, 0x00ea, 0x00ed, 0x01e2, 0x03d1, 0x03d3, 0x03e0, 0x07d8,
    0x0fcf, 0x0fd5, 0x0036, 0x0034, 0x0071, 0x00e8, 0x00ec, 0x01e1,
    0x03cf, 0x03dd, 0x03db, 0x07d0, 0x0fc7, 0x0fd4, 0x0fe4, 0x00e6,
    0x0070, 0x00e9, 0x01dd, 0x01e3, 0x03d2, 0x03dc, 0x07cc, 0x07ca,
    0x07de, 0x0fd8, 0x0fea, 0x1fdb, 0x01df, 0x00eb, 0x01dc, 0x01e6,
    0x03d5, 0x03de, 0x07cb, 0x07dd, 0x07dc, 0x0fcd, 0x0fe2, 0x0fe7,
    0x1fe1, 0x03d0, 0x01e0, 0x01e4, 0x03d6, 0x07c5, 0x07d1, 0x07db,
    0x0fd2, 0x07e0, 0x0fd9, 0x0feb, 0x1fe3, 0x1fe9, 0x07c4, 0x01e5,
    0x03d7, 0x07c6, 0x07cf, 0x07da, 0x0fcb, 0x0fda, 0x0fe3, 0x0fe9,
    0x1fe6, 0x1ff3, 0x1ff7, 0x07d3, 0x03d8, 0x03e1, 0x07d4, 0x07d9,
    0x0fd3, 0x0fde, 0x1fdd, 0x1fd9, 0x1fe2, 0x1fea, 0x1ff1, 0x1ff6,
    0x07d2, 0x03d4, 0x03da, 0x07c7, 0x07d7, 0x07e2, 0x0fce, 0x0fdb,
    0x1fd8, 0x1fee, 0x3ff0, 0x1ff4, 0x3ff2, 0x07e1, 0x03df, 0x07c9,
    0x07d6, 0x0fca, 0x0fd0, 0x0fe5, 0x0fe6, 0x1feb, 0x1fef, 0x3ff3,
    0x3ff4, 0x3ff5, 0x0fe0, 0x07ce, 0x07d5, 0x0fc6, 0x0fd1, 0x0fe1,
    0x1fe0, 0x1fe8, 0x1ff0, 0x3ff1, 0x3ff8, 0x3ff6, 0x7ffc, 0x0fe8,
    0x07df, 0x0fc9, 0x0fd7, 0x0fdc, 0x1fdc, 0x1fdf, 0x1fed, 0x1ff5,
    0x3ff9, 0x3ffb, 0x7ffd, 0x7ffe, 0x1fe7, 0x0fcc, 0x0fd6, 0x0fdf,
    0x1fde, 0x1fda, 0x1fe5, 0x1ff2, 0x3ffa, 0x3ff7, 0x3ffc, 0x3ffd,
    0x7fff,
};

static const uint8_t bits9[169] = {
     1, 3, 6, 8, 9, 10, 10, 11, 11, 12, 12, 13, 13, 3, 4, 6,
     7, 8, 8, 9, 10, 10, 10, 11, 12, 12, 6, 6, 7, 8, 8, 9,
    10, 10, 10, 11, 12, 12, 12, 8, 7, 8, 9, 9, 10, 10, 11, 11,
    11, 12, 12, 13, 9, 8, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12,
    13, 10, 9, 9, 10, 11, 11, 11, 12, 11, 12, 12, 13, 13, 11, 9,
    10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 11, 10, 10, 11, 11,
    12, 12, 13, 13, 13, 13, 13, 13, 11, 10, 10, 11, 11, 11, 12, 12,
    13, 13, 14, 13, 14, 11, 10, 11, 11, 12, 12, 12, 12, 13, 13, 14,
    14, 14, 12, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 12,
    11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 15, 15, 13, 12, 12, 12,
    13, 13, 13, 13, 14, 14, 14, 14, 15,
};

static const uint16_t codes10[169] = {
    0x022, 0x008, 0x01d, 0x026, 0x05f, 0x0d3, 0x1cf, 0x3d0,
    0x3d7, 0x3ed, 0x7f0, 0x7f6, 0xffd, 0x007, 0x000, 0x001,
    0x009, 0x020, 0x054, 0x060, 0x0d5, 0x0dc, 0x1d4, 0x3cd,
    0x3de, 0x7e7, 0x01c, 0x002, 0x006, 0x00c, 0x01e, 0x028,
    0x05b, 0x0cd, 0x0d9, 0x1ce, 0x1dc, 0x3d9, 0x3f1, 0x025,
    0x00b, 0x00a, 0x00d, 0x024, 0x057, 0x061, 0x0cc, 0x0dd,
    0x1cc, 0x1de, 0x3d3, 0x3e7, 0x05d, 0x021, 0x01f, 0x023,
    0x027, 0x059, 0x064, 0x0d8, 0x0df, 0x1d2, 0x1e2, 0x3dd,
    0x3ee, 0x0d1, 0x055, 0x029, 0x056, 0x058, 0x062, 0x0ce,
    0x0e0, 0x0e2, 0x1da, 0x3d4, 0x3e3, 0x7eb, 0x1c9, 0x05e,
    0x05a, 0x05c, 0x063, 0x0ca, 0x0da, 0x1c7, 0x1ca, 0x1e0,
    0x3db, 0x3e8, 0x7ec, 0x1e3, 0x0d2, 0x0cb, 0x0d0, 0x0d7,
    0x0db, 0x1c6, 0x1d5, 0x1d8, 0x3ca, 0x3da, 0x7ea, 0x7f1,
    0x1e1, 0x0d4, 0x0cf, 0x0d6, 0x0de, 0x0e1, 0x1d0, 0x1d6,
    0x3d1, 0x3d5, 0x3f2, 0x7ee, 0x7fb, 0x3e9, 0x1cd, 0x1c8,
    0x1cb, 0x1d1, 0x1d7, 0x1df, 0x3cf, 0x3e0, 0x3ef, 0x7e6,
    0x7f8, 0xffa, 0x3eb, 0x1dd, 0x1d3, 0x1d9, 0x1db, 0x3d2,
    0x3cc, 0x3dc, 0x3ea, 0x7ed, 0x7f3, 0x7f9, 0xff9, 0x7f2,
    0x3ce, 0x1e4, 0x3cb, 0x3d8, 0x3d6, 0x3e2, 0x3e5, 0x7e8,
    0x7f4, 0x7f5, 0x7f7, 0xffb, 0x7fa, 0x3ec, 0x3df, 0x3e1,
    0x3e4, 0x3e6, 0x3f0, 0x7e9, 0x7ef, 0xff8, 0xffe, 0xffc,
    0xfff,
};

static const uint8_t bits10[169] = {
     6, 5, 6, 6, 7, 8, 9, 10, 10, 10, 11, 11, 12, 5, 4, 4,
     5, 6, 7, 7, 8, 8, 9, 10, 10, 11, 6, 4, 5, 5, 6, 6,
     7, 8, 8, 9, 9, 10, 10, 6, 5, 5, 5, 6, 7, 7, 8, 8,
     9, 9, 10, 10, 7, 6, 6, 6, 6, 7, 7, 8, 8, 9, 9, 10,
    10, 8, 7, 6, 7, 7, 7, 8, 8, 8, 9, 10, 10, 11, 9, 7,
     7, 7, 7, 8, 8, 9, 9, 9, 10, 10, 11, 9, 8, 8, 8, 8,
     8, 9, 9, 9, 10, 10, 11, 11, 9, 8, 8, 8, 8, 8, 9, 9,
    10, 10, 10, 11, 11, 10, 9, 9, 9, 9, 9, 9, 10, 10, 10, 11,
    11, 12, 10, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 12, 11,
    10, 9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 12, 11, 10, 10, 10,
    10, 10, 10, 11, 11, 12, 12, 12, 12,
};

static const uint16_t codes11[289] = {
    0x000, 0x006, 0x019, 0x03d, 0x09c, 0x0c6, 0x1a7, 0x390,
    0x3c2, 0x3df, 0x7e6, 0x7f3, 0xffb, 0x7ec, 0xffa, 0xffe,
    0x38e, 0x005, 0x001, 0x008, 0x014, 0x037, 0x042, 0x092,
    0x0af, 0x191, 0x1a5, 0x1b5, 0x39e, 0x3c0, 0x3a2, 0x3cd,
    0x7d6, 0x0ae, 0x017, 0x007, 0x009, 0x018, 0x039, 0x040,
    0x08e, 0x0a3, 0x0b8, 0x199, 0x1ac, 0x1c1, 0x3b1, 0x396,
    0x3be, 0x3ca, 0x09d, 0x03c, 0x015, 0x016, 0x01a, 0x03b,
    0x044, 0x091, 0x0a5, 0x0be, 0x196, 0x1ae, 0x1b9, 0x3a1,
    0x391, 0x3a5, 0x3d5, 0x094, 0x09a, 0x036, 0x038, 0x03a,
    0x041, 0x08c, 0x09b, 0x0b0, 0x0c3, 0x19e, 0x1ab, 0x1bc,
    0x39f, 0x38f, 0x3a9, 0x3cf, 0x093, 0x0bf, 0x03e, 0x03f,
    0x043, 0x045, 0x09e, 0x0a7, 0x0b9, 0x194, 0x1a2, 0x1ba,
    0x1c3, 0x3a6, 0x3a7, 0x3bb, 0x3d4, 0x09f, 0x1a0, 0x08f,
    0x08d, 0x090, 0x098, 0x0a6, 0x0b6, 0x0c4, 0x19f, 0x1af,
    0x1bf, 0x399, 0x3bf, 0x3b4, 0x3c9, 0x3e7, 0x0a8, 0x1b6,
    0x0ab, 0x0a4, 0x0aa, 0x0b2, 0x0c2, 0x0c5, 0x198, 0x1a4,
    0x1b8, 0x38c, 0x3a4, 0x3c4, 0x3c6, 0x3dd, 0x3e8, 0x0ad,
    0x3af, 0x192, 0x0bd, 0x0bc, 0x18e, 0x197, 0x19a, 0x1a3,
    0x1b1, 0x38d, 0x398, 0x3b7, 0x3d3, 0x3d1, 0x3db, 0x7dd,
    0x0b4, 0x3de, 0x1a9, 0x19b, 0x19c, 0x1a1, 0x1aa, 0x1ad,
    0x1b3, 0x38b, 0x3b2, 0x3b8, 0x3ce, 0x3e1, 0x3e0, 0x7d2,
    0x7e5, 0x0b7, 0x7e3, 0x1bb, 0x1a8, 0x1a6, 0x1b0, 0x1b2,
    0x1b7, 0x39b, 0x39a, 0x3ba, 0x3b5, 0x3d6, 0x7d7, 0x3e4,
    0x7d8, 0x7ea, 0x0ba, 0x7e8, 0x3a0, 0x1bd, 0x1b4, 0x38a,
    0x1c4, 0x392, 0x3aa, 0x3b0, 0x3bc, 0x3d7, 0x7d4, 0x7dc,
    0x7db, 0x7d5, 0x7f0, 0x0c1, 0x7fb, 0x3c8, 0x3a3, 0x395,
    0x39d, 0x3ac, 0x3ae, 0x3c5, 0x3d8, 0x3e2, 0x3e6, 0x7e4,
    0x7e7, 0x7e0, 0x7e9, 0x7f7, 0x190, 0x7f2, 0x393, 0x1be,
    0x1c0, 0x394, 0x397, 0x3ad, 0x3c3, 0x3c1, 0x3d2, 0x7da,
    0x7d9, 0x7df, 0x7eb, 0x7f4, 0x7fa, 0x195, 0x7f8, 0x3bd,
    0x39c, 0x3ab, 0x3a8, 0x3b3, 0x3b9, 0x3d0, 0x3e3, 0x3e5,
    0x7e2, 0x7de, 0x7ed, 0x7f1, 0x7f9, 0x7fc, 0x193, 0xffd,
    0x3dc, 0x3b6, 0x3c7, 0x3cc, 0x3cb, 0x3d9, 0x3da, 0x7d3,
    0x7e1, 0x7ee, 0x7ef, 0x7f5, 0x7f6, 0xffc, 0xfff, 0x19d,
    0x1c2, 0x0b5, 0x0a1, 0x096, 0x097, 0x095, 0x099, 0x0a0,
    0x0a2, 0x0ac, 0x0a9, 0x0b1, 0x0b3, 0x0bb, 0x0c0, 0x18f,
    0x004,
};

static const uint8_t bits11[289] = {
     4, 5, 6, 7, 8, 8, 9, 10, 10, 10, 11, 11, 12, 11, 12, 12,
    10, 5, 4, 5, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10, 10, 10,
    11, 8, 6, 5, 5, 6, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10,
    10, 10, 8, 7, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 10, 8, 8, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9,
    10, 10, 10, 10, 8, 8, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9,
     9, 10, 10, 10, 10, 8, 9, 8, 8, 8, 8, 8, 8, 8, 9, 9,
     9, 10, 10, 10, 10, 10, 8, 9, 8, 8, 8, 8, 8, 8, 9, 9,
     9, 10, 10, 10, 10, 10, 10, 8, 10, 9, 8, 8, 9, 9, 9, 9,
     9, 10, 10, 10, 10, 10, 10, 11, 8, 10, 9, 9, 9, 9, 9, 9,
     9, 10, 10, 10, 10, 10, 10, 11, 11, 8, 11, 9, 9, 9, 9, 9,
     9, 10, 10, 10, 10, 10, 11, 10, 11, 11, 8, 11, 10, 9, 9, 10,
     9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 8, 11, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 9, 11, 10, 9,
     9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 9, 11, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 9, 12,
    10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 9,
     9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9,
     5,
};

const uint16_t * const ff_aac_spectral_codes[11] = {
    codes1, codes2, codes3, codes4, codes5, codes6, codes7, codes8,
    codes9, codes10, codes11,
};

const uint8_t * const ff_aac_spectral_bits[11] = {
    bits1, bits2, bits3, bits4, bits5, bits6, bits7, bits8,
    bits9, bits10, bits11,
};

const uint16_t ff_aac_spectral_sizes[11] = {
    81, 81, 81, 81, 81, 81, 64, 64, 169, 169, 289,
};





static const float __attribute__ ((aligned (16))) codebook_vector0[324] = {
 -1.0000000, -1.0000000, -1.0000000, -1.0000000,
 -1.0000000, -1.0000000, -1.0000000, 0.0000000,
 -1.0000000, -1.0000000, -1.0000000, 1.0000000,
 -1.0000000, -1.0000000, 0.0000000, -1.0000000,
 -1.0000000, -1.0000000, 0.0000000, 0.0000000,
 -1.0000000, -1.0000000, 0.0000000, 1.0000000,
 -1.0000000, -1.0000000, 1.0000000, -1.0000000,
 -1.0000000, -1.0000000, 1.0000000, 0.0000000,
 -1.0000000, -1.0000000, 1.0000000, 1.0000000,
 -1.0000000, 0.0000000, -1.0000000, -1.0000000,
 -1.0000000, 0.0000000, -1.0000000, 0.0000000,
 -1.0000000, 0.0000000, -1.0000000, 1.0000000,
 -1.0000000, 0.0000000, 0.0000000, -1.0000000,
 -1.0000000, 0.0000000, 0.0000000, 0.0000000,
 -1.0000000, 0.0000000, 0.0000000, 1.0000000,
 -1.0000000, 0.0000000, 1.0000000, -1.0000000,
 -1.0000000, 0.0000000, 1.0000000, 0.0000000,
 -1.0000000, 0.0000000, 1.0000000, 1.0000000,
 -1.0000000, 1.0000000, -1.0000000, -1.0000000,
 -1.0000000, 1.0000000, -1.0000000, 0.0000000,
 -1.0000000, 1.0000000, -1.0000000, 1.0000000,
 -1.0000000, 1.0000000, 0.0000000, -1.0000000,
 -1.0000000, 1.0000000, 0.0000000, 0.0000000,
 -1.0000000, 1.0000000, 0.0000000, 1.0000000,
 -1.0000000, 1.0000000, 1.0000000, -1.0000000,
 -1.0000000, 1.0000000, 1.0000000, 0.0000000,
 -1.0000000, 1.0000000, 1.0000000, 1.0000000,
  0.0000000, -1.0000000, -1.0000000, -1.0000000,
  0.0000000, -1.0000000, -1.0000000, 0.0000000,
  0.0000000, -1.0000000, -1.0000000, 1.0000000,
  0.0000000, -1.0000000, 0.0000000, -1.0000000,
  0.0000000, -1.0000000, 0.0000000, 0.0000000,
  0.0000000, -1.0000000, 0.0000000, 1.0000000,
  0.0000000, -1.0000000, 1.0000000, -1.0000000,
  0.0000000, -1.0000000, 1.0000000, 0.0000000,
  0.0000000, -1.0000000, 1.0000000, 1.0000000,
  0.0000000, 0.0000000, -1.0000000, -1.0000000,
  0.0000000, 0.0000000, -1.0000000, 0.0000000,
  0.0000000, 0.0000000, -1.0000000, 1.0000000,
  0.0000000, 0.0000000, 0.0000000, -1.0000000,
  0.0000000, 0.0000000, 0.0000000, 0.0000000,
  0.0000000, 0.0000000, 0.0000000, 1.0000000,
  0.0000000, 0.0000000, 1.0000000, -1.0000000,
  0.0000000, 0.0000000, 1.0000000, 0.0000000,
  0.0000000, 0.0000000, 1.0000000, 1.0000000,
  0.0000000, 1.0000000, -1.0000000, -1.0000000,
  0.0000000, 1.0000000, -1.0000000, 0.0000000,
  0.0000000, 1.0000000, -1.0000000, 1.0000000,
  0.0000000, 1.0000000, 0.0000000, -1.0000000,
  0.0000000, 1.0000000, 0.0000000, 0.0000000,
  0.0000000, 1.0000000, 0.0000000, 1.0000000,
  0.0000000, 1.0000000, 1.0000000, -1.0000000,
  0.0000000, 1.0000000, 1.0000000, 0.0000000,
  0.0000000, 1.0000000, 1.0000000, 1.0000000,
  1.0000000, -1.0000000, -1.0000000, -1.0000000,
  1.0000000, -1.0000000, -1.0000000, 0.0000000,
  1.0000000, -1.0000000, -1.0000000, 1.0000000,
  1.0000000, -1.0000000, 0.0000000, -1.0000000,
  1.0000000, -1.0000000, 0.0000000, 0.0000000,
  1.0000000, -1.0000000, 0.0000000, 1.0000000,
  1.0000000, -1.0000000, 1.0000000, -1.0000000,
  1.0000000, -1.0000000, 1.0000000, 0.0000000,
  1.0000000, -1.0000000, 1.0000000, 1.0000000,
  1.0000000, 0.0000000, -1.0000000, -1.0000000,
  1.0000000, 0.0000000, -1.0000000, 0.0000000,
  1.0000000, 0.0000000, -1.0000000, 1.0000000,
  1.0000000, 0.0000000, 0.0000000, -1.0000000,
  1.0000000, 0.0000000, 0.0000000, 0.0000000,
  1.0000000, 0.0000000, 0.0000000, 1.0000000,
  1.0000000, 0.0000000, 1.0000000, -1.0000000,
  1.0000000, 0.0000000, 1.0000000, 0.0000000,
  1.0000000, 0.0000000, 1.0000000, 1.0000000,
  1.0000000, 1.0000000, -1.0000000, -1.0000000,
  1.0000000, 1.0000000, -1.0000000, 0.0000000,
  1.0000000, 1.0000000, -1.0000000, 1.0000000,
  1.0000000, 1.0000000, 0.0000000, -1.0000000,
  1.0000000, 1.0000000, 0.0000000, 0.0000000,
  1.0000000, 1.0000000, 0.0000000, 1.0000000,
  1.0000000, 1.0000000, 1.0000000, -1.0000000,
  1.0000000, 1.0000000, 1.0000000, 0.0000000,
  1.0000000, 1.0000000, 1.0000000, 1.0000000,
};

static const float __attribute__ ((aligned (16))) codebook_vector2[324] = {
  0.0000000, 0.0000000, 0.0000000, 0.0000000,
  0.0000000, 0.0000000, 0.0000000, 1.0000000,
  0.0000000, 0.0000000, 0.0000000, 2.5198421,
  0.0000000, 0.0000000, 1.0000000, 0.0000000,
  0.0000000, 0.0000000, 1.0000000, 1.0000000,
  0.0000000, 0.0000000, 1.0000000, 2.5198421,
  0.0000000, 0.0000000, 2.5198421, 0.0000000,
  0.0000000, 0.0000000, 2.5198421, 1.0000000,
  0.0000000, 0.0000000, 2.5198421, 2.5198421,
  0.0000000, 1.0000000, 0.0000000, 0.0000000,
  0.0000000, 1.0000000, 0.0000000, 1.0000000,
  0.0000000, 1.0000000, 0.0000000, 2.5198421,
  0.0000000, 1.0000000, 1.0000000, 0.0000000,
  0.0000000, 1.0000000, 1.0000000, 1.0000000,
  0.0000000, 1.0000000, 1.0000000, 2.5198421,
  0.0000000, 1.0000000, 2.5198421, 0.0000000,
  0.0000000, 1.0000000, 2.5198421, 1.0000000,
  0.0000000, 1.0000000, 2.5198421, 2.5198421,
  0.0000000, 2.5198421, 0.0000000, 0.0000000,
  0.0000000, 2.5198421, 0.0000000, 1.0000000,
  0.0000000, 2.5198421, 0.0000000, 2.5198421,
  0.0000000, 2.5198421, 1.0000000, 0.0000000,
  0.0000000, 2.5198421, 1.0000000, 1.0000000,
  0.0000000, 2.5198421, 1.0000000, 2.5198421,
  0.0000000, 2.5198421, 2.5198421, 0.0000000,
  0.0000000, 2.5198421, 2.5198421, 1.0000000,
  0.0000000, 2.5198421, 2.5198421, 2.5198421,
  1.0000000, 0.0000000, 0.0000000, 0.0000000,
  1.0000000, 0.0000000, 0.0000000, 1.0000000,
  1.0000000, 0.0000000, 0.0000000, 2.5198421,
  1.0000000, 0.0000000, 1.0000000, 0.0000000,
  1.0000000, 0.0000000, 1.0000000, 1.0000000,
  1.0000000, 0.0000000, 1.0000000, 2.5198421,
  1.0000000, 0.0000000, 2.5198421, 0.0000000,
  1.0000000, 0.0000000, 2.5198421, 1.0000000,
  1.0000000, 0.0000000, 2.5198421, 2.5198421,
  1.0000000, 1.0000000, 0.0000000, 0.0000000,
  1.0000000, 1.0000000, 0.0000000, 1.0000000,
  1.0000000, 1.0000000, 0.0000000, 2.5198421,
  1.0000000, 1.0000000, 1.0000000, 0.0000000,
  1.0000000, 1.0000000, 1.0000000, 1.0000000,
  1.0000000, 1.0000000, 1.0000000, 2.5198421,
  1.0000000, 1.0000000, 2.5198421, 0.0000000,
  1.0000000, 1.0000000, 2.5198421, 1.0000000,
  1.0000000, 1.0000000, 2.5198421, 2.5198421,
  1.0000000, 2.5198421, 0.0000000, 0.0000000,
  1.0000000, 2.5198421, 0.0000000, 1.0000000,
  1.0000000, 2.5198421, 0.0000000, 2.5198421,
  1.0000000, 2.5198421, 1.0000000, 0.0000000,
  1.0000000, 2.5198421, 1.0000000, 1.0000000,
  1.0000000, 2.5198421, 1.0000000, 2.5198421,
  1.0000000, 2.5198421, 2.5198421, 0.0000000,
  1.0000000, 2.5198421, 2.5198421, 1.0000000,
  1.0000000, 2.5198421, 2.5198421, 2.5198421,
  2.5198421, 0.0000000, 0.0000000, 0.0000000,
  2.5198421, 0.0000000, 0.0000000, 1.0000000,
  2.5198421, 0.0000000, 0.0000000, 2.5198421,
  2.5198421, 0.0000000, 1.0000000, 0.0000000,
  2.5198421, 0.0000000, 1.0000000, 1.0000000,
  2.5198421, 0.0000000, 1.0000000, 2.5198421,
  2.5198421, 0.0000000, 2.5198421, 0.0000000,
  2.5198421, 0.0000000, 2.5198421, 1.0000000,
  2.5198421, 0.0000000, 2.5198421, 2.5198421,
  2.5198421, 1.0000000, 0.0000000, 0.0000000,
  2.5198421, 1.0000000, 0.0000000, 1.0000000,
  2.5198421, 1.0000000, 0.0000000, 2.5198421,
  2.5198421, 1.0000000, 1.0000000, 0.0000000,
  2.5198421, 1.0000000, 1.0000000, 1.0000000,
  2.5198421, 1.0000000, 1.0000000, 2.5198421,
  2.5198421, 1.0000000, 2.5198421, 0.0000000,
  2.5198421, 1.0000000, 2.5198421, 1.0000000,
  2.5198421, 1.0000000, 2.5198421, 2.5198421,
  2.5198421, 2.5198421, 0.0000000, 0.0000000,
  2.5198421, 2.5198421, 0.0000000, 1.0000000,
  2.5198421, 2.5198421, 0.0000000, 2.5198421,
  2.5198421, 2.5198421, 1.0000000, 0.0000000,
  2.5198421, 2.5198421, 1.0000000, 1.0000000,
  2.5198421, 2.5198421, 1.0000000, 2.5198421,
  2.5198421, 2.5198421, 2.5198421, 0.0000000,
  2.5198421, 2.5198421, 2.5198421, 1.0000000,
  2.5198421, 2.5198421, 2.5198421, 2.5198421,
};

static const float __attribute__ ((aligned (16))) codebook_vector4[162] = {
 -6.3496042, -6.3496042, -6.3496042, -4.3267487,
 -6.3496042, -2.5198421, -6.3496042, -1.0000000,
 -6.3496042, 0.0000000, -6.3496042, 1.0000000,
 -6.3496042, 2.5198421, -6.3496042, 4.3267487,
 -6.3496042, 6.3496042, -4.3267487, -6.3496042,
 -4.3267487, -4.3267487, -4.3267487, -2.5198421,
 -4.3267487, -1.0000000, -4.3267487, 0.0000000,
 -4.3267487, 1.0000000, -4.3267487, 2.5198421,
 -4.3267487, 4.3267487, -4.3267487, 6.3496042,
 -2.5198421, -6.3496042, -2.5198421, -4.3267487,
 -2.5198421, -2.5198421, -2.5198421, -1.0000000,
 -2.5198421, 0.0000000, -2.5198421, 1.0000000,
 -2.5198421, 2.5198421, -2.5198421, 4.3267487,
 -2.5198421, 6.3496042, -1.0000000, -6.3496042,
 -1.0000000, -4.3267487, -1.0000000, -2.5198421,
 -1.0000000, -1.0000000, -1.0000000, 0.0000000,
 -1.0000000, 1.0000000, -1.0000000, 2.5198421,
 -1.0000000, 4.3267487, -1.0000000, 6.3496042,
  0.0000000, -6.3496042, 0.0000000, -4.3267487,
  0.0000000, -2.5198421, 0.0000000, -1.0000000,
  0.0000000, 0.0000000, 0.0000000, 1.0000000,
  0.0000000, 2.5198421, 0.0000000, 4.3267487,
  0.0000000, 6.3496042, 1.0000000, -6.3496042,
  1.0000000, -4.3267487, 1.0000000, -2.5198421,
  1.0000000, -1.0000000, 1.0000000, 0.0000000,
  1.0000000, 1.0000000, 1.0000000, 2.5198421,
  1.0000000, 4.3267487, 1.0000000, 6.3496042,
  2.5198421, -6.3496042, 2.5198421, -4.3267487,
  2.5198421, -2.5198421, 2.5198421, -1.0000000,
  2.5198421, 0.0000000, 2.5198421, 1.0000000,
  2.5198421, 2.5198421, 2.5198421, 4.3267487,
  2.5198421, 6.3496042, 4.3267487, -6.3496042,
  4.3267487, -4.3267487, 4.3267487, -2.5198421,
  4.3267487, -1.0000000, 4.3267487, 0.0000000,
  4.3267487, 1.0000000, 4.3267487, 2.5198421,
  4.3267487, 4.3267487, 4.3267487, 6.3496042,
  6.3496042, -6.3496042, 6.3496042, -4.3267487,
  6.3496042, -2.5198421, 6.3496042, -1.0000000,
  6.3496042, 0.0000000, 6.3496042, 1.0000000,
  6.3496042, 2.5198421, 6.3496042, 4.3267487,
  6.3496042, 6.3496042,
};

static const float __attribute__ ((aligned (16))) codebook_vector6[128] = {
  0.0000000, 0.0000000, 0.0000000, 1.0000000,
  0.0000000, 2.5198421, 0.0000000, 4.3267487,
  0.0000000, 6.3496042, 0.0000000, 8.5498797,
  0.0000000, 10.9027236, 0.0000000, 13.3905183,
  1.0000000, 0.0000000, 1.0000000, 1.0000000,
  1.0000000, 2.5198421, 1.0000000, 4.3267487,
  1.0000000, 6.3496042, 1.0000000, 8.5498797,
  1.0000000, 10.9027236, 1.0000000, 13.3905183,
  2.5198421, 0.0000000, 2.5198421, 1.0000000,
  2.5198421, 2.5198421, 2.5198421, 4.3267487,
  2.5198421, 6.3496042, 2.5198421, 8.5498797,
  2.5198421, 10.9027236, 2.5198421, 13.3905183,
  4.3267487, 0.0000000, 4.3267487, 1.0000000,
  4.3267487, 2.5198421, 4.3267487, 4.3267487,
  4.3267487, 6.3496042, 4.3267487, 8.5498797,
  4.3267487, 10.9027236, 4.3267487, 13.3905183,
  6.3496042, 0.0000000, 6.3496042, 1.0000000,
  6.3496042, 2.5198421, 6.3496042, 4.3267487,
  6.3496042, 6.3496042, 6.3496042, 8.5498797,
  6.3496042, 10.9027236, 6.3496042, 13.3905183,
  8.5498797, 0.0000000, 8.5498797, 1.0000000,
  8.5498797, 2.5198421, 8.5498797, 4.3267487,
  8.5498797, 6.3496042, 8.5498797, 8.5498797,
  8.5498797, 10.9027236, 8.5498797, 13.3905183,
 10.9027236, 0.0000000, 10.9027236, 1.0000000,
 10.9027236, 2.5198421, 10.9027236, 4.3267487,
 10.9027236, 6.3496042, 10.9027236, 8.5498797,
 10.9027236, 10.9027236, 10.9027236, 13.3905183,
 13.3905183, 0.0000000, 13.3905183, 1.0000000,
 13.3905183, 2.5198421, 13.3905183, 4.3267487,
 13.3905183, 6.3496042, 13.3905183, 8.5498797,
 13.3905183, 10.9027236, 13.3905183, 13.3905183,
};

static const float __attribute__ ((aligned (16))) codebook_vector8[338] = {
  0.0000000, 0.0000000, 0.0000000, 1.0000000,
  0.0000000, 2.5198421, 0.0000000, 4.3267487,
  0.0000000, 6.3496042, 0.0000000, 8.5498797,
  0.0000000, 10.9027236, 0.0000000, 13.3905183,
  0.0000000, 16.0000000, 0.0000000, 18.7207544,
  0.0000000, 21.5443469, 0.0000000, 24.4637810,
  0.0000000, 27.4731418, 1.0000000, 0.0000000,
  1.0000000, 1.0000000, 1.0000000, 2.5198421,
  1.0000000, 4.3267487, 1.0000000, 6.3496042,
  1.0000000, 8.5498797, 1.0000000, 10.9027236,
  1.0000000, 13.3905183, 1.0000000, 16.0000000,
  1.0000000, 18.7207544, 1.0000000, 21.5443469,
  1.0000000, 24.4637810, 1.0000000, 27.4731418,
  2.5198421, 0.0000000, 2.5198421, 1.0000000,
  2.5198421, 2.5198421, 2.5198421, 4.3267487,
  2.5198421, 6.3496042, 2.5198421, 8.5498797,
  2.5198421, 10.9027236, 2.5198421, 13.3905183,
  2.5198421, 16.0000000, 2.5198421, 18.7207544,
  2.5198421, 21.5443469, 2.5198421, 24.4637810,
  2.5198421, 27.4731418, 4.3267487, 0.0000000,
  4.3267487, 1.0000000, 4.3267487, 2.5198421,
  4.3267487, 4.3267487, 4.3267487, 6.3496042,
  4.3267487, 8.5498797, 4.3267487, 10.9027236,
  4.3267487, 13.3905183, 4.3267487, 16.0000000,
  4.3267487, 18.7207544, 4.3267487, 21.5443469,
  4.3267487, 24.4637810, 4.3267487, 27.4731418,
  6.3496042, 0.0000000, 6.3496042, 1.0000000,
  6.3496042, 2.5198421, 6.3496042, 4.3267487,
  6.3496042, 6.3496042, 6.3496042, 8.5498797,
  6.3496042, 10.9027236, 6.3496042, 13.3905183,
  6.3496042, 16.0000000, 6.3496042, 18.7207544,
  6.3496042, 21.5443469, 6.3496042, 24.4637810,
  6.3496042, 27.4731418, 8.5498797, 0.0000000,
  8.5498797, 1.0000000, 8.5498797, 2.5198421,
  8.5498797, 4.3267487, 8.5498797, 6.3496042,
  8.5498797, 8.5498797, 8.5498797, 10.9027236,
  8.5498797, 13.3905183, 8.5498797, 16.0000000,
  8.5498797, 18.7207544, 8.5498797, 21.5443469,
  8.5498797, 24.4637810, 8.5498797, 27.4731418,
 10.9027236, 0.0000000, 10.9027236, 1.0000000,
 10.9027236, 2.5198421, 10.9027236, 4.3267487,
 10.9027236, 6.3496042, 10.9027236, 8.5498797,
 10.9027236, 10.9027236, 10.9027236, 13.3905183,
 10.9027236, 16.0000000, 10.9027236, 18.7207544,
 10.9027236, 21.5443469, 10.9027236, 24.4637810,
 10.9027236, 27.4731418, 13.3905183, 0.0000000,
 13.3905183, 1.0000000, 13.3905183, 2.5198421,
 13.3905183, 4.3267487, 13.3905183, 6.3496042,
 13.3905183, 8.5498797, 13.3905183, 10.9027236,
 13.3905183, 13.3905183, 13.3905183, 16.0000000,
 13.3905183, 18.7207544, 13.3905183, 21.5443469,
 13.3905183, 24.4637810, 13.3905183, 27.4731418,
 16.0000000, 0.0000000, 16.0000000, 1.0000000,
 16.0000000, 2.5198421, 16.0000000, 4.3267487,
 16.0000000, 6.3496042, 16.0000000, 8.5498797,
 16.0000000, 10.9027236, 16.0000000, 13.3905183,
 16.0000000, 16.0000000, 16.0000000, 18.7207544,
 16.0000000, 21.5443469, 16.0000000, 24.4637810,
 16.0000000, 27.4731418, 18.7207544, 0.0000000,
 18.7207544, 1.0000000, 18.7207544, 2.5198421,
 18.7207544, 4.3267487, 18.7207544, 6.3496042,
 18.7207544, 8.5498797, 18.7207544, 10.9027236,
 18.7207544, 13.3905183, 18.7207544, 16.0000000,
 18.7207544, 18.7207544, 18.7207544, 21.5443469,
 18.7207544, 24.4637810, 18.7207544, 27.4731418,
 21.5443469, 0.0000000, 21.5443469, 1.0000000,
 21.5443469, 2.5198421, 21.5443469, 4.3267487,
 21.5443469, 6.3496042, 21.5443469, 8.5498797,
 21.5443469, 10.9027236, 21.5443469, 13.3905183,
 21.5443469, 16.0000000, 21.5443469, 18.7207544,
 21.5443469, 21.5443469, 21.5443469, 24.4637810,
 21.5443469, 27.4731418, 24.4637810, 0.0000000,
 24.4637810, 1.0000000, 24.4637810, 2.5198421,
 24.4637810, 4.3267487, 24.4637810, 6.3496042,
 24.4637810, 8.5498797, 24.4637810, 10.9027236,
 24.4637810, 13.3905183, 24.4637810, 16.0000000,
 24.4637810, 18.7207544, 24.4637810, 21.5443469,
 24.4637810, 24.4637810, 24.4637810, 27.4731418,
 27.4731418, 0.0000000, 27.4731418, 1.0000000,
 27.4731418, 2.5198421, 27.4731418, 4.3267487,
 27.4731418, 6.3496042, 27.4731418, 8.5498797,
 27.4731418, 10.9027236, 27.4731418, 13.3905183,
 27.4731418, 16.0000000, 27.4731418, 18.7207544,
 27.4731418, 21.5443469, 27.4731418, 24.4637810,
 27.4731418, 27.4731418,
};

static const float __attribute__ ((aligned (16))) codebook_vector10[578] = {
  0.0000000, 0.0000000, 0.0000000, 1.0000000,
  0.0000000, 2.5198421, 0.0000000, 4.3267487,
  0.0000000, 6.3496042, 0.0000000, 8.5498797,
  0.0000000, 10.9027236, 0.0000000, 13.3905183,
  0.0000000, 16.0000000, 0.0000000, 18.7207544,
  0.0000000, 21.5443469, 0.0000000, 24.4637810,
  0.0000000, 27.4731418, 0.0000000, 30.5673509,
  0.0000000, 33.7419917, 0.0000000, 36.9931811,
  0.0000000, 64.0f, 1.0000000, 0.0000000,
  1.0000000, 1.0000000, 1.0000000, 2.5198421,
  1.0000000, 4.3267487, 1.0000000, 6.3496042,
  1.0000000, 8.5498797, 1.0000000, 10.9027236,
  1.0000000, 13.3905183, 1.0000000, 16.0000000,
  1.0000000, 18.7207544, 1.0000000, 21.5443469,
  1.0000000, 24.4637810, 1.0000000, 27.4731418,
  1.0000000, 30.5673509, 1.0000000, 33.7419917,
  1.0000000, 36.9931811, 1.0000000, 64.0f,
  2.5198421, 0.0000000, 2.5198421, 1.0000000,
  2.5198421, 2.5198421, 2.5198421, 4.3267487,
  2.5198421, 6.3496042, 2.5198421, 8.5498797,
  2.5198421, 10.9027236, 2.5198421, 13.3905183,
  2.5198421, 16.0000000, 2.5198421, 18.7207544,
  2.5198421, 21.5443469, 2.5198421, 24.4637810,
  2.5198421, 27.4731418, 2.5198421, 30.5673509,
  2.5198421, 33.7419917, 2.5198421, 36.9931811,
  2.5198421, 64.0f, 4.3267487, 0.0000000,
  4.3267487, 1.0000000, 4.3267487, 2.5198421,
  4.3267487, 4.3267487, 4.3267487, 6.3496042,
  4.3267487, 8.5498797, 4.3267487, 10.9027236,
  4.3267487, 13.3905183, 4.3267487, 16.0000000,
  4.3267487, 18.7207544, 4.3267487, 21.5443469,
  4.3267487, 24.4637810, 4.3267487, 27.4731418,
  4.3267487, 30.5673509, 4.3267487, 33.7419917,
  4.3267487, 36.9931811, 4.3267487, 64.0f,
  6.3496042, 0.0000000, 6.3496042, 1.0000000,
  6.3496042, 2.5198421, 6.3496042, 4.3267487,
  6.3496042, 6.3496042, 6.3496042, 8.5498797,
  6.3496042, 10.9027236, 6.3496042, 13.3905183,
  6.3496042, 16.0000000, 6.3496042, 18.7207544,
  6.3496042, 21.5443469, 6.3496042, 24.4637810,
  6.3496042, 27.4731418, 6.3496042, 30.5673509,
  6.3496042, 33.7419917, 6.3496042, 36.9931811,
  6.3496042, 64.0f, 8.5498797, 0.0000000,
  8.5498797, 1.0000000, 8.5498797, 2.5198421,
  8.5498797, 4.3267487, 8.5498797, 6.3496042,
  8.5498797, 8.5498797, 8.5498797, 10.9027236,
  8.5498797, 13.3905183, 8.5498797, 16.0000000,
  8.5498797, 18.7207544, 8.5498797, 21.5443469,
  8.5498797, 24.4637810, 8.5498797, 27.4731418,
  8.5498797, 30.5673509, 8.5498797, 33.7419917,
  8.5498797, 36.9931811, 8.5498797, 64.0f,
 10.9027236, 0.0000000, 10.9027236, 1.0000000,
 10.9027236, 2.5198421, 10.9027236, 4.3267487,
 10.9027236, 6.3496042, 10.9027236, 8.5498797,
 10.9027236, 10.9027236, 10.9027236, 13.3905183,
 10.9027236, 16.0000000, 10.9027236, 18.7207544,
 10.9027236, 21.5443469, 10.9027236, 24.4637810,
 10.9027236, 27.4731418, 10.9027236, 30.5673509,
 10.9027236, 33.7419917, 10.9027236, 36.9931811,
 10.9027236, 64.0f, 13.3905183, 0.0000000,
 13.3905183, 1.0000000, 13.3905183, 2.5198421,
 13.3905183, 4.3267487, 13.3905183, 6.3496042,
 13.3905183, 8.5498797, 13.3905183, 10.9027236,
 13.3905183, 13.3905183, 13.3905183, 16.0000000,
 13.3905183, 18.7207544, 13.3905183, 21.5443469,
 13.3905183, 24.4637810, 13.3905183, 27.4731418,
 13.3905183, 30.5673509, 13.3905183, 33.7419917,
 13.3905183, 36.9931811, 13.3905183, 64.0f,
 16.0000000, 0.0000000, 16.0000000, 1.0000000,
 16.0000000, 2.5198421, 16.0000000, 4.3267487,
 16.0000000, 6.3496042, 16.0000000, 8.5498797,
 16.0000000, 10.9027236, 16.0000000, 13.3905183,
 16.0000000, 16.0000000, 16.0000000, 18.7207544,
 16.0000000, 21.5443469, 16.0000000, 24.4637810,
 16.0000000, 27.4731418, 16.0000000, 30.5673509,
 16.0000000, 33.7419917, 16.0000000, 36.9931811,
 16.0000000, 64.0f, 18.7207544, 0.0000000,
 18.7207544, 1.0000000, 18.7207544, 2.5198421,
 18.7207544, 4.3267487, 18.7207544, 6.3496042,
 18.7207544, 8.5498797, 18.7207544, 10.9027236,
 18.7207544, 13.3905183, 18.7207544, 16.0000000,
 18.7207544, 18.7207544, 18.7207544, 21.5443469,
 18.7207544, 24.4637810, 18.7207544, 27.4731418,
 18.7207544, 30.5673509, 18.7207544, 33.7419917,
 18.7207544, 36.9931811, 18.7207544, 64.0f,
 21.5443469, 0.0000000, 21.5443469, 1.0000000,
 21.5443469, 2.5198421, 21.5443469, 4.3267487,
 21.5443469, 6.3496042, 21.5443469, 8.5498797,
 21.5443469, 10.9027236, 21.5443469, 13.3905183,
 21.5443469, 16.0000000, 21.5443469, 18.7207544,
 21.5443469, 21.5443469, 21.5443469, 24.4637810,
 21.5443469, 27.4731418, 21.5443469, 30.5673509,
 21.5443469, 33.7419917, 21.5443469, 36.9931811,
 21.5443469, 64.0f, 24.4637810, 0.0000000,
 24.4637810, 1.0000000, 24.4637810, 2.5198421,
 24.4637810, 4.3267487, 24.4637810, 6.3496042,
 24.4637810, 8.5498797, 24.4637810, 10.9027236,
 24.4637810, 13.3905183, 24.4637810, 16.0000000,
 24.4637810, 18.7207544, 24.4637810, 21.5443469,
 24.4637810, 24.4637810, 24.4637810, 27.4731418,
 24.4637810, 30.5673509, 24.4637810, 33.7419917,
 24.4637810, 36.9931811, 24.4637810, 64.0f,
 27.4731418, 0.0000000, 27.4731418, 1.0000000,
 27.4731418, 2.5198421, 27.4731418, 4.3267487,
 27.4731418, 6.3496042, 27.4731418, 8.5498797,
 27.4731418, 10.9027236, 27.4731418, 13.3905183,
 27.4731418, 16.0000000, 27.4731418, 18.7207544,
 27.4731418, 21.5443469, 27.4731418, 24.4637810,
 27.4731418, 27.4731418, 27.4731418, 30.5673509,
 27.4731418, 33.7419917, 27.4731418, 36.9931811,
 27.4731418, 64.0f, 30.5673509, 0.0000000,
 30.5673509, 1.0000000, 30.5673509, 2.5198421,
 30.5673509, 4.3267487, 30.5673509, 6.3496042,
 30.5673509, 8.5498797, 30.5673509, 10.9027236,
 30.5673509, 13.3905183, 30.5673509, 16.0000000,
 30.5673509, 18.7207544, 30.5673509, 21.5443469,
 30.5673509, 24.4637810, 30.5673509, 27.4731418,
 30.5673509, 30.5673509, 30.5673509, 33.7419917,
 30.5673509, 36.9931811, 30.5673509, 64.0f,
 33.7419917, 0.0000000, 33.7419917, 1.0000000,
 33.7419917, 2.5198421, 33.7419917, 4.3267487,
 33.7419917, 6.3496042, 33.7419917, 8.5498797,
 33.7419917, 10.9027236, 33.7419917, 13.3905183,
 33.7419917, 16.0000000, 33.7419917, 18.7207544,
 33.7419917, 21.5443469, 33.7419917, 24.4637810,
 33.7419917, 27.4731418, 33.7419917, 30.5673509,
 33.7419917, 33.7419917, 33.7419917, 36.9931811,
 33.7419917, 64.0f, 36.9931811, 0.0000000,
 36.9931811, 1.0000000, 36.9931811, 2.5198421,
 36.9931811, 4.3267487, 36.9931811, 6.3496042,
 36.9931811, 8.5498797, 36.9931811, 10.9027236,
 36.9931811, 13.3905183, 36.9931811, 16.0000000,
 36.9931811, 18.7207544, 36.9931811, 21.5443469,
 36.9931811, 24.4637810, 36.9931811, 27.4731418,
 36.9931811, 30.5673509, 36.9931811, 33.7419917,
 36.9931811, 36.9931811, 36.9931811, 64.0f,
      64.0f, 0.0000000, 64.0f, 1.0000000,
      64.0f, 2.5198421, 64.0f, 4.3267487,
      64.0f, 6.3496042, 64.0f, 8.5498797,
      64.0f, 10.9027236, 64.0f, 13.3905183,
      64.0f, 16.0000000, 64.0f, 18.7207544,
      64.0f, 21.5443469, 64.0f, 24.4637810,
      64.0f, 27.4731418, 64.0f, 30.5673509,
      64.0f, 33.7419917, 64.0f, 36.9931811,
      64.0f, 64.0f,
};

const float * const ff_aac_codebook_vectors[] = {
    codebook_vector0, codebook_vector0, codebook_vector2,
    codebook_vector2, codebook_vector4, codebook_vector4,
    codebook_vector6, codebook_vector6, codebook_vector8,
    codebook_vector8, codebook_vector10,
};

static const float codebook_vector0_vals[] = {
   -1.0000000, 0.0000000, 1.0000000
};






static const uint16_t codebook_vector02_idx[] = {
    0x0000, 0x8140, 0x8180, 0x4110, 0xc250, 0xc290, 0x4120, 0xc260, 0xc2a0,
    0x2104, 0xa244, 0xa284, 0x6214, 0xe354, 0xe394, 0x6224, 0xe364, 0xe3a4,
    0x2108, 0xa248, 0xa288, 0x6218, 0xe358, 0xe398, 0x6228, 0xe368, 0xe3a8,
    0x1101, 0x9241, 0x9281, 0x5211, 0xd351, 0xd391, 0x5221, 0xd361, 0xd3a1,
    0x3205, 0xb345, 0xb385, 0x7315, 0xf455, 0xf495, 0x7325, 0xf465, 0xf4a5,
    0x3209, 0xb349, 0xb389, 0x7319, 0xf459, 0xf499, 0x7329, 0xf469, 0xf4a9,
    0x1102, 0x9242, 0x9282, 0x5212, 0xd352, 0xd392, 0x5222, 0xd362, 0xd3a2,
    0x3206, 0xb346, 0xb386, 0x7316, 0xf456, 0xf496, 0x7326, 0xf466, 0xf4a6,
    0x320a, 0xb34a, 0xb38a, 0x731a, 0xf45a, 0xf49a, 0x732a, 0xf46a, 0xf4aa,
};

static const float codebook_vector4_vals[] = {
   -6.3496042, -4.3267487,
   -2.5198421, -1.0000000,
    0.0000000, 1.0000000,
    2.5198421, 4.3267487,
    6.3496042,
};




static const uint16_t codebook_vector4_idx[] = {
    0x0000, 0x0010, 0x0020, 0x0030, 0x0040, 0x0050, 0x0060, 0x0070, 0x0080,
    0x0001, 0x0011, 0x0021, 0x0031, 0x0041, 0x0051, 0x0061, 0x0071, 0x0081,
    0x0002, 0x0012, 0x0022, 0x0032, 0x0042, 0x0052, 0x0062, 0x0072, 0x0082,
    0x0003, 0x0013, 0x0023, 0x0033, 0x0043, 0x0053, 0x0063, 0x0073, 0x0083,
    0x0004, 0x0014, 0x0024, 0x0034, 0x0044, 0x0054, 0x0064, 0x0074, 0x0084,
    0x0005, 0x0015, 0x0025, 0x0035, 0x0045, 0x0055, 0x0065, 0x0075, 0x0085,
    0x0006, 0x0016, 0x0026, 0x0036, 0x0046, 0x0056, 0x0066, 0x0076, 0x0086,
    0x0007, 0x0017, 0x0027, 0x0037, 0x0047, 0x0057, 0x0067, 0x0077, 0x0087,
    0x0008, 0x0018, 0x0028, 0x0038, 0x0048, 0x0058, 0x0068, 0x0078, 0x0088,
};







static const uint16_t codebook_vector6_idx[] = {
    0x0000, 0x0110, 0x0120, 0x0130, 0x0140, 0x0150, 0x0160, 0x0170,
    0x1101, 0x0211, 0x0221, 0x0231, 0x0241, 0x0251, 0x0261, 0x0271,
    0x1102, 0x0212, 0x0222, 0x0232, 0x0242, 0x0252, 0x0262, 0x0272,
    0x1103, 0x0213, 0x0223, 0x0233, 0x0243, 0x0253, 0x0263, 0x0273,
    0x1104, 0x0214, 0x0224, 0x0234, 0x0244, 0x0254, 0x0264, 0x0274,
    0x1105, 0x0215, 0x0225, 0x0235, 0x0245, 0x0255, 0x0265, 0x0275,
    0x1106, 0x0216, 0x0226, 0x0236, 0x0246, 0x0256, 0x0266, 0x0276,
    0x1107, 0x0217, 0x0227, 0x0237, 0x0247, 0x0257, 0x0267, 0x0277,
};







static const uint16_t codebook_vector8_idx[] = {
  0x0000, 0x0110, 0x0120, 0x0130, 0x0140, 0x0150, 0x0160,
  0x0170, 0x0180, 0x0190, 0x01a0, 0x01b0, 0x01c0,
  0x1101, 0x0211, 0x0221, 0x0231, 0x0241, 0x0251, 0x0261,
  0x0271, 0x0281, 0x0291, 0x02a1, 0x02b1, 0x02c1,
  0x1102, 0x0212, 0x0222, 0x0232, 0x0242, 0x0252, 0x0262,
  0x0272, 0x0282, 0x0292, 0x02a2, 0x02b2, 0x02c2,
  0x1103, 0x0213, 0x0223, 0x0233, 0x0243, 0x0253, 0x0263,
  0x0273, 0x0283, 0x0293, 0x02a3, 0x02b3, 0x02c3,
  0x1104, 0x0214, 0x0224, 0x0234, 0x0244, 0x0254, 0x0264,
  0x0274, 0x0284, 0x0294, 0x02a4, 0x02b4, 0x02c4,
  0x1105, 0x0215, 0x0225, 0x0235, 0x0245, 0x0255, 0x0265,
  0x0275, 0x0285, 0x0295, 0x02a5, 0x02b5, 0x02c5,
  0x1106, 0x0216, 0x0226, 0x0236, 0x0246, 0x0256, 0x0266,
  0x0276, 0x0286, 0x0296, 0x02a6, 0x02b6, 0x02c6,
  0x1107, 0x0217, 0x0227, 0x0237, 0x0247, 0x0257, 0x0267,
  0x0277, 0x0287, 0x0297, 0x02a7, 0x02b7, 0x02c7,
  0x1108, 0x0218, 0x0228, 0x0238, 0x0248, 0x0258, 0x0268,
  0x0278, 0x0288, 0x0298, 0x02a8, 0x02b8, 0x02c8,
  0x1109, 0x0219, 0x0229, 0x0239, 0x0249, 0x0259, 0x0269,
  0x0279, 0x0289, 0x0299, 0x02a9, 0x02b9, 0x02c9,
  0x110a, 0x021a, 0x022a, 0x023a, 0x024a, 0x025a, 0x026a,
  0x027a, 0x028a, 0x029a, 0x02aa, 0x02ba, 0x02ca,
  0x110b, 0x021b, 0x022b, 0x023b, 0x024b, 0x025b, 0x026b,
  0x027b, 0x028b, 0x029b, 0x02ab, 0x02bb, 0x02cb,
  0x110c, 0x021c, 0x022c, 0x023c, 0x024c, 0x025c, 0x026c,
  0x027c, 0x028c, 0x029c, 0x02ac, 0x02bc, 0x02cc,
};

static const float codebook_vector10_vals[] = {
     0.0000000, 1.0000000,
     2.5198421, 4.3267487,
     6.3496042, 8.5498797,
    10.9027236, 13.3905183,
    16.0000000, 18.7207544,
    21.5443469, 24.4637810,
    27.4731418, 30.5673509,
    33.7419917, 36.9931811,
};






static const uint16_t codebook_vector10_idx[] = {
    0x0000, 0x1010, 0x1020, 0x1030, 0x1040, 0x1050, 0x1060, 0x1070,
    0x1080, 0x1090, 0x10a0, 0x10b0, 0x10c0, 0x10d0, 0x10e0, 0x10f0, 0x1200,
    0x1001, 0x2011, 0x2021, 0x2031, 0x2041, 0x2051, 0x2061, 0x2071,
    0x2081, 0x2091, 0x20a1, 0x20b1, 0x20c1, 0x20d1, 0x20e1, 0x20f1, 0x2201,
    0x1002, 0x2012, 0x2022, 0x2032, 0x2042, 0x2052, 0x2062, 0x2072,
    0x2082, 0x2092, 0x20a2, 0x20b2, 0x20c2, 0x20d2, 0x20e2, 0x20f2, 0x2202,
    0x1003, 0x2013, 0x2023, 0x2033, 0x2043, 0x2053, 0x2063, 0x2073,
    0x2083, 0x2093, 0x20a3, 0x20b3, 0x20c3, 0x20d3, 0x20e3, 0x20f3, 0x2203,
    0x1004, 0x2014, 0x2024, 0x2034, 0x2044, 0x2054, 0x2064, 0x2074,
    0x2084, 0x2094, 0x20a4, 0x20b4, 0x20c4, 0x20d4, 0x20e4, 0x20f4, 0x2204,
    0x1005, 0x2015, 0x2025, 0x2035, 0x2045, 0x2055, 0x2065, 0x2075,
    0x2085, 0x2095, 0x20a5, 0x20b5, 0x20c5, 0x20d5, 0x20e5, 0x20f5, 0x2205,
    0x1006, 0x2016, 0x2026, 0x2036, 0x2046, 0x2056, 0x2066, 0x2076,
    0x2086, 0x2096, 0x20a6, 0x20b6, 0x20c6, 0x20d6, 0x20e6, 0x20f6, 0x2206,
    0x1007, 0x2017, 0x2027, 0x2037, 0x2047, 0x2057, 0x2067, 0x2077,
    0x2087, 0x2097, 0x20a7, 0x20b7, 0x20c7, 0x20d7, 0x20e7, 0x20f7, 0x2207,
    0x1008, 0x2018, 0x2028, 0x2038, 0x2048, 0x2058, 0x2068, 0x2078,
    0x2088, 0x2098, 0x20a8, 0x20b8, 0x20c8, 0x20d8, 0x20e8, 0x20f8, 0x2208,
    0x1009, 0x2019, 0x2029, 0x2039, 0x2049, 0x2059, 0x2069, 0x2079,
    0x2089, 0x2099, 0x20a9, 0x20b9, 0x20c9, 0x20d9, 0x20e9, 0x20f9, 0x2209,
    0x100a, 0x201a, 0x202a, 0x203a, 0x204a, 0x205a, 0x206a, 0x207a,
    0x208a, 0x209a, 0x20aa, 0x20ba, 0x20ca, 0x20da, 0x20ea, 0x20fa, 0x220a,
    0x100b, 0x201b, 0x202b, 0x203b, 0x204b, 0x205b, 0x206b, 0x207b,
    0x208b, 0x209b, 0x20ab, 0x20bb, 0x20cb, 0x20db, 0x20eb, 0x20fb, 0x220b,
    0x100c, 0x201c, 0x202c, 0x203c, 0x204c, 0x205c, 0x206c, 0x207c,
    0x208c, 0x209c, 0x20ac, 0x20bc, 0x20cc, 0x20dc, 0x20ec, 0x20fc, 0x220c,
    0x100d, 0x201d, 0x202d, 0x203d, 0x204d, 0x205d, 0x206d, 0x207d,
    0x208d, 0x209d, 0x20ad, 0x20bd, 0x20cd, 0x20dd, 0x20ed, 0x20fd, 0x220d,
    0x100e, 0x201e, 0x202e, 0x203e, 0x204e, 0x205e, 0x206e, 0x207e,
    0x208e, 0x209e, 0x20ae, 0x20be, 0x20ce, 0x20de, 0x20ee, 0x20fe, 0x220e,
    0x100f, 0x201f, 0x202f, 0x203f, 0x204f, 0x205f, 0x206f, 0x207f,
    0x208f, 0x209f, 0x20af, 0x20bf, 0x20cf, 0x20df, 0x20ef, 0x20ff, 0x220f,
    0x1100, 0x2110, 0x2120, 0x2130, 0x2140, 0x2150, 0x2160, 0x2170,
    0x2180, 0x2190, 0x21a0, 0x21b0, 0x21c0, 0x21d0, 0x21e0, 0x21f0, 0x2300,
};

const float *const ff_aac_codebook_vector_vals[] = {
    codebook_vector0_vals, codebook_vector0_vals,
    codebook_vector10_vals, codebook_vector10_vals,
    codebook_vector4_vals, codebook_vector4_vals,
    codebook_vector10_vals, codebook_vector10_vals,
    codebook_vector10_vals, codebook_vector10_vals,
    codebook_vector10_vals,
};

const uint16_t *const ff_aac_codebook_vector_idx[] = {
    codebook_vector02_idx, codebook_vector02_idx,
    codebook_vector02_idx, codebook_vector02_idx,
    codebook_vector4_idx, codebook_vector4_idx,
    codebook_vector6_idx, codebook_vector6_idx,
    codebook_vector8_idx, codebook_vector8_idx,
    codebook_vector10_idx,
};
static const uint16_t swb_offset_1024_96[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 48, 52, 56, 64,
     72, 80, 88, 96, 108, 120, 132, 144,
    156, 172, 188, 212, 240, 276, 320, 384,
    448, 512, 576, 640, 704, 768, 832, 896,
    960, 1024
};

static const uint16_t swb_offset_128_96[] = {
    0, 4, 8, 12, 16, 20, 24, 32, 40, 48, 64, 92, 128
};

static const uint16_t swb_offset_1024_64[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 48, 52, 56, 64,
     72, 80, 88, 100, 112, 124, 140, 156,
    172, 192, 216, 240, 268, 304, 344, 384,
    424, 464, 504, 544, 584, 624, 664, 704,
    744, 784, 824, 864, 904, 944, 984, 1024
};

static const uint16_t swb_offset_1024_48[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 48, 56, 64, 72, 80,
     88, 96, 108, 120, 132, 144, 160, 176,
    196, 216, 240, 264, 292, 320, 352, 384,
    416, 448, 480, 512, 544, 576, 608, 640,
    672, 704, 736, 768, 800, 832, 864, 896,
    928, 1024
};

static const uint16_t swb_offset_512_48[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 48, 52, 56, 60,
     68, 76, 84, 92, 100, 112, 124, 136,
    148, 164, 184, 208, 236, 268, 300, 332,
    364, 396, 428, 460, 512
};

static const uint16_t swb_offset_480_48[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 48, 52, 56, 64,
     72, 80, 88, 96, 108, 120, 132, 144,
    156, 172, 188, 212, 240, 272, 304, 336,
    368, 400, 432, 480
};

static const uint16_t swb_offset_128_48[] = {
     0, 4, 8, 12, 16, 20, 28, 36,
    44, 56, 68, 80, 96, 112, 128
};

static const uint16_t swb_offset_1024_32[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 48, 56, 64, 72, 80,
     88, 96, 108, 120, 132, 144, 160, 176,
    196, 216, 240, 264, 292, 320, 352, 384,
    416, 448, 480, 512, 544, 576, 608, 640,
    672, 704, 736, 768, 800, 832, 864, 896,
    928, 960, 992, 1024
};

static const uint16_t swb_offset_512_32[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 48, 52, 56, 64,
     72, 80, 88, 96, 108, 120, 132, 144,
    160, 176, 192, 212, 236, 260, 288, 320,
    352, 384, 416, 448, 480, 512
};

static const uint16_t swb_offset_480_32[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 48, 52, 56, 60,
     64, 72, 80, 88, 96, 104, 112, 124,
    136, 148, 164, 180, 200, 224, 256, 288,
    320, 352, 384, 416, 448, 480
 };

static const uint16_t swb_offset_1024_24[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 52, 60, 68, 76,
     84, 92, 100, 108, 116, 124, 136, 148,
    160, 172, 188, 204, 220, 240, 260, 284,
    308, 336, 364, 396, 432, 468, 508, 552,
    600, 652, 704, 768, 832, 896, 960, 1024
};

static const uint16_t swb_offset_512_24[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 52, 60, 68, 80,
     92, 104, 120, 140, 164, 192, 224, 256,
    288, 320, 352, 384, 416, 448, 480, 512,
};

static const uint16_t swb_offset_480_24[] = {
      0, 4, 8, 12, 16, 20, 24, 28,
     32, 36, 40, 44, 52, 60, 68, 80,
     92, 104, 120, 140, 164, 192, 224, 256,
    288, 320, 352, 384, 416, 448, 480
};

static const uint16_t swb_offset_128_24[] = {
     0, 4, 8, 12, 16, 20, 24, 28,
    36, 44, 52, 64, 76, 92, 108, 128
};

static const uint16_t swb_offset_1024_16[] = {
      0, 8, 16, 24, 32, 40, 48, 56,
     64, 72, 80, 88, 100, 112, 124, 136,
    148, 160, 172, 184, 196, 212, 228, 244,
    260, 280, 300, 320, 344, 368, 396, 424,
    456, 492, 532, 572, 616, 664, 716, 772,
    832, 896, 960, 1024
};

static const uint16_t swb_offset_128_16[] = {
     0, 4, 8, 12, 16, 20, 24, 28,
    32, 40, 48, 60, 72, 88, 108, 128
};

static const uint16_t swb_offset_1024_8[] = {
      0, 12, 24, 36, 48, 60, 72, 84,
     96, 108, 120, 132, 144, 156, 172, 188,
    204, 220, 236, 252, 268, 288, 308, 328,
    348, 372, 396, 420, 448, 476, 508, 544,
    580, 620, 664, 712, 764, 820, 880, 944,
    1024
};

static const uint16_t swb_offset_128_8[] = {
     0, 4, 8, 12, 16, 20, 24, 28,
    36, 44, 52, 60, 72, 88, 108, 128
};

static const uint16_t swb_offset_960_96[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36,
    40, 44, 48, 52, 56, 64, 72, 80, 88, 96,
    108, 120, 132, 144, 156, 172, 188, 212, 240, 276,
    320, 384, 448, 512, 576, 640, 704, 768, 832, 896,
    960
};

static const uint16_t swb_offset_960_64[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36,
    40, 44, 48, 52, 56, 64, 72, 80, 88, 100,
    112, 124, 140, 156, 172, 192, 216, 240, 268, 304,
    344, 384, 424, 464, 504, 544, 584, 624, 664, 704,
    744, 784, 824, 864, 904, 944, 960
};

static const uint16_t swb_offset_960_48[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36,
    40, 48, 56, 64, 72, 80, 88, 96, 108, 120,
    132, 144, 160, 176, 196, 216, 240, 264, 292, 320,
    352, 384, 416, 448, 480, 512, 544, 576, 608, 640,
    672, 704, 736, 768, 800, 832, 864, 896, 928, 960
};

static const uint16_t swb_offset_960_32[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36,
    40, 48, 56, 64, 72, 80, 88, 96, 108, 120,
    132, 144, 160, 176, 196, 216, 240, 264, 292, 320,
    352, 384, 416, 448, 480, 512, 544, 576, 608, 640,
    672, 704, 736, 768, 800, 832, 864, 896, 928, 960
};

static const uint16_t swb_offset_960_24[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36,
    40, 44, 52, 60, 68, 76, 84, 92, 100, 108,
    116, 124, 136, 148, 160, 172, 188, 204, 220, 240,
    260, 284, 308, 336, 364, 396, 432, 468, 508, 552,
    600, 652, 704, 768, 832, 896, 960
};

static const uint16_t swb_offset_960_16[] =
{
    0, 8, 16, 24, 32, 40, 48, 56, 64, 72,
    80, 88, 100, 112, 124, 136, 148, 160, 172, 184,
    196, 212, 228, 244, 260, 280, 300, 320, 344, 368,
    396, 424, 456, 492, 532, 572, 616, 664, 716, 772,
    832, 896, 960
};

static const uint16_t swb_offset_960_8[] =
{
    0, 12, 24, 36, 48, 60, 72, 84, 96, 108,
    120, 132, 144, 156, 172, 188, 204, 220, 236, 252,
    268, 288, 308, 328, 348, 372, 396, 420, 448, 476,
    508, 544, 580, 620, 664, 712, 764, 820, 880, 944,
    960
};


static const uint16_t swb_offset_120_96[] =
{
    0, 4, 8, 12, 16, 20, 24, 32, 40, 48, 64, 92, 120
};

static const uint16_t swb_offset_120_64[] =
{
    0, 4, 8, 12, 16, 20, 24, 32, 40, 48, 64, 92, 120
};

static const uint16_t swb_offset_120_48[] =
{
    0, 4, 8, 12, 16, 20, 28, 36, 44, 56, 68, 80, 96, 112, 120
};

static const uint16_t swb_offset_120_24[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 36, 44, 52, 64, 76, 92, 108, 120
};

static const uint16_t swb_offset_120_16[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 40, 48, 60, 72, 88, 108, 120
};

static const uint16_t swb_offset_120_8[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 36, 44, 52, 60, 72, 88, 108, 120
};

const uint16_t * const ff_swb_offset_1024[] = {
    swb_offset_1024_96, swb_offset_1024_96, swb_offset_1024_64,
    swb_offset_1024_48, swb_offset_1024_48, swb_offset_1024_32,
    swb_offset_1024_24, swb_offset_1024_24, swb_offset_1024_16,
    swb_offset_1024_16, swb_offset_1024_16, swb_offset_1024_8,
    swb_offset_1024_8
};

const uint16_t * const ff_swb_offset_960[] = {
    swb_offset_960_96, swb_offset_960_96, swb_offset_960_64,
    swb_offset_960_48, swb_offset_960_48, swb_offset_960_32,
    swb_offset_960_24, swb_offset_960_24, swb_offset_960_16,
    swb_offset_960_16, swb_offset_960_16, swb_offset_960_8,
    swb_offset_960_8
};

const uint16_t * const ff_swb_offset_512[] = {
    
   ((void *)0)
       , 
                       ((void *)0)
                           , 
                                           ((void *)0)
                                               ,
    swb_offset_512_48, swb_offset_512_48, swb_offset_512_32,
    swb_offset_512_24, swb_offset_512_24, 
                                           ((void *)0)
                                               ,
    
   ((void *)0)
       , 
                       ((void *)0)
                           , 
                                           ((void *)0)
                                               ,
    
   ((void *)0)

};

const uint16_t * const ff_swb_offset_480[] = {
    
   ((void *)0)
       , 
                       ((void *)0)
                           , 
                                           ((void *)0)
                                               ,
    swb_offset_480_48, swb_offset_480_48, swb_offset_480_32,
    swb_offset_480_24, swb_offset_480_24, 
                                           ((void *)0)
                                               ,
    
   ((void *)0)
       , 
                       ((void *)0)
                           , 
                                           ((void *)0)
                                               ,
    
   ((void *)0)

};

const uint16_t * const ff_swb_offset_128[] = {


    swb_offset_128_96, swb_offset_128_96, swb_offset_128_96,
    swb_offset_128_48, swb_offset_128_48, swb_offset_128_48,
    swb_offset_128_24, swb_offset_128_24, swb_offset_128_16,
    swb_offset_128_16, swb_offset_128_16, swb_offset_128_8,
    swb_offset_128_8
};

const uint16_t * const ff_swb_offset_120[] = {
    swb_offset_120_96, swb_offset_120_96, swb_offset_120_96,
    swb_offset_120_48, swb_offset_120_48, swb_offset_120_48,
    swb_offset_120_24, swb_offset_120_24, swb_offset_120_16,
    swb_offset_120_16, swb_offset_120_16, swb_offset_120_8,
    swb_offset_120_8
};
const uint8_t ff_tns_max_bands_1024[] = {
    31, 31, 34, 40, 42, 51, 46, 46, 42, 42, 42, 39, 39
};

const uint8_t ff_tns_max_bands_512[] = {
    0, 0, 0, 31, 32, 37, 31, 31, 0, 0, 0, 0, 0
};

const uint8_t ff_tns_max_bands_480[] = {
    0, 0, 0, 31, 32, 37, 30, 30, 0, 0, 0, 0, 0
};

const uint8_t ff_tns_max_bands_128[] = {
    9, 9, 10, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14
};


const float __attribute__ ((aligned (32))) ff_aac_eld_window_512[1920] = {
     0.00338834, 0.00567745, 0.00847677, 0.01172641,
     0.01532555, 0.01917664, 0.02318809, 0.02729259,
     0.03144503, 0.03560261, 0.03972499, 0.04379783,
     0.04783094, 0.05183357, 0.05581342, 0.05977723,
     0.06373173, 0.06768364, 0.07163937, 0.07559976,
     0.07956096, 0.08352024, 0.08747623, 0.09143035,
     0.09538618, 0.09934771, 0.10331917, 0.10730456,
     0.11130697, 0.11532867, 0.11937133, 0.12343922,
     0.12753911, 0.13167705, 0.13585812, 0.14008529,
     0.14435986, 0.14868291, 0.15305531, 0.15747594,
     0.16194193, 0.16645070, 0.17099991, 0.17558633,
     0.18020600, 0.18485548, 0.18953191, 0.19423322,
     0.19895800, 0.20370512, 0.20847374, 0.21326312,
     0.21807244, 0.22290083, 0.22774742, 0.23261210,
     0.23749542, 0.24239767, 0.24731889, 0.25225887,
     0.25721719, 0.26219330, 0.26718648, 0.27219630,
     0.27722262, 0.28226514, 0.28732336, 0.29239628,
     0.29748247, 0.30258055, 0.30768914, 0.31280508,
     0.31792385, 0.32304172, 0.32815579, 0.33326397,
     0.33836470, 0.34345661, 0.34853868, 0.35361188,
     0.35867865, 0.36374072, 0.36879900, 0.37385347,
     0.37890349, 0.38394836, 0.38898730, 0.39401912,
     0.39904236, 0.40405575, 0.40905820, 0.41404819,
     0.41902398, 0.42398423, 0.42892805, 0.43385441,
     0.43876210, 0.44365014, 0.44851786, 0.45336632,
     0.45819759, 0.46301302, 0.46781309, 0.47259722,
     0.47736435, 0.48211365, 0.48684450, 0.49155594,
     0.49624679, 0.50091636, 0.50556440, 0.51019132,
     0.51479771, 0.51938391, 0.52394998, 0.52849587,
     0.53302151, 0.53752680, 0.54201160, 0.54647575,
     0.55091916, 0.55534181, 0.55974376, 0.56412513,
     0.56848615, 0.57282710, 0.57714834, 0.58145030,
     0.58492489, 0.58918511, 0.59342326, 0.59763936,
     0.60183347, 0.60600561, 0.61015581, 0.61428412,
     0.61839056, 0.62247517, 0.62653799, 0.63057912,
     0.63459872, 0.63859697, 0.64257403, 0.64653001,
     0.65046495, 0.65437887, 0.65827181, 0.66214383,
     0.66599499, 0.66982535, 0.67363499, 0.67742394,
     0.68119219, 0.68493972, 0.68866653, 0.69237258,
     0.69605778, 0.69972207, 0.70336537, 0.70698758,
     0.71058862, 0.71416837, 0.71772674, 0.72126361,
     0.72477889, 0.72827246, 0.73174419, 0.73519392,
     0.73862141, 0.74202643, 0.74540874, 0.74876817,
     0.75210458, 0.75541785, 0.75870785, 0.76197437,
     0.76521709, 0.76843570, 0.77162988, 0.77479939,
     0.77794403, 0.78106359, 0.78415789, 0.78722670,
     0.79026979, 0.79328694, 0.79627791, 0.79924244,
     0.80218027, 0.80509112, 0.80797472, 0.81083081,
     0.81365915, 0.81645949, 0.81923160, 0.82197528,
     0.82469037, 0.82737673, 0.83003419, 0.83266262,
     0.83526186, 0.83783176, 0.84037217, 0.84288297,
     0.84536401, 0.84781517, 0.85023632, 0.85262739,
     0.85498836, 0.85731921, 0.85961993, 0.86189052,
     0.86413101, 0.86634140, 0.86852173, 0.87067211,
     0.87279275, 0.87488384, 0.87694559, 0.87897824,
     0.88098206, 0.88295729, 0.88490423, 0.88682332,
     0.88871519, 0.89058048, 0.89241983, 0.89423391,
     0.89602338, 0.89778893, 0.89953126, 0.90125142,
     0.90295086, 0.90463104, 0.90629341, 0.90793946,
     0.90957067, 0.91118856, 0.91279464, 0.91439073,
     0.91597898, 0.91756153, 0.91914049, 0.92071690,
     0.92229070, 0.92386182, 0.92542993, 0.92698946,
     0.92852960, 0.93003929, 0.93150727, 0.93291739,
     0.93424863, 0.93547974, 0.93658982, 0.93756587,
     0.93894072, 0.93922780, 0.93955477, 0.93991290,
     0.94029104, 0.94067794, 0.94106258, 0.94144084,
     0.94181549, 0.94218963, 0.94256628, 0.94294662,
     0.94332998, 0.94371562, 0.94410280, 0.94449122,
     0.94488106, 0.94527249, 0.94566568, 0.94606074,
     0.94645772, 0.94685665, 0.94725759, 0.94766054,
     0.94806547, 0.94847234, 0.94888115, 0.94929190,
     0.94970469, 0.95011960, 0.95053672, 0.95095604,
     0.95137751, 0.95180105, 0.95222658, 0.95265413,
     0.95308380, 0.95351571, 0.95394994, 0.95438653,
     0.95482538, 0.95526643, 0.95570958, 0.95615486,
     0.95660234, 0.95705214, 0.95750433, 0.95795892,
     0.95841582, 0.95887493, 0.95933616, 0.95979949,
     0.96026500, 0.96073277, 0.96120286, 0.96167526,
     0.96214986, 0.96262655, 0.96310522, 0.96358586,
     0.96406853, 0.96455330, 0.96504026, 0.96552936,
     0.96602051, 0.96651360, 0.96700850, 0.96750520,
     0.96800376, 0.96850424, 0.96900670, 0.96951112,
     0.97001738, 0.97052533, 0.97103488, 0.97154597,
     0.97205867, 0.97257304, 0.97308915, 0.97360694,
     0.97412631, 0.97464711, 0.97516923, 0.97569262,
     0.97621735, 0.97674350, 0.97727111, 0.97780016,
     0.97833051, 0.97886205, 0.97939463, 0.97992823,
     0.98046291, 0.98099875, 0.98153580, 0.98207405,
     0.98261337, 0.98315364, 0.98369474, 0.98423664,
     0.98477941, 0.98532311, 0.98586780, 0.98641348,
     0.98696003, 0.98750734, 0.98805530, 0.98860389,
     0.98915320, 0.98970328, 0.99025423, 0.99080602,
     0.99135855, 0.99191171, 0.99246541, 0.99301962,
     0.99357443, 0.99412992, 0.99468617, 0.99524320,
     0.99580092, 0.99635926, 0.99691814, 0.99747748,
     0.99803721, 0.99859725, 0.99915752, 0.99971793,
     1.00028215, 1.00084319, 1.00140472, 1.00196665,
     1.00252889, 1.00309139, 1.00365404, 1.00421679,
     1.00477954, 1.00534221, 1.00590474, 1.00646713,
     1.00702945, 1.00759179, 1.00815424, 1.00871678,
     1.00927930, 1.00984169, 1.01040384, 1.01096575,
     1.01152747, 1.01208910, 1.01265070, 1.01321226,
     1.01377365, 1.01433478, 1.01489551, 1.01545584,
     1.01601582, 1.01657553, 1.01713502, 1.01769427,
     1.01825316, 1.01881154, 1.01936929, 1.01992639,
     1.02048289, 1.02103888, 1.02159441, 1.02214945,
     1.02270387, 1.02325751, 1.02381025, 1.02436204,
     1.02491295, 1.02546304, 1.02601238, 1.02656092,
     1.02710853, 1.02765508, 1.02820041, 1.02874449,
     1.02928737, 1.02982913, 1.03036981, 1.03090937,
     1.03144768, 1.03198460, 1.03252000, 1.03305384,
     1.03358617, 1.03411707, 1.03464659, 1.03517470,
     1.03570128, 1.03622620, 1.03674934, 1.03727066,
     1.03779024, 1.03830815, 1.03882446, 1.03933914,
     1.03985206, 1.04036312, 1.04087217, 1.04137920,
     1.04188428, 1.04238748, 1.04288888, 1.04338845,
     1.04388610, 1.04438170, 1.04487515, 1.04536645,
     1.04585569, 1.04634297, 1.04682838, 1.04731192,
     1.04779350, 1.04827303, 1.04875042, 1.04922568,
     1.04969891, 1.05017022, 1.05063974, 1.05110746,
     1.05157332, 1.05203721, 1.05249907, 1.05295889,
     1.05341676, 1.05387277, 1.05432700, 1.05477948,
     1.05523018, 1.05567906, 1.05612608, 1.05657124,
     1.05701459, 1.05745616, 1.05789601, 1.05833426,
     1.05877109, 1.05920669, 1.05964125, 1.06007444,
     1.06050542, 1.06093335, 1.06135746, 1.06177909,
     1.06220164, 1.06262858, 1.06306309, 1.06350050,
     1.06392837, 1.06433391, 1.06470443, 1.06502996,
     1.06481076, 1.06469765, 1.06445004, 1.06408002,
     1.06361382, 1.06307719, 1.06249453, 1.06188365,
     1.06125612, 1.06062291, 1.05999418, 1.05937132,
     1.05874726, 1.05811486, 1.05746728, 1.05680000,
     1.05611070, 1.05539715, 1.05465735, 1.05389329,
     1.05311083, 1.05231578, 1.05151372, 1.05070811,
     1.04990044, 1.04909210, 1.04828434, 1.04747647,
     1.04666590, 1.04585003, 1.04502628, 1.04419009,
     1.04333499, 1.04245452, 1.04154244, 1.04059452,
     1.03960846, 1.03858207, 1.03751326, 1.03640189,
     1.03524976, 1.03405868, 1.03283047, 1.03156812,
     1.03027574, 1.02895743, 1.02761717, 1.02625804,
     1.02488222, 1.02349184, 1.02208892, 1.02067450,
     1.01924861, 1.01781123, 1.01636229, 1.01490045,
     1.01342315, 1.01192778, 1.01041175, 1.00887284,
     1.00730915, 1.00571882, 1.00409996, 1.00245032,
     1.00076734, 0.99904842, 0.99729101, 0.99549380,
     0.99365664, 0.99177946, 0.98986234, 0.98791024,
     0.98593294, 0.98394037, 0.98194226, 0.97994532,
     0.97795324, 0.97596955, 0.97399748, 0.97203326,
     0.97006624, 0.96808546, 0.96608018, 0.96404416,
     0.96197556, 0.95987276, 0.95773420, 0.95556018,
     0.95335291, 0.95111462, 0.94884764, 0.94655663,
     0.94424858, 0.94193055, 0.93960953, 0.93729154,
     0.93498157, 0.93268456, 0.93040503, 0.92813771,
     0.92586755, 0.92357910, 0.92125731, 0.91889642,
     0.91649998, 0.91407191, 0.91161623, 0.90913975,
     0.90665202, 0.90416271, 0.90168115, 0.89920934,
     0.89674189, 0.89427312, 0.89179743, 0.88931147,
     0.88681415, 0.88430445, 0.88178141, 0.87924528,
     0.87669753, 0.87413966, 0.87157318, 0.86899958,
     0.86642037, 0.86383703, 0.86125106, 0.85866393,
     0.85604236, 0.85344385, 0.85083093, 0.84820550,
     0.84556943, 0.84292458, 0.84027278, 0.83761586,
     0.83495565, 0.83229393, 0.82963243, 0.82697135,
     0.82430933, 0.82164496, 0.81897669, 0.81630017,
     0.81360822, 0.81089355, 0.80814924, 0.80537741,
     0.80258920, 0.79979611, 0.79700954, 0.79423813,
     0.79148780, 0.78876432, 0.78607290, 0.78340590,
     0.78074288, 0.77806279, 0.77534514, 0.77258187,
     0.76977737, 0.76693654, 0.76406441, 0.76116851,
     0.75825892, 0.75534582, 0.75243924, 0.74954634,
     0.74667135, 0.74381840, 0.74099145, 0.73819147,
     0.73541641, 0.73266408, 0.72993193, 0.72720913,
     0.72447661, 0.72171494, 0.71890515, 0.71603932,
     0.71312056, 0.71015250, 0.70713900, 0.70409084,
     0.70102565, 0.69796137, 0.69491556, 0.69189772,
     0.68890931, 0.68595141, 0.68302498, 0.68012852,
     0.67725801, 0.67440936, 0.67157841, 0.66876081,
     0.66595195, 0.66314722, 0.66034194, 0.65753027,
     0.65470525, 0.65185984, 0.64898709, 0.64608214,
     0.64314221, 0.64016460, 0.63714680, 0.63409034,
     0.63100082, 0.62788400, 0.62474577, 0.62159473,
     0.61844225, 0.61529977, 0.61217866, 0.60908811,
     0.60603510, 0.60302654, 0.60006916, 0.59716588,
     0.59431580, 0.59151787, 0.58877068, 0.58606495,
     0.58338353, 0.58070891, 0.57802356, 0.57530864,
     0.57254404, 0.56970958, 0.56678577, 0.56376860,
     0.56066951, 0.55750064, 0.55427451, 0.55101301,
     0.54774732, 0.54450907, 0.54132936, 0.53822744,
     0.53521072, 0.53228613, 0.52945979, 0.52671997,
     0.52403708, 0.52138072, 0.51872085, 0.51603570,
     0.51331170, 0.51053560, 0.50769466, 0.50478931,
     0.50183308, 0.49884001, 0.49582406, 0.49279905,
     0.48985748, 0.48679641, 0.48379429, 0.48085363,
     0.47796576, 0.47512151, 0.47231151, 0.46952402,
     0.46674486, 0.46395978, 0.46115496, 0.45832607,
     0.45547830, 0.45261727, 0.44974866, 0.44688011,
     0.44402125, 0.44118178, 0.43837094, 0.43558772,
     0.43282082, 0.43005847, 0.42728913, 0.42450572,
     0.42170567, 0.41888658, 0.41604633, 0.41318897,
     0.41032472, 0.40746405, 0.40461724, 0.40178943,
     0.39898066, 0.39619073, 0.39341940, 0.39066519,
     0.38792536, 0.38519713, 0.38247773, 0.37976476,
     0.37705620, 0.37435006, 0.37164438, 0.36893869,
     0.36623396, 0.36353124, 0.36083153, 0.35813533,
     0.35544262, 0.35275338, 0.35006755, 0.34738530,
     0.34470699, 0.34203296, 0.33936359, 0.33669922,
     0.33404027, 0.33138711, 0.32874013, 0.32609944,
     0.32346493, 0.32083645, 0.31821388, 0.31559703,
     0.31298573, 0.31037987, 0.30777941, 0.30518446,
     0.30259525, 0.30001202, 0.29743499, 0.29486428,
     0.29229989, 0.28974179, 0.28718997, 0.28464452,
     0.28210562, 0.27957346, 0.27704820, 0.27452992,
     0.27201854, 0.26951399, 0.26701622, 0.26452533,
     0.26204158, 0.25956526, 0.25709662, 0.25463583,
     0.25218294, 0.24973798, 0.24730100, 0.24487207,
     0.24245133, 0.24003893, 0.23763500, 0.23523959,
     0.23285262, 0.23047401, 0.22810369, 0.22574170,
     0.22338818, 0.22104329, 0.21870719, 0.21637986,
     0.21406117, 0.21175095, 0.20944904, 0.20715535,
     0.20486987, 0.20259261, 0.20032356, 0.19806259,
     0.19580944, 0.19356385, 0.19132556, 0.18909442,
     0.18687040, 0.18465350, 0.18244372, 0.18024164,
     0.17804841, 0.17586521, 0.17369322, 0.17153360,
     0.16938755, 0.16725622, 0.16514081, 0.16304247,
     0.16098974, 0.15896561, 0.15696026, 0.15497259,
     0.15300151, 0.15104590, 0.14910466, 0.14717666,
     0.14526081, 0.14335599, 0.14146111, 0.13957570,
     0.13769993, 0.13583399, 0.13397806, 0.13213229,
     0.13029682, 0.12847178, 0.12665729, 0.12485353,
     0.12306074, 0.12127916, 0.11950900, 0.11775043,
     0.11600347, 0.11426820, 0.11254464, 0.11083292,
     0.10913318, 0.10744559, 0.10577028, 0.10410733,
     0.10245672, 0.10081842, 0.09919240, 0.09757872,
     0.09597750, 0.09438884, 0.09281288, 0.09124964,
     0.08969907, 0.08816111, 0.08663570, 0.08512288,
     0.08362274, 0.08213540, 0.08066096, 0.07919944,
     0.07775076, 0.07631484, 0.07489161, 0.07348108,
     0.07208335, 0.07069851, 0.06932666, 0.06796781,
     0.06662187, 0.06528874, 0.06396833, 0.06266065,
     0.06136578, 0.06008380, 0.05881480, 0.05755876,
     0.05631557, 0.05508511, 0.05386728, 0.05266206,
     0.05146951, 0.05028971, 0.04912272, 0.04796855,
     0.04682709, 0.04569825, 0.04458194, 0.04347817,
     0.04238704, 0.04130868, 0.04024318, 0.03919056,
     0.03815071, 0.03712352, 0.03610890, 0.03510679,
     0.03411720, 0.03314013, 0.03217560, 0.03122343,
     0.03028332, 0.02935494, 0.02843799, 0.02753230,
     0.02663788, 0.02575472, 0.02488283, 0.02402232,
     0.02317341, 0.02233631, 0.02151124, 0.02069866,
     0.01989922, 0.01911359, 0.01834241, 0.01758563,
     0.01684248, 0.01611219, 0.01539397, 0.01468726,
     0.01399167, 0.01330687, 0.01263250, 0.01196871,
     0.01131609, 0.01067527, 0.01004684, 0.00943077,
     0.00882641, 0.00823307, 0.00765011, 0.00707735,
     0.00651513, 0.00596377, 0.00542364, 0.00489514,
     0.00437884, 0.00387530, 0.00338509, 0.00290795,
     0.00244282, 0.00198860, 0.00154417, 0.00110825,
     0.00067934, 0.00025589, -0.00016357, -0.00057897,
    -0.00098865, -0.00139089, -0.00178397, -0.00216547,
    -0.00253230, -0.00288133, -0.00320955, -0.00351626,
    -0.00380315, -0.00407198, -0.00432457, -0.00456373,
    -0.00479326, -0.00501699, -0.00523871, -0.00546066,
    -0.00568360, -0.00590821, -0.00613508, -0.00636311,
    -0.00658944, -0.00681117, -0.00702540, -0.00722982,
    -0.00742268, -0.00760226, -0.00776687, -0.00791580,
    -0.00804933, -0.00816774, -0.00827139, -0.00836122,
    -0.00843882, -0.00850583, -0.00856383, -0.00861430,
    -0.00865853, -0.00869781, -0.00873344, -0.00876633,
    -0.00879707, -0.00882622, -0.00885433, -0.00888132,
    -0.00890652, -0.00892925, -0.00894881, -0.00896446,
    -0.00897541, -0.00898088, -0.00898010, -0.00897234,
    -0.00895696, -0.00893330, -0.00890076, -0.00885914,
    -0.00880875, -0.00874987, -0.00868282, -0.00860825,
    -0.00852716, -0.00844055, -0.00834941, -0.00825485,
    -0.00815807, -0.00806025, -0.00796253, -0.00786519,
    -0.00776767, -0.00766937, -0.00756971, -0.00746790,
    -0.00736305, -0.00725422, -0.00714055, -0.00702161,
    -0.00689746, -0.00676816, -0.00663381, -0.00649489,
    -0.00635230, -0.00620694, -0.00605969, -0.00591116,
    -0.00576167, -0.00561155, -0.00546110, -0.00531037,
    -0.00515917, -0.00500732, -0.00485462, -0.00470075,
    -0.00454530, -0.00438786, -0.00422805, -0.00406594,
    -0.00390204, -0.00373686, -0.00357091, -0.00340448,
    -0.00323770, -0.00307066, -0.00290344, -0.00273610,
    -0.00256867, -0.00240117, -0.00223365, -0.00206614,
    -0.00189866, -0.00173123, -0.00156390, -0.00139674,
    -0.00122989, -0.00106351, -0.00089772, -0.00073267,
    -0.00056849, -0.00040530, -0.00024324, -0.00008241,
     0.00008214, 0.00024102, 0.00039922, 0.00055660,
     0.00071299, 0.00086826, 0.00102224, 0.00117480,
     0.00132579, 0.00147507, 0.00162252, 0.00176804,
     0.00191161, 0.00205319, 0.00219277, 0.00233029,
     0.00246567, 0.00259886, 0.00272975, 0.00285832,
     0.00298453, 0.00310839, 0.00322990, 0.00334886,
     0.00346494, 0.00357778, 0.00368706, 0.00379273,
     0.00389501, 0.00399411, 0.00409020, 0.00418350,
     0.00427419, 0.00436249, 0.00444858, 0.00453250,
     0.00461411, 0.00469328, 0.00476988, 0.00484356,
     0.00491375, 0.00497987, 0.00504139, 0.00509806,
     0.00514990, 0.00519693, 0.00523920, 0.00527700,
     0.00531083, 0.00534122, 0.00536864, 0.00539357,
     0.00541649, 0.00543785, 0.00545809, 0.00547713,
     0.00549441, 0.00550936, 0.00552146, 0.00553017,
     0.00553494, 0.00553524, 0.00553058, 0.00552065,
     0.00550536, 0.00548459, 0.00545828, 0.00542662,
     0.00539007, 0.00534910, 0.00530415, 0.00525568,
     0.00520417, 0.00515009, 0.00509387, 0.00503595,
     0.00497674, 0.00491665, 0.00485605, 0.00479503,
     0.00473336, 0.00467082, 0.00460721, 0.00454216,
     0.00447517, 0.00440575, 0.00433344, 0.00425768,
     0.00417786, 0.00409336, 0.00400363, 0.00390837,
     0.00380759, 0.00370130, 0.00358952, 0.00347268,
     0.00335157, 0.00322699, 0.00309975, 0.00297088,
     0.00284164, 0.00271328, 0.00258700, 0.00246328,
     0.00234195, 0.00222281, 0.00210562, 0.00198958,
     0.00187331, 0.00175546, 0.00163474, 0.00151020,
     0.00138130, 0.00124750, 0.00110831, 0.00096411,
     0.00081611, 0.00066554, 0.00051363, 0.00036134,
     0.00020940, 0.00005853, -0.00009058, -0.00023783,
    -0.00038368, -0.00052861, -0.00067310, -0.00081757,
    -0.00096237, -0.00110786, -0.00125442, -0.00140210,
    -0.00155065, -0.00169984, -0.00184940, -0.00199910,
    -0.00214872, -0.00229798, -0.00244664, -0.00259462,
    -0.00274205, -0.00288912, -0.00303596, -0.00318259,
    -0.00332890, -0.00347480, -0.00362024, -0.00376519,
    -0.00390962, -0.00405345, -0.00419658, -0.00433902,
    -0.00448085, -0.00462219, -0.00476309, -0.00490357,
    -0.00504361, -0.00518321, -0.00532243, -0.00546132,
    -0.00559988, -0.00573811, -0.00587602, -0.00601363,
    -0.00615094, -0.00628795, -0.00642466, -0.00656111,
    -0.00669737, -0.00683352, -0.00696963, -0.00710578,
    -0.00724208, -0.00737862, -0.00751554, -0.00765295,
    -0.00779098, -0.00792976, -0.00806941, -0.00821006,
    -0.00835183, -0.00849485, -0.00863926, -0.00878522,
    -0.00893293, -0.00908260, -0.00923444, -0.00938864,
    -0.00954537, -0.00970482, -0.00986715, -0.01003173,
    -0.01019711, -0.01036164, -0.01052357, -0.01068184,
    -0.01083622, -0.01098652, -0.01113252, -0.01127409,
    -0.01141114, -0.01154358, -0.01167135, -0.01179439,
    -0.01191268, -0.01202619, -0.01213493, -0.01223891,
    -0.01233817, -0.01243275, -0.01252272, -0.01260815,
    -0.01268915, -0.01276583, -0.01283832, -0.01290685,
    -0.01297171, -0.01303320, -0.01309168, -0.01314722,
    -0.01319969, -0.01324889, -0.01329466, -0.01333693,
    -0.01337577, -0.01341125, -0.01344345, -0.01347243,
    -0.01349823, -0.01352089, -0.01354045, -0.01355700,
    -0.01357068, -0.01358164, -0.01359003, -0.01359587,
    -0.01359901, -0.01359931, -0.01359661, -0.01359087,
    -0.01358219, -0.01357065, -0.01355637, -0.01353935,
    -0.01351949, -0.01349670, -0.01347088, -0.01344214,
    -0.01341078, -0.01337715, -0.01334158, -0.01330442,
    -0.01326601, -0.01322671, -0.01318689, -0.01314692,
    -0.01310123, -0.01306470, -0.01302556, -0.01298381,
    -0.01293948, -0.01289255, -0.01284305, -0.01279095,
    -0.01273625, -0.01267893, -0.01261897, -0.01255632,
    -0.01249096, -0.01242283, -0.01235190, -0.01227827,
    -0.01220213, -0.01212366, -0.01204304, -0.01196032,
    -0.01187543, -0.01178829, -0.01169884, -0.01160718,
    -0.01151352, -0.01141809, -0.01132111, -0.01122272,
    -0.01112304, -0.01102217, -0.01092022, -0.01081730,
    -0.01071355, -0.01060912, -0.01050411, -0.01039854,
    -0.01029227, -0.01018521, -0.01007727, -0.00996859,
    -0.00985959, -0.00975063, -0.00964208, -0.00953420,
    -0.00942723, -0.00932135, -0.00921677, -0.00911364,
    -0.00901208, -0.00891220, -0.00881412, -0.00871792,
    -0.00862369, -0.00853153, -0.00844149, -0.00835360,
    -0.00826785, -0.00818422, -0.00810267, -0.00802312,
    -0.00794547, -0.00786959, -0.00779533, -0.00772165,
    -0.00764673, -0.00756886, -0.00748649, -0.00739905,
    -0.00730681, -0.00721006, -0.00710910, -0.00700419,
    -0.00689559, -0.00678354, -0.00666829, -0.00655007,
    -0.00642916, -0.00630579, -0.00618022, -0.00605267,
    -0.00592333, -0.00579240, -0.00566006, -0.00552651,
    -0.00539194, -0.00525653, -0.00512047, -0.00498390,
    -0.00484693, -0.00470969, -0.00457228, -0.00443482,
    -0.00429746, -0.00416034, -0.00402359, -0.00388738,
    -0.00375185, -0.00361718, -0.00348350, -0.00335100,
    -0.00321991, -0.00309043, -0.00296276, -0.00283698,
    -0.00271307, -0.00259098, -0.00247066, -0.00235210,
    -0.00223531, -0.00212030, -0.00200709, -0.00189576,
    -0.00178647, -0.00167936, -0.00157457, -0.00147216,
    -0.00137205, -0.00127418, -0.00117849, -0.00108498,
    -0.00099375, -0.00090486, -0.00081840, -0.00073444,
    -0.00065309, -0.00057445, -0.00049860, -0.00042551,
    -0.00035503, -0.00028700, -0.00022125, -0.00015761,
    -0.00009588, -0.00003583, 0.00002272, 0.00007975,
     0.00013501, 0.00018828, 0.00023933, 0.00028784,
     0.00033342, 0.00037572, 0.00041438, 0.00044939,
     0.00048103, 0.00050958, 0.00053533, 0.00055869,
     0.00058015, 0.00060022, 0.00061935, 0.00063781,
     0.00065568, 0.00067303, 0.00068991, 0.00070619,
     0.00072155, 0.00073567, 0.00074826, 0.00075912,
     0.00076811, 0.00077509, 0.00077997, 0.00078275,
     0.00078351, 0.00078237, 0.00077943, 0.00077484,
     0.00076884, 0.00076160, 0.00075335, 0.00074423,
     0.00073442, 0.00072404, 0.00071323, 0.00070209,
     0.00069068, 0.00067906, 0.00066728, 0.00065534,
     0.00064321, 0.00063086, 0.00061824, 0.00060534,
     0.00059211, 0.00057855, 0.00056462, 0.00055033,
     0.00053566, 0.00052063, 0.00050522, 0.00048949,
     0.00047349, 0.00045728, 0.00044092, 0.00042447,
     0.00040803, 0.00039166, 0.00037544, 0.00035943,
     0.00034371, 0.00032833, 0.00031333, 0.00029874,
     0.00028452, 0.00027067, 0.00025715, 0.00024395,
     0.00023104, 0.00021842, 0.00020606, 0.00019398,
     0.00018218, 0.00017069, 0.00015953, 0.00014871,
     0.00013827, 0.00012823, 0.00011861, 0.00010942,
     0.00010067, 0.00009236, 0.00008448, 0.00007703,
     0.00006999, 0.00006337, 0.00005714, 0.00005129,
     0.00004583, 0.00004072, 0.00003597, 0.00003157,
     0.00002752, 0.00002380, 0.00002042, 0.00001736,
     0.00001461, 0.00001215, 0.00000998, 0.00000807,
     0.00000641, 0.00000499, 0.00000378, 0.00000278,
     0.00000196, 0.00000132, 0.00000082, 0.00000046,
     0.00000020, 0.00000005, -0.00000003, -0.00000006,
    -0.00000004, -0.00000001, 0.00000001, 0.00000001,
     0.00000001, 0.00000001, -0.00000001, -0.00000004,
    -0.00000005, -0.00000003, 0.00000005, 0.00000020,
     0.00000043, 0.00000077, 0.00000123, 0.00000183,
     0.00000257, 0.00000348, 0.00000455, 0.00000581,
     0.00000727, 0.00000893, 0.00001080, 0.00001290,
     0.00001522, 0.00001778, 0.00002057, 0.00002362,
     0.00002691, 0.00003044, 0.00003422, 0.00003824,
     0.00004250, 0.00004701, 0.00005176, 0.00005676,
     0.00006200, 0.00006749, 0.00007322, 0.00007920,
     0.00008541, 0.00009186, 0.00009854, 0.00010543,
     0.00011251, 0.00011975, 0.00012714, 0.00013465,
     0.00014227, 0.00014997, 0.00015775, 0.00016558,
     0.00017348, 0.00018144, 0.00018947, 0.00019756,
     0.00020573, 0.00021399, 0.00022233, 0.00023076,
     0.00023924, 0.00024773, 0.00025621, 0.00026462,
     0.00027293, 0.00028108, 0.00028904, 0.00029675,
     0.00030419, 0.00031132, 0.00031810, 0.00032453,
     0.00033061, 0.00033632, 0.00034169, 0.00034672,
     0.00035142, 0.00035580, 0.00035988, 0.00036369,
     0.00036723, 0.00037053, 0.00037361, 0.00037647,
     0.00037909, 0.00038145, 0.00038352, 0.00038527,
     0.00038663, 0.00038757, 0.00038801, 0.00038790,
     0.00038717, 0.00038572, 0.00038350, 0.00038044,
     0.00037651, 0.00037170, 0.00036597, 0.00035936,
     0.00035191, 0.00034370, 0.00033480, 0.00032531,
     0.00031537, 0.00030512, 0.00029470, 0.00028417,
     0.00027354, 0.00026279, 0.00025191, 0.00024081,
     0.00022933, 0.00021731, 0.00020458, 0.00019101,
     0.00017654, 0.00016106, 0.00014452, 0.00012694,
     0.00010848, 0.00008929, 0.00006953, 0.00004935,
     0.00002884, 0.00000813, -0.00001268, -0.00003357,
    -0.00005457, -0.00007574, -0.00009714, -0.00011882,
    -0.00014082, -0.00016318, -0.00018595, -0.00020912,
    -0.00023265, -0.00025650, -0.00028060, -0.00030492,
    -0.00032941, -0.00035400, -0.00037865, -0.00040333,
    -0.00042804, -0.00045279, -0.00047759, -0.00050243,
    -0.00052728, -0.00055209, -0.00057685, -0.00060153,
    -0.00062611, -0.00065056, -0.00067485, -0.00069895,
    -0.00072287, -0.00074660, -0.00077013, -0.00079345,
    -0.00081653, -0.00083936, -0.00086192, -0.00088421,
    -0.00090619, -0.00092786, -0.00094919, -0.00097017,
    -0.00099077, -0.00101098, -0.00103077, -0.00105012,
    -0.00106904, -0.00108750, -0.00110549, -0.00112301,
    -0.00114005, -0.00115660, -0.00117265, -0.00118821,
    -0.00120325, -0.00121779, -0.00123180, -0.00124528,
    -0.00125822, -0.00127061, -0.00128243, -0.00129368,
    -0.00130435, -0.00131445, -0.00132395, -0.00133285,
    -0.00134113, -0.00134878, -0.00135577, -0.00136215,
    -0.00136797, -0.00137333, -0.00137834, -0.00138305,
    -0.00138748, -0.00139163, -0.00139551, -0.00139913,
    -0.00140249, -0.00140559, -0.00140844, -0.00141102,
    -0.00141334, -0.00141538, -0.00141714, -0.00141861,
    -0.00141978, -0.00142064, -0.00142117, -0.00142138,
    -0.00142125, -0.00142077, -0.00141992, -0.00141870,
    -0.00141710, -0.00141510, -0.00141268, -0.00140986,
    -0.00140663, -0.00140301, -0.00139900, -0.00139460,
    -0.00138981, -0.00138464, -0.00137908, -0.00137313,
    -0.00136680, -0.00136010, -0.00135301, -0.00134555,
    -0.00133772, -0.00132952, -0.00132095, -0.00131201,
    -0.00130272, -0.00129307, -0.00128309, -0.00127277,
    -0.00126211, -0.00125113, -0.00123981, -0.00122817,
    -0.00121622, -0.00120397, -0.00119141, -0.00117859,
    -0.00116552, -0.00115223, -0.00113877, -0.00112517,
    -0.00111144, -0.00109764, -0.00108377, -0.00106989,
};


const int __attribute__ ((aligned (32))) ff_aac_eld_window_512_fixed[1920] = {
    0x003783ba, 0x005d04f4, 0x008ae226, 0x00c02021,
    0x00fb1804, 0x013a30a8, 0x017be9e6, 0x01bf296c,
    0x02033204, 0x0247502c, 0x028adab0, 0x02cd9568,
    0x030fa980, 0x03513dc0, 0x03927274, 0x03d363e0,
    0x04142e40, 0x0454edc0, 0x0495bd48, 0x04d6a060,
    0x051786d8, 0x05586548, 0x059935e8, 0x05d9feb0,
    0x061acea0, 0x065bb680, 0x069cc800, 0x06de13f0,
    0x071fa748, 0x07618b80, 0x07a3c7a8, 0x07e66da0,
    0x082999d0, 0x086d6590, 0x08b1e640, 0x08f72850,
    0x093d3120, 0x09840550, 0x09cba880, 0x0a1415f0,
    0x0a5d41b0, 0x0aa720d0, 0x0af1a9a0, 0x0b3cce70,
    0x0b887ec0, 0x0bd4ac10, 0x0c214a70, 0x0c6e5130,
    0x0cbbba50, 0x0d098130, 0x0d57a240, 0x0da61a60,
    0x0df4e620, 0x0e4401d0, 0x0e9369f0, 0x0ee31de0,
    0x0f332000, 0x0f837180, 0x0fd412a0, 0x10250260,
    0x10763f20, 0x10c7c660, 0x11199560, 0x116baa00,
    0x11be0400, 0x1210a1c0, 0x12638180, 0x12b69ee0,
    0x1309f3e0, 0x135d7ac0, 0x13b12dc0, 0x1404ffa0,
    0x1458dd40, 0x14acb720, 0x15008120, 0x15543260,
    0x15a7c460, 0x15fb3160, 0x164e7520, 0x16a193c0,
    0x16f49740, 0x17478720, 0x179a6720, 0x17ed3720,
    0x183ff460, 0x18929c20, 0x18e52b00, 0x19379c00,
    0x1989e900, 0x19dc0ca0, 0x1a2e0280, 0x1a7fc400,
    0x1ad14a00, 0x1b228ec0, 0x1b738ea0, 0x1bc44540,
    0x1c14ada0, 0x1c64c380, 0x1cb48440, 0x1d03f420,
    0x1d531c00, 0x1da20160, 0x1df0a660, 0x1e3f0860,
    0x1e8d2340, 0x1edaf340, 0x1f2875e0, 0x1f75a700,
    0x1fc281e0, 0x200f0380, 0x205b2ac0, 0x20a6f980,
    0x20f27200, 0x213d9600, 0x21886580, 0x21d2e040,
    0x221d0640, 0x2266d6c0, 0x22b05180, 0x22f97580,
    0x23424280, 0x238ab880, 0x23d2d780, 0x241aa040,
    0x246213c0, 0x24a93300, 0x24efff80, 0x25367b40,
    0x256f68c0, 0x25b53580, 0x25faa580, 0x263fb940,
    0x26847080, 0x26c8cbc0, 0x270ccb00, 0x27506e40,
    0x2793b600, 0x27d6a200, 0x281932c0, 0x285b6880,
    0x289d4400, 0x28dec5c0, 0x291feec0, 0x2960bf80,
    0x29a137c0, 0x29e15800, 0x2a212000, 0x2a609080,
    0x2a9fa980, 0x2ade6b40, 0x2b1cd600, 0x2b5aea00,
    0x2b98a740, 0x2bd60d80, 0x2c131cc0, 0x2c4fd500,
    0x2c8c3600, 0x2cc83f00, 0x2d03f040, 0x2d3f48c0,
    0x2d7a48c0, 0x2db4ef40, 0x2def3c40, 0x2e292ec0,
    0x2e62c700, 0x2e9c0400, 0x2ed4e580, 0x2f0d6ac0,
    0x2f4592c0, 0x2f7d5c80, 0x2fb4c6c0, 0x2febd140,
    0x30227b40, 0x3058c400, 0x308eab40, 0x30c43040,
    0x30f95100, 0x312e0d00, 0x31626240, 0x31965040,
    0x31c9d5c0, 0x31fcf240, 0x322fa480, 0x3261ec00,
    0x3293c7c0, 0x32c53680, 0x32f63780, 0x3326c9c0,
    0x3356ec00, 0x33869d00, 0x33b5db80, 0x33e4a700,
    0x3412fdc0, 0x3440df40, 0x346e4a80, 0x349b3e40,
    0x34c7ba00, 0x34f3bd80, 0x351f47c0, 0x354a5840,
    0x3574ee40, 0x359f0900, 0x35c8a840, 0x35f1cb80,
    0x361a71c0, 0x36429a80, 0x366a4580, 0x36917280,
    0x36b82100, 0x36de5180, 0x37040340, 0x372936c0,
    0x374dec40, 0x37722340, 0x3795dc40, 0x37b91780,
    0x37dbd600, 0x37fe18c0, 0x381fe080, 0x38412e00,
    0x38620280, 0x38825f40, 0x38a24540, 0x38c1b680,
    0x38e0b5c0, 0x38ff4540, 0x391d6800, 0x393b20c0,
    0x39587280, 0x39755fc0, 0x3991eb80, 0x39ae1a80,
    0x39c9f280, 0x39e57980, 0x3a00b600, 0x3a1bae00,
    0x3a366800, 0x3a50e9c0, 0x3a6b3a40, 0x3a8560c0,
    0x3a9f6640, 0x3ab95400, 0x3ad332c0, 0x3aed0680,
    0x3b06cf80, 0x3b208d40, 0x3b3a3e80, 0x3b53cb80,
    0x3b6d0780, 0x3b85c380, 0x3b9dd0c0, 0x3bb4eb40,
    0x3bcabac0, 0x3bdee680, 0x3bf11680, 0x3c011440,
    0x3c179ac0, 0x3c1c4f00, 0x3c21aa40, 0x3c278880,
    0x3c2dba80, 0x3c341140, 0x3c3a5e80, 0x3c409100,
    0x3c46b480, 0x3c4cd5c0, 0x3c530180, 0x3c593cc0,
    0x3c5f84c0, 0x3c65d640, 0x3c6c2e40, 0x3c728b40,
    0x3c78ee80, 0x3c7f5840, 0x3c85c940, 0x3c8c4240,
    0x3c92c380, 0x3c994cc0, 0x3c9fde40, 0x3ca67880,
    0x3cad1ac0, 0x3cb3c540, 0x3cba7800, 0x3cc132c0,
    0x3cc7f640, 0x3ccec280, 0x3cd59800, 0x3cdc76c0,
    0x3ce35e80, 0x3cea4f00, 0x3cf147c0, 0x3cf84900,
    0x3cff5340, 0x3d0666c0, 0x3d0d8400, 0x3d14ab40,
    0x3d1bdc00, 0x3d2315c0, 0x3d2a5880, 0x3d31a440,
    0x3d38f900, 0x3d405780, 0x3d47c040, 0x3d4f3300,
    0x3d56af40, 0x3d5e3500, 0x3d65c380, 0x3d6d5ac0,
    0x3d74fb40, 0x3d7ca540, 0x3d845900, 0x3d8c1680,
    0x3d93dd00, 0x3d9bac80, 0x3da38400, 0x3dab6400,
    0x3db34c80, 0x3dbb3dc0, 0x3dc33840, 0x3dcb3bc0,
    0x3dd347c0, 0x3ddb5bc0, 0x3de37780, 0x3deb9b00,
    0x3df3c600, 0x3dfbf940, 0x3e0434c0, 0x3e0c7840,
    0x3e14c3c0, 0x3e1d1640, 0x3e256f80, 0x3e2dcf40,
    0x3e363580, 0x3e3ea300, 0x3e4717c0, 0x3e4f9380,
    0x3e581600, 0x3e609e40, 0x3e692c40, 0x3e71bf80,
    0x3e7a5840, 0x3e82f740, 0x3e8b9c40, 0x3e944700,
    0x3e9cf780, 0x3ea5ad00, 0x3eae66c0, 0x3eb72500,
    0x3ebfe780, 0x3ec8af00, 0x3ed17b80, 0x3eda4d00,
    0x3ee32340, 0x3eebfd40, 0x3ef4dac0, 0x3efdbbc0,
    0x3f06a040, 0x3f0f88c0, 0x3f187540, 0x3f216600,
    0x3f2a5a80, 0x3f335200, 0x3f3c4c40, 0x3f454940,
    0x3f4e4940, 0x3f574c80, 0x3f605340, 0x3f695dc0,
    0x3f726b40, 0x3f7b7b40, 0x3f848dc0, 0x3f8da240,
    0x3f96b940, 0x3f9fd300, 0x3fa8f040, 0x3fb21080,
    0x3fbb33c0, 0x3fc459c0, 0x3fcd81c0, 0x3fd6abc0,
    0x3fdfd780, 0x3fe90480, 0x3ff23280, 0x3ffb6100,
    0x40049f80, 0x400dd080, 0x40170400, 0x40203880,
    0x40296f00, 0x4032a600, 0x403bde00, 0x40451680,
    0x404e4f00, 0x40578700, 0x4060be80, 0x4069f500,
    0x40732b80, 0x407c6280, 0x40859980, 0x408ed100,
    0x40980800, 0x40a13f00, 0x40aa7500, 0x40b3a980,
    0x40bcdd80, 0x40c61180, 0x40cf4500, 0x40d87800,
    0x40e1ab00, 0x40eadc80, 0x40f40c80, 0x40fd3a80,
    0x41066700, 0x410f9300, 0x4118bd80, 0x4121e700,
    0x412b0f80, 0x41343580, 0x413d5880, 0x41467980,
    0x414f9780, 0x4158b380, 0x4161cd80, 0x416ae580,
    0x4173fb00, 0x417d0d00, 0x41861b80, 0x418f2600,
    0x41982c80, 0x41a12f80, 0x41aa3000, 0x41b32c80,
    0x41bc2580, 0x41c51a00, 0x41ce0900, 0x41d6f300,
    0x41dfd800, 0x41e8b880, 0x41f19400, 0x41fa6b80,
    0x42033d00, 0x420c0900, 0x4214cf00, 0x421d8e00,
    0x42264680, 0x422ef980, 0x4237a680, 0x42404d80,
    0x4248ee00, 0x42518780, 0x425a1a00, 0x4262a480,
    0x426b2800, 0x4273a400, 0x427c1980, 0x42848880,
    0x428cef80, 0x42954f00, 0x429da680, 0x42a5f500,
    0x42ae3b80, 0x42b67a00, 0x42beb100, 0x42c6e080,
    0x42cf0780, 0x42d72680, 0x42df3c00, 0x42e74880,
    0x42ef4c80, 0x42f74880, 0x42ff3c80, 0x43072880,
    0x430f0c80, 0x4316e800, 0x431eba00, 0x43268380,
    0x432e4480, 0x4335fd00, 0x433dae80, 0x43455800,
    0x434cfa00, 0x43549400, 0x435c2500, 0x4363ad80,
    0x436b2e00, 0x4372a700, 0x437a1800, 0x43818200,
    0x4388e400, 0x43903f00, 0x43979200, 0x439edd00,
    0x43a62080, 0x43ad5c80, 0x43b49180, 0x43bbbf80,
    0x43c2e800, 0x43ca0b00, 0x43d12980, 0x43d84280,
    0x43df5200, 0x43e65500, 0x43ed4800, 0x43f43080,
    0x43fb1c80, 0x44021b80, 0x44093a00, 0x44106480,
    0x44176700, 0x441e0c00, 0x44241e00, 0x44297380,
    0x4425dc00, 0x44240180, 0x441ff300, 0x4419e300,
    0x44123f80, 0x44097500, 0x43ffe900, 0x43f5e700,
    0x43eb9f00, 0x43e13f00, 0x43d6f200, 0x43ccbd80,
    0x43c28400, 0x43b82780, 0x43ad8b00, 0x43a29c80,
    0x43975180, 0x438ba080, 0x437f8180, 0x4372fd00,
    0x43662b00, 0x43592480, 0x434c0000, 0x433ecd00,
    0x43319180, 0x43245300, 0x43171700, 0x4309da80,
    0x42fc9300, 0x42ef3500, 0x42e1b600, 0x42d40280,
    0x42c60000, 0x42b79300, 0x42a8a180, 0x42991a00,
    0x4288f200, 0x42782100, 0x42669e00, 0x42546880,
    0x42418800, 0x422e0480, 0x4219e500, 0x42053680,
    0x41f00980, 0x41da7080, 0x41c47b00, 0x41ae3600,
    0x4197ab80, 0x4180e400, 0x4169e780, 0x4152bb00,
    0x413b5e80, 0x4123d180, 0x410c1480, 0x40f42100,
    0x40dbed00, 0x40c36c80, 0x40aa9600, 0x40915f80,
    0x4077c100, 0x405db280, 0x40432c80, 0x40282580,
    0x400c9280, 0x3ff068c0, 0x3fd39dc0, 0x3fb62bc0,
    0x3f981200, 0x3f795080, 0x3f59e780, 0x3f39ebc0,
    0x3f198680, 0x3ef8e100, 0x3ed82440, 0x3eb76c80,
    0x3e96c940, 0x3e764900, 0x3e55f980, 0x3e35cb00,
    0x3e1590c0, 0x3df51cc0, 0x3dd44200, 0x3db2e640,
    0x3d910200, 0x3d6e8e40, 0x3d4b8480, 0x3d27e600,
    0x3d03bc00, 0x3cdf0fc0, 0x3cb9eb80, 0x3c946240,
    0x3c6e9180, 0x3c489700, 0x3c229000, 0x3bfc95c0,
    0x3bd6bd00, 0x3bb11a80, 0x3b8bc180, 0x3b669bc0,
    0x3b416a00, 0x3b1beb80, 0x3af5e140, 0x3acf3300,
    0x3aa7ef80, 0x3a802780, 0x3a57eb80, 0x3a2f5880,
    0x3a069640, 0x39ddcd40, 0x39b524c0, 0x398ca540,
    0x39643800, 0x393bc540, 0x39133580, 0x38ea7ac0,
    0x38c19040, 0x389871c0, 0x386f1b40, 0x38458e00,
    0x381bd000, 0x37f1e780, 0x37c7db00, 0x379db080,
    0x37736e80, 0x37491b00, 0x371ebcc0, 0x36f45980,
    0x36c96600, 0x369ed300, 0x36740380, 0x3648ffc0,
    0x361dcf40, 0x35f27a00, 0x35c70780, 0x359b7f80,
    0x356fe9c0, 0x35444dc0, 0x3518b280, 0x34ed1940,
    0x34c17c00, 0x3495d4c0, 0x346a1d40, 0x343e4300,
    0x34122840, 0x33e5ae00, 0x33b8b780, 0x338b4dc0,
    0x335d9f00, 0x332fdc00, 0x33023440, 0x32d4cc40,
    0x32a7bc80, 0x327b1d40, 0x324f04c0, 0x32235280,
    0x31f7b100, 0x31cbc7c0, 0x319f4140, 0x3171fb40,
    0x31440840, 0x31157d00, 0x30e66e80, 0x30b6fc40,
    0x30875080, 0x30579600, 0x3027f700, 0x2ff89140,
    0x2fc976c0, 0x2f9ab880, 0x2f6c6780, 0x2f3e8780,
    0x2f111000, 0x2ee3f800, 0x2eb73480, 0x2e8a9840,
    0x2e5dd340, 0x2e3093c0, 0x2e028ac0, 0x2dd39680,
    0x2da3c480, 0x2d732380, 0x2d41c400, 0x2d0fd300,
    0x2cdd9ac0, 0x2cab6640, 0x2c797f00, 0x2c480d40,
    0x2c171700, 0x2be6a0c0, 0x2bb6ae80, 0x2b8739c0,
    0x2b583200, 0x2b298600, 0x2afb2400, 0x2accfa40,
    0x2a9ef500, 0x2a710100, 0x2a430ac0, 0x2a14f9c0,
    0x29e6b0c0, 0x29b81240, 0x29890140, 0x29596900,
    0x29293e00, 0x28f87500, 0x28c70340, 0x2894efc0,
    0x28625140, 0x282f4040, 0x27fbd5c0, 0x27c83540,
    0x27948ec0, 0x27611240, 0x272def80, 0x26fb4cc0,
    0x26c94780, 0x2697fcc0, 0x26678880, 0x2637f740,
    0x26094540, 0x25db6dc0, 0x25ae6b40, 0x25821680,
    0x255627c0, 0x252a55c0, 0x24fe5680, 0x24d1db40,
    0x24a48fc0, 0x24761f40, 0x244637c0, 0x2414c900,
    0x23e20240, 0x23ae1740, 0x23793bc0, 0x2343cc00,
    0x230e4ac0, 0x22d93c80, 0x22a52400, 0x22725180,
    0x2240e480, 0x2210f9c0, 0x21e2ab40, 0x21b5c7c0,
    0x2189d2c0, 0x215e4d40, 0x2132b900, 0x2106ba80,
    0x20da1940, 0x20ac9d80, 0x207e11c0, 0x204e77c0,
    0x201e0880, 0x1fecfea0, 0x1fbb94e0, 0x1f8a0500,
    0x1f59d340, 0x1f27ac20, 0x1ef67c60, 0x1ec64e40,
    0x1e96fdc0, 0x1e686400, 0x1e3a5a00, 0x1e0cae80,
    0x1ddf25e0, 0x1db18460, 0x1d839020, 0x1d5536e0,
    0x1d268e80, 0x1cf7ae60, 0x1cc8aea0, 0x1c99af00,
    0x1c6ad820, 0x1c3c5280, 0x1c0e4500, 0x1be0ab60,
    0x1bb35620, 0x1b861400, 0x1b58b480, 0x1b2b1a00,
    0x1afd39c0, 0x1acf09a0, 0x1aa080c0, 0x1a71b020,
    0x1a42c2a0, 0x1a13e420, 0x19e53fc0, 0x19b6eb00,
    0x1988e620, 0x195b3060, 0x192dc8a0, 0x1900a8a0,
    0x18d3c4e0, 0x18a711e0, 0x187a83e0, 0x184e10e0,
    0x1821b060, 0x17f55a00, 0x17c90580, 0x179cb100,
    0x177060a0, 0x17441880, 0x1717dd20, 0x16ebb080,
    0x16bf9260, 0x169382e0, 0x166781c0, 0x163b8f80,
    0x160fade0, 0x15e3de40, 0x15b82220, 0x158c7ae0,
    0x1560ea80, 0x15357240, 0x150a1400, 0x14ded020,
    0x14b3a640, 0x148895a0, 0x145d9dc0, 0x1432bde0,
    0x1407f540, 0x13dd4380, 0x13b2a860, 0x13882460,
    0x135db880, 0x133365a0, 0x13092cc0, 0x12df0e60,
    0x12b50aa0, 0x128b2120, 0x12615200, 0x12379da0,
    0x120e04c0, 0x11e48820, 0x11bb2860, 0x1191e600,
    0x1168c080, 0x113fb7a0, 0x1116cb40, 0x10edfba0,
    0x10c54a00, 0x109cb7a0, 0x10744560, 0x104bf420,
    0x1023c3e0, 0x0ffbb500, 0x0fd3c790, 0x0fabfbe0,
    0x0f845290, 0x0f5ccc40, 0x0f356970, 0x0f0e2a60,
    0x0ee70eb0, 0x0ec01610, 0x0e994040, 0x0e728d50,
    0x0e4bfdf0, 0x0e2592c0, 0x0dff4c70, 0x0dd92af0,
    0x0db32da0, 0x0d8d53e0, 0x0d679cf0, 0x0d420880,
    0x0d1c9680, 0x0cf74700, 0x0cd219f0, 0x0cad0eb0,
    0x0c882450, 0x0c6359a0, 0x0c3ead90, 0x0c1a1f80,
    0x0bf5af40, 0x0bd15cf0, 0x0bad2870, 0x0b891440,
    0x0b652530, 0x0b416020, 0x0b1dca30, 0x0afa6810,
    0x0ad73ee0, 0x0ab45370, 0x0a91aac0, 0x0a6f49b0,
    0x0a4da7f0, 0x0a2c7e20, 0x0a0ba310, 0x09eb1220,
    0x09cac6e0, 0x09aabc70, 0x098aee40, 0x096b57a0,
    0x094bf400, 0x092cbea0, 0x090db2e0, 0x08eecef0,
    0x08d01360, 0x08b18110, 0x089318b0, 0x0874db00,
    0x0856c880, 0x0838e1b0, 0x081b2730, 0x07fd99a8,
    0x07e03a28, 0x07c309a8, 0x07a60910, 0x07893918,
    0x076c99d0, 0x07502b90, 0x0733ee70, 0x0717e2f8,
    0x06fc09b8, 0x06e06378, 0x06c4f0b8, 0x06a9b1c8,
    0x068ea6a0, 0x0673cf18, 0x06592b18, 0x063ebad0,
    0x06247ed0, 0x060a7780, 0x05f0a570, 0x05d708b8,
    0x05bda128, 0x05a46e80, 0x058b7078, 0x0572a740,
    0x055a1330, 0x0541b4d8, 0x05298c98, 0x05119a88,
    0x04f9de50, 0x04e257a0, 0x04cb0630, 0x04b3ea00,
    0x049d0378, 0x04865308, 0x046fd918, 0x045995a8,
    0x04438860, 0x042db0d0, 0x04180ea0, 0x0402a1d0,
    0x03ed6abc, 0x03d869b8, 0x03c39f28, 0x03af0af0,
    0x039aaca0, 0x038683b4, 0x03728fc0, 0x035ed0b0,
    0x034b46c4, 0x0337f254, 0x0324d3a0, 0x0311eab0,
    0x02ff370c, 0x02ecb85c, 0x02da6e34, 0x02c858a8,
    0x02b67820, 0x02a4cd28, 0x02935820, 0x02821920,
    0x02710fac, 0x02603b54, 0x024f9bb4, 0x023f308c,
    0x022ef9e8, 0x021ef7c8, 0x020f2a40, 0x01ff908e,
    0x01f02974, 0x01e0f38a, 0x01d1ed94, 0x01c316d6,
    0x01b46f5e, 0x01a5f720, 0x0197ae28, 0x018994ea,
    0x017bac54, 0x016df546, 0x016070ae, 0x01532078,
    0x01460760, 0x01392834, 0x012c85a4, 0x01201f7a,
    0x0113f27c, 0x0107fb6c, 0x00fc36fd, 0x00f0a2d5,
    0x00e53d51, 0x00da050f, 0x00cef88c, 0x00c41869,
    0x00b9671f, 0x00aee754, 0x00a49b80, 0x009a8384,
    0x00909ca6, 0x0086e400, 0x007d56e3, 0x0073f48e,
    0x006abe70, 0x0061b5de, 0x0058dc65, 0x005033b4,
    0x0047be30, 0x003f7e30, 0x00377619, 0x002fa4d4,
    0x002805ee, 0x002094cb, 0x00194cb8, 0x00122856,
    0x000b215c, 0x00043148, 0xfffd51f0, 0xfff683a0,
    0xffefcd4d, 0xffe9362f, 0xffe2c57d, 0xffdc855c,
    0xffd682c4, 0xffd0cad4, 0xffcb6a2c, 0xffc663bc,
    0xffc1b06f, 0xffbd48e1, 0xffb92570, 0xffb53a54,
    0xffb1779c, 0xffadcd38, 0xffaa2b42, 0xffa68855,
    0xffa2e141, 0xff9f332c, 0xff9b7b9c, 0xff97bf2e,
    0xff9409e2, 0xff9067e2, 0xff8ce556, 0xff898bf0,
    0xff866306, 0xff8371d0, 0xff80bf63, 0xff7e4eba,
    0xff7c1eaa, 0xff7a2e04, 0xff787b47, 0xff770280,
    0xff75bd06, 0xff74a3f7, 0xff73b0b2, 0xff72dd02,
    0xff72237e, 0xff717ebe, 0xff70e94c, 0xff705f59,
    0xff6fde6a, 0xff6f6426, 0xff6eee40, 0xff6e7d0b,
    0xff6e1359, 0xff6db403, 0xff6d61f8, 0xff6d2054,
    0xff6cf267, 0xff6cdb76, 0xff6cdebb, 0xff6cff47,
    0xff6d3fc9, 0xff6da306, 0xff6e2b82, 0xff6eda13,
    0xff6fad6d, 0xff70a463, 0xff71bd9d, 0xff72f662,
    0xff744a80, 0xff75b5c4, 0xff773409, 0xff78c0a6,
    0xff7a5693, 0xff7bf0dc, 0xff7d8abb, 0xff7f2301,
    0xff80bc08, 0xff825854, 0xff83fa56, 0xff85a55c,
    0xff875d22, 0xff892598, 0xff8b025d, 0xff8cf53c,
    0xff8efdf4, 0xff911c48, 0xff934fc9, 0xff959675,
    0xff97ec86, 0xff9a4e35, 0xff9cb7d2, 0xff9f26cc,
    0xffa199ce, 0xffa40f74, 0xffa6867c, 0xffa8feb2,
    0xffab78e0, 0xffadf5c7, 0xffb07640, 0xffb2fba0,
    0xffb587a2, 0xffb81bfb, 0xffbaba46, 0xffbd6236,
    0xffc011a8, 0xffc2c679, 0xffc57e84, 0xffc83894,
    0xffcaf41a, 0xffcdb0b8, 0xffd06e17, 0xffd32bf7,
    0xffd5ea38, 0xffd8a8c3, 0xffdb6764, 0xffde25fb,
    0xffe0e471, 0xffe3a2b2, 0xffe66087, 0xffe91da6,
    0xffebd978, 0xffee9351, 0xfff14ab0, 0xfff3fef6,
    0xfff6af94, 0xfff95c0c, 0xfffc03c7, 0xfffea659,
    0x00015885, 0x0003f2e9, 0x00068a73, 0x00091e8d,
    0x000bae7f, 0x000e39bf, 0x0010bf96, 0x00133f78,
    0x0015b8c4, 0x00182ae4, 0x001a9558, 0x001cf7b2,
    0x001f51e0, 0x0021a3b4, 0x0023ed25, 0x00262df2,
    0x002865c5, 0x002a9469, 0x002cb967, 0x002ed4aa,
    0x0030e607, 0x0032ed88, 0x0034eb2f, 0x0036de23,
    0x0038c503, 0x003a9e4c, 0x003c68a6, 0x003e23dd,
    0x003fd0db, 0x00417083, 0x0043038b, 0x00448adf,
    0x00460740, 0x0047799c, 0x0048e2b2, 0x004a42af,
    0x004b98fb, 0x004ce50b, 0x004e2654, 0x004f5b5d,
    0x005081c3, 0x00519716, 0x00529920, 0x005386d0,
    0x0054603f, 0x00552581, 0x0055d6cc, 0x00567558,
    0x0057033c, 0x005782b4, 0x0057f5b6, 0x00585e46,
    0x0058be68, 0x005917ff, 0x00596ce4, 0x0059bcc0,
    0x005a053a, 0x005a43ee, 0x005a76ae, 0x005a9b37,
    0x005aaf38, 0x005ab07a, 0x005a9cef, 0x005a7349,
    0x005a3328, 0x0059dc0a, 0x00596db0, 0x0058e8e5,
    0x00584f98, 0x0057a3c0, 0x0056e738, 0x00561bec,
    0x005543df, 0x0054610b, 0x0053753e, 0x0052824e,
    0x005189f6, 0x00508dec, 0x004f8fc0, 0x004e8fd0,
    0x004d8d26, 0x004c86d7, 0x004b7c0a, 0x004a6b33,
    0x00495239, 0x00482f0e, 0x0046ffc4, 0x0045c201,
    0x00447337, 0x004310cc, 0x00419871, 0x004008e4,
    0x003e6231, 0x003ca460, 0x003acf8a, 0x0038e57a,
    0x0036e981, 0x0034defa, 0x0032c94b, 0x0030acc6,
    0x002e8eb4, 0x002c7452, 0x002a62aa, 0x00285bbf,
    0x00265eda, 0x00246b24, 0x00227f9c, 0x002098e7,
    0x001eb13b, 0x001cc2ef, 0x001ac899, 0x0018be3d,
    0x0016a198, 0x00147065, 0x00122897, 0x000fcbc5,
    0x000d5f03, 0x000ae77a, 0x00086a52, 0x0005eb92,
    0x00036e4a, 0x0000f57e, 0xfffe8414, 0xfffc1a78,
    0xfff9b6bb, 0xfff756d9, 0xfff4f8d0, 0xfff29add,
    0xfff03b87, 0xffedd94c, 0xffeb7295, 0xffe9072b,
    0xffe6981a, 0xffe4265b, 0xffe1b30e, 0xffdf3f2b,
    0xffdccb9e, 0xffda5993, 0xffd7ea0c, 0xffd57d60,
    0xffd31302, 0xffd0aa27, 0xffce4243, 0xffcbdb40,
    0xffc97595, 0xffc711a2, 0xffc4af9d, 0xffc24fa6,
    0xffbff1de, 0xffbd9699, 0xffbb3e44, 0xffb8e8d5,
    0xffb695f4, 0xffb44522, 0xffb1f627, 0xffafa8f0,
    0xffad5d91, 0xffab140a, 0xffa8cc1c, 0xffa68590,
    0xffa44066, 0xffa1fca0, 0xff9fba30, 0xff9d7902,
    0xff9b3916, 0xff98fa6d, 0xff96bd06, 0xff9480b6,
    0xff924532, 0xff900a24, 0xff8dcf41, 0xff8b9433,
    0xff895884, 0xff871bd3, 0xff84dd8a, 0xff829d34,
    0xff805a43, 0xff7e142d, 0xff7bca71, 0xff797c83,
    0xff7729e3, 0xff74d204, 0xff727451, 0xff70101e,
    0xff6da493, 0xff6b30d1, 0xff68b3f4, 0xff662d31,
    0xff639bd1, 0xff60ff09, 0xff5e562c, 0xff5ba3e0,
    0xff58ee39, 0xff563c22, 0xff5394f3, 0xff50fd1e,
    0xff4e7599, 0xff4bff32, 0xff499ad4, 0xff47490a,
    0xff450a36, 0xff42deb7, 0xff40c6cf, 0xff3ec2be,
    0xff3cd299, 0xff3af681, 0xff392e6a, 0xff377a4a,
    0xff35d9f7, 0xff344d44, 0xff32d3e8, 0xff316d96,
    0xff3019d9, 0xff2ed83a, 0xff2da82f, 0xff2c88bf,
    0xff2b78b4, 0xff2a76cc, 0xff298184, 0xff289890,
    0xff27bc7d, 0xff26ee21, 0xff262e28, 0xff257cdc,
    0xff24d9f4, 0xff244524, 0xff23be15, 0xff234488,
    0xff22d852, 0xff227947, 0xff22273d, 0xff21e1d2,
    0xff21a871, 0xff217a79, 0xff215748, 0xff213eca,
    0xff21319e, 0xff21305c, 0xff213baf, 0xff2153c2,
    0xff21782b, 0xff21a892, 0xff21e477, 0xff222bda,
    0xff227f26, 0xff22debd, 0xff234b09, 0xff23c394,
    0xff24471d, 0xff24d42b, 0xff25695c, 0xff260538,
    0xff26a652, 0xff274b28, 0xff27f22d, 0xff2899d2,
    0xff295975, 0xff29f2ad, 0xff2a96d7, 0xff2b45f4,
    0xff2bffe3, 0xff2cc4ba, 0xff2d9458, 0xff2e6ede,
    0xff2f544c, 0xff3044b7, 0xff314034, 0xff3246fa,
    0xff33591e, 0xff3476e0, 0xff35a060, 0xff36d534,
    0xff38148f, 0xff395daf, 0xff3aafd4, 0xff3c0ac8,
    0xff3d6ed6, 0xff3edc54, 0xff405382, 0xff41d3f5,
    0xff435ccc, 0xff44ed0f, 0xff4683d3, 0xff482080,
    0xff49c297, 0xff4b69ab, 0xff4d1547, 0xff4ec4f5,
    0xff50781d, 0xff522e20, 0xff53e692, 0xff55a15d,
    0xff575f17, 0xff592022, 0xff5ae4de, 0xff5cacb4,
    0xff5e75e2, 0xff603ee5, 0xff62062f, 0xff63caab,
    0xff658b55, 0xff67476d, 0xff68fe11, 0xff6aaea0,
    0xff6c5899, 0xff6dfb86, 0xff6f96e7, 0xff712a65,
    0xff72b59f, 0xff74382b, 0xff75b1d3, 0xff772276,
    0xff788a20, 0xff79e8e5, 0xff7b3ef0, 0xff7c8c98,
    0xff7dd249, 0xff7f108c, 0xff804804, 0xff817d0e,
    0xff82b74a, 0xff83fde6, 0xff855762, 0xff86c622,
    0xff884904, 0xff89ded1, 0xff8b8646, 0xff8d3e4c,
    0xff8f05cc, 0xff90dbc6, 0xff92bf2a, 0xff94af04,
    0xff96aa26, 0xff98af9a, 0xff9abe48, 0xff9cd543,
    0xff9ef3c1, 0xffa118ea, 0xffa343fd, 0xffa57423,
    0xffa7a890, 0xffa9e084, 0xffac1b31, 0xffae5802,
    0xffb09680, 0xffb2d621, 0xffb51678, 0xffb75704,
    0xffb99726, 0xffbbd645, 0xffbe13d7, 0xffc04f26,
    0xffc2879a, 0xffc4bc72, 0xffc6ed24, 0xffc918e3,
    0xffcb3eb8, 0xffcd5dcc, 0xffcf7549, 0xffd184d8,
    0xffd38c8f, 0xffd58ca4, 0xffd7854d, 0xffd97694,
    0xffdb606e, 0xffdd42d1, 0xffdf1da8, 0xffe0f09b,
    0xffe2bb00, 0xffe47c41, 0xffe633c6, 0xffe7e150,
    0xffe98534, 0xffeb1fb4, 0xffecb10e, 0xffee3944,
    0xffefb7e9, 0xfff12cbe, 0xfff29762, 0xfff3f789,
    0xfff54cbe, 0xfff69695, 0xfff7d4b8, 0xfff90748,
    0xfffa2ee5, 0xfffb4c3c, 0xfffc6003, 0xfffd6af0,
    0xfffe6dda, 0xffff69b8, 0x00005f4b, 0x00014e7f,
    0x00023646, 0x000315b4, 0x0003ebd3, 0x0004b74a,
    0x00057677, 0x000627e2, 0x0006ca09, 0x00075ce1,
    0x0007e196, 0x00085955, 0x0008c556, 0x00092751,
    0x00098153, 0x0009d581, 0x000a25be, 0x000a732b,
    0x000abe1f, 0x000b06e4, 0x000b4db1, 0x000b91fa,
    0x000bd266, 0x000c0da0, 0x000c426e, 0x000c6ffb,
    0x000c95b0, 0x000cb2f7, 0x000cc76e, 0x000cd317,
    0x000cd647, 0x000cd17f, 0x000cc52b, 0x000cb1ea,
    0x000c98c0, 0x000c7a62, 0x000c57c7, 0x000c3187,
    0x000c0862, 0x000bdcd8, 0x000baf81, 0x000b80c7,
    0x000b50ec, 0x000b202f, 0x000aeec6, 0x000abcb2,
    0x000a89d2, 0x000a5605, 0x000a2116, 0x0009eafb,
    0x0009b37d, 0x00097a9d, 0x00094030, 0x00090440,
    0x0008c6b9, 0x000887ae, 0x0008470c, 0x00080512,
    0x0007c1f6, 0x00077df9, 0x0007395a, 0x0006f45b,
    0x0006af67, 0x00066abe, 0x000626b6, 0x0005e38f,
    0x0005a1a0, 0x0005611e, 0x00052234, 0x0004e502,
    0x0004a95d, 0x00046f46, 0x00043691, 0x0003ff33,
    0x0003c90d, 0x0003941f, 0x00036047, 0x00032d9c,
    0x0002fc1e, 0x0002cbed, 0x00029d1e, 0x00026fbc,
    0x000243f2, 0x000219d6, 0x0001f17d, 0x0001caf1,
    0x0001a63e, 0x00018363, 0x00016256, 0x00014316,
    0x0001258f, 0x000109cb, 0x0000efaa, 0x0000d720,
    0x0000c03a, 0x0000aacb, 0x000096de, 0x0000846a,
    0x0000736d, 0x000063d3, 0x000055a6, 0x000048d0,
    0x00003d47, 0x000032f6, 0x000029dc, 0x000021d9,
    0x00001ae3, 0x000014ee, 0x00000fdb, 0x00000ba9,
    0x00000839, 0x00000589, 0x00000370, 0x000001ee,
    0x000000d7, 0x00000036, 0xffffffe0, 0xffffffc0,
    0xffffffd5, 0xfffffff5, 0x0000000b, 0x0000000b,
    0x0000000b, 0x0000000b, 0xfffffff5, 0xffffffd5,
    0xffffffca, 0xffffffe0, 0x00000036, 0x000000d7,
    0x000001ce, 0x0000033b, 0x00000529, 0x000007ad,
    0x00000ac8, 0x00000e99, 0x00001316, 0x0000185e,
    0x00001e7e, 0x00002575, 0x00002d4c, 0x0000361b,
    0x00003fd6, 0x00004a93, 0x00005647, 0x00006312,
    0x000070de, 0x00007fad, 0x00008f87, 0x0000a064,
    0x0000b242, 0x0000c52d, 0x0000d919, 0x0000ee12,
    0x0001040c, 0x00011b13, 0x0001331b, 0x00014c30,
    0x0001663c, 0x0001814a, 0x00019d4f, 0x0001ba35,
    0x0001d7e7, 0x0001f645, 0x00021544, 0x000234c3,
    0x000254b9, 0x00027505, 0x000295a7, 0x0002b67e,
    0x0002d7a1, 0x0002f904, 0x00031ab2, 0x00033ca0,
    0x00035ee5, 0x0003818a, 0x0003a485, 0x0003c7e1,
    0x0003eb72, 0x00040f0e, 0x0004329f, 0x000455e6,
    0x000478c0, 0x00049aef, 0x0004bc52, 0x0004dca9,
    0x0004fbde, 0x000519c5, 0x00053635, 0x0005512d,
    0x00056aae, 0x000582a1, 0x00059927, 0x0005ae40,
    0x0005c1f6, 0x0005d455, 0x0005e572, 0x0005f56d,
    0x00060446, 0x0006121e, 0x00061f09, 0x00062b08,
    0x00063605, 0x00063feb, 0x00064899, 0x00064ff0,
    0x000655a5, 0x00065996, 0x00065b6f, 0x00065af8,
    0x000657e9, 0x000651d4, 0x00064884, 0x00063bae,
    0x00062b33, 0x00061706, 0x0005fefd, 0x0005e344,
    0x0005c404, 0x0005a195, 0x00057c41, 0x00055473,
    0x00052ac2, 0x0004ffc4, 0x0004d410, 0x0004a7e5,
    0x00047b4f, 0x00044e39, 0x00042096, 0x0003f208,
    0x0003c1e1, 0x00038f77, 0x00035a12, 0x00032127,
    0x0002e476, 0x0002a389, 0x00025e29, 0x0002146d,
    0x0001c700, 0x00017682, 0x000123a1, 0x0000cefd,
    0x000078f7, 0x0000221a, 0xffffcad1, 0xffff7332,
    0xffff1b1e, 0xfffec253, 0xfffe6891, 0xfffe0da2,
    0xfffdb15c, 0xfffd5393, 0xfffcf412, 0xfffc92e3,
    0xfffc3032, 0xfffbcc29, 0xfffb6714, 0xfffb0113,
    0xfffa9a5b, 0xfffa3337, 0xfff9cbd4, 0xfff96450,
    0xfff8fcac, 0xfff894dc, 0xfff82cd8, 0xfff7c4a8,
    0xfff75c6d, 0xfff6f45e, 0xfff68c84, 0xfff62500,
    0xfff5bde8, 0xfff5575a, 0xfff4f179, 0xfff48c64,
    0xfff42810, 0xfff3c488, 0xfff361d7, 0xfff30008,
    0xfff29f3a, 0xfff23f78, 0xfff1e0d8, 0xfff1835b,
    0xfff1272a, 0xfff0cc46, 0xfff072cf, 0xfff01ad0,
    0xffefc469, 0xffef6fa4, 0xffef1ca3, 0xffeecb7a,
    0xffee7c1f, 0xffee2eb2, 0xffede33d, 0xffed99c1,
    0xffed5249, 0xffed0cde, 0xffecc98d, 0xffec8849,
    0xffec4934, 0xffec0c38, 0xffebd175, 0xffeb98eb,
    0xffeb62a4, 0xffeb2ead, 0xffeafd19, 0xffeacdea,
    0xffeaa129, 0xffea76cc, 0xffea4ef4, 0xffea299f,
    0xffea06e5, 0xffe9e6ce, 0xffe9c97d, 0xffe9aebb,
    0xffe99651, 0xffe97fd6, 0xffe96ad3, 0xffe95711,
    0xffe9447d, 0xffe93315, 0xffe922ce, 0xffe913a0,
    0xffe90588, 0xffe8f887, 0xffe8ec93, 0xffe8e1c1,
    0xffe8d806, 0xffe8cf77, 0xffe8c816, 0xffe8c1eb,
    0xffe8bd03, 0xffe8b967, 0xffe8b72e, 0xffe8b64d,
    0xffe8b6d8, 0xffe8b8dc, 0xffe8bc6c, 0xffe8c18a,
    0xffe8c840, 0xffe8d0a4, 0xffe8daca, 0xffe8e69e,
    0xffe8f42a, 0xffe9035a, 0xffe9142b, 0xffe926a0,
    0xffe93ab7, 0xffe95066, 0xffe967b8, 0xffe980ad,
    0xffe99b3a, 0xffe9b754, 0xffe9d511, 0xffe9f45b,
    0xffea1532, 0xffea3797, 0xffea5b89, 0xffea8108,
    0xffeaa7ff, 0xffead079, 0xffeafa55, 0xffeb259e,
    0xffeb5254, 0xffeb8061, 0xffebafdc, 0xffebe0ae,
    0xffec12ce, 0xffec462f, 0xffec7add, 0xffecb0a3,
    0xffece774, 0xffed1f32, 0xffed57a7, 0xffed90b2,
    0xffedca48, 0xffee042a, 0xffee3e57, 0xffee788e,
};

const float __attribute__ ((aligned (32))) ff_aac_eld_window_480[1800] = {
     0.00101191, 0.00440397, 0.00718669, 0.01072130,
     0.01459757, 0.01875954, 0.02308987, 0.02751541,
     0.03198130, 0.03643738, 0.04085290, 0.04522835,
     0.04957620, 0.05390454, 0.05821503, 0.06251214,
     0.06680463, 0.07109582, 0.07538014, 0.07965207,
     0.08390857, 0.08815177, 0.09238785, 0.09662163,
     0.10085860, 0.10510892, 0.10938110, 0.11367819,
     0.11800355, 0.12236410, 0.12676834, 0.13122384,
     0.13573476, 0.14030106, 0.14492340, 0.14960315,
     0.15433828, 0.15912396, 0.16395663, 0.16883310,
     0.17374837, 0.17869679, 0.18367394, 0.18867661,
     0.19370368, 0.19875413, 0.20382641, 0.20892055,
     0.21403775, 0.21917761, 0.22433899, 0.22952250,
     0.23472991, 0.23996189, 0.24521859, 0.25049930,
     0.25580312, 0.26112942, 0.26647748, 0.27184703,
     0.27723785, 0.28264967, 0.28808086, 0.29352832,
     0.29898979, 0.30446379, 0.30994292, 0.31541664,
     0.32087942, 0.32632772, 0.33176291, 0.33718641,
     0.34259612, 0.34799346, 0.35338857, 0.35878843,
     0.36419504, 0.36960630, 0.37501567, 0.38042067,
     0.38582069, 0.39121276, 0.39659312, 0.40195993,
     0.40731155, 0.41264382, 0.41795277, 0.42323670,
     0.42849480, 0.43372753, 0.43893452, 0.44411398,
     0.44927117, 0.45441882, 0.45956191, 0.46470167,
     0.46983016, 0.47493636, 0.48001827, 0.48507480,
     0.49010240, 0.49509781, 0.50005986, 0.50499037,
     0.50989790, 0.51478708, 0.51965805, 0.52450975,
     0.52933955, 0.53414668, 0.53893113, 0.54369178,
     0.54842731, 0.55313757, 0.55782259, 0.56248253,
     0.56711762, 0.57172819, 0.57631468, 0.58087761,
     0.58719976, 0.59173064, 0.59623644, 0.60071719,
     0.60517294, 0.60960372, 0.61400958, 0.61839056,
     0.62274670, 0.62707805, 0.63138475, 0.63566700,
     0.63992500, 0.64415895, 0.64836893, 0.65255499,
     0.65671715, 0.66085548, 0.66497005, 0.66906094,
     0.67312824, 0.67717199, 0.68119219, 0.68518882,
     0.68916187, 0.69311129, 0.69703698, 0.70093884,
     0.70481679, 0.70867071, 0.71250047, 0.71630596,
     0.72008705, 0.72384360, 0.72757549, 0.73128256,
     0.73496463, 0.73862141, 0.74225263, 0.74585799,
     0.74943730, 0.75299039, 0.75651711, 0.76001729,
     0.76349062, 0.76693670, 0.77035516, 0.77374564,
     0.77710790, 0.78044169, 0.78374678, 0.78702291,
     0.79026979, 0.79348715, 0.79667471, 0.79983215,
     0.80295914, 0.80605536, 0.80912047, 0.81215417,
     0.81515616, 0.81812616, 0.82106389, 0.82396915,
     0.82684176, 0.82968154, 0.83248830, 0.83526186,
     0.83800204, 0.84070866, 0.84338156, 0.84602058,
     0.84862556, 0.85119636, 0.85373292, 0.85623523,
     0.85870326, 0.86113701, 0.86353649, 0.86590173,
     0.86823275, 0.87052968, 0.87279275, 0.87502220,
     0.87721829, 0.87938130, 0.88151157, 0.88360940,
     0.88567517, 0.88770954, 0.88971328, 0.89168716,
     0.89363199, 0.89554856, 0.89743771, 0.89930025,
     0.90113740, 0.90295086, 0.90474240, 0.90651380,
     0.90826684, 0.91000335, 0.91172515, 0.91343416,
     0.91513276, 0.91682357, 0.91850924, 0.92019170,
     0.92187129, 0.92354778, 0.92522116, 0.92688597,
     0.92852960, 0.93013861, 0.93169897, 0.93319114,
     0.93458502, 0.93587626, 0.93694276, 0.93825562,
     0.93882222, 0.93910780, 0.93944183, 0.93981497,
     0.94021434, 0.94062629, 0.94103714, 0.94144084,
     0.94184042, 0.94223966, 0.94264206, 0.94304859,
     0.94345831, 0.94387033, 0.94428390, 0.94469895,
     0.94511572, 0.94553441, 0.94595520, 0.94637816,
     0.94680335, 0.94723080, 0.94766054, 0.94809253,
     0.94852674, 0.94896314, 0.94940178, 0.94984276,
     0.95028618, 0.95073213, 0.95118056, 0.95163139,
     0.95208451, 0.95253992, 0.95299770, 0.95345799,
     0.95392092, 0.95438653, 0.95485472, 0.95532539,
     0.95579847, 0.95627397, 0.95675201, 0.95723273,
     0.95771618, 0.95820232, 0.95869103, 0.95918218,
     0.95967573, 0.96017172, 0.96067026, 0.96117144,
     0.96167526, 0.96218157, 0.96269026, 0.96320119,
     0.96371437, 0.96422988, 0.96474782, 0.96526824,
     0.96579106, 0.96631614, 0.96684334, 0.96737257,
     0.96790390, 0.96843740, 0.96897315, 0.96951112,
     0.97005119, 0.97059318, 0.97113697, 0.97168253,
     0.97222994, 0.97277928, 0.97333058, 0.97388375,
     0.97443863, 0.97499505, 0.97555292, 0.97611230,
     0.97667326, 0.97723589, 0.97780016, 0.97836591,
     0.97893300, 0.97950127, 0.98007071, 0.98064139,
     0.98121342, 0.98178684, 0.98236156, 0.98293743,
     0.98351428, 0.98409205, 0.98467078, 0.98525056,
     0.98583146, 0.98641348, 0.98699650, 0.98758037,
     0.98816497, 0.98875030, 0.98933647, 0.98992356,
     0.99051163, 0.99110062, 0.99169038, 0.99228079,
     0.99287177, 0.99346341, 0.99405581, 0.99464907,
     0.99524320, 0.99583812, 0.99643375, 0.99702997,
     0.99762671, 0.99822386, 0.99882134, 0.99941903,
     1.00058131, 1.00118006, 1.00177930, 1.00237893,
     1.00297887, 1.00357902, 1.00417927, 1.00477954,
     1.00537972, 1.00597973, 1.00657959, 1.00717940,
     1.00777926, 1.00837925, 1.00897929, 1.00957926,
     1.01017901, 1.01077847, 1.01137769, 1.01197678,
     1.01257582, 1.01317482, 1.01377365, 1.01437217,
     1.01497025, 1.01556786, 1.01616510, 1.01676205,
     1.01735876, 1.01795514, 1.01855103, 1.01914627,
     1.01974076, 1.02033455, 1.02092772, 1.02152037,
     1.02211247, 1.02270387, 1.02329439, 1.02388387,
     1.02447229, 1.02505972, 1.02564624, 1.02623190,
     1.02681660, 1.02740017, 1.02798242, 1.02856326,
     1.02914272, 1.02972087, 1.03029778, 1.03087344,
     1.03144768, 1.03202035, 1.03259127, 1.03316042,
     1.03372788, 1.03429373, 1.03485801, 1.03542064,
     1.03598146, 1.03654030, 1.03709708, 1.03765185,
     1.03820470, 1.03875571, 1.03930488, 1.03985206,
     1.04039712, 1.04093989, 1.04148037, 1.04201865,
     1.04255481, 1.04308893, 1.04362093, 1.04415068,
     1.04467803, 1.04520292, 1.04572542, 1.04624566,
     1.04676376, 1.04727974, 1.04779350, 1.04830493,
     1.04881391, 1.04932048, 1.04982477, 1.05032693,
     1.05082705, 1.05132510, 1.05182098, 1.05231457,
     1.05280584, 1.05329485, 1.05378171, 1.05426654,
     1.05474937, 1.05523018, 1.05570892, 1.05618554,
     1.05666005, 1.05713251, 1.05760297, 1.05807149,
     1.05853828, 1.05900355, 1.05946756, 1.05993024,
     1.06039075, 1.06084806, 1.06130111, 1.06175099,
     1.06220164, 1.06265732, 1.06312146, 1.06358726,
     1.06403924, 1.06446186, 1.06484048, 1.06516440,
     1.06527864, 1.06498077, 1.06470196, 1.06425743,
     1.06372091, 1.06311464, 1.06246622, 1.06179277,
     1.06110808, 1.06042455, 1.05974495, 1.05906206,
     1.05836706, 1.05765243, 1.05691470, 1.05615178,
     1.05536069, 1.05454152, 1.05370030, 1.05284445,
     1.05198094, 1.05111433, 1.05024634, 1.04937859,
     1.04851245, 1.04764614, 1.04677586, 1.04589855,
     1.04501046, 1.04410500, 1.04317417, 1.04221010,
     1.04120649, 1.04016012, 1.03906851, 1.03792894,
     1.03674090, 1.03550649, 1.03422800, 1.03290769,
     1.03154944, 1.03015834, 1.02873938, 1.02729712,
     1.02583470, 1.02435463, 1.02285952, 1.02135114,
     1.01982974, 1.01829520, 1.01674752, 1.01518534,
     1.01360559, 1.01200510, 1.01038076, 1.00872996,
     1.00705045, 1.00533999, 1.00359618, 1.00181613,
     0.99999673, 0.99813477, 0.99622793, 0.99427571,
     0.99227814, 0.99023501, 0.98815128, 0.98603857,
     0.98390898, 0.98177413, 0.97964151, 0.97751528,
     0.97539999, 0.97329751, 0.97119933, 0.96909179,
     0.96696152, 0.96479824, 0.96259840, 0.96036028,
     0.95808180, 0.95576295, 0.95340622, 0.95101436,
     0.94859030, 0.94614009, 0.94367232, 0.94119555,
     0.93871796, 0.93624630, 0.93378636, 0.93134465,
     0.92892076, 0.92649974, 0.92406255, 0.92159041,
     0.91907411, 0.91651711, 0.91392425, 0.91130056,
     0.90865471, 0.90599838, 0.90334350, 0.90069934,
     0.89806435, 0.89543132, 0.89279335, 0.89014496,
     0.88748403, 0.88480945, 0.88211997, 0.87941558,
     0.87669794, 0.87396891, 0.87123030, 0.86848394,
     0.86573164, 0.86297523, 0.86021649, 0.85745725,
     0.85474342, 0.85193656, 0.84911455, 0.84627969,
     0.84343424, 0.84058046, 0.83772057, 0.83485680,
     0.83199134, 0.82912621, 0.82626143, 0.82339529,
     0.82052619, 0.81765147, 0.81476433, 0.81185593,
     0.80891701, 0.80594452, 0.80294885, 0.79994431,
     0.79694485, 0.79396166, 0.79100220, 0.78807349,
     0.78518123, 0.78231422, 0.77944709, 0.77655407,
     0.77361369, 0.77062281, 0.76758806, 0.76451506,
     0.76141145, 0.75828860, 0.75515892, 0.75203479,
     0.74892561, 0.74583682, 0.74277342, 0.73974008,
     0.73673754, 0.73376310, 0.73081444, 0.72788616,
     0.72496070, 0.72201426, 0.71902283, 0.71596990,
     0.71285541, 0.70968427, 0.70646064, 0.70319589,
     0.69991077, 0.69662714, 0.69336592, 0.69013742,
     0.68694302, 0.68378420, 0.68066143, 0.67757157,
     0.67450951, 0.67147030, 0.66844879, 0.66543949,
     0.66243677, 0.65943505, 0.65642754, 0.65340591,
     0.65036160, 0.64728630, 0.64417440, 0.64102268,
     0.63782771, 0.63458757, 0.63130628, 0.62799109,
     0.62464879, 0.62128816, 0.61792203, 0.61456438,
     0.61122915, 0.60792802, 0.60466971, 0.60146257,
     0.59831460, 0.59522876, 0.59220375, 0.58923859,
     0.58632936, 0.58346064, 0.58061078, 0.57775874,
     0.57488246, 0.57195790, 0.56896078, 0.56586637,
     0.56266594, 0.55937186, 0.55599898, 0.55256299,
     0.54909184, 0.54562376, 0.54219742, 0.53884728,
     0.53559047, 0.53243453, 0.52938894, 0.52645052,
     0.52358958, 0.52076862, 0.51795080, 0.51510761,
     0.51222179, 0.50927733, 0.50625944, 0.50317073,
     0.50002767, 0.49685021, 0.49364116, 0.49048690,
     0.48726128, 0.48404889, 0.48090875, 0.47783482,
     0.47481564, 0.47184024, 0.46889391, 0.46595836,
     0.46301611, 0.46005089, 0.45705924, 0.45404822,
     0.45102447, 0.44799543, 0.44497138, 0.44196397,
     0.43898547, 0.43604105, 0.43312057, 0.43020942,
     0.42729337, 0.42436272, 0.42141388, 0.41844400,
     0.41545081, 0.41244014, 0.40942464, 0.40641716,
     0.40342874, 0.40046292, 0.39751923, 0.39459758,
     0.39169692, 0.38881435, 0.38594643, 0.38308980,
     0.38024146, 0.37739896, 0.37455986, 0.37172187,
     0.36888463, 0.36604937, 0.36321735, 0.36038967,
     0.35756668, 0.35474832, 0.35193455, 0.34912542,
     0.34632129, 0.34352258, 0.34072974, 0.33794323,
     0.33516354, 0.33239114, 0.32962648, 0.32686967,
     0.32412042, 0.32137919, 0.31864044, 0.31588373,
     0.31309909, 0.31028631, 0.30745528, 0.30462678,
     0.30180656, 0.29899424, 0.29619082, 0.29339717,
     0.29061333, 0.28783935, 0.28507563, 0.28232266,
     0.27958067, 0.27684984, 0.27413017, 0.27142157,
     0.26872396, 0.26603737, 0.26336211, 0.26069855,
     0.25804700, 0.25540830, 0.25278329, 0.25017211,
     0.24757451, 0.24498713, 0.24240740, 0.23983550,
     0.23727200, 0.23471866, 0.23217624, 0.22964458,
     0.22712346, 0.22461258, 0.22211202, 0.21962197,
     0.21714290, 0.21467522, 0.21221877, 0.20977323,
     0.20733693, 0.20490860, 0.20248823, 0.20007615,
     0.19767358, 0.19528091, 0.19289781, 0.19052347,
     0.18815661, 0.18579693, 0.18344441, 0.18110010,
     0.17876595, 0.17644344, 0.17413400, 0.17183905,
     0.16956003, 0.16729836, 0.16505547, 0.16283278,
     0.15990780, 0.15776021, 0.15563325, 0.15352557,
     0.15143584, 0.14936270, 0.14730481, 0.14526081,
     0.14322937, 0.14120918, 0.13919977, 0.13720138,
     0.13521422, 0.13323852, 0.13127445, 0.12932216,
     0.12738181, 0.12545358, 0.12353773, 0.12163457,
     0.11974436, 0.11786730, 0.11600347, 0.11415293,
     0.11231573, 0.11049201, 0.10868196, 0.10688578,
     0.10510362, 0.10333551, 0.10158143, 0.09984133,
     0.09811524, 0.09640327, 0.09470556, 0.09302228,
     0.09135347, 0.08969907, 0.08805903, 0.08643326,
     0.08482183, 0.08322486, 0.08164249, 0.08007481,
     0.07852179, 0.07698335, 0.07545938, 0.07394984,
     0.07245482, 0.07097444, 0.06950883, 0.06805800,
     0.06662187, 0.06520031, 0.06379324, 0.06240065,
     0.06102266, 0.05965936, 0.05831084, 0.05697701,
     0.05565775, 0.05435290, 0.05306239, 0.05178628,
     0.05052464, 0.04927758, 0.04804510, 0.04682709,
     0.04562344, 0.04443405, 0.04325893, 0.04209822,
     0.04095208, 0.03982059, 0.03870371, 0.03760131,
     0.03651325, 0.03543944, 0.03437987, 0.03333454,
     0.03230348, 0.03128653, 0.03028332, 0.02929346,
     0.02831658, 0.02735252, 0.02640127, 0.02546283,
     0.02453725, 0.02362471, 0.02272547, 0.02183980,
     0.02096810, 0.02011108, 0.01926957, 0.01844439,
     0.01763565, 0.01684248, 0.01606394, 0.01529909,
     0.01454726, 0.01380802, 0.01308092, 0.01236569,
     0.01166273, 0.01097281, 0.01029671, 0.00963479,
     0.00898646, 0.00835089, 0.00772725, 0.00711521,
     0.00651513, 0.00592741, 0.00535249, 0.00479089,
     0.00424328, 0.00371041, 0.00319271, 0.00268947,
     0.00219928, 0.00172084, 0.00125271, 0.00079311,
     0.00034023, -0.00010786, -0.00055144, -0.00098865,
    -0.00141741, -0.00183557, -0.00224010, -0.00262725,
    -0.00299314, -0.00333475, -0.00365250, -0.00394867,
    -0.00422533, -0.00448528, -0.00473278, -0.00497252,
    -0.00520916, -0.00544584, -0.00568360, -0.00592326,
    -0.00616547, -0.00640861, -0.00664914, -0.00688354,
    -0.00710845, -0.00732136, -0.00752022, -0.00770289,
    -0.00786789, -0.00801521, -0.00814526, -0.00825839,
    -0.00835563, -0.00843882, -0.00850996, -0.00857097,
    -0.00862360, -0.00866943, -0.00871004, -0.00874688,
    -0.00878091, -0.00881277, -0.00884320, -0.00887248,
    -0.00890002, -0.00892494, -0.00894641, -0.00896355,
    -0.00897541, -0.00898104, -0.00897948, -0.00896990,
    -0.00895149, -0.00892346, -0.00888519, -0.00883670,
    -0.00877839, -0.00871058, -0.00863388, -0.00854936,
    -0.00845826, -0.00836179, -0.00826124, -0.00815807,
    -0.00805372, -0.00794953, -0.00784572, -0.00774156,
    -0.00763634, -0.00752929, -0.00741941, -0.00730556,
    -0.00718664, -0.00706184, -0.00693107, -0.00679443,
    -0.00665200, -0.00650428, -0.00635230, -0.00619718,
    -0.00603995, -0.00588133, -0.00572169, -0.00556143,
    -0.00540085, -0.00523988, -0.00507828, -0.00491582,
    -0.00475220, -0.00458693, -0.00441953, -0.00424950,
    -0.00407681, -0.00390204, -0.00372581, -0.00354874,
    -0.00337115, -0.00319318, -0.00301494, -0.00283652,
    -0.00265797, -0.00247934, -0.00230066, -0.00212197,
    -0.00194331, -0.00176471, -0.00158620, -0.00140787,
    -0.00122989, -0.00105244, -0.00087567, -0.00069976,
    -0.00052487, -0.00035115, -0.00017875, -0.00000782,
     0.00000779, 0.00017701, 0.00034552, 0.00051313,
     0.00067966, 0.00084492, 0.00100873, 0.00117093,
     0.00133133, 0.00148978, 0.00164611, 0.00180023,
     0.00195211, 0.00210172, 0.00224898, 0.00239383,
     0.00253618, 0.00267593, 0.00281306, 0.00294756,
     0.00307942, 0.00320864, 0.00333502, 0.00345816,
     0.00357762, 0.00369297, 0.00380414, 0.00391140,
     0.00401499, 0.00411524, 0.00421242, 0.00430678,
     0.00439859, 0.00448799, 0.00457487, 0.00465908,
     0.00474045, 0.00481857, 0.00489277, 0.00496235,
     0.00502666, 0.00508546, 0.00513877, 0.00518662,
     0.00522904, 0.00526648, 0.00529956, 0.00532895,
     0.00535532, 0.00537929, 0.00540141, 0.00542228,
     0.00544196, 0.00545981, 0.00547515, 0.00548726,
     0.00549542, 0.00549899, 0.00549732, 0.00548986,
     0.00547633, 0.00545664, 0.00543067, 0.00539849,
     0.00536061, 0.00531757, 0.00526993, 0.00521822,
     0.00516300, 0.00510485, 0.00504432, 0.00498194,
     0.00491822, 0.00485364, 0.00478862, 0.00472309,
     0.00465675, 0.00458939, 0.00452067, 0.00445003,
     0.00437688, 0.00430063, 0.00422062, 0.00413609,
     0.00404632, 0.00395060, 0.00384863, 0.00374044,
     0.00362600, 0.00350540, 0.00337934, 0.00324885,
     0.00311486, 0.00297849, 0.00284122, 0.00270458,
     0.00257013, 0.00243867, 0.00231005, 0.00218399,
     0.00206023, 0.00193766, 0.00181460, 0.00168938,
     0.00156050, 0.00142701, 0.00128830, 0.00114365,
     0.00099297, 0.00083752, 0.00067884, 0.00051845,
     0.00035760, 0.00019720, 0.00003813, -0.00011885,
    -0.00027375, -0.00042718, -0.00057975, -0.00073204,
    -0.00088453, -0.00103767, -0.00119192, -0.00134747,
    -0.00150411, -0.00166151, -0.00181932, -0.00197723,
    -0.00213493, -0.00229210, -0.00244849, -0.00260415,
    -0.00275928, -0.00291410, -0.00306879, -0.00322332,
    -0.00337759, -0.00353145, -0.00368470, -0.00383722,
    -0.00398892, -0.00413972, -0.00428967, -0.00443889,
    -0.00458749, -0.00473571, -0.00488366, -0.00503137,
    -0.00517887, -0.00532610, -0.00547302, -0.00561965,
    -0.00576598, -0.00591199, -0.00605766, -0.00620300,
    -0.00634801, -0.00649273, -0.00663727, -0.00678170,
    -0.00692617, -0.00707084, -0.00721583, -0.00736129,
    -0.00750735, -0.00765415, -0.00780184, -0.00795059,
    -0.00810058, -0.00825195, -0.00840487, -0.00855950,
    -0.00871607, -0.00887480, -0.00903596, -0.00919978,
    -0.00936650, -0.00953635, -0.00970931, -0.00988421,
    -0.01005916, -0.01023208, -0.01040130, -0.01056627,
    -0.01072678, -0.01088259, -0.01103348, -0.01117933,
    -0.01132004, -0.01145552, -0.01158573, -0.01171065,
    -0.01183025, -0.01194454, -0.01205352, -0.01215722,
    -0.01225572, -0.01234911, -0.01243749, -0.01252102,
    -0.01259985, -0.01267419, -0.01274437, -0.01281078,
    -0.01287379, -0.01293350, -0.01298972, -0.01304224,
    -0.01309086, -0.01313556, -0.01317644, -0.01321357,
    -0.01324707, -0.01327697, -0.01330334, -0.01332622,
    -0.01334570, -0.01336194, -0.01337510, -0.01338538,
    -0.01339276, -0.01339708, -0.01339816, -0.01339584,
    -0.01339014, -0.01338116, -0.01336903, -0.01335382,
    -0.01333545, -0.01331381, -0.01328876, -0.01326033,
    -0.01322880, -0.01319457, -0.01315806, -0.01311968,
    -0.01307987, -0.01303906, -0.01299769, -0.01295623,
    -0.01308207, -0.01304153, -0.01299802, -0.01295155,
    -0.01290215, -0.01284980, -0.01279450, -0.01273625,
    -0.01267501, -0.01261077, -0.01254347, -0.01247306,
    -0.01239950, -0.01232277, -0.01224304, -0.01216055,
    -0.01207554, -0.01198813, -0.01189829, -0.01180590,
    -0.01171090, -0.01161335, -0.01151352, -0.01141167,
    -0.01130807, -0.01120289, -0.01109626, -0.01098830,
    -0.01087916, -0.01076898, -0.01065793, -0.01054618,
    -0.01043380, -0.01032068, -0.01020670, -0.01009171,
    -0.00997585, -0.00985959, -0.00974338, -0.00962765,
    -0.00951273, -0.00939888, -0.00928634, -0.00917534,
    -0.00906604, -0.00895860, -0.00885313, -0.00874977,
    -0.00864862, -0.00854979, -0.00845337, -0.00835939,
    -0.00826785, -0.00817872, -0.00809195, -0.00800745,
    -0.00792506, -0.00784469, -0.00776588, -0.00768695,
    -0.00760568, -0.00752004, -0.00742875, -0.00733186,
    -0.00722976, -0.00712279, -0.00701130, -0.00689559,
    -0.00677595, -0.00665269, -0.00652610, -0.00639649,
    -0.00626417, -0.00612943, -0.00599252, -0.00585368,
    -0.00571315, -0.00557115, -0.00542792, -0.00528367,
    -0.00513864, -0.00499301, -0.00484693, -0.00470054,
    -0.00455395, -0.00440733, -0.00426086, -0.00411471,
    -0.00396904, -0.00382404, -0.00367991, -0.00353684,
    -0.00339502, -0.00325472, -0.00311618, -0.00297967,
    -0.00284531, -0.00271307, -0.00258290, -0.00245475,
    -0.00232860, -0.00220447, -0.00208236, -0.00196233,
    -0.00184450, -0.00172906, -0.00161620, -0.00150603,
    -0.00139852, -0.00129358, -0.00119112, -0.00109115,
    -0.00099375, -0.00089902, -0.00080705, -0.00071796,
    -0.00063185, -0.00054886, -0.00046904, -0.00039231,
    -0.00031845, -0.00024728, -0.00017860, -0.00011216,
    -0.00004771, 0.00001500, 0.00007600, 0.00013501,
     0.00019176, 0.00024595, 0.00029720, 0.00034504,
     0.00038902, 0.00042881, 0.00046456, 0.00049662,
     0.00052534, 0.00055114, 0.00057459, 0.00059629,
     0.00061684, 0.00063660, 0.00065568, 0.00067417,
     0.00069213, 0.00070935, 0.00072545, 0.00074005,
     0.00075283, 0.00076356, 0.00077209, 0.00077828,
     0.00078205, 0.00078350, 0.00078275, 0.00077992,
     0.00077520, 0.00076884, 0.00076108, 0.00075218,
     0.00074232, 0.00073170, 0.00072048, 0.00070881,
     0.00069680, 0.00068450, 0.00067201, 0.00065934,
     0.00064647, 0.00063335, 0.00061994, 0.00060621,
     0.00059211, 0.00057763, 0.00056274, 0.00054743,
     0.00053169, 0.00051553, 0.00049897, 0.00048206,
     0.00046487, 0.00044748, 0.00042996, 0.00041241,
     0.00039492, 0.00037759, 0.00036049, 0.00034371,
     0.00032732, 0.00031137, 0.00029587, 0.00028079,
     0.00026612, 0.00025183, 0.00023789, 0.00022428,
     0.00021097, 0.00019797, 0.00018530, 0.00017297,
     0.00016100, 0.00014942, 0.00013827, 0.00012757,
     0.00011736, 0.00010764, 0.00009841, 0.00008969,
     0.00008145, 0.00007369, 0.00006641, 0.00005958,
     0.00005320, 0.00004725, 0.00004171, 0.00003659,
     0.00003186, 0.00002752, 0.00002357, 0.00001999,
     0.00001679, 0.00001392, 0.00001140, 0.00000918,
     0.00000726, 0.00000562, 0.00000424, 0.00000309,
     0.00000217, 0.00000143, 0.00000088, 0.00000048,
     0.00000020, 0.00000004, -0.00000004, -0.00000006,
    -0.00000004, -0.00000000, 0.00000002, 0.00000000,
     0.00000000, 0.00000002, -0.00000000, -0.00000004,
    -0.00000005, -0.00000004, 0.00000004, 0.00000019,
     0.00000045, 0.00000083, 0.00000134, 0.00000201,
     0.00000285, 0.00000387, 0.00000510, 0.00000654,
     0.00000821, 0.00001011, 0.00001227, 0.00001468,
     0.00001735, 0.00002030, 0.00002352, 0.00002702,
     0.00003080, 0.00003486, 0.00003918, 0.00004379,
     0.00004866, 0.00005382, 0.00005924, 0.00006495,
     0.00007093, 0.00007719, 0.00008373, 0.00009053,
     0.00009758, 0.00010488, 0.00011240, 0.00012010,
     0.00012796, 0.00013596, 0.00014406, 0.00015226,
     0.00016053, 0.00016886, 0.00017725, 0.00018571,
     0.00019424, 0.00020286, 0.00021156, 0.00022037,
     0.00022928, 0.00023825, 0.00024724, 0.00025621,
     0.00026509, 0.00027385, 0.00028241, 0.00029072,
     0.00029874, 0.00030643, 0.00031374, 0.00032065,
     0.00032715, 0.00033325, 0.00033895, 0.00034425,
     0.00034917, 0.00035374, 0.00035796, 0.00036187,
     0.00036549, 0.00036883, 0.00037194, 0.00037479,
     0.00037736, 0.00037963, 0.00038154, 0.00038306,
     0.00038411, 0.00038462, 0.00038453, 0.00038373,
     0.00038213, 0.00037965, 0.00037621, 0.00037179,
     0.00036636, 0.00035989, 0.00035244, 0.00034407,
     0.00033488, 0.00032497, 0.00031449, 0.00030361,
     0.00029252, 0.00028133, 0.00027003, 0.00025862,
     0.00024706, 0.00023524, 0.00022297, 0.00021004,
     0.00019626, 0.00018150, 0.00016566, 0.00014864,
     0.00013041, 0.00011112, 0.00009096, 0.00007014,
     0.00004884, 0.00002718, 0.00000530, -0.00001667,
    -0.00003871, -0.00006090, -0.00008331, -0.00010600,
    -0.00012902, -0.00015244, -0.00017631, -0.00020065,
    -0.00022541, -0.00025052, -0.00027594, -0.00030159,
    -0.00032740, -0.00035332, -0.00037928, -0.00040527,
    -0.00043131, -0.00045741, -0.00048357, -0.00050978,
    -0.00053599, -0.00056217, -0.00058827, -0.00061423,
    -0.00064002, -0.00066562, -0.00069100, -0.00071616,
    -0.00074110, -0.00076584, -0.00079036, -0.00081465,
    -0.00083869, -0.00086245, -0.00088590, -0.00090901,
    -0.00093176, -0.00095413, -0.00097608, -0.00099758,
    -0.00101862, -0.00103918, -0.00105924, -0.00107879,
    -0.00109783, -0.00111635, -0.00113434, -0.00115181,
    -0.00116873, -0.00118510, -0.00120091, -0.00121615,
    -0.00123082, -0.00124490, -0.00125838, -0.00127125,
    -0.00128350, -0.00129511, -0.00130610, -0.00131643,
    -0.00132610, -0.00133509, -0.00134334, -0.00135069,
    -0.00135711, -0.00136272, -0.00136768, -0.00137225,
    -0.00137649, -0.00138042, -0.00138404, -0.00138737,
    -0.00139041, -0.00139317, -0.00139565, -0.00139785,
    -0.00139976, -0.00140137, -0.00140267, -0.00140366,
    -0.00140432, -0.00140464, -0.00140461, -0.00140423,
    -0.00140347, -0.00140235, -0.00140084, -0.00139894,
    -0.00139664, -0.00139388, -0.00139065, -0.00138694,
    -0.00138278, -0.00137818, -0.00137317, -0.00136772,
    -0.00136185, -0.00135556, -0.00134884, -0.00134170,
    -0.00133415, -0.00132619, -0.00131784, -0.00130908,
    -0.00129991, -0.00129031, -0.00128031, -0.00126990,
    -0.00125912, -0.00124797, -0.00123645, -0.00122458,
    -0.00121233, -0.00119972, -0.00118676, -0.00117347,
    -0.00115988, -0.00114605, -0.00113200, -0.00111778,
    -0.00110343, -0.00108898, -0.00107448, -0.00105995,
};

const int __attribute__ ((aligned (32))) ff_aac_eld_window_480_fixed[1800] = {
    0x00109442, 0x00482797, 0x0075bf2a, 0x00afa864,
    0x00ef2aa5, 0x01335b36, 0x017a4df0, 0x01c2cffe,
    0x020bfb4c, 0x0254fd74, 0x029d557c, 0x02e50574,
    0x032c41a8, 0x03732c08, 0x03b9cb88, 0x040032e8,
    0x044686f0, 0x048cd578, 0x04d30738, 0x05190500,
    0x055ec210, 0x05a44750, 0x05e9aeb8, 0x062f0c80,
    0x067477a0, 0x06ba1ac0, 0x07001998, 0x074680e0,
    0x078d5ec0, 0x07d4d038, 0x081cf8f0, 0x0865f8b0,
    0x08afe0e0, 0x08fab150, 0x09466cd0, 0x09931910,
    0x09e0adb0, 0x0a2f1640, 0x0a7e43f0, 0x0ace2960,
    0x0b1eb180, 0x0b6fc4b0, 0x0bc15050, 0x0c134710,
    0x0c65a420, 0x0cb86340, 0x0d0b7df0, 0x0d5ef450,
    0x0db2cb60, 0x0e070180, 0x0e5b91f0, 0x0eb07f20,
    0x0f05d0a0, 0x0f5b8920, 0x0fb1a950, 0x10082e40,
    0x105f1400, 0x10b65820, 0x110df780, 0x1165f120,
    0x11be43e0, 0x1216eea0, 0x126feac0, 0x12c92b00,
    0x1322a620, 0x137c55c0, 0x13d61ae0, 0x142fc940,
    0x148949e0, 0x14e28da0, 0x153b9a80, 0x15947640,
    0x15ed1840, 0x16458660, 0x169deb20, 0x16f663c0,
    0x174ef8c0, 0x17a7a120, 0x180041c0, 0x1858d000,
    0x18b14940, 0x1909a140, 0x1961c820, 0x19b9b620,
    0x1a116480, 0x1a68c1a0, 0x1abfbd00, 0x1b164f60,
    0x1b6c7580, 0x1bc23120, 0x1c1780e0, 0x1c6c5d00,
    0x1cc0dbe0, 0x1d1532a0, 0x1d697660, 0x1dbdac20,
    0x1e11b280, 0x1e655b80, 0x1eb89e80, 0x1f0b7720,
    0x1f5dd680, 0x1fafaec0, 0x2000fb00, 0x2051c340,
    0x20a22ac0, 0x20f24580, 0x214213c0, 0x21919140,
    0x21e0b300, 0x222f7580, 0x227dd900, 0x22cbd880,
    0x23196ec0, 0x23669b00, 0x23b35d80, 0x23ffb6c0,
    0x244ba7c0, 0x249731c0, 0x24e25700, 0x252d1940,
    0x2594ae40, 0x25deea40, 0x2628bd00, 0x26722680,
    0x26bb2740, 0x2703bf40, 0x274beec0, 0x2793b600,
    0x27db1500, 0x28220c00, 0x28689b80, 0x28aec4c0,
    0x28f48800, 0x2939e680, 0x297ee080, 0x29c37600,
    0x2a07a740, 0x2a4b74c0, 0x2a8ede80, 0x2ad1e500,
    0x2b148880, 0x2b56c940, 0x2b98a740, 0x2bda2240,
    0x2c1b3a80, 0x2c5bef80, 0x2c9c4100, 0x2cdc2e80,
    0x2d1bb800, 0x2d5adc80, 0x2d999b80, 0x2dd7f500,
    0x2e15e800, 0x2e537400, 0x2e9098c0, 0x2ecd5540,
    0x2f09a900, 0x2f4592c0, 0x2f811140, 0x2fbc2340,
    0x2ff6c7c0, 0x3030fe80, 0x306ac6c0, 0x30a41f80,
    0x30dd07c0, 0x31157dc0, 0x314d7fc0, 0x31850c80,
    0x31bc22c0, 0x31f2c1c0, 0x3228e840, 0x325e9540,
    0x3293c7c0, 0x32c87e40, 0x32fcb800, 0x33307340,
    0x3363aec0, 0x33966940, 0x33c8a140, 0x33fa5580,
    0x342b84c0, 0x345c2dc0, 0x348c4f80, 0x34bbe900,
    0x34eaf9c0, 0x35198080, 0x35477d00, 0x3574ee40,
    0x35a1d340, 0x35ce2bc0, 0x35f9f6c0, 0x36253380,
    0x364fe180, 0x367a0040, 0x36a38f80, 0x36cc8ec0,
    0x36f4fe80, 0x371cde80, 0x37442e80, 0x376aef00,
    0x37912000, 0x37b6c200, 0x37dbd600, 0x38005d00,
    0x38245840, 0x3847c880, 0x386aaf80, 0x388d0e80,
    0x38aee700, 0x38d03bc0, 0x38f11000, 0x39116700,
    0x39314440, 0x3950ab00, 0x396f9e80, 0x398e22c0,
    0x39ac3c40, 0x39c9f280, 0x39e74cc0, 0x3a045280,
    0x3a210b40, 0x3a3d7ec0, 0x3a59b480, 0x3a75b480,
    0x3a918900, 0x3aad3cc0, 0x3ac8db00, 0x3ae46bc0,
    0x3afff080, 0x3b1b6840, 0x3b36d2c0, 0x3b521980,
    0x3b6d0780, 0x3b876400, 0x3ba0f4c0, 0x3bb96740,
    0x3bd03dc0, 0x3be56580, 0x3bf6dec0, 0x3c0c6140,
    0x3c15a9c0, 0x3c1a5780, 0x3c1fd0c0, 0x3c25edc0,
    0x3c2c78c0, 0x3c333880, 0x3c39f3c0, 0x3c409100,
    0x3c471d00, 0x3c4da780, 0x3c543f40, 0x3c5ae880,
    0x3c619f00, 0x3c685f00, 0x3c6f25c0, 0x3c75f280,
    0x3c7cc6c0, 0x3c83a2c0, 0x3c8a87c0, 0x3c9175c0,
    0x3c986d00, 0x3c9f6e00, 0x3ca67880, 0x3cad8c40,
    0x3cb4a980, 0x3cbbd000, 0x3cc2ffc0, 0x3cca3940,
    0x3cd17d40, 0x3cd8cb80, 0x3ce02480, 0x3ce78740,
    0x3ceef3c0, 0x3cf66a00, 0x3cfdea00, 0x3d0574c0,
    0x3d0d0a40, 0x3d14ab40, 0x3d1c5700, 0x3d240d00,
    0x3d2bcd40, 0x3d3397c0, 0x3d3b6cc0, 0x3d434d00,
    0x3d4b38c0, 0x3d532fc0, 0x3d5b3180, 0x3d633dc0,
    0x3d6b53c0, 0x3d737400, 0x3d7b9f00, 0x3d83d540,
    0x3d8c1680, 0x3d946200, 0x3d9cb780, 0x3da51680,
    0x3dad7f00, 0x3db5f140, 0x3dbe6dc0, 0x3dc6f480,
    0x3dcf8540, 0x3dd81fc0, 0x3de0c300, 0x3de96ec0,
    0x3df22340, 0x3dfae0c0, 0x3e03a800, 0x3e0c7840,
    0x3e155180, 0x3e1e32c0, 0x3e271bc0, 0x3e300c00,
    0x3e390400, 0x3e420400, 0x3e4b0c40, 0x3e541c80,
    0x3e5d33c0, 0x3e6651c0, 0x3e6f7580, 0x3e789fc0,
    0x3e81d080, 0x3e8b0880, 0x3e944700, 0x3e9d8c00,
    0x3ea6d680, 0x3eb02600, 0x3eb97a80, 0x3ec2d400,
    0x3ecc3340, 0x3ed59880, 0x3edf0300, 0x3ee87280,
    0x3ef1e600, 0x3efb5d40, 0x3f04d880, 0x3f0e5840,
    0x3f17dcc0, 0x3f216600, 0x3f2af340, 0x3f348440,
    0x3f3e1840, 0x3f47af40, 0x3f514a00, 0x3f5ae840,
    0x3f648b00, 0x3f6e3140, 0x3f77db00, 0x3f818740,
    0x3f8b3600, 0x3f94e780, 0x3f9e9c40, 0x3fa85480,
    0x3fb21080, 0x3fbbcfc0, 0x3fc59200, 0x3fcf56c0,
    0x3fd91dc0, 0x3fe2e640, 0x3fecb040, 0x3ff67b40,
    0x40098600, 0x40135580, 0x401d2700, 0x4026fa00,
    0x4030ce80, 0x403aa380, 0x40447900, 0x404e4f00,
    0x40582400, 0x4061f900, 0x406bcd00, 0x4075a080,
    0x407f7480, 0x40894900, 0x40931e00, 0x409cf280,
    0x40a6c600, 0x40b09800, 0x40ba6980, 0x40c43a80,
    0x40ce0b00, 0x40d7db00, 0x40e1ab00, 0x40eb7980,
    0x40f54600, 0x40ff1080, 0x4108d980, 0x4112a100,
    0x411c6800, 0x41262d80, 0x412ff080, 0x4139b180,
    0x41436e80, 0x414d2980, 0x4156e100, 0x41609700,
    0x416a4a80, 0x4173fb00, 0x417da800, 0x41875000,
    0x4190f400, 0x419a9400, 0x41a43000, 0x41adc880,
    0x41b75d00, 0x41c0ec80, 0x41ca7700, 0x41d3fb00,
    0x41dd7980, 0x41e6f280, 0x41f06600, 0x41f9d480,
    0x42033d00, 0x420c9f00, 0x4215f980, 0x421f4d00,
    0x42289900, 0x4231de80, 0x423b1d00, 0x42445500,
    0x424d8500, 0x4256ad00, 0x425fcc80, 0x4268e380,
    0x4271f200, 0x427af900, 0x4283f880, 0x428cef80,
    0x4295de00, 0x429ec280, 0x42a79d80, 0x42b06f00,
    0x42b93800, 0x42c1f800, 0x42caaf80, 0x42d35d80,
    0x42dc0100, 0x42e49b00, 0x42ed2a80, 0x42f5b080,
    0x42fe2d80, 0x4306a180, 0x430f0c80, 0x43176d80,
    0x431fc480, 0x43281100, 0x43305400, 0x43388e80,
    0x4340c000, 0x4348e900, 0x43510900, 0x43591f00,
    0x43612b80, 0x43692f00, 0x43712900, 0x43791a80,
    0x43810380, 0x4388e400, 0x4390bc00, 0x43988b00,
    0x43a05180, 0x43a80f00, 0x43afc480, 0x43b77180,
    0x43bf1780, 0x43c6b700, 0x43ce5100, 0x43d5e580,
    0x43dd7100, 0x43e4ef80, 0x43ec5b80, 0x43f3ba80,
    0x43fb1c80, 0x44029400, 0x440a2e80, 0x4411d080,
    0x44193800, 0x44202480, 0x44265880, 0x442ba780,
    0x442d8680, 0x4428a500, 0x44241380, 0x441ccb00,
    0x44140100, 0x440a1200, 0x43ff7280, 0x43f46980,
    0x43e93200, 0x43ddff00, 0x43d2dc80, 0x43c7ac00,
    0x43bc4900, 0x43b09400, 0x43a47d80, 0x4397fd80,
    0x438b0780, 0x437d9b80, 0x436fd380, 0x4361cd80,
    0x4353a800, 0x43457500, 0x43373c80, 0x43290500,
    0x431ad400, 0x430ca280, 0x42fe6000, 0x42f00080,
    0x42e17380, 0x42d29e00, 0x42c35d80, 0x42b39200,
    0x42a32080, 0x4291fc00, 0x42801900, 0x426d6d80,
    0x4259f680, 0x4245bd00, 0x4230ca80, 0x421b2900,
    0x4204e800, 0x41ee1d00, 0x41d6dd80, 0x41bf3c80,
    0x41a74680, 0x418f0680, 0x41768800, 0x415dd100,
    0x4144e400, 0x412bbf80, 0x41126400, 0x40f8cc00,
    0x40deea00, 0x40c4b100, 0x40aa1400, 0x408f0800,
    0x40738380, 0x40577d80, 0x403aeb80, 0x401dc180,
    0x3ffff240, 0x3fe170c0, 0x3fc232c0, 0x3fa23680,
    0x3f817c40, 0x3f6002c0, 0x3f3ddec0, 0x3f1b4180,
    0x3ef85d40, 0x3ed56340, 0x3eb27240, 0x3e8f9c40,
    0x3e6cf400, 0x3e4a81c0, 0x3e282140, 0x3e059980,
    0x3de2b280, 0x3dbf4100, 0x3d9b3640, 0x3d768b00,
    0x3d513640, 0x3d2b3840, 0x3d049b80, 0x3cdd6b40,
    0x3cb5b400, 0x3c8d8f40, 0x3c652080, 0x3c3c8c40,
    0x3c13f480, 0x3beb7580, 0x3bc327c0, 0x3b9b2680,
    0x3b737000, 0x3b4bc580, 0x3b23d740, 0x3afb5640,
    0x3ad21c40, 0x3aa83780, 0x3a7dbc40, 0x3a52bf80,
    0x3a276600, 0x39fbe0c0, 0x39d06140, 0x39a50ec0,
    0x3979e300, 0x394ebf40, 0x392386c0, 0x38f82280,
    0x38cc89c0, 0x38a0b7c0, 0x3874a740, 0x38485840,
    0x381bd1c0, 0x37ef1b40, 0x37c23cc0, 0x37953dc0,
    0x376825c0, 0x373afc80, 0x370dc980, 0x36e09440,
    0x36b41dc0, 0x36862100, 0x3657e480, 0x36297240,
    0x35fad380, 0x35cc1200, 0x359d36c0, 0x356e4b40,
    0x353f5880, 0x35106780, 0x34e17780, 0x34b28240,
    0x34838040, 0x345466c0, 0x34251940, 0x33f57280,
    0x33c54bc0, 0x33949840, 0x33638380, 0x33324980,
    0x33012500, 0x32d04480, 0x329fc7c0, 0x326fcbc0,
    0x324068c0, 0x32116fc0, 0x31e27600, 0x31b30fc0,
    0x3182e300, 0x3151e240, 0x312029c0, 0x30edd080,
    0x30baf700, 0x3087cd00, 0x30548600, 0x30215680,
    0x2fee65c0, 0x2fbbca40, 0x2f899980, 0x2f57e6c0,
    0x2f26b540, 0x2ef5f980, 0x2ec5aa00, 0x2e95afc0,
    0x2e65c180, 0x2e357b40, 0x2e047840, 0x2dd27380,
    0x2d9f6c40, 0x2d6b7780, 0x2d36a6c0, 0x2d012940,
    0x2ccb5680, 0x2c958a00, 0x2c601b80, 0x2c2b3640,
    0x2bf6dfc0, 0x2bc31ec0, 0x2b8ff500, 0x2b5d5540,
    0x2b2b2a00, 0x2af95e80, 0x2ac7dd80, 0x2a968f80,
    0x2a655d40, 0x2a342f00, 0x2a02e8c0, 0x29d16700,
    0x299f8640, 0x296d2380, 0x293a2740, 0x29068400,
    0x28d22b40, 0x289d1540, 0x28675280, 0x28310180,
    0x27fa3f00, 0x27c32f80, 0x278c08c0, 0x275505c0,
    0x271e60c0, 0x26e84b00, 0x26b2e880, 0x267e5cc0,
    0x264ac940, 0x26183a40, 0x25e6aa80, 0x25b615c0,
    0x25866b80, 0x25576b40, 0x2528ba00, 0x24f9ffc0,
    0x24cadfc0, 0x249af540, 0x2469da80, 0x24372780,
    0x2402b800, 0x23ccbfc0, 0x23957cc0, 0x235d3140,
    0x23245200, 0x22eb8000, 0x22b35cc0, 0x227c7940,
    0x22471d40, 0x22136840, 0x21e18240, 0x21b15d80,
    0x21827dc0, 0x21544600, 0x21261b00, 0x20f78600,
    0x20c83e00, 0x20980000, 0x20668e00, 0x2033f300,
    0x20007400, 0x1fcc64e0, 0x1f97d120, 0x1f642320,
    0x1f2f49e0, 0x1efaa840, 0x1ec73580, 0x1e94d880,
    0x1e636120, 0x1e32a160, 0x1e025ba0, 0x1dd24300,
    0x1da20e60, 0x1d717940, 0x1d407560, 0x1d0f2040,
    0x1cdd95c0, 0x1cabf500, 0x1c7a6940, 0x1c492340,
    0x1c185680, 0x1be818c0, 0x1bb83f60, 0x1b888d20,
    0x1b58c640, 0x1b28c240, 0x1af871e0, 0x1ac7c960,
    0x1a96bf00, 0x1a656b60, 0x1a340360, 0x1a02bd20,
    0x19d1c6c0, 0x19a12f40, 0x1970f480, 0x19411640,
    0x19119000, 0x18e255a0, 0x18b358a0, 0x18848b20,
    0x1855e040, 0x18274e00, 0x17f8c9e0, 0x17ca4a80,
    0x179bce40, 0x176d5a60, 0x173ef400, 0x17109fe0,
    0x16e25f60, 0x16b43240, 0x16861880, 0x16581220,
    0x162a20c0, 0x15fc4620, 0x15ce8420, 0x15a0dca0,
    0x157351c0, 0x1545e580, 0x151899a0, 0x14eb6ec0,
    0x14be63a0, 0x14917a00, 0x14649ae0, 0x14377060,
    0x1409d0c0, 0x13dbbb20, 0x13ad58e0, 0x137f0160,
    0x1350cc80, 0x1322b8c0, 0x12f4ca60, 0x12c704e0,
    0x129968a0, 0x126bf5c0, 0x123eade0, 0x12119300,
    0x11e4a660, 0x11b7e860, 0x118b5940, 0x115ef8a0,
    0x1132c600, 0x1106c1a0, 0x10daecc0, 0x10af4900,
    0x1083d7a0, 0x10589c00, 0x102d9a00, 0x1002d1e0,
    0x0fd842c0, 0x0fadde80, 0x0f839a50, 0x0f597700,
    0x0f2f76e0, 0x0f05a170, 0x0edbf9c0, 0x0eb27f30,
    0x0e8930d0, 0x0e600d70, 0x0e371550, 0x0e0e4950,
    0x0de5ab50, 0x0dbd3d20, 0x0d94fe10, 0x0d6cecb0,
    0x0d450220, 0x0d1d38f0, 0x0cf59130, 0x0cce0c30,
    0x0ca6af10, 0x0c7f7b80, 0x0c587010, 0x0c318960,
    0x0c0ac200, 0x0be418d0, 0x0bbd8da0, 0x0b9724e0,
    0x0b70e6c0, 0x0b4ad970, 0x0b2502f0, 0x0aff6930,
    0x0ada1250, 0x0ab50430, 0x0a9044d0, 0x0a6bda30,
    0x0a3bedf0, 0x0a18be40, 0x09f5e530, 0x09d35cf0,
    0x09b11ff0, 0x098f2890, 0x096d7120, 0x094bf400,
    0x092aab80, 0x09099240, 0x08e8a620, 0x08c7e850,
    0x08a75990, 0x0886fae0, 0x0866ccf0, 0x0846d070,
    0x08270610, 0x08076e70, 0x07e80ac8, 0x07c8dc60,
    0x07a9e440, 0x078b2348, 0x076c99d0, 0x074e4818,
    0x07302e50, 0x07124d18, 0x06f4a530, 0x06d73778,
    0x06ba0488, 0x069d0c88, 0x06804f68, 0x0663cce0,
    0x06478528, 0x062b78a0, 0x060fa7e8, 0x05f413b8,
    0x05d8bc38, 0x05bda128, 0x05a2c258, 0x05881f60,
    0x056db888, 0x05538e60, 0x0539a170, 0x051ff218,
    0x05068040, 0x04ed4b90, 0x04d45398, 0x04bb9820,
    0x04a31988, 0x048ad860, 0x0472d528, 0x045b0ff0,
    0x04438860, 0x042c3de8, 0x04153040, 0x03fe5f4c,
    0x03e7cb98, 0x03d17580, 0x03bb5d64, 0x03a582e8,
    0x038fe588, 0x037a8494, 0x03655fcc, 0x03507768,
    0x033bcbb4, 0x03275d28, 0x03132bc0, 0x02ff370c,
    0x02eb7e94, 0x02d801e8, 0x02c4c11c, 0x02b1bcbc,
    0x029ef578, 0x028c6ba8, 0x027a1f20, 0x02680f54,
    0x02563bac, 0x0244a3c8, 0x023347a0, 0x02222730,
    0x0211429c, 0x02009938, 0x01f02974, 0x01dff1ae,
    0x01cff058, 0x01c024c8, 0x01b08ef4, 0x01a12eda,
    0x019204b0, 0x01831138, 0x01745588, 0x0165d2c2,
    0x01578a96, 0x01497ffc, 0x013bb670, 0x012e3160,
    0x0120f146, 0x0113f27c, 0x0107310c, 0x00faa909,
    0x00ee57a1, 0x00e23b09, 0x00d6515b, 0x00ca9977,
    0x00bf1509, 0x00b3c74d, 0x00a8b388, 0x009ddb3d,
    0x00933bf2, 0x0088d22c, 0x007e9a70, 0x0074935a,
    0x006abe70, 0x00611d5c, 0x0057b1f8, 0x004e7e73,
    0x0045859b, 0x003cca96, 0x00344f32, 0x002c1074,
    0x00240873, 0x001c31ba, 0x0014863f, 0x000cfe8b,
    0x00059307, 0xfffe3b9a, 0xfff6f718, 0xffefcd4d,
    0xffe8c6f4, 0xffe1ed10, 0xffdb4c57, 0xffd4f484,
    0xffcef5dc, 0xffc95d0c, 0xffc4284e, 0xffbf4e14,
    0xffbac5ae, 0xffb68360, 0xffb27548, 0xffae87be,
    0xffaaa733, 0xffa6c67e, 0xffa2e141, 0xff9ef40c,
    0xff9afc25, 0xff970058, 0xff930f7c, 0xff8f3857,
    0xff8b8900, 0xff880bfe, 0xff84c9ea, 0xff81cbbd,
    0xff7f17ad, 0xff7cadc6, 0xff7a8c4e, 0xff78b1cd,
    0xff7719f3, 0xff75bd06, 0xff7492a4, 0xff7392bf,
    0xff72b600, 0xff71f5c6, 0xff714b72, 0xff70b0ed,
    0xff702232, 0xff6f9c90, 0xff6f1cee, 0xff6ea21f,
    0xff6e2e9c, 0xff6dc617, 0xff6d6c09, 0xff6d2425,
    0xff6cf267, 0xff6cdaca, 0xff6ce155, 0xff6d0983,
    0xff6d56bb, 0xff6dcc4c, 0xff6e6cd0, 0xff6f3832,
    0xff702cc4, 0xff71492e, 0xff728ae2, 0xff73ed63,
    0xff756b7c, 0xff77001c, 0xff78a5d9, 0xff7a5693,
    0xff7c0c40, 0xff7dc141, 0xff7f74aa, 0xff81298b,
    0xff82e2de, 0xff84a3de, 0xff8670bd, 0xff884e42,
    0xff8a410c, 0xff8c4c7f, 0xff8e70fc, 0xff90ae18,
    0xff93037e, 0xff956f12, 0xff97ec86, 0xff9a7724,
    0xff9d0a9d, 0xff9fa3ea, 0xffa2417e, 0xffa4e1ac,
    0xffa78332, 0xffaa265a, 0xffaccc26, 0xffaf758e,
    0xffb223d4, 0xffb4d906, 0xffb79726, 0xffba604e,
    0xffbd349e, 0xffc011a8, 0xffc2f4d2, 0xffc5db82,
    0xffc8c45f, 0xffcbaed5, 0xffce9a6d, 0xffd186c6,
    0xffd473aa, 0xffd760e5, 0xffda4e55, 0xffdd3bd0,
    0xffe0292b, 0xffe31645, 0xffe602ff, 0xffe8eef7,
    0xffebd978, 0xffeec1bf, 0xfff1a72c, 0xfff488fe,
    0xfff76689, 0xfffa3f2c, 0xfffd1245, 0xffffdf33,
    0x000020ac, 0x0002e66f, 0x0005a937, 0x00086839,
    0x000b22b3, 0x000dd7da, 0x001086ec, 0x00132f3c,
    0x0015d001, 0x00186897, 0x001af849, 0x001d7eb6,
    0x001ffbbe, 0x00226f41, 0x0024d8e8, 0x00273874,
    0x00298d82, 0x002bd7aa, 0x002e16d4, 0x00304af6,
    0x00327406, 0x00349203, 0x0036a416, 0x0038a893,
    0x003a9da0, 0x003c8170, 0x003e53b8, 0x0040159a,
    0x0041c816, 0x00436c92, 0x0045042c, 0x00468ff2,
    0x00481106, 0x004987fe, 0x004af466, 0x004c5599,
    0x004daae4, 0x004ef28c, 0x005029c4, 0x00514d9a,
    0x00525b57, 0x005351f7, 0x00543190, 0x0054fa43,
    0x0055ac2f, 0x00564938, 0x0056d3f7, 0x00574f3c,
    0x0057bdd7, 0x00582260, 0x00587f28, 0x0058d6b1,
    0x0059293c, 0x0059741a, 0x0059b472, 0x0059e73c,
    0x005a0976, 0x005a1870, 0x005a116e, 0x0059f224,
    0x0059b964, 0x005966ce, 0x0058f9e2, 0x005872e8,
    0x0057d407, 0x00571f82, 0x005657b0, 0x00557ecd,
    0x00549731, 0x0053a34b, 0x0052a56a, 0x00519fc6,
    0x00509482, 0x004f85a4, 0x004e74ee, 0x004d6214,
    0x004c4bd3, 0x004b314c, 0x004a1110, 0x0048e8c8,
    0x0047b5f7, 0x00467626, 0x00452690, 0x0043c405,
    0x00424b7f, 0x0040ba04, 0x003f0e53, 0x003d488b,
    0x003b688c, 0x00396eb6, 0x00375dfb, 0x00353aaa,
    0x003308ac, 0x0030ccb1, 0x002e8cf1, 0x002c4fd5,
    0x002a1be8, 0x0027f486, 0x0025d90d, 0x0023c852,
    0x0021c13b, 0x001fbf23, 0x001dbafc, 0x001badc6,
    0x00199136, 0x00176150, 0x00151b86, 0x0012bcd1,
    0x001044d1, 0x000db8d0, 0x000b1f43, 0x00087e89,
    0x0005dbe2, 0x00033b1e, 0x00009fee, 0xfffe0d82,
    0xfffb83cf, 0xfff90047, 0xfff6805a, 0xfff4019a,
    0xfff18203, 0xffeeffb2, 0xffec78ba, 0xffe9ec4d,
    0xffe75b4e, 0xffe4c71f, 0xffe23138, 0xffdf9ae6,
    0xffdd0574, 0xffda723c, 0xffd7e24a, 0xffd55567,
    0xffd2cabe, 0xffd04161, 0xffcdb890, 0xffcb306a,
    0xffc8a95c, 0xffc62406, 0xffc3a140, 0xffc12188,
    0xffbea542, 0xffbc2cc2, 0xffb9b7d2, 0xffb745f2,
    0xffb4d6ac, 0xffb268fe, 0xffaffc72, 0xffad90e8,
    0xffab263e, 0xffa8bcb8, 0xffa6547e, 0xffa3ed7b,
    0xffa187ba, 0xff9f2351, 0xff9cc055, 0xff9a5ebc,
    0xff97fe84, 0xff959f84, 0xff934146, 0xff90e37d,
    0xff8e858a, 0xff8c26c0, 0xff89c69e, 0xff876483,
    0xff84ffe4, 0xff82982b, 0xff802cb6, 0xff7dbccf,
    0xff7b47b4, 0xff78ccd0, 0xff764b6c, 0xff73c2db,
    0xff713227, 0xff6e9864, 0xff6bf470, 0xff694553,
    0xff668a0d, 0xff63c1a6, 0xff60ec34, 0xff5e0e9e,
    0xff5b30d3, 0xff585b8c, 0xff5595c9, 0xff52e1da,
    0xff5040a0, 0xff4db31c, 0xff4b3a3b, 0xff48d67e,
    0xff468850, 0xff445011, 0xff422ded, 0xff4021f9,
    0xff3e2c56, 0xff3c4cf8, 0xff3a83df, 0xff38d0ec,
    0xff3733c9, 0xff35ac14, 0xff343963, 0xff32db09,
    0xff319066, 0xff305898, 0xff2f323d, 0xff2e1bb2,
    0xff2d1369, 0xff2c18f8, 0xff2b2d2a, 0xff2a50e1,
    0xff2984f4, 0xff28c978, 0xff281e01, 0xff278245,
    0xff26f5c3, 0xff26785a, 0xff2609bf, 0xff25a9c8,
    0xff255814, 0xff2513f6, 0xff24dcc4, 0xff24b1a6,
    0xff2492b1, 0xff248093, 0xff247c0b, 0xff2485c6,
    0xff249daf, 0xff24c359, 0xff24f639, 0xff253605,
    0xff258312, 0xff25ddd5, 0xff2646e7, 0xff26be25,
    0xff274264, 0xff27d1f6, 0xff286b19, 0xff290c13,
    0xff29b30d, 0xff2a5e38, 0xff2b0bbd, 0xff2bb9a2,
    0xff29a9d2, 0xff2a53dc, 0xff2b0a5a, 0xff2bcd43,
    0xff2c9c76, 0xff2d7808, 0xff2e5ffa, 0xff2f544c,
    0xff305528, 0xff316299, 0xff327ce0, 0xff33a432,
    0xff34d8ba, 0xff361a8e, 0xff3768f8, 0xff38c2f5,
    0xff3a2784, 0xff3b9623, 0xff3d0ef4, 0xff3e9277,
    0xff4020ed, 0xff41ba14, 0xff435ccc, 0xff4507fd,
    0xff46ba84, 0xff4873ac, 0xff4a32ea, 0xff4bf7bb,
    0xff4dc17f, 0xff4f8fa0, 0xff516167, 0xff53361d,
    0xff550d79, 0xff56e7ee, 0xff58c5ff, 0xff5aa84d,
    0xff5c8e41, 0xff5e75e2, 0xff605d4d, 0xff6242b6,
    0xff6424b8, 0xff66023d, 0xff67da44, 0xff69abd6,
    0xff6b7646, 0xff6d38e8, 0xff6ef348, 0xff70a4ce,
    0xff724d0f, 0xff73eb95, 0xff757fff, 0xff770a2d,
    0xff788a20, 0xff79fff6, 0xff7b6be7, 0xff7cce52,
    0xff7e27e4, 0xff7f78fc, 0xff80c38a, 0xff820e98,
    0xff836378, 0xff84caaa, 0xff864990, 0xff87dff4,
    0xff898c30, 0xff8b4cda, 0xff8d207a, 0xff8f05cc,
    0xff90fb9b, 0xff930098, 0xff95138e, 0xff97332d,
    0xff995e2a, 0xff9b934e, 0xff9dd18c, 0xffa017e3,
    0xffa26550, 0xffa4b8e7, 0xffa711a8, 0xffa96eae,
    0xffabcefc, 0xffae31cc, 0xffb09680, 0xffb2fc82,
    0xffb5635a, 0xffb7ca52, 0xffba30a8, 0xffbc95a8,
    0xffbef8a4, 0xffc158d0, 0xffc3b557, 0xffc60d6b,
    0xffc86041, 0xffcaacb7, 0xffccf1cb, 0xffcf2e5c,
    0xffd161e8, 0xffd38c8f, 0xffd5ae88, 0xffd7c808,
    0xffd9d925, 0xffdbe1c8, 0xffdde1f3, 0xffdfd964,
    0xffe1c79b, 0xffe3abcc, 0xffe5852a, 0xffe75341,
    0xffe9162f, 0xffeace55, 0xffec7c15, 0xffee1f63,
    0xffefb7e9, 0xfff1453d, 0xfff2c6fd, 0xfff43ca8,
    0xfff5a5d4, 0xfff701ea, 0xfff850b4, 0xfff99288,
    0xfffac853, 0xfffbf2d5, 0xfffd12e6, 0xfffe2991,
    0xffff37e4, 0x00003eea, 0x00013ec4, 0x00023646,
    0x0003244d, 0x00040797, 0x0004de8c, 0x0005a734,
    0x00065fab, 0x0007068f, 0x00079c82, 0x000822fa,
    0x00089b70, 0x000907a6, 0x00096a01, 0x0009c506,
    0x000a1b37, 0x000a6e18, 0x000abe1f, 0x000b0bac,
    0x000b5701, 0x000b9f3b, 0x000be2c2, 0x000c1fff,
    0x000c5599, 0x000c829a, 0x000ca661, 0x000cc058,
    0x000cd028, 0x000cd63d, 0x000cd317, 0x000cc739,
    0x000cb36d, 0x000c98c0, 0x000c7833, 0x000c52df,
    0x000c2984, 0x000bfcf9, 0x000bcdea, 0x000b9cf7,
    0x000b6a97, 0x000b3700, 0x000b029d, 0x000acd79,
    0x000a977e, 0x000a6076, 0x000a2838, 0x0009eea1,
    0x0009b37d, 0x000976c2, 0x0009384e, 0x0008f816,
    0x0008b612, 0x0008724a, 0x00082cd5, 0x0007e5e8,
    0x00079dce, 0x000754de, 0x00070b62, 0x0006c1c6,
    0x0006786a, 0x00062fba, 0x0005e801, 0x0005a1a0,
    0x00055ce1, 0x000519fb, 0x0004d8f8, 0x000499b8,
    0x00045c30, 0x00042040, 0x0003e5c8, 0x0003acb3,
    0x000374df, 0x00033e59, 0x00030934, 0x0002d57d,
    0x0002a348, 0x000272b6, 0x000243f2, 0x00021711,
    0x0001ec3e, 0x0001c37a, 0x00019cc3, 0x00017830,
    0x000155a0, 0x00013514, 0x0001168b, 0x0000f9e6,
    0x0000df23, 0x0000c62e, 0x0000aef2, 0x00009978,
    0x000085a1, 0x0000736d, 0x000062dc, 0x000053d8,
    0x0000466c, 0x00003a62, 0x00002fd1, 0x00002681,
    0x00001e73, 0x00001792, 0x000011c9, 0x00000cf6,
    0x0000091a, 0x000005ff, 0x000003b1, 0x00000203,
    0x000000d7, 0x0000002b, 0xffffffd5, 0xffffffc0,
    0xffffffd5, 0x00000000, 0x00000015, 0x00000000,
    0x00000000, 0x00000015, 0x00000000, 0xffffffd5,
    0xffffffca, 0xffffffd5, 0x0000002b, 0x000000cc,
    0x000001e3, 0x0000037b, 0x0000059f, 0x0000086e,
    0x00000bf4, 0x0000103b, 0x00001564, 0x00001b6e,
    0x0000226f, 0x00002a68, 0x00003377, 0x00003d93,
    0x000048c5, 0x00005525, 0x000062a6, 0x00007155,
    0x0000812f, 0x00009237, 0x0000a455, 0x0000b7ab,
    0x0000cc18, 0x0000e1bd, 0x0000f878, 0x0001106c,
    0x00012981, 0x000143c2, 0x00015f30, 0x00017bb6,
    0x00019948, 0x0001b7e6, 0x0001d771, 0x0001f7bc,
    0x000218b4, 0x00023a42, 0x00025c3b, 0x00027ea0,
    0x0002a150, 0x0002c440, 0x0002e771, 0x00030aed,
    0x00032eb4, 0x000352db, 0x00037759, 0x00039c4c,
    0x0003c1ac, 0x0003e74b, 0x00040d00, 0x0004329f,
    0x000457de, 0x00047c9c, 0x0004a083, 0x0004c35e,
    0x0004e502, 0x00050543, 0x000523ec, 0x000540e7,
    0x00055c2b, 0x000575c0, 0x00058da9, 0x0005a3e4,
    0x0005b886, 0x0005cbb1, 0x0005dd65, 0x0005edcb,
    0x0005fcfa, 0x00060afc, 0x00061808, 0x000623fc,
    0x00062ec3, 0x00063849, 0x0006404b, 0x000646ac,
    0x00064b13, 0x00064d37, 0x00064cd6, 0x0006497b,
    0x000642c5, 0x0006385e, 0x000629f0, 0x00061766,
    0x000600a0, 0x0005e57d, 0x0005c63e, 0x0005a322,
    0x00057c97, 0x00055306, 0x00052711, 0x0004f96f,
    0x0004caeb, 0x00049bfc, 0x00046c96, 0x00043cbb,
    0x00040c3f, 0x0003daab, 0x0003a734, 0x000370f9,
    0x0003372d, 0x0002f944, 0x0002b6d4, 0x00026f71,
    0x000222fb, 0x0001d212, 0x00017d84, 0x00012630,
    0x0000ccda, 0x00007200, 0x0000163b, 0xffffba15,
    0xffff5da3, 0xffff0091, 0xfffea293, 0xfffe4367,
    0xfffde2da, 0xfffd809f, 0xfffd1c81, 0xfffcb66a,
    0xfffc4e90, 0xfffbe53e, 0xfffb7aa0, 0xfffb0f0a,
    0xfffaa2c9, 0xfffa3612, 0xfff9c92f, 0xfff95c2d,
    0xfff8eef4, 0xfff8817c, 0xfff813c3, 0xfff7a5d4,
    0xfff737e5, 0xfff6ca17, 0xfff65c9e, 0xfff5efbc,
    0xfff58390, 0xfff51830, 0xfff4adbc, 0xfff44435,
    0xfff3db9a, 0xfff373d6, 0xfff30cfd, 0xfff2a71c,
    0xfff24248, 0xfff1de9f, 0xfff17c44, 0xfff11b56,
    0xfff0bbea, 0xfff05e17, 0xfff00206, 0xffefa7d9,
    0xffef4f99, 0xffeef95d, 0xffeea53a, 0xffee533a,
    0xffee035e, 0xffedb5b0, 0xffed6a3c, 0xffed20f5,
    0xffecd9fe, 0xffec9555, 0xffec5305, 0xffec1319,
    0xffebd591, 0xffeb9a83, 0xffeb61f9, 0xffeb2bfe,
    0xffeaf89c, 0xffeac7ea, 0xffea99d2, 0xffea6e7e,
    0xffea45ef, 0xffea203a, 0xffe9fda0, 0xffe9decc,
    0xffe9c3de, 0xffe9ac56, 0xffe99789, 0xffe9845e,
    0xffe97295, 0xffe96219, 0xffe952ea, 0xffe944f3,
    0xffe93833, 0xffe92c9f, 0xffe92238, 0xffe918fe,
    0xffe910fb, 0xffe90a3a, 0xffe904c6, 0xffe900a0,
    0xffe8fddb, 0xffe8fc83, 0xffe8fca4, 0xffe8fe3c,
    0xffe9016c, 0xffe9061e, 0xffe90c74, 0xffe9146c,
    0xffe91e11, 0xffe929a5, 0xffe93731, 0xffe946c0,
    0xffe95833, 0xffe96b7e, 0xffe98082, 0xffe9975e,
    0xffe9affd, 0xffe9ca5e, 0xffe9e68e, 0xffea0481,
    0xffea242b, 0xffea458e, 0xffea6894, 0xffea8d52,
    0xffeab3c8, 0xffeadc0c, 0xffeb05fe, 0xffeb31a7,
    0xffeb5ede, 0xffeb8da2, 0xffebbdf4, 0xffebefbd,
    0xffec231f, 0xffec5802, 0xffec8e5e, 0xffecc61c,
    0xffecff1c, 0xffed391e, 0xffed740c, 0xffedafb1,
    0xffedebe1, 0xffee287d, 0xffee654e, 0xffeea23f,
};

static void aac_tableinit(void)
{

    static const float exp2_lut[] = {
        1.00000000000000000000,
        1.04427378242741384032,
        1.09050773266525765921,
        1.13878863475669165370,
        1.18920711500272106672,
        1.24185781207348404859,
        1.29683955465100966593,
        1.35425554693689272830,
        1.41421356237309504880,
        1.47682614593949931139,
        1.54221082540794082361,
        1.61049033194925430818,
        1.68179283050742908606,
        1.75625216037329948311,
        1.83400808640934246349,
        1.91520656139714729387,
    };
    float t1 = 8.8817841970012523233890533447265625e-16;
    float t2 = 3.63797880709171295166015625e-12;
    int t1_inc_cur, t2_inc_cur;
    int t1_inc_prev = 0;
    int t2_inc_prev = 8;

    for (int i = 0; i < 428; i++) {
        t1_inc_cur = 4 * (i % 4);
        t2_inc_cur = (8 + 3*i) % 16;
        if (t1_inc_cur < t1_inc_prev)
            t1 *= 2;
        if (t2_inc_cur < t2_inc_prev)
            t2 *= 2;



        ff_aac_pow2sf_tab[i] = t1 * exp2_lut[t1_inc_cur];
        ff_aac_pow34sf_tab[i] = t2 * exp2_lut[t2_inc_cur];
        t1_inc_prev = t1_inc_cur;
        t2_inc_prev = t2_inc_cur;
    }
}

void ff_aac_tableinit(void)
{
    static pthread_once_t init_static_once = 
                                    0
                                                ;
    pthread_once(&init_static_once, aac_tableinit);
}
typedef struct ParseContext{
    uint8_t *buffer;
    int index;
    int last_index;
    unsigned int buffer_size;
    uint32_t state;
    int frame_start_found;
    int overread;
    int overread_index;
    uint64_t state64;
} ParseContext;
int ff_combine_frame(ParseContext *pc, int next, const uint8_t **buf, int *buf_size);
int ff_mpeg4video_split(AVCodecContext *avctx, const uint8_t *buf,
                        int buf_size);
void ff_parse_close(AVCodecParserContext *s);







void ff_fetch_timestamp(AVCodecParserContext *s, int off, int remove, int fuzzy);

typedef enum {
    AAC_AC3_PARSE_ERROR_SYNC = -0x1030c0a,
    AAC_AC3_PARSE_ERROR_BSID = -0x2030c0a,
    AAC_AC3_PARSE_ERROR_SAMPLE_RATE = -0x3030c0a,
    AAC_AC3_PARSE_ERROR_FRAME_SIZE = -0x4030c0a,
    AAC_AC3_PARSE_ERROR_FRAME_TYPE = -0x5030c0a,
    AAC_AC3_PARSE_ERROR_CRC = -0x6030c0a,
    AAC_AC3_PARSE_ERROR_CHANNEL_CFG = -0x7030c0a,
} AACAC3ParseError;

typedef struct AACAC3ParseContext {
    ParseContext pc;
    int frame_size;
    int header_size;
    int (*sync)(uint64_t state, struct AACAC3ParseContext *hdr_info,
            int *need_next_header, int *new_frame_start);

    int channels;
    int sample_rate;
    int bit_rate;
    int samples;
    uint64_t channel_layout;
    int service_type;

    int remaining_size;
    uint64_t state;

    int need_next_header;
    int frame_number;
    enum AVCodecID codec_id;
} AACAC3ParseContext;

int ff_aac_ac3_parse(AVCodecParserContext *s1,
                     AVCodecContext *avctx,
                     const uint8_t **poutbuf, int *poutbuf_size,
                     const uint8_t *buf, int buf_size);
typedef struct AACADTSHeaderInfo {
    uint32_t sample_rate;
    uint32_t samples;
    uint32_t bit_rate;
    uint8_t crc_absent;
    uint8_t object_type;
    uint8_t sampling_index;
    uint8_t chan_config;
    uint8_t num_aac_frames;
} AACADTSHeaderInfo;
int ff_adts_header_parse(GetBitContext *gbc, AACADTSHeaderInfo *hdr);
int av_adts_header_parse(const uint8_t *buf, uint32_t *samples,
                         uint8_t *frames);



int ff_adts_header_parse(GetBitContext *gbc, AACADTSHeaderInfo *hdr)
{
    int size, rdb, ch, sr;
    int aot, crc_abs;

    if (get_bits(gbc, 12) != 0xfff)
        return AAC_AC3_PARSE_ERROR_SYNC;

    skip_bits1(gbc);
    skip_bits(gbc, 2);
    crc_abs = get_bits1(gbc);
    aot = get_bits(gbc, 2);
    sr = get_bits(gbc, 4);
    if (!avpriv_mpeg4audio_sample_rates[sr])
        return AAC_AC3_PARSE_ERROR_SAMPLE_RATE;
    skip_bits1(gbc);
    ch = get_bits(gbc, 3);

    skip_bits1(gbc);
    skip_bits1(gbc);


    skip_bits1(gbc);
    skip_bits1(gbc);
    size = get_bits(gbc, 13);
    if (size < 7)
        return AAC_AC3_PARSE_ERROR_FRAME_SIZE;

    skip_bits(gbc, 11);
    rdb = get_bits(gbc, 2);

    hdr->object_type = aot + 1;
    hdr->chan_config = ch;
    hdr->crc_absent = crc_abs;
    hdr->num_aac_frames = rdb + 1;
    hdr->sampling_index = sr;
    hdr->sample_rate = avpriv_mpeg4audio_sample_rates[sr];
    hdr->samples = (rdb + 1) * 1024;
    hdr->bit_rate = size * 8 * hdr->sample_rate / hdr->samples;

    return size;
}
static int parse_config_ALS(GetBitContext *gb, MPEG4AudioConfig *c, void *logctx)
{
    if (get_bits_left(gb) < 112)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    if (get_bits_long(gb, 32) != (('\0') | (('S') << 8) | (('L') << 16) | ((unsigned)('A') << 24)))
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));



    c->sample_rate = get_bits_long(gb, 32);

    if (c->sample_rate <= 0) {
        av_log(logctx, 16, "Invalid sample rate %d\n", c->sample_rate);
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }


    skip_bits_long(gb, 32);


    c->chan_config = 0;
    c->channels = get_bits(gb, 16) + 1;

    return 0;
}



const int avpriv_mpeg4audio_sample_rates[16] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025, 8000, 7350
};

const uint8_t ff_mpeg4audio_channels[14] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    8,
    0,
    0,
    0,
    7,
    8,
    24
};

static inline int get_object_type(GetBitContext *gb)
{
    int object_type = get_bits(gb, 5);
    if (object_type == AOT_ESCAPE)
        object_type = 32 + get_bits(gb, 6);
    return object_type;
}

static inline int get_sample_rate(GetBitContext *gb, int *index)
{
    *index = get_bits(gb, 4);
    return *index == 0x0f ? get_bits(gb, 24) :
        avpriv_mpeg4audio_sample_rates[*index];
}

int ff_mpeg4audio_get_config_gb(MPEG4AudioConfig *c, GetBitContext *gb,
                                int sync_extension, void *logctx)
{
    int specific_config_bitindex, ret;
    int start_bit_index = get_bits_count(gb);
    c->object_type = get_object_type(gb);
    c->sample_rate = get_sample_rate(gb, &c->sampling_index);
    c->chan_config = get_bits(gb, 4);
    if (c->chan_config < (sizeof(ff_mpeg4audio_channels) / sizeof((ff_mpeg4audio_channels)[0])))
        c->channels = ff_mpeg4audio_channels[c->chan_config];
    else {
        av_log(logctx, 16, "Invalid chan_config %d\n", c->chan_config);
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }
    c->sbr = -1;
    c->ps = -1;
    if (c->object_type == AOT_SBR || (c->object_type == AOT_PS &&

        !(show_bits(gb, 3) & 0x03 && !(show_bits(gb, 9) & 0x3F)))) {
        if (c->object_type == AOT_PS)
            c->ps = 1;
        c->ext_object_type = AOT_SBR;
        c->sbr = 1;
        c->ext_sample_rate = get_sample_rate(gb, &c->ext_sampling_index);
        c->object_type = get_object_type(gb);
        if (c->object_type == AOT_ER_BSAC)
            c->ext_chan_config = get_bits(gb, 4);
    } else {
        c->ext_object_type = AOT_NULL;
        c->ext_sample_rate = 0;
    }
    specific_config_bitindex = get_bits_count(gb);

    if (c->object_type == AOT_ALS) {
        skip_bits(gb, 5);
        if (show_bits(gb, 24) != (('S') | (('L') << 8) | (('A') << 16) | ((unsigned)('\0') << 24)))
            skip_bits(gb, 24);

        specific_config_bitindex = get_bits_count(gb);

        ret = parse_config_ALS(gb, c, logctx);
        if (ret < 0)
            return ret;
    }

    if (c->ext_object_type != AOT_SBR && sync_extension) {
        while (get_bits_left(gb) > 15) {
            if (show_bits(gb, 11) == 0x2b7) {
                get_bits(gb, 11);
                c->ext_object_type = get_object_type(gb);
                if (c->ext_object_type == AOT_SBR && (c->sbr = get_bits1(gb)) == 1) {
                    c->ext_sample_rate = get_sample_rate(gb, &c->ext_sampling_index);
                    if (c->ext_sample_rate == c->sample_rate)
                        c->sbr = -1;
                }
                if (get_bits_left(gb) > 11 && get_bits(gb, 11) == 0x548)
                    c->ps = get_bits1(gb);
                break;
            } else
                get_bits1(gb);
        }
    }


    if (!c->sbr)
        c->ps = 0;

    if ((c->ps == -1 && c->object_type != AOT_AAC_LC) || c->channels & ~0x01)
        c->ps = 0;

    return specific_config_bitindex - start_bit_index;
}


int avpriv_mpeg4audio_get_config(MPEG4AudioConfig *c, const uint8_t *buf,
                                 int bit_size, int sync_extension)
{
    GetBitContext gb;
    int ret;

    if (bit_size <= 0)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    ret = init_get_bits(&gb, buf, bit_size);
    if (ret < 0)
        return ret;

    return ff_mpeg4audio_get_config_gb(c, &gb, sync_extension, 
                                                              ((void *)0)
                                                                  );
}


int avpriv_mpeg4audio_get_config2(MPEG4AudioConfig *c, const uint8_t *buf,
                                  int size, int sync_extension, void *logctx)
{
    GetBitContext gb;
    int ret;

    if (size <= 0)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    ret = init_get_bits8(&gb, buf, size);
    if (ret < 0)
        return ret;

    return ff_mpeg4audio_get_config_gb(c, &gb, sync_extension, logctx);
}
static float sbr_sum_square_c(float (*x)[2], int n)
{
    float sum0 = 0.0f, sum1 = 0.0f;
    int i;

    for (i = 0; i < n; i += 2)
    {
        sum0 += x[i + 0][0] * x[i + 0][0];
        sum1 += x[i + 0][1] * x[i + 0][1];
        sum0 += x[i + 1][0] * x[i + 1][0];
        sum1 += x[i + 1][1] * x[i + 1][1];
    }

    return sum0 + sum1;
}

static void sbr_neg_odd_64_c(float *x)
{
    union av_intfloat32 *xi = (union av_intfloat32*) x;
    int i;
    for (i = 1; i < 64; i += 4) {
        xi[i + 0].i ^= 1U << 31;
        xi[i + 2].i ^= 1U << 31;
    }
}

static void sbr_qmf_pre_shuffle_c(float *z)
{
    union av_intfloat32 *zi = (union av_intfloat32*) z;
    int k;
    zi[64].i = zi[0].i;
    zi[65].i = zi[1].i;
    for (k = 1; k < 31; k += 2) {
        zi[64 + 2 * k + 0].i = zi[64 - k].i ^ (1U << 31);
        zi[64 + 2 * k + 1].i = zi[ k + 1].i;
        zi[64 + 2 * k + 2].i = zi[63 - k].i ^ (1U << 31);
        zi[64 + 2 * k + 3].i = zi[ k + 2].i;
    }

    zi[64 + 2 * 31 + 0].i = zi[64 - 31].i ^ (1U << 31);
    zi[64 + 2 * 31 + 1].i = zi[31 + 1].i;
}

static void sbr_qmf_post_shuffle_c(float W[32][2], const float *z)
{
    const union av_intfloat32 *zi = (const union av_intfloat32*) z;
    union av_intfloat32 *Wi = (union av_intfloat32*) W;
    int k;
    for (k = 0; k < 32; k += 2) {
        Wi[2 * k + 0].i = zi[63 - k].i ^ (1U << 31);
        Wi[2 * k + 1].i = zi[ k + 0].i;
        Wi[2 * k + 2].i = zi[62 - k].i ^ (1U << 31);
        Wi[2 * k + 3].i = zi[ k + 1].i;
    }
}

static void sbr_qmf_deint_neg_c(float *v, const float *src)
{
    const union av_intfloat32 *si = (const union av_intfloat32*)src;
    union av_intfloat32 *vi = (union av_intfloat32*)v;
    int i;
    for (i = 0; i < 32; i++) {
        vi[ i].i = si[63 - 2 * i ].i;
        vi[63 - i].i = si[63 - 2 * i - 1].i ^ (1U << 31);
    }
}
static void sbr_autocorrelate_c(const float x[40][2], float phi[3][2][2])
{
    float real_sum2 = x[0][0] * x[2][0] + x[0][1] * x[2][1];
    float imag_sum2 = x[0][0] * x[2][1] - x[0][1] * x[2][0];
    float real_sum1 = 0.0f, imag_sum1 = 0.0f, real_sum0 = 0.0f;
    int i;
    for (i = 1; i < 38; i++) {
        real_sum0 += x[i][0] * x[i ][0] + x[i][1] * x[i ][1];
        real_sum1 += x[i][0] * x[i + 1][0] + x[i][1] * x[i + 1][1];
        imag_sum1 += x[i][0] * x[i + 1][1] - x[i][1] * x[i + 1][0];
        real_sum2 += x[i][0] * x[i + 2][0] + x[i][1] * x[i + 2][1];
        imag_sum2 += x[i][0] * x[i + 2][1] - x[i][1] * x[i + 2][0];
    }
    phi[2 - 2][1][0] = real_sum2;
    phi[2 - 2][1][1] = imag_sum2;
    phi[2 ][1][0] = real_sum0 + x[ 0][0] * x[ 0][0] + x[ 0][1] * x[ 0][1];
    phi[1 ][0][0] = real_sum0 + x[38][0] * x[38][0] + x[38][1] * x[38][1];
    phi[2 - 1][1][0] = real_sum1 + x[ 0][0] * x[ 1][0] + x[ 0][1] * x[ 1][1];
    phi[2 - 1][1][1] = imag_sum1 + x[ 0][0] * x[ 1][1] - x[ 0][1] * x[ 1][0];
    phi[0 ][0][0] = real_sum1 + x[38][0] * x[39][0] + x[38][1] * x[39][1];
    phi[0 ][0][1] = imag_sum1 + x[38][0] * x[39][1] - x[38][1] * x[39][0];
}


static void sbr_hf_gen_c(float (*X_high)[2], const float (*X_low)[2],
                         const float alpha0[2], const float alpha1[2],
                         float bw, int start, int end)
{
    float alpha[4];
    int i;

    alpha[0] = alpha1[0] * bw * bw;
    alpha[1] = alpha1[1] * bw * bw;
    alpha[2] = alpha0[0] * bw;
    alpha[3] = alpha0[1] * bw;

    for (i = start; i < end; i++) {
        X_high[i][0] =
            X_low[i - 2][0] * alpha[0] -
            X_low[i - 2][1] * alpha[1] +
            X_low[i - 1][0] * alpha[2] -
            X_low[i - 1][1] * alpha[3] +
            X_low[i][0];
        X_high[i][1] =
            X_low[i - 2][1] * alpha[0] +
            X_low[i - 2][0] * alpha[1] +
            X_low[i - 1][1] * alpha[2] +
            X_low[i - 1][0] * alpha[3] +
            X_low[i][1];
    }
}

static void sbr_hf_g_filt_c(float (*Y)[2], const float (*X_high)[40][2],
                            const float *g_filt, int m_max, intptr_t ixh)
{
    int m;

    for (m = 0; m < m_max; m++) {
        Y[m][0] = X_high[m][ixh][0] * g_filt[m];
        Y[m][1] = X_high[m][ixh][1] * g_filt[m];
    }
}

static __attribute__((always_inline)) inline void sbr_hf_apply_noise(float (*Y)[2],
                                                const float *s_m,
                                                const float *q_filt,
                                                int noise,
                                                float phi_sign0,
                                                float phi_sign1,
                                                int m_max)
{
    int m;

    for (m = 0; m < m_max; m++) {
        float y0 = Y[m][0];
        float y1 = Y[m][1];
        noise = (noise + 1) & 0x1ff;
        if (s_m[m]) {
            y0 += s_m[m] * phi_sign0;
            y1 += s_m[m] * phi_sign1;
        } else {
            y0 += q_filt[m] * ff_sbr_noise_table[noise][0];
            y1 += q_filt[m] * ff_sbr_noise_table[noise][1];
        }
        Y[m][0] = y0;
        Y[m][1] = y1;
        phi_sign1 = -phi_sign1;
    }
}

static void sbr_sum64x5_c(INTFLOAT *z)
{
    int k;
    for (k = 0; k < 64; k++) {
        INTFLOAT f = z[k] + z[k + 64] + z[k + 128] + z[k + 192] + z[k + 256];
        z[k] = f;
    }
}

static void sbr_qmf_deint_bfly_c(INTFLOAT *v, const INTFLOAT *src0, const INTFLOAT *src1)
{
    int i;
    for (i = 0; i < 64; i++) {




        v[ i] = src0[i] - src1[63 - i];
        v[127 - i] = src0[i] + src1[63 - i];

    }
}

static void sbr_hf_apply_noise_0(INTFLOAT (*Y)[2], const AAC_FLOAT *s_m,
                                 const AAC_FLOAT *q_filt, int noise,
                                 int kx, int m_max)
{
    sbr_hf_apply_noise(Y, s_m, q_filt, noise, (INTFLOAT)1.0, (INTFLOAT)0.0, m_max);
}

static void sbr_hf_apply_noise_1(INTFLOAT (*Y)[2], const AAC_FLOAT *s_m,
                                 const AAC_FLOAT *q_filt, int noise,
                                 int kx, int m_max)
{
    INTFLOAT phi_sign = 1 - 2 * (kx & 1);
    sbr_hf_apply_noise(Y, s_m, q_filt, noise, (INTFLOAT)0.0, phi_sign, m_max);
}

static void sbr_hf_apply_noise_2(INTFLOAT (*Y)[2], const AAC_FLOAT *s_m,
                                 const AAC_FLOAT *q_filt, int noise,
                                 int kx, int m_max)
{
    sbr_hf_apply_noise(Y, s_m, q_filt, noise, (INTFLOAT)-1.0, (INTFLOAT)0.0, m_max);
}

static void sbr_hf_apply_noise_3(INTFLOAT (*Y)[2], const AAC_FLOAT *s_m,
                                 const AAC_FLOAT *q_filt, int noise,
                                 int kx, int m_max)
{
    INTFLOAT phi_sign = 1 - 2 * (kx & 1);
    sbr_hf_apply_noise(Y, s_m, q_filt, noise, (INTFLOAT)0.0, -phi_sign, m_max);
}

__attribute__((cold)) void ff_sbrdsp_init(SBRDSPContext *s)
{
    s->sum64x5 = sbr_sum64x5_c;
    s->sum_square = sbr_sum_square_c;
    s->neg_odd_64 = sbr_neg_odd_64_c;
    s->qmf_pre_shuffle = sbr_qmf_pre_shuffle_c;
    s->qmf_post_shuffle = sbr_qmf_post_shuffle_c;
    s->qmf_deint_neg = sbr_qmf_deint_neg_c;
    s->qmf_deint_bfly = sbr_qmf_deint_bfly_c;
    s->autocorrelate = sbr_autocorrelate_c;
    s->hf_gen = sbr_hf_gen_c;
    s->hf_g_filt = sbr_hf_g_filt_c;

    s->hf_apply_noise[0] = sbr_hf_apply_noise_0;
    s->hf_apply_noise[1] = sbr_hf_apply_noise_1;
    s->hf_apply_noise[2] = sbr_hf_apply_noise_2;
    s->hf_apply_noise[3] = sbr_hf_apply_noise_3;


    if (0)
        ff_sbrdsp_init_arm(s);
    if (0)
        ff_sbrdsp_init_aarch64(s);
    if (0)
        ff_sbrdsp_init_x86(s);
    if (0)
        ff_sbrdsp_init_mips(s);

}
static void ps_add_squares_c(INTFLOAT *dst, const INTFLOAT (*src)[2], int n)
{
    int i;
    for (i = 0; i < n; i++)
        dst[i] += (UINTFLOAT)((src[i][0]) * (src[i][0]) + (src[i][1]) * (src[i][1]));
}

static void ps_mul_pair_single_c(INTFLOAT (*dst)[2], INTFLOAT (*src0)[2], INTFLOAT *src1,
                                 int n)
{
    int i;
    for (i = 0; i < n; i++) {
        dst[i][0] = ((src0[i][0]) * (src1[i]));
        dst[i][1] = ((src0[i][1]) * (src1[i]));
    }
}

static void ps_hybrid_analysis_c(INTFLOAT (*out)[2], INTFLOAT (*in)[2],
                                 const INTFLOAT (*filter)[8][2],
                                 ptrdiff_t stride, int n)
{
    int i, j;

    for (i = 0; i < n; i++) {
        INT64FLOAT sum_re = (INT64FLOAT)filter[i][6][0] * in[6][0];
        INT64FLOAT sum_im = (INT64FLOAT)filter[i][6][0] * in[6][1];

        for (j = 0; j < 6; j++) {
            INT64FLOAT in0_re = in[j][0];
            INT64FLOAT in0_im = in[j][1];
            INT64FLOAT in1_re = in[12-j][0];
            INT64FLOAT in1_im = in[12-j][1];
            sum_re += (INT64FLOAT)filter[i][j][0] * (in0_re + in1_re) -
                      (INT64FLOAT)filter[i][j][1] * (in0_im - in1_im);
            sum_im += (INT64FLOAT)filter[i][j][0] * (in0_im + in1_im) +
                      (INT64FLOAT)filter[i][j][1] * (in0_re - in1_re);
        }




        out[i * stride][0] = sum_re;
        out[i * stride][1] = sum_im;

    }
}

static void ps_hybrid_analysis_ileave_c(INTFLOAT (*out)[32][2], INTFLOAT L[2][38][64],
                                      int i, int len)
{
    int j;

    for (; i < 64; i++) {
        for (j = 0; j < len; j++) {
            out[i][j][0] = L[0][j][i];
            out[i][j][1] = L[1][j][i];
        }
    }
}

static void ps_hybrid_synthesis_deint_c(INTFLOAT out[2][38][64],
                                      INTFLOAT (*in)[32][2],
                                      int i, int len)
{
    int n;

    for (; i < 64; i++) {
        for (n = 0; n < len; n++) {
            out[0][n][i] = in[i][n][0];
            out[1][n][i] = in[i][n][1];
        }
    }
}

static void ps_decorrelate_c(INTFLOAT (*out)[2], INTFLOAT (*delay)[2],
                             INTFLOAT (*ap_delay)[32 + 5][2],
                             const INTFLOAT phi_fract[2], const INTFLOAT (*Q_fract)[2],
                             const INTFLOAT *transient_gain,
                             INTFLOAT g_decay_slope,
                             int len)
{
    static const INTFLOAT a[] = { ((float)(0.65143905753106f)),
                               ((float)(0.56471812200776f)),
                               ((float)(0.48954165955695f)) };
    INTFLOAT ag[3];
    int m, n;

    for (m = 0; m < 3; m++)
        ag[m] = ((a[m]) * (g_decay_slope));

    for (n = 0; n < len; n++) {
        INTFLOAT in_re = ((delay[n][0]) * (phi_fract[0]) - (delay[n][1]) * (phi_fract[1]));
        INTFLOAT in_im = ((delay[n][0]) * (phi_fract[1]) + (delay[n][1]) * (phi_fract[0]));
        for (m = 0; m < 3; m++) {
            INTFLOAT a_re = ((ag[m]) * (in_re));
            INTFLOAT a_im = ((ag[m]) * (in_im));
            INTFLOAT link_delay_re = ap_delay[m][n+2-m][0];
            INTFLOAT link_delay_im = ap_delay[m][n+2-m][1];
            INTFLOAT fractional_delay_re = Q_fract[m][0];
            INTFLOAT fractional_delay_im = Q_fract[m][1];
            INTFLOAT apd_re = in_re;
            INTFLOAT apd_im = in_im;
            in_re = ((link_delay_re) * (fractional_delay_re) - (link_delay_im) * (fractional_delay_im))
                                                       ;
            in_re -= (UINTFLOAT)a_re;
            in_im = ((link_delay_re) * (fractional_delay_im) + (link_delay_im) * (fractional_delay_re))
                                                       ;
            in_im -= (UINTFLOAT)a_im;
            ap_delay[m][n+5][0] = apd_re + (UINTFLOAT)((ag[m]) * (in_re));
            ap_delay[m][n+5][1] = apd_im + (UINTFLOAT)((ag[m]) * (in_im));
        }
        out[n][0] = ((transient_gain[n]) * (in_re));
        out[n][1] = ((transient_gain[n]) * (in_im));
    }
}

static void ps_stereo_interpolate_c(INTFLOAT (*l)[2], INTFLOAT (*r)[2],
                                    INTFLOAT h[2][4], INTFLOAT h_step[2][4],
                                    int len)
{
    INTFLOAT h0 = h[0][0];
    INTFLOAT h1 = h[0][1];
    INTFLOAT h2 = h[0][2];
    INTFLOAT h3 = h[0][3];
    UINTFLOAT hs0 = h_step[0][0];
    UINTFLOAT hs1 = h_step[0][1];
    UINTFLOAT hs2 = h_step[0][2];
    UINTFLOAT hs3 = h_step[0][3];
    int n;

    for (n = 0; n < len; n++) {

        INTFLOAT l_re = l[n][0];
        INTFLOAT l_im = l[n][1];
        INTFLOAT r_re = r[n][0];
        INTFLOAT r_im = r[n][1];
        h0 += hs0;
        h1 += hs1;
        h2 += hs2;
        h3 += hs3;
        l[n][0] = ((h0) * (l_re) + (h2) * (r_re));
        l[n][1] = ((h0) * (l_im) + (h2) * (r_im));
        r[n][0] = ((h1) * (l_re) + (h3) * (r_re));
        r[n][1] = ((h1) * (l_im) + (h3) * (r_im));
    }
}

static void ps_stereo_interpolate_ipdopd_c(INTFLOAT (*l)[2], INTFLOAT (*r)[2],
                                           INTFLOAT h[2][4], INTFLOAT h_step[2][4],
                                           int len)
{
    INTFLOAT h00 = h[0][0], h10 = h[1][0];
    INTFLOAT h01 = h[0][1], h11 = h[1][1];
    INTFLOAT h02 = h[0][2], h12 = h[1][2];
    INTFLOAT h03 = h[0][3], h13 = h[1][3];
    UINTFLOAT hs00 = h_step[0][0], hs10 = h_step[1][0];
    UINTFLOAT hs01 = h_step[0][1], hs11 = h_step[1][1];
    UINTFLOAT hs02 = h_step[0][2], hs12 = h_step[1][2];
    UINTFLOAT hs03 = h_step[0][3], hs13 = h_step[1][3];
    int n;

    for (n = 0; n < len; n++) {

        INTFLOAT l_re = l[n][0];
        INTFLOAT l_im = l[n][1];
        INTFLOAT r_re = r[n][0];
        INTFLOAT r_im = r[n][1];
        h00 += hs00;
        h01 += hs01;
        h02 += hs02;
        h03 += hs03;
        h10 += hs10;
        h11 += hs11;
        h12 += hs12;
        h13 += hs13;

        l[n][0] = ((h00) * (l_re) + (h02) * (r_re) - (h10) * (l_im) - (h12) * (r_im));
        l[n][1] = ((h00) * (l_im) + (h02) * (r_im) + (h10) * (l_re) + (h12) * (r_re));
        r[n][0] = ((h01) * (l_re) + (h03) * (r_re) - (h11) * (l_im) - (h13) * (r_im));
        r[n][1] = ((h01) * (l_im) + (h03) * (r_im) + (h11) * (l_re) + (h13) * (r_re));
    }
}

__attribute__((cold)) void ff_psdsp_init(PSDSPContext *s)
{
    s->add_squares = ps_add_squares_c;
    s->mul_pair_single = ps_mul_pair_single_c;
    s->hybrid_analysis = ps_hybrid_analysis_c;
    s->hybrid_analysis_ileave = ps_hybrid_analysis_ileave_c;
    s->hybrid_synthesis_deint = ps_hybrid_synthesis_deint_c;
    s->decorrelate = ps_decorrelate_c;
    s->stereo_interpolate[0] = ps_stereo_interpolate_c;
    s->stereo_interpolate[1] = ps_stereo_interpolate_ipdopd_c;


    if (0)
        ff_psdsp_init_arm(s);
    if (0)
        ff_psdsp_init_aarch64(s);
    if (0)
        ff_psdsp_init_mips(s);
    if (0)
        ff_psdsp_init_x86(s);

}
static const uint8_t huff_iid_df1_bits[] = {
    18, 18, 18, 18, 18, 18, 18, 18, 18, 17, 18, 17, 17, 16, 16, 15, 14, 14,
    13, 12, 12, 11, 10, 10, 8, 7, 6, 5, 4, 3, 1, 3, 4, 5, 6, 7,
     8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 16, 17, 17, 18, 17, 18, 18,
    18, 18, 18, 18, 18, 18, 18,
};

static const uint32_t huff_iid_df1_codes[] = {
    0x01FEB4, 0x01FEB5, 0x01FD76, 0x01FD77, 0x01FD74, 0x01FD75, 0x01FE8A,
    0x01FE8B, 0x01FE88, 0x00FE80, 0x01FEB6, 0x00FE82, 0x00FEB8, 0x007F42,
    0x007FAE, 0x003FAF, 0x001FD1, 0x001FE9, 0x000FE9, 0x0007EA, 0x0007FB,
    0x0003FB, 0x0001FB, 0x0001FF, 0x00007C, 0x00003C, 0x00001C, 0x00000C,
    0x000000, 0x000001, 0x000001, 0x000002, 0x000001, 0x00000D, 0x00001D,
    0x00003D, 0x00007D, 0x0000FC, 0x0001FC, 0x0003FC, 0x0003F4, 0x0007EB,
    0x000FEA, 0x001FEA, 0x001FD6, 0x003FD0, 0x007FAF, 0x007F43, 0x00FEB9,
    0x00FE83, 0x01FEB7, 0x00FE81, 0x01FE89, 0x01FE8E, 0x01FE8F, 0x01FE8C,
    0x01FE8D, 0x01FEB2, 0x01FEB3, 0x01FEB0, 0x01FEB1,
};

static const uint8_t huff_iid_dt1_bits[] = {
    16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 14, 14, 13,
    13, 13, 12, 12, 11, 10, 9, 9, 7, 6, 5, 3, 1, 2, 5, 6, 7, 8,
     9, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16,
};

static const uint16_t huff_iid_dt1_codes[] = {
    0x004ED4, 0x004ED5, 0x004ECE, 0x004ECF, 0x004ECC, 0x004ED6, 0x004ED8,
    0x004F46, 0x004F60, 0x002718, 0x002719, 0x002764, 0x002765, 0x00276D,
    0x0027B1, 0x0013B7, 0x0013D6, 0x0009C7, 0x0009E9, 0x0009ED, 0x0004EE,
    0x0004F7, 0x000278, 0x000139, 0x00009A, 0x00009F, 0x000020, 0x000011,
    0x00000A, 0x000003, 0x000001, 0x000000, 0x00000B, 0x000012, 0x000021,
    0x00004C, 0x00009B, 0x00013A, 0x000279, 0x000270, 0x0004EF, 0x0004E2,
    0x0009EA, 0x0009D8, 0x0013D7, 0x0013D0, 0x0027B2, 0x0027A2, 0x00271A,
    0x00271B, 0x004F66, 0x004F67, 0x004F61, 0x004F47, 0x004ED9, 0x004ED7,
    0x004ECD, 0x004ED2, 0x004ED3, 0x004ED0, 0x004ED1,
};

static const uint8_t huff_iid_df0_bits[] = {
    17, 17, 17, 17, 16, 15, 13, 10, 9, 7, 6, 5, 4, 3, 1, 3, 4, 5,
     6, 6, 8, 11, 13, 14, 14, 15, 17, 18, 18,
};

static const uint32_t huff_iid_df0_codes[] = {
    0x01FFFB, 0x01FFFC, 0x01FFFD, 0x01FFFA, 0x00FFFC, 0x007FFC, 0x001FFD,
    0x0003FE, 0x0001FE, 0x00007E, 0x00003C, 0x00001D, 0x00000D, 0x000005,
    0x000000, 0x000004, 0x00000C, 0x00001C, 0x00003D, 0x00003E, 0x0000FE,
    0x0007FE, 0x001FFC, 0x003FFC, 0x003FFD, 0x007FFD, 0x01FFFE, 0x03FFFE,
    0x03FFFF,
};

static const uint8_t huff_iid_dt0_bits[] = {
    19, 19, 19, 20, 20, 20, 17, 15, 12, 10, 8, 6, 4, 2, 1, 3, 5, 7,
     9, 11, 13, 14, 17, 19, 20, 20, 20, 20, 20,
};

static const uint32_t huff_iid_dt0_codes[] = {
    0x07FFF9, 0x07FFFA, 0x07FFFB, 0x0FFFF8, 0x0FFFF9, 0x0FFFFA, 0x01FFFD,
    0x007FFE, 0x000FFE, 0x0003FE, 0x0000FE, 0x00003E, 0x00000E, 0x000002,
    0x000000, 0x000006, 0x00001E, 0x00007E, 0x0001FE, 0x0007FE, 0x001FFE,
    0x003FFE, 0x01FFFC, 0x07FFF8, 0x0FFFFB, 0x0FFFFC, 0x0FFFFD, 0x0FFFFE,
    0x0FFFFF,
};

static const uint8_t huff_icc_df_bits[] = {
    14, 14, 12, 10, 7, 5, 3, 1, 2, 4, 6, 8, 9, 11, 13,
};

static const uint16_t huff_icc_df_codes[] = {
    0x3FFF, 0x3FFE, 0x0FFE, 0x03FE, 0x007E, 0x001E, 0x0006, 0x0000,
    0x0002, 0x000E, 0x003E, 0x00FE, 0x01FE, 0x07FE, 0x1FFE,
};

static const uint8_t huff_icc_dt_bits[] = {
    14, 13, 11, 9, 7, 5, 3, 1, 2, 4, 6, 8, 10, 12, 14,
};

static const uint16_t huff_icc_dt_codes[] = {
    0x3FFE, 0x1FFE, 0x07FE, 0x01FE, 0x007E, 0x001E, 0x0006, 0x0000,
    0x0002, 0x000E, 0x003E, 0x00FE, 0x03FE, 0x0FFE, 0x3FFF,
};

static const uint8_t huff_ipd_df_bits[] = {
    1, 3, 4, 4, 4, 4, 4, 4,
};

static const uint8_t huff_ipd_df_codes[] = {
    0x01, 0x00, 0x06, 0x04, 0x02, 0x03, 0x05, 0x07,
};

static const uint8_t huff_ipd_dt_bits[] = {
    1, 3, 4, 5, 5, 4, 4, 3,
};

static const uint8_t huff_ipd_dt_codes[] = {
    0x01, 0x02, 0x02, 0x03, 0x02, 0x00, 0x03, 0x03,
};

static const uint8_t huff_opd_df_bits[] = {
    1, 3, 4, 4, 5, 5, 4, 3,
};

static const uint8_t huff_opd_df_codes[] = {
    0x01, 0x01, 0x06, 0x04, 0x0F, 0x0E, 0x05, 0x00,
};

static const uint8_t huff_opd_dt_bits[] = {
    1, 3, 4, 5, 5, 4, 4, 3,
};

static const uint8_t huff_opd_dt_codes[] = {
    0x01, 0x02, 0x01, 0x07, 0x06, 0x00, 0x02, 0x03,
};

static const int8_t huff_offset[] = {
    30, 30,
    14, 14,
    7, 7,
    0, 0,
    0, 0,
};


const int8_t ff_k_to_i_20[] = {
     1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 14, 15,
    15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18,
    18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19
};

const int8_t ff_k_to_i_34[] = {
     0, 1, 2, 3, 4, 5, 6, 6, 7, 2, 1, 0, 10, 10, 4, 5, 6, 7, 8,
     9, 10, 11, 12, 9, 14, 11, 12, 13, 14, 15, 16, 13, 16, 17, 18, 19, 20, 21,
    22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29,
    30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33,
    33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33
};

static const int8_t num_env_tab[2][4] = {
    { 0, 1, 2, 4, },
    { 1, 2, 3, 4, },
};

static const int8_t nr_iidicc_par_tab[] = {
    10, 20, 34, 10, 20, 34,
};

static const int8_t nr_iidopd_par_tab[] = {
     5, 11, 17, 5, 11, 17,
};

enum {
    huff_iid_df1,
    huff_iid_dt1,
    huff_iid_df0,
    huff_iid_dt0,
    huff_icc_df,
    huff_icc_dt,
    huff_ipd_df,
    huff_ipd_dt,
    huff_opd_df,
    huff_opd_dt,
};

static const int huff_iid[] = {
    huff_iid_df0,
    huff_iid_df1,
    huff_iid_dt0,
    huff_iid_dt1,
};

static VLC vlc_ps[10];
static int read_iid_data(AVCodecContext *avctx, GetBitContext *gb, PSCommonContext *ps, int8_t (*iid)[34], int table_idx, int e, int dt) { int b, num = ps->nr_iid_par; int16_t (*vlc_table)[2] = vlc_ps[table_idx].table; if (dt) { int e_prev = e ? e - 1 : ps->num_env_old - 1; e_prev = ((e_prev) > (0) ? (e_prev) : (0)); for (b = 0; b < num; b++) { int val = iid[e_prev][b] + get_vlc2(gb, vlc_table, 9, 3) - huff_offset[table_idx]; if (0) val &= 0; iid[e][b] = val; if (((ps->iid_par[e][b]) >= 0 ? (ps->iid_par[e][b]) : (-(ps->iid_par[e][b]))) > 7 + 8 * ps->iid_quant) goto err; } } else { int val = 0; for (b = 0; b < num; b++) { val += get_vlc2(gb, vlc_table, 9, 3) - huff_offset[table_idx]; if (0) val &= 0; iid[e][b] = val; if (((ps->iid_par[e][b]) >= 0 ? (ps->iid_par[e][b]) : (-(ps->iid_par[e][b]))) > 7 + 8 * ps->iid_quant) goto err; } } return 0; err: av_log(avctx, 16, "illegal ""iid""\n"); return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24))); }
static int read_icc_data(AVCodecContext *avctx, GetBitContext *gb, PSCommonContext *ps, int8_t (*icc)[34], int table_idx, int e, int dt) { int b, num = ps->nr_icc_par; int16_t (*vlc_table)[2] = vlc_ps[table_idx].table; if (dt) { int e_prev = e ? e - 1 : ps->num_env_old - 1; e_prev = ((e_prev) > (0) ? (e_prev) : (0)); for (b = 0; b < num; b++) { int val = icc[e_prev][b] + get_vlc2(gb, vlc_table, 9, 2) - huff_offset[table_idx]; if (0) val &= 0; icc[e][b] = val; if (ps->icc_par[e][b] > 7U) goto err; } } else { int val = 0; for (b = 0; b < num; b++) { val += get_vlc2(gb, vlc_table, 9, 2) - huff_offset[table_idx]; if (0) val &= 0; icc[e][b] = val; if (ps->icc_par[e][b] > 7U) goto err; } } return 0; err: av_log(avctx, 16, "illegal ""icc""\n"); return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24))); }
static int read_ipdopd_data(AVCodecContext *avctx, GetBitContext *gb, PSCommonContext *ps, int8_t (*ipdopd)[34], int table_idx, int e, int dt) { int b, num = ps->nr_ipdopd_par; int16_t (*vlc_table)[2] = vlc_ps[table_idx].table; if (dt) { int e_prev = e ? e - 1 : ps->num_env_old - 1; e_prev = ((e_prev) > (0) ? (e_prev) : (0)); for (b = 0; b < num; b++) { int val = ipdopd[e_prev][b] + get_vlc2(gb, vlc_table, 5, 1) - 0; if (0x07) val &= 0x07; ipdopd[e][b] = val; if (0) goto err; } } else { int val = 0; for (b = 0; b < num; b++) { val += get_vlc2(gb, vlc_table, 5, 1) - 0; if (0x07) val &= 0x07; ipdopd[e][b] = val; if (0) goto err; } } return 0; err: av_log(avctx, 16, "illegal ""ipdopd""\n"); return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24))); }

static int ps_read_extension_data(GetBitContext *gb, PSCommonContext *ps,
                                  int ps_extension_id)
{
    int e;
    int count = get_bits_count(gb);

    if (ps_extension_id)
        return 0;

    ps->enable_ipdopd = get_bits1(gb);
    if (ps->enable_ipdopd) {
        for (e = 0; e < ps->num_env; e++) {
            int dt = get_bits1(gb);
            read_ipdopd_data(
                            ((void *)0)
                                , gb, ps, ps->ipd_par, dt ? huff_ipd_dt : huff_ipd_df, e, dt);
            dt = get_bits1(gb);
            read_ipdopd_data(
                            ((void *)0)
                                , gb, ps, ps->opd_par, dt ? huff_opd_dt : huff_opd_df, e, dt);
        }
    }
    skip_bits1(gb);
    return get_bits_count(gb) - count;
}

int ff_ps_read_data(AVCodecContext *avctx, GetBitContext *gb_host,
                    PSCommonContext *ps, int bits_left)
{
    int e;
    int bit_count_start = get_bits_count(gb_host);
    int header;
    int bits_consumed;
    GetBitContext gbc = *gb_host, *gb = &gbc;

    header = get_bits1(gb);
    if (header) {
        ps->enable_iid = get_bits1(gb);
        if (ps->enable_iid) {
            int iid_mode = get_bits(gb, 3);
            if (iid_mode > 5) {
                av_log(avctx, 16, "iid_mode %d is reserved.\n",
                       iid_mode);
                goto err;
            }
            ps->nr_iid_par = nr_iidicc_par_tab[iid_mode];
            ps->iid_quant = iid_mode > 2;
            ps->nr_ipdopd_par = nr_iidopd_par_tab[iid_mode];
        }
        ps->enable_icc = get_bits1(gb);
        if (ps->enable_icc) {
            ps->icc_mode = get_bits(gb, 3);
            if (ps->icc_mode > 5) {
                av_log(avctx, 16, "icc_mode %d is reserved.\n",
                       ps->icc_mode);
                goto err;
            }
            ps->nr_icc_par = nr_iidicc_par_tab[ps->icc_mode];
        }
        ps->enable_ext = get_bits1(gb);
    }

    ps->frame_class = get_bits1(gb);
    ps->num_env_old = ps->num_env;
    ps->num_env = num_env_tab[ps->frame_class][get_bits(gb, 2)];

    ps->border_position[0] = -1;
    if (ps->frame_class) {
        for (e = 1; e <= ps->num_env; e++) {
            ps->border_position[e] = get_bits(gb, 5);
            if (ps->border_position[e] < ps->border_position[e-1]) {
                av_log(avctx, 16, "border_position non monotone.\n");
                goto err;
            }
        }
    } else
        for (e = 1; e <= ps->num_env; e++)
            ps->border_position[e] = (e * 32 >> ff_log2_tab[ps->num_env]) - 1;

    if (ps->enable_iid) {
        for (e = 0; e < ps->num_env; e++) {
            int dt = get_bits1(gb);
            if (read_iid_data(avctx, gb, ps, ps->iid_par, huff_iid[2*dt+ps->iid_quant], e, dt))
                goto err;
        }
    } else
        memset(ps->iid_par, 0, sizeof(ps->iid_par));

    if (ps->enable_icc)
        for (e = 0; e < ps->num_env; e++) {
            int dt = get_bits1(gb);
            if (read_icc_data(avctx, gb, ps, ps->icc_par, dt ? huff_icc_dt : huff_icc_df, e, dt))
                goto err;
        }
    else
        memset(ps->icc_par, 0, sizeof(ps->icc_par));

    if (ps->enable_ext) {
        int cnt = get_bits(gb, 4);
        if (cnt == 15) {
            cnt += get_bits(gb, 8);
        }
        cnt *= 8;
        while (cnt > 7) {
            int ps_extension_id = get_bits(gb, 2);
            cnt -= 2 + ps_read_extension_data(gb, ps, ps_extension_id);
        }
        if (cnt < 0) {
            av_log(avctx, 16, "ps extension overflow %d\n", cnt);
            goto err;
        }
        skip_bits(gb, cnt);
    }

    ps->enable_ipdopd &= !0;


    if (!ps->num_env || ps->border_position[ps->num_env] < 32 - 1) {

        int source = ps->num_env ? ps->num_env - 1 : ps->num_env_old - 1;
        int b;
        if (source >= 0 && source != ps->num_env) {
            if (ps->enable_iid) {
                memcpy(ps->iid_par+ps->num_env, ps->iid_par+source, sizeof(ps->iid_par[0]));
            }
            if (ps->enable_icc) {
                memcpy(ps->icc_par+ps->num_env, ps->icc_par+source, sizeof(ps->icc_par[0]));
            }
            if (ps->enable_ipdopd) {
                memcpy(ps->ipd_par+ps->num_env, ps->ipd_par+source, sizeof(ps->ipd_par[0]));
                memcpy(ps->opd_par+ps->num_env, ps->opd_par+source, sizeof(ps->opd_par[0]));
            }
        }
        if (ps->enable_iid){
            for (b = 0; b < ps->nr_iid_par; b++) {
                if (((ps->iid_par[ps->num_env][b]) >= 0 ? (ps->iid_par[ps->num_env][b]) : (-(ps->iid_par[ps->num_env][b]))) > 7 + 8 * ps->iid_quant) {
                    av_log(avctx, 16, "iid_par invalid\n");
                    goto err;
                }
            }
        }
        if (ps->enable_icc){
            for (b = 0; b < ps->nr_iid_par; b++) {
                if (ps->icc_par[ps->num_env][b] > 7U) {
                    av_log(avctx, 16, "icc_par invalid\n");
                    goto err;
                }
            }
        }
        ps->num_env++;
        ps->border_position[ps->num_env] = 32 - 1;
    }


    ps->is34bands_old = ps->is34bands;
    if (!0 && (ps->enable_iid || ps->enable_icc))
        ps->is34bands = (ps->enable_iid && ps->nr_iid_par == 34) ||
                        (ps->enable_icc && ps->nr_icc_par == 34);


    if (!ps->enable_ipdopd) {
        memset(ps->ipd_par, 0, sizeof(ps->ipd_par));
        memset(ps->opd_par, 0, sizeof(ps->opd_par));
    }

    if (header)
        ps->start = 1;

    bits_consumed = get_bits_count(gb) - bit_count_start;
    if (bits_consumed <= bits_left) {
        skip_bits_long(gb_host, bits_consumed);
        return bits_consumed;
    }
    av_log(avctx, 16, "Expected to read %d PS bits actually read %d.\n", bits_left, bits_consumed);
err:
    ps->start = 0;
    skip_bits_long(gb_host, bits_left);
    memset(ps->iid_par, 0, sizeof(ps->iid_par));
    memset(ps->icc_par, 0, sizeof(ps->icc_par));
    memset(ps->ipd_par, 0, sizeof(ps->ipd_par));
    memset(ps->opd_par, 0, sizeof(ps->opd_par));
    return bits_left;
}
static __attribute__((cold)) void ps_init_common(void)
{

    static const struct {
        const void *ps_codes, *ps_bits;
        const unsigned int table_size, elem_size;
    } ps_tmp[] = {
        { huff_iid_df1_codes, huff_iid_df1_bits, sizeof(huff_iid_df1_codes), sizeof(huff_iid_df1_codes[0]) },
        { huff_iid_dt1_codes, huff_iid_dt1_bits, sizeof(huff_iid_dt1_codes), sizeof(huff_iid_dt1_codes[0]) },
        { huff_iid_df0_codes, huff_iid_df0_bits, sizeof(huff_iid_df0_codes), sizeof(huff_iid_df0_codes[0]) },
        { huff_iid_dt0_codes, huff_iid_dt0_bits, sizeof(huff_iid_dt0_codes), sizeof(huff_iid_dt0_codes[0]) },
        { huff_icc_df_codes, huff_icc_df_bits, sizeof(huff_icc_df_codes), sizeof(huff_icc_df_codes[0]) },
        { huff_icc_dt_codes, huff_icc_dt_bits, sizeof(huff_icc_dt_codes), sizeof(huff_icc_dt_codes[0]) },
        { huff_ipd_df_codes, huff_ipd_df_bits, sizeof(huff_ipd_df_codes), sizeof(huff_ipd_df_codes[0]) },
        { huff_ipd_dt_codes, huff_ipd_dt_bits, sizeof(huff_ipd_dt_codes), sizeof(huff_ipd_dt_codes[0]) },
        { huff_opd_df_codes, huff_opd_df_bits, sizeof(huff_opd_df_codes), sizeof(huff_opd_df_codes[0]) },
        { huff_opd_dt_codes, huff_opd_dt_bits, sizeof(huff_opd_dt_codes), sizeof(huff_opd_dt_codes[0]) },
    };

    do { static int16_t table[1544][2]; (&vlc_ps[0])->table = table; (&vlc_ps[0])->table_allocated = 1544; ff_init_vlc_sparse(&vlc_ps[0], 9, ps_tmp[0].table_size / ps_tmp[0].elem_size, ps_tmp[0].ps_bits, 1, 1, ps_tmp[0].ps_codes, ps_tmp[0].elem_size, ps_tmp[0].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[832][2]; (&vlc_ps[1])->table = table; (&vlc_ps[1])->table_allocated = 832; ff_init_vlc_sparse(&vlc_ps[1], 9, ps_tmp[1].table_size / ps_tmp[1].elem_size, ps_tmp[1].ps_bits, 1, 1, ps_tmp[1].ps_codes, ps_tmp[1].elem_size, ps_tmp[1].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[1024][2]; (&vlc_ps[2])->table = table; (&vlc_ps[2])->table_allocated = 1024; ff_init_vlc_sparse(&vlc_ps[2], 9, ps_tmp[2].table_size / ps_tmp[2].elem_size, ps_tmp[2].ps_bits, 1, 1, ps_tmp[2].ps_codes, ps_tmp[2].elem_size, ps_tmp[2].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[1036][2]; (&vlc_ps[3])->table = table; (&vlc_ps[3])->table_allocated = 1036; ff_init_vlc_sparse(&vlc_ps[3], 9, ps_tmp[3].table_size / ps_tmp[3].elem_size, ps_tmp[3].ps_bits, 1, 1, ps_tmp[3].ps_codes, ps_tmp[3].elem_size, ps_tmp[3].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[544][2]; (&vlc_ps[4])->table = table; (&vlc_ps[4])->table_allocated = 544; ff_init_vlc_sparse(&vlc_ps[4], 9, ps_tmp[4].table_size / ps_tmp[4].elem_size, ps_tmp[4].ps_bits, 1, 1, ps_tmp[4].ps_codes, ps_tmp[4].elem_size, ps_tmp[4].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[544][2]; (&vlc_ps[5])->table = table; (&vlc_ps[5])->table_allocated = 544; ff_init_vlc_sparse(&vlc_ps[5], 9, ps_tmp[5].table_size / ps_tmp[5].elem_size, ps_tmp[5].ps_bits, 1, 1, ps_tmp[5].ps_codes, ps_tmp[5].elem_size, ps_tmp[5].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[32][2]; (&vlc_ps[6])->table = table; (&vlc_ps[6])->table_allocated = 32; ff_init_vlc_sparse(&vlc_ps[6], 5, ps_tmp[6].table_size / ps_tmp[6].elem_size, ps_tmp[6].ps_bits, 1, 1, ps_tmp[6].ps_codes, ps_tmp[6].elem_size, ps_tmp[6].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[32][2]; (&vlc_ps[7])->table = table; (&vlc_ps[7])->table_allocated = 32; ff_init_vlc_sparse(&vlc_ps[7], 5, ps_tmp[7].table_size / ps_tmp[7].elem_size, ps_tmp[7].ps_bits, 1, 1, ps_tmp[7].ps_codes, ps_tmp[7].elem_size, ps_tmp[7].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[32][2]; (&vlc_ps[8])->table = table; (&vlc_ps[8])->table_allocated = 32; ff_init_vlc_sparse(&vlc_ps[8], 5, ps_tmp[8].table_size / ps_tmp[8].elem_size, ps_tmp[8].ps_bits, 1, 1, ps_tmp[8].ps_codes, ps_tmp[8].elem_size, ps_tmp[8].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
    do { static int16_t table[32][2]; (&vlc_ps[9])->table = table; (&vlc_ps[9])->table_allocated = 32; ff_init_vlc_sparse(&vlc_ps[9], 5, ps_tmp[9].table_size / ps_tmp[9].elem_size, ps_tmp[9].ps_bits, 1, 1, ps_tmp[9].ps_codes, ps_tmp[9].elem_size, ps_tmp[9].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);;
}

__attribute__((cold)) void ff_ps_init_common(void)
{
    static pthread_once_t init_static_once = 
                                    0
                                                ;
    pthread_once(&init_static_once, ps_init_common);
}




void avpriv_report_missing_feature(void *avc,
                                   const char *msg, ...) __attribute__((__format__(__printf__, 2, 3)));
void avpriv_request_sample(void *avc,
                           const char *msg, ...) __attribute__((__format__(__printf__, 2, 3)));
static __attribute__((always_inline)) inline __attribute__((const)) int64_t ff_rint64_clip(double a, int64_t amin, int64_t amax)
{
    int64_t res;





    if (a >= 9223372036854775808.0)
        return amax;
    if (a <= -9223372036854775808.0)
        return amin;


    res = llrint(a);
    if (res > amax)
        return amax;
    if (res < amin)
        return amin;
    return res;
}




__attribute__((warn_unused_result))
int avpriv_open(const char *filename, int flags, ...);
int avpriv_tempfile(const char *prefix, char **filename, int log_offset, void *log_ctx);

int avpriv_set_systematic_pal2(uint32_t pal[256], enum AVPixelFormat pix_fmt);

static __attribute__((always_inline)) inline __attribute__((const)) int avpriv_mirror(int x, int w)
{
    if (!w)
        return 0;

    while ((unsigned)x > (unsigned)w) {
        x = -x;
        if (x < 0)
            x += 2 * w;
    }
    return x;
}

void ff_check_pixfmt_descriptors(void);
int avpriv_dict_set_timestamp(AVDictionary **dict, const char *key, int64_t timestamp);

static float pd_re_smooth[8*8*8];
static float pd_im_smooth[8*8*8];
static float HA[46][8][4];
static float HB[46][8][4];
static float __attribute__ ((aligned (16))) f20_0_8 [ 8][8][2];
static float __attribute__ ((aligned (16))) f34_0_12[12][8][2];
static float __attribute__ ((aligned (16))) f34_1_8 [ 8][8][2];
static float __attribute__ ((aligned (16))) f34_2_4 [ 4][8][2];
static float __attribute__ ((aligned (16))) Q_fract_allpass[2][50][3][2];
static float __attribute__ ((aligned (16))) phi_fract[2][50][2];

static const float g0_Q8[] = {
    0.00746082949812f, 0.02270420949825f, 0.04546865930473f, 0.07266113929591f,
    0.09885108575264f, 0.11793710567217f, 0.125f
};

static const float g0_Q12[] = {
    0.04081179924692f, 0.03812810994926f, 0.05144908135699f, 0.06399831151592f,
    0.07428313801106f, 0.08100347892914f, 0.08333333333333f
};

static const float g1_Q8[] = {
    0.01565675600122f, 0.03752716391991f, 0.05417891378782f, 0.08417044116767f,
    0.10307344158036f, 0.12222452249753f, 0.125f
};

static const float g2_Q4[] = {
    -0.05908211155639f, -0.04871498374946f, 0.0f, 0.07778723915851f,
     0.16486303567403f, 0.23279856662996f, 0.25f
};

static __attribute__((cold)) void make_filters_from_proto(float (*filter)[8][2], const float *proto, int bands)
{
    int q, n;
    for (q = 0; q < bands; q++) {
        for (n = 0; n < 7; n++) {
            double theta = 2 * 
                              3.14159265358979323846 
                                   * (q + 0.5) * (n - 6) / bands;
            filter[q][n][0] = proto[n] * cos(theta);
            filter[q][n][1] = proto[n] * -sin(theta);
        }
    }
}

static __attribute__((cold)) void ps_tableinit(void)
{
    static const float ipdopd_sin[] = { 0, 
                                          0.70710678118654752440
                                                   , 1, 
                                                         0.70710678118654752440
                                                                  , 0, -
                                                                         0.70710678118654752440
                                                                                  , -1, -
                                                                                         0.70710678118654752440 
                                                                                                   };
    static const float ipdopd_cos[] = { 1, 
                                          0.70710678118654752440
                                                   , 0, -
                                                         0.70710678118654752440
                                                                  , -1, -
                                                                         0.70710678118654752440
                                                                                  , 0, 
                                                                                         0.70710678118654752440 
                                                                                                   };
    int pd0, pd1, pd2;

    static const float iid_par_dequant[] = {

        0.05623413251903, 0.12589254117942, 0.19952623149689, 0.31622776601684,
        0.44668359215096, 0.63095734448019, 0.79432823472428, 1,
        1.25892541179417, 1.58489319246111, 2.23872113856834, 3.16227766016838,
        5.01187233627272, 7.94328234724282, 17.7827941003892,

        0.00316227766017, 0.00562341325190, 0.01, 0.01778279410039,
        0.03162277660168, 0.05623413251903, 0.07943282347243, 0.11220184543020,
        0.15848931924611, 0.22387211385683, 0.31622776601684, 0.39810717055350,
        0.50118723362727, 0.63095734448019, 0.79432823472428, 1,
        1.25892541179417, 1.58489319246111, 1.99526231496888, 2.51188643150958,
        3.16227766016838, 4.46683592150963, 6.30957344480193, 8.91250938133745,
        12.5892541179417, 17.7827941003892, 31.6227766016838, 56.2341325190349,
        100, 177.827941003892, 316.227766016837,
    };
    static const float icc_invq[] = {
        1, 0.937, 0.84118, 0.60092, 0.36764, 0, -0.589, -1
    };
    static const float acos_icc_invq[] = {
        0, 0.35685527, 0.57133466, 0.92614472, 1.1943263, 
                                                         3.14159265358979323846
                                                             /2, 2.2006171, 
                                                                            3.14159265358979323846
    
   };
    int iid, icc;

    int k, m;
    static const int8_t f_center_20[] = {
        -3, -1, 1, 3, 5, 7, 10, 14, 18, 22,
    };
    static const int8_t f_center_34[] = {
         2, 6, 10, 14, 18, 22, 26, 30,
        34,-10, -6, -2, 51, 57, 15, 21,
        27, 33, 39, 45, 54, 66, 78, 42,
       102, 66, 78, 90,102,114,126, 90,
    };
    static const float fractional_delay_links[] = { 0.43f, 0.75f, 0.347f };
    const float fractional_delay_gain = 0.39f;

    for (pd0 = 0; pd0 < 8; pd0++) {
        float pd0_re = ipdopd_cos[pd0];
        float pd0_im = ipdopd_sin[pd0];
        for (pd1 = 0; pd1 < 8; pd1++) {
            float pd1_re = ipdopd_cos[pd1];
            float pd1_im = ipdopd_sin[pd1];
            for (pd2 = 0; pd2 < 8; pd2++) {
                float pd2_re = ipdopd_cos[pd2];
                float pd2_im = ipdopd_sin[pd2];
                float re_smooth = 0.25f * pd0_re + 0.5f * pd1_re + pd2_re;
                float im_smooth = 0.25f * pd0_im + 0.5f * pd1_im + pd2_im;
                float pd_mag = 1 / hypot(im_smooth, re_smooth);
                pd_re_smooth[pd0*64+pd1*8+pd2] = re_smooth * pd_mag;
                pd_im_smooth[pd0*64+pd1*8+pd2] = im_smooth * pd_mag;
            }
        }
    }

    for (iid = 0; iid < 46; iid++) {
        float c = iid_par_dequant[iid];
        float c1 = (float)
                         1.41421356237309504880 
                                 / sqrtf(1.0f + c*c);
        float c2 = c * c1;
        for (icc = 0; icc < 8; icc++) {
                                                     {
                float alpha = 0.5f * acos_icc_invq[icc];
                float beta = alpha * (c1 - c2) * (float)
                                                        0.70710678118654752440
                                                                 ;
                HA[iid][icc][0] = c2 * cosf(beta + alpha);
                HA[iid][icc][1] = c1 * cosf(beta - alpha);
                HA[iid][icc][2] = c2 * sinf(beta + alpha);
                HA[iid][icc][3] = c1 * sinf(beta - alpha);
            } {
                float alpha, gamma, mu, rho;
                float alpha_c, alpha_s, gamma_c, gamma_s;
                rho = ((icc_invq[icc]) > (0.05f) ? (icc_invq[icc]) : (0.05f));
                alpha = 0.5f * atan2f(2.0f * c * rho, c*c - 1.0f);
                mu = c + 1.0f / c;
                mu = sqrtf(1 + (4 * rho * rho - 4)/(mu * mu));
                gamma = atanf(sqrtf((1.0f - mu)/(1.0f + mu)));
                if (alpha < 0) alpha += 
                                       3.14159265358979323846
                                           /2;
                alpha_c = cosf(alpha);
                alpha_s = sinf(alpha);
                gamma_c = cosf(gamma);
                gamma_s = sinf(gamma);
                HB[iid][icc][0] = 
                                  1.41421356237309504880 
                                          * alpha_c * gamma_c;
                HB[iid][icc][1] = 
                                  1.41421356237309504880 
                                          * alpha_s * gamma_c;
                HB[iid][icc][2] = -
                                  1.41421356237309504880 
                                          * alpha_s * gamma_s;
                HB[iid][icc][3] = 
                                  1.41421356237309504880 
                                          * alpha_c * gamma_s;
            }
        }
    }

    for (k = 0; k < 30; k++) {
        double f_center, theta;
        if (k < (sizeof(f_center_20) / sizeof((f_center_20)[0])))
            f_center = f_center_20[k] * 0.125;
        else
            f_center = k - 6.5f;
        for (m = 0; m < 3; m++) {
            theta = -
                    3.14159265358979323846 
                         * fractional_delay_links[m] * f_center;
            Q_fract_allpass[0][k][m][0] = cos(theta);
            Q_fract_allpass[0][k][m][1] = sin(theta);
        }
        theta = -
                3.14159265358979323846
                    *fractional_delay_gain*f_center;
        phi_fract[0][k][0] = cos(theta);
        phi_fract[0][k][1] = sin(theta);
    }
    for (k = 0; k < 50; k++) {
        double f_center, theta;
        if (k < (sizeof(f_center_34) / sizeof((f_center_34)[0])))
            f_center = f_center_34[k] / 24.0;
        else
            f_center = k - 26.5f;
        for (m = 0; m < 3; m++) {
            theta = -
                    3.14159265358979323846 
                         * fractional_delay_links[m] * f_center;
            Q_fract_allpass[1][k][m][0] = cos(theta);
            Q_fract_allpass[1][k][m][1] = sin(theta);
        }
        theta = -
                3.14159265358979323846
                    *fractional_delay_gain*f_center;
        phi_fract[1][k][0] = cos(theta);
        phi_fract[1][k][1] = sin(theta);
    }

    make_filters_from_proto(f20_0_8, g0_Q8, 8);
    make_filters_from_proto(f34_0_12, g0_Q12, 12);
    make_filters_from_proto(f34_1_8, g1_Q8, 8);
    make_filters_from_proto(f34_2_4, g2_Q4, 4);
}


static const INTFLOAT g1_Q2[] = {
    ((float)(0.0f)), ((float)(0.01899487526049f)), ((float)(0.0f)), ((float)(-0.07293139167538f)),
    ((float)(0.0f)), ((float)(0.30596630545168f)), ((float)(0.5f))
};

static void ipdopd_reset(int8_t *ipd_hist, int8_t *opd_hist)
{
    int i;
    for (i = 0; i < 17; i++) {
        opd_hist[i] = 0;
        ipd_hist[i] = 0;
    }
}



static void hybrid2_re(INTFLOAT (*in)[2], INTFLOAT (*out)[32][2], const INTFLOAT filter[8], int len, int reverse)
{
    int i, j;
    for (i = 0; i < len; i++, in++) {
        INT64FLOAT re_in = ((filter[6]) * (in[6][0]));
        INT64FLOAT re_op = 0.0f;
        INT64FLOAT im_in = ((filter[6]) * (in[6][1]));
        INT64FLOAT im_op = 0.0f;
        for (j = 0; j < 6; j += 2) {
            re_op += (INT64FLOAT)filter[j+1] * (in[j+1][0] + in[12-j-1][0]);
            im_op += (INT64FLOAT)filter[j+1] * (in[j+1][1] + in[12-j-1][1]);
        }






        out[ reverse][i][0] = (INTFLOAT)(re_in + re_op);
        out[ reverse][i][1] = (INTFLOAT)(im_in + im_op);
        out[!reverse][i][0] = (INTFLOAT)(re_in - re_op);
        out[!reverse][i][1] = (INTFLOAT)(im_in - im_op);
    }
}


static void hybrid6_cx(PSDSPContext *dsp, INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                       INTFLOAT (*filter)[8][2], int len)
{
    int i;
    int N = 8;
    INTFLOAT __attribute__ ((aligned (16))) la_temp [8] [2]; INTFLOAT (*temp) [2] = la_temp;

    for (i = 0; i < len; i++, in++) {
        dsp->hybrid_analysis(temp, in, (const INTFLOAT (*)[8][2]) filter, 1, N);
        out[0][i][0] = temp[6][0];
        out[0][i][1] = temp[6][1];
        out[1][i][0] = temp[7][0];
        out[1][i][1] = temp[7][1];
        out[2][i][0] = temp[0][0];
        out[2][i][1] = temp[0][1];
        out[3][i][0] = temp[1][0];
        out[3][i][1] = temp[1][1];
        out[4][i][0] = temp[2][0] + temp[5][0];
        out[4][i][1] = temp[2][1] + temp[5][1];
        out[5][i][0] = temp[3][0] + temp[4][0];
        out[5][i][1] = temp[3][1] + temp[4][1];
    }
}

static void hybrid4_8_12_cx(PSDSPContext *dsp,
                            INTFLOAT (*in)[2], INTFLOAT (*out)[32][2],
                            INTFLOAT (*filter)[8][2], int N, int len)
{
    int i;

    for (i = 0; i < len; i++, in++) {
        dsp->hybrid_analysis(out[0] + i, in, (const INTFLOAT (*)[8][2]) filter, 32, N);
    }
}

static void hybrid_analysis(PSDSPContext *dsp, INTFLOAT out[91][32][2],
                            INTFLOAT in[5][44][2], INTFLOAT L[2][38][64],
                            int is34, int len)
{
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 38; j++) {
            in[i][j+6][0] = L[0][j][i];
            in[i][j+6][1] = L[1][j][i];
        }
    }
    if (is34) {
        hybrid4_8_12_cx(dsp, in[0], out, f34_0_12, 12, len);
        hybrid4_8_12_cx(dsp, in[1], out+12, f34_1_8, 8, len);
        hybrid4_8_12_cx(dsp, in[2], out+20, f34_2_4, 4, len);
        hybrid4_8_12_cx(dsp, in[3], out+24, f34_2_4, 4, len);
        hybrid4_8_12_cx(dsp, in[4], out+28, f34_2_4, 4, len);
        dsp->hybrid_analysis_ileave(out + 27, L, 5, len);
    } else {
        hybrid6_cx(dsp, in[0], out, f20_0_8, len);
        hybrid2_re(in[1], out+6, g1_Q2, len, 1);
        hybrid2_re(in[2], out+8, g1_Q2, len, 0);
        dsp->hybrid_analysis_ileave(out + 7, L, 3, len);
    }

    for (i = 0; i < 5; i++) {
        memcpy(in[i], in[i]+32, 6 * sizeof(in[i][0]));
    }
}

static void hybrid_synthesis(PSDSPContext *dsp, INTFLOAT out[2][38][64],
                             INTFLOAT in[91][32][2], int is34, int len)
{
    int i, n;
    if (is34) {
        for (n = 0; n < len; n++) {
            memset(out[0][n], 0, 5*sizeof(out[0][n][0]));
            memset(out[1][n], 0, 5*sizeof(out[1][n][0]));
            for (i = 0; i < 12; i++) {
                out[0][n][0] += (UINTFLOAT)in[ i][n][0];
                out[1][n][0] += (UINTFLOAT)in[ i][n][1];
            }
            for (i = 0; i < 8; i++) {
                out[0][n][1] += (UINTFLOAT)in[12+i][n][0];
                out[1][n][1] += (UINTFLOAT)in[12+i][n][1];
            }
            for (i = 0; i < 4; i++) {
                out[0][n][2] += (UINTFLOAT)in[20+i][n][0];
                out[1][n][2] += (UINTFLOAT)in[20+i][n][1];
                out[0][n][3] += (UINTFLOAT)in[24+i][n][0];
                out[1][n][3] += (UINTFLOAT)in[24+i][n][1];
                out[0][n][4] += (UINTFLOAT)in[28+i][n][0];
                out[1][n][4] += (UINTFLOAT)in[28+i][n][1];
            }
        }
        dsp->hybrid_synthesis_deint(out, in + 27, 5, len);
    } else {
        for (n = 0; n < len; n++) {
            out[0][n][0] = (UINTFLOAT)in[0][n][0] + in[1][n][0] + in[2][n][0] +
                           (UINTFLOAT)in[3][n][0] + in[4][n][0] + in[5][n][0];
            out[1][n][0] = (UINTFLOAT)in[0][n][1] + in[1][n][1] + in[2][n][1] +
                           (UINTFLOAT)in[3][n][1] + in[4][n][1] + in[5][n][1];
            out[0][n][1] = (UINTFLOAT)in[6][n][0] + in[7][n][0];
            out[1][n][1] = (UINTFLOAT)in[6][n][1] + in[7][n][1];
            out[0][n][2] = (UINTFLOAT)in[8][n][0] + in[9][n][0];
            out[1][n][2] = (UINTFLOAT)in[8][n][1] + in[9][n][1];
        }
        dsp->hybrid_synthesis_deint(out, in + 7, 3, len);
    }
}




static const int NR_PAR_BANDS[] = { 20, 34 };
static const int NR_IPDOPD_BANDS[] = { 11, 17 };

static const int NR_BANDS[] = { 71, 91 };

static const int DECAY_CUTOFF[] = { 10, 32 };

static const int NR_ALLPASS_BANDS[] = { 30, 50 };

static const int SHORT_DELAY_BAND[] = { 42, 62 };


static void map_idx_10_to_20(int8_t *par_mapped, const int8_t *par, int full)
{
    int b;
    if (full)
        b = 9;
    else {
        b = 4;
        par_mapped[10] = 0;
    }
    for (; b >= 0; b--) {
        par_mapped[2*b+1] = par_mapped[2*b] = par[b];
    }
}

static void map_idx_34_to_20(int8_t *par_mapped, const int8_t *par, int full)
{
    par_mapped[ 0] = (2*par[ 0] + par[ 1]) / 3;
    par_mapped[ 1] = ( par[ 1] + 2*par[ 2]) / 3;
    par_mapped[ 2] = (2*par[ 3] + par[ 4]) / 3;
    par_mapped[ 3] = ( par[ 4] + 2*par[ 5]) / 3;
    par_mapped[ 4] = ( par[ 6] + par[ 7]) / 2;
    par_mapped[ 5] = ( par[ 8] + par[ 9]) / 2;
    par_mapped[ 6] = par[10];
    par_mapped[ 7] = par[11];
    par_mapped[ 8] = ( par[12] + par[13]) / 2;
    par_mapped[ 9] = ( par[14] + par[15]) / 2;
    par_mapped[10] = par[16];
    if (full) {
        par_mapped[11] = par[17];
        par_mapped[12] = par[18];
        par_mapped[13] = par[19];
        par_mapped[14] = ( par[20] + par[21]) / 2;
        par_mapped[15] = ( par[22] + par[23]) / 2;
        par_mapped[16] = ( par[24] + par[25]) / 2;
        par_mapped[17] = ( par[26] + par[27]) / 2;
        par_mapped[18] = ( par[28] + par[29] + par[30] + par[31]) / 4;
        par_mapped[19] = ( par[32] + par[33]) / 2;
    }
}

static void map_val_34_to_20(INTFLOAT par[34])
{
    par[ 0] = (2*par[ 0] + par[ 1]) * 0.33333333f;
    par[ 1] = ( par[ 1] + 2*par[ 2]) * 0.33333333f;
    par[ 2] = (2*par[ 3] + par[ 4]) * 0.33333333f;
    par[ 3] = ( par[ 4] + 2*par[ 5]) * 0.33333333f;

    par[ 4] = ((par[ 6]) + (par[ 7])) * 0.5f;
    par[ 5] = ((par[ 8]) + (par[ 9])) * 0.5f;
    par[ 6] = par[10];
    par[ 7] = par[11];
    par[ 8] = ((par[12]) + (par[13])) * 0.5f;
    par[ 9] = ((par[14]) + (par[15])) * 0.5f;
    par[10] = par[16];
    par[11] = par[17];
    par[12] = par[18];
    par[13] = par[19];
    par[14] = ((par[20]) + (par[21])) * 0.5f;
    par[15] = ((par[22]) + (par[23])) * 0.5f;
    par[16] = ((par[24]) + (par[25])) * 0.5f;
    par[17] = ((par[26]) + (par[27])) * 0.5f;



    par[18] = ( par[28] + par[29] + par[30] + par[31]) * 0.25f;

    par[19] = ((par[32]) + (par[33])) * 0.5f;
}

static void map_idx_10_to_34(int8_t *par_mapped, const int8_t *par, int full)
{
    if (full) {
        par_mapped[33] = par[9];
        par_mapped[32] = par[9];
        par_mapped[31] = par[9];
        par_mapped[30] = par[9];
        par_mapped[29] = par[9];
        par_mapped[28] = par[9];
        par_mapped[27] = par[8];
        par_mapped[26] = par[8];
        par_mapped[25] = par[8];
        par_mapped[24] = par[8];
        par_mapped[23] = par[7];
        par_mapped[22] = par[7];
        par_mapped[21] = par[7];
        par_mapped[20] = par[7];
        par_mapped[19] = par[6];
        par_mapped[18] = par[6];
        par_mapped[17] = par[5];
        par_mapped[16] = par[5];
    } else {
        par_mapped[16] = 0;
    }
    par_mapped[15] = par[4];
    par_mapped[14] = par[4];
    par_mapped[13] = par[4];
    par_mapped[12] = par[4];
    par_mapped[11] = par[3];
    par_mapped[10] = par[3];
    par_mapped[ 9] = par[2];
    par_mapped[ 8] = par[2];
    par_mapped[ 7] = par[2];
    par_mapped[ 6] = par[2];
    par_mapped[ 5] = par[1];
    par_mapped[ 4] = par[1];
    par_mapped[ 3] = par[1];
    par_mapped[ 2] = par[0];
    par_mapped[ 1] = par[0];
    par_mapped[ 0] = par[0];
}

static void map_idx_20_to_34(int8_t *par_mapped, const int8_t *par, int full)
{
    if (full) {
        par_mapped[33] = par[19];
        par_mapped[32] = par[19];
        par_mapped[31] = par[18];
        par_mapped[30] = par[18];
        par_mapped[29] = par[18];
        par_mapped[28] = par[18];
        par_mapped[27] = par[17];
        par_mapped[26] = par[17];
        par_mapped[25] = par[16];
        par_mapped[24] = par[16];
        par_mapped[23] = par[15];
        par_mapped[22] = par[15];
        par_mapped[21] = par[14];
        par_mapped[20] = par[14];
        par_mapped[19] = par[13];
        par_mapped[18] = par[12];
        par_mapped[17] = par[11];
    }
    par_mapped[16] = par[10];
    par_mapped[15] = par[ 9];
    par_mapped[14] = par[ 9];
    par_mapped[13] = par[ 8];
    par_mapped[12] = par[ 8];
    par_mapped[11] = par[ 7];
    par_mapped[10] = par[ 6];
    par_mapped[ 9] = par[ 5];
    par_mapped[ 8] = par[ 5];
    par_mapped[ 7] = par[ 4];
    par_mapped[ 6] = par[ 4];
    par_mapped[ 5] = par[ 3];
    par_mapped[ 4] = (par[ 2] + par[ 3]) / 2;
    par_mapped[ 3] = par[ 2];
    par_mapped[ 2] = par[ 1];
    par_mapped[ 1] = (par[ 0] + par[ 1]) / 2;
    par_mapped[ 0] = par[ 0];
}

static void map_val_20_to_34(INTFLOAT par[34])
{
    par[33] = par[19];
    par[32] = par[19];
    par[31] = par[18];
    par[30] = par[18];
    par[29] = par[18];
    par[28] = par[18];
    par[27] = par[17];
    par[26] = par[17];
    par[25] = par[16];
    par[24] = par[16];
    par[23] = par[15];
    par[22] = par[15];
    par[21] = par[14];
    par[20] = par[14];
    par[19] = par[13];
    par[18] = par[12];
    par[17] = par[11];
    par[16] = par[10];
    par[15] = par[ 9];
    par[14] = par[ 9];
    par[13] = par[ 8];
    par[12] = par[ 8];
    par[11] = par[ 7];
    par[10] = par[ 6];
    par[ 9] = par[ 5];
    par[ 8] = par[ 5];
    par[ 7] = par[ 4];
    par[ 6] = par[ 4];
    par[ 5] = par[ 3];
    par[ 4] = ((par[ 2]) + (par[ 3])) * 0.5f;
    par[ 3] = par[ 2];
    par[ 2] = par[ 1];
    par[ 1] = ((par[ 0]) + (par[ 1])) * 0.5f;
}

static void decorrelation(PSContext *ps, INTFLOAT (*out)[32][2], const INTFLOAT (*s)[32][2], int is34)
{
    INTFLOAT __attribute__ ((aligned (16))) la_power [34] [32]; INTFLOAT (*power) [32] = la_power;
    INTFLOAT __attribute__ ((aligned (16))) la_transient_gain [34] [32]; INTFLOAT (*transient_gain) [32] = la_transient_gain;
    INTFLOAT *peak_decay_nrg = ps->peak_decay_nrg;
    INTFLOAT *power_smooth = ps->power_smooth;
    INTFLOAT *peak_decay_diff_smooth = ps->peak_decay_diff_smooth;
    INTFLOAT (*delay)[32 + 14][2] = ps->delay;
    INTFLOAT (*ap_delay)[3][32 + 5][2] = ps->ap_delay;

    const float transient_impact = 1.5f;
    const float a_smooth = 0.25f;

    const int8_t *const k_to_i = is34 ? ff_k_to_i_34 : ff_k_to_i_20;
    int i, k, m, n;
    int n0 = 0, nL = 32;
    const INTFLOAT peak_decay_factor = ((float)(0.76592833836465f));

    memset(power, 0, 34 * sizeof(*power));

    if (is34 != ps->common.is34bands_old) {
        memset(ps->peak_decay_nrg, 0, sizeof(ps->peak_decay_nrg));
        memset(ps->power_smooth, 0, sizeof(ps->power_smooth));
        memset(ps->peak_decay_diff_smooth, 0, sizeof(ps->peak_decay_diff_smooth));
        memset(ps->delay, 0, sizeof(ps->delay));
        memset(ps->ap_delay, 0, sizeof(ps->ap_delay));
    }

    for (k = 0; k < NR_BANDS[is34]; k++) {
        int i = k_to_i[k];
        ps->dsp.add_squares(power[i], s[k], nL - n0);
    }
    for (i = 0; i < NR_PAR_BANDS[is34]; i++) {
        for (n = n0; n < nL; n++) {
            float decayed_peak = peak_decay_factor * peak_decay_nrg[i];
            float denom;
            peak_decay_nrg[i] = ((decayed_peak) > (power[i][n]) ? (decayed_peak) : (power[i][n]));
            power_smooth[i] += a_smooth * (power[i][n] - power_smooth[i]);
            peak_decay_diff_smooth[i] += a_smooth * (peak_decay_nrg[i] - power[i][n] - peak_decay_diff_smooth[i]);
            denom = transient_impact * peak_decay_diff_smooth[i];
            transient_gain[i][n] = (denom > power_smooth[i]) ?
                                         power_smooth[i] / denom : 1.0f;
        }
    }
    for (k = 0; k < NR_ALLPASS_BANDS[is34]; k++) {
        int b = k_to_i[k];
        float g_decay_slope = 1.f - ((float)(0.05f)) * (k - DECAY_CUTOFF[is34]);
        g_decay_slope = av_clipf_c(g_decay_slope, 0.f, 1.f);

        memcpy(delay[k], delay[k]+nL, 14*sizeof(delay[k][0]));
        memcpy(delay[k]+14, s[k], 32*sizeof(delay[k][0]));
        for (m = 0; m < 3; m++) {
            memcpy(ap_delay[k][m], ap_delay[k][m]+32, 5*sizeof(ap_delay[k][m][0]));
        }
        ps->dsp.decorrelate(out[k], delay[k] + 14 - 2, ap_delay[k],
                            phi_fract[is34][k],
                            (const INTFLOAT (*)[2]) Q_fract_allpass[is34][k],
                            transient_gain[b], g_decay_slope, nL - n0);
    }
    for (; k < SHORT_DELAY_BAND[is34]; k++) {
        int i = k_to_i[k];
        memcpy(delay[k], delay[k]+nL, 14*sizeof(delay[k][0]));
        memcpy(delay[k]+14, s[k], 32*sizeof(delay[k][0]));

        ps->dsp.mul_pair_single(out[k], delay[k] + 14 - 14,
                                transient_gain[i], nL - n0);
    }
    for (; k < NR_BANDS[is34]; k++) {
        int i = k_to_i[k];
        memcpy(delay[k], delay[k]+nL, 14*sizeof(delay[k][0]));
        memcpy(delay[k]+14, s[k], 32*sizeof(delay[k][0]));

        ps->dsp.mul_pair_single(out[k], delay[k] + 14 - 1,
                                transient_gain[i], nL - n0);
    }
}

static void remap34(int8_t (**p_par_mapped)[34],
                    int8_t (*par)[34],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[34] = *p_par_mapped;
    int e;
    if (num_par == 20 || num_par == 11) {
        for (e = 0; e < num_env; e++) {
            map_idx_20_to_34(par_mapped[e], par[e], full);
        }
    } else if (num_par == 10 || num_par == 5) {
        for (e = 0; e < num_env; e++) {
            map_idx_10_to_34(par_mapped[e], par[e], full);
        }
    } else {
        *p_par_mapped = par;
    }
}

static void remap20(int8_t (**p_par_mapped)[34],
                    int8_t (*par)[34],
                    int num_par, int num_env, int full)
{
    int8_t (*par_mapped)[34] = *p_par_mapped;
    int e;
    if (num_par == 34 || num_par == 17) {
        for (e = 0; e < num_env; e++) {
            map_idx_34_to_20(par_mapped[e], par[e], full);
        }
    } else if (num_par == 10 || num_par == 5) {
        for (e = 0; e < num_env; e++) {
            map_idx_10_to_20(par_mapped[e], par[e], full);
        }
    } else {
        *p_par_mapped = par;
    }
}

static void stereo_processing(PSContext *ps, INTFLOAT (*l)[32][2], INTFLOAT (*r)[32][2], int is34)
{
    int e, b, k;

    PSCommonContext *const ps2 = &ps->common;
    INTFLOAT (*H11)[5 +1][34] = ps->H11;
    INTFLOAT (*H12)[5 +1][34] = ps->H12;
    INTFLOAT (*H21)[5 +1][34] = ps->H21;
    INTFLOAT (*H22)[5 +1][34] = ps->H22;
    int8_t *opd_hist = ps->opd_hist;
    int8_t *ipd_hist = ps->ipd_hist;
    int8_t iid_mapped_buf[5][34];
    int8_t icc_mapped_buf[5][34];
    int8_t ipd_mapped_buf[5][34];
    int8_t opd_mapped_buf[5][34];
    int8_t (*iid_mapped)[34] = iid_mapped_buf;
    int8_t (*icc_mapped)[34] = icc_mapped_buf;
    int8_t (*ipd_mapped)[34] = ipd_mapped_buf;
    int8_t (*opd_mapped)[34] = opd_mapped_buf;
    const int8_t *const k_to_i = is34 ? ff_k_to_i_34 : ff_k_to_i_20;
    INTFLOAT (*H_LUT)[8][4] = (0 || ps2->icc_mode < 3) ? HA : HB;


    if (ps2->num_env_old) {
        memcpy(H11[0][0], H11[0][ps2->num_env_old], sizeof(H11[0][0]));
        memcpy(H11[1][0], H11[1][ps2->num_env_old], sizeof(H11[1][0]));
        memcpy(H12[0][0], H12[0][ps2->num_env_old], sizeof(H12[0][0]));
        memcpy(H12[1][0], H12[1][ps2->num_env_old], sizeof(H12[1][0]));
        memcpy(H21[0][0], H21[0][ps2->num_env_old], sizeof(H21[0][0]));
        memcpy(H21[1][0], H21[1][ps2->num_env_old], sizeof(H21[1][0]));
        memcpy(H22[0][0], H22[0][ps2->num_env_old], sizeof(H22[0][0]));
        memcpy(H22[1][0], H22[1][ps2->num_env_old], sizeof(H22[1][0]));
    }

    if (is34) {
        remap34(&iid_mapped, ps2->iid_par, ps2->nr_iid_par, ps2->num_env, 1);
        remap34(&icc_mapped, ps2->icc_par, ps2->nr_icc_par, ps2->num_env, 1);
        if (ps2->enable_ipdopd) {
            remap34(&ipd_mapped, ps2->ipd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
            remap34(&opd_mapped, ps2->opd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
        }
        if (!ps2->is34bands_old) {
            map_val_20_to_34(H11[0][0]);
            map_val_20_to_34(H11[1][0]);
            map_val_20_to_34(H12[0][0]);
            map_val_20_to_34(H12[1][0]);
            map_val_20_to_34(H21[0][0]);
            map_val_20_to_34(H21[1][0]);
            map_val_20_to_34(H22[0][0]);
            map_val_20_to_34(H22[1][0]);
            ipdopd_reset(ipd_hist, opd_hist);
        }
    } else {
        remap20(&iid_mapped, ps2->iid_par, ps2->nr_iid_par, ps2->num_env, 1);
        remap20(&icc_mapped, ps2->icc_par, ps2->nr_icc_par, ps2->num_env, 1);
        if (ps2->enable_ipdopd) {
            remap20(&ipd_mapped, ps2->ipd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
            remap20(&opd_mapped, ps2->opd_par, ps2->nr_ipdopd_par, ps2->num_env, 0);
        }
        if (ps2->is34bands_old) {
            map_val_34_to_20(H11[0][0]);
            map_val_34_to_20(H11[1][0]);
            map_val_34_to_20(H12[0][0]);
            map_val_34_to_20(H12[1][0]);
            map_val_34_to_20(H21[0][0]);
            map_val_34_to_20(H21[1][0]);
            map_val_34_to_20(H22[0][0]);
            map_val_34_to_20(H22[1][0]);
            ipdopd_reset(ipd_hist, opd_hist);
        }
    }


    for (e = 0; e < ps2->num_env; e++) {
        for (b = 0; b < NR_PAR_BANDS[is34]; b++) {
            INTFLOAT h11, h12, h21, h22;
            h11 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][0];
            h12 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][1];
            h21 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][2];
            h22 = H_LUT[iid_mapped[e][b] + 7 + 23 * ps2->iid_quant][icc_mapped[e][b]][3];

            if (!0 && ps2->enable_ipdopd && b < NR_IPDOPD_BANDS[is34]) {


                INTFLOAT h11i, h12i, h21i, h22i;
                INTFLOAT ipd_adj_re, ipd_adj_im;
                int opd_idx = opd_hist[b] * 8 + opd_mapped[e][b];
                int ipd_idx = ipd_hist[b] * 8 + ipd_mapped[e][b];
                INTFLOAT opd_re = pd_re_smooth[opd_idx];
                INTFLOAT opd_im = pd_im_smooth[opd_idx];
                INTFLOAT ipd_re = pd_re_smooth[ipd_idx];
                INTFLOAT ipd_im = pd_im_smooth[ipd_idx];
                opd_hist[b] = opd_idx & 0x3F;
                ipd_hist[b] = ipd_idx & 0x3F;

                ipd_adj_re = ((opd_re) * (ipd_re) + (opd_im) * (ipd_im));
                ipd_adj_im = ((opd_im) * (ipd_re) - (opd_re) * (ipd_im));
                h11i = ((h11) * (opd_im));
                h11 = ((h11) * (opd_re));
                h12i = ((h12) * (ipd_adj_im));
                h12 = ((h12) * (ipd_adj_re));
                h21i = ((h21) * (opd_im));
                h21 = ((h21) * (opd_re));
                h22i = ((h22) * (ipd_adj_im));
                h22 = ((h22) * (ipd_adj_re));
                H11[1][e+1][b] = h11i;
                H12[1][e+1][b] = h12i;
                H21[1][e+1][b] = h21i;
                H22[1][e+1][b] = h22i;
            }
            H11[0][e+1][b] = h11;
            H12[0][e+1][b] = h12;
            H21[0][e+1][b] = h21;
            H22[0][e+1][b] = h22;
        }
        for (k = 0; k < NR_BANDS[is34]; k++) {
            INTFLOAT __attribute__ ((aligned (16))) la_h [2] [4]; INTFLOAT (*h) [4] = la_h;
            INTFLOAT __attribute__ ((aligned (16))) la_h_step [2] [4]; INTFLOAT (*h_step) [4] = la_h_step;
            int start = ps2->border_position[e];
            int stop = ps2->border_position[e+1];
            INTFLOAT width = ((float)(1.f)) / ((stop - start) ? (stop - start) : 1);



            b = k_to_i[k];
            h[0][0] = H11[0][e][b];
            h[0][1] = H12[0][e][b];
            h[0][2] = H21[0][e][b];
            h[0][3] = H22[0][e][b];
            if (!0 && ps2->enable_ipdopd) {

            if ((is34 && k <= 13 && k >= 9) || (!is34 && k <= 1)) {
                h[1][0] = -H11[1][e][b];
                h[1][1] = -H12[1][e][b];
                h[1][2] = -H21[1][e][b];
                h[1][3] = -H22[1][e][b];
            } else {
                h[1][0] = H11[1][e][b];
                h[1][1] = H12[1][e][b];
                h[1][2] = H21[1][e][b];
                h[1][3] = H22[1][e][b];
            }
            }

            h_step[0][0] = ((H11[0][e+1][b]) - (h[0][0])) * (width);
            h_step[0][1] = ((H12[0][e+1][b]) - (h[0][1])) * (width);
            h_step[0][2] = ((H21[0][e+1][b]) - (h[0][2])) * (width);
            h_step[0][3] = ((H22[0][e+1][b]) - (h[0][3])) * (width);
            if (!0 && ps2->enable_ipdopd) {
                h_step[1][0] = ((H11[1][e+1][b]) - (h[1][0])) * (width);
                h_step[1][1] = ((H12[1][e+1][b]) - (h[1][1])) * (width);
                h_step[1][2] = ((H21[1][e+1][b]) - (h[1][2])) * (width);
                h_step[1][3] = ((H22[1][e+1][b]) - (h[1][3])) * (width);
            }
            if (stop - start)
                ps->dsp.stereo_interpolate[!0 && ps2->enable_ipdopd](
                    l[k] + 1 + start, r[k] + 1 + start,
                    h, h_step, stop - start);
        }
    }
}

int ff_ps_apply(AVCodecContext *avctx, PSContext *ps, INTFLOAT L[2][38][64], INTFLOAT R[2][38][64], int top)
{
    INTFLOAT (*Lbuf)[32][2] = ps->Lbuf;
    INTFLOAT (*Rbuf)[32][2] = ps->Rbuf;
    const int len = 32;
    int is34 = ps->common.is34bands;

    top += NR_BANDS[is34] - 64;
    memset(ps->delay+top, 0, (NR_BANDS[is34] - top)*sizeof(ps->delay[0]));
    if (top < NR_ALLPASS_BANDS[is34])
        memset(ps->ap_delay + top, 0, (NR_ALLPASS_BANDS[is34] - top)*sizeof(ps->ap_delay[0]));

    hybrid_analysis(&ps->dsp, Lbuf, ps->in_buf, L, is34, len);
    decorrelation(ps, Rbuf, (const INTFLOAT (*)[32][2]) Lbuf, is34);
    stereo_processing(ps, Lbuf, Rbuf, is34);
    hybrid_synthesis(&ps->dsp, L, Lbuf, is34, len);
    hybrid_synthesis(&ps->dsp, R, Rbuf, is34, len);

    return 0;
}

__attribute__((cold)) void ff_ps_init(void) {
    ps_tableinit();
    ff_ps_init_common();
}

__attribute__((cold)) void ff_ps_ctx_init(PSContext *ps)
{
    ff_psdsp_init(&ps->dsp);
}
enum {
    T_HUFFMAN_ENV_1_5DB,
    F_HUFFMAN_ENV_1_5DB,
    T_HUFFMAN_ENV_BAL_1_5DB,
    F_HUFFMAN_ENV_BAL_1_5DB,
    T_HUFFMAN_ENV_3_0DB,
    F_HUFFMAN_ENV_3_0DB,
    T_HUFFMAN_ENV_BAL_3_0DB,
    F_HUFFMAN_ENV_BAL_3_0DB,
    T_HUFFMAN_NOISE_3_0DB,
    T_HUFFMAN_NOISE_BAL_3_0DB,
};




enum {
    FIXFIX,
    FIXVAR,
    VARFIX,
    VARVAR,
};

enum {
    EXTENSION_ID_PS = 2,
};

static const int8_t vlc_sbr_lav[10] =
    { 60, 60, 24, 24, 31, 31, 12, 12, 31, 12 };
void ff_aac_sbr_init(void);

void ff_aac_sbr_ctx_init(AACContext *ac, SpectralBandReplication *sbr, int id_aac);

void ff_aac_sbr_ctx_close(SpectralBandReplication *sbr);

int ff_decode_sbr_extension(AACContext *ac, SpectralBandReplication *sbr,
                            GetBitContext *gb, int crc, int cnt, int id_aac);

void ff_sbr_apply(AACContext *ac, SpectralBandReplication *sbr, int id_aac,
                  INTFLOAT* L, INTFLOAT *R);

void ff_aacsbr_func_ptr_init_mips(AACSBRContext *c);
static const uint8_t t_huffman_env_1_5dB_bits[121] = {
    18, 18, 18, 18, 18, 18, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 17, 18, 16, 17, 18, 17,
    16, 16, 16, 16, 15, 14, 14, 13,
    13, 12, 11, 10, 9, 8, 7, 6,
     5, 4, 3, 2, 2, 3, 4, 5,
     6, 7, 8, 9, 10, 12, 13, 14,
    14, 15, 16, 17, 16, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19,
};

static const uint32_t t_huffman_env_1_5dB_codes[121] = {
    0x3ffd6, 0x3ffd7, 0x3ffd8, 0x3ffd9, 0x3ffda, 0x3ffdb, 0x7ffb8, 0x7ffb9,
    0x7ffba, 0x7ffbb, 0x7ffbc, 0x7ffbd, 0x7ffbe, 0x7ffbf, 0x7ffc0, 0x7ffc1,
    0x7ffc2, 0x7ffc3, 0x7ffc4, 0x7ffc5, 0x7ffc6, 0x7ffc7, 0x7ffc8, 0x7ffc9,
    0x7ffca, 0x7ffcb, 0x7ffcc, 0x7ffcd, 0x7ffce, 0x7ffcf, 0x7ffd0, 0x7ffd1,
    0x7ffd2, 0x7ffd3, 0x1ffe6, 0x3ffd4, 0x0fff0, 0x1ffe9, 0x3ffd5, 0x1ffe7,
    0x0fff1, 0x0ffec, 0x0ffed, 0x0ffee, 0x07ff4, 0x03ff9, 0x03ff7, 0x01ffa,
    0x01ff9, 0x00ffb, 0x007fc, 0x003fc, 0x001fd, 0x000fd, 0x0007d, 0x0003d,
    0x0001d, 0x0000d, 0x00005, 0x00001, 0x00000, 0x00004, 0x0000c, 0x0001c,
    0x0003c, 0x0007c, 0x000fc, 0x001fc, 0x003fd, 0x00ffa, 0x01ff8, 0x03ff6,
    0x03ff8, 0x07ff5, 0x0ffef, 0x1ffe8, 0x0fff2, 0x7ffd4, 0x7ffd5, 0x7ffd6,
    0x7ffd7, 0x7ffd8, 0x7ffd9, 0x7ffda, 0x7ffdb, 0x7ffdc, 0x7ffdd, 0x7ffde,
    0x7ffdf, 0x7ffe0, 0x7ffe1, 0x7ffe2, 0x7ffe3, 0x7ffe4, 0x7ffe5, 0x7ffe6,
    0x7ffe7, 0x7ffe8, 0x7ffe9, 0x7ffea, 0x7ffeb, 0x7ffec, 0x7ffed, 0x7ffee,
    0x7ffef, 0x7fff0, 0x7fff1, 0x7fff2, 0x7fff3, 0x7fff4, 0x7fff5, 0x7fff6,
    0x7fff7, 0x7fff8, 0x7fff9, 0x7fffa, 0x7fffb, 0x7fffc, 0x7fffd, 0x7fffe,
    0x7ffff,
};

static const uint8_t f_huffman_env_1_5dB_bits[121] = {
    19, 19, 20, 20, 20, 20, 20, 20,
    20, 19, 20, 20, 20, 20, 19, 20,
    19, 19, 20, 18, 20, 20, 20, 19,
    20, 20, 20, 19, 20, 19, 18, 19,
    18, 18, 17, 18, 17, 17, 17, 16,
    16, 16, 15, 15, 14, 13, 13, 12,
    12, 11, 10, 9, 9, 8, 7, 6,
     5, 4, 3, 2, 2, 3, 4, 5,
     6, 8, 8, 9, 10, 11, 11, 11,
    12, 12, 13, 13, 14, 14, 16, 16,
    17, 17, 18, 18, 18, 18, 18, 18,
    18, 20, 19, 20, 20, 20, 20, 20,
    20, 19, 20, 20, 20, 20, 19, 20,
    18, 20, 20, 19, 19, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20,
    20,
};

static const uint32_t f_huffman_env_1_5dB_codes[121] = {
    0x7ffe7, 0x7ffe8, 0xfffd2, 0xfffd3, 0xfffd4, 0xfffd5, 0xfffd6, 0xfffd7,
    0xfffd8, 0x7ffda, 0xfffd9, 0xfffda, 0xfffdb, 0xfffdc, 0x7ffdb, 0xfffdd,
    0x7ffdc, 0x7ffdd, 0xfffde, 0x3ffe4, 0xfffdf, 0xfffe0, 0xfffe1, 0x7ffde,
    0xfffe2, 0xfffe3, 0xfffe4, 0x7ffdf, 0xfffe5, 0x7ffe0, 0x3ffe8, 0x7ffe1,
    0x3ffe0, 0x3ffe9, 0x1ffef, 0x3ffe5, 0x1ffec, 0x1ffed, 0x1ffee, 0x0fff4,
    0x0fff3, 0x0fff0, 0x07ff7, 0x07ff6, 0x03ffa, 0x01ffa, 0x01ff9, 0x00ffa,
    0x00ff8, 0x007f9, 0x003fb, 0x001fc, 0x001fa, 0x000fb, 0x0007c, 0x0003c,
    0x0001c, 0x0000c, 0x00005, 0x00001, 0x00000, 0x00004, 0x0000d, 0x0001d,
    0x0003d, 0x000fa, 0x000fc, 0x001fb, 0x003fa, 0x007f8, 0x007fa, 0x007fb,
    0x00ff9, 0x00ffb, 0x01ff8, 0x01ffb, 0x03ff8, 0x03ff9, 0x0fff1, 0x0fff2,
    0x1ffea, 0x1ffeb, 0x3ffe1, 0x3ffe2, 0x3ffea, 0x3ffe3, 0x3ffe6, 0x3ffe7,
    0x3ffeb, 0xfffe6, 0x7ffe2, 0xfffe7, 0xfffe8, 0xfffe9, 0xfffea, 0xfffeb,
    0xfffec, 0x7ffe3, 0xfffed, 0xfffee, 0xfffef, 0xffff0, 0x7ffe4, 0xffff1,
    0x3ffec, 0xffff2, 0xffff3, 0x7ffe5, 0x7ffe6, 0xffff4, 0xffff5, 0xffff6,
    0xffff7, 0xffff8, 0xffff9, 0xffffa, 0xffffb, 0xffffc, 0xffffd, 0xffffe,
    0xfffff,
};

static const uint8_t t_huffman_env_bal_1_5dB_bits[49] = {
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 12, 11, 9, 7, 5, 3,
     1, 2, 4, 6, 8, 11, 12, 15,
    16, 16, 16, 16, 16, 16, 16, 17,
    17, 17, 17, 17, 17, 17, 17, 17,
    17,
};

static const uint32_t t_huffman_env_bal_1_5dB_codes[49] = {
    0x0ffe4, 0x0ffe5, 0x0ffe6, 0x0ffe7, 0x0ffe8, 0x0ffe9, 0x0ffea, 0x0ffeb,
    0x0ffec, 0x0ffed, 0x0ffee, 0x0ffef, 0x0fff0, 0x0fff1, 0x0fff2, 0x0fff3,
    0x0fff4, 0x0ffe2, 0x00ffc, 0x007fc, 0x001fe, 0x0007e, 0x0001e, 0x00006,
    0x00000, 0x00002, 0x0000e, 0x0003e, 0x000fe, 0x007fd, 0x00ffd, 0x07ff0,
    0x0ffe3, 0x0fff5, 0x0fff6, 0x0fff7, 0x0fff8, 0x0fff9, 0x0fffa, 0x1fff6,
    0x1fff7, 0x1fff8, 0x1fff9, 0x1fffa, 0x1fffb, 0x1fffc, 0x1fffd, 0x1fffe,
    0x1ffff,
};

static const uint8_t f_huffman_env_bal_1_5dB_bits[49] = {
    18, 18, 18, 18, 18, 18, 18, 18,
    18, 18, 18, 18, 18, 18, 18, 16,
    17, 14, 11, 11, 8, 7, 4, 2,
     1, 3, 5, 6, 9, 11, 12, 15,
    16, 18, 18, 18, 18, 18, 18, 18,
    18, 18, 18, 18, 18, 18, 18, 19,
    19,
};

static const uint32_t f_huffman_env_bal_1_5dB_codes[49] = {
    0x3ffe2, 0x3ffe3, 0x3ffe4, 0x3ffe5, 0x3ffe6, 0x3ffe7, 0x3ffe8, 0x3ffe9,
    0x3ffea, 0x3ffeb, 0x3ffec, 0x3ffed, 0x3ffee, 0x3ffef, 0x3fff0, 0x0fff7,
    0x1fff0, 0x03ffc, 0x007fe, 0x007fc, 0x000fe, 0x0007e, 0x0000e, 0x00002,
    0x00000, 0x00006, 0x0001e, 0x0003e, 0x001fe, 0x007fd, 0x00ffe, 0x07ffa,
    0x0fff6, 0x3fff1, 0x3fff2, 0x3fff3, 0x3fff4, 0x3fff5, 0x3fff6, 0x3fff7,
    0x3fff8, 0x3fff9, 0x3fffa, 0x3fffb, 0x3fffc, 0x3fffd, 0x3fffe, 0x7fffe,
    0x7ffff,
};

static const uint8_t t_huffman_env_3_0dB_bits[63] = {
    18, 18, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 17, 16, 16, 16, 14, 14, 14,
    13, 12, 11, 8, 6, 4, 2, 1,
     3, 5, 7, 9, 11, 13, 14, 14,
    15, 16, 17, 18, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 19, 19, 19,
};

static const uint32_t t_huffman_env_3_0dB_codes[63] = {
    0x3ffed, 0x3ffee, 0x7ffde, 0x7ffdf, 0x7ffe0, 0x7ffe1, 0x7ffe2, 0x7ffe3,
    0x7ffe4, 0x7ffe5, 0x7ffe6, 0x7ffe7, 0x7ffe8, 0x7ffe9, 0x7ffea, 0x7ffeb,
    0x7ffec, 0x1fff4, 0x0fff7, 0x0fff9, 0x0fff8, 0x03ffb, 0x03ffa, 0x03ff8,
    0x01ffa, 0x00ffc, 0x007fc, 0x000fe, 0x0003e, 0x0000e, 0x00002, 0x00000,
    0x00006, 0x0001e, 0x0007e, 0x001fe, 0x007fd, 0x01ffb, 0x03ff9, 0x03ffc,
    0x07ffa, 0x0fff6, 0x1fff5, 0x3ffec, 0x7ffed, 0x7ffee, 0x7ffef, 0x7fff0,
    0x7fff1, 0x7fff2, 0x7fff3, 0x7fff4, 0x7fff5, 0x7fff6, 0x7fff7, 0x7fff8,
    0x7fff9, 0x7fffa, 0x7fffb, 0x7fffc, 0x7fffd, 0x7fffe, 0x7ffff,
};

static const uint8_t f_huffman_env_3_0dB_bits[63] = {
    20, 20, 20, 20, 20, 20, 20, 18,
    19, 19, 19, 19, 18, 18, 20, 19,
    17, 18, 17, 16, 16, 15, 14, 12,
    11, 10, 9, 8, 6, 4, 2, 1,
     3, 5, 8, 9, 10, 11, 12, 13,
    14, 15, 15, 16, 16, 17, 17, 18,
    18, 18, 20, 19, 19, 19, 20, 19,
    19, 20, 20, 20, 20, 20, 20,
};

static const uint32_t f_huffman_env_3_0dB_codes[63] = {
    0xffff0, 0xffff1, 0xffff2, 0xffff3, 0xffff4, 0xffff5, 0xffff6, 0x3fff3,
    0x7fff5, 0x7ffee, 0x7ffef, 0x7fff6, 0x3fff4, 0x3fff2, 0xffff7, 0x7fff0,
    0x1fff5, 0x3fff0, 0x1fff4, 0x0fff7, 0x0fff6, 0x07ff8, 0x03ffb, 0x00ffd,
    0x007fd, 0x003fd, 0x001fd, 0x000fd, 0x0003e, 0x0000e, 0x00002, 0x00000,
    0x00006, 0x0001e, 0x000fc, 0x001fc, 0x003fc, 0x007fc, 0x00ffc, 0x01ffc,
    0x03ffa, 0x07ff9, 0x07ffa, 0x0fff8, 0x0fff9, 0x1fff6, 0x1fff7, 0x3fff5,
    0x3fff6, 0x3fff1, 0xffff8, 0x7fff1, 0x7fff2, 0x7fff3, 0xffff9, 0x7fff7,
    0x7fff4, 0xffffa, 0xffffb, 0xffffc, 0xffffd, 0xffffe, 0xfffff,
};

static const uint8_t t_huffman_env_bal_3_0dB_bits[25] = {
    13, 13, 13, 13, 13, 13, 13, 12,
     8, 7, 4, 3, 1, 2, 5, 6,
     9, 13, 13, 13, 13, 13, 13, 14,
    14,
};

static const uint16_t t_huffman_env_bal_3_0dB_codes[25] = {
    0x1ff2, 0x1ff3, 0x1ff4, 0x1ff5, 0x1ff6, 0x1ff7, 0x1ff8, 0x0ff8,
    0x00fe, 0x007e, 0x000e, 0x0006, 0x0000, 0x0002, 0x001e, 0x003e,
    0x01fe, 0x1ff9, 0x1ffa, 0x1ffb, 0x1ffc, 0x1ffd, 0x1ffe, 0x3ffe,
    0x3fff,
};

static const uint8_t f_huffman_env_bal_3_0dB_bits[25] = {
    13, 13, 13, 13, 13, 14, 14, 11,
     8, 7, 4, 2, 1, 3, 5, 6,
     9, 12, 13, 14, 14, 14, 14, 14,
    14,
};

static const uint16_t f_huffman_env_bal_3_0dB_codes[25] = {
    0x1ff7, 0x1ff8, 0x1ff9, 0x1ffa, 0x1ffb, 0x3ff8, 0x3ff9, 0x07fc,
    0x00fe, 0x007e, 0x000e, 0x0002, 0x0000, 0x0006, 0x001e, 0x003e,
    0x01fe, 0x0ffa, 0x1ff6, 0x3ffa, 0x3ffb, 0x3ffc, 0x3ffd, 0x3ffe,
    0x3fff,
};

static const uint8_t t_huffman_noise_3_0dB_bits[63] = {
    13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 11, 8, 6, 4, 3, 1,
     2, 5, 8, 10, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 14, 14,
};

static const uint16_t t_huffman_noise_3_0dB_codes[63] = {
    0x1fce, 0x1fcf, 0x1fd0, 0x1fd1, 0x1fd2, 0x1fd3, 0x1fd4, 0x1fd5,
    0x1fd6, 0x1fd7, 0x1fd8, 0x1fd9, 0x1fda, 0x1fdb, 0x1fdc, 0x1fdd,
    0x1fde, 0x1fdf, 0x1fe0, 0x1fe1, 0x1fe2, 0x1fe3, 0x1fe4, 0x1fe5,
    0x1fe6, 0x1fe7, 0x07f2, 0x00fd, 0x003e, 0x000e, 0x0006, 0x0000,
    0x0002, 0x001e, 0x00fc, 0x03f8, 0x1fcc, 0x1fe8, 0x1fe9, 0x1fea,
    0x1feb, 0x1fec, 0x1fcd, 0x1fed, 0x1fee, 0x1fef, 0x1ff0, 0x1ff1,
    0x1ff2, 0x1ff3, 0x1ff4, 0x1ff5, 0x1ff6, 0x1ff7, 0x1ff8, 0x1ff9,
    0x1ffa, 0x1ffb, 0x1ffc, 0x1ffd, 0x1ffe, 0x3ffe, 0x3fff,
};

static const uint8_t t_huffman_noise_bal_3_0dB_bits[25] = {
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 5, 2, 1, 3, 6, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8,
};

static const uint8_t t_huffman_noise_bal_3_0dB_codes[25] = {
    0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3,
    0xf4, 0xf5, 0x1c, 0x02, 0x00, 0x06, 0x3a, 0xf6,
    0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe,
    0xff,
};

static const int8_t sbr_offset[6][16] = {
    {-8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7},
    {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13},
    {-5, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 16},
    {-6, -4, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 16},
    {-4, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 16, 20},
    {-2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 16, 20, 24},
};


const INTFLOAT __attribute__ ((aligned (16))) ff_sbr_noise_table[][2] = {
{((float)(-0.99948153278296f)), ((float)(-0.59483417516607f))}, {((float)(0.97113454393991f)), ((float)(-0.67528515225647f))},
{((float)(0.14130051758487f)), ((float)(-0.95090983575689f))}, {((float)(-0.47005496701697f)), ((float)(-0.37340549728647f))},
{((float)(0.80705063769351f)), ((float)(0.29653668284408f))}, {((float)(-0.38981478896926f)), ((float)(0.89572605717087f))},
{((float)(-0.01053049862020f)), ((float)(-0.66959058036166f))}, {((float)(-0.91266367957293f)), ((float)(-0.11522938140034f))},
{((float)(0.54840422910309f)), ((float)(0.75221367176302f))}, {((float)(0.40009252867955f)), ((float)(-0.98929400334421f))},
{((float)(-0.99867974711855f)), ((float)(-0.88147068645358f))}, {((float)(-0.95531076805040f)), ((float)(0.90908757154593f))},
{((float)(-0.45725933317144f)), ((float)(-0.56716323646760f))}, {((float)(-0.72929675029275f)), ((float)(-0.98008272727324f))},
{((float)(0.75622801399036f)), ((float)(0.20950329995549f))}, {((float)(0.07069442601050f)), ((float)(-0.78247898470706f))},
{((float)(0.74496252926055f)), ((float)(-0.91169004445807f))}, {((float)(-0.96440182703856f)), ((float)(-0.94739918296622f))},
{((float)(0.30424629369539f)), ((float)(-0.49438267012479f))}, {((float)(0.66565033746925f)), ((float)(0.64652935542491f))},
{((float)(0.91697008020594f)), ((float)(0.17514097332009f))}, {((float)(-0.70774918760427f)), ((float)(0.52548653416543f))},
{((float)(-0.70051415345560f)), ((float)(-0.45340028808763f))}, {((float)(-0.99496513054797f)), ((float)(-0.90071908066973f))},
{((float)(0.98164490790123f)), ((float)(-0.77463155528697f))}, {((float)(-0.54671580548181f)), ((float)(-0.02570928536004f))},
{((float)(-0.01689629065389f)), ((float)(0.00287506445732f))}, {((float)(-0.86110349531986f)), ((float)(0.42548583726477f))},
{((float)(-0.98892980586032f)), ((float)(-0.87881132267556f))}, {((float)(0.51756627678691f)), ((float)(0.66926784710139f))},
{((float)(-0.99635026409640f)), ((float)(-0.58107730574765f))}, {((float)(-0.99969370862163f)), ((float)(0.98369989360250f))},
{((float)(0.55266258627194f)), ((float)(0.59449057465591f))}, {((float)(0.34581177741673f)), ((float)(0.94879421061866f))},
{((float)(0.62664209577999f)), ((float)(-0.74402970906471f))}, {((float)(-0.77149701404973f)), ((float)(-0.33883658042801f))},
{((float)(-0.91592244254432f)), ((float)(0.03687901376713f))}, {((float)(-0.76285492357887f)), ((float)(-0.91371867919124f))},
{((float)(0.79788337195331f)), ((float)(-0.93180971199849f))}, {((float)(0.54473080610200f)), ((float)(-0.11919206037186f))},
{((float)(-0.85639281671058f)), ((float)(0.42429854760451f))}, {((float)(-0.92882402971423f)), ((float)(0.27871809078609f))},
{((float)(-0.11708371046774f)), ((float)(-0.99800843444966f))}, {((float)(0.21356749817493f)), ((float)(-0.90716295627033f))},
{((float)(-0.76191692573909f)), ((float)(0.99768118356265f))}, {((float)(0.98111043100884f)), ((float)(-0.95854459734407f))},
{((float)(-0.85913269895572f)), ((float)(0.95766566168880f))}, {((float)(-0.93307242253692f)), ((float)(0.49431757696466f))},
{((float)(0.30485754879632f)), ((float)(-0.70540034357529f))}, {((float)(0.85289650925190f)), ((float)(0.46766131791044f))},
{((float)(0.91328082618125f)), ((float)(-0.99839597361769f))}, {((float)(-0.05890199924154f)), ((float)(0.70741827819497f))},
{((float)(0.28398686150148f)), ((float)(0.34633555702188f))}, {((float)(0.95258164539612f)), ((float)(-0.54893416026939f))},
{((float)(-0.78566324168507f)), ((float)(-0.75568541079691f))}, {((float)(-0.95789495447877f)), ((float)(-0.20423194696966f))},
{((float)(0.82411158711197f)), ((float)(0.96654618432562f))}, {((float)(-0.65185446735885f)), ((float)(-0.88734990773289f))},
{((float)(-0.93643603134666f)), ((float)(0.99870790442385f))}, {((float)(0.91427159529618f)), ((float)(-0.98290505544444f))},
{((float)(-0.70395684036886f)), ((float)(0.58796798221039f))}, {((float)(0.00563771969365f)), ((float)(0.61768196727244f))},
{((float)(0.89065051931895f)), ((float)(0.52783352697585f))}, {((float)(-0.68683707712762f)), ((float)(0.80806944710339f))},
{((float)(0.72165342518718f)), ((float)(-0.69259857349564f))}, {((float)(-0.62928247730667f)), ((float)(0.13627037407335f))},
{((float)(0.29938434065514f)), ((float)(-0.46051329682246f))}, {((float)(-0.91781958879280f)), ((float)(-0.74012716684186f))},
{((float)(0.99298717043688f)), ((float)(0.40816610075661f))}, {((float)(0.82368298622748f)), ((float)(-0.74036047190173f))},
{((float)(-0.98512833386833f)), ((float)(-0.99972330709594f))}, {((float)(-0.95915368242257f)), ((float)(-0.99237800466040f))},
{((float)(-0.21411126572790f)), ((float)(-0.93424819052545f))}, {((float)(-0.68821476106884f)), ((float)(-0.26892306315457f))},
{((float)(0.91851997982317f)), ((float)(0.09358228901785f))}, {((float)(-0.96062769559127f)), ((float)(0.36099095133739f))},
{((float)(0.51646184922287f)), ((float)(-0.71373332873917f))}, {((float)(0.61130721139669f)), ((float)(0.46950141175917f))},
{((float)(0.47336129371299f)), ((float)(-0.27333178296162f))}, {((float)(0.90998308703519f)), ((float)(0.96715662938132f))},
{((float)(0.44844799194357f)), ((float)(0.99211574628306f))}, {((float)(0.66614891079092f)), ((float)(0.96590176169121f))},
{((float)(0.74922239129237f)), ((float)(-0.89879858826087f))}, {((float)(-0.99571588506485f)), ((float)(0.52785521494349f))},
{((float)(0.97401082477563f)), ((float)(-0.16855870075190f))}, {((float)(0.72683747733879f)), ((float)(-0.48060774432251f))},
{((float)(0.95432193457128f)), ((float)(0.68849603408441f))}, {((float)(-0.72962208425191f)), ((float)(-0.76608443420917f))},
{((float)(-0.85359479233537f)), ((float)(0.88738125901579f))}, {((float)(-0.81412430338535f)), ((float)(-0.97480768049637f))},
{((float)(-0.87930772356786f)), ((float)(0.74748307690436f))}, {((float)(-0.71573331064977f)), ((float)(-0.98570608178923f))},
{((float)(0.83524300028228f)), ((float)(0.83702537075163f))}, {((float)(-0.48086065601423f)), ((float)(-0.98848504923531f))},
{((float)(0.97139128574778f)), ((float)(0.80093621198236f))}, {((float)(0.51992825347895f)), ((float)(0.80247631400510f))},
{((float)(-0.00848591195325f)), ((float)(-0.76670128000486f))}, {((float)(-0.70294374303036f)), ((float)(0.55359910445577f))},
{((float)(-0.95894428168140f)), ((float)(-0.43265504344783f))}, {((float)(0.97079252950321f)), ((float)(0.09325857238682f))},
{((float)(-0.92404293670797f)), ((float)(0.85507704027855f))}, {((float)(-0.69506469500450f)), ((float)(0.98633412625459f))},
{((float)(0.26559203620024f)), ((float)(0.73314307966524f))}, {((float)(0.28038443336943f)), ((float)(0.14537913654427f))},
{((float)(-0.74138124825523f)), ((float)(0.99310339807762f))}, {((float)(-0.01752795995444f)), ((float)(-0.82616635284178f))},
{((float)(-0.55126773094930f)), ((float)(-0.98898543862153f))}, {((float)(0.97960898850996f)), ((float)(-0.94021446752851f))},
{((float)(-0.99196309146936f)), ((float)(0.67019017358456f))}, {((float)(-0.67684928085260f)), ((float)(0.12631491649378f))},
{((float)(0.09140039465500f)), ((float)(-0.20537731453108f))}, {((float)(-0.71658965751996f)), ((float)(-0.97788200391224f))},
{((float)(0.81014640078925f)), ((float)(0.53722648362443f))}, {((float)(0.40616991671205f)), ((float)(-0.26469008598449f))},
{((float)(-0.67680188682972f)), ((float)(0.94502052337695f))}, {((float)(0.86849774348749f)), ((float)(-0.18333598647899f))},
{((float)(-0.99500381284851f)), ((float)(-0.02634122068550f))}, {((float)(0.84329189340667f)), ((float)(0.10406957462213f))},
{((float)(-0.09215968531446f)), ((float)(0.69540012101253f))}, {((float)(0.99956173327206f)), ((float)(-0.12358542001404f))},
{((float)(-0.79732779473535f)), ((float)(-0.91582524736159f))}, {((float)(0.96349973642406f)), ((float)(0.96640458041000f))},
{((float)(-0.79942778496547f)), ((float)(0.64323902822857f))}, {((float)(-0.11566039853896f)), ((float)(0.28587846253726f))},
{((float)(-0.39922954514662f)), ((float)(0.94129601616966f))}, {((float)(0.99089197565987f)), ((float)(-0.92062625581587f))},
{((float)(0.28631285179909f)), ((float)(-0.91035047143603f))}, {((float)(-0.83302725605608f)), ((float)(-0.67330410892084f))},
{((float)(0.95404443402072f)), ((float)(0.49162765398743f))}, {((float)(-0.06449863579434f)), ((float)(0.03250560813135f))},
{((float)(-0.99575054486311f)), ((float)(0.42389784469507f))}, {((float)(-0.65501142790847f)), ((float)(0.82546114655624f))},
{((float)(-0.81254441908887f)), ((float)(-0.51627234660629f))}, {((float)(-0.99646369485481f)), ((float)(0.84490533520752f))},
{((float)(0.00287840603348f)), ((float)(0.64768261158166f))}, {((float)(0.70176989408455f)), ((float)(-0.20453028573322f))},
{((float)(0.96361882270190f)), ((float)(0.40706967140989f))}, {((float)(-0.68883758192426f)), ((float)(0.91338958840772f))},
{((float)(-0.34875585502238f)), ((float)(0.71472290693300f))}, {((float)(0.91980081243087f)), ((float)(0.66507455644919f))},
{((float)(-0.99009048343881f)), ((float)(0.85868021604848f))}, {((float)(0.68865791458395f)), ((float)(0.55660316809678f))},
{((float)(-0.99484402129368f)), ((float)(-0.20052559254934f))}, {((float)(0.94214511408023f)), ((float)(-0.99696425367461f))},
{((float)(-0.67414626793544f)), ((float)(0.49548221180078f))}, {((float)(-0.47339353684664f)), ((float)(-0.85904328834047f))},
{((float)(0.14323651387360f)), ((float)(-0.94145598222488f))}, {((float)(-0.29268293575672f)), ((float)(0.05759224927952f))},
{((float)(0.43793861458754f)), ((float)(-0.78904969892724f))}, {((float)(-0.36345126374441f)), ((float)(0.64874435357162f))},
{((float)(-0.08750604656825f)), ((float)(0.97686944362527f))}, {((float)(-0.96495267812511f)), ((float)(-0.53960305946511f))},
{((float)(0.55526940659947f)), ((float)(0.78891523734774f))}, {((float)(0.73538215752630f)), ((float)(0.96452072373404f))},
{((float)(-0.30889773919437f)), ((float)(-0.80664389776860f))}, {((float)(0.03574995626194f)), ((float)(-0.97325616900959f))},
{((float)(0.98720684660488f)), ((float)(0.48409133691962f))}, {((float)(-0.81689296271203f)), ((float)(-0.90827703628298f))},
{((float)(0.67866860118215f)), ((float)(0.81284503870856f))}, {((float)(-0.15808569732583f)), ((float)(0.85279555024382f))},
{((float)(0.80723395114371f)), ((float)(-0.24717418514605f))}, {((float)(0.47788757329038f)), ((float)(-0.46333147839295f))},
{((float)(0.96367554763201f)), ((float)(0.38486749303242f))}, {((float)(-0.99143875716818f)), ((float)(-0.24945277239809f))},
{((float)(0.83081876925833f)), ((float)(-0.94780851414763f))}, {((float)(-0.58753191905341f)), ((float)(0.01290772389163f))},
{((float)(0.95538108220960f)), ((float)(-0.85557052096538f))}, {((float)(-0.96490920476211f)), ((float)(-0.64020970923102f))},
{((float)(-0.97327101028521f)), ((float)(0.12378128133110f))}, {((float)(0.91400366022124f)), ((float)(0.57972471346930f))},
{((float)(-0.99925837363824f)), ((float)(0.71084847864067f))}, {((float)(-0.86875903507313f)), ((float)(-0.20291699203564f))},
{((float)(-0.26240034795124f)), ((float)(-0.68264554369108f))}, {((float)(-0.24664412953388f)), ((float)(-0.87642273115183f))},
{((float)(0.02416275806869f)), ((float)(0.27192914288905f))}, {((float)(0.82068619590515f)), ((float)(-0.85087787994476f))},
{((float)(0.88547373760759f)), ((float)(-0.89636802901469f))}, {((float)(-0.18173078152226f)), ((float)(-0.26152145156800f))},
{((float)(0.09355476558534f)), ((float)(0.54845123045604f))}, {((float)(-0.54668414224090f)), ((float)(0.95980774020221f))},
{((float)(0.37050990604091f)), ((float)(-0.59910140383171f))}, {((float)(-0.70373594262891f)), ((float)(0.91227665827081f))},
{((float)(-0.34600785879594f)), ((float)(-0.99441426144200f))}, {((float)(-0.68774481731008f)), ((float)(-0.30238837956299f))},
{((float)(-0.26843291251234f)), ((float)(0.83115668004362f))}, {((float)(0.49072334613242f)), ((float)(-0.45359708737775f))},
{((float)(0.38975993093975f)), ((float)(0.95515358099121f))}, {((float)(-0.97757125224150f)), ((float)(0.05305894580606f))},
{((float)(-0.17325552859616f)), ((float)(-0.92770672250494f))}, {((float)(0.99948035025744f)), ((float)(0.58285545563426f))},
{((float)(-0.64946246527458f)), ((float)(0.68645507104960f))}, {((float)(-0.12016920576437f)), ((float)(-0.57147322153312f))},
{((float)(-0.58947456517751f)), ((float)(-0.34847132454388f))}, {((float)(-0.41815140454465f)), ((float)(0.16276422358861f))},
{((float)(0.99885650204884f)), ((float)(0.11136095490444f))}, {((float)(-0.56649614128386f)), ((float)(-0.90494866361587f))},
{((float)(0.94138021032330f)), ((float)(0.35281916733018f))}, {((float)(-0.75725076534641f)), ((float)(0.53650549640587f))},
{((float)(0.20541973692630f)), ((float)(-0.94435144369918f))}, {((float)(0.99980371023351f)), ((float)(0.79835913565599f))},
{((float)(0.29078277605775f)), ((float)(0.35393777921520f))}, {((float)(-0.62858772103030f)), ((float)(0.38765693387102f))},
{((float)(0.43440904467688f)), ((float)(-0.98546330463232f))}, {((float)(-0.98298583762390f)), ((float)(0.21021524625209f))},
{((float)(0.19513029146934f)), ((float)(-0.94239832251867f))}, {((float)(-0.95476662400101f)), ((float)(0.98364554179143f))},
{((float)(0.93379635304810f)), ((float)(-0.70881994583682f))}, {((float)(-0.85235410573336f)), ((float)(-0.08342347966410f))},
{((float)(-0.86425093011245f)), ((float)(-0.45795025029466f))}, {((float)(0.38879779059045f)), ((float)(0.97274429344593f))},
{((float)(0.92045124735495f)), ((float)(-0.62433652524220f))}, {((float)(0.89162532251878f)), ((float)(0.54950955570563f))},
{((float)(-0.36834336949252f)), ((float)(0.96458298020975f))}, {((float)(0.93891760988045f)), ((float)(-0.89968353740388f))},
{((float)(0.99267657565094f)), ((float)(-0.03757034316958f))}, {((float)(-0.94063471614176f)), ((float)(0.41332338538963f))},
{((float)(0.99740224117019f)), ((float)(-0.16830494996370f))}, {((float)(-0.35899413170555f)), ((float)(-0.46633226649613f))},
{((float)(0.05237237274947f)), ((float)(-0.25640361602661f))}, {((float)(0.36703583957424f)), ((float)(-0.38653265641875f))},
{((float)(0.91653180367913f)), ((float)(-0.30587628726597f))}, {((float)(0.69000803499316f)), ((float)(0.90952171386132f))},
{((float)(-0.38658751133527f)), ((float)(0.99501571208985f))}, {((float)(-0.29250814029851f)), ((float)(0.37444994344615f))},
{((float)(-0.60182204677608f)), ((float)(0.86779651036123f))}, {((float)(-0.97418588163217f)), ((float)(0.96468523666475f))},
{((float)(0.88461574003963f)), ((float)(0.57508405276414f))}, {((float)(0.05198933055162f)), ((float)(0.21269661669964f))},
{((float)(-0.53499621979720f)), ((float)(0.97241553731237f))}, {((float)(-0.49429560226497f)), ((float)(0.98183865291903f))},
{((float)(-0.98935142339139f)), ((float)(-0.40249159006933f))}, {((float)(-0.98081380091130f)), ((float)(-0.72856895534041f))},
{((float)(-0.27338148835532f)), ((float)(0.99950922447209f))}, {((float)(0.06310802338302f)), ((float)(-0.54539587529618f))},
{((float)(-0.20461677199539f)), ((float)(-0.14209977628489f))}, {((float)(0.66223843141647f)), ((float)(0.72528579940326f))},
{((float)(-0.84764345483665f)), ((float)(0.02372316801261f))}, {((float)(-0.89039863483811f)), ((float)(0.88866581484602f))},
{((float)(0.95903308477986f)), ((float)(0.76744927173873f))}, {((float)(0.73504123909879f)), ((float)(-0.03747203173192f))},
{((float)(-0.31744434966056f)), ((float)(-0.36834111883652f))}, {((float)(-0.34110827591623f)), ((float)(0.40211222807691f))},
{((float)(0.47803883714199f)), ((float)(-0.39423219786288f))}, {((float)(0.98299195879514f)), ((float)(0.01989791390047f))},
{((float)(-0.30963073129751f)), ((float)(-0.18076720599336f))}, {((float)(0.99992588229018f)), ((float)(-0.26281872094289f))},
{((float)(-0.93149731080767f)), ((float)(-0.98313162570490f))}, {((float)(0.99923472302773f)), ((float)(-0.80142993767554f))},
{((float)(-0.26024169633417f)), ((float)(-0.75999759855752f))}, {((float)(-0.35712514743563f)), ((float)(0.19298963768574f))},
{((float)(-0.99899084509530f)), ((float)(0.74645156992493f))}, {((float)(0.86557171579452f)), ((float)(0.55593866696299f))},
{((float)(0.33408042438752f)), ((float)(0.86185953874709f))}, {((float)(0.99010736374716f)), ((float)(0.04602397576623f))},
{((float)(-0.66694269691195f)), ((float)(-0.91643611810148f))}, {((float)(0.64016792079480f)), ((float)(0.15649530836856f))},
{((float)(0.99570534804836f)), ((float)(0.45844586038111f))}, {((float)(-0.63431466947340f)), ((float)(0.21079116459234f))},
{((float)(-0.07706847005931f)), ((float)(-0.89581437101329f))}, {((float)(0.98590090577724f)), ((float)(0.88241721133981f))},
{((float)(0.80099335254678f)), ((float)(-0.36851896710853f))}, {((float)(0.78368131392666f)), ((float)(0.45506999802597f))},
{((float)(0.08707806671691f)), ((float)(0.80938994918745f))}, {((float)(-0.86811883080712f)), ((float)(0.39347308654705f))},
{((float)(-0.39466529740375f)), ((float)(-0.66809432114456f))}, {((float)(0.97875325649683f)), ((float)(-0.72467840967746f))},
{((float)(-0.95038560288864f)), ((float)(0.89563219587625f))}, {((float)(0.17005239424212f)), ((float)(0.54683053962658f))},
{((float)(-0.76910792026848f)), ((float)(-0.96226617549298f))}, {((float)(0.99743281016846f)), ((float)(0.42697157037567f))},
{((float)(0.95437383549973f)), ((float)(0.97002324109952f))}, {((float)(0.99578905365569f)), ((float)(-0.54106826257356f))},
{((float)(0.28058259829990f)), ((float)(-0.85361420634036f))}, {((float)(0.85256524470573f)), ((float)(-0.64567607735589f))},
{((float)(-0.50608540105128f)), ((float)(-0.65846015480300f))}, {((float)(-0.97210735183243f)), ((float)(-0.23095213067791f))},
{((float)(0.95424048234441f)), ((float)(-0.99240147091219f))}, {((float)(-0.96926570524023f)), ((float)(0.73775654896574f))},
{((float)(0.30872163214726f)), ((float)(0.41514960556126f))}, {((float)(-0.24523839572639f)), ((float)(0.63206633394807f))},
{((float)(-0.33813265086024f)), ((float)(-0.38661779441897f))}, {((float)(-0.05826828420146f)), ((float)(-0.06940774188029f))},
{((float)(-0.22898461455054f)), ((float)(0.97054853316316f))}, {((float)(-0.18509915019881f)), ((float)(0.47565762892084f))},
{((float)(-0.10488238045009f)), ((float)(-0.87769947402394f))}, {((float)(-0.71886586182037f)), ((float)(0.78030982480538f))},
{((float)(0.99793873738654f)), ((float)(0.90041310491497f))}, {((float)(0.57563307626120f)), ((float)(-0.91034337352097f))},
{((float)(0.28909646383717f)), ((float)(0.96307783970534f))}, {((float)(0.42188998312520f)), ((float)(0.48148651230437f))},
{((float)(0.93335049681047f)), ((float)(-0.43537023883588f))}, {((float)(-0.97087374418267f)), ((float)(0.86636445711364f))},
{((float)(0.36722871286923f)), ((float)(0.65291654172961f))}, {((float)(-0.81093025665696f)), ((float)(0.08778370229363f))},
{((float)(-0.26240603062237f)), ((float)(-0.92774095379098f))}, {((float)(0.83996497984604f)), ((float)(0.55839849139647f))},
{((float)(-0.99909615720225f)), ((float)(-0.96024605713970f))}, {((float)(0.74649464155061f)), ((float)(0.12144893606462f))},
{((float)(-0.74774595569805f)), ((float)(-0.26898062008959f))}, {((float)(0.95781667469567f)), ((float)(-0.79047927052628f))},
{((float)(0.95472308713099f)), ((float)(-0.08588776019550f))}, {((float)(0.48708332746299f)), ((float)(0.99999041579432f))},
{((float)(0.46332038247497f)), ((float)(0.10964126185063f))}, {((float)(-0.76497004940162f)), ((float)(0.89210929242238f))},
{((float)(0.57397389364339f)), ((float)(0.35289703373760f))}, {((float)(0.75374316974495f)), ((float)(0.96705214651335f))},
{((float)(-0.59174397685714f)), ((float)(-0.89405370422752f))}, {((float)(0.75087906691890f)), ((float)(-0.29612672982396f))},
{((float)(-0.98607857336230f)), ((float)(0.25034911730023f))}, {((float)(-0.40761056640505f)), ((float)(-0.90045573444695f))},
{((float)(0.66929266740477f)), ((float)(0.98629493401748f))}, {((float)(-0.97463695257310f)), ((float)(-0.00190223301301f))},
{((float)(0.90145509409859f)), ((float)(0.99781390365446f))}, {((float)(-0.87259289048043f)), ((float)(0.99233587353666f))},
{((float)(-0.91529461447692f)), ((float)(-0.15698707534206f))}, {((float)(-0.03305738840705f)), ((float)(-0.37205262859764f))},
{((float)(0.07223051368337f)), ((float)(-0.88805001733626f))}, {((float)(0.99498012188353f)), ((float)(0.97094358113387f))},
{((float)(-0.74904939500519f)), ((float)(0.99985483641521f))}, {((float)(0.04585228574211f)), ((float)(0.99812337444082f))},
{((float)(-0.89054954257993f)), ((float)(-0.31791913188064f))}, {((float)(-0.83782144651251f)), ((float)(0.97637632547466f))},
{((float)(0.33454804933804f)), ((float)(-0.86231516800408f))}, {((float)(-0.99707579362824f)), ((float)(0.93237990079441f))},
{((float)(-0.22827527843994f)), ((float)(0.18874759397997f))}, {((float)(0.67248046289143f)), ((float)(-0.03646211390569f))},
{((float)(-0.05146538187944f)), ((float)(-0.92599700120679f))}, {((float)(0.99947295749905f)), ((float)(0.93625229707912f))},
{((float)(0.66951124390363f)), ((float)(0.98905825623893f))}, {((float)(-0.99602956559179f)), ((float)(-0.44654715757688f))},
{((float)(0.82104905483590f)), ((float)(0.99540741724928f))}, {((float)(0.99186510988782f)), ((float)(0.72023001312947f))},
{((float)(-0.65284592392918f)), ((float)(0.52186723253637f))}, {((float)(0.93885443798188f)), ((float)(-0.74895312615259f))},
{((float)(0.96735248738388f)), ((float)(0.90891816978629f))}, {((float)(-0.22225968841114f)), ((float)(0.57124029781228f))},
{((float)(-0.44132783753414f)), ((float)(-0.92688840659280f))}, {((float)(-0.85694974219574f)), ((float)(0.88844532719844f))},
{((float)(0.91783042091762f)), ((float)(-0.46356892383970f))}, {((float)(0.72556974415690f)), ((float)(-0.99899555770747f))},
{((float)(-0.99711581834508f)), ((float)(0.58211560180426f))}, {((float)(0.77638976371966f)), ((float)(0.94321834873819f))},
{((float)(0.07717324253925f)), ((float)(0.58638399856595f))}, {((float)(-0.56049829194163f)), ((float)(0.82522301569036f))},
{((float)(0.98398893639988f)), ((float)(0.39467440420569f))}, {((float)(0.47546946844938f)), ((float)(0.68613044836811f))},
{((float)(0.65675089314631f)), ((float)(0.18331637134880f))}, {((float)(0.03273375457980f)), ((float)(-0.74933109564108f))},
{((float)(-0.38684144784738f)), ((float)(0.51337349030406f))}, {((float)(-0.97346267944545f)), ((float)(-0.96549364384098f))},
{((float)(-0.53282156061942f)), ((float)(-0.91423265091354f))}, {((float)(0.99817310731176f)), ((float)(0.61133572482148f))},
{((float)(-0.50254500772635f)), ((float)(-0.88829338134294f))}, {((float)(0.01995873238855f)), ((float)(0.85223515096765f))},
{((float)(0.99930381973804f)), ((float)(0.94578896296649f))}, {((float)(0.82907767600783f)), ((float)(-0.06323442598128f))},
{((float)(-0.58660709669728f)), ((float)(0.96840773806582f))}, {((float)(-0.17573736667267f)), ((float)(-0.48166920859485f))},
{((float)(0.83434292401346f)), ((float)(-0.13023450646997f))}, {((float)(0.05946491307025f)), ((float)(0.20511047074866f))},
{((float)(0.81505484574602f)), ((float)(-0.94685947861369f))}, {((float)(-0.44976380954860f)), ((float)(0.40894572671545f))},
{((float)(-0.89746474625671f)), ((float)(0.99846578838537f))}, {((float)(0.39677256130792f)), ((float)(-0.74854668609359f))},
{((float)(-0.07588948563079f)), ((float)(0.74096214084170f))}, {((float)(0.76343198951445f)), ((float)(0.41746629422634f))},
{((float)(-0.74490104699626f)), ((float)(0.94725911744610f))}, {((float)(0.64880119792759f)), ((float)(0.41336660830571f))},
{((float)(0.62319537462542f)), ((float)(-0.93098313552599f))}, {((float)(0.42215817594807f)), ((float)(-0.07712787385208f))},
{((float)(0.02704554141885f)), ((float)(-0.05417518053666f))}, {((float)(0.80001773566818f)), ((float)(0.91542195141039f))},
{((float)(-0.79351832348816f)), ((float)(-0.36208897989136f))}, {((float)(0.63872359151636f)), ((float)(0.08128252493444f))},
{((float)(0.52890520960295f)), ((float)(0.60048872455592f))}, {((float)(0.74238552914587f)), ((float)(0.04491915291044f))},
{((float)(0.99096131449250f)), ((float)(-0.19451182854402f))}, {((float)(-0.80412329643109f)), ((float)(-0.88513818199457f))},
{((float)(-0.64612616129736f)), ((float)(0.72198674804544f))}, {((float)(0.11657770663191f)), ((float)(-0.83662833815041f))},
{((float)(-0.95053182488101f)), ((float)(-0.96939905138082f))}, {((float)(-0.62228872928622f)), ((float)(0.82767262846661f))},
{((float)(0.03004475787316f)), ((float)(-0.99738896333384f))}, {((float)(-0.97987214341034f)), ((float)(0.36526129686425f))},
{((float)(-0.99986980746200f)), ((float)(-0.36021610299715f))}, {((float)(0.89110648599879f)), ((float)(-0.97894250343044f))},
{((float)(0.10407960510582f)), ((float)(0.77357793811619f))}, {((float)(0.95964737821728f)), ((float)(-0.35435818285502f))},
{((float)(0.50843233159162f)), ((float)(0.96107691266205f))}, {((float)(0.17006334670615f)), ((float)(-0.76854025314829f))},
{((float)(0.25872675063360f)), ((float)(0.99893303933816f))}, {((float)(-0.01115998681937f)), ((float)(0.98496019742444f))},
{((float)(-0.79598702973261f)), ((float)(0.97138411318894f))}, {((float)(-0.99264708948101f)), ((float)(-0.99542822402536f))},
{((float)(-0.99829663752818f)), ((float)(0.01877138824311f))}, {((float)(-0.70801016548184f)), ((float)(0.33680685948117f))},
{((float)(-0.70467057786826f)), ((float)(0.93272777501857f))}, {((float)(0.99846021905254f)), ((float)(-0.98725746254433f))},
{((float)(-0.63364968534650f)), ((float)(-0.16473594423746f))}, {((float)(-0.16258217500792f)), ((float)(-0.95939125400802f))},
{((float)(-0.43645594360633f)), ((float)(-0.94805030113284f))}, {((float)(-0.99848471702976f)), ((float)(0.96245166923809f))},
{((float)(-0.16796458968998f)), ((float)(-0.98987511890470f))}, {((float)(-0.87979225745213f)), ((float)(-0.71725725041680f))},
{((float)(0.44183099021786f)), ((float)(-0.93568974498761f))}, {((float)(0.93310180125532f)), ((float)(-0.99913308068246f))},
{((float)(-0.93941931782002f)), ((float)(-0.56409379640356f))}, {((float)(-0.88590003188677f)), ((float)(0.47624600491382f))},
{((float)(0.99971463703691f)), ((float)(-0.83889954253462f))}, {((float)(-0.75376385639978f)), ((float)(0.00814643438625f))},
{((float)(0.93887685615875f)), ((float)(-0.11284528204636f))}, {((float)(0.85126435782309f)), ((float)(0.52349251543547f))},
{((float)(0.39701421446381f)), ((float)(0.81779634174316f))}, {((float)(-0.37024464187437f)), ((float)(-0.87071656222959f))},
{((float)(-0.36024828242896f)), ((float)(0.34655735648287f))}, {((float)(-0.93388812549209f)), ((float)(-0.84476541096429f))},
{((float)(-0.65298804552119f)), ((float)(-0.18439575450921f))}, {((float)(0.11960319006843f)), ((float)(0.99899346780168f))},
{((float)(0.94292565553160f)), ((float)(0.83163906518293f))}, {((float)(0.75081145286948f)), ((float)(-0.35533223142265f))},
{((float)(0.56721979748394f)), ((float)(-0.24076836414499f))}, {((float)(0.46857766746029f)), ((float)(-0.30140233457198f))},
{((float)(0.97312313923635f)), ((float)(-0.99548191630031f))}, {((float)(-0.38299976567017f)), ((float)(0.98516909715427f))},
{((float)(0.41025800019463f)), ((float)(0.02116736935734f))}, {((float)(0.09638062008048f)), ((float)(0.04411984381457f))},
{((float)(-0.85283249275397f)), ((float)(0.91475563922421f))}, {((float)(0.88866808958124f)), ((float)(-0.99735267083226f))},
{((float)(-0.48202429536989f)), ((float)(-0.96805608884164f))}, {((float)(0.27572582416567f)), ((float)(0.58634753335832f))},
{((float)(-0.65889129659168f)), ((float)(0.58835634138583f))}, {((float)(0.98838086953732f)), ((float)(0.99994349600236f))},
{((float)(-0.20651349620689f)), ((float)(0.54593044066355f))}, {((float)(-0.62126416356920f)), ((float)(-0.59893681700392f))},
{((float)(0.20320105410437f)), ((float)(-0.86879180355289f))}, {((float)(-0.97790548600584f)), ((float)(0.96290806999242f))},
{((float)(0.11112534735126f)), ((float)(0.21484763313301f))}, {((float)(-0.41368337314182f)), ((float)(0.28216837680365f))},
{((float)(0.24133038992960f)), ((float)(0.51294362630238f))}, {((float)(-0.66393410674885f)), ((float)(-0.08249679629081f))},
{((float)(-0.53697829178752f)), ((float)(-0.97649903936228f))}, {((float)(-0.97224737889348f)), ((float)(0.22081333579837f))},
{((float)(0.87392477144549f)), ((float)(-0.12796173740361f))}, {((float)(0.19050361015753f)), ((float)(0.01602615387195f))},
{((float)(-0.46353441212724f)), ((float)(-0.95249041539006f))}, {((float)(-0.07064096339021f)), ((float)(-0.94479803205886f))},
{((float)(-0.92444085484466f)), ((float)(-0.10457590187436f))}, {((float)(-0.83822593578728f)), ((float)(-0.01695043208885f))},
{((float)(0.75214681811150f)), ((float)(-0.99955681042665f))}, {((float)(-0.42102998829339f)), ((float)(0.99720941999394f))},
{((float)(-0.72094786237696f)), ((float)(-0.35008961934255f))}, {((float)(0.78843311019251f)), ((float)(0.52851398958271f))},
{((float)(0.97394027897442f)), ((float)(-0.26695944086561f))}, {((float)(0.99206463477946f)), ((float)(-0.57010120849429f))},
{((float)(0.76789609461795f)), ((float)(-0.76519356730966f))}, {((float)(-0.82002421836409f)), ((float)(-0.73530179553767f))},
{((float)(0.81924990025724f)), ((float)(0.99698425250579f))}, {((float)(-0.26719850873357f)), ((float)(0.68903369776193f))},
{((float)(-0.43311260380975f)), ((float)(0.85321815947490f))}, {((float)(0.99194979673836f)), ((float)(0.91876249766422f))},
{((float)(-0.80692001248487f)), ((float)(-0.32627540663214f))}, {((float)(0.43080003649976f)), ((float)(-0.21919095636638f))},
{((float)(0.67709491937357f)), ((float)(-0.95478075822906f))}, {((float)(0.56151770568316f)), ((float)(-0.70693811747778f))},
{((float)(0.10831862810749f)), ((float)(-0.08628837174592f))}, {((float)(0.91229417540436f)), ((float)(-0.65987351408410f))},
{((float)(-0.48972893932274f)), ((float)(0.56289246362686f))}, {((float)(-0.89033658689697f)), ((float)(-0.71656563987082f))},
{((float)(0.65269447475094f)), ((float)(0.65916004833932f))}, {((float)(0.67439478141121f)), ((float)(-0.81684380846796f))},
{((float)(-0.47770832416973f)), ((float)(-0.16789556203025f))}, {((float)(-0.99715979260878f)), ((float)(-0.93565784007648f))},
{((float)(-0.90889593602546f)), ((float)(0.62034397054380f))}, {((float)(-0.06618622548177f)), ((float)(-0.23812217221359f))},
{((float)(0.99430266919728f)), ((float)(0.18812555317553f))}, {((float)(0.97686402381843f)), ((float)(-0.28664534366620f))},
{((float)(0.94813650221268f)), ((float)(-0.97506640027128f))}, {((float)(-0.95434497492853f)), ((float)(-0.79607978501983f))},
{((float)(-0.49104783137150f)), ((float)(0.32895214359663f))}, {((float)(0.99881175120751f)), ((float)(0.88993983831354f))},
{((float)(0.50449166760303f)), ((float)(-0.85995072408434f))}, {((float)(0.47162891065108f)), ((float)(-0.18680204049569f))},
{((float)(-0.62081581361840f)), ((float)(0.75000676218956f))}, {((float)(-0.43867015250812f)), ((float)(0.99998069244322f))},
{((float)(0.98630563232075f)), ((float)(-0.53578899600662f))}, {((float)(-0.61510362277374f)), ((float)(-0.89515019899997f))},
{((float)(-0.03841517601843f)), ((float)(-0.69888815681179f))}, {((float)(-0.30102157304644f)), ((float)(-0.07667808922205f))},
{((float)(0.41881284182683f)), ((float)(0.02188098922282f))}, {((float)(-0.86135454941237f)), ((float)(0.98947480909359f))},
{((float)(0.67226861393788f)), ((float)(-0.13494389011014f))}, {((float)(-0.70737398842068f)), ((float)(-0.76547349325992f))},
{((float)(0.94044946687963f)), ((float)(0.09026201157416f))}, {((float)(-0.82386352534327f)), ((float)(0.08924768823676f))},
{((float)(-0.32070666698656f)), ((float)(0.50143421908753f))}, {((float)(0.57593163224487f)), ((float)(-0.98966422921509f))},
{((float)(-0.36326018419965f)), ((float)(0.07440243123228f))}, {((float)(0.99979044674350f)), ((float)(-0.14130287347405f))},
{((float)(-0.92366023326932f)), ((float)(-0.97979298068180f))}, {((float)(-0.44607178518598f)), ((float)(-0.54233252016394f))},
{((float)(0.44226800932956f)), ((float)(0.71326756742752f))}, {((float)(0.03671907158312f)), ((float)(0.63606389366675f))},
{((float)(0.52175424682195f)), ((float)(-0.85396826735705f))}, {((float)(-0.94701139690956f)), ((float)(-0.01826348194255f))},
{((float)(-0.98759606946049f)), ((float)(0.82288714303073f))}, {((float)(0.87434794743625f)), ((float)(0.89399495655433f))},
{((float)(-0.93412041758744f)), ((float)(0.41374052024363f))}, {((float)(0.96063943315511f)), ((float)(0.93116709541280f))},
{((float)(0.97534253457837f)), ((float)(0.86150930812689f))}, {((float)(0.99642466504163f)), ((float)(0.70190043427512f))},
{((float)(-0.94705089665984f)), ((float)(-0.29580042814306f))}, {((float)(0.91599807087376f)), ((float)(-0.98147830385781f))},

{((float)(-0.99948153278296f)), ((float)(-0.59483417516607f))}, {((float)(0.97113454393991f)), ((float)(-0.67528515225647f))},
{((float)(0.14130051758487f)), ((float)(-0.95090983575689f))}, {((float)(-0.47005496701697f)), ((float)(-0.37340549728647f))},
{((float)(0.80705063769351f)), ((float)(0.29653668284408f))}, {((float)(-0.38981478896926f)), ((float)(0.89572605717087f))},
{((float)(-0.01053049862020f)), ((float)(-0.66959058036166f))}, {((float)(-0.91266367957293f)), ((float)(-0.11522938140034f))},
};


static INTFLOAT __attribute__ ((aligned (32))) sbr_qmf_window_ds[320];


static const INTFLOAT __attribute__ ((aligned (32))) sbr_qmf_window_us[640] = {
     ((float)(0.0000000000f)), ((float)(-0.0005525286f)), ((float)(-0.0005617692f)), ((float)(-0.0004947518f)),
     ((float)(-0.0004875227f)), ((float)(-0.0004893791f)), ((float)(-0.0005040714f)), ((float)(-0.0005226564f)),
     ((float)(-0.0005466565f)), ((float)(-0.0005677802f)), ((float)(-0.0005870930f)), ((float)(-0.0006132747f)),
     ((float)(-0.0006312493f)), ((float)(-0.0006540333f)), ((float)(-0.0006777690f)), ((float)(-0.0006941614f)),
     ((float)(-0.0007157736f)), ((float)(-0.0007255043f)), ((float)(-0.0007440941f)), ((float)(-0.0007490598f)),
     ((float)(-0.0007681371f)), ((float)(-0.0007724848f)), ((float)(-0.0007834332f)), ((float)(-0.0007779869f)),
     ((float)(-0.0007803664f)), ((float)(-0.0007801449f)), ((float)(-0.0007757977f)), ((float)(-0.0007630793f)),
     ((float)(-0.0007530001f)), ((float)(-0.0007319357f)), ((float)(-0.0007215391f)), ((float)(-0.0006917937f)),
     ((float)(-0.0006650415f)), ((float)(-0.0006341594f)), ((float)(-0.0005946118f)), ((float)(-0.0005564576f)),
     ((float)(-0.0005145572f)), ((float)(-0.0004606325f)), ((float)(-0.0004095121f)), ((float)(-0.0003501175f)),
     ((float)(-0.0002896981f)), ((float)(-0.0002098337f)), ((float)(-0.0001446380f)), ((float)(-0.0000617334f)),
     ((float)(0.0000134949f)), ((float)(0.0001094383f)), ((float)(0.0002043017f)), ((float)(0.0002949531f)),
     ((float)(0.0004026540f)), ((float)(0.0005107388f)), ((float)(0.0006239376f)), ((float)(0.0007458025f)),
     ((float)(0.0008608443f)), ((float)(0.0009885988f)), ((float)(0.0011250155f)), ((float)(0.0012577884f)),
     ((float)(0.0013902494f)), ((float)(0.0015443219f)), ((float)(0.0016868083f)), ((float)(0.0018348265f)),
     ((float)(0.0019841140f)), ((float)(0.0021461583f)), ((float)(0.0023017254f)), ((float)(0.0024625616f)),
     ((float)(0.0026201758f)), ((float)(0.0027870464f)), ((float)(0.0029469447f)), ((float)(0.0031125420f)),
     ((float)(0.0032739613f)), ((float)(0.0034418874f)), ((float)(0.0036008268f)), ((float)(0.0037603922f)),
     ((float)(0.0039207432f)), ((float)(0.0040819753f)), ((float)(0.0042264269f)), ((float)(0.0043730719f)),
     ((float)(0.0045209852f)), ((float)(0.0046606460f)), ((float)(0.0047932560f)), ((float)(0.0049137603f)),
     ((float)(0.0050393022f)), ((float)(0.0051407353f)), ((float)(0.0052461166f)), ((float)(0.0053471681f)),
     ((float)(0.0054196775f)), ((float)(0.0054876040f)), ((float)(0.0055475714f)), ((float)(0.0055938023f)),
     ((float)(0.0056220643f)), ((float)(0.0056455196f)), ((float)(0.0056389199f)), ((float)(0.0056266114f)),
     ((float)(0.0055917128f)), ((float)(0.0055404363f)), ((float)(0.0054753783f)), ((float)(0.0053838975f)),
     ((float)(0.0052715758f)), ((float)(0.0051382275f)), ((float)(0.0049839687f)), ((float)(0.0048109469f)),
     ((float)(0.0046039530f)), ((float)(0.0043801861f)), ((float)(0.0041251642f)), ((float)(0.0038456408f)),
     ((float)(0.0035401246f)), ((float)(0.0032091885f)), ((float)(0.0028446757f)), ((float)(0.0024508540f)),
     ((float)(0.0020274176f)), ((float)(0.0015784682f)), ((float)(0.0010902329f)), ((float)(0.0005832264f)),
     ((float)(0.0000276045f)), ((float)(-0.0005464280f)), ((float)(-0.0011568135f)), ((float)(-0.0018039472f)),
     ((float)(-0.0024826723f)), ((float)(-0.0031933778f)), ((float)(-0.0039401124f)), ((float)(-0.0047222596f)),
     ((float)(-0.0055337211f)), ((float)(-0.0063792293f)), ((float)(-0.0072615816f)), ((float)(-0.0081798233f)),
     ((float)(-0.0091325329f)), ((float)(-0.0101150215f)), ((float)(-0.0111315548f)), ((float)(-0.0121849995f)),
     ((float)(0.0132718220f)), ((float)(0.0143904666f)), ((float)(0.0155405553f)), ((float)(0.0167324712f)),
     ((float)(0.0179433381f)), ((float)(0.0191872431f)), ((float)(0.0204531793f)), ((float)(0.0217467550f)),
     ((float)(0.0230680169f)), ((float)(0.0244160992f)), ((float)(0.0257875847f)), ((float)(0.0271859429f)),
     ((float)(0.0286072173f)), ((float)(0.0300502657f)), ((float)(0.0315017608f)), ((float)(0.0329754081f)),
     ((float)(0.0344620948f)), ((float)(0.0359697560f)), ((float)(0.0374812850f)), ((float)(0.0390053679f)),
     ((float)(0.0405349170f)), ((float)(0.0420649094f)), ((float)(0.0436097542f)), ((float)(0.0451488405f)),
     ((float)(0.0466843027f)), ((float)(0.0482165720f)), ((float)(0.0497385755f)), ((float)(0.0512556155f)),
     ((float)(0.0527630746f)), ((float)(0.0542452768f)), ((float)(0.0557173648f)), ((float)(0.0571616450f)),
     ((float)(0.0585915683f)), ((float)(0.0599837480f)), ((float)(0.0613455171f)), ((float)(0.0626857808f)),
     ((float)(0.0639715898f)), ((float)(0.0652247106f)), ((float)(0.0664367512f)), ((float)(0.0676075985f)),
     ((float)(0.0687043828f)), ((float)(0.0697630244f)), ((float)(0.0707628710f)), ((float)(0.0717002673f)),
     ((float)(0.0725682583f)), ((float)(0.0733620255f)), ((float)(0.0741003642f)), ((float)(0.0747452558f)),
     ((float)(0.0753137336f)), ((float)(0.0758008358f)), ((float)(0.0761992479f)), ((float)(0.0764992170f)),
     ((float)(0.0767093490f)), ((float)(0.0768173975f)), ((float)(0.0768230011f)), ((float)(0.0767204924f)),
     ((float)(0.0765050718f)), ((float)(0.0761748321f)), ((float)(0.0757305756f)), ((float)(0.0751576255f)),
     ((float)(0.0744664394f)), ((float)(0.0736406005f)), ((float)(0.0726774642f)), ((float)(0.0715826364f)),
     ((float)(0.0703533073f)), ((float)(0.0689664013f)), ((float)(0.0674525021f)), ((float)(0.0657690668f)),
     ((float)(0.0639444805f)), ((float)(0.0619602779f)), ((float)(0.0598166570f)), ((float)(0.0575152691f)),
     ((float)(0.0550460034f)), ((float)(0.0524093821f)), ((float)(0.0495978676f)), ((float)(0.0466303305f)),
     ((float)(0.0434768782f)), ((float)(0.0401458278f)), ((float)(0.0366418116f)), ((float)(0.0329583930f)),
     ((float)(0.0290824006f)), ((float)(0.0250307561f)), ((float)(0.0207997072f)), ((float)(0.0163701258f)),
     ((float)(0.0117623832f)), ((float)(0.0069636862f)), ((float)(0.0019765601f)), ((float)(-0.0032086896f)),
     ((float)(-0.0085711749f)), ((float)(-0.0141288827f)), ((float)(-0.0198834129f)), ((float)(-0.0258227288f)),
     ((float)(-0.0319531274f)), ((float)(-0.0382776572f)), ((float)(-0.0447806821f)), ((float)(-0.0514804176f)),
     ((float)(-0.0583705326f)), ((float)(-0.0654409853f)), ((float)(-0.0726943300f)), ((float)(-0.0801372934f)),
     ((float)(-0.0877547536f)), ((float)(-0.0955533352f)), ((float)(-0.1035329531f)), ((float)(-0.1116826931f)),
     ((float)(-0.1200077984f)), ((float)(-0.1285002850f)), ((float)(-0.1371551761f)), ((float)(-0.1459766491f)),
     ((float)(-0.1549607071f)), ((float)(-0.1640958855f)), ((float)(-0.1733808172f)), ((float)(-0.1828172548f)),
     ((float)(-0.1923966745f)), ((float)(-0.2021250176f)), ((float)(-0.2119735853f)), ((float)(-0.2219652696f)),
     ((float)(-0.2320690870f)), ((float)(-0.2423016884f)), ((float)(-0.2526480309f)), ((float)(-0.2631053299f)),
     ((float)(-0.2736634040f)), ((float)(-0.2843214189f)), ((float)(-0.2950716717f)), ((float)(-0.3059098575f)),
     ((float)(-0.3168278913f)), ((float)(-0.3278113727f)), ((float)(-0.3388722693f)), ((float)(-0.3499914122f)),
     ((float)(0.3611589903f)), ((float)(0.3723795546f)), ((float)(0.3836350013f)), ((float)(0.3949211761f)),
     ((float)(0.4062317676f)), ((float)(0.4175696896f)), ((float)(0.4289119920f)), ((float)(0.4402553754f)),
     ((float)(0.4515996535f)), ((float)(0.4629308085f)), ((float)(0.4742453214f)), ((float)(0.4855253091f)),
     ((float)(0.4967708254f)), ((float)(0.5079817500f)), ((float)(0.5191234970f)), ((float)(0.5302240895f)),
     ((float)(0.5412553448f)), ((float)(0.5522051258f)), ((float)(0.5630789140f)), ((float)(0.5738524131f)),
     ((float)(0.5845403235f)), ((float)(0.5951123086f)), ((float)(0.6055783538f)), ((float)(0.6159109932f)),
     ((float)(0.6261242695f)), ((float)(0.6361980107f)), ((float)(0.6461269695f)), ((float)(0.6559016302f)),
     ((float)(0.6655139880f)), ((float)(0.6749663190f)), ((float)(0.6842353293f)), ((float)(0.6933282376f)),
     ((float)(0.7022388719f)), ((float)(0.7109410426f)), ((float)(0.7194462634f)), ((float)(0.7277448900f)),
     ((float)(0.7358211758f)), ((float)(0.7436827863f)), ((float)(0.7513137456f)), ((float)(0.7587080760f)),
     ((float)(0.7658674865f)), ((float)(0.7727780881f)), ((float)(0.7794287519f)), ((float)(0.7858353120f)),
     ((float)(0.7919735841f)), ((float)(0.7978466413f)), ((float)(0.8034485751f)), ((float)(0.8087695004f)),
     ((float)(0.8138191270f)), ((float)(0.8185776004f)), ((float)(0.8230419890f)), ((float)(0.8272275347f)),
     ((float)(0.8311038457f)), ((float)(0.8346937361f)), ((float)(0.8379717337f)), ((float)(0.8409541392f)),
     ((float)(0.8436238281f)), ((float)(0.8459818469f)), ((float)(0.8480315777f)), ((float)(0.8497805198f)),
     ((float)(0.8511971524f)), ((float)(0.8523047035f)), ((float)(0.8531020949f)), ((float)(0.8535720573f)),
     ((float)(0.8537385600f)), ((float)(0.8535720573f)), ((float)(0.8531020949f)), ((float)(0.8523047035f)),
     ((float)(0.8511971524f)), ((float)(0.8497805198f)), ((float)(0.8480315777f)), ((float)(0.8459818469f)),
     ((float)(0.8436238281f)), ((float)(0.8409541392f)), ((float)(0.8379717337f)), ((float)(0.8346937361f)),
     ((float)(0.8311038457f)), ((float)(0.8272275347f)), ((float)(0.8230419890f)), ((float)(0.8185776004f)),
     ((float)(0.8138191270f)), ((float)(0.8087695004f)), ((float)(0.8034485751f)), ((float)(0.7978466413f)),
     ((float)(0.7919735841f)), ((float)(0.7858353120f)), ((float)(0.7794287519f)), ((float)(0.7727780881f)),
     ((float)(0.7658674865f)), ((float)(0.7587080760f)), ((float)(0.7513137456f)), ((float)(0.7436827863f)),
     ((float)(0.7358211758f)), ((float)(0.7277448900f)), ((float)(0.7194462634f)), ((float)(0.7109410426f)),
     ((float)(0.7022388719f)), ((float)(0.6933282376f)), ((float)(0.6842353293f)), ((float)(0.6749663190f)),
     ((float)(0.6655139880f)), ((float)(0.6559016302f)), ((float)(0.6461269695f)), ((float)(0.6361980107f)),
     ((float)(0.6261242695f)), ((float)(0.6159109932f)), ((float)(0.6055783538f)), ((float)(0.5951123086f)),
     ((float)(0.5845403235f)), ((float)(0.5738524131f)), ((float)(0.5630789140f)), ((float)(0.5522051258f)),
     ((float)(0.5412553448f)), ((float)(0.5302240895f)), ((float)(0.5191234970f)), ((float)(0.5079817500f)),
     ((float)(0.4967708254f)), ((float)(0.4855253091f)), ((float)(0.4742453214f)), ((float)(0.4629308085f)),
     ((float)(0.4515996535f)), ((float)(0.4402553754f)), ((float)(0.4289119920f)), ((float)(0.4175696896f)),
     ((float)(0.4062317676f)), ((float)(0.3949211761f)), ((float)(0.3836350013f)), ((float)(0.3723795546f)),
    -((float)(0.3611589903f)), ((float)(-0.3499914122f)), ((float)(-0.3388722693f)), ((float)(-0.3278113727f)),
     ((float)(-0.3168278913f)), ((float)(-0.3059098575f)), ((float)(-0.2950716717f)), ((float)(-0.2843214189f)),
     ((float)(-0.2736634040f)), ((float)(-0.2631053299f)), ((float)(-0.2526480309f)), ((float)(-0.2423016884f)),
     ((float)(-0.2320690870f)), ((float)(-0.2219652696f)), ((float)(-0.2119735853f)), ((float)(-0.2021250176f)),
     ((float)(-0.1923966745f)), ((float)(-0.1828172548f)), ((float)(-0.1733808172f)), ((float)(-0.1640958855f)),
     ((float)(-0.1549607071f)), ((float)(-0.1459766491f)), ((float)(-0.1371551761f)), ((float)(-0.1285002850f)),
     ((float)(-0.1200077984f)), ((float)(-0.1116826931f)), ((float)(-0.1035329531f)), ((float)(-0.0955533352f)),
     ((float)(-0.0877547536f)), ((float)(-0.0801372934f)), ((float)(-0.0726943300f)), ((float)(-0.0654409853f)),
     ((float)(-0.0583705326f)), ((float)(-0.0514804176f)), ((float)(-0.0447806821f)), ((float)(-0.0382776572f)),
     ((float)(-0.0319531274f)), ((float)(-0.0258227288f)), ((float)(-0.0198834129f)), ((float)(-0.0141288827f)),
     ((float)(-0.0085711749f)), ((float)(-0.0032086896f)), ((float)(0.0019765601f)), ((float)(0.0069636862f)),
     ((float)(0.0117623832f)), ((float)(0.0163701258f)), ((float)(0.0207997072f)), ((float)(0.0250307561f)),
     ((float)(0.0290824006f)), ((float)(0.0329583930f)), ((float)(0.0366418116f)), ((float)(0.0401458278f)),
     ((float)(0.0434768782f)), ((float)(0.0466303305f)), ((float)(0.0495978676f)), ((float)(0.0524093821f)),
     ((float)(0.0550460034f)), ((float)(0.0575152691f)), ((float)(0.0598166570f)), ((float)(0.0619602779f)),
     ((float)(0.0639444805f)), ((float)(0.0657690668f)), ((float)(0.0674525021f)), ((float)(0.0689664013f)),
     ((float)(0.0703533073f)), ((float)(0.0715826364f)), ((float)(0.0726774642f)), ((float)(0.0736406005f)),
     ((float)(0.0744664394f)), ((float)(0.0751576255f)), ((float)(0.0757305756f)), ((float)(0.0761748321f)),
     ((float)(0.0765050718f)), ((float)(0.0767204924f)), ((float)(0.0768230011f)), ((float)(0.0768173975f)),
     ((float)(0.0767093490f)), ((float)(0.0764992170f)), ((float)(0.0761992479f)), ((float)(0.0758008358f)),
     ((float)(0.0753137336f)), ((float)(0.0747452558f)), ((float)(0.0741003642f)), ((float)(0.0733620255f)),
     ((float)(0.0725682583f)), ((float)(0.0717002673f)), ((float)(0.0707628710f)), ((float)(0.0697630244f)),
     ((float)(0.0687043828f)), ((float)(0.0676075985f)), ((float)(0.0664367512f)), ((float)(0.0652247106f)),
     ((float)(0.0639715898f)), ((float)(0.0626857808f)), ((float)(0.0613455171f)), ((float)(0.0599837480f)),
     ((float)(0.0585915683f)), ((float)(0.0571616450f)), ((float)(0.0557173648f)), ((float)(0.0542452768f)),
     ((float)(0.0527630746f)), ((float)(0.0512556155f)), ((float)(0.0497385755f)), ((float)(0.0482165720f)),
     ((float)(0.0466843027f)), ((float)(0.0451488405f)), ((float)(0.0436097542f)), ((float)(0.0420649094f)),
     ((float)(0.0405349170f)), ((float)(0.0390053679f)), ((float)(0.0374812850f)), ((float)(0.0359697560f)),
     ((float)(0.0344620948f)), ((float)(0.0329754081f)), ((float)(0.0315017608f)), ((float)(0.0300502657f)),
     ((float)(0.0286072173f)), ((float)(0.0271859429f)), ((float)(0.0257875847f)), ((float)(0.0244160992f)),
     ((float)(0.0230680169f)), ((float)(0.0217467550f)), ((float)(0.0204531793f)), ((float)(0.0191872431f)),
     ((float)(0.0179433381f)), ((float)(0.0167324712f)), ((float)(0.0155405553f)), ((float)(0.0143904666f)),
    -((float)(0.0132718220f)), ((float)(-0.0121849995f)), ((float)(-0.0111315548f)), ((float)(-0.0101150215f)),
     ((float)(-0.0091325329f)), ((float)(-0.0081798233f)), ((float)(-0.0072615816f)), ((float)(-0.0063792293f)),
     ((float)(-0.0055337211f)), ((float)(-0.0047222596f)), ((float)(-0.0039401124f)), ((float)(-0.0031933778f)),
     ((float)(-0.0024826723f)), ((float)(-0.0018039472f)), ((float)(-0.0011568135f)), ((float)(-0.0005464280f)),
     ((float)(0.0000276045f)), ((float)(0.0005832264f)), ((float)(0.0010902329f)), ((float)(0.0015784682f)),
     ((float)(0.0020274176f)), ((float)(0.0024508540f)), ((float)(0.0028446757f)), ((float)(0.0032091885f)),
     ((float)(0.0035401246f)), ((float)(0.0038456408f)), ((float)(0.0041251642f)), ((float)(0.0043801861f)),
     ((float)(0.0046039530f)), ((float)(0.0048109469f)), ((float)(0.0049839687f)), ((float)(0.0051382275f)),
     ((float)(0.0052715758f)), ((float)(0.0053838975f)), ((float)(0.0054753783f)), ((float)(0.0055404363f)),
     ((float)(0.0055917128f)), ((float)(0.0056266114f)), ((float)(0.0056389199f)), ((float)(0.0056455196f)),
     ((float)(0.0056220643f)), ((float)(0.0055938023f)), ((float)(0.0055475714f)), ((float)(0.0054876040f)),
     ((float)(0.0054196775f)), ((float)(0.0053471681f)), ((float)(0.0052461166f)), ((float)(0.0051407353f)),
     ((float)(0.0050393022f)), ((float)(0.0049137603f)), ((float)(0.0047932560f)), ((float)(0.0046606460f)),
     ((float)(0.0045209852f)), ((float)(0.0043730719f)), ((float)(0.0042264269f)), ((float)(0.0040819753f)),
     ((float)(0.0039207432f)), ((float)(0.0037603922f)), ((float)(0.0036008268f)), ((float)(0.0034418874f)),
     ((float)(0.0032739613f)), ((float)(0.0031125420f)), ((float)(0.0029469447f)), ((float)(0.0027870464f)),
     ((float)(0.0026201758f)), ((float)(0.0024625616f)), ((float)(0.0023017254f)), ((float)(0.0021461583f)),
     ((float)(0.0019841140f)), ((float)(0.0018348265f)), ((float)(0.0016868083f)), ((float)(0.0015443219f)),
     ((float)(0.0013902494f)), ((float)(0.0012577884f)), ((float)(0.0011250155f)), ((float)(0.0009885988f)),
     ((float)(0.0008608443f)), ((float)(0.0007458025f)), ((float)(0.0006239376f)), ((float)(0.0005107388f)),
     ((float)(0.0004026540f)), ((float)(0.0002949531f)), ((float)(0.0002043017f)), ((float)(0.0001094383f)),
     ((float)(0.0000134949f)), ((float)(-0.0000617334f)), ((float)(-0.0001446380f)), ((float)(-0.0002098337f)),
     ((float)(-0.0002896981f)), ((float)(-0.0003501175f)), ((float)(-0.0004095121f)), ((float)(-0.0004606325f)),
     ((float)(-0.0005145572f)), ((float)(-0.0005564576f)), ((float)(-0.0005946118f)), ((float)(-0.0006341594f)),
     ((float)(-0.0006650415f)), ((float)(-0.0006917937f)), ((float)(-0.0007215391f)), ((float)(-0.0007319357f)),
     ((float)(-0.0007530001f)), ((float)(-0.0007630793f)), ((float)(-0.0007757977f)), ((float)(-0.0007801449f)),
     ((float)(-0.0007803664f)), ((float)(-0.0007779869f)), ((float)(-0.0007834332f)), ((float)(-0.0007724848f)),
     ((float)(-0.0007681371f)), ((float)(-0.0007490598f)), ((float)(-0.0007440941f)), ((float)(-0.0007255043f)),
     ((float)(-0.0007157736f)), ((float)(-0.0006941614f)), ((float)(-0.0006777690f)), ((float)(-0.0006540333f)),
     ((float)(-0.0006312493f)), ((float)(-0.0006132747f)), ((float)(-0.0005870930f)), ((float)(-0.0005677802f)),
     ((float)(-0.0005466565f)), ((float)(-0.0005226564f)), ((float)(-0.0005040714f)), ((float)(-0.0004893791f)),
     ((float)(-0.0004875227f)), ((float)(-0.0004947518f)), ((float)(-0.0005617692f)), ((float)(-0.0005525286f)),
};
static VLC vlc_sbr[10];
static void aacsbr_func_ptr_init(AACSBRContext *c);

static void make_bands(int16_t* bands, int start, int stop, int num_bands)
{
    int k, previous, present;
    float base, prod;

    base = powf((float)stop / start, 1.0f / num_bands);
    prod = start;
    previous = start;

    for (k = 0; k < num_bands-1; k++) {
        prod *= base;
        present = lrintf(prod);
        bands[k] = present - previous;
        previous = present;
    }
    bands[num_bands-1] = stop - previous;
}


static void sbr_dequant(SpectralBandReplication *sbr, int id_aac)
{
    int k, e;
    int ch;
    static const double exp2_tab[2] = {1, 
                                         1.41421356237309504880
                                                };
    if (id_aac == TYPE_CPE && sbr->bs_coupling) {
        int pan_offset = sbr->data[0].bs_amp_res ? 12 : 24;
        for (e = 1; e <= sbr->data[0].bs_num_env; e++) {
            for (k = 0; k < sbr->n[sbr->data[0].bs_freq_res[e]]; k++) {
                float temp1, temp2, fac;
                if (sbr->data[0].bs_amp_res) {
                    temp1 = ff_exp2fi(sbr->data[0].env_facs_q[e][k] + 7);
                    temp2 = ff_exp2fi(pan_offset - sbr->data[1].env_facs_q[e][k]);
                }
                else {
                    temp1 = ff_exp2fi((sbr->data[0].env_facs_q[e][k]>>1) + 7) *
                            exp2_tab[sbr->data[0].env_facs_q[e][k] & 1];
                    temp2 = ff_exp2fi((pan_offset - sbr->data[1].env_facs_q[e][k])>>1) *
                            exp2_tab[(pan_offset - sbr->data[1].env_facs_q[e][k]) & 1];
                }
                if (temp1 > 1E20) {
                    av_log(
                          ((void *)0)
                              , 16, "envelope scalefactor overflow in dequant\n");
                    temp1 = 1;
                }
                fac = temp1 / (1.0f + temp2);
                sbr->data[0].env_facs[e][k] = fac;
                sbr->data[1].env_facs[e][k] = fac * temp2;
            }
        }
        for (e = 1; e <= sbr->data[0].bs_num_noise; e++) {
            for (k = 0; k < sbr->n_q; k++) {
                float temp1 = ff_exp2fi(6 - sbr->data[0].noise_facs_q[e][k] + 1);
                float temp2 = ff_exp2fi(12 - sbr->data[1].noise_facs_q[e][k]);
                float fac;
                do { if (!(temp1 <= 1E20)) { av_log(
               ((void *)0)
               , 0, "Assertion %s failed at %s:%d\n", "temp1 <= 1E20", "libavcodec/aacsbr.c", 107); abort(); } } while (0);
                fac = temp1 / (1.0f + temp2);
                sbr->data[0].noise_facs[e][k] = fac;
                sbr->data[1].noise_facs[e][k] = fac * temp2;
            }
        }
    } else {
        for (ch = 0; ch < (id_aac == TYPE_CPE) + 1; ch++) {
            for (e = 1; e <= sbr->data[ch].bs_num_env; e++)
                for (k = 0; k < sbr->n[sbr->data[ch].bs_freq_res[e]]; k++){
                    if (sbr->data[ch].bs_amp_res)
                        sbr->data[ch].env_facs[e][k] = ff_exp2fi(sbr->data[ch].env_facs_q[e][k] + 6);
                    else
                        sbr->data[ch].env_facs[e][k] = ff_exp2fi((sbr->data[ch].env_facs_q[e][k]>>1) + 6)
                                                       * exp2_tab[sbr->data[ch].env_facs_q[e][k] & 1];
                    if (sbr->data[ch].env_facs[e][k] > 1E20) {
                        av_log(
                              ((void *)0)
                                  , 16, "envelope scalefactor overflow in dequant\n");
                        sbr->data[ch].env_facs[e][k] = 1;
                    }
                }

            for (e = 1; e <= sbr->data[ch].bs_num_noise; e++)
                for (k = 0; k < sbr->n_q; k++)
                    sbr->data[ch].noise_facs[e][k] =
                        ff_exp2fi(6 - sbr->data[ch].noise_facs_q[e][k]);
        }
    }
}





static void sbr_hf_inverse_filter(SBRDSPContext *dsp,
                                  float (*alpha0)[2], float (*alpha1)[2],
                                  const float X_low[32][40][2], int k0)
{
    int k;
    for (k = 0; k < k0; k++) {
        float __attribute__ ((aligned (16))) la_phi [3] [2][2]; float (*phi) [2][2] = la_phi;
        float dk;

        dsp->autocorrelate(X_low[k], phi);

        dk = phi[2][1][0] * phi[1][0][0] -
             (phi[1][1][0] * phi[1][1][0] + phi[1][1][1] * phi[1][1][1]) / 1.000001f;

        if (!dk) {
            alpha1[k][0] = 0;
            alpha1[k][1] = 0;
        } else {
            float temp_real, temp_im;
            temp_real = phi[0][0][0] * phi[1][1][0] -
                        phi[0][0][1] * phi[1][1][1] -
                        phi[0][1][0] * phi[1][0][0];
            temp_im = phi[0][0][0] * phi[1][1][1] +
                        phi[0][0][1] * phi[1][1][0] -
                        phi[0][1][1] * phi[1][0][0];

            alpha1[k][0] = temp_real / dk;
            alpha1[k][1] = temp_im / dk;
        }

        if (!phi[1][0][0]) {
            alpha0[k][0] = 0;
            alpha0[k][1] = 0;
        } else {
            float temp_real, temp_im;
            temp_real = phi[0][0][0] + alpha1[k][0] * phi[1][1][0] +
                                       alpha1[k][1] * phi[1][1][1];
            temp_im = phi[0][0][1] + alpha1[k][1] * phi[1][1][0] -
                                       alpha1[k][0] * phi[1][1][1];

            alpha0[k][0] = -temp_real / phi[1][0][0];
            alpha0[k][1] = -temp_im / phi[1][0][0];
        }

        if (alpha1[k][0] * alpha1[k][0] + alpha1[k][1] * alpha1[k][1] >= 16.0f ||
           alpha0[k][0] * alpha0[k][0] + alpha0[k][1] * alpha0[k][1] >= 16.0f) {
            alpha1[k][0] = 0;
            alpha1[k][1] = 0;
            alpha0[k][0] = 0;
            alpha0[k][1] = 0;
        }
    }
}


static void sbr_chirp(SpectralBandReplication *sbr, SBRData *ch_data)
{
    int i;
    float new_bw;
    static const float bw_tab[] = { 0.0f, 0.75f, 0.9f, 0.98f };

    for (i = 0; i < sbr->n_q; i++) {
        if (ch_data->bs_invf_mode[0][i] + ch_data->bs_invf_mode[1][i] == 1) {
            new_bw = 0.6f;
        } else
            new_bw = bw_tab[ch_data->bs_invf_mode[0][i]];

        if (new_bw < ch_data->bw_array[i]) {
            new_bw = 0.75f * new_bw + 0.25f * ch_data->bw_array[i];
        } else
            new_bw = 0.90625f * new_bw + 0.09375f * ch_data->bw_array[i];
        ch_data->bw_array[i] = new_bw < 0.015625f ? 0.0f : new_bw;
    }
}





static void sbr_gain_calc(AACContext *ac, SpectralBandReplication *sbr,
                          SBRData *ch_data, const int e_a[2])
{
    int e, k, m;

    static const float limgain[4] = { 0.70795, 1.0, 1.41254, 10000000000 };

    for (e = 0; e < ch_data->bs_num_env; e++) {
        int delta = !((e == e_a[1]) || (e == e_a[0]));
        for (k = 0; k < sbr->n_lim; k++) {
            float gain_boost, gain_max;
            float sum[2] = { 0.0f, 0.0f };
            for (m = sbr->f_tablelim[k] - sbr->kx[1]; m < sbr->f_tablelim[k + 1] - sbr->kx[1]; m++) {
                const float temp = sbr->e_origmapped[e][m] / (1.0f + sbr->q_mapped[e][m]);
                sbr->q_m[e][m] = sqrtf(temp * sbr->q_mapped[e][m]);
                sbr->s_m[e][m] = sqrtf(temp * ch_data->s_indexmapped[e + 1][m]);
                if (!sbr->s_mapped[e][m]) {
                    sbr->gain[e][m] = sqrtf(sbr->e_origmapped[e][m] /
                                            ((1.0f + sbr->e_curr[e][m]) *
                                             (1.0f + sbr->q_mapped[e][m] * delta)));
                } else {
                    sbr->gain[e][m] = sqrtf(sbr->e_origmapped[e][m] * sbr->q_mapped[e][m] /
                                            ((1.0f + sbr->e_curr[e][m]) *
                                             (1.0f + sbr->q_mapped[e][m])));
                }
                sbr->gain[e][m] += 1.17549435082228750796873653722224568e-38F
                                         ;
            }
            for (m = sbr->f_tablelim[k] - sbr->kx[1]; m < sbr->f_tablelim[k + 1] - sbr->kx[1]; m++) {
                sum[0] += sbr->e_origmapped[e][m];
                sum[1] += sbr->e_curr[e][m];
            }
            gain_max = limgain[sbr->bs_limiter_gains] * sqrtf((1.19209289550781250000000000000000000e-7F 
                                                                          + sum[0]) / (1.19209289550781250000000000000000000e-7F 
                                                                                                   + sum[1]));
            gain_max = ((100000.f) > (gain_max) ? (gain_max) : (100000.f));
            for (m = sbr->f_tablelim[k] - sbr->kx[1]; m < sbr->f_tablelim[k + 1] - sbr->kx[1]; m++) {
                float q_m_max = sbr->q_m[e][m] * gain_max / sbr->gain[e][m];
                sbr->q_m[e][m] = ((sbr->q_m[e][m]) > (q_m_max) ? (q_m_max) : (sbr->q_m[e][m]));
                sbr->gain[e][m] = ((sbr->gain[e][m]) > (gain_max) ? (gain_max) : (sbr->gain[e][m]));
            }
            sum[0] = sum[1] = 0.0f;
            for (m = sbr->f_tablelim[k] - sbr->kx[1]; m < sbr->f_tablelim[k + 1] - sbr->kx[1]; m++) {
                sum[0] += sbr->e_origmapped[e][m];
                sum[1] += sbr->e_curr[e][m] * sbr->gain[e][m] * sbr->gain[e][m]
                          + sbr->s_m[e][m] * sbr->s_m[e][m]
                          + (delta && !sbr->s_m[e][m]) * sbr->q_m[e][m] * sbr->q_m[e][m];
            }
            gain_boost = sqrtf((1.19209289550781250000000000000000000e-7F 
                                           + sum[0]) / (1.19209289550781250000000000000000000e-7F 
                                                                    + sum[1]));
            gain_boost = ((1.584893192f) > (gain_boost) ? (gain_boost) : (1.584893192f));
            for (m = sbr->f_tablelim[k] - sbr->kx[1]; m < sbr->f_tablelim[k + 1] - sbr->kx[1]; m++) {
                sbr->gain[e][m] *= gain_boost;
                sbr->q_m[e][m] *= gain_boost;
                sbr->s_m[e][m] *= gain_boost;
            }
        }
    }
}


static void sbr_hf_assemble(float Y1[38][64][2],
                            const float X_high[64][40][2],
                            SpectralBandReplication *sbr, SBRData *ch_data,
                            const int e_a[2])
{
    int e, i, j, m;
    const int h_SL = 4 * !sbr->bs_smoothing_mode;
    const int kx = sbr->kx[1];
    const int m_max = sbr->m[1];
    static const float h_smooth[5] = {
        0.33333333333333,
        0.30150283239582,
        0.21816949906249,
        0.11516383427084,
        0.03183050093751,
    };
    float (*g_temp)[48] = ch_data->g_temp, (*q_temp)[48] = ch_data->q_temp;
    int indexnoise = ch_data->f_indexnoise;
    int indexsine = ch_data->f_indexsine;

    if (sbr->reset) {
        for (i = 0; i < h_SL; i++) {
            memcpy(g_temp[i + 2*ch_data->t_env[0]], sbr->gain[0], m_max * sizeof(sbr->gain[0][0]));
            memcpy(q_temp[i + 2*ch_data->t_env[0]], sbr->q_m[0], m_max * sizeof(sbr->q_m[0][0]));
        }
    } else if (h_SL) {
        for (i = 0; i < 4; i++) {
            memcpy(g_temp[i + 2 * ch_data->t_env[0]],
                   g_temp[i + 2 * ch_data->t_env_num_env_old],
                   sizeof(g_temp[0]));
            memcpy(q_temp[i + 2 * ch_data->t_env[0]],
                   q_temp[i + 2 * ch_data->t_env_num_env_old],
                   sizeof(q_temp[0]));
        }
    }

    for (e = 0; e < ch_data->bs_num_env; e++) {
        for (i = 2 * ch_data->t_env[e]; i < 2 * ch_data->t_env[e + 1]; i++) {
            memcpy(g_temp[h_SL + i], sbr->gain[e], m_max * sizeof(sbr->gain[0][0]));
            memcpy(q_temp[h_SL + i], sbr->q_m[e], m_max * sizeof(sbr->q_m[0][0]));
        }
    }

    for (e = 0; e < ch_data->bs_num_env; e++) {
        for (i = 2 * ch_data->t_env[e]; i < 2 * ch_data->t_env[e + 1]; i++) {
            float __attribute__ ((aligned (16))) la_g_filt_tab [48] ; float (*g_filt_tab) = la_g_filt_tab;
            float __attribute__ ((aligned (16))) la_q_filt_tab [48] ; float (*q_filt_tab) = la_q_filt_tab;
            float *g_filt, *q_filt;

            if (h_SL && e != e_a[0] && e != e_a[1]) {
                g_filt = g_filt_tab;
                q_filt = q_filt_tab;
                for (m = 0; m < m_max; m++) {
                    const int idx1 = i + h_SL;
                    g_filt[m] = 0.0f;
                    q_filt[m] = 0.0f;
                    for (j = 0; j <= h_SL; j++) {
                        g_filt[m] += g_temp[idx1 - j][m] * h_smooth[j];
                        q_filt[m] += q_temp[idx1 - j][m] * h_smooth[j];
                    }
                }
            } else {
                g_filt = g_temp[i + h_SL];
                q_filt = q_temp[i];
            }

            sbr->dsp.hf_g_filt(Y1[i] + kx, X_high + kx, g_filt, m_max,
                               i + 2);

            if (e != e_a[0] && e != e_a[1]) {
                sbr->dsp.hf_apply_noise[indexsine](Y1[i] + kx, sbr->s_m[e],
                                                   q_filt, indexnoise,
                                                   kx, m_max);
            } else {
                int idx = indexsine&1;
                int A = (1-((indexsine+(kx & 1))&2));
                int B = (A^(-idx)) + idx;
                float *out = &Y1[i][kx][idx];
                float *in = sbr->s_m[e];
                for (m = 0; m+1 < m_max; m+=2) {
                    out[2*m ] += in[m ] * A;
                    out[2*m+2] += in[m+1] * B;
                }
                if(m_max&1)
                    out[2*m ] += in[m ] * A;
            }
            indexnoise = (indexnoise + m_max) & 0x1ff;
            indexsine = (indexsine + 1) & 3;
        }
    }
    ch_data->f_indexnoise = indexnoise;
    ch_data->f_indexsine = indexsine;
}

static __attribute__((cold)) void aacsbr_tableinit(void)
{
    int n;

    for (n = 0; n < 320; n++)
        sbr_qmf_window_ds[n] = sbr_qmf_window_us[2*n];
}

__attribute__((cold)) void ff_aac_sbr_init(void)
{
    static const struct {
        const void *sbr_codes, *sbr_bits;
        const unsigned int table_size, elem_size;
    } sbr_tmp[] = {
        { t_huffman_env_1_5dB_codes, t_huffman_env_1_5dB_bits, sizeof(t_huffman_env_1_5dB_codes), sizeof(t_huffman_env_1_5dB_codes[0]) },
        { f_huffman_env_1_5dB_codes, f_huffman_env_1_5dB_bits, sizeof(f_huffman_env_1_5dB_codes), sizeof(f_huffman_env_1_5dB_codes[0]) },
        { t_huffman_env_bal_1_5dB_codes, t_huffman_env_bal_1_5dB_bits, sizeof(t_huffman_env_bal_1_5dB_codes), sizeof(t_huffman_env_bal_1_5dB_codes[0]) },
        { f_huffman_env_bal_1_5dB_codes, f_huffman_env_bal_1_5dB_bits, sizeof(f_huffman_env_bal_1_5dB_codes), sizeof(f_huffman_env_bal_1_5dB_codes[0]) },
        { t_huffman_env_3_0dB_codes, t_huffman_env_3_0dB_bits, sizeof(t_huffman_env_3_0dB_codes), sizeof(t_huffman_env_3_0dB_codes[0]) },
        { f_huffman_env_3_0dB_codes, f_huffman_env_3_0dB_bits, sizeof(f_huffman_env_3_0dB_codes), sizeof(f_huffman_env_3_0dB_codes[0]) },
        { t_huffman_env_bal_3_0dB_codes, t_huffman_env_bal_3_0dB_bits, sizeof(t_huffman_env_bal_3_0dB_codes), sizeof(t_huffman_env_bal_3_0dB_codes[0]) },
        { f_huffman_env_bal_3_0dB_codes, f_huffman_env_bal_3_0dB_bits, sizeof(f_huffman_env_bal_3_0dB_codes), sizeof(f_huffman_env_bal_3_0dB_codes[0]) },
        { t_huffman_noise_3_0dB_codes, t_huffman_noise_3_0dB_bits, sizeof(t_huffman_noise_3_0dB_codes), sizeof(t_huffman_noise_3_0dB_codes[0]) },
        { t_huffman_noise_bal_3_0dB_codes, t_huffman_noise_bal_3_0dB_bits, sizeof(t_huffman_noise_bal_3_0dB_codes), sizeof(t_huffman_noise_bal_3_0dB_codes[0]) },
    };


    do { static int16_t table[1098][2]; (&vlc_sbr[0])->table = table; (&vlc_sbr[0])->table_allocated = 1098; ff_init_vlc_sparse(&vlc_sbr[0], 9, sbr_tmp[0].table_size / sbr_tmp[0].elem_size, sbr_tmp[0].sbr_bits, 1, 1, sbr_tmp[0].sbr_codes, sbr_tmp[0].elem_size, sbr_tmp[0].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[1092][2]; (&vlc_sbr[1])->table = table; (&vlc_sbr[1])->table_allocated = 1092; ff_init_vlc_sparse(&vlc_sbr[1], 9, sbr_tmp[1].table_size / sbr_tmp[1].elem_size, sbr_tmp[1].sbr_bits, 1, 1, sbr_tmp[1].sbr_codes, sbr_tmp[1].elem_size, sbr_tmp[1].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[768][2]; (&vlc_sbr[2])->table = table; (&vlc_sbr[2])->table_allocated = 768; ff_init_vlc_sparse(&vlc_sbr[2], 9, sbr_tmp[2].table_size / sbr_tmp[2].elem_size, sbr_tmp[2].sbr_bits, 1, 1, sbr_tmp[2].sbr_codes, sbr_tmp[2].elem_size, sbr_tmp[2].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[1026][2]; (&vlc_sbr[3])->table = table; (&vlc_sbr[3])->table_allocated = 1026; ff_init_vlc_sparse(&vlc_sbr[3], 9, sbr_tmp[3].table_size / sbr_tmp[3].elem_size, sbr_tmp[3].sbr_bits, 1, 1, sbr_tmp[3].sbr_codes, sbr_tmp[3].elem_size, sbr_tmp[3].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[1058][2]; (&vlc_sbr[4])->table = table; (&vlc_sbr[4])->table_allocated = 1058; ff_init_vlc_sparse(&vlc_sbr[4], 9, sbr_tmp[4].table_size / sbr_tmp[4].elem_size, sbr_tmp[4].sbr_bits, 1, 1, sbr_tmp[4].sbr_codes, sbr_tmp[4].elem_size, sbr_tmp[4].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[1052][2]; (&vlc_sbr[5])->table = table; (&vlc_sbr[5])->table_allocated = 1052; ff_init_vlc_sparse(&vlc_sbr[5], 9, sbr_tmp[5].table_size / sbr_tmp[5].elem_size, sbr_tmp[5].sbr_bits, 1, 1, sbr_tmp[5].sbr_codes, sbr_tmp[5].elem_size, sbr_tmp[5].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[544][2]; (&vlc_sbr[6])->table = table; (&vlc_sbr[6])->table_allocated = 544; ff_init_vlc_sparse(&vlc_sbr[6], 9, sbr_tmp[6].table_size / sbr_tmp[6].elem_size, sbr_tmp[6].sbr_bits, 1, 1, sbr_tmp[6].sbr_codes, sbr_tmp[6].elem_size, sbr_tmp[6].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[544][2]; (&vlc_sbr[7])->table = table; (&vlc_sbr[7])->table_allocated = 544; ff_init_vlc_sparse(&vlc_sbr[7], 9, sbr_tmp[7].table_size / sbr_tmp[7].elem_size, sbr_tmp[7].sbr_bits, 1, 1, sbr_tmp[7].sbr_codes, sbr_tmp[7].elem_size, sbr_tmp[7].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[592][2]; (&vlc_sbr[8])->table = table; (&vlc_sbr[8])->table_allocated = 592; ff_init_vlc_sparse(&vlc_sbr[8], 9, sbr_tmp[8].table_size / sbr_tmp[8].elem_size, sbr_tmp[8].sbr_bits, 1, 1, sbr_tmp[8].sbr_codes, sbr_tmp[8].elem_size, sbr_tmp[8].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);
    do { static int16_t table[512][2]; (&vlc_sbr[9])->table = table; (&vlc_sbr[9])->table_allocated = 512; ff_init_vlc_sparse(&vlc_sbr[9], 9, sbr_tmp[9].table_size / sbr_tmp[9].elem_size, sbr_tmp[9].sbr_bits, 1, 1, sbr_tmp[9].sbr_codes, sbr_tmp[9].elem_size, sbr_tmp[9].elem_size, 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0);

    aacsbr_tableinit();

    ff_ps_init();
}


static void sbr_turnoff(SpectralBandReplication *sbr) {
    sbr->start = 0;
    sbr->ready_for_dequant = 0;

    sbr->kx[1] = 32;
    sbr->m[1] = 0;

    sbr->data[0].e_a[1] = sbr->data[1].e_a[1] = -1;
    memset(&sbr->spectrum_params, -1, sizeof(SpectrumParameters));
}

__attribute__((cold)) void ff_aac_sbr_ctx_init(AACContext *ac, SpectralBandReplication *sbr, int id_aac)
{
    if(sbr->mdct.mdct_bits)
        return;
    sbr->kx[0] = sbr->kx[1];
    sbr->id_aac = id_aac;
    sbr_turnoff(sbr);
    sbr->data[0].synthesis_filterbank_samples_offset = ((1280-128)*2) - (1280 - 128);
    sbr->data[1].synthesis_filterbank_samples_offset = ((1280-128)*2) - (1280 - 128);



    ff_mdct_init(&sbr->mdct, 7, 1, 1.0 / (64 * 32768.0));
    ff_mdct_init(&sbr->mdct_ana, 7, 1, -2.0 * 32768.0);
    ff_ps_ctx_init(&sbr->ps);
    ff_sbrdsp_init(&sbr->dsp);
    aacsbr_func_ptr_init(&sbr->c);
}

__attribute__((cold)) void ff_aac_sbr_ctx_close(SpectralBandReplication *sbr)
{
    ff_mdct_end(&sbr->mdct);
    ff_mdct_end(&sbr->mdct_ana);
}

static int qsort_comparison_function_int16(const void *a, const void *b)
{
    return *(const int16_t *)a - *(const int16_t *)b;
}

static inline int in_table_int16(const int16_t *table, int last_el, int16_t needle)
{
    int i;
    for (i = 0; i <= last_el; i++)
        if (table[i] == needle)
            return 1;
    return 0;
}


static void sbr_make_f_tablelim(SpectralBandReplication *sbr)
{
    int k;
    if (sbr->bs_limiter_bands > 0) {
        static const INTFLOAT bands_warped[3] = { ((float)(1.32715174233856803909f)),
                                               ((float)(1.18509277094158210129f)),
                                               ((float)(1.11987160404675912501f)) };
        const INTFLOAT lim_bands_per_octave_warped = bands_warped[sbr->bs_limiter_bands - 1];
        int16_t patch_borders[7];
        uint16_t *in = sbr->f_tablelim + 1, *out = sbr->f_tablelim;

        patch_borders[0] = sbr->kx[1];
        for (k = 1; k <= sbr->num_patches; k++)
            patch_borders[k] = patch_borders[k-1] + sbr->patch_num_subbands[k-1];

        memcpy(sbr->f_tablelim, sbr->f_tablelow,
               (sbr->n[0] + 1) * sizeof(sbr->f_tablelow[0]));
        if (sbr->num_patches > 1)
            memcpy(sbr->f_tablelim + sbr->n[0] + 1, patch_borders + 1,
                   (sbr->num_patches - 1) * sizeof(patch_borders[0]));

        do { void *stack[64][2]; int sp= 1; stack[0][0] = sbr->f_tablelim; stack[0][1] = (sbr->f_tablelim)+(sbr->num_patches + sbr->n[0])-1; while(sp){ uint16_t *start= stack[--sp][0]; uint16_t *end = stack[ sp][1]; while(start < end){ if(start < end-1) { int checksort=0; uint16_t *right = end-2; uint16_t *left = start+1; uint16_t *mid = start + ((end-start)>>1); if(qsort_comparison_function_int16(start, end) > 0) { if(qsort_comparison_function_int16( end, mid) > 0) do{uint16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else do{uint16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); }else{ if(qsort_comparison_function_int16(start, mid) > 0) do{uint16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else checksort= 1; } if(qsort_comparison_function_int16(mid, end) > 0){ do{uint16_t SWAP_tmp= *end; *end= *mid; *mid= SWAP_tmp;}while(0); checksort=0; } if(start == end-2) break; do{uint16_t SWAP_tmp= *mid; *mid= end[-1]; end[-1]= SWAP_tmp;}while(0); while(left <= right){ while(left<=right && qsort_comparison_function_int16(left, end-1) < 0) left++; while(left<=right && qsort_comparison_function_int16(right, end-1) > 0) right--; if(left <= right){ do{uint16_t SWAP_tmp= *right; *right= *left; *left= SWAP_tmp;}while(0); left++; right--; } } do{uint16_t SWAP_tmp= *left; *left= end[-1]; end[-1]= SWAP_tmp;}while(0); if(checksort && (mid == left-1 || mid == left)){ mid= start; while(mid<end && qsort_comparison_function_int16(mid, mid+1) <= 0) mid++; if(mid==end) break; } if(end-left < left-start){ stack[sp ][0]= start; stack[sp++][1]= right; start = left+1; }else{ stack[sp ][0]= left+1; stack[sp++][1]= end; end = right; } }else{ if(qsort_comparison_function_int16(start, end) > 0) do{uint16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); break; } } }} while (0)

                                              ;

        sbr->n_lim = sbr->n[0] + sbr->num_patches - 1;
        while (out < sbr->f_tablelim + sbr->n_lim) {



            if (*in >= *out * lim_bands_per_octave_warped) {

                *++out = *in++;
            } else if (*in == *out ||
                !in_table_int16(patch_borders, sbr->num_patches, *in)) {
                in++;
                sbr->n_lim--;
            } else if (!in_table_int16(patch_borders, sbr->num_patches, *out)) {
                *out = *in++;
                sbr->n_lim--;
            } else {
                *++out = *in++;
            }
        }
    } else {
        sbr->f_tablelim[0] = sbr->f_tablelow[0];
        sbr->f_tablelim[1] = sbr->f_tablelow[sbr->n[0]];
        sbr->n_lim = 1;
    }
}

static unsigned int read_sbr_header(SpectralBandReplication *sbr, GetBitContext *gb)
{
    unsigned int cnt = get_bits_count(gb);
    uint8_t bs_header_extra_1;
    uint8_t bs_header_extra_2;
    int old_bs_limiter_bands = sbr->bs_limiter_bands;
    SpectrumParameters old_spectrum_params;

    sbr->start = 1;
    sbr->ready_for_dequant = 0;


    memcpy(&old_spectrum_params, &sbr->spectrum_params, sizeof(SpectrumParameters));

    sbr->bs_amp_res_header = get_bits1(gb);
    sbr->spectrum_params.bs_start_freq = get_bits(gb, 4);
    sbr->spectrum_params.bs_stop_freq = get_bits(gb, 4);
    sbr->spectrum_params.bs_xover_band = get_bits(gb, 3);
                                          skip_bits(gb, 2);

    bs_header_extra_1 = get_bits1(gb);
    bs_header_extra_2 = get_bits1(gb);

    if (bs_header_extra_1) {
        sbr->spectrum_params.bs_freq_scale = get_bits(gb, 2);
        sbr->spectrum_params.bs_alter_scale = get_bits1(gb);
        sbr->spectrum_params.bs_noise_bands = get_bits(gb, 2);
    } else {
        sbr->spectrum_params.bs_freq_scale = 2;
        sbr->spectrum_params.bs_alter_scale = 1;
        sbr->spectrum_params.bs_noise_bands = 2;
    }


    if (memcmp(&old_spectrum_params, &sbr->spectrum_params, sizeof(SpectrumParameters)))
        sbr->reset = 1;

    if (bs_header_extra_2) {
        sbr->bs_limiter_bands = get_bits(gb, 2);
        sbr->bs_limiter_gains = get_bits(gb, 2);
        sbr->bs_interpol_freq = get_bits1(gb);
        sbr->bs_smoothing_mode = get_bits1(gb);
    } else {
        sbr->bs_limiter_bands = 2;
        sbr->bs_limiter_gains = 2;
        sbr->bs_interpol_freq = 1;
        sbr->bs_smoothing_mode = 1;
    }

    if (sbr->bs_limiter_bands != old_bs_limiter_bands && !sbr->reset)
        sbr_make_f_tablelim(sbr);

    return get_bits_count(gb) - cnt;
}

static int array_min_int16(const int16_t *array, int nel)
{
    int i, min = array[0];
    for (i = 1; i < nel; i++)
        min = ((array[i]) > (min) ? (min) : (array[i]));
    return min;
}

static int check_n_master(AVCodecContext *avctx, int n_master, int bs_xover_band)
{

    if (n_master <= 0) {
        av_log(avctx, 16, "Invalid n_master: %d\n", n_master);
        return -1;
    }
    if (bs_xover_band >= n_master) {
        av_log(avctx, 16,
               "Invalid bitstream, crossover band index beyond array bounds: %d\n",
               bs_xover_band);
        return -1;
    }
    return 0;
}


static int sbr_make_f_master(AACContext *ac, SpectralBandReplication *sbr,
                             SpectrumParameters *spectrum)
{
    unsigned int temp, max_qmf_subbands = 0;
    unsigned int start_min, stop_min;
    int k;
    const int8_t *sbr_offset_ptr;
    int16_t stop_dk[13];

    switch (sbr->sample_rate) {
    case 16000:
        sbr_offset_ptr = sbr_offset[0];
        break;
    case 22050:
        sbr_offset_ptr = sbr_offset[1];
        break;
    case 24000:
        sbr_offset_ptr = sbr_offset[2];
        break;
    case 32000:
        sbr_offset_ptr = sbr_offset[3];
        break;
    case 44100: case 48000: case 64000:
        sbr_offset_ptr = sbr_offset[4];
        break;
    case 88200: case 96000: case 128000: case 176400: case 192000:
        sbr_offset_ptr = sbr_offset[5];
        break;
    default:
        av_log(ac->avctx, 16,
               "Unsupported sample rate for SBR: %d\n", sbr->sample_rate);
        return -1;
    }

    if (sbr->sample_rate < 32000) {
        temp = 3000;
    } else if (sbr->sample_rate < 64000) {
        temp = 4000;
    } else
        temp = 5000;

    start_min = ((temp << 7) + (sbr->sample_rate >> 1)) / sbr->sample_rate;
    stop_min = ((temp << 8) + (sbr->sample_rate >> 1)) / sbr->sample_rate;

    sbr->k[0] = start_min + sbr_offset_ptr[spectrum->bs_start_freq];

    if (spectrum->bs_stop_freq < 14) {
        sbr->k[2] = stop_min;
        make_bands(stop_dk, stop_min, 64, 13);
        do { void *stack[64][2]; int sp= 1; stack[0][0] = stop_dk; stack[0][1] = (stop_dk)+(13)-1; while(sp){ int16_t *start= stack[--sp][0]; int16_t *end = stack[ sp][1]; while(start < end){ if(start < end-1) { int checksort=0; int16_t *right = end-2; int16_t *left = start+1; int16_t *mid = start + ((end-start)>>1); if(qsort_comparison_function_int16(start, end) > 0) { if(qsort_comparison_function_int16( end, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); }else{ if(qsort_comparison_function_int16(start, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else checksort= 1; } if(qsort_comparison_function_int16(mid, end) > 0){ do{int16_t SWAP_tmp= *end; *end= *mid; *mid= SWAP_tmp;}while(0); checksort=0; } if(start == end-2) break; do{int16_t SWAP_tmp= *mid; *mid= end[-1]; end[-1]= SWAP_tmp;}while(0); while(left <= right){ while(left<=right && qsort_comparison_function_int16(left, end-1) < 0) left++; while(left<=right && qsort_comparison_function_int16(right, end-1) > 0) right--; if(left <= right){ do{int16_t SWAP_tmp= *right; *right= *left; *left= SWAP_tmp;}while(0); left++; right--; } } do{int16_t SWAP_tmp= *left; *left= end[-1]; end[-1]= SWAP_tmp;}while(0); if(checksort && (mid == left-1 || mid == left)){ mid= start; while(mid<end && qsort_comparison_function_int16(mid, mid+1) <= 0) mid++; if(mid==end) break; } if(end-left < left-start){ stack[sp ][0]= start; stack[sp++][1]= right; start = left+1; }else{ stack[sp ][0]= left+1; stack[sp++][1]= end; end = right; } }else{ if(qsort_comparison_function_int16(start, end) > 0) do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); break; } } }} while (0);
        for (k = 0; k < spectrum->bs_stop_freq; k++)
            sbr->k[2] += stop_dk[k];
    } else if (spectrum->bs_stop_freq == 14) {
        sbr->k[2] = 2*sbr->k[0];
    } else if (spectrum->bs_stop_freq == 15) {
        sbr->k[2] = 3*sbr->k[0];
    } else {
        av_log(ac->avctx, 16,
               "Invalid bs_stop_freq: %d\n", spectrum->bs_stop_freq);
        return -1;
    }
    sbr->k[2] = ((64) > (sbr->k[2]) ? (sbr->k[2]) : (64));


    if (sbr->sample_rate <= 32000) {
        max_qmf_subbands = 48;
    } else if (sbr->sample_rate == 44100) {
        max_qmf_subbands = 35;
    } else if (sbr->sample_rate >= 48000)
        max_qmf_subbands = 32;
    else
        do { if (!(0)) { av_log(
       ((void *)0)
       , 0, "Assertion %s failed at %s:%d\n", "0", "libavcodec/aacsbr_template.c", 334); abort(); } } while (0);

    if (sbr->k[2] - sbr->k[0] > max_qmf_subbands) {
        av_log(ac->avctx, 16,
               "Invalid bitstream, too many QMF subbands: %d\n", sbr->k[2] - sbr->k[0]);
        return -1;
    }

    if (!spectrum->bs_freq_scale) {
        int dk, k2diff;

        dk = spectrum->bs_alter_scale + 1;
        sbr->n_master = ((sbr->k[2] - sbr->k[0] + (dk&2)) >> dk) << 1;
        if (check_n_master(ac->avctx, sbr->n_master, sbr->spectrum_params.bs_xover_band))
            return -1;

        for (k = 1; k <= sbr->n_master; k++)
            sbr->f_master[k] = dk;

        k2diff = sbr->k[2] - sbr->k[0] - sbr->n_master * dk;
        if (k2diff < 0) {
            sbr->f_master[1]--;
            sbr->f_master[2]-= (k2diff < -1);
        } else if (k2diff) {
            sbr->f_master[sbr->n_master]++;
        }

        sbr->f_master[0] = sbr->k[0];
        for (k = 1; k <= sbr->n_master; k++)
            sbr->f_master[k] += sbr->f_master[k - 1];

    } else {
        int half_bands = 7 - spectrum->bs_freq_scale;
        int two_regions, num_bands_0;
        int vdk0_max, vdk1_min;
        int16_t vk0[49];




        if (49 * sbr->k[2] > 110 * sbr->k[0]) {
            two_regions = 1;
            sbr->k[1] = 2 * sbr->k[0];
        } else {
            two_regions = 0;
            sbr->k[1] = sbr->k[2];
        }
        num_bands_0 = lrintf(half_bands * log2f(sbr->k[1] / (float)sbr->k[0])) * 2;


        if (num_bands_0 <= 0) {
            av_log(ac->avctx, 16, "Invalid num_bands_0: %d\n", num_bands_0);
            return -1;
        }

        vk0[0] = 0;

        make_bands(vk0+1, sbr->k[0], sbr->k[1], num_bands_0);

        do { void *stack[64][2]; int sp= 1; stack[0][0] = vk0 + 1; stack[0][1] = (vk0 + 1)+(num_bands_0)-1; while(sp){ int16_t *start= stack[--sp][0]; int16_t *end = stack[ sp][1]; while(start < end){ if(start < end-1) { int checksort=0; int16_t *right = end-2; int16_t *left = start+1; int16_t *mid = start + ((end-start)>>1); if(qsort_comparison_function_int16(start, end) > 0) { if(qsort_comparison_function_int16( end, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); }else{ if(qsort_comparison_function_int16(start, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else checksort= 1; } if(qsort_comparison_function_int16(mid, end) > 0){ do{int16_t SWAP_tmp= *end; *end= *mid; *mid= SWAP_tmp;}while(0); checksort=0; } if(start == end-2) break; do{int16_t SWAP_tmp= *mid; *mid= end[-1]; end[-1]= SWAP_tmp;}while(0); while(left <= right){ while(left<=right && qsort_comparison_function_int16(left, end-1) < 0) left++; while(left<=right && qsort_comparison_function_int16(right, end-1) > 0) right--; if(left <= right){ do{int16_t SWAP_tmp= *right; *right= *left; *left= SWAP_tmp;}while(0); left++; right--; } } do{int16_t SWAP_tmp= *left; *left= end[-1]; end[-1]= SWAP_tmp;}while(0); if(checksort && (mid == left-1 || mid == left)){ mid= start; while(mid<end && qsort_comparison_function_int16(mid, mid+1) <= 0) mid++; if(mid==end) break; } if(end-left < left-start){ stack[sp ][0]= start; stack[sp++][1]= right; start = left+1; }else{ stack[sp ][0]= left+1; stack[sp++][1]= end; end = right; } }else{ if(qsort_comparison_function_int16(start, end) > 0) do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); break; } } }} while (0);
        vdk0_max = vk0[num_bands_0];

        vk0[0] = sbr->k[0];
        for (k = 1; k <= num_bands_0; k++) {
            if (vk0[k] <= 0) {
                av_log(ac->avctx, 16, "Invalid vDk0[%d]: %d\n", k, vk0[k]);
                return -1;
            }
            vk0[k] += vk0[k-1];
        }

        if (two_regions) {
            int16_t vk1[49];
            float invwarp = spectrum->bs_alter_scale ? 0.76923076923076923077f
                                                     : 1.0f;
            int num_bands_1 = lrintf(half_bands * invwarp *
                                     log2f(sbr->k[2] / (float)sbr->k[1])) * 2;

            make_bands(vk1+1, sbr->k[1], sbr->k[2], num_bands_1);

            vdk1_min = array_min_int16(vk1 + 1, num_bands_1);

            if (vdk1_min < vdk0_max) {
                int change;
                do { void *stack[64][2]; int sp= 1; stack[0][0] = vk1 + 1; stack[0][1] = (vk1 + 1)+(num_bands_1)-1; while(sp){ int16_t *start= stack[--sp][0]; int16_t *end = stack[ sp][1]; while(start < end){ if(start < end-1) { int checksort=0; int16_t *right = end-2; int16_t *left = start+1; int16_t *mid = start + ((end-start)>>1); if(qsort_comparison_function_int16(start, end) > 0) { if(qsort_comparison_function_int16( end, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); }else{ if(qsort_comparison_function_int16(start, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else checksort= 1; } if(qsort_comparison_function_int16(mid, end) > 0){ do{int16_t SWAP_tmp= *end; *end= *mid; *mid= SWAP_tmp;}while(0); checksort=0; } if(start == end-2) break; do{int16_t SWAP_tmp= *mid; *mid= end[-1]; end[-1]= SWAP_tmp;}while(0); while(left <= right){ while(left<=right && qsort_comparison_function_int16(left, end-1) < 0) left++; while(left<=right && qsort_comparison_function_int16(right, end-1) > 0) right--; if(left <= right){ do{int16_t SWAP_tmp= *right; *right= *left; *left= SWAP_tmp;}while(0); left++; right--; } } do{int16_t SWAP_tmp= *left; *left= end[-1]; end[-1]= SWAP_tmp;}while(0); if(checksort && (mid == left-1 || mid == left)){ mid= start; while(mid<end && qsort_comparison_function_int16(mid, mid+1) <= 0) mid++; if(mid==end) break; } if(end-left < left-start){ stack[sp ][0]= start; stack[sp++][1]= right; start = left+1; }else{ stack[sp ][0]= left+1; stack[sp++][1]= end; end = right; } }else{ if(qsort_comparison_function_int16(start, end) > 0) do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); break; } } }} while (0);
                change = ((vdk0_max - vk1[1]) > ((vk1[num_bands_1] - vk1[1]) >> 1) ? ((vk1[num_bands_1] - vk1[1]) >> 1) : (vdk0_max - vk1[1]));
                vk1[1] += change;
                vk1[num_bands_1] -= change;
            }

            do { void *stack[64][2]; int sp= 1; stack[0][0] = vk1 + 1; stack[0][1] = (vk1 + 1)+(num_bands_1)-1; while(sp){ int16_t *start= stack[--sp][0]; int16_t *end = stack[ sp][1]; while(start < end){ if(start < end-1) { int checksort=0; int16_t *right = end-2; int16_t *left = start+1; int16_t *mid = start + ((end-start)>>1); if(qsort_comparison_function_int16(start, end) > 0) { if(qsort_comparison_function_int16( end, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); }else{ if(qsort_comparison_function_int16(start, mid) > 0) do{int16_t SWAP_tmp= *mid; *mid= *start; *start= SWAP_tmp;}while(0); else checksort= 1; } if(qsort_comparison_function_int16(mid, end) > 0){ do{int16_t SWAP_tmp= *end; *end= *mid; *mid= SWAP_tmp;}while(0); checksort=0; } if(start == end-2) break; do{int16_t SWAP_tmp= *mid; *mid= end[-1]; end[-1]= SWAP_tmp;}while(0); while(left <= right){ while(left<=right && qsort_comparison_function_int16(left, end-1) < 0) left++; while(left<=right && qsort_comparison_function_int16(right, end-1) > 0) right--; if(left <= right){ do{int16_t SWAP_tmp= *right; *right= *left; *left= SWAP_tmp;}while(0); left++; right--; } } do{int16_t SWAP_tmp= *left; *left= end[-1]; end[-1]= SWAP_tmp;}while(0); if(checksort && (mid == left-1 || mid == left)){ mid= start; while(mid<end && qsort_comparison_function_int16(mid, mid+1) <= 0) mid++; if(mid==end) break; } if(end-left < left-start){ stack[sp ][0]= start; stack[sp++][1]= right; start = left+1; }else{ stack[sp ][0]= left+1; stack[sp++][1]= end; end = right; } }else{ if(qsort_comparison_function_int16(start, end) > 0) do{int16_t SWAP_tmp= *end; *end= *start; *start= SWAP_tmp;}while(0); break; } } }} while (0);

            vk1[0] = sbr->k[1];
            for (k = 1; k <= num_bands_1; k++) {
                if (vk1[k] <= 0) {
                    av_log(ac->avctx, 16, "Invalid vDk1[%d]: %d\n", k, vk1[k]);
                    return -1;
                }
                vk1[k] += vk1[k-1];
            }

            sbr->n_master = num_bands_0 + num_bands_1;
            if (check_n_master(ac->avctx, sbr->n_master, sbr->spectrum_params.bs_xover_band))
                return -1;
            memcpy(&sbr->f_master[0], vk0,
                   (num_bands_0 + 1) * sizeof(sbr->f_master[0]));
            memcpy(&sbr->f_master[num_bands_0 + 1], vk1 + 1,
                    num_bands_1 * sizeof(sbr->f_master[0]));

        } else {
            sbr->n_master = num_bands_0;
            if (check_n_master(ac->avctx, sbr->n_master, sbr->spectrum_params.bs_xover_band))
                return -1;
            memcpy(sbr->f_master, vk0, (num_bands_0 + 1) * sizeof(sbr->f_master[0]));
        }
    }

    return 0;
}


static int sbr_hf_calc_npatches(AACContext *ac, SpectralBandReplication *sbr)
{
    int i, k, last_k = -1, last_msb = -1, sb = 0;
    int msb = sbr->k[0];
    int usb = sbr->kx[1];
    int goal_sb = ((1000 << 11) + (sbr->sample_rate >> 1)) / sbr->sample_rate;

    sbr->num_patches = 0;

    if (goal_sb < sbr->kx[1] + sbr->m[1]) {
        for (k = 0; sbr->f_master[k] < goal_sb; k++) ;
    } else
        k = sbr->n_master;

    do {
        int odd = 0;
        if (k == last_k && msb == last_msb) {
            av_log(ac->avctx, 16, "patch construction failed\n");
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
        last_k = k;
        last_msb = msb;
        for (i = k; i == k || sb > (sbr->k[0] - 1 + msb - odd); i--) {
            sb = sbr->f_master[i];
            odd = (sb + sbr->k[0]) & 1;
        }





        if (sbr->num_patches > 5) {
            av_log(ac->avctx, 16, "Too many patches: %d\n", sbr->num_patches);
            return -1;
        }

        sbr->patch_num_subbands[sbr->num_patches] = ((sb - usb) > (0) ? (sb - usb) : (0));
        sbr->patch_start_subband[sbr->num_patches] = sbr->k[0] - odd - sbr->patch_num_subbands[sbr->num_patches];

        if (sbr->patch_num_subbands[sbr->num_patches] > 0) {
            usb = sb;
            msb = sb;
            sbr->num_patches++;
        } else
            msb = sbr->kx[1];

        if (sbr->f_master[k] - sb < 3)
            k = sbr->n_master;
    } while (sb != sbr->kx[1] + sbr->m[1]);

    if (sbr->num_patches > 1 &&
        sbr->patch_num_subbands[sbr->num_patches - 1] < 3)
        sbr->num_patches--;

    return 0;
}


static int sbr_make_f_derived(AACContext *ac, SpectralBandReplication *sbr)
{
    int k, temp;




    sbr->n[1] = sbr->n_master - sbr->spectrum_params.bs_xover_band;
    sbr->n[0] = (sbr->n[1] + 1) >> 1;

    memcpy(sbr->f_tablehigh, &sbr->f_master[sbr->spectrum_params.bs_xover_band],
           (sbr->n[1] + 1) * sizeof(sbr->f_master[0]));
    sbr->m[1] = sbr->f_tablehigh[sbr->n[1]] - sbr->f_tablehigh[0];
    sbr->kx[1] = sbr->f_tablehigh[0];


    if (sbr->kx[1] + sbr->m[1] > 64) {
        av_log(ac->avctx, 16,
               "Stop frequency border too high: %d\n", sbr->kx[1] + sbr->m[1]);
        return -1;
    }
    if (sbr->kx[1] > 32) {
        av_log(ac->avctx, 16, "Start frequency border too high: %d\n", sbr->kx[1]);
        return -1;
    }

    sbr->f_tablelow[0] = sbr->f_tablehigh[0];
    temp = sbr->n[1] & 1;
    for (k = 1; k <= sbr->n[0]; k++)
        sbr->f_tablelow[k] = sbr->f_tablehigh[2 * k - temp];
    sbr->n_q = ((1) > (lrintf(sbr->spectrum_params.bs_noise_bands * log2f(sbr->k[2] / (float)sbr->kx[1]))) ? (1) : (lrintf(sbr->spectrum_params.bs_noise_bands * log2f(sbr->k[2] / (float)sbr->kx[1]))))
                                                                     ;


    if (sbr->n_q > 5) {
        av_log(ac->avctx, 16, "Too many noise floor scale factors: %d\n", sbr->n_q);
        return -1;
    }

    sbr->f_tablenoise[0] = sbr->f_tablelow[0];
    temp = 0;
    for (k = 1; k <= sbr->n_q; k++) {
        temp += (sbr->n[0] - temp) / (sbr->n_q + 1 - k);
        sbr->f_tablenoise[k] = sbr->f_tablelow[temp];
    }

    if (sbr_hf_calc_npatches(ac, sbr) < 0)
        return -1;

    sbr_make_f_tablelim(sbr);

    sbr->data[0].f_indexnoise = 0;
    sbr->data[1].f_indexnoise = 0;

    return 0;
}

static __attribute__((always_inline)) inline void get_bits1_vector(GetBitContext *gb, uint8_t *vec,
                                              int elements)
{
    int i;
    for (i = 0; i < elements; i++) {
        vec[i] = get_bits1(gb);
    }
}


static const int8_t ceil_log2[] = {
    0, 1, 2, 2, 3, 3,
};

static int read_sbr_grid(AACContext *ac, SpectralBandReplication *sbr,
                         GetBitContext *gb, SBRData *ch_data)
{
    int i;
    int bs_pointer = 0;

    int abs_bord_trail = 16;
    int num_rel_lead, num_rel_trail;
    unsigned bs_num_env_old = ch_data->bs_num_env;
    int bs_frame_class, bs_num_env;

    ch_data->bs_freq_res[0] = ch_data->bs_freq_res[ch_data->bs_num_env];
    ch_data->bs_amp_res = sbr->bs_amp_res_header;
    ch_data->t_env_num_env_old = ch_data->t_env[bs_num_env_old];

    switch (bs_frame_class = get_bits(gb, 2)) {
    case FIXFIX:
        bs_num_env = 1 << get_bits(gb, 2);
        if (bs_num_env > 4) {
            av_log(ac->avctx, 16,
                   "Invalid bitstream, too many SBR envelopes in FIXFIX type SBR frame: %d\n",
                   bs_num_env);
            return -1;
        }
        ch_data->bs_num_env = bs_num_env;
        num_rel_lead = ch_data->bs_num_env - 1;
        if (ch_data->bs_num_env == 1)
            ch_data->bs_amp_res = 0;


        ch_data->t_env[0] = 0;
        ch_data->t_env[ch_data->bs_num_env] = abs_bord_trail;

        abs_bord_trail = (abs_bord_trail + (ch_data->bs_num_env >> 1)) /
                   ch_data->bs_num_env;
        for (i = 0; i < num_rel_lead; i++)
            ch_data->t_env[i + 1] = ch_data->t_env[i] + abs_bord_trail;

        ch_data->bs_freq_res[1] = get_bits1(gb);
        for (i = 1; i < ch_data->bs_num_env; i++)
            ch_data->bs_freq_res[i + 1] = ch_data->bs_freq_res[1];
        break;
    case FIXVAR:
        abs_bord_trail += get_bits(gb, 2);
        num_rel_trail = get_bits(gb, 2);
        ch_data->bs_num_env = num_rel_trail + 1;
        ch_data->t_env[0] = 0;
        ch_data->t_env[ch_data->bs_num_env] = abs_bord_trail;

        for (i = 0; i < num_rel_trail; i++)
            ch_data->t_env[ch_data->bs_num_env - 1 - i] =
                ch_data->t_env[ch_data->bs_num_env - i] - 2 * get_bits(gb, 2) - 2;

        bs_pointer = get_bits(gb, ceil_log2[ch_data->bs_num_env]);

        for (i = 0; i < ch_data->bs_num_env; i++)
            ch_data->bs_freq_res[ch_data->bs_num_env - i] = get_bits1(gb);
        break;
    case VARFIX:
        ch_data->t_env[0] = get_bits(gb, 2);
        num_rel_lead = get_bits(gb, 2);
        ch_data->bs_num_env = num_rel_lead + 1;
        ch_data->t_env[ch_data->bs_num_env] = abs_bord_trail;

        for (i = 0; i < num_rel_lead; i++)
            ch_data->t_env[i + 1] = ch_data->t_env[i] + 2 * get_bits(gb, 2) + 2;

        bs_pointer = get_bits(gb, ceil_log2[ch_data->bs_num_env]);

        get_bits1_vector(gb, ch_data->bs_freq_res + 1, ch_data->bs_num_env);
        break;
    case VARVAR:
        ch_data->t_env[0] = get_bits(gb, 2);
        abs_bord_trail += get_bits(gb, 2);
        num_rel_lead = get_bits(gb, 2);
        num_rel_trail = get_bits(gb, 2);
        bs_num_env = num_rel_lead + num_rel_trail + 1;

        if (bs_num_env > 5) {
            av_log(ac->avctx, 16,
                   "Invalid bitstream, too many SBR envelopes in VARVAR type SBR frame: %d\n",
                   bs_num_env);
            return -1;
        }
        ch_data->bs_num_env = bs_num_env;

        ch_data->t_env[ch_data->bs_num_env] = abs_bord_trail;

        for (i = 0; i < num_rel_lead; i++)
            ch_data->t_env[i + 1] = ch_data->t_env[i] + 2 * get_bits(gb, 2) + 2;
        for (i = 0; i < num_rel_trail; i++)
            ch_data->t_env[ch_data->bs_num_env - 1 - i] =
                ch_data->t_env[ch_data->bs_num_env - i] - 2 * get_bits(gb, 2) - 2;

        bs_pointer = get_bits(gb, ceil_log2[ch_data->bs_num_env]);

        get_bits1_vector(gb, ch_data->bs_freq_res + 1, ch_data->bs_num_env);
        break;
    }
    ch_data->bs_frame_class = bs_frame_class;

    do { if (!(bs_pointer >= 0)) { av_log(
   ((void *)0)
   , 0, "Assertion %s failed at %s:%d\n", "bs_pointer >= 0", "libavcodec/aacsbr_template.c", 727); abort(); } } while (0);
    if (bs_pointer > ch_data->bs_num_env + 1) {
        av_log(ac->avctx, 16,
               "Invalid bitstream, bs_pointer points to a middle noise border outside the time borders table: %d\n",
               bs_pointer);
        return -1;
    }

    for (i = 1; i <= ch_data->bs_num_env; i++) {
        if (ch_data->t_env[i-1] >= ch_data->t_env[i]) {
            av_log(ac->avctx, 16, "Not strictly monotone time borders\n");
            return -1;
        }
    }

    ch_data->bs_num_noise = (ch_data->bs_num_env > 1) + 1;

    ch_data->t_q[0] = ch_data->t_env[0];
    ch_data->t_q[ch_data->bs_num_noise] = ch_data->t_env[ch_data->bs_num_env];
    if (ch_data->bs_num_noise > 1) {
        int idx;
        if (ch_data->bs_frame_class == FIXFIX) {
            idx = ch_data->bs_num_env >> 1;
        } else if (ch_data->bs_frame_class & 1) {
            idx = ch_data->bs_num_env - ((bs_pointer - 1) > (1) ? (bs_pointer - 1) : (1));
        } else {
            if (!bs_pointer)
                idx = 1;
            else if (bs_pointer == 1)
                idx = ch_data->bs_num_env - 1;
            else
                idx = bs_pointer - 1;
        }
        ch_data->t_q[1] = ch_data->t_env[idx];
    }

    ch_data->e_a[0] = -(ch_data->e_a[1] != bs_num_env_old);
    ch_data->e_a[1] = -1;
    if ((ch_data->bs_frame_class & 1) && bs_pointer) {
        ch_data->e_a[1] = ch_data->bs_num_env + 1 - bs_pointer;
    } else if ((ch_data->bs_frame_class == 2) && (bs_pointer > 1))
        ch_data->e_a[1] = bs_pointer - 1;

    return 0;
}

static void copy_sbr_grid(SBRData *dst, const SBRData *src) {

    dst->bs_freq_res[0] = dst->bs_freq_res[dst->bs_num_env];
    dst->t_env_num_env_old = dst->t_env[dst->bs_num_env];
    dst->e_a[0] = -(dst->e_a[1] != dst->bs_num_env);


    memcpy(dst->bs_freq_res+1, src->bs_freq_res+1, sizeof(dst->bs_freq_res)-sizeof(*dst->bs_freq_res));
    memcpy(dst->t_env, src->t_env, sizeof(dst->t_env));
    memcpy(dst->t_q, src->t_q, sizeof(dst->t_q));
    dst->bs_num_env = src->bs_num_env;
    dst->bs_amp_res = src->bs_amp_res;
    dst->bs_num_noise = src->bs_num_noise;
    dst->bs_frame_class = src->bs_frame_class;
    dst->e_a[1] = src->e_a[1];
}


static void read_sbr_dtdf(SpectralBandReplication *sbr, GetBitContext *gb,
                          SBRData *ch_data)
{
    get_bits1_vector(gb, ch_data->bs_df_env, ch_data->bs_num_env);
    get_bits1_vector(gb, ch_data->bs_df_noise, ch_data->bs_num_noise);
}


static void read_sbr_invf(SpectralBandReplication *sbr, GetBitContext *gb,
                          SBRData *ch_data)
{
    int i;

    memcpy(ch_data->bs_invf_mode[1], ch_data->bs_invf_mode[0], 5 * sizeof(uint8_t));
    for (i = 0; i < sbr->n_q; i++)
        ch_data->bs_invf_mode[0][i] = get_bits(gb, 2);
}

static int read_sbr_envelope(AACContext *ac, SpectralBandReplication *sbr, GetBitContext *gb,
                              SBRData *ch_data, int ch)
{
    int bits;
    int i, j, k;
    int16_t (*t_huff)[2], (*f_huff)[2];
    int t_lav, f_lav;
    const int delta = (ch == 1 && sbr->bs_coupling == 1) + 1;
    const int odd = sbr->n[1] & 1;

    if (sbr->bs_coupling && ch) {
        if (ch_data->bs_amp_res) {
            bits = 5;
            t_huff = vlc_sbr[T_HUFFMAN_ENV_BAL_3_0DB].table;
            t_lav = vlc_sbr_lav[T_HUFFMAN_ENV_BAL_3_0DB];
            f_huff = vlc_sbr[F_HUFFMAN_ENV_BAL_3_0DB].table;
            f_lav = vlc_sbr_lav[F_HUFFMAN_ENV_BAL_3_0DB];
        } else {
            bits = 6;
            t_huff = vlc_sbr[T_HUFFMAN_ENV_BAL_1_5DB].table;
            t_lav = vlc_sbr_lav[T_HUFFMAN_ENV_BAL_1_5DB];
            f_huff = vlc_sbr[F_HUFFMAN_ENV_BAL_1_5DB].table;
            f_lav = vlc_sbr_lav[F_HUFFMAN_ENV_BAL_1_5DB];
        }
    } else {
        if (ch_data->bs_amp_res) {
            bits = 6;
            t_huff = vlc_sbr[T_HUFFMAN_ENV_3_0DB].table;
            t_lav = vlc_sbr_lav[T_HUFFMAN_ENV_3_0DB];
            f_huff = vlc_sbr[F_HUFFMAN_ENV_3_0DB].table;
            f_lav = vlc_sbr_lav[F_HUFFMAN_ENV_3_0DB];
        } else {
            bits = 7;
            t_huff = vlc_sbr[T_HUFFMAN_ENV_1_5DB].table;
            t_lav = vlc_sbr_lav[T_HUFFMAN_ENV_1_5DB];
            f_huff = vlc_sbr[F_HUFFMAN_ENV_1_5DB].table;
            f_lav = vlc_sbr_lav[F_HUFFMAN_ENV_1_5DB];
        }
    }

    for (i = 0; i < ch_data->bs_num_env; i++) {
        if (ch_data->bs_df_env[i]) {

            if (ch_data->bs_freq_res[i + 1] == ch_data->bs_freq_res[i]) {
                for (j = 0; j < sbr->n[ch_data->bs_freq_res[i + 1]]; j++) {
                    ch_data->env_facs_q[i + 1][j] = ch_data->env_facs_q[i][j] + delta * (get_vlc2(gb, t_huff, 9, 3) - t_lav);
                    if (ch_data->env_facs_q[i + 1][j] > 127U) {
                        av_log(ac->avctx, 16, "env_facs_q %d is invalid\n", ch_data->env_facs_q[i + 1][j]);
                        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                    }
                }
            } else if (ch_data->bs_freq_res[i + 1]) {
                for (j = 0; j < sbr->n[ch_data->bs_freq_res[i + 1]]; j++) {
                    k = (j + odd) >> 1;
                    ch_data->env_facs_q[i + 1][j] = ch_data->env_facs_q[i][k] + delta * (get_vlc2(gb, t_huff, 9, 3) - t_lav);
                    if (ch_data->env_facs_q[i + 1][j] > 127U) {
                        av_log(ac->avctx, 16, "env_facs_q %d is invalid\n", ch_data->env_facs_q[i + 1][j]);
                        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                    }
                }
            } else {
                for (j = 0; j < sbr->n[ch_data->bs_freq_res[i + 1]]; j++) {
                    k = j ? 2*j - odd : 0;
                    ch_data->env_facs_q[i + 1][j] = ch_data->env_facs_q[i][k] + delta * (get_vlc2(gb, t_huff, 9, 3) - t_lav);
                    if (ch_data->env_facs_q[i + 1][j] > 127U) {
                        av_log(ac->avctx, 16, "env_facs_q %d is invalid\n", ch_data->env_facs_q[i + 1][j]);
                        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                    }
                }
            }
        } else {
            ch_data->env_facs_q[i + 1][0] = delta * get_bits(gb, bits);
            for (j = 1; j < sbr->n[ch_data->bs_freq_res[i + 1]]; j++) {
                ch_data->env_facs_q[i + 1][j] = ch_data->env_facs_q[i + 1][j - 1] + delta * (get_vlc2(gb, f_huff, 9, 3) - f_lav);
                if (ch_data->env_facs_q[i + 1][j] > 127U) {
                    av_log(ac->avctx, 16, "env_facs_q %d is invalid\n", ch_data->env_facs_q[i + 1][j]);
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
            }
        }
    }


    memcpy(ch_data->env_facs_q[0], ch_data->env_facs_q[ch_data->bs_num_env],
           sizeof(ch_data->env_facs_q[0]));

    return 0;
}

static int read_sbr_noise(AACContext *ac, SpectralBandReplication *sbr, GetBitContext *gb,
                           SBRData *ch_data, int ch)
{
    int i, j;
    int16_t (*t_huff)[2], (*f_huff)[2];
    int t_lav, f_lav;
    int delta = (ch == 1 && sbr->bs_coupling == 1) + 1;

    if (sbr->bs_coupling && ch) {
        t_huff = vlc_sbr[T_HUFFMAN_NOISE_BAL_3_0DB].table;
        t_lav = vlc_sbr_lav[T_HUFFMAN_NOISE_BAL_3_0DB];
        f_huff = vlc_sbr[F_HUFFMAN_ENV_BAL_3_0DB].table;
        f_lav = vlc_sbr_lav[F_HUFFMAN_ENV_BAL_3_0DB];
    } else {
        t_huff = vlc_sbr[T_HUFFMAN_NOISE_3_0DB].table;
        t_lav = vlc_sbr_lav[T_HUFFMAN_NOISE_3_0DB];
        f_huff = vlc_sbr[F_HUFFMAN_ENV_3_0DB].table;
        f_lav = vlc_sbr_lav[F_HUFFMAN_ENV_3_0DB];
    }

    for (i = 0; i < ch_data->bs_num_noise; i++) {
        if (ch_data->bs_df_noise[i]) {
            for (j = 0; j < sbr->n_q; j++) {
                ch_data->noise_facs_q[i + 1][j] = ch_data->noise_facs_q[i][j] + delta * (get_vlc2(gb, t_huff, 9, 2) - t_lav);
                if (ch_data->noise_facs_q[i + 1][j] > 30U) {
                    av_log(ac->avctx, 16, "noise_facs_q %d is invalid\n", ch_data->noise_facs_q[i + 1][j]);
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
            }
        } else {
            ch_data->noise_facs_q[i + 1][0] = delta * get_bits(gb, 5);
            for (j = 1; j < sbr->n_q; j++) {
                ch_data->noise_facs_q[i + 1][j] = ch_data->noise_facs_q[i + 1][j - 1] + delta * (get_vlc2(gb, f_huff, 9, 3) - f_lav);
                if (ch_data->noise_facs_q[i + 1][j] > 30U) {
                    av_log(ac->avctx, 16, "noise_facs_q %d is invalid\n", ch_data->noise_facs_q[i + 1][j]);
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
            }
        }
    }


    memcpy(ch_data->noise_facs_q[0], ch_data->noise_facs_q[ch_data->bs_num_noise],
           sizeof(ch_data->noise_facs_q[0]));
    return 0;
}

static void read_sbr_extension(AACContext *ac, SpectralBandReplication *sbr,
                               GetBitContext *gb,
                               int bs_extension_id, int *num_bits_left)
{
    switch (bs_extension_id) {
    case EXTENSION_ID_PS:
        if (!ac->oc[1].m4ac.ps) {
            av_log(ac->avctx, 16, "Parametric Stereo signaled to be not-present but was found in the bitstream.\n");
            skip_bits_long(gb, *num_bits_left);
            *num_bits_left = 0;
        } else {
            *num_bits_left -= ff_ps_read_data(ac->avctx, gb, &sbr->ps.common, *num_bits_left);
            ac->avctx->profile = 28;
        }
        break;
    default:

        if (bs_extension_id || *num_bits_left > 16 || show_bits(gb, *num_bits_left))
            avpriv_request_sample(ac->avctx, "Reserved SBR extensions");
        skip_bits_long(gb, *num_bits_left);
        *num_bits_left = 0;
        break;
    }
}

static int read_sbr_single_channel_element(AACContext *ac,
                                            SpectralBandReplication *sbr,
                                            GetBitContext *gb)
{
    int ret;

    if (get_bits1(gb))
        skip_bits(gb, 4);

    if (read_sbr_grid(ac, sbr, gb, &sbr->data[0]))
        return -1;
    read_sbr_dtdf(sbr, gb, &sbr->data[0]);
    read_sbr_invf(sbr, gb, &sbr->data[0]);
    if((ret = read_sbr_envelope(ac, sbr, gb, &sbr->data[0], 0)) < 0)
        return ret;
    if((ret = read_sbr_noise(ac, sbr, gb, &sbr->data[0], 0)) < 0)
        return ret;

    if ((sbr->data[0].bs_add_harmonic_flag = get_bits1(gb)))
        get_bits1_vector(gb, sbr->data[0].bs_add_harmonic, sbr->n[1]);

    return 0;
}

static int read_sbr_channel_pair_element(AACContext *ac,
                                          SpectralBandReplication *sbr,
                                          GetBitContext *gb)
{
    int ret;

    if (get_bits1(gb))
        skip_bits(gb, 8);

    if ((sbr->bs_coupling = get_bits1(gb))) {
        if (read_sbr_grid(ac, sbr, gb, &sbr->data[0]))
            return -1;
        copy_sbr_grid(&sbr->data[1], &sbr->data[0]);
        read_sbr_dtdf(sbr, gb, &sbr->data[0]);
        read_sbr_dtdf(sbr, gb, &sbr->data[1]);
        read_sbr_invf(sbr, gb, &sbr->data[0]);
        memcpy(sbr->data[1].bs_invf_mode[1], sbr->data[1].bs_invf_mode[0], sizeof(sbr->data[1].bs_invf_mode[0]));
        memcpy(sbr->data[1].bs_invf_mode[0], sbr->data[0].bs_invf_mode[0], sizeof(sbr->data[1].bs_invf_mode[0]));
        if((ret = read_sbr_envelope(ac, sbr, gb, &sbr->data[0], 0)) < 0)
            return ret;
        if((ret = read_sbr_noise(ac, sbr, gb, &sbr->data[0], 0)) < 0)
            return ret;
        if((ret = read_sbr_envelope(ac, sbr, gb, &sbr->data[1], 1)) < 0)
            return ret;
        if((ret = read_sbr_noise(ac, sbr, gb, &sbr->data[1], 1)) < 0)
            return ret;
    } else {
        if (read_sbr_grid(ac, sbr, gb, &sbr->data[0]) ||
            read_sbr_grid(ac, sbr, gb, &sbr->data[1]))
            return -1;
        read_sbr_dtdf(sbr, gb, &sbr->data[0]);
        read_sbr_dtdf(sbr, gb, &sbr->data[1]);
        read_sbr_invf(sbr, gb, &sbr->data[0]);
        read_sbr_invf(sbr, gb, &sbr->data[1]);
        if((ret = read_sbr_envelope(ac, sbr, gb, &sbr->data[0], 0)) < 0)
            return ret;
        if((ret = read_sbr_envelope(ac, sbr, gb, &sbr->data[1], 1)) < 0)
            return ret;
        if((ret = read_sbr_noise(ac, sbr, gb, &sbr->data[0], 0)) < 0)
            return ret;
        if((ret = read_sbr_noise(ac, sbr, gb, &sbr->data[1], 1)) < 0)
            return ret;
    }

    if ((sbr->data[0].bs_add_harmonic_flag = get_bits1(gb)))
        get_bits1_vector(gb, sbr->data[0].bs_add_harmonic, sbr->n[1]);
    if ((sbr->data[1].bs_add_harmonic_flag = get_bits1(gb)))
        get_bits1_vector(gb, sbr->data[1].bs_add_harmonic, sbr->n[1]);

    return 0;
}

static unsigned int read_sbr_data(AACContext *ac, SpectralBandReplication *sbr,
                                  GetBitContext *gb, int id_aac)
{
    unsigned int cnt = get_bits_count(gb);

    sbr->id_aac = id_aac;
    sbr->ready_for_dequant = 1;

    if (id_aac == TYPE_SCE || id_aac == TYPE_CCE) {
        if (read_sbr_single_channel_element(ac, sbr, gb)) {
            sbr_turnoff(sbr);
            return get_bits_count(gb) - cnt;
        }
    } else if (id_aac == TYPE_CPE) {
        if (read_sbr_channel_pair_element(ac, sbr, gb)) {
            sbr_turnoff(sbr);
            return get_bits_count(gb) - cnt;
        }
    } else {
        av_log(ac->avctx, 16,
            "Invalid bitstream - cannot apply SBR to element type %d\n", id_aac);
        sbr_turnoff(sbr);
        return get_bits_count(gb) - cnt;
    }
    if (get_bits1(gb)) {
        int num_bits_left = get_bits(gb, 4);
        if (num_bits_left == 15)
            num_bits_left += get_bits(gb, 8);

        num_bits_left <<= 3;
        while (num_bits_left > 7) {
            num_bits_left -= 2;
            read_sbr_extension(ac, sbr, gb, get_bits(gb, 2), &num_bits_left);
        }
        if (num_bits_left < 0) {
            av_log(ac->avctx, 16, "SBR Extension over read.\n");
        }
        if (num_bits_left > 0)
            skip_bits(gb, num_bits_left);
    }

    return get_bits_count(gb) - cnt;
}

static void sbr_reset(AACContext *ac, SpectralBandReplication *sbr)
{
    int err;
    err = sbr_make_f_master(ac, sbr, &sbr->spectrum_params);
    if (err >= 0)
        err = sbr_make_f_derived(ac, sbr);
    if (err < 0) {
        av_log(ac->avctx, 16,
               "SBR reset failed. Switching SBR to pure upsampling mode.\n");
        sbr_turnoff(sbr);
    }
}
int ff_decode_sbr_extension(AACContext *ac, SpectralBandReplication *sbr,
                            GetBitContext *gb_host, int crc, int cnt, int id_aac)
{
    unsigned int num_sbr_bits = 0, num_align_bits;
    unsigned bytes_read;
    GetBitContext gbc = *gb_host, *gb = &gbc;
    skip_bits_long(gb_host, cnt*8 - 4);

    sbr->reset = 0;

    if (!sbr->sample_rate)
        sbr->sample_rate = 2 * ac->oc[1].m4ac.sample_rate;
    if (!ac->oc[1].m4ac.ext_sample_rate)
        ac->oc[1].m4ac.ext_sample_rate = 2 * ac->oc[1].m4ac.sample_rate;

    if (crc) {
        skip_bits(gb, 10);
        num_sbr_bits += 10;
    }


    sbr->kx[0] = sbr->kx[1];
    sbr->m[0] = sbr->m[1];
    sbr->kx_and_m_pushed = 1;

    num_sbr_bits++;
    if (get_bits1(gb))
        num_sbr_bits += read_sbr_header(sbr, gb);

    if (sbr->reset)
        sbr_reset(ac, sbr);

    if (sbr->start)
        num_sbr_bits += read_sbr_data(ac, sbr, gb, id_aac);

    num_align_bits = ((cnt << 3) - 4 - num_sbr_bits) & 7;
    bytes_read = ((num_sbr_bits + num_align_bits + 4) >> 3);

    if (bytes_read > cnt) {
        av_log(ac->avctx, 16,
               "Expected to read %d SBR bytes actually read %d.\n", cnt, bytes_read);
        sbr_turnoff(sbr);
    }
    return cnt;
}
static void sbr_qmf_analysis(AVFloatDSPContext *dsp, FFTContext *mdct,

                             SBRDSPContext *sbrdsp, const INTFLOAT *in, INTFLOAT *x,
                             INTFLOAT z[320], INTFLOAT W[2][32][32][2], int buf_idx)
{
    int i;



    memcpy(x , x+1024, (320-32)*sizeof(x[0]));
    memcpy(x+288, in, 1024*sizeof(x[0]));
    for (i = 0; i < 32; i++) {

        dsp->vector_fmul_reverse(z, sbr_qmf_window_ds, x, 320);
        sbrdsp->sum64x5(z);
        sbrdsp->qmf_pre_shuffle(z);
        mdct->imdct_half(mdct, z, z+64);
        sbrdsp->qmf_post_shuffle(W[buf_idx][i], z);
        x += 32;
    }
}







static void sbr_qmf_synthesis(FFTContext *mdct,



                              SBRDSPContext *sbrdsp, AVFloatDSPContext *dsp,

                              INTFLOAT *out, INTFLOAT X[2][38][64],
                              INTFLOAT mdct_buf[2][64],
                              INTFLOAT *v0, int *v_off, const unsigned int div)
{
    int i, n;
    const INTFLOAT *sbr_qmf_window = div ? sbr_qmf_window_ds : sbr_qmf_window_us;
    const int step = 128 >> div;
    INTFLOAT *v;
    for (i = 0; i < 32; i++) {
        if (*v_off < step) {
            int saved_samples = (1280 - 128) >> div;
            memcpy(&v0[((1280-128)*2) - saved_samples], v0, saved_samples * sizeof(INTFLOAT));
            *v_off = ((1280-128)*2) - saved_samples - step;
        } else {
            *v_off -= step;
        }
        v = v0 + *v_off;
        if (div) {
            for (n = 0; n < 32; n++) {
                X[0][i][ n] = -X[0][i][n];
                X[0][i][32+n] = X[1][i][31-n];
            }
            mdct->imdct_half(mdct, mdct_buf[0], X[0][i]);
            sbrdsp->qmf_deint_neg(v, mdct_buf[0]);
        } else {
            sbrdsp->neg_odd_64(X[1][i]);
            mdct->imdct_half(mdct, mdct_buf[0], X[0][i]);
            mdct->imdct_half(mdct, mdct_buf[1], X[1][i]);
            sbrdsp->qmf_deint_bfly(v, mdct_buf[1], mdct_buf[0]);
        }
        dsp->vector_fmul (out, v , sbr_qmf_window , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 192 >> div), sbr_qmf_window + ( 64 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 256 >> div), sbr_qmf_window + (128 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 448 >> div), sbr_qmf_window + (192 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 512 >> div), sbr_qmf_window + (256 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 704 >> div), sbr_qmf_window + (320 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 768 >> div), sbr_qmf_window + (384 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + ( 960 >> div), sbr_qmf_window + (448 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + (1024 >> div), sbr_qmf_window + (512 >> div), out , 64 >> div);
        dsp->vector_fmul_add(out, v + (1216 >> div), sbr_qmf_window + (576 >> div), out , 64 >> div);
        out += 64 >> div;
    }
}



static int sbr_lf_gen(AACContext *ac, SpectralBandReplication *sbr,
                      INTFLOAT X_low[32][40][2], const INTFLOAT W[2][32][32][2],
                      int buf_idx)
{
    int i, k;
    const int t_HFGen = 8;
    const int i_f = 32;
    memset(X_low, 0, 32*sizeof(*X_low));
    for (k = 0; k < sbr->kx[1]; k++) {
        for (i = t_HFGen; i < i_f + t_HFGen; i++) {
            X_low[k][i][0] = W[buf_idx][i - t_HFGen][k][0];
            X_low[k][i][1] = W[buf_idx][i - t_HFGen][k][1];
        }
    }
    buf_idx = 1-buf_idx;
    for (k = 0; k < sbr->kx[0]; k++) {
        for (i = 0; i < t_HFGen; i++) {
            X_low[k][i][0] = W[buf_idx][i + i_f - t_HFGen][k][0];
            X_low[k][i][1] = W[buf_idx][i + i_f - t_HFGen][k][1];
        }
    }
    return 0;
}


static int sbr_hf_gen(AACContext *ac, SpectralBandReplication *sbr,
                      INTFLOAT X_high[64][40][2], const INTFLOAT X_low[32][40][2],
                      const INTFLOAT (*alpha0)[2], const INTFLOAT (*alpha1)[2],
                      const INTFLOAT bw_array[5], const uint8_t *t_env,
                      int bs_num_env)
{
    int j, x;
    int g = 0;
    int k = sbr->kx[1];
    for (j = 0; j < sbr->num_patches; j++) {
        for (x = 0; x < sbr->patch_num_subbands[j]; x++, k++) {
            const int p = sbr->patch_start_subband[j] + x;
            while (g <= sbr->n_q && k >= sbr->f_tablenoise[g])
                g++;
            g--;

            if (g < 0) {
                av_log(ac->avctx, 16,
                       "ERROR : no subband found for frequency %d\n", k);
                return -1;
            }

            sbr->dsp.hf_gen(X_high[k] + 2,
                            X_low[p] + 2,
                            alpha0[p], alpha1[p], bw_array[g],
                            2 * t_env[0], 2 * t_env[bs_num_env]);
        }
    }
    if (k < sbr->m[1] + sbr->kx[1])
        memset(X_high + k, 0, (sbr->m[1] + sbr->kx[1] - k) * sizeof(*X_high));

    return 0;
}


static int sbr_x_gen(SpectralBandReplication *sbr, INTFLOAT X[2][38][64],
                     const INTFLOAT Y0[38][64][2], const INTFLOAT Y1[38][64][2],
                     const INTFLOAT X_low[32][40][2], int ch)
{
    int k, i;
    const int i_f = 32;
    const int i_Temp = ((2*sbr->data[ch].t_env_num_env_old - i_f) > (0) ? (2*sbr->data[ch].t_env_num_env_old - i_f) : (0));
    memset(X, 0, 2*sizeof(*X));
    for (k = 0; k < sbr->kx[0]; k++) {
        for (i = 0; i < i_Temp; i++) {
            X[0][i][k] = X_low[k][i + 2][0];
            X[1][i][k] = X_low[k][i + 2][1];
        }
    }
    for (; k < sbr->kx[0] + sbr->m[0]; k++) {
        for (i = 0; i < i_Temp; i++) {
            X[0][i][k] = Y0[i + i_f][k][0];
            X[1][i][k] = Y0[i + i_f][k][1];
        }
    }

    for (k = 0; k < sbr->kx[1]; k++) {
        for (i = i_Temp; i < 38; i++) {
            X[0][i][k] = X_low[k][i + 2][0];
            X[1][i][k] = X_low[k][i + 2][1];
        }
    }
    for (; k < sbr->kx[1] + sbr->m[1]; k++) {
        for (i = i_Temp; i < i_f; i++) {
            X[0][i][k] = Y1[i][k][0];
            X[1][i][k] = Y1[i][k][1];
        }
    }
    return 0;
}




static int sbr_mapping(AACContext *ac, SpectralBandReplication *sbr,
                        SBRData *ch_data, int e_a[2])
{
    int e, i, m;

    memset(ch_data->s_indexmapped[1], 0, 7*sizeof(ch_data->s_indexmapped[1]));
    for (e = 0; e < ch_data->bs_num_env; e++) {
        const unsigned int ilim = sbr->n[ch_data->bs_freq_res[e + 1]];
        uint16_t *table = ch_data->bs_freq_res[e + 1] ? sbr->f_tablehigh : sbr->f_tablelow;
        int k;

        if (sbr->kx[1] != table[0]) {
            av_log(ac->avctx, 16, "kx != f_table{high,low}[0]. "
                   "Derived frequency tables were not regenerated.\n");
            sbr_turnoff(sbr);
            return (-(int)(('B') | (('U') << 8) | (('G') << 16) | ((unsigned)('!') << 24)));
        }
        for (i = 0; i < ilim; i++)
            for (m = table[i]; m < table[i + 1]; m++)
                sbr->e_origmapped[e][m - sbr->kx[1]] = ch_data->env_facs[e+1][i];


        k = (ch_data->bs_num_noise > 1) && (ch_data->t_env[e] >= ch_data->t_q[1]);
        for (i = 0; i < sbr->n_q; i++)
            for (m = sbr->f_tablenoise[i]; m < sbr->f_tablenoise[i + 1]; m++)
                sbr->q_mapped[e][m - sbr->kx[1]] = ch_data->noise_facs[k+1][i];

        for (i = 0; i < sbr->n[1]; i++) {
            if (ch_data->bs_add_harmonic_flag) {
                const unsigned int m_midpoint =
                    (sbr->f_tablehigh[i] + sbr->f_tablehigh[i + 1]) >> 1;

                ch_data->s_indexmapped[e + 1][m_midpoint - sbr->kx[1]] = ch_data->bs_add_harmonic[i] *
                    (e >= e_a[1] || (ch_data->s_indexmapped[0][m_midpoint - sbr->kx[1]] == 1));
            }
        }

        for (i = 0; i < ilim; i++) {
            int additional_sinusoid_present = 0;
            for (m = table[i]; m < table[i + 1]; m++) {
                if (ch_data->s_indexmapped[e + 1][m - sbr->kx[1]]) {
                    additional_sinusoid_present = 1;
                    break;
                }
            }
            memset(&sbr->s_mapped[e][table[i] - sbr->kx[1]], additional_sinusoid_present,
                   (table[i + 1] - table[i]) * sizeof(sbr->s_mapped[e][0]));
        }
    }

    memcpy(ch_data->s_indexmapped[0], ch_data->s_indexmapped[ch_data->bs_num_env], sizeof(ch_data->s_indexmapped[0]));
    return 0;
}


static void sbr_env_estimate(AAC_FLOAT (*e_curr)[48], INTFLOAT X_high[64][40][2],
                             SpectralBandReplication *sbr, SBRData *ch_data)
{
    int e, m;
    int kx1 = sbr->kx[1];

    if (sbr->bs_interpol_freq) {
        for (e = 0; e < ch_data->bs_num_env; e++) {



            const float recip_env_size = 0.5f / (ch_data->t_env[e + 1] - ch_data->t_env[e]);

            int ilb = ch_data->t_env[e] * 2 + 2;
            int iub = ch_data->t_env[e + 1] * 2 + 2;

            for (m = 0; m < sbr->m[1]; m++) {
                AAC_FLOAT sum = sbr->dsp.sum_square(X_high[m+kx1] + ilb, iub - ilb);



                e_curr[e][m] = sum * recip_env_size;

            }
        }
    } else {
        int k, p;

        for (e = 0; e < ch_data->bs_num_env; e++) {
            const int env_size = 2 * (ch_data->t_env[e + 1] - ch_data->t_env[e]);
            int ilb = ch_data->t_env[e] * 2 + 2;
            int iub = ch_data->t_env[e + 1] * 2 + 2;
            const uint16_t *table = ch_data->bs_freq_res[e + 1] ? sbr->f_tablehigh : sbr->f_tablelow;

            for (p = 0; p < sbr->n[ch_data->bs_freq_res[e + 1]]; p++) {
                float sum = 0.0f;
                const int den = env_size * (table[p + 1] - table[p]);

                for (k = table[p]; k < table[p + 1]; k++) {
                    sum += sbr->dsp.sum_square(X_high[k] + ilb, iub - ilb);
                }
                sum /= den;

                for (k = table[p]; k < table[p + 1]; k++) {
                    e_curr[e][k - kx1] = sum;
                }
            }
        }
    }
}

void ff_sbr_apply(AACContext *ac, SpectralBandReplication *sbr, int id_aac,
                  INTFLOAT* L, INTFLOAT* R)
{
    int downsampled = ac->oc[1].m4ac.ext_sample_rate < sbr->sample_rate;
    int ch;
    int nch = (id_aac == TYPE_CPE) ? 2 : 1;
    int err;

    if (id_aac != sbr->id_aac) {
        av_log(ac->avctx, id_aac == TYPE_LFE ? 40 : 24,
            "element type mismatch %d != %d\n", id_aac, sbr->id_aac);
        sbr_turnoff(sbr);
    }

    if (sbr->start && !sbr->ready_for_dequant) {
        av_log(ac->avctx, 16,
               "No quantized data read for sbr_dequant.\n");
        sbr_turnoff(sbr);
    }

    if (!sbr->kx_and_m_pushed) {
        sbr->kx[0] = sbr->kx[1];
        sbr->m[0] = sbr->m[1];
    } else {
        sbr->kx_and_m_pushed = 0;
    }

    if (sbr->start) {
        sbr_dequant(sbr, id_aac);
        sbr->ready_for_dequant = 0;
    }
    for (ch = 0; ch < nch; ch++) {

        sbr_qmf_analysis(ac->fdsp, &sbr->mdct_ana, &sbr->dsp, ch ? R : L, sbr->data[ch].analysis_filterbank_samples,
                         (INTFLOAT*)sbr->qmf_filter_scratch,
                         sbr->data[ch].W, sbr->data[ch].Ypos);
        sbr->c.sbr_lf_gen(ac, sbr, sbr->X_low,
                          (const INTFLOAT (*)[32][32][2]) sbr->data[ch].W,
                          sbr->data[ch].Ypos);
        sbr->data[ch].Ypos ^= 1;
        if (sbr->start) {
            sbr->c.sbr_hf_inverse_filter(&sbr->dsp, sbr->alpha0, sbr->alpha1,
                                         (const INTFLOAT (*)[40][2]) sbr->X_low, sbr->k[0]);
            sbr_chirp(sbr, &sbr->data[ch]);
            do { if (!(sbr->data[ch].bs_num_env > 0)) { av_log(
           ((void *)0)
           , 0, "Assertion %s failed at %s:%d\n", "sbr->data[ch].bs_num_env > 0", "libavcodec/aacsbr_template.c", 1519); abort(); } } while (0);
            sbr_hf_gen(ac, sbr, sbr->X_high,
                       (const INTFLOAT (*)[40][2]) sbr->X_low,
                       (const INTFLOAT (*)[2]) sbr->alpha0,
                       (const INTFLOAT (*)[2]) sbr->alpha1,
                       sbr->data[ch].bw_array, sbr->data[ch].t_env,
                       sbr->data[ch].bs_num_env);


            err = sbr_mapping(ac, sbr, &sbr->data[ch], sbr->data[ch].e_a);
            if (!err) {
                sbr_env_estimate(sbr->e_curr, sbr->X_high, sbr, &sbr->data[ch]);
                sbr_gain_calc(ac, sbr, &sbr->data[ch], sbr->data[ch].e_a);
                sbr->c.sbr_hf_assemble(sbr->data[ch].Y[sbr->data[ch].Ypos],
                                (const INTFLOAT (*)[40][2]) sbr->X_high,
                                sbr, &sbr->data[ch],
                                sbr->data[ch].e_a);
            }
        }


        sbr->c.sbr_x_gen(sbr, sbr->X[ch],
                  (const INTFLOAT (*)[64][2]) sbr->data[ch].Y[1-sbr->data[ch].Ypos],
                  (const INTFLOAT (*)[64][2]) sbr->data[ch].Y[ sbr->data[ch].Ypos],
                  (const INTFLOAT (*)[40][2]) sbr->X_low, ch);
    }

    if (ac->oc[1].m4ac.ps == 1) {
        if (sbr->ps.common.start) {
            ff_ps_apply(ac->avctx, &sbr->ps, sbr->X[0], sbr->X[1], sbr->kx[1] + sbr->m[1]);
        } else {
            memcpy(sbr->X[1], sbr->X[0], sizeof(sbr->X[0]));
        }
        nch = 2;
    }

    sbr_qmf_synthesis(&sbr->mdct, &sbr->dsp, ac->fdsp,
                      L, sbr->X[0], sbr->qmf_filter_scratch,
                      sbr->data[0].synthesis_filterbank_samples,
                      &sbr->data[0].synthesis_filterbank_samples_offset,
                      downsampled);
    if (nch == 2)
        sbr_qmf_synthesis(&sbr->mdct, &sbr->dsp, ac->fdsp,
                          R, sbr->X[1], sbr->qmf_filter_scratch,
                          sbr->data[1].synthesis_filterbank_samples,
                          &sbr->data[1].synthesis_filterbank_samples_offset,
                          downsampled);
}

static void aacsbr_func_ptr_init(AACSBRContext *c)
{
    c->sbr_lf_gen = sbr_lf_gen;
    c->sbr_hf_assemble = sbr_hf_assemble;
    c->sbr_x_gen = sbr_x_gen;
    c->sbr_hf_inverse_filter = sbr_hf_inverse_filter;


    if(0)
        ff_aacsbr_func_ptr_init_mips(c);

}
enum AVOptionType{
    AV_OPT_TYPE_FLAGS,
    AV_OPT_TYPE_INT,
    AV_OPT_TYPE_INT64,
    AV_OPT_TYPE_DOUBLE,
    AV_OPT_TYPE_FLOAT,
    AV_OPT_TYPE_STRING,
    AV_OPT_TYPE_RATIONAL,
    AV_OPT_TYPE_BINARY,
    AV_OPT_TYPE_DICT,
    AV_OPT_TYPE_UINT64,
    AV_OPT_TYPE_CONST,
    AV_OPT_TYPE_IMAGE_SIZE,
    AV_OPT_TYPE_PIXEL_FMT,
    AV_OPT_TYPE_SAMPLE_FMT,
    AV_OPT_TYPE_VIDEO_RATE,
    AV_OPT_TYPE_DURATION,
    AV_OPT_TYPE_COLOR,
    AV_OPT_TYPE_CHANNEL_LAYOUT,
    AV_OPT_TYPE_BOOL,
};




typedef struct AVOption {
    const char *name;





    const char *help;





    int offset;
    enum AVOptionType type;




    union {
        int64_t i64;
        double dbl;
        const char *str;

        AVRational q;
    } default_val;
    double min;
    double max;

    int flags;
    const char *unit;
} AVOption;




typedef struct AVOptionRange {
    const char *str;





    double value_min, value_max;




    double component_min, component_max;




    int is_range;
} AVOptionRange;




typedef struct AVOptionRanges {
    AVOptionRange **range;



    int nb_ranges;



    int nb_components;
} AVOptionRanges;
int av_opt_show2(void *obj, void *av_log_obj, int req_flags, int rej_flags);






void av_opt_set_defaults(void *s);
void av_opt_set_defaults2(void *s, int mask, int flags);
int av_set_options_string(void *ctx, const char *opts,
                          const char *key_val_sep, const char *pairs_sep);
int av_opt_set_from_string(void *ctx, const char *opts,
                           const char *const *shorthand,
                           const char *key_val_sep, const char *pairs_sep);



void av_opt_free(void *obj);
int av_opt_flag_is_set(void *obj, const char *field_name, const char *flag_name);
int av_opt_set_dict(void *obj, struct AVDictionary **options);
int av_opt_set_dict2(void *obj, struct AVDictionary **options, int search_flags);
int av_opt_get_key_value(const char **ropts,
                         const char *key_val_sep, const char *pairs_sep,
                         unsigned flags,
                         char **rkey, char **rval);

enum {





    AV_OPT_FLAG_IMPLICIT_KEY = 1,
};
int av_opt_eval_flags (void *obj, const AVOption *o, const char *val, int *flags_out);
int av_opt_eval_int (void *obj, const AVOption *o, const char *val, int *int_out);
int av_opt_eval_int64 (void *obj, const AVOption *o, const char *val, int64_t *int64_out);
int av_opt_eval_float (void *obj, const AVOption *o, const char *val, float *float_out);
int av_opt_eval_double(void *obj, const AVOption *o, const char *val, double *double_out);
int av_opt_eval_q (void *obj, const AVOption *o, const char *val, AVRational *q_out);
const AVOption *av_opt_find(void *obj, const char *name, const char *unit,
                            int opt_flags, int search_flags);
const AVOption *av_opt_find2(void *obj, const char *name, const char *unit,
                             int opt_flags, int search_flags, void **target_obj);
const AVOption *av_opt_next(const void *obj, const AVOption *prev);







void *av_opt_child_next(void *obj, void *prev);
__attribute__((deprecated))
const AVClass *av_opt_child_class_next(const AVClass *parent, const AVClass *prev);
const AVClass *av_opt_child_class_iterate(const AVClass *parent, void **iter);
int av_opt_set (void *obj, const char *name, const char *val, int search_flags);
int av_opt_set_int (void *obj, const char *name, int64_t val, int search_flags);
int av_opt_set_double (void *obj, const char *name, double val, int search_flags);
int av_opt_set_q (void *obj, const char *name, AVRational val, int search_flags);
int av_opt_set_bin (void *obj, const char *name, const uint8_t *val, int size, int search_flags);
int av_opt_set_image_size(void *obj, const char *name, int w, int h, int search_flags);
int av_opt_set_pixel_fmt (void *obj, const char *name, enum AVPixelFormat fmt, int search_flags);
int av_opt_set_sample_fmt(void *obj, const char *name, enum AVSampleFormat fmt, int search_flags);
int av_opt_set_video_rate(void *obj, const char *name, AVRational val, int search_flags);
int av_opt_set_channel_layout(void *obj, const char *name, int64_t ch_layout, int search_flags);




int av_opt_set_dict_val(void *obj, const char *name, const AVDictionary *val, int search_flags);
int av_opt_get (void *obj, const char *name, int search_flags, uint8_t **out_val);
int av_opt_get_int (void *obj, const char *name, int search_flags, int64_t *out_val);
int av_opt_get_double (void *obj, const char *name, int search_flags, double *out_val);
int av_opt_get_q (void *obj, const char *name, int search_flags, AVRational *out_val);
int av_opt_get_image_size(void *obj, const char *name, int search_flags, int *w_out, int *h_out);
int av_opt_get_pixel_fmt (void *obj, const char *name, int search_flags, enum AVPixelFormat *out_fmt);
int av_opt_get_sample_fmt(void *obj, const char *name, int search_flags, enum AVSampleFormat *out_fmt);
int av_opt_get_video_rate(void *obj, const char *name, int search_flags, AVRational *out_val);
int av_opt_get_channel_layout(void *obj, const char *name, int search_flags, int64_t *ch_layout);




int av_opt_get_dict_val(void *obj, const char *name, int search_flags, AVDictionary **out_val);
void *av_opt_ptr(const AVClass *avclass, void *obj, const char *name);




void av_opt_freep_ranges(AVOptionRanges **ranges);
int av_opt_query_ranges(AVOptionRanges **, void *obj, const char *key, int flags);
int av_opt_copy(void *dest, const void *src);
int av_opt_query_ranges_default(AVOptionRanges **, void *obj, const char *key, int flags);
int av_opt_is_set_to_default(void *obj, const AVOption *o);
int av_opt_is_set_to_default_by_name(void *obj, const char *name, int search_flags);
int av_opt_serialize(void *obj, int opt_flags, int flags, char **buffer,
                     const char key_val_sep, const char pairs_sep);





typedef struct LLSModel {
    double __attribute__ ((aligned (32))) covariance[(((32 +1)+(4)-1)&~((4)-1))][(((32 +1)+(4)-1)&~((4)-1))];
    double __attribute__ ((aligned (32))) coeff[32][32];
    double variance[32];
    int indep_count;







    void (*update_lls)(struct LLSModel *m, const double *var);






    double (*evaluate_lls)(struct LLSModel *m, const double *var, int order);
} LLSModel;

void avpriv_init_lls(LLSModel *m, int indep_count);
void ff_init_lls_x86(LLSModel *m);
void avpriv_solve_lls(LLSModel *m, double threshold, unsigned short min_order);
enum FFLPCType {
    FF_LPC_TYPE_DEFAULT = -1,
    FF_LPC_TYPE_NONE = 0,
    FF_LPC_TYPE_FIXED = 1,
    FF_LPC_TYPE_LEVINSON = 2,
    FF_LPC_TYPE_CHOLESKY = 3,
    FF_LPC_TYPE_NB ,
};

typedef struct LPCContext {
    int blocksize;
    int max_order;
    enum FFLPCType lpc_type;
    double *windowed_buffer;
    double *windowed_samples;
    void (*lpc_apply_welch_window)(const int32_t *data, int len,
                                   double *w_data);
    void (*lpc_compute_autocorr)(const double *data, int len, int lag,
                                 double *autoc);


    LLSModel lls_models[2];
} LPCContext;





int ff_lpc_calc_coefs(LPCContext *s,
                      const int32_t *samples, int blocksize, int min_order,
                      int max_order, int precision,
                      int32_t coefs[][32], int *shift,
                      enum FFLPCType lpc_type, int lpc_passes,
                      int omethod, int min_shift, int max_shift, int zero_shift);

int ff_lpc_calc_ref_coefs(LPCContext *s,
                          const int32_t *samples, int order, double *ref);

double ff_lpc_calc_ref_coefs_f(LPCContext *s, const float *samples, int len,
                               int order, double *ref);




int ff_lpc_init(LPCContext *s, int blocksize, int max_order,
                enum FFLPCType lpc_type);
void ff_lpc_init_x86(LPCContext *s);




void ff_lpc_end(LPCContext *s);
typedef float LPC_TYPE;
typedef float LPC_TYPE_U;







static inline void compute_ref_coefs(const LPC_TYPE *autoc, int max_order,
                                     LPC_TYPE *ref, LPC_TYPE *error)
{
    int i, j;
    LPC_TYPE err;
    LPC_TYPE gen0[32], gen1[32];

    for (i = 0; i < max_order; i++)
        gen0[i] = gen1[i] = autoc[i + 1];

    err = autoc[0];
    ref[0] = -gen1[0] / err;
    err += gen1[0] * ref[0];
    if (error)
        error[0] = err;
    for (i = 1; i < max_order; i++) {
        for (j = 0; j < max_order - i; j++) {
            gen1[j] = gen1[j + 1] + ref[i - 1] * gen0[j];
            gen0[j] = gen1[j + 1] * ref[i - 1] + gen0[j];
        }
        ref[i] = -gen1[0] / err;
        err += gen1[0] * ref[i];
        if (error)
            error[i] = err;
    }
}





static inline int compute_lpc_coefs(const LPC_TYPE *autoc, int max_order,
                                    LPC_TYPE *lpc, int lpc_stride, int fail,
                                    int normalize)
{
    int i, j;
    LPC_TYPE err = 0;
    LPC_TYPE *lpc_last = lpc;

    ((void)0);

    if (normalize)
        err = *autoc++;

    if (fail && (autoc[max_order - 1] == 0 || err <= 0))
        return -1;

    for(i=0; i<max_order; i++) {
        LPC_TYPE r = (-autoc[i]);

        if (normalize) {
            for(j=0; j<i; j++)
                r -= lpc_last[j] * autoc[i-j-1];

            r /= err;
            err *= ((float)(1.0)) - (r * r);
        }

        lpc[i] = r;

        for(j=0; j < (i+1)>>1; j++) {
            LPC_TYPE f = lpc_last[ j];
            LPC_TYPE b = lpc_last[i-1-j];
            lpc[ j] = f + (LPC_TYPE_U)((r) * (b));
            lpc[i-1-j] = b + (LPC_TYPE_U)((r) * (f));
        }

        if (fail && err < 0)
            return -1;

        lpc_last = lpc;
        lpc += lpc_stride;
    }

    return 0;
}





static const int8_t tags_per_config[16] = { 0, 1, 1, 2, 3, 3, 4, 5, 0, 0, 0, 4, 5, 16, 5, 0 };

static const uint8_t aac_channel_layout_map[16][16][3] = {
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, },
    { { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, { TYPE_SCE, 1, AAC_CHANNEL_BACK }, },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 1, AAC_CHANNEL_BACK }, },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 1, AAC_CHANNEL_BACK }, { TYPE_LFE, 0, AAC_CHANNEL_LFE }, },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 1, AAC_CHANNEL_FRONT }, { TYPE_CPE, 2, AAC_CHANNEL_BACK }, { TYPE_LFE, 0, AAC_CHANNEL_LFE }, },
    { { 0, } },
    { { 0, } },
    { { 0, } },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 1, AAC_CHANNEL_BACK }, { TYPE_SCE, 1, AAC_CHANNEL_BACK }, { TYPE_LFE, 0, AAC_CHANNEL_LFE }, },
    { { TYPE_SCE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 0, AAC_CHANNEL_FRONT }, { TYPE_CPE, 1, AAC_CHANNEL_SIDE }, { TYPE_CPE, 2, AAC_CHANNEL_BACK }, { TYPE_LFE, 0, AAC_CHANNEL_LFE }, },
    {
      { TYPE_SCE, 0, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 0, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 1, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 2, AAC_CHANNEL_SIDE },
      { TYPE_CPE, 3, AAC_CHANNEL_BACK },
      { TYPE_SCE, 1, AAC_CHANNEL_BACK },
      { TYPE_LFE, 0, AAC_CHANNEL_LFE },
      { TYPE_LFE, 1, AAC_CHANNEL_LFE },
      { TYPE_SCE, 2, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 4, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 5, AAC_CHANNEL_SIDE },
      { TYPE_SCE, 3, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 6, AAC_CHANNEL_BACK },
      { TYPE_SCE, 4, AAC_CHANNEL_BACK },
      { TYPE_SCE, 5, AAC_CHANNEL_FRONT },
      { TYPE_CPE, 7, AAC_CHANNEL_FRONT },
    },
    { { 0, } },

};

static const uint64_t aac_channel_layout[16] = {
    (0x00000004),
    (0x00000001|0x00000002),
    ((0x00000001|0x00000002)|0x00000004),
    (((0x00000001|0x00000002)|0x00000004)|0x00000100),
    (((0x00000001|0x00000002)|0x00000004)|0x00000010|0x00000020),
    ((((0x00000001|0x00000002)|0x00000004)|0x00000010|0x00000020)|0x00000008),
    (((((0x00000001|0x00000002)|0x00000004)|0x00000010|0x00000020)|0x00000008)|0x00000040|0x00000080),
    0,
    0,
    0,
    (((((0x00000001|0x00000002)|0x00000004)|0x00000200|0x00000400)|0x00000008)|0x00000100),
    (((((0x00000001|0x00000002)|0x00000004)|0x00000200|0x00000400)|0x00000008)|0x00000010|0x00000020),
    (((((0x00000001|0x00000002)|0x00000004)|0x00000010|0x00000020)|0x00000008)|0x00000040|0x00000080|0x00000100|0x0000000800000000ULL|0x00000200|0x00000400|0x00001000|0x00004000|0x00002000|0x00000800|0x00008000|0x00020000|0x0000001000000000ULL|0x0000002000000000ULL|0x00010000|0x0000004000000000ULL|0x0000008000000000ULL|0x0000010000000000ULL),
    0,

};





extern const AVProfile ff_aac_profiles[];
extern const AVProfile ff_dca_profiles[];
extern const AVProfile ff_dnxhd_profiles[];
extern const AVProfile ff_h264_profiles[];
extern const AVProfile ff_hevc_profiles[];
extern const AVProfile ff_vvc_profiles[];
extern const AVProfile ff_jpeg2000_profiles[];
extern const AVProfile ff_mpeg2_video_profiles[];
extern const AVProfile ff_mpeg4_video_profiles[];
extern const AVProfile ff_vc1_profiles[];
extern const AVProfile ff_vp9_profiles[];
extern const AVProfile ff_av1_profiles[];
extern const AVProfile ff_sbc_profiles[];
extern const AVProfile ff_prores_profiles[];
extern const AVProfile ff_mjpeg_profiles[];
extern const AVProfile ff_arib_caption_profiles[];
static INTFLOAT __attribute__ ((aligned (32))) sine_120[120];
static INTFLOAT __attribute__ ((aligned (32))) sine_960[960];
static INTFLOAT __attribute__ ((aligned (32))) aac_kbd_long_960[960];
static INTFLOAT __attribute__ ((aligned (32))) aac_kbd_short_120[120];

static __attribute__((always_inline)) inline void reset_predict_state(PredictorState *ps)
{
    ps->r0 = 0.0f;
    ps->r1 = 0.0f;
    ps->cor0 = 0.0f;
    ps->cor1 = 0.0f;
    ps->var0 = 1.0f;
    ps->var1 = 1.0f;
}


static inline float *VMUL2(float *dst, const float *v, unsigned idx,
                           const float *scale)
{
    float s = *scale;
    *dst++ = v[idx & 15] * s;
    *dst++ = v[idx>>4 & 15] * s;
    return dst;
}



static inline float *VMUL4(float *dst, const float *v, unsigned idx,
                           const float *scale)
{
    float s = *scale;
    *dst++ = v[idx & 3] * s;
    *dst++ = v[idx>>2 & 3] * s;
    *dst++ = v[idx>>4 & 3] * s;
    *dst++ = v[idx>>6 & 3] * s;
    return dst;
}



static inline float *VMUL2S(float *dst, const float *v, unsigned idx,
                            unsigned sign, const float *scale)
{
    union av_intfloat32 s0, s1;

    s0.f = s1.f = *scale;
    s0.i ^= sign >> 1 << 31;
    s1.i ^= sign << 31;

    *dst++ = v[idx & 15] * s0.f;
    *dst++ = v[idx>>4 & 15] * s1.f;

    return dst;
}



static inline float *VMUL4S(float *dst, const float *v, unsigned idx,
                            unsigned sign, const float *scale)
{
    unsigned nz = idx >> 12;
    union av_intfloat32 s = { .f = *scale };
    union av_intfloat32 t;

    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx & 3] * t.f;

    sign <<= nz & 1; nz >>= 1;
    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx>>2 & 3] * t.f;

    sign <<= nz & 1; nz >>= 1;
    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx>>4 & 3] * t.f;

    sign <<= nz & 1;
    t.i = s.i ^ (sign & 1U<<31);
    *dst++ = v[idx>>6 & 3] * t.f;

    return dst;
}


static __attribute__((always_inline)) inline float flt16_round(float pf)
{
    union av_intfloat32 tmp;
    tmp.f = pf;
    tmp.i = (tmp.i + 0x00008000U) & 0xFFFF0000U;
    return tmp.f;
}

static __attribute__((always_inline)) inline float flt16_even(float pf)
{
    union av_intfloat32 tmp;
    tmp.f = pf;
    tmp.i = (tmp.i + 0x00007FFFU + (tmp.i & 0x00010000U >> 16)) & 0xFFFF0000U;
    return tmp.f;
}

static __attribute__((always_inline)) inline float flt16_trunc(float pf)
{
    union av_intfloat32 pun;
    pun.f = pf;
    pun.i &= 0xFFFF0000U;
    return pun.f;
}

static __attribute__((always_inline)) inline void predict(PredictorState *ps, float *coef,
                                     int output_enable)
{
    const float a = 0.953125;
    const float alpha = 0.90625;
    float e0, e1;
    float pv;
    float k1, k2;
    float r0 = ps->r0, r1 = ps->r1;
    float cor0 = ps->cor0, cor1 = ps->cor1;
    float var0 = ps->var0, var1 = ps->var1;

    k1 = var0 > 1 ? cor0 * flt16_even(a / var0) : 0;
    k2 = var1 > 1 ? cor1 * flt16_even(a / var1) : 0;

    pv = flt16_round(k1 * r0 + k2 * r1);
    if (output_enable)
        *coef += pv;

    e0 = *coef;
    e1 = e0 - k1 * r0;

    ps->cor1 = flt16_trunc(alpha * cor1 + r1 * e1);
    ps->var1 = flt16_trunc(alpha * var1 + 0.5f * (r1 * r1 + e1 * e1));
    ps->cor0 = flt16_trunc(alpha * cor0 + r0 * e0);
    ps->var0 = flt16_trunc(alpha * var0 + 0.5f * (r0 * r0 + e0 * e0));

    ps->r1 = flt16_trunc(a * (r0 - k1 * e0));
    ps->r0 = flt16_trunc(a * e0);
}






static void apply_dependent_coupling(AACContext *ac,
                                     SingleChannelElement *target,
                                     ChannelElement *cce, int index)
{
    IndividualChannelStream *ics = &cce->ch[0].ics;
    const uint16_t *offsets = ics->swb_offset;
    float *dest = target->coeffs;
    const float *src = cce->ch[0].coeffs;
    int g, i, group, k, idx = 0;
    if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP) {
        av_log(ac->avctx, 16,
               "Dependent coupling is not supported together with LTP\n");
        return;
    }
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            if (cce->ch[0].band_type[idx] != ZERO_BT) {
                const float gain = cce->coup.gain[index][idx];
                for (group = 0; group < ics->group_len[g]; group++) {
                    for (k = offsets[i]; k < offsets[i + 1]; k++) {

                        dest[group * 128 + k] += gain * src[group * 128 + k];
                    }
                }
            }
        }
        dest += ics->group_len[g] * 128;
        src += ics->group_len[g] * 128;
    }
}






static void apply_independent_coupling(AACContext *ac,
                                       SingleChannelElement *target,
                                       ChannelElement *cce, int index)
{
    const float gain = cce->coup.gain[index][0];
    const float *src = cce->ch[0].ret;
    float *dest = target->ret;
    const int len = 1024 << (ac->oc[1].m4ac.sbr == 1);

    ac->fdsp->vector_fmac_scalar(dest, src, gain, len);
}

static VLC vlc_scalefactors;
static VLC vlc_spectral[11];

static int output_configure(AACContext *ac,
                            uint8_t layout_map[16*4][3], int tags,
                            enum OCStatus oc_type, int get_new_frame);



static int count_channels(uint8_t (*layout)[3], int tags)
{
    int i, sum = 0;
    for (i = 0; i < tags; i++) {
        int syn_ele = layout[i][0];
        int pos = layout[i][2];
        sum += (1 + (syn_ele == TYPE_CPE)) *
               (pos != AAC_CHANNEL_OFF && pos != AAC_CHANNEL_CC);
    }
    return sum;
}
static __attribute__((cold)) int che_configure(AACContext *ac,
                                 enum ChannelPosition che_pos,
                                 int type, int id, int *channels)
{
    if (*channels >= 64)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    if (che_pos) {
        if (!ac->che[type][id]) {
            if (!(ac->che[type][id] = av_mallocz(sizeof(ChannelElement))))
                return (-(
                      12
                      ));
            ff_aac_sbr_ctx_init(ac, &ac->che[type][id]->sbr, type);
        }
        if (type != TYPE_CCE) {
            if (*channels >= 64 - (type == TYPE_CPE || (type == TYPE_SCE && ac->oc[1].m4ac.ps == 1))) {
                av_log(ac->avctx, 16, "Too many channels\n");
                return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
            }
            ac->output_element[(*channels)++] = &ac->che[type][id]->ch[0];
            if (type == TYPE_CPE ||
                (type == TYPE_SCE && ac->oc[1].m4ac.ps == 1)) {
                ac->output_element[(*channels)++] = &ac->che[type][id]->ch[1];
            }
        }
    } else {
        if (ac->che[type][id])
            ff_aac_sbr_ctx_close(&ac->che[type][id]->sbr);
        av_freep(&ac->che[type][id]);
    }
    return 0;
}

static int frame_configure_elements(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int type, id, ch, ret;


    for (type = 0; type < 4; type++) {
        for (id = 0; id < 16; id++) {
            ChannelElement *che = ac->che[type][id];
            if (che) {
                che->ch[0].ret = che->ch[0].ret_buf;
                che->ch[1].ret = che->ch[1].ret_buf;
            }
        }
    }


    av_frame_unref(ac->frame);
    if (!avctx->channels)
        return 1;

    ac->frame->nb_samples = 2048;
    if ((ret = ff_get_buffer(avctx, ac->frame, 0)) < 0)
        return ret;


    for (ch = 0; ch < avctx->channels; ch++) {
        if (ac->output_element[ch])
            ac->output_element[ch]->ret = (INTFLOAT *)ac->frame->extended_data[ch];
    }

    return 0;
}

struct elem_to_channel {
    uint64_t av_position;
    uint8_t syn_ele;
    uint8_t elem_id;
    uint8_t aac_position;
};

static int assign_pair(struct elem_to_channel e2c_vec[16],
                       uint8_t (*layout_map)[3], int offset, uint64_t left,
                       uint64_t right, int pos, uint64_t *layout)
{
    if (layout_map[offset][0] == TYPE_CPE) {
        e2c_vec[offset] = (struct elem_to_channel) {
            .av_position = left | right,
            .syn_ele = TYPE_CPE,
            .elem_id = layout_map[offset][1],
            .aac_position = pos
        };
        if (e2c_vec[offset].av_position != 
                                          (18446744073709551615UL)
                                                    )
            *layout |= e2c_vec[offset].av_position;

        return 1;
    } else {
        e2c_vec[offset] = (struct elem_to_channel) {
            .av_position = left,
            .syn_ele = TYPE_SCE,
            .elem_id = layout_map[offset][1],
            .aac_position = pos
        };
        e2c_vec[offset + 1] = (struct elem_to_channel) {
            .av_position = right,
            .syn_ele = TYPE_SCE,
            .elem_id = layout_map[offset + 1][1],
            .aac_position = pos
        };
        if (left != 
                   (18446744073709551615UL)
                             )
            *layout |= left;

        if (right != 
                    (18446744073709551615UL)
                              )
            *layout |= right;

        return 2;
    }
}

static int count_paired_channels(uint8_t (*layout_map)[3], int tags, int pos,
                                 int *current)
{
    int num_pos_channels = 0;
    int first_cpe = 0;
    int sce_parity = 0;
    int i;
    for (i = *current; i < tags; i++) {
        if (layout_map[i][2] != pos)
            break;
        if (layout_map[i][0] == TYPE_CPE) {
            if (sce_parity) {
                if (pos == AAC_CHANNEL_FRONT && !first_cpe) {
                    sce_parity = 0;
                } else {
                    return -1;
                }
            }
            num_pos_channels += 2;
            first_cpe = 1;
        } else {
            num_pos_channels++;
            sce_parity ^= 1;
        }
    }
    if (sce_parity &&
        ((pos == AAC_CHANNEL_FRONT && first_cpe) || pos == AAC_CHANNEL_SIDE))
        return -1;
    *current = i;
    return num_pos_channels;
}


static uint64_t sniff_channel_order(uint8_t (*layout_map)[3], int tags)
{
    int i, n, total_non_cc_elements;
    struct elem_to_channel e2c_vec[4 * 16] = { { 0 } };
    int num_front_channels, num_side_channels, num_back_channels;
    uint64_t layout = 0;

    if ((sizeof(e2c_vec) / sizeof((e2c_vec)[0])) < tags)
        return 0;

    i = 0;
    num_front_channels =
        count_paired_channels(layout_map, tags, AAC_CHANNEL_FRONT, &i);
    if (num_front_channels < 0)
        return 0;
    num_side_channels =
        count_paired_channels(layout_map, tags, AAC_CHANNEL_SIDE, &i);
    if (num_side_channels < 0)
        return 0;
    num_back_channels =
        count_paired_channels(layout_map, tags, AAC_CHANNEL_BACK, &i);
    if (num_back_channels < 0)
        return 0;

    if (num_side_channels == 0 && num_back_channels >= 4) {
        num_side_channels = 2;
        num_back_channels -= 2;
    }

    i = 0;
    if (num_front_channels & 1) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x00000004,
            .syn_ele = TYPE_SCE,
            .elem_id = layout_map[i][1],
            .aac_position = AAC_CHANNEL_FRONT
        };
        layout |= e2c_vec[i].av_position;
        i++;
        num_front_channels--;
    }
    if (num_front_channels >= 4) {
        i += assign_pair(e2c_vec, layout_map, i,
                         0x00000040,
                         0x00000080,
                         AAC_CHANNEL_FRONT, &layout);
        num_front_channels -= 2;
    }
    if (num_front_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         0x00000001,
                         0x00000002,
                         AAC_CHANNEL_FRONT, &layout);
        num_front_channels -= 2;
    }
    while (num_front_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         
                        (18446744073709551615UL)
                                  ,
                         
                        (18446744073709551615UL)
                                  ,
                         AAC_CHANNEL_FRONT, &layout);
        num_front_channels -= 2;
    }

    if (num_side_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         0x00000200,
                         0x00000400,
                         AAC_CHANNEL_FRONT, &layout);
        num_side_channels -= 2;
    }
    while (num_side_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         
                        (18446744073709551615UL)
                                  ,
                         
                        (18446744073709551615UL)
                                  ,
                         AAC_CHANNEL_SIDE, &layout);
        num_side_channels -= 2;
    }

    while (num_back_channels >= 4) {
        i += assign_pair(e2c_vec, layout_map, i,
                         
                        (18446744073709551615UL)
                                  ,
                         
                        (18446744073709551615UL)
                                  ,
                         AAC_CHANNEL_BACK, &layout);
        num_back_channels -= 2;
    }
    if (num_back_channels >= 2) {
        i += assign_pair(e2c_vec, layout_map, i,
                         0x00000010,
                         0x00000020,
                         AAC_CHANNEL_BACK, &layout);
        num_back_channels -= 2;
    }
    if (num_back_channels) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x00000100,
            .syn_ele = TYPE_SCE,
            .elem_id = layout_map[i][1],
            .aac_position = AAC_CHANNEL_BACK
        };
        layout |= e2c_vec[i].av_position;
        i++;
        num_back_channels--;
    }

    if (i < tags && layout_map[i][2] == AAC_CHANNEL_LFE) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x00000008,
            .syn_ele = TYPE_LFE,
            .elem_id = layout_map[i][1],
            .aac_position = AAC_CHANNEL_LFE
        };
        layout |= e2c_vec[i].av_position;
        i++;
    }
    if (i < tags && layout_map[i][2] == AAC_CHANNEL_LFE) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x0000000800000000ULL,
            .syn_ele = TYPE_LFE,
            .elem_id = layout_map[i][1],
            .aac_position = AAC_CHANNEL_LFE
        };
        layout |= e2c_vec[i].av_position;
        i++;
    }
    while (i < tags && layout_map[i][2] == AAC_CHANNEL_LFE) {
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 
                           (18446744073709551615UL)
                                     ,
            .syn_ele = TYPE_LFE,
            .elem_id = layout_map[i][1],
            .aac_position = AAC_CHANNEL_LFE
        };
        i++;
    }


    if (layout == ((((((0x00000001|0x00000002)|0x00000004)|0x00000010|0x00000020)|0x00000008)|0x00000040|0x00000080)|0x00000100|0x00000200|0x00000400|0x0000000800000000ULL) && tags == 16 && i == 8) {
        const uint8_t (*reference_layout_map)[3] = aac_channel_layout_map[12];
        for (int j = 0; j < tags; j++) {
            if (layout_map[j][0] != reference_layout_map[j][0] ||
                layout_map[j][2] != reference_layout_map[j][2])
                goto end_of_layout_definition;
        }

        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x00002000,
            .syn_ele = layout_map[i][0],
            .elem_id = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        i += assign_pair(e2c_vec, layout_map, i,
                         0x00001000,
                         0x00004000,
                         AAC_CHANNEL_FRONT,
                         &layout);
        i += assign_pair(e2c_vec, layout_map, i,
                         0x0000001000000000ULL,
                         0x0000002000000000ULL,
                         AAC_CHANNEL_SIDE,
                         &layout);
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x00000800,
            .syn_ele = layout_map[i][0],
            .elem_id = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        i += assign_pair(e2c_vec, layout_map, i,
                         0x00008000,
                         0x00020000,
                         AAC_CHANNEL_BACK,
                         &layout);
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x00010000,
            .syn_ele = layout_map[i][0],
            .elem_id = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        e2c_vec[i] = (struct elem_to_channel) {
            .av_position = 0x0000004000000000ULL,
            .syn_ele = layout_map[i][0],
            .elem_id = layout_map[i][1],
            .aac_position = layout_map[i][2]
        }; layout |= e2c_vec[i].av_position; i++;
        i += assign_pair(e2c_vec, layout_map, i,
                         0x0000008000000000ULL,
                         0x0000010000000000ULL,
                         AAC_CHANNEL_FRONT,
                         &layout);
    }

end_of_layout_definition:

    total_non_cc_elements = n = i;

    if (layout == (((((0x00000001|0x00000002)|0x00000004)|0x00000010|0x00000020)|0x00000008)|0x00000040|0x00000080|0x00000100|0x0000000800000000ULL|0x00000200|0x00000400|0x00001000|0x00004000|0x00002000|0x00000800|0x00008000|0x00020000|0x0000001000000000ULL|0x0000002000000000ULL|0x00010000|0x0000004000000000ULL|0x0000008000000000ULL|0x0000010000000000ULL)) {

        do{struct elem_to_channel SWAP_tmp= e2c_vec[0]; e2c_vec[0]= e2c_vec[2]; e2c_vec[2]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[1]; e2c_vec[1]= e2c_vec[2]; e2c_vec[2]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[2]; e2c_vec[2]= e2c_vec[6]; e2c_vec[6]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[3]; e2c_vec[3]= e2c_vec[4]; e2c_vec[4]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[4]; e2c_vec[4]= e2c_vec[6]; e2c_vec[6]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[6]; e2c_vec[6]= e2c_vec[7]; e2c_vec[7]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[8]; e2c_vec[8]= e2c_vec[9]; e2c_vec[9]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[10]; e2c_vec[10]= e2c_vec[11]; e2c_vec[11]= SWAP_tmp;}while(0);
        do{struct elem_to_channel SWAP_tmp= e2c_vec[11]; e2c_vec[11]= e2c_vec[12]; e2c_vec[12]= SWAP_tmp;}while(0);
    } else {


        do {
            int next_n = 0;
            for (i = 1; i < n; i++)
                if (e2c_vec[i - 1].av_position > e2c_vec[i].av_position) {
                    do{struct elem_to_channel SWAP_tmp= e2c_vec[i]; e2c_vec[i]= e2c_vec[i - 1]; e2c_vec[i - 1]= SWAP_tmp;}while(0);
                    next_n = i;
                }
            n = next_n;
        } while (n > 0);

    }

    for (i = 0; i < total_non_cc_elements; i++) {
        layout_map[i][0] = e2c_vec[i].syn_ele;
        layout_map[i][1] = e2c_vec[i].elem_id;
        layout_map[i][2] = e2c_vec[i].aac_position;
    }

    return layout;
}




static int push_output_configuration(AACContext *ac) {
    int pushed = 0;

    if (ac->oc[1].status == OC_LOCKED || ac->oc[0].status == OC_NONE) {
        ac->oc[0] = ac->oc[1];
        pushed = 1;
    }
    ac->oc[1].status = OC_NONE;
    return pushed;
}





static void pop_output_configuration(AACContext *ac) {
    if (ac->oc[1].status != OC_LOCKED && ac->oc[0].status != OC_NONE) {
        ac->oc[1] = ac->oc[0];
        ac->avctx->channels = ac->oc[1].channels;
        ac->avctx->channel_layout = ac->oc[1].channel_layout;
        output_configure(ac, ac->oc[1].layout_map, ac->oc[1].layout_map_tags,
                         ac->oc[1].status, 0);
    }
}







static int output_configure(AACContext *ac,
                            uint8_t layout_map[16 * 4][3], int tags,
                            enum OCStatus oc_type, int get_new_frame)
{
    AVCodecContext *avctx = ac->avctx;
    int i, channels = 0, ret;
    uint64_t layout = 0;
    uint8_t id_map[TYPE_END][16] = {{ 0 }};
    uint8_t type_counts[TYPE_END] = { 0 };

    if (ac->oc[1].layout_map != layout_map) {
        memcpy(ac->oc[1].layout_map, layout_map, tags * sizeof(layout_map[0]));
        ac->oc[1].layout_map_tags = tags;
    }
    for (i = 0; i < tags; i++) {
        int type = layout_map[i][0];
        int id = layout_map[i][1];
        id_map[type][id] = type_counts[type]++;
        if (id_map[type][id] >= 16) {
            avpriv_request_sample(ac->avctx, "Too large remapped id");
            return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
        }
    }


    if (avctx->request_channel_layout != 0x8000000000000000ULL)
        layout = sniff_channel_order(layout_map, tags);
    for (i = 0; i < tags; i++) {
        int type = layout_map[i][0];
        int id = layout_map[i][1];
        int iid = id_map[type][id];
        int position = layout_map[i][2];


        ret = che_configure(ac, position, type, iid, &channels);
        if (ret < 0)
            return ret;
        ac->tag_che_map[type][id] = ac->che[type][iid];
    }
    if (ac->oc[1].m4ac.ps == 1 && channels == 2) {
        if (layout == 0x00000004) {
            layout = 0x00000001|0x00000002;
        } else {
            layout = 0;
        }
    }

    if (layout) avctx->channel_layout = layout;
                            ac->oc[1].channel_layout = layout;
    avctx->channels = ac->oc[1].channels = channels;
    ac->oc[1].status = oc_type;

    if (get_new_frame) {
        if ((ret = frame_configure_elements(ac->avctx)) < 0)
            return ret;
    }

    return 0;
}

static void flush(AVCodecContext *avctx)
{
    AACContext *ac= avctx->priv_data;
    int type, i, j;

    for (type = 3; type >= 0; type--) {
        for (i = 0; i < 16; i++) {
            ChannelElement *che = ac->che[type][i];
            if (che) {
                for (j = 0; j <= 1; j++) {
                    memset(che->ch[j].saved, 0, sizeof(che->ch[j].saved));
                }
            }
        }
    }
}







static int set_default_channel_config(AACContext *ac, AVCodecContext *avctx,
                                      uint8_t (*layout_map)[3],
                                      int *tags,
                                      int channel_config)
{
    if (channel_config < 1 || (channel_config > 7 && channel_config < 11) ||
        channel_config > 13) {
        av_log(avctx, 16,
               "invalid default channel configuration (%d)\n",
               channel_config);
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }
    *tags = tags_per_config[channel_config];
    memcpy(layout_map, aac_channel_layout_map[channel_config - 1],
           *tags * sizeof(*layout_map));
    if (channel_config == 7 && avctx->strict_std_compliance < 1) {
        layout_map[2][2] = AAC_CHANNEL_SIDE;

        if (!ac || !ac->warned_71_wide++) {
            av_log(avctx, 32, "Assuming an incorrectly encoded 7.1 channel layout"
                   " instead of a spec-compliant 7.1(wide) layout, use -strict %d to decode"
                   " according to the specification instead.\n", 1);
        }
    }

    return 0;
}

static ChannelElement *get_che(AACContext *ac, int type, int elem_id)
{


    if (!ac->oc[1].m4ac.chan_config) {
        return ac->tag_che_map[type][elem_id];
    }

    if (!ac->tags_mapped && type == TYPE_CPE &&
        ac->oc[1].m4ac.chan_config == 1) {
        uint8_t layout_map[16*4][3];
        int layout_map_tags;
        push_output_configuration(ac);

        av_log(ac->avctx, 48, "mono with CPE\n");

        if (set_default_channel_config(ac, ac->avctx, layout_map,
                                       &layout_map_tags, 2) < 0)
            return 
                  ((void *)0)
                      ;
        if (output_configure(ac, layout_map, layout_map_tags,
                             OC_TRIAL_FRAME, 1) < 0)
            return 
                  ((void *)0)
                      ;

        ac->oc[1].m4ac.chan_config = 2;
        ac->oc[1].m4ac.ps = 0;
    }

    if (!ac->tags_mapped && type == TYPE_SCE &&
        ac->oc[1].m4ac.chan_config == 2) {
        uint8_t layout_map[16 * 4][3];
        int layout_map_tags;
        push_output_configuration(ac);

        av_log(ac->avctx, 48, "stereo with SCE\n");

        if (set_default_channel_config(ac, ac->avctx, layout_map,
                                       &layout_map_tags, 1) < 0)
            return 
                  ((void *)0)
                      ;
        if (output_configure(ac, layout_map, layout_map_tags,
                             OC_TRIAL_FRAME, 1) < 0)
            return 
                  ((void *)0)
                      ;

        ac->oc[1].m4ac.chan_config = 1;
        if (ac->oc[1].m4ac.sbr)
            ac->oc[1].m4ac.ps = -1;
    }


    switch (ac->oc[1].m4ac.chan_config) {
    case 13:
        if (ac->tags_mapped > 3 && ((type == TYPE_CPE && elem_id < 8) ||
                                    (type == TYPE_SCE && elem_id < 6) ||
                                    (type == TYPE_LFE && elem_id < 2))) {
            ac->tags_mapped++;
            return ac->tag_che_map[type][elem_id] = ac->che[type][elem_id];
        }
    case 12:
    case 7:
        if (ac->tags_mapped == 3 && type == TYPE_CPE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_CPE][elem_id] = ac->che[TYPE_CPE][2];
        }
    case 11:
        if (ac->tags_mapped == 2 &&
            ac->oc[1].m4ac.chan_config == 11 &&
            type == TYPE_SCE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_SCE][elem_id] = ac->che[TYPE_SCE][1];
        }
    case 6:






        if (ac->tags_mapped == tags_per_config[ac->oc[1].m4ac.chan_config] - 1 && (type == TYPE_LFE || type == TYPE_SCE)) {
            if (!ac->warned_remapping_once && (type != TYPE_LFE || elem_id != 0)) {
                av_log(ac->avctx, 24,
                   "This stream seems to incorrectly report its last channel as %s[%d], mapping to LFE[0]\n",
                   type == TYPE_SCE ? "SCE" : "LFE", elem_id);
                ac->warned_remapping_once++;
            }
            ac->tags_mapped++;
            return ac->tag_che_map[type][elem_id] = ac->che[TYPE_LFE][0];
        }
    case 5:
        if (ac->tags_mapped == 2 && type == TYPE_CPE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_CPE][elem_id] = ac->che[TYPE_CPE][1];
        }
    case 4:






        if (ac->tags_mapped == tags_per_config[ac->oc[1].m4ac.chan_config] - 1 && (type == TYPE_LFE || type == TYPE_SCE)) {
            if (!ac->warned_remapping_once && (type != TYPE_SCE || elem_id != 1)) {
                av_log(ac->avctx, 24,
                   "This stream seems to incorrectly report its last channel as %s[%d], mapping to SCE[1]\n",
                   type == TYPE_SCE ? "SCE" : "LFE", elem_id);
                ac->warned_remapping_once++;
            }
            ac->tags_mapped++;
            return ac->tag_che_map[type][elem_id] = ac->che[TYPE_SCE][1];
        }
        if (ac->tags_mapped == 2 &&
            ac->oc[1].m4ac.chan_config == 4 &&
            type == TYPE_SCE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_SCE][elem_id] = ac->che[TYPE_SCE][1];
        }
    case 3:
    case 2:
        if (ac->tags_mapped == (ac->oc[1].m4ac.chan_config != 2) &&
            type == TYPE_CPE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_CPE][elem_id] = ac->che[TYPE_CPE][0];
        } else if (ac->oc[1].m4ac.chan_config == 2) {
            return 
                  ((void *)0)
                      ;
        }
    case 1:
        if (!ac->tags_mapped && type == TYPE_SCE) {
            ac->tags_mapped++;
            return ac->tag_che_map[TYPE_SCE][elem_id] = ac->che[TYPE_SCE][0];
        }
    default:
        return 
              ((void *)0)
                  ;
    }
}







static void decode_channel_map(uint8_t layout_map[][3],
                               enum ChannelPosition type,
                               GetBitContext *gb, int n)
{
    while (n--) {
        enum RawDataBlockType syn_ele;
        switch (type) {
        case AAC_CHANNEL_FRONT:
        case AAC_CHANNEL_BACK:
        case AAC_CHANNEL_SIDE:
            syn_ele = get_bits1(gb);
            break;
        case AAC_CHANNEL_CC:
            skip_bits1(gb);
            syn_ele = TYPE_CCE;
            break;
        case AAC_CHANNEL_LFE:
            syn_ele = TYPE_LFE;
            break;
        default:

            do { if (!(0)) { av_log(
           ((void *)0)
           , 0, "Assertion %s failed at %s:%d\n", "0", "libavcodec/aacdec_template.c", 815); abort(); } } while (0);
        }
        layout_map[0][0] = syn_ele;
        layout_map[0][1] = get_bits(gb, 4);
        layout_map[0][2] = type;
        layout_map++;
    }
}

static inline void relative_align_get_bits(GetBitContext *gb,
                                           int reference_position) {
    int n = (reference_position - get_bits_count(gb) & 7);
    if (n)
        skip_bits(gb, n);
}






static int decode_pce(AVCodecContext *avctx, MPEG4AudioConfig *m4ac,
                      uint8_t (*layout_map)[3],
                      GetBitContext *gb, int byte_align_ref)
{
    int num_front, num_side, num_back, num_lfe, num_assoc_data, num_cc;
    int sampling_index;
    int comment_len;
    int tags;

    skip_bits(gb, 2);

    sampling_index = get_bits(gb, 4);
    if (m4ac->sampling_index != sampling_index)
        av_log(avctx, 24,
               "Sample rate index in program config element does not "
               "match the sample rate index configured by the container.\n");

    num_front = get_bits(gb, 4);
    num_side = get_bits(gb, 4);
    num_back = get_bits(gb, 4);
    num_lfe = get_bits(gb, 2);
    num_assoc_data = get_bits(gb, 3);
    num_cc = get_bits(gb, 4);

    if (get_bits1(gb))
        skip_bits(gb, 4);
    if (get_bits1(gb))
        skip_bits(gb, 4);

    if (get_bits1(gb))
        skip_bits(gb, 3);

    if (get_bits_left(gb) < 5 * (num_front + num_side + num_back + num_cc) + 4 *(num_lfe + num_assoc_data + num_cc)) {
        av_log(avctx, 16, "decode_pce: " "Input buffer exhausted before END element found\n");
        return -1;
    }
    decode_channel_map(layout_map , AAC_CHANNEL_FRONT, gb, num_front);
    tags = num_front;
    decode_channel_map(layout_map + tags, AAC_CHANNEL_SIDE, gb, num_side);
    tags += num_side;
    decode_channel_map(layout_map + tags, AAC_CHANNEL_BACK, gb, num_back);
    tags += num_back;
    decode_channel_map(layout_map + tags, AAC_CHANNEL_LFE, gb, num_lfe);
    tags += num_lfe;

    skip_bits_long(gb, 4 * num_assoc_data);

    decode_channel_map(layout_map + tags, AAC_CHANNEL_CC, gb, num_cc);
    tags += num_cc;

    relative_align_get_bits(gb, byte_align_ref);


    comment_len = get_bits(gb, 8) * 8;
    if (get_bits_left(gb) < comment_len) {
        av_log(avctx, 16, "decode_pce: " "Input buffer exhausted before END element found\n");
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }
    skip_bits_long(gb, comment_len);
    return tags;
}
static int decode_ga_specific_config(AACContext *ac, AVCodecContext *avctx,
                                     GetBitContext *gb,
                                     int get_bit_alignment,
                                     MPEG4AudioConfig *m4ac,
                                     int channel_config)
{
    int extension_flag, ret, ep_config, res_flags;
    uint8_t layout_map[16*4][3];
    int tags = 0;
    m4ac->frame_length_short = get_bits1(gb);
    if (m4ac->frame_length_short && m4ac->sbr == 1) {
      avpriv_report_missing_feature(avctx, "SBR with 960 frame length");
      if (ac) ac->warned_960_sbr = 1;
      m4ac->sbr = 0;
      m4ac->ps = 0;
    }


    if (get_bits1(gb))
        skip_bits(gb, 14);
    extension_flag = get_bits1(gb);

    if (m4ac->object_type == AOT_AAC_SCALABLE ||
        m4ac->object_type == AOT_ER_AAC_SCALABLE)
        skip_bits(gb, 3);

    if (channel_config == 0) {
        skip_bits(gb, 4);
        tags = decode_pce(avctx, m4ac, layout_map, gb, get_bit_alignment);
        if (tags < 0)
            return tags;
    } else {
        if ((ret = set_default_channel_config(ac, avctx, layout_map,
                                              &tags, channel_config)))
            return ret;
    }

    if (count_channels(layout_map, tags) > 1) {
        m4ac->ps = 0;
    } else if (m4ac->sbr == 1 && m4ac->ps == -1)
        m4ac->ps = 1;

    if (ac && (ret = output_configure(ac, layout_map, tags, OC_GLOBAL_HDR, 0)))
        return ret;

    if (extension_flag) {
        switch (m4ac->object_type) {
        case AOT_ER_BSAC:
            skip_bits(gb, 5);
            skip_bits(gb, 11);
            break;
        case AOT_ER_AAC_LC:
        case AOT_ER_AAC_LTP:
        case AOT_ER_AAC_SCALABLE:
        case AOT_ER_AAC_LD:
            res_flags = get_bits(gb, 3);
            if (res_flags) {
                avpriv_report_missing_feature(avctx,
                                              "AAC data resilience (flags %x)",
                                              res_flags);
                return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
            }
            break;
        }
        skip_bits1(gb);
    }
    switch (m4ac->object_type) {
    case AOT_ER_AAC_LC:
    case AOT_ER_AAC_LTP:
    case AOT_ER_AAC_SCALABLE:
    case AOT_ER_AAC_LD:
        ep_config = get_bits(gb, 2);
        if (ep_config) {
            avpriv_report_missing_feature(avctx,
                                          "epConfig %d", ep_config);
            return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
        }
    }
    return 0;
}

static int decode_eld_specific_config(AACContext *ac, AVCodecContext *avctx,
                                     GetBitContext *gb,
                                     MPEG4AudioConfig *m4ac,
                                     int channel_config)
{
    int ret, ep_config, res_flags;
    uint8_t layout_map[16*4][3];
    int tags = 0;
    const int ELDEXT_TERM = 0;

    m4ac->ps = 0;
    m4ac->sbr = 0;






    m4ac->frame_length_short = get_bits1(gb);

    res_flags = get_bits(gb, 3);
    if (res_flags) {
        avpriv_report_missing_feature(avctx,
                                      "AAC data resilience (flags %x)",
                                      res_flags);
        return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
    }

    if (get_bits1(gb)) {
        avpriv_report_missing_feature(avctx,
                                      "Low Delay SBR");
        return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
    }

    while (get_bits(gb, 4) != ELDEXT_TERM) {
        int len = get_bits(gb, 4);
        if (len == 15)
            len += get_bits(gb, 8);
        if (len == 15 + 255)
            len += get_bits(gb, 16);
        if (get_bits_left(gb) < len * 8 + 4) {
            av_log(avctx, 16, "Input buffer exhausted before END element found\n");
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
        skip_bits_long(gb, 8 * len);
    }

    if ((ret = set_default_channel_config(ac, avctx, layout_map,
                                          &tags, channel_config)))
        return ret;

    if (ac && (ret = output_configure(ac, layout_map, tags, OC_GLOBAL_HDR, 0)))
        return ret;

    ep_config = get_bits(gb, 2);
    if (ep_config) {
        avpriv_report_missing_feature(avctx,
                                      "epConfig %d", ep_config);
        return (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
    }
    return 0;
}
static int decode_audio_specific_config_gb(AACContext *ac,
                                           AVCodecContext *avctx,
                                           MPEG4AudioConfig *m4ac,
                                           GetBitContext *gb,
                                           int get_bit_alignment,
                                           int sync_extension)
{
    int i, ret;
    GetBitContext gbc = *gb;

    if ((i = ff_mpeg4audio_get_config_gb(m4ac, &gbc, sync_extension, avctx)) < 0)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    if (m4ac->sampling_index > 12) {
        av_log(avctx, 16,
               "invalid sampling rate index %d\n",
               m4ac->sampling_index);
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }
    if (m4ac->object_type == AOT_ER_AAC_LD &&
        (m4ac->sampling_index < 3 || m4ac->sampling_index > 7)) {
        av_log(avctx, 16,
               "invalid low delay sampling rate index %d\n",
               m4ac->sampling_index);
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }

    skip_bits_long(gb, i);

    switch (m4ac->object_type) {
    case AOT_AAC_MAIN:
    case AOT_AAC_LC:
    case AOT_AAC_SSR:
    case AOT_AAC_LTP:
    case AOT_ER_AAC_LC:
    case AOT_ER_AAC_LD:
        if ((ret = decode_ga_specific_config(ac, avctx, gb, get_bit_alignment,
                                            m4ac, m4ac->chan_config)) < 0)
            return ret;
        break;
    case AOT_ER_AAC_ELD:
        if ((ret = decode_eld_specific_config(ac, avctx, gb,
                                              m4ac, m4ac->chan_config)) < 0)
            return ret;
        break;
    default:
        avpriv_report_missing_feature(avctx,
                                      "Audio object type %s%d",
                                      m4ac->sbr == 1 ? "SBR+" : "",
                                      m4ac->object_type);
        return (-(
              38
              ));
    }

    do { if (0) av_log(avctx, 48, "AOT %d chan config %d sampling index %d (%d) SBR %d PS %d\n", m4ac->object_type, m4ac->chan_config, m4ac->sampling_index, m4ac->sample_rate, m4ac->sbr, m4ac->ps); } while (0)



                     ;

    return get_bits_count(gb);
}

static int decode_audio_specific_config(AACContext *ac,
                                        AVCodecContext *avctx,
                                        MPEG4AudioConfig *m4ac,
                                        const uint8_t *data, int64_t bit_size,
                                        int sync_extension)
{
    int i, ret;
    GetBitContext gb;

    if (bit_size < 0 || bit_size > 0x7fffffff
                                         ) {
        av_log(avctx, 16, "Audio specific config size is invalid\n");
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }

    do { if (0) av_log(avctx, 48, "audio specific config size %d\n", (int)bit_size >> 3); } while (0);
    for (i = 0; i < bit_size >> 3; i++)
        do { if (0) av_log(avctx, 48, "%02x ", data[i]); } while (0);
    do { if (0) av_log(avctx, 48, "\n"); } while (0);

    if ((ret = init_get_bits(&gb, data, bit_size)) < 0)
        return ret;

    return decode_audio_specific_config_gb(ac, avctx, m4ac, &gb, 0,
                                           sync_extension);
}
static __attribute__((always_inline)) inline int lcg_random(unsigned previous_val)
{
    union { unsigned u; int s; } v = { previous_val * 1664525u + 1013904223 };
    return v.s;
}

static void reset_all_predictors(PredictorState *ps)
{
    int i;
    for (i = 0; i < 672; i++)
        reset_predict_state(&ps[i]);
}

static int sample_rate_idx (int rate)
{
         if (92017 <= rate) return 0;
    else if (75132 <= rate) return 1;
    else if (55426 <= rate) return 2;
    else if (46009 <= rate) return 3;
    else if (37566 <= rate) return 4;
    else if (27713 <= rate) return 5;
    else if (23004 <= rate) return 6;
    else if (18783 <= rate) return 7;
    else if (13856 <= rate) return 8;
    else if (11502 <= rate) return 9;
    else if (9391 <= rate) return 10;
    else return 11;
}

static void reset_predictor_group(PredictorState *ps, int group_num)
{
    int i;
    for (i = group_num - 1; i < 672; i += 30)
        reset_predict_state(&ps[i]);
}

static void aacdec_init(AACContext *ac);

static __attribute__((cold)) void aac_static_table_init(void)
{
    static int16_t vlc_buf[304 + 270 + 550 + 300 + 328 +
                            294 + 306 + 268 + 510 + 366 + 462][2];
    for (unsigned i = 0, offset = 0; i < 11; i++) {
        vlc_spectral[i].table = &vlc_buf[offset];
        vlc_spectral[i].table_allocated = (sizeof(vlc_buf) / sizeof((vlc_buf)[0])) - offset;
        ff_init_vlc_sparse(&vlc_spectral[i], 8, ff_aac_spectral_sizes[i],
                           ff_aac_spectral_bits[i], sizeof(ff_aac_spectral_bits[i][0]),
                                                          sizeof(ff_aac_spectral_bits[i][0]),
                           ff_aac_spectral_codes[i], sizeof(ff_aac_spectral_codes[i][0]),
                                                          sizeof(ff_aac_spectral_codes[i][0]),
                           ff_aac_codebook_vector_idx[i], sizeof(ff_aac_codebook_vector_idx[i][0]),
                                                          sizeof(ff_aac_codebook_vector_idx[i][0]),
                 (1 | 4));
        offset += vlc_spectral[i].table_size;
    }

    ff_aac_sbr_init();

    ff_aac_tableinit();

    do { static int16_t table[352][2]; (&vlc_scalefactors)->table = table; (&vlc_scalefactors)->table_allocated = 352; ff_init_vlc_sparse(&vlc_scalefactors, 7, (sizeof(ff_aac_scalefactor_code) / sizeof((ff_aac_scalefactor_code)[0])), ff_aac_scalefactor_bits, sizeof(ff_aac_scalefactor_bits[0]), sizeof(ff_aac_scalefactor_bits[0]), ff_aac_scalefactor_code, sizeof(ff_aac_scalefactor_code[0]), sizeof(ff_aac_scalefactor_code[0]), 
   ((void *)0)
   , 0, 0, 0 | 4); } while (0)







                        ;



    ff_kbd_window_init(aac_kbd_long_960, 4.0, 960);
    ff_kbd_window_init(aac_kbd_short_120, 6.0, 120);
    ff_sine_window_init(sine_960, 960);
    ff_sine_window_init(sine_120, 120);
    ff_init_ff_sine_windows(9);
    ff_aac_float_common_init();






    ff_cbrt_tableinit();
}

static pthread_once_t aac_table_init = 
                              0
                                          ;

static __attribute__((cold)) int aac_decode_init(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int ret;

    if (avctx->sample_rate > 96000)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    ret = pthread_once(&aac_table_init, &aac_static_table_init);
    if (ret != 0)
        return (-(int)(('U') | (('N') << 8) | (('K') << 16) | ((unsigned)('N') << 24)));

    ac->avctx = avctx;
    ac->oc[1].m4ac.sample_rate = avctx->sample_rate;

    aacdec_init(ac);



    avctx->sample_fmt = AV_SAMPLE_FMT_FLTP;


    if (avctx->extradata_size > 0) {
        if ((ret = decode_audio_specific_config(ac, ac->avctx, &ac->oc[1].m4ac,
                                                avctx->extradata,
                                                avctx->extradata_size * 8LL,
                                                1)) < 0)
            return ret;
    } else {
        int sr, i;
        uint8_t layout_map[16*4][3];
        int layout_map_tags;

        sr = sample_rate_idx(avctx->sample_rate);
        ac->oc[1].m4ac.sampling_index = sr;
        ac->oc[1].m4ac.channels = avctx->channels;
        ac->oc[1].m4ac.sbr = -1;
        ac->oc[1].m4ac.ps = -1;

        for (i = 0; i < (sizeof(ff_mpeg4audio_channels) / sizeof((ff_mpeg4audio_channels)[0])); i++)
            if (ff_mpeg4audio_channels[i] == avctx->channels)
                break;
        if (i == (sizeof(ff_mpeg4audio_channels) / sizeof((ff_mpeg4audio_channels)[0]))) {
            i = 0;
        }
        ac->oc[1].m4ac.chan_config = i;

        if (ac->oc[1].m4ac.chan_config) {
            int ret = set_default_channel_config(ac, avctx, layout_map,
                &layout_map_tags, ac->oc[1].m4ac.chan_config);
            if (!ret)
                output_configure(ac, layout_map, layout_map_tags,
                                 OC_GLOBAL_HDR, 0);
            else if (avctx->err_recognition & (1<<3))
                return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
    }

    if (avctx->channels > 64) {
        av_log(avctx, 16, "Too many channels\n");
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }




    ac->fdsp = avpriv_float_dsp_alloc(avctx->flags & (1 << 23));

    if (!ac->fdsp) {
        return (-(
              12
              ));
    }

    ac->random_state = 0x1f2e3d4c;

    ff_mdct_init(&ac->mdct, 11, 1, 1.0 / (32768.0 * (1024.0)));
    ff_mdct_init(&ac->mdct_ld, 10, 1, 1.0 / (32768.0 * (512.0)));
    ff_mdct_init(&ac->mdct_small, 8, 1, 1.0 / (32768.0 * (128.0)));
    ff_mdct_init(&ac->mdct_ltp, 11, 0, (32768.0 * (-2.0)));

    ret = ff_mdct15_init(&ac->mdct120, 1, 3, 1.0f/(16*1024*120*2));
    if (ret < 0)
        return ret;
    ret = ff_mdct15_init(&ac->mdct480, 1, 5, 1.0f/(16*1024*960));
    if (ret < 0)
        return ret;
    ret = ff_mdct15_init(&ac->mdct960, 1, 6, 1.0f/(16*1024*960*2));
    if (ret < 0)
        return ret;


    return 0;
}




static int skip_data_stream_element(AACContext *ac, GetBitContext *gb)
{
    int byte_align = get_bits1(gb);
    int count = get_bits(gb, 8);
    if (count == 255)
        count += get_bits(gb, 8);
    if (byte_align)
        align_get_bits(gb);

    if (get_bits_left(gb) < 8 * count) {
        av_log(ac->avctx, 16, "skip_data_stream_element: ""Input buffer exhausted before END element found\n");
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }
    skip_bits_long(gb, 8 * count);
    return 0;
}

static int decode_prediction(AACContext *ac, IndividualChannelStream *ics,
                             GetBitContext *gb)
{
    int sfb;
    if (get_bits1(gb)) {
        ics->predictor_reset_group = get_bits(gb, 5);
        if (ics->predictor_reset_group == 0 ||
            ics->predictor_reset_group > 30) {
            av_log(ac->avctx, 16,
                   "Invalid Predictor Reset Group.\n");
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
    }
    for (sfb = 0; sfb < ((ics->max_sfb) > (ff_aac_pred_sfb_max[ac->oc[1].m4ac.sampling_index]) ? (ff_aac_pred_sfb_max[ac->oc[1].m4ac.sampling_index]) : (ics->max_sfb)); sfb++) {
        ics->prediction_used[sfb] = get_bits1(gb);
    }
    return 0;
}




static void decode_ltp(LongTermPrediction *ltp,
                       GetBitContext *gb, uint8_t max_sfb)
{
    int sfb;

    ltp->lag = get_bits(gb, 11);
    ltp->coef = ltp_coef[get_bits(gb, 3)];
    for (sfb = 0; sfb < ((max_sfb) > (40) ? (40) : (max_sfb)); sfb++)
        ltp->used[sfb] = get_bits1(gb);
}




static int decode_ics_info(AACContext *ac, IndividualChannelStream *ics,
                           GetBitContext *gb)
{
    const MPEG4AudioConfig *const m4ac = &ac->oc[1].m4ac;
    const int aot = m4ac->object_type;
    const int sampling_index = m4ac->sampling_index;
    int ret_fail = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    if (aot != AOT_ER_AAC_ELD) {
        if (get_bits1(gb)) {
            av_log(ac->avctx, 16, "Reserved bit set.\n");
            if (ac->avctx->err_recognition & (1<<1))
                return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
        ics->window_sequence[1] = ics->window_sequence[0];
        ics->window_sequence[0] = get_bits(gb, 2);
        if (aot == AOT_ER_AAC_LD &&
            ics->window_sequence[0] != ONLY_LONG_SEQUENCE) {
            av_log(ac->avctx, 16,
                   "AAC LD is only defined for ONLY_LONG_SEQUENCE but "
                   "window sequence %d found.\n", ics->window_sequence[0]);
            ics->window_sequence[0] = ONLY_LONG_SEQUENCE;
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
        ics->use_kb_window[1] = ics->use_kb_window[0];
        ics->use_kb_window[0] = get_bits1(gb);
    }
    ics->num_window_groups = 1;
    ics->group_len[0] = 1;
    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        int i;
        ics->max_sfb = get_bits(gb, 4);
        for (i = 0; i < 7; i++) {
            if (get_bits1(gb)) {
                ics->group_len[ics->num_window_groups - 1]++;
            } else {
                ics->num_window_groups++;
                ics->group_len[ics->num_window_groups - 1] = 1;
            }
        }
        ics->num_windows = 8;
        if (m4ac->frame_length_short) {
            ics->swb_offset = ff_swb_offset_120[sampling_index];
            ics->num_swb = ff_aac_num_swb_120[sampling_index];
        } else {
            ics->swb_offset = ff_swb_offset_128[sampling_index];
            ics->num_swb = ff_aac_num_swb_128[sampling_index];
        }
        ics->tns_max_bands = ff_tns_max_bands_128[sampling_index];
        ics->predictor_present = 0;
    } else {
        ics->max_sfb = get_bits(gb, 6);
        ics->num_windows = 1;
        if (aot == AOT_ER_AAC_LD || aot == AOT_ER_AAC_ELD) {
            if (m4ac->frame_length_short) {
                ics->swb_offset = ff_swb_offset_480[sampling_index];
                ics->num_swb = ff_aac_num_swb_480[sampling_index];
                ics->tns_max_bands = ff_tns_max_bands_480[sampling_index];
            } else {
                ics->swb_offset = ff_swb_offset_512[sampling_index];
                ics->num_swb = ff_aac_num_swb_512[sampling_index];
                ics->tns_max_bands = ff_tns_max_bands_512[sampling_index];
            }
            if (!ics->num_swb || !ics->swb_offset) {
                ret_fail = (-(int)(('B') | (('U') << 8) | (('G') << 16) | ((unsigned)('!') << 24)));
                goto fail;
            }
        } else {
            if (m4ac->frame_length_short) {
                ics->num_swb = ff_aac_num_swb_960[sampling_index];
                ics->swb_offset = ff_swb_offset_960[sampling_index];
            } else {
                ics->num_swb = ff_aac_num_swb_1024[sampling_index];
                ics->swb_offset = ff_swb_offset_1024[sampling_index];
            }
            ics->tns_max_bands = ff_tns_max_bands_1024[sampling_index];
        }
        if (aot != AOT_ER_AAC_ELD) {
            ics->predictor_present = get_bits1(gb);
            ics->predictor_reset_group = 0;
        }
        if (ics->predictor_present) {
            if (aot == AOT_AAC_MAIN) {
                if (decode_prediction(ac, ics, gb)) {
                    goto fail;
                }
            } else if (aot == AOT_AAC_LC ||
                       aot == AOT_ER_AAC_LC) {
                av_log(ac->avctx, 16,
                       "Prediction is not allowed in AAC-LC.\n");
                goto fail;
            } else {
                if (aot == AOT_ER_AAC_LD) {
                    av_log(ac->avctx, 16,
                           "LTP in ER AAC LD not yet implemented.\n");
                    ret_fail = (-(int)(('P') | (('A') << 8) | (('W') << 16) | ((unsigned)('E') << 24)));
                    goto fail;
                }
                if ((ics->ltp.present = get_bits(gb, 1)))
                    decode_ltp(&ics->ltp, gb, ics->max_sfb);
            }
        }
    }

    if (ics->max_sfb > ics->num_swb) {
        av_log(ac->avctx, 16,
               "Number of scalefactor bands in group (%d) "
               "exceeds limit (%d).\n",
               ics->max_sfb, ics->num_swb);
        goto fail;
    }

    return 0;
fail:
    ics->max_sfb = 0;
    return ret_fail;
}
static int decode_band_types(AACContext *ac, enum BandType band_type[120],
                             int band_type_run_end[120], GetBitContext *gb,
                             IndividualChannelStream *ics)
{
    int g, idx = 0;
    const int bits = (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) ? 3 : 5;
    for (g = 0; g < ics->num_window_groups; g++) {
        int k = 0;
        while (k < ics->max_sfb) {
            uint8_t sect_end = k;
            int sect_len_incr;
            int sect_band_type = get_bits(gb, 4);
            if (sect_band_type == 12) {
                av_log(ac->avctx, 16, "invalid band type\n");
                return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
            }
            do {
                sect_len_incr = get_bits(gb, bits);
                sect_end += sect_len_incr;
                if (get_bits_left(gb) < 0) {
                    av_log(ac->avctx, 16, "decode_band_types: ""Input buffer exhausted before END element found\n");
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
                if (sect_end > ics->max_sfb) {
                    av_log(ac->avctx, 16,
                           "Number of bands (%d) exceeds limit (%d).\n",
                           sect_end, ics->max_sfb);
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
            } while (sect_len_incr == (1 << bits) - 1);
            for (; k < sect_end; k++) {
                band_type [idx] = sect_band_type;
                band_type_run_end[idx++] = sect_end;
            }
        }
    }
    return 0;
}
static int decode_scalefactors(AACContext *ac, INTFLOAT sf[120], GetBitContext *gb,
                               unsigned int global_gain,
                               IndividualChannelStream *ics,
                               enum BandType band_type[120],
                               int band_type_run_end[120])
{
    int g, i, idx = 0;
    int offset[3] = { global_gain, global_gain - 90, 0 };
    int clipped_offset;
    int noise_flag = 1;
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb;) {
            int run_end = band_type_run_end[idx];
            if (band_type[idx] == ZERO_BT) {
                for (; i < run_end; i++, idx++)
                    sf[idx] = ((float)(0.));
            } else if ((band_type[idx] == INTENSITY_BT) ||
                       (band_type[idx] == INTENSITY_BT2)) {
                for (; i < run_end; i++, idx++) {
                    offset[2] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                    clipped_offset = av_clip_c(offset[2], -155, 100);
                    if (offset[2] != clipped_offset) {
                        avpriv_request_sample(ac->avctx,
                                              "If you heard an audible artifact, there may be a bug in the decoder. "
                                              "Clipped intensity stereo position (%d -> %d)",
                                              offset[2], clipped_offset);
                    }



                    sf[idx] = ff_aac_pow2sf_tab[-clipped_offset + 200];

                }
            } else if (band_type[idx] == NOISE_BT) {
                for (; i < run_end; i++, idx++) {
                    if (noise_flag-- > 0)
                        offset[1] += get_bits(gb, 9) - 256;
                    else
                        offset[1] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                    clipped_offset = av_clip_c(offset[1], -100, 155);
                    if (offset[1] != clipped_offset) {
                        avpriv_request_sample(ac->avctx,
                                              "If you heard an audible artifact, there may be a bug in the decoder. "
                                              "Clipped noise gain (%d -> %d)",
                                              offset[1], clipped_offset);
                    }



                    sf[idx] = -ff_aac_pow2sf_tab[clipped_offset + 200];

                }
            } else {
                for (; i < run_end; i++, idx++) {
                    offset[0] += get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                    if (offset[0] > 255U) {
                        av_log(ac->avctx, 16,
                               "Scalefactor (%d) out of range.\n", offset[0]);
                        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                    }



                    sf[idx] = -ff_aac_pow2sf_tab[offset[0] - 100 + 200];

                }
            }
        }
    }
    return 0;
}




static int decode_pulses(Pulse *pulse, GetBitContext *gb,
                         const uint16_t *swb_offset, int num_swb)
{
    int i, pulse_swb;
    pulse->num_pulse = get_bits(gb, 2) + 1;
    pulse_swb = get_bits(gb, 6);
    if (pulse_swb >= num_swb)
        return -1;
    pulse->pos[0] = swb_offset[pulse_swb];
    pulse->pos[0] += get_bits(gb, 5);
    if (pulse->pos[0] >= swb_offset[num_swb])
        return -1;
    pulse->amp[0] = get_bits(gb, 4);
    for (i = 1; i < pulse->num_pulse; i++) {
        pulse->pos[i] = get_bits(gb, 5) + pulse->pos[i - 1];
        if (pulse->pos[i] >= swb_offset[num_swb])
            return -1;
        pulse->amp[i] = get_bits(gb, 4);
    }
    return 0;
}






static int decode_tns(AACContext *ac, TemporalNoiseShaping *tns,
                      GetBitContext *gb, const IndividualChannelStream *ics)
{
    int w, filt, i, coef_len, coef_res, coef_compress;
    const int is8 = ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE;
    const int tns_max_order = is8 ? 7 : ac->oc[1].m4ac.object_type == AOT_AAC_MAIN ? 20 : 12;
    for (w = 0; w < ics->num_windows; w++) {
        if ((tns->n_filt[w] = get_bits(gb, 2 - is8))) {
            coef_res = get_bits1(gb);

            for (filt = 0; filt < tns->n_filt[w]; filt++) {
                int tmp2_idx;
                tns->length[w][filt] = get_bits(gb, 6 - 2 * is8);

                if ((tns->order[w][filt] = get_bits(gb, 5 - 2 * is8)) > tns_max_order) {
                    av_log(ac->avctx, 16,
                           "TNS filter order %d is greater than maximum %d.\n",
                           tns->order[w][filt], tns_max_order);
                    tns->order[w][filt] = 0;
                    return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                }
                if (tns->order[w][filt]) {
                    tns->direction[w][filt] = get_bits1(gb);
                    coef_compress = get_bits1(gb);
                    coef_len = coef_res + 3 - coef_compress;
                    tmp2_idx = 2 * coef_compress + coef_res;

                    for (i = 0; i < tns->order[w][filt]; i++)
                        tns->coef[w][filt][i] = tns_tmp2_map[tmp2_idx][get_bits(gb, coef_len)];
                }
            }
        }
    }
    return 0;
}
static void decode_mid_side_stereo(ChannelElement *cpe, GetBitContext *gb,
                                   int ms_present)
{
    int idx;
    int max_idx = cpe->ch[0].ics.num_window_groups * cpe->ch[0].ics.max_sfb;
    if (ms_present == 1) {
        for (idx = 0; idx < max_idx; idx++)
            cpe->ms_mask[idx] = get_bits1(gb);
    } else if (ms_present == 2) {
        memset(cpe->ms_mask, 1, max_idx * sizeof(cpe->ms_mask[0]));
    }
}
static int decode_spectrum_and_dequant(AACContext *ac, INTFLOAT coef[1024],
                                       GetBitContext *gb, const INTFLOAT sf[120],
                                       int pulse_present, const Pulse *pulse,
                                       const IndividualChannelStream *ics,
                                       enum BandType band_type[120])
{
    int i, k, g, idx = 0;
    const int c = 1024 / ics->num_windows;
    const uint16_t *offsets = ics->swb_offset;
    INTFLOAT *coef_base = coef;

    for (g = 0; g < ics->num_windows; g++)
        memset(coef + g * 128 + offsets[ics->max_sfb], 0,
               sizeof(INTFLOAT) * (c - offsets[ics->max_sfb]));

    for (g = 0; g < ics->num_window_groups; g++) {
        unsigned g_len = ics->group_len[g];

        for (i = 0; i < ics->max_sfb; i++, idx++) {
            const unsigned cbt_m1 = band_type[idx] - 1;
            INTFLOAT *cfo = coef + offsets[i];
            int off_len = offsets[i + 1] - offsets[i];
            int group;

            if (cbt_m1 >= INTENSITY_BT2 - 1) {
                for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                    memset(cfo, 0, off_len * sizeof(*cfo));
                }
            } else if (cbt_m1 == NOISE_BT - 1) {
                for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                    INTFLOAT band_energy;
                    float scale;

                    for (k = 0; k < off_len; k++) {
                        ac->random_state = lcg_random(ac->random_state);
                        cfo[k] = ac->random_state;
                    }

                    band_energy = ac->fdsp->scalarproduct_float(cfo, cfo, off_len);
                    scale = sf[idx] / sqrtf(band_energy);
                    ac->fdsp->vector_fmul_scalar(cfo, cfo, scale, off_len);

                }
            } else {

                const float *vq = ff_aac_codebook_vector_vals[cbt_m1];

                int16_t (*vlc_tab)[2] = vlc_spectral[cbt_m1].table;
                unsigned int re_index = (gb)->index; unsigned int __attribute__((unused)) re_cache; unsigned int re_size_plus8 = (gb)->size_in_bits_plus8;

                switch (cbt_m1 >> 1) {
                case 0:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;

                        do {
                            int code;
                            unsigned cb_idx;

                            re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
                            do { int n, nb_bits; unsigned int index; index = (((uint32_t)(re_cache))>>(32-(8))); code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 1 && n < 0) { re_index = ((re_size_plus8) > (re_index + (8)) ? (re_index + (8)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 2 && n < 0) { re_index = ((re_size_plus8) > (re_index + (nb_bits)) ? (re_index + (nb_bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; } } do { re_cache <<= (n); re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8)); } while (0); } while (0);
                            cb_idx = code;



                            cf = VMUL4(cf, vq, cb_idx, sf + idx);

                        } while (len -= 4);
                    }
                    break;

                case 1:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;

                        do {
                            int code;
                            unsigned nnz;
                            unsigned cb_idx;
                            uint32_t bits;

                            re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
                            do { int n, nb_bits; unsigned int index; index = (((uint32_t)(re_cache))>>(32-(8))); code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 1 && n < 0) { re_index = ((re_size_plus8) > (re_index + (8)) ? (re_index + (8)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 2 && n < 0) { re_index = ((re_size_plus8) > (re_index + (nb_bits)) ? (re_index + (nb_bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; } } do { re_cache <<= (n); re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8)); } while (0); } while (0);
                            cb_idx = code;
                            nnz = cb_idx >> 8 & 15;
                            bits = nnz ? ((uint32_t) re_cache) : 0;
                            re_index = ((re_size_plus8) > (re_index + (nnz)) ? (re_index + (nnz)) : (re_size_plus8));



                            cf = VMUL4S(cf, vq, cb_idx, bits, sf + idx);

                        } while (len -= 4);
                    }
                    break;

                case 2:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;

                        do {
                            int code;
                            unsigned cb_idx;

                            re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
                            do { int n, nb_bits; unsigned int index; index = (((uint32_t)(re_cache))>>(32-(8))); code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 1 && n < 0) { re_index = ((re_size_plus8) > (re_index + (8)) ? (re_index + (8)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 2 && n < 0) { re_index = ((re_size_plus8) > (re_index + (nb_bits)) ? (re_index + (nb_bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; } } do { re_cache <<= (n); re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8)); } while (0); } while (0);
                            cb_idx = code;



                            cf = VMUL2(cf, vq, cb_idx, sf + idx);

                        } while (len -= 2);
                    }
                    break;

                case 3:
                case 4:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {
                        INTFLOAT *cf = cfo;
                        int len = off_len;

                        do {
                            int code;
                            unsigned nnz;
                            unsigned cb_idx;
                            unsigned sign;

                            re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
                            do { int n, nb_bits; unsigned int index; index = (((uint32_t)(re_cache))>>(32-(8))); code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 1 && n < 0) { re_index = ((re_size_plus8) > (re_index + (8)) ? (re_index + (8)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 2 && n < 0) { re_index = ((re_size_plus8) > (re_index + (nb_bits)) ? (re_index + (nb_bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; } } do { re_cache <<= (n); re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8)); } while (0); } while (0);
                            cb_idx = code;
                            nnz = cb_idx >> 8 & 15;
                            sign = nnz ? (((uint32_t)(re_cache))>>(32-(nnz))) << (cb_idx >> 12) : 0;
                            re_index = ((re_size_plus8) > (re_index + (nnz)) ? (re_index + (nnz)) : (re_size_plus8));



                            cf = VMUL2S(cf, vq, cb_idx, sign, sf + idx);

                        } while (len -= 2);
                    }
                    break;

                default:
                    for (group = 0; group < (AAC_SIGNE)g_len; group++, cfo+=128) {




                        float *cf = cfo;
                        uint32_t *icf = (uint32_t *) cf;

                        int len = off_len;

                        do {
                            int code;
                            unsigned nzt, nnz;
                            unsigned cb_idx;
                            uint32_t bits;
                            int j;

                            re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
                            do { int n, nb_bits; unsigned int index; index = (((uint32_t)(re_cache))>>(32-(8))); code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 1 && n < 0) { re_index = ((re_size_plus8) > (re_index + (8)) ? (re_index + (8)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; if (2 > 2 && n < 0) { re_index = ((re_size_plus8) > (re_index + (nb_bits)) ? (re_index + (nb_bits)) : (re_size_plus8)); re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7); nb_bits = -n; index = (((uint32_t)(re_cache))>>(32-(nb_bits))) + code; code = vlc_tab[index][0]; n = vlc_tab[index][1]; } } do { re_cache <<= (n); re_index = ((re_size_plus8) > (re_index + (n)) ? (re_index + (n)) : (re_size_plus8)); } while (0); } while (0);
                            cb_idx = code;

                            if (cb_idx == 0x0000) {
                                *icf++ = 0;
                                *icf++ = 0;
                                continue;
                            }

                            nnz = cb_idx >> 12;
                            nzt = cb_idx >> 8;
                            bits = (((uint32_t)(re_cache))>>(32-(nnz))) << (32-nnz);
                            re_index = ((re_size_plus8) > (re_index + (nnz)) ? (re_index + (nnz)) : (re_size_plus8));

                            for (j = 0; j < 2; j++) {
                                if (nzt & 1<<j) {
                                    uint32_t b;
                                    int n;


                                    re_cache = av_bswap32((((const union unaligned_32 *) ((gb)->buffer + (re_index >> 3)))->l)) << (re_index & 7);
                                    b = ((uint32_t) re_cache);
                                    b = 31 - av_log2(~b);

                                    if (b > 8) {
                                        av_log(ac->avctx, 16, "error in spectral data, ESC overflow\n");
                                        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                                    }

                                    do { re_cache <<= (b + 1); re_index = ((re_size_plus8) > (re_index + (b + 1)) ? (re_index + (b + 1)) : (re_size_plus8)); } while (0);
                                    b += 4;
                                    n = (1 << b) + (((uint32_t)(re_cache))>>(32-(b)));
                                    re_index = ((re_size_plus8) > (re_index + (b)) ? (re_index + (b)) : (re_size_plus8));






                                    *icf++ = ff_cbrt_tab[n] | (bits & 1U<<31);

                                    bits <<= 1;
                                } else {






                                    unsigned v = ((const uint32_t*)vq)[cb_idx & 15];
                                    *icf++ = (bits & 1U<<31) | v;

                                    bits <<= !!v;
                                }
                                cb_idx >>= 4;
                            }
                        } while (len -= 2);

                        ac->fdsp->vector_fmul_scalar(cfo, cfo, sf[idx], off_len);

                    }
                }

                (gb)->index = re_index;
            }
        }
        coef += g_len << 7;
    }

    if (pulse_present) {
        idx = 0;
        for (i = 0; i < pulse->num_pulse; i++) {
            INTFLOAT co = coef_base[ pulse->pos[i] ];
            while (offsets[idx + 1] <= pulse->pos[i])
                idx++;
            if (band_type[idx] != NOISE_BT && sf[idx]) {
                INTFLOAT ico = -pulse->amp[i];






                if (co) {
                    co /= sf[idx];
                    ico = co / sqrtf(sqrtf(fabsf(co))) + (co > 0 ? -ico : ico);
                }
                coef_base[ pulse->pos[i] ] = cbrtf(fabsf(ico)) * ico * sf[idx];

            }
        }
    }
    return 0;
}




static void apply_prediction(AACContext *ac, SingleChannelElement *sce)
{
    int sfb, k;

    if (!sce->ics.predictor_initialized) {
        reset_all_predictors(sce->predictor_state);
        sce->ics.predictor_initialized = 1;
    }

    if (sce->ics.window_sequence[0] != EIGHT_SHORT_SEQUENCE) {
        for (sfb = 0;
             sfb < ff_aac_pred_sfb_max[ac->oc[1].m4ac.sampling_index];
             sfb++) {
            for (k = sce->ics.swb_offset[sfb];
                 k < sce->ics.swb_offset[sfb + 1];
                 k++) {
                predict(&sce->predictor_state[k], &sce->coeffs[k],
                        sce->ics.predictor_present &&
                        sce->ics.prediction_used[sfb]);
            }
        }
        if (sce->ics.predictor_reset_group)
            reset_predictor_group(sce->predictor_state,
                                  sce->ics.predictor_reset_group);
    } else
        reset_all_predictors(sce->predictor_state);
}

static void decode_gain_control(SingleChannelElement * sce, GetBitContext * gb)
{

    static const uint8_t gain_mode[4][3] = {
        {1, 0, 5},
        {2, 1, 2},
        {8, 0, 2},
        {2, 1, 5},
    };

    const int mode = sce->ics.window_sequence[0];
    uint8_t bd, wd, ad;


    uint8_t max_band = get_bits(gb, 2);
    for (bd = 0; bd < max_band; bd++) {
        for (wd = 0; wd < gain_mode[mode][0]; wd++) {
            uint8_t adjust_num = get_bits(gb, 3);
            for (ad = 0; ad < adjust_num; ad++) {
                skip_bits(gb, 4 + ((wd == 0 && gain_mode[mode][1])
                                     ? 4
                                     : gain_mode[mode][2]));
            }
        }
    }
}
static int decode_ics(AACContext *ac, SingleChannelElement *sce,
                      GetBitContext *gb, int common_window, int scale_flag)
{
    Pulse pulse;
    TemporalNoiseShaping *tns = &sce->tns;
    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *out = sce->coeffs;
    int global_gain, eld_syntax, er_syntax, pulse_present = 0;
    int ret;

    eld_syntax = ac->oc[1].m4ac.object_type == AOT_ER_AAC_ELD;
    er_syntax = ac->oc[1].m4ac.object_type == AOT_ER_AAC_LC ||
                 ac->oc[1].m4ac.object_type == AOT_ER_AAC_LTP ||
                 ac->oc[1].m4ac.object_type == AOT_ER_AAC_LD ||
                 ac->oc[1].m4ac.object_type == AOT_ER_AAC_ELD;




    pulse.num_pulse = 0;

    global_gain = get_bits(gb, 8);

    if (!common_window && !scale_flag) {
        ret = decode_ics_info(ac, ics, gb);
        if (ret < 0)
            goto fail;
    }

    if ((ret = decode_band_types(ac, sce->band_type,
                                 sce->band_type_run_end, gb, ics)) < 0)
        goto fail;
    if ((ret = decode_scalefactors(ac, sce->sf, gb, global_gain, ics,
                                  sce->band_type, sce->band_type_run_end)) < 0)
        goto fail;

    pulse_present = 0;
    if (!scale_flag) {
        if (!eld_syntax && (pulse_present = get_bits1(gb))) {
            if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
                av_log(ac->avctx, 16,
                       "Pulse tool not allowed in eight short sequence.\n");
                ret = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                goto fail;
            }
            if (decode_pulses(&pulse, gb, ics->swb_offset, ics->num_swb)) {
                av_log(ac->avctx, 16,
                       "Pulse data corrupt or invalid.\n");
                ret = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                goto fail;
            }
        }
        tns->present = get_bits1(gb);
        if (tns->present && !er_syntax) {
            ret = decode_tns(ac, tns, gb, ics);
            if (ret < 0)
                goto fail;
        }
        if (!eld_syntax && get_bits1(gb)) {
            decode_gain_control(sce, gb);
            if (!ac->warned_gain_control) {
                avpriv_report_missing_feature(ac->avctx, "Gain control");
                ac->warned_gain_control = 1;
            }
        }


        if (tns->present && er_syntax) {
            ret = decode_tns(ac, tns, gb, ics);
            if (ret < 0)
                goto fail;
        }
    }

    ret = decode_spectrum_and_dequant(ac, out, gb, sce->sf, pulse_present,
                                    &pulse, ics, sce->band_type);
    if (ret < 0)
        goto fail;

    if (ac->oc[1].m4ac.object_type == AOT_AAC_MAIN && !common_window)
        apply_prediction(ac, sce);

    return 0;
fail:
    tns->present = 0;
    return ret;
}




static void apply_mid_side_stereo(AACContext *ac, ChannelElement *cpe)
{
    const IndividualChannelStream *ics = &cpe->ch[0].ics;
    INTFLOAT *ch0 = cpe->ch[0].coeffs;
    INTFLOAT *ch1 = cpe->ch[1].coeffs;
    int g, i, group, idx = 0;
    const uint16_t *offsets = ics->swb_offset;
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb; i++, idx++) {
            if (cpe->ms_mask[idx] &&
                cpe->ch[0].band_type[idx] < NOISE_BT &&
                cpe->ch[1].band_type[idx] < NOISE_BT) {






                for (group = 0; group < ics->group_len[g]; group++) {
                    ac->fdsp->butterflies_float(ch0 + group * 128 + offsets[i],
                                               ch1 + group * 128 + offsets[i],
                                               offsets[i+1] - offsets[i]);

                }
            }
        }
        ch0 += ics->group_len[g] * 128;
        ch1 += ics->group_len[g] * 128;
    }
}
static void apply_intensity_stereo(AACContext *ac,
                                   ChannelElement *cpe, int ms_present)
{
    const IndividualChannelStream *ics = &cpe->ch[1].ics;
    SingleChannelElement *sce1 = &cpe->ch[1];
    INTFLOAT *coef0 = cpe->ch[0].coeffs, *coef1 = cpe->ch[1].coeffs;
    const uint16_t *offsets = ics->swb_offset;
    int g, group, i, idx = 0;
    int c;
    INTFLOAT scale;
    for (g = 0; g < ics->num_window_groups; g++) {
        for (i = 0; i < ics->max_sfb;) {
            if (sce1->band_type[idx] == INTENSITY_BT ||
                sce1->band_type[idx] == INTENSITY_BT2) {
                const int bt_run_end = sce1->band_type_run_end[idx];
                for (; i < bt_run_end; i++, idx++) {
                    c = -1 + 2 * (sce1->band_type[idx] - 14);
                    if (ms_present)
                        c *= 1 - 2 * cpe->ms_mask[idx];
                    scale = c * sce1->sf[idx];
                    for (group = 0; group < ics->group_len[g]; group++)







                        ac->fdsp->vector_fmul_scalar(coef1 + group * 128 + offsets[i],
                                                    coef0 + group * 128 + offsets[i],
                                                    scale,
                                                    offsets[i + 1] - offsets[i]);

                }
            } else {
                int bt_run_end = sce1->band_type_run_end[idx];
                idx += bt_run_end - i;
                i = bt_run_end;
            }
        }
        coef0 += ics->group_len[g] * 128;
        coef1 += ics->group_len[g] * 128;
    }
}






static int decode_cpe(AACContext *ac, GetBitContext *gb, ChannelElement *cpe)
{
    int i, ret, common_window, ms_present = 0;
    int eld_syntax = ac->oc[1].m4ac.object_type == AOT_ER_AAC_ELD;

    common_window = eld_syntax || get_bits1(gb);
    if (common_window) {
        if (decode_ics_info(ac, &cpe->ch[0].ics, gb))
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        i = cpe->ch[1].ics.use_kb_window[0];
        cpe->ch[1].ics = cpe->ch[0].ics;
        cpe->ch[1].ics.use_kb_window[1] = i;
        if (cpe->ch[1].ics.predictor_present &&
            (ac->oc[1].m4ac.object_type != AOT_AAC_MAIN))
            if ((cpe->ch[1].ics.ltp.present = get_bits(gb, 1)))
                decode_ltp(&cpe->ch[1].ics.ltp, gb, cpe->ch[1].ics.max_sfb);
        ms_present = get_bits(gb, 2);
        if (ms_present == 3) {
            av_log(ac->avctx, 16, "ms_present = 3 is reserved.\n");
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        } else if (ms_present)
            decode_mid_side_stereo(cpe, gb, ms_present);
    }
    if ((ret = decode_ics(ac, &cpe->ch[0], gb, common_window, 0)))
        return ret;
    if ((ret = decode_ics(ac, &cpe->ch[1], gb, common_window, 0)))
        return ret;

    if (common_window) {
        if (ms_present)
            apply_mid_side_stereo(ac, cpe);
        if (ac->oc[1].m4ac.object_type == AOT_AAC_MAIN) {
            apply_prediction(ac, &cpe->ch[0]);
            apply_prediction(ac, &cpe->ch[1]);
        }
    }

    apply_intensity_stereo(ac, cpe, ms_present);
    return 0;
}

static const float cce_scale[] = {
    1.09050773266525765921,
    1.18920711500272106672,
    
   1.41421356237309504880
          ,
    2,
};






static int decode_cce(AACContext *ac, GetBitContext *gb, ChannelElement *che)
{
    int num_gain = 0;
    int c, g, sfb, ret;
    int sign;
    INTFLOAT scale;
    SingleChannelElement *sce = &che->ch[0];
    ChannelCoupling *coup = &che->coup;

    coup->coupling_point = 2 * get_bits1(gb);
    coup->num_coupled = get_bits(gb, 3);
    for (c = 0; c <= coup->num_coupled; c++) {
        num_gain++;
        coup->type[c] = get_bits1(gb) ? TYPE_CPE : TYPE_SCE;
        coup->id_select[c] = get_bits(gb, 4);
        if (coup->type[c] == TYPE_CPE) {
            coup->ch_select[c] = get_bits(gb, 2);
            if (coup->ch_select[c] == 3)
                num_gain++;
        } else
            coup->ch_select[c] = 2;
    }
    coup->coupling_point += get_bits1(gb) || (coup->coupling_point >> 1);

    sign = get_bits(gb, 1);



    scale = cce_scale[get_bits(gb, 2)];


    if ((ret = decode_ics(ac, sce, gb, 0, 0)))
        return ret;

    for (c = 0; c < num_gain; c++) {
        int idx = 0;
        int cge = 1;
        int gain = 0;
        INTFLOAT gain_cache = ((float)(1.));
        if (c) {
            cge = coup->coupling_point == AFTER_IMDCT ? 1 : get_bits1(gb);
            gain = cge ? get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60: 0;
            gain_cache = powf((scale), -(gain));




        }
        if (coup->coupling_point == AFTER_IMDCT) {
            coup->gain[c][0] = gain_cache;
        } else {
            for (g = 0; g < sce->ics.num_window_groups; g++) {
                for (sfb = 0; sfb < sce->ics.max_sfb; sfb++, idx++) {
                    if (sce->band_type[idx] != ZERO_BT) {
                        if (!cge) {
                            int t = get_vlc2(gb, vlc_scalefactors.table, 7, 3) - 60;
                            if (t) {
                                int s = 1;
                                t = gain += t;
                                if (sign) {
                                    s -= 2 * (t & 0x1);
                                    t >>= 1;
                                }
                                gain_cache = powf((scale), -(t)) * s;




                            }
                        }
                        coup->gain[c][idx] = gain_cache;
                    }
                }
            }
        }
    }
    return 0;
}






static int decode_drc_channel_exclusions(DynamicRangeControl *che_drc,
                                         GetBitContext *gb)
{
    int i;
    int num_excl_chan = 0;

    do {
        for (i = 0; i < 7; i++)
            che_drc->exclude_mask[num_excl_chan++] = get_bits1(gb);
    } while (num_excl_chan < 64 - 7 && get_bits1(gb));

    return num_excl_chan / 7;
}






static int decode_dynamic_range(DynamicRangeControl *che_drc,
                                GetBitContext *gb)
{
    int n = 1;
    int drc_num_bands = 1;
    int i;


    if (get_bits1(gb)) {
        che_drc->pce_instance_tag = get_bits(gb, 4);
        skip_bits(gb, 4);
        n++;
    }


    if (get_bits1(gb)) {
        n += decode_drc_channel_exclusions(che_drc, gb);
    }


    if (get_bits1(gb)) {
        che_drc->band_incr = get_bits(gb, 4);
        che_drc->interpolation_scheme = get_bits(gb, 4);
        n++;
        drc_num_bands += che_drc->band_incr;
        for (i = 0; i < drc_num_bands; i++) {
            che_drc->band_top[i] = get_bits(gb, 8);
            n++;
        }
    }


    if (get_bits1(gb)) {
        che_drc->prog_ref_level = get_bits(gb, 7);
        skip_bits1(gb);
        n++;
    }

    for (i = 0; i < drc_num_bands; i++) {
        che_drc->dyn_rng_sgn[i] = get_bits1(gb);
        che_drc->dyn_rng_ctl[i] = get_bits(gb, 7);
        n++;
    }

    return n;
}

static int decode_fill(AACContext *ac, GetBitContext *gb, int len) {
    uint8_t buf[256];
    int i, major, minor;

    if (len < 13+7*8)
        goto unknown;

    get_bits(gb, 13); len -= 13;

    for(i=0; i+1<sizeof(buf) && len>=8; i++, len-=8)
        buf[i] = get_bits(gb, 8);

    buf[i] = 0;
    if (ac->avctx->debug & 1)
        av_log(ac->avctx, 48, "FILL:%s\n", buf);

    if (sscanf(buf, "libfaac %d.%d", &major, &minor) == 2){
        ac->avctx->internal->skip_samples = 1024;
    }

unknown:
    skip_bits_long(gb, len);

    return 0;
}
static int decode_extension_payload(AACContext *ac, GetBitContext *gb, int cnt,
                                    ChannelElement *che, enum RawDataBlockType elem_type)
{
    int crc_flag = 0;
    int res = cnt;
    int type = get_bits(gb, 4);

    if (ac->avctx->debug & 0x00000100)
        av_log(ac->avctx, 48, "extension type: %d len:%d\n", type, cnt);

    switch (type) {
    case EXT_SBR_DATA_CRC:
        crc_flag++;
    case EXT_SBR_DATA:
        if (!che) {
            av_log(ac->avctx, 16, "SBR was found before the first channel element.\n");
            return res;
        } else if (ac->oc[1].m4ac.frame_length_short) {
            if (!ac->warned_960_sbr)
              avpriv_report_missing_feature(ac->avctx,
                                            "SBR with 960 frame length");
            ac->warned_960_sbr = 1;
            skip_bits_long(gb, 8 * cnt - 4);
            return res;
        } else if (!ac->oc[1].m4ac.sbr) {
            av_log(ac->avctx, 16, "SBR signaled to be not-present but was found in the bitstream.\n");
            skip_bits_long(gb, 8 * cnt - 4);
            return res;
        } else if (ac->oc[1].m4ac.sbr == -1 && ac->oc[1].status == OC_LOCKED) {
            av_log(ac->avctx, 16, "Implicit SBR was found with a first occurrence after the first frame.\n");
            skip_bits_long(gb, 8 * cnt - 4);
            return res;
        } else if (ac->oc[1].m4ac.ps == -1 && ac->oc[1].status < OC_LOCKED && ac->avctx->channels == 1) {
            ac->oc[1].m4ac.sbr = 1;
            ac->oc[1].m4ac.ps = 1;
            ac->avctx->profile = 28;
            output_configure(ac, ac->oc[1].layout_map, ac->oc[1].layout_map_tags,
                             ac->oc[1].status, 1);
        } else {
            ac->oc[1].m4ac.sbr = 1;
            ac->avctx->profile = 4;
        }
        res = ff_decode_sbr_extension(ac, &che->sbr, gb, crc_flag, cnt, elem_type);
        break;
    case EXT_DYNAMIC_RANGE:
        res = decode_dynamic_range(&ac->che_drc, gb);
        break;
    case EXT_FILL:
        decode_fill(ac, gb, 8 * cnt - 4);
        break;
    case EXT_FILL_DATA:
    case EXT_DATA_ELEMENT:
    default:
        skip_bits_long(gb, 8 * cnt - 4);
        break;
    };
    return res;
}







static void apply_tns(INTFLOAT coef_param[1024], TemporalNoiseShaping *tns,
                      IndividualChannelStream *ics, int decode)
{
    const int mmm = ((ics->tns_max_bands) > (ics->max_sfb) ? (ics->max_sfb) : (ics->tns_max_bands));
    int w, filt, m, i;
    int bottom, top, order, start, end, size, inc;
    INTFLOAT lpc[20];
    INTFLOAT tmp[20 +1];
    UINTFLOAT *coef = coef_param;

    if(!mmm)
        return;

    for (w = 0; w < ics->num_windows; w++) {
        bottom = ics->num_swb;
        for (filt = 0; filt < tns->n_filt[w]; filt++) {
            top = bottom;
            bottom = ((0) > (top - tns->length[w][filt]) ? (0) : (top - tns->length[w][filt]));
            order = tns->order[w][filt];
            if (order == 0)
                continue;


            compute_lpc_coefs(tns->coef[w][filt], order, lpc, 0, 0, 0);

            start = ics->swb_offset[((bottom) > (mmm) ? (mmm) : (bottom))];
            end = ics->swb_offset[((top) > (mmm) ? (mmm) : (top))];
            if ((size = end - start) <= 0)
                continue;
            if (tns->direction[w][filt]) {
                inc = -1;
                start = end - 1;
            } else {
                inc = 1;
            }
            start += w * 128;

            if (decode) {

                for (m = 0; m < size; m++, start += inc)
                    for (i = 1; i <= ((m) > (order) ? (order) : (m)); i++)
                        coef[start] -= (((INTFLOAT)coef[start - i * inc]) * (lpc[i - 1]));
            } else {

                for (m = 0; m < size; m++, start += inc) {
                    tmp[0] = coef[start];
                    for (i = 1; i <= ((m) > (order) ? (order) : (m)); i++)
                        coef[start] += ((tmp[i]) * (lpc[i - 1]));
                    for (i = order; i > 0; i--)
                        tmp[i] = tmp[i - 1];
                }
            }
        }
    }
}





static void windowing_and_mdct_ltp(AACContext *ac, INTFLOAT *out,
                                   INTFLOAT *in, IndividualChannelStream *ics)
{
    const INTFLOAT *lwindow = ics->use_kb_window[0] ? ff_aac_kbd_long_1024 : ff_sine_1024;
    const INTFLOAT *swindow = ics->use_kb_window[0] ? ff_aac_kbd_short_128 : ff_sine_128;
    const INTFLOAT *lwindow_prev = ics->use_kb_window[1] ? ff_aac_kbd_long_1024 : ff_sine_1024;
    const INTFLOAT *swindow_prev = ics->use_kb_window[1] ? ff_aac_kbd_short_128 : ff_sine_128;

    if (ics->window_sequence[0] != LONG_STOP_SEQUENCE) {
        ac->fdsp->vector_fmul(in, in, lwindow_prev, 1024);
    } else {
        memset(in, 0, 448 * sizeof(*in));
        ac->fdsp->vector_fmul(in + 448, in + 448, swindow_prev, 128);
    }
    if (ics->window_sequence[0] != LONG_START_SEQUENCE) {
        ac->fdsp->vector_fmul_reverse(in + 1024, in + 1024, lwindow, 1024);
    } else {
        ac->fdsp->vector_fmul_reverse(in + 1024 + 448, in + 1024 + 448, swindow, 128);
        memset(in + 1024 + 576, 0, 448 * sizeof(*in));
    }
    ac->mdct_ltp.mdct_calc(&ac->mdct_ltp, out, in);
}




static void apply_ltp(AACContext *ac, SingleChannelElement *sce)
{
    const LongTermPrediction *ltp = &sce->ics.ltp;
    const uint16_t *offsets = sce->ics.swb_offset;
    int i, sfb;

    if (sce->ics.window_sequence[0] != EIGHT_SHORT_SEQUENCE) {
        INTFLOAT *predTime = sce->ret;
        INTFLOAT *predFreq = ac->buf_mdct;
        int16_t num_samples = 2048;

        if (ltp->lag < 1024)
            num_samples = ltp->lag + 1024;
        for (i = 0; i < num_samples; i++)
            predTime[i] = ((sce->ltp_state[i + 2048 - ltp->lag]) * (ltp->coef));
        memset(&predTime[i], 0, (2048 - i) * sizeof(*predTime));

        ac->windowing_and_mdct_ltp(ac, predFreq, predTime, &sce->ics);

        if (sce->tns.present)
            ac->apply_tns(predFreq, &sce->tns, &sce->ics, 0);

        for (sfb = 0; sfb < ((sce->ics.max_sfb) > (40) ? (40) : (sce->ics.max_sfb)); sfb++)
            if (ltp->used[sfb])
                for (i = offsets[sfb]; i < offsets[sfb + 1]; i++)
                    sce->coeffs[i] += (UINTFLOAT)predFreq[i];
    }
}




static void update_ltp(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *saved = sce->saved;
    INTFLOAT *saved_ltp = sce->coeffs;
    const INTFLOAT *lwindow = ics->use_kb_window[0] ? ff_aac_kbd_long_1024 : ff_sine_1024;
    const INTFLOAT *swindow = ics->use_kb_window[0] ? ff_aac_kbd_short_128 : ff_sine_128;
    int i;

    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        memcpy(saved_ltp, saved, 512 * sizeof(*saved_ltp));
        memset(saved_ltp + 576, 0, 448 * sizeof(*saved_ltp));
        ac->fdsp->vector_fmul_reverse(saved_ltp + 448, ac->buf_mdct + 960, &swindow[64], 64);

        for (i = 0; i < 64; i++)
            saved_ltp[i + 512] = ((ac->buf_mdct[1023 - i]) * (swindow[63 - i]));
    } else if (ics->window_sequence[0] == LONG_START_SEQUENCE) {
        memcpy(saved_ltp, ac->buf_mdct + 512, 448 * sizeof(*saved_ltp));
        memset(saved_ltp + 576, 0, 448 * sizeof(*saved_ltp));
        ac->fdsp->vector_fmul_reverse(saved_ltp + 448, ac->buf_mdct + 960, &swindow[64], 64);

        for (i = 0; i < 64; i++)
            saved_ltp[i + 512] = ((ac->buf_mdct[1023 - i]) * (swindow[63 - i]));
    } else {
        ac->fdsp->vector_fmul_reverse(saved_ltp, ac->buf_mdct + 512, &lwindow[512], 512);

        for (i = 0; i < 512; i++)
            saved_ltp[i + 512] = ((ac->buf_mdct[1023 - i]) * (lwindow[511 - i]));
    }

    memcpy(sce->ltp_state, sce->ltp_state+1024, 1024 * sizeof(*sce->ltp_state));
    memcpy(sce->ltp_state+1024, sce->ret, 1024 * sizeof(*sce->ltp_state));
    memcpy(sce->ltp_state+2048, saved_ltp, 1024 * sizeof(*sce->ltp_state));
}




static void imdct_and_windowing(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *in = sce->coeffs;
    INTFLOAT *out = sce->ret;
    INTFLOAT *saved = sce->saved;
    const INTFLOAT *swindow = ics->use_kb_window[0] ? ff_aac_kbd_short_128 : ff_sine_128;
    const INTFLOAT *lwindow_prev = ics->use_kb_window[1] ? ff_aac_kbd_long_1024 : ff_sine_1024;
    const INTFLOAT *swindow_prev = ics->use_kb_window[1] ? ff_aac_kbd_short_128 : ff_sine_128;
    INTFLOAT *buf = ac->buf_mdct;
    INTFLOAT *temp = ac->temp;
    int i;


    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        for (i = 0; i < 1024; i += 128)
            ac->mdct_small.imdct_half(&ac->mdct_small, buf + i, in + i);
    } else {
        ac->mdct.imdct_half(&ac->mdct, buf, in);




    }







    if ((ics->window_sequence[1] == ONLY_LONG_SEQUENCE || ics->window_sequence[1] == LONG_STOP_SEQUENCE) &&
            (ics->window_sequence[0] == ONLY_LONG_SEQUENCE || ics->window_sequence[0] == LONG_START_SEQUENCE)) {
        ac->fdsp->vector_fmul_window( out, saved, buf, lwindow_prev, 512);
    } else {
        memcpy( out, saved, 448 * sizeof(*out));

        if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
            ac->fdsp->vector_fmul_window(out + 448 + 0*128, saved + 448, buf + 0*128, swindow_prev, 64);
            ac->fdsp->vector_fmul_window(out + 448 + 1*128, buf + 0*128 + 64, buf + 1*128, swindow, 64);
            ac->fdsp->vector_fmul_window(out + 448 + 2*128, buf + 1*128 + 64, buf + 2*128, swindow, 64);
            ac->fdsp->vector_fmul_window(out + 448 + 3*128, buf + 2*128 + 64, buf + 3*128, swindow, 64);
            ac->fdsp->vector_fmul_window(temp, buf + 3*128 + 64, buf + 4*128, swindow, 64);
            memcpy( out + 448 + 4*128, temp, 64 * sizeof(*out));
        } else {
            ac->fdsp->vector_fmul_window(out + 448, saved + 448, buf, swindow_prev, 64);
            memcpy( out + 576, buf + 64, 448 * sizeof(*out));
        }
    }


    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        memcpy( saved, temp + 64, 64 * sizeof(*saved));
        ac->fdsp->vector_fmul_window(saved + 64, buf + 4*128 + 64, buf + 5*128, swindow, 64);
        ac->fdsp->vector_fmul_window(saved + 192, buf + 5*128 + 64, buf + 6*128, swindow, 64);
        ac->fdsp->vector_fmul_window(saved + 320, buf + 6*128 + 64, buf + 7*128, swindow, 64);
        memcpy( saved + 448, buf + 7*128 + 64, 64 * sizeof(*saved));
    } else if (ics->window_sequence[0] == LONG_START_SEQUENCE) {
        memcpy( saved, buf + 512, 448 * sizeof(*saved));
        memcpy( saved + 448, buf + 7*128 + 64, 64 * sizeof(*saved));
    } else {
        memcpy( saved, buf + 512, 512 * sizeof(*saved));
    }
}




static void imdct_and_windowing_960(AACContext *ac, SingleChannelElement *sce)
{

    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *in = sce->coeffs;
    INTFLOAT *out = sce->ret;
    INTFLOAT *saved = sce->saved;
    const INTFLOAT *swindow = ics->use_kb_window[0] ? aac_kbd_short_120 : sine_120;
    const INTFLOAT *lwindow_prev = ics->use_kb_window[1] ? aac_kbd_long_960 : sine_960;
    const INTFLOAT *swindow_prev = ics->use_kb_window[1] ? aac_kbd_short_120 : sine_120;
    INTFLOAT *buf = ac->buf_mdct;
    INTFLOAT *temp = ac->temp;
    int i;


    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        for (i = 0; i < 8; i++)
            ac->mdct120->imdct_half(ac->mdct120, buf + i * 120, in + i * 128, 1);
    } else {
        ac->mdct960->imdct_half(ac->mdct960, buf, in, 1);
    }
    if ((ics->window_sequence[1] == ONLY_LONG_SEQUENCE || ics->window_sequence[1] == LONG_STOP_SEQUENCE) &&
        (ics->window_sequence[0] == ONLY_LONG_SEQUENCE || ics->window_sequence[0] == LONG_START_SEQUENCE)) {
        ac->fdsp->vector_fmul_window( out, saved, buf, lwindow_prev, 480);
    } else {
        memcpy( out, saved, 420 * sizeof(*out));

        if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
            ac->fdsp->vector_fmul_window(out + 420 + 0*120, saved + 420, buf + 0*120, swindow_prev, 60);
            ac->fdsp->vector_fmul_window(out + 420 + 1*120, buf + 0*120 + 60, buf + 1*120, swindow, 60);
            ac->fdsp->vector_fmul_window(out + 420 + 2*120, buf + 1*120 + 60, buf + 2*120, swindow, 60);
            ac->fdsp->vector_fmul_window(out + 420 + 3*120, buf + 2*120 + 60, buf + 3*120, swindow, 60);
            ac->fdsp->vector_fmul_window(temp, buf + 3*120 + 60, buf + 4*120, swindow, 60);
            memcpy( out + 420 + 4*120, temp, 60 * sizeof(*out));
        } else {
            ac->fdsp->vector_fmul_window(out + 420, saved + 420, buf, swindow_prev, 60);
            memcpy( out + 540, buf + 60, 420 * sizeof(*out));
        }
    }


    if (ics->window_sequence[0] == EIGHT_SHORT_SEQUENCE) {
        memcpy( saved, temp + 60, 60 * sizeof(*saved));
        ac->fdsp->vector_fmul_window(saved + 60, buf + 4*120 + 60, buf + 5*120, swindow, 60);
        ac->fdsp->vector_fmul_window(saved + 180, buf + 5*120 + 60, buf + 6*120, swindow, 60);
        ac->fdsp->vector_fmul_window(saved + 300, buf + 6*120 + 60, buf + 7*120, swindow, 60);
        memcpy( saved + 420, buf + 7*120 + 60, 60 * sizeof(*saved));
    } else if (ics->window_sequence[0] == LONG_START_SEQUENCE) {
        memcpy( saved, buf + 480, 420 * sizeof(*saved));
        memcpy( saved + 420, buf + 7*120 + 60, 60 * sizeof(*saved));
    } else {
        memcpy( saved, buf + 480, 480 * sizeof(*saved));
    }

}
static void imdct_and_windowing_ld(AACContext *ac, SingleChannelElement *sce)
{
    IndividualChannelStream *ics = &sce->ics;
    INTFLOAT *in = sce->coeffs;
    INTFLOAT *out = sce->ret;
    INTFLOAT *saved = sce->saved;
    INTFLOAT *buf = ac->buf_mdct;





    ac->mdct.imdct_half(&ac->mdct_ld, buf, in);







    if (ics->use_kb_window[1]) {

        memcpy(out, saved, 192 * sizeof(*out));
        ac->fdsp->vector_fmul_window(out + 192, saved + 192, buf, ff_sine_128, 64);
        memcpy( out + 320, buf + 64, 192 * sizeof(*out));
    } else {
        ac->fdsp->vector_fmul_window(out, saved, buf, ff_sine_512, 256);
    }


    memcpy(saved, buf + 256, 256 * sizeof(*saved));
}

static void imdct_and_windowing_eld(AACContext *ac, SingleChannelElement *sce)
{
    UINTFLOAT *in = sce->coeffs;
    INTFLOAT *out = sce->ret;
    INTFLOAT *saved = sce->saved;
    INTFLOAT *buf = ac->buf_mdct;
    int i;
    const int n = ac->oc[1].m4ac.frame_length_short ? 480 : 512;
    const int n2 = n >> 1;
    const int n4 = n >> 2;
    const INTFLOAT *const window = n == 480 ? ff_aac_eld_window_480 :
                                           ff_aac_eld_window_512;






    for (i = 0; i < n2; i+=2) {
        INTFLOAT temp;
        temp = in[i ]; in[i ] = -in[n - 1 - i]; in[n - 1 - i] = temp;
        temp = -in[i + 1]; in[i + 1] = in[n - 2 - i]; in[n - 2 - i] = temp;
    }

    if (n == 480)
        ac->mdct480->imdct_half(ac->mdct480, buf, in, 1);
    else

        ac->mdct.imdct_half(&ac->mdct_ld, buf, in);






    for (i = 0; i < n; i+=2) {
        buf[i] = -buf[i];
    }







    for (i = n4; i < n2; i ++) {
        out[i - n4] = ((buf[ n2 - 1 - i]) * (window[i - n4])) +
                      ((saved[ i + n2]) * (window[i + n - n4])) +
                      ((-saved[n + n2 - 1 - i]) * (window[i + 2*n - n4])) +
                      ((-saved[ 2*n + n2 + i]) * (window[i + 3*n - n4]));
    }
    for (i = 0; i < n2; i ++) {
        out[n4 + i] = ((buf[ i]) * (window[i + n2 - n4])) +
                      ((-saved[ n - 1 - i]) * (window[i + n2 + n - n4])) +
                      ((-saved[ n + i]) * (window[i + n2 + 2*n - n4])) +
                      ((saved[2*n + n - 1 - i]) * (window[i + n2 + 3*n - n4]));
    }
    for (i = 0; i < n4; i ++) {
        out[n2 + n4 + i] = ((buf[ i + n2]) * (window[i + n - n4])) +
                           ((-saved[n2 - 1 - i]) * (window[i + 2*n - n4])) +
                           ((-saved[n + n2 + i]) * (window[i + 3*n - n4]));
    }


    memmove(saved + n, saved, 2 * n * sizeof(*saved));
    memcpy( saved, buf, n * sizeof(*saved));
}






static void apply_channel_coupling(AACContext *ac, ChannelElement *cc,
                                   enum RawDataBlockType type, int elem_id,
                                   enum CouplingPoint coupling_point,
                                   void (*apply_coupling_method)(AACContext *ac, SingleChannelElement *target, ChannelElement *cce, int index))
{
    int i, c;

    for (i = 0; i < 16; i++) {
        ChannelElement *cce = ac->che[TYPE_CCE][i];
        int index = 0;

        if (cce && cce->coup.coupling_point == coupling_point) {
            ChannelCoupling *coup = &cce->coup;

            for (c = 0; c <= coup->num_coupled; c++) {
                if (coup->type[c] == type && coup->id_select[c] == elem_id) {
                    if (coup->ch_select[c] != 1) {
                        apply_coupling_method(ac, &cc->ch[0], cce, index);
                        if (coup->ch_select[c] != 0)
                            index++;
                    }
                    if (coup->ch_select[c] != 2)
                        apply_coupling_method(ac, &cc->ch[1], cce, index++);
                } else
                    index += 1 + (coup->ch_select[c] == 3);
            }
        }
    }
}




static void spectral_to_sample(AACContext *ac, int samples)
{
    int i, type;
    void (*imdct_and_window)(AACContext *ac, SingleChannelElement *sce);
    switch (ac->oc[1].m4ac.object_type) {
    case AOT_ER_AAC_LD:
        imdct_and_window = imdct_and_windowing_ld;
        break;
    case AOT_ER_AAC_ELD:
        imdct_and_window = imdct_and_windowing_eld;
        break;
    default:
        if (ac->oc[1].m4ac.frame_length_short)
            imdct_and_window = imdct_and_windowing_960;
        else
            imdct_and_window = ac->imdct_and_windowing;
    }
    for (type = 3; type >= 0; type--) {
        for (i = 0; i < 16; i++) {
            ChannelElement *che = ac->che[type][i];
            if (che && che->present) {
                if (type <= TYPE_CPE)
                    apply_channel_coupling(ac, che, type, i, BEFORE_TNS, apply_dependent_coupling);
                if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP) {
                    if (che->ch[0].ics.predictor_present) {
                        if (che->ch[0].ics.ltp.present)
                            ac->apply_ltp(ac, &che->ch[0]);
                        if (che->ch[1].ics.ltp.present && type == TYPE_CPE)
                            ac->apply_ltp(ac, &che->ch[1]);
                    }
                }
                if (che->ch[0].tns.present)
                    ac->apply_tns(che->ch[0].coeffs, &che->ch[0].tns, &che->ch[0].ics, 1);
                if (che->ch[1].tns.present)
                    ac->apply_tns(che->ch[1].coeffs, &che->ch[1].tns, &che->ch[1].ics, 1);
                if (type <= TYPE_CPE)
                    apply_channel_coupling(ac, che, type, i, BETWEEN_TNS_AND_IMDCT, apply_dependent_coupling);
                if (type != TYPE_CCE || che->coup.coupling_point == AFTER_IMDCT) {
                    imdct_and_window(ac, &che->ch[0]);
                    if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP)
                        ac->update_ltp(ac, &che->ch[0]);
                    if (type == TYPE_CPE) {
                        imdct_and_window(ac, &che->ch[1]);
                        if (ac->oc[1].m4ac.object_type == AOT_AAC_LTP)
                            ac->update_ltp(ac, &che->ch[1]);
                    }
                    if (ac->oc[1].m4ac.sbr > 0) {
                        ff_sbr_apply(ac, &che->sbr, type, che->ch[0].ret, che->ch[1].ret);
                    }
                }
                if (type <= TYPE_CCE)
                    apply_channel_coupling(ac, che, type, i, AFTER_IMDCT, apply_independent_coupling);
                che->present = 0;
            } else if (che) {
                av_log(ac->avctx, 40, "ChannelElement %d.%d missing \n", type, i);
            }
        }
    }
}

static int parse_adts_frame_header(AACContext *ac, GetBitContext *gb)
{
    int size;
    AACADTSHeaderInfo hdr_info;
    uint8_t layout_map[16*4][3];
    int layout_map_tags, ret;

    size = ff_adts_header_parse(gb, &hdr_info);
    if (size > 0) {
        if (!ac->warned_num_aac_frames && hdr_info.num_aac_frames != 1) {


            avpriv_report_missing_feature(ac->avctx,
                                          "More than one AAC RDB per ADTS frame");
            ac->warned_num_aac_frames = 1;
        }
        push_output_configuration(ac);
        if (hdr_info.chan_config) {
            ac->oc[1].m4ac.chan_config = hdr_info.chan_config;
            if ((ret = set_default_channel_config(ac, ac->avctx,
                                                  layout_map,
                                                  &layout_map_tags,
                                                  hdr_info.chan_config)) < 0)
                return ret;
            if ((ret = output_configure(ac, layout_map, layout_map_tags,
                                        ((ac->oc[1].status) > (OC_TRIAL_FRAME) ? (ac->oc[1].status) : (OC_TRIAL_FRAME))
                                                             , 0)) < 0)
                return ret;
        } else {
            ac->oc[1].m4ac.chan_config = 0;





            if (ac->dmono_mode && ac->oc[0].status == OC_NONE) {
                layout_map_tags = 2;
                layout_map[0][0] = layout_map[1][0] = TYPE_SCE;
                layout_map[0][2] = layout_map[1][2] = AAC_CHANNEL_FRONT;
                layout_map[0][1] = 0;
                layout_map[1][1] = 1;
                if (output_configure(ac, layout_map, layout_map_tags,
                                     OC_TRIAL_FRAME, 0))
                    return -7;
            }
        }
        ac->oc[1].m4ac.sample_rate = hdr_info.sample_rate;
        ac->oc[1].m4ac.sampling_index = hdr_info.sampling_index;
        ac->oc[1].m4ac.object_type = hdr_info.object_type;
        ac->oc[1].m4ac.frame_length_short = 0;
        if (ac->oc[0].status != OC_LOCKED ||
            ac->oc[0].m4ac.chan_config != hdr_info.chan_config ||
            ac->oc[0].m4ac.sample_rate != hdr_info.sample_rate) {
            ac->oc[1].m4ac.sbr = -1;
            ac->oc[1].m4ac.ps = -1;
        }
        if (!hdr_info.crc_absent)
            skip_bits(gb, 16);
    }
    return size;
}

static int aac_decode_er_frame(AVCodecContext *avctx, void *data,
                               int *got_frame_ptr, GetBitContext *gb)
{
    AACContext *ac = avctx->priv_data;
    const MPEG4AudioConfig *const m4ac = &ac->oc[1].m4ac;
    ChannelElement *che;
    int err, i;
    int samples = m4ac->frame_length_short ? 960 : 1024;
    int chan_config = m4ac->chan_config;
    int aot = m4ac->object_type;

    if (aot == AOT_ER_AAC_LD || aot == AOT_ER_AAC_ELD)
        samples >>= 1;

    ac->frame = data;

    if ((err = frame_configure_elements(avctx)) < 0)
        return err;



    ac->avctx->profile = aot - 1;

    ac->tags_mapped = 0;

    if (chan_config < 0 || (chan_config >= 8 && chan_config < 11) || chan_config >= 13) {
        avpriv_request_sample(avctx, "Unknown ER channel configuration %d",
                              chan_config);
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }
    for (i = 0; i < tags_per_config[chan_config]; i++) {
        const int elem_type = aac_channel_layout_map[chan_config-1][i][0];
        const int elem_id = aac_channel_layout_map[chan_config-1][i][1];
        if (!(che=get_che(ac, elem_type, elem_id))) {
            av_log(ac->avctx, 16,
                   "channel element %d.%d is not allocated\n",
                   elem_type, elem_id);
            return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        }
        che->present = 1;
        if (aot != AOT_ER_AAC_ELD)
            skip_bits(gb, 4);
        switch (elem_type) {
        case TYPE_SCE:
            err = decode_ics(ac, &che->ch[0], gb, 0, 0);
            break;
        case TYPE_CPE:
            err = decode_cpe(ac, gb, che);
            break;
        case TYPE_LFE:
            err = decode_ics(ac, &che->ch[0], gb, 0, 0);
            break;
        }
        if (err < 0)
            return err;
    }

    spectral_to_sample(ac, samples);

    if (!ac->frame->data[0] && samples) {
        av_log(avctx, 16, "no frame data found\n");
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
    }

    ac->frame->nb_samples = samples;
    ac->frame->sample_rate = avctx->sample_rate;
    *got_frame_ptr = 1;

    skip_bits_long(gb, get_bits_left(gb));
    return 0;
}

static int aac_decode_frame_int(AVCodecContext *avctx, void *data,
                                int *got_frame_ptr, GetBitContext *gb,
                                const AVPacket *avpkt)
{
    AACContext *ac = avctx->priv_data;
    ChannelElement *che = 
                         ((void *)0)
                             , *che_prev = 
                                           ((void *)0)
                                               ;
    enum RawDataBlockType elem_type, che_prev_type = TYPE_END;
    int err, elem_id;
    int samples = 0, multiplier, audio_found = 0, pce_found = 0;
    int is_dmono, sce_count = 0;
    int payload_alignment;
    uint8_t che_presence[4][16] = {{0}};

    ac->frame = data;

    if (show_bits(gb, 12) == 0xfff) {
        if ((err = parse_adts_frame_header(ac, gb)) < 0) {
            av_log(avctx, 16, "Error decoding AAC frame header.\n");
            goto fail;
        }
        if (ac->oc[1].m4ac.sampling_index > 12) {
            av_log(ac->avctx, 16, "invalid sampling rate index %d\n", ac->oc[1].m4ac.sampling_index);
            err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
            goto fail;
        }
    }

    if ((err = frame_configure_elements(avctx)) < 0)
        goto fail;



    ac->avctx->profile = ac->oc[1].m4ac.object_type - 1;

    payload_alignment = get_bits_count(gb);
    ac->tags_mapped = 0;

    while ((elem_type = get_bits(gb, 3)) != TYPE_END) {
        elem_id = get_bits(gb, 4);

        if (avctx->debug & 0x00000100)
            av_log(avctx, 48, "Elem type:%x id:%x\n", elem_type, elem_id);

        if (!avctx->channels && elem_type != TYPE_PCE) {
            err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
            goto fail;
        }

        if (elem_type < TYPE_DSE) {
            if (che_presence[elem_type][elem_id]) {
                int error = che_presence[elem_type][elem_id] > 1;
                av_log(ac->avctx, error ? 16 : 48, "channel element %d.%d duplicate\n",
                       elem_type, elem_id);
                if (error) {
                    err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                    goto fail;
                }
            }
            che_presence[elem_type][elem_id]++;

            if (!(che=get_che(ac, elem_type, elem_id))) {
                av_log(ac->avctx, 16, "channel element %d.%d is not allocated\n",
                       elem_type, elem_id);
                err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                goto fail;
            }
            samples = ac->oc[1].m4ac.frame_length_short ? 960 : 1024;
            che->present = 1;
        }

        switch (elem_type) {

        case TYPE_SCE:
            err = decode_ics(ac, &che->ch[0], gb, 0, 0);
            audio_found = 1;
            sce_count++;
            break;

        case TYPE_CPE:
            err = decode_cpe(ac, gb, che);
            audio_found = 1;
            break;

        case TYPE_CCE:
            err = decode_cce(ac, gb, che);
            break;

        case TYPE_LFE:
            err = decode_ics(ac, &che->ch[0], gb, 0, 0);
            audio_found = 1;
            break;

        case TYPE_DSE:
            err = skip_data_stream_element(ac, gb);
            break;

        case TYPE_PCE: {
            uint8_t layout_map[16*4][3] = {{0}};
            int tags;

            int pushed = push_output_configuration(ac);
            if (pce_found && !pushed) {
                err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                goto fail;
            }

            tags = decode_pce(avctx, &ac->oc[1].m4ac, layout_map, gb,
                              payload_alignment);
            if (tags < 0) {
                err = tags;
                break;
            }
            if (pce_found) {
                av_log(avctx, 16,
                       "Not evaluating a further program_config_element as this construct is dubious at best.\n");
                pop_output_configuration(ac);
            } else {
                err = output_configure(ac, layout_map, tags, OC_TRIAL_PCE, 1);
                if (!err)
                    ac->oc[1].m4ac.chan_config = 0;
                pce_found = 1;
            }
            break;
        }

        case TYPE_FIL:
            if (elem_id == 15)
                elem_id += get_bits(gb, 8) - 1;
            if (get_bits_left(gb) < 8 * elem_id) {
                    av_log(avctx, 16, "TYPE_FIL: ""Input buffer exhausted before END element found\n");
                    err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
                    goto fail;
            }
            err = 0;
            while (elem_id > 0) {
                int ret = decode_extension_payload(ac, gb, elem_id, che_prev, che_prev_type);
                if (ret < 0) {
                    err = ret;
                    break;
                }
                elem_id -= ret;
            }
            break;

        default:
            err = (-(int)(('B') | (('U') << 8) | (('G') << 16) | ((unsigned)('!') << 24)));
            break;
        }

        if (elem_type < TYPE_DSE) {
            che_prev = che;
            che_prev_type = elem_type;
        }

        if (err)
            goto fail;

        if (get_bits_left(gb) < 3) {
            av_log(avctx, 16, "Input buffer exhausted before END element found\n");
            err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
            goto fail;
        }
    }

    if (!avctx->channels) {
        *got_frame_ptr = 0;
        return 0;
    }

    multiplier = (ac->oc[1].m4ac.sbr == 1) ? ac->oc[1].m4ac.ext_sample_rate > ac->oc[1].m4ac.sample_rate : 0;
    samples <<= multiplier;

    spectral_to_sample(ac, samples);

    if (ac->oc[1].status && audio_found) {
        avctx->sample_rate = ac->oc[1].m4ac.sample_rate << multiplier;
        avctx->frame_size = samples;
        ac->oc[1].status = OC_LOCKED;
    }

    if (multiplier)
        avctx->internal->skip_samples_multiplier = 2;

    if (!ac->frame->data[0] && samples) {
        av_log(avctx, 16, "no frame data found\n");
        err = (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));
        goto fail;
    }

    if (samples) {
        ac->frame->nb_samples = samples;
        ac->frame->sample_rate = avctx->sample_rate;
    } else
        av_frame_unref(ac->frame);
    *got_frame_ptr = !!samples;


    is_dmono = ac->dmono_mode && sce_count == 2 &&
               ac->oc[1].channel_layout == (0x00000001 | 0x00000002);
    if (is_dmono) {
        if (ac->dmono_mode == 1)
            ((AVFrame *)data)->data[1] =((AVFrame *)data)->data[0];
        else if (ac->dmono_mode == 2)
            ((AVFrame *)data)->data[0] =((AVFrame *)data)->data[1];
    }

    return 0;
fail:
    pop_output_configuration(ac);
    return err;
}

static int aac_decode_frame(AVCodecContext *avctx, void *data,
                            int *got_frame_ptr, AVPacket *avpkt)
{
    AACContext *ac = avctx->priv_data;
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    GetBitContext gb;
    int buf_consumed;
    int buf_offset;
    int err;
    int new_extradata_size;
    const uint8_t *new_extradata = av_packet_get_side_data(avpkt,
                                       AV_PKT_DATA_NEW_EXTRADATA,
                                       &new_extradata_size);
    int jp_dualmono_size;
    const uint8_t *jp_dualmono = av_packet_get_side_data(avpkt,
                                       AV_PKT_DATA_JP_DUALMONO,
                                       &jp_dualmono_size);

    if (new_extradata) {

        ac->oc[1].status = OC_NONE;
        err = decode_audio_specific_config(ac, ac->avctx, &ac->oc[1].m4ac,
                                           new_extradata,
                                           new_extradata_size * 8LL, 1);
        if (err < 0) {
            return err;
        }
    }

    ac->dmono_mode = 0;
    if (jp_dualmono && jp_dualmono_size > 0)
        ac->dmono_mode = 1 + *jp_dualmono;
    if (ac->force_dmono_mode >= 0)
        ac->dmono_mode = ac->force_dmono_mode;

    if (0x7fffffff 
               / 8 <= buf_size)
        return (-(int)(('I') | (('N') << 8) | (('D') << 16) | ((unsigned)('A') << 24)));

    if ((err = init_get_bits8(&gb, buf, buf_size)) < 0)
        return err;

    switch (ac->oc[1].m4ac.object_type) {
    case AOT_ER_AAC_LC:
    case AOT_ER_AAC_LTP:
    case AOT_ER_AAC_LD:
    case AOT_ER_AAC_ELD:
        err = aac_decode_er_frame(avctx, data, got_frame_ptr, &gb);
        break;
    default:
        err = aac_decode_frame_int(avctx, data, got_frame_ptr, &gb, avpkt);
    }
    if (err < 0)
        return err;

    buf_consumed = (get_bits_count(&gb) + 7) >> 3;
    for (buf_offset = buf_consumed; buf_offset < buf_size; buf_offset++)
        if (buf[buf_offset])
            break;

    return buf_size > buf_offset ? buf_consumed : buf_size;
}

static __attribute__((cold)) int aac_decode_close(AVCodecContext *avctx)
{
    AACContext *ac = avctx->priv_data;
    int i, type;

    for (i = 0; i < 16; i++) {
        for (type = 0; type < 4; type++) {
            if (ac->che[type][i])
                ff_aac_sbr_ctx_close(&ac->che[type][i]->sbr);
            av_freep(&ac->che[type][i]);
        }
    }

    ff_mdct_end(&ac->mdct);
    ff_mdct_end(&ac->mdct_small);
    ff_mdct_end(&ac->mdct_ld);
    ff_mdct_end(&ac->mdct_ltp);

    ff_mdct15_uninit(&ac->mdct120);
    ff_mdct15_uninit(&ac->mdct480);
    ff_mdct15_uninit(&ac->mdct960);

    av_freep(&ac->fdsp);
    return 0;
}

static void aacdec_init(AACContext *c)
{
    c->imdct_and_windowing = imdct_and_windowing;
    c->apply_ltp = apply_ltp;
    c->apply_tns = apply_tns;
    c->windowing_and_mdct_ltp = windowing_and_mdct_ltp;
    c->update_ltp = update_ltp;






    if(0)
        ff_aacdec_init_mips(c);

}




static const AVOption options[] = {
    {"dual_mono_mode", "Select the channel to decode for dual mono",
     
    __builtin_offsetof (
    AACContext
    , 
    force_dmono_mode
    )
                                          , AV_OPT_TYPE_INT, {.i64=-1}, -1, 2,
     2 | 8, "dual_mono_mode"},

    {"auto", "autoselection", 0, AV_OPT_TYPE_CONST, {.i64=-1}, 
                                                                         (-0x7fffffff - 1)
                                                                                , 0x7fffffff
                                                                                         , 2 | 8, "dual_mono_mode"},
    {"main", "Select Main/Left channel", 0, AV_OPT_TYPE_CONST, {.i64= 1}, 
                                                                         (-0x7fffffff - 1)
                                                                                , 0x7fffffff
                                                                                         , 2 | 8, "dual_mono_mode"},
    {"sub" , "Select Sub/Right channel", 0, AV_OPT_TYPE_CONST, {.i64= 2}, 
                                                                         (-0x7fffffff - 1)
                                                                                , 0x7fffffff
                                                                                         , 2 | 8, "dual_mono_mode"},
    {"both", "Select both channels", 0, AV_OPT_TYPE_CONST, {.i64= 0}, 
                                                                         (-0x7fffffff - 1)
                                                                                , 0x7fffffff
                                                                                         , 2 | 8, "dual_mono_mode"},

    {
    ((void *)0)
        },
};

static const AVClass aac_decoder_class = {
    .class_name = "AAC decoder",
    .item_name = av_default_item_name,
    .option = options,
    .version = ((56)<<16 | (70)<<8 | (100)),
};




/* ---- Architecture-specific dispatch stubs (no-ops on all platforms) ---- */
void ff_float_dsp_init_aarch64(AVFloatDSPContext* s){(void)s;}
void ff_float_dsp_init_arm(AVFloatDSPContext* s){(void)s;}
void ff_float_dsp_init_ppc(AVFloatDSPContext* s,int b){(void)s;(void)b;}
void ff_float_dsp_init_x86(AVFloatDSPContext* s){(void)s;}
void ff_float_dsp_init_mips(AVFloatDSPContext* s){(void)s;}
void ff_fft_init_aarch64(FFTContext* s){(void)s;}
void ff_fft_init_arm(FFTContext* s){(void)s;}
void ff_fft_init_ppc(FFTContext* s){(void)s;}
void ff_fft_init_x86(FFTContext* s){(void)s;}
void ff_fft_init_mips(FFTContext* s){(void)s;}
void ff_mdct15_init_x86(MDCT15Context* s){(void)s;}
void ff_rdft_init_arm(RDFTContext* s){(void)s;}
void ff_psdsp_init_arm(PSDSPContext* s){(void)s;}
void ff_psdsp_init_aarch64(PSDSPContext* s){(void)s;}
void ff_psdsp_init_mips(PSDSPContext* s){(void)s;}
void ff_psdsp_init_x86(PSDSPContext* s){(void)s;}
void ff_sbrdsp_init_arm(SBRDSPContext* s){(void)s;}
void ff_sbrdsp_init_aarch64(SBRDSPContext* s){(void)s;}
void ff_sbrdsp_init_x86(SBRDSPContext* s){(void)s;}
void ff_sbrdsp_init_mips(SBRDSPContext* s){(void)s;}
void ff_aacdec_init_mips(AACContext* s){(void)s;}
void ff_aacsbr_func_ptr_init_mips(AACSBRContext* s){(void)s;}
/* ---- FFmpeg utility stubs ---- */
uint8_t* av_packet_get_side_data(const AVPacket* p,enum AVPacketSideDataType t,int* sz){(void)p;(void)t;if(sz)*sz=0;return NULL;}
static void* ff_aac__mallocz(size_t sz){void* p=malloc(sz);if(p)memset(p,0,sz);return p;}
const char* av_default_item_name(void* p){(void)p;return "ff_aac";}
void* av_mallocz(size_t sz){return ff_aac__mallocz(sz);}
void* av_malloc(size_t sz){return malloc(sz);}
void* av_malloc_array(size_t n,size_t s){if(s&&n>(size_t)-1/s)return NULL;return ff_aac__mallocz(n*s);}
void  av_free(void* p){free(p);}
void  av_freep(void* pp){void**p=(void**)pp;free(*p);*p=NULL;}
void* av_realloc_f(void* p,size_t n,size_t ss){if(ss&&n>(size_t)-1/ss){free(p);return NULL;}return realloc(p,n*ss);}
static int ff_aac__log2i(unsigned v){int n=0;if(v>>16){n+=16;v>>=16;}if(v>>8){n+=8;v>>=8;}if(v>>4){n+=4;v>>=4;}if(v>>2){n+=2;v>>=2;}n+=v>>1;return n;}
int av_log2(unsigned v){return ff_aac__log2i(v);}
void av_log(void* a,int l,const char* f,...){(void)a;(void)l;(void)f;}
void ff_dlog(void* a,const char* f,...){(void)a;(void)f;}
void avpriv_report_missing_feature(void* a,const char* f,...){(void)a;(void)f;}
void avpriv_request_sample(void* a,const char* f,...){(void)a;(void)f;}
const uint8_t ff_reverse[256]={
0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF};
int ff_get_buffer(AVCodecContext* avctx,AVFrame* frame,int flags){
    int ch;(void)flags;
    for(ch=0;ch<avctx->channels&&ch<AV_NUM_DATA_POINTERS;ch++)
        if(!frame->data[ch]){frame->data[ch]=(uint8_t*)ff_aac__mallocz((size_t)frame->nb_samples*sizeof(float));if(!frame->data[ch])return AVERROR(ENOMEM);}
    frame->extended_data=frame->data;return 0;}
void av_frame_unref(AVFrame* frame){
    int ch;if(!frame)return;
    for(ch=0;ch<AV_NUM_DATA_POINTERS;ch++){free(frame->data[ch]);frame->data[ch]=NULL;}
    frame->extended_data=NULL;frame->nb_samples=0;}
const AVProfile ff_aac_profiles[]={
    {FF_PROFILE_AAC_MAIN,"Main"},{FF_PROFILE_AAC_LOW,"LC"},
    {FF_PROFILE_AAC_SSR,"SSR"},{FF_PROFILE_AAC_LTP,"LTP"},
    {FF_PROFILE_AAC_HE,"HE-AAC"},{FF_PROFILE_UNKNOWN,NULL}};

/* ======================================================================== */
/*  Minimal M4A/MP4 demuxer - wraps raw AAC-LC frames in synthetic ADTS     */
/* ======================================================================== */
typedef struct {
    uint32_t  n_samples;
    uint32_t* sample_sizes;   /* stsz */
    uint32_t  n_chunks;
    uint32_t* chunk_offsets;  /* stco */
    uint32_t  n_stsc;
    uint32_t* stsc_first;     /* stsc: first_chunk */
    uint32_t* stsc_spc;       /* stsc: samples_per_chunk */
    uint32_t  sample_idx;
    uint32_t  chunk_idx;
    uint32_t  chunk_sample;
    uint32_t  chunk_spc;
    uint8_t   sr_idx;
    uint8_t   channels;
    int       is_m4a;
} ff_aac__mp4;

static uint32_t ff_aac__u32be(const uint8_t* p){
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];}
static uint16_t ff_aac__u16be(const uint8_t* p){return ((uint16_t)p[0]<<8)|p[1];}

/* Read MPEG-4 expandable length */
static uint32_t ff_aac__desc_len(const uint8_t* d,size_t sz,size_t* off){
    uint32_t len=0; int i;
    for(i=0;i<4&&*off<sz;i++){uint8_t b=d[(*off)++];len=(len<<7)|(b&0x7F);if(!(b&0x80))break;}
    return len;}

/* Find a box by 4-char name within [start,end); return box-body offset or 0 */
static size_t ff_aac__find_box(const uint8_t* d,size_t start,size_t end,const char* name){
    size_t o=start;
    while(o+8<=end){
        uint32_t sz=ff_aac__u32be(d+o);
        if(sz<8||o+sz>end){sz=(uint32_t)(end-o);}
        if(d[o+4]==name[0]&&d[o+5]==name[1]&&d[o+6]==name[2]&&d[o+7]==name[3])
            return o+8;
        if(sz==0)break;
        o+=sz;}
    return 0;}

/* Recursively find box through a path like "moov/trak/mdia/minf/stbl/stsd" */
static size_t ff_aac__box_path(const uint8_t* d,size_t dlen,const char** path,int depth){
    size_t start=0,end=dlen;
    for(int i=0;i<depth;i++){
        size_t body=ff_aac__find_box(d,start,end,path[i]);
        if(!body)return 0;
        /* find the end of this box */
        uint32_t sz=ff_aac__u32be(d+body-8);
        if(sz<8)return 0;
        start=body; end=body-8+sz;}
    return start;}

static void ff_aac__mp4_free(ff_aac__mp4* m){
    if(!m)return;
    free(m->sample_sizes); free(m->chunk_offsets);
    free(m->stsc_first); free(m->stsc_spc); free(m);}

/* Returns allocated ff_aac__mp4* on success, NULL if not M4A */
static ff_aac__mp4* ff_aac__mp4_open(const uint8_t* d, size_t dlen){
    /* Quick check: must have ftyp or moov */
    if(dlen<8)return NULL;
    /* Check for moov box anywhere in first 64KB or at end */
    const char* moov_path[]={"moov"};
    size_t moov=ff_aac__box_path(d,dlen,moov_path,1);
    if(!moov)return NULL;

    /* locate moov end */
    size_t moov_start=moov-8;
    uint32_t moov_size=ff_aac__u32be(d+moov_start);
    size_t moov_end=moov_start+moov_size;

    /* trak -> mdia -> minf -> stbl */
    const char* trak_p[]={"trak"}; size_t trak=ff_aac__find_box(d,moov,moov_end,"trak"); if(!trak)return NULL;
    uint32_t trak_sz=ff_aac__u32be(d+trak-8); size_t trak_end=trak-8+trak_sz;
    size_t mdia=ff_aac__find_box(d,trak,trak_end,"mdia"); if(!mdia)return NULL;
    uint32_t mdia_sz=ff_aac__u32be(d+mdia-8); size_t mdia_end=mdia-8+mdia_sz;
    size_t minf=ff_aac__find_box(d,mdia,mdia_end,"minf"); if(!minf)return NULL;
    uint32_t minf_sz=ff_aac__u32be(d+minf-8); size_t minf_end=minf-8+minf_sz;
    size_t stbl=ff_aac__find_box(d,minf,minf_end,"stbl"); if(!stbl)return NULL;
    uint32_t stbl_sz=ff_aac__u32be(d+stbl-8); size_t stbl_end=stbl-8+stbl_sz;

    /* stsd -> mp4a -> esds: get sr_idx + channels from AudioSpecificConfig */
    size_t stsd=ff_aac__find_box(d,stbl,stbl_end,"stsd"); if(!stsd)return NULL;
    /* stsd: 4 ver/flags + 4 entry_count, then AudioSampleEntry */
    size_t ae=stsd+8;  /* mp4a box start */
    if(ae+36>dlen)return NULL;
    uint8_t sr_idx=4, channels=2;
    /* AudioSampleEntry fixed fields: +24 channelcount, +32 samplerate(16.16) */
    channels=(uint8_t)ff_aac__u16be(d+ae+24);
    uint32_t sr_fp=ff_aac__u32be(d+ae+32);
    uint32_t sr=sr_fp>>16;
    /* map sr to ADTS sr_idx */
    static const uint32_t sr_table[]={96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350};
    sr_idx=4;
    for(int i=0;i<13;i++){if(sr_table[i]==sr){sr_idx=(uint8_t)i;break;}}
    /* Try esds for AudioSpecificConfig (more reliable) */
    uint32_t ae_sz=ff_aac__u32be(d+ae); size_t ae_end=ae+ae_sz;
    size_t esds_body=ff_aac__find_box(d,ae+36,ae_end,"esds");
    if(esds_body&&esds_body+4<=dlen){
        /* skip 4 ver/flags, parse ES_Descriptor */
        size_t o=esds_body+4;
        if(o<dlen&&d[o]==0x03){o++; ff_aac__desc_len(d,dlen,&o); o+=3;} /* ES_ID+flags */
        if(o<dlen&&d[o]==0x04){o++; ff_aac__desc_len(d,dlen,&o); o+=13;} /* DecoderConfig */
        if(o<dlen&&d[o]==0x05){o++; uint32_t l=ff_aac__desc_len(d,dlen,&o);
            if(l>=2&&o+2<=dlen){
                uint8_t b0=d[o],b1=d[o+1];
                sr_idx=(uint8_t)(((b0&7)<<1)|(b1>>7));
                channels=(uint8_t)((b1>>3)&0xF);}}
    }

    /* stsz */
    size_t stsz=ff_aac__find_box(d,stbl,stbl_end,"stsz"); if(!stsz||stsz+12>dlen)return NULL;
    uint32_t n_stsz=ff_aac__u32be(d+stsz+8);  /* +0=ver/flags +4=uniform_size +8=count */
    if(n_stsz==0||stsz+12+(uint64_t)n_stsz*4>dlen)return NULL;
    uint32_t* szarr=(uint32_t*)malloc(n_stsz*sizeof(uint32_t));
    if(!szarr)return NULL;
    for(uint32_t i=0;i<n_stsz;i++) szarr[i]=ff_aac__u32be(d+stsz+12+i*4);

    /* stco */
    size_t stco=ff_aac__find_box(d,stbl,stbl_end,"stco"); if(!stco||stco+8>dlen){free(szarr);return NULL;}
    uint32_t n_co=ff_aac__u32be(d+stco+4);  /* +0=ver/flags +4=count */
    if(n_co==0||stco+8+(uint64_t)n_co*4>dlen){free(szarr);return NULL;}
    uint32_t* coarr=(uint32_t*)malloc(n_co*sizeof(uint32_t));
    if(!coarr){free(szarr);return NULL;}
    for(uint32_t i=0;i<n_co;i++) coarr[i]=ff_aac__u32be(d+stco+8+i*4);

    /* stsc */
    size_t stsc=ff_aac__find_box(d,stbl,stbl_end,"stsc"); if(!stsc||stsc+8>dlen){free(szarr);free(coarr);return NULL;}
    uint32_t n_stsc=ff_aac__u32be(d+stsc+4);  /* +0=ver/flags +4=count */
    if(n_stsc==0||stsc+8+(uint64_t)n_stsc*12>dlen){free(szarr);free(coarr);return NULL;}
    uint32_t* stsc_first=(uint32_t*)malloc(n_stsc*sizeof(uint32_t));
    uint32_t* stsc_spc  =(uint32_t*)malloc(n_stsc*sizeof(uint32_t));
    if(!stsc_first||!stsc_spc){free(szarr);free(coarr);free(stsc_first);free(stsc_spc);return NULL;}
    for(uint32_t i=0;i<n_stsc;i++){
        stsc_first[i]=ff_aac__u32be(d+stsc+8+i*12);
        stsc_spc  [i]=ff_aac__u32be(d+stsc+12+i*12);}

    ff_aac__mp4* m=(ff_aac__mp4*)calloc(1,sizeof(*m));
    if(!m){free(szarr);free(coarr);free(stsc_first);free(stsc_spc);return NULL;}
    m->n_samples=n_stsz; m->sample_sizes=szarr;
    m->n_chunks=n_co;    m->chunk_offsets=coarr;
    m->n_stsc=n_stsc;    m->stsc_first=stsc_first; m->stsc_spc=stsc_spc;
    m->sr_idx=sr_idx;    m->channels=channels;     m->is_m4a=1;
    /* init chunk iterator */
    m->chunk_idx=0; m->chunk_sample=0;
    m->chunk_spc=(n_stsc>0)?stsc_spc[0]:1;
    return m;}

/* Make a 7-byte ADTS header for a raw frame of `frame_len` bytes */
static void ff_aac__make_adts(uint8_t* hdr,uint32_t frame_len,uint8_t sr_idx,uint8_t ch){
    uint32_t total=frame_len+7;
    hdr[0]=0xFF; hdr[1]=0xF1;
    hdr[2]=(uint8_t)(((1/*LC*/)<<6)|((sr_idx&0xF)<<2)|((ch>>2)&1));
    hdr[3]=(uint8_t)(((ch&3)<<6)|((total>>11)&3));
    hdr[4]=(uint8_t)((total>>3)&0xFF);
    hdr[5]=(uint8_t)(((total&7)<<5)|0x1F);
    hdr[6]=0xFC;}

/* Get next raw AAC frame from M4A, synthesize ADTS and return malloc'd buffer.
   Returns frame total length (header+data) or 0 at end. */
static uint32_t ff_aac__mp4_next(ff_aac__mp4* m, const uint8_t* filedata,
                                  size_t filesize, uint8_t** out){
    if(m->sample_idx>=m->n_samples)return 0;
    uint32_t sidx=m->sample_idx;
    uint32_t raw_sz=m->sample_sizes[sidx];

    /* Advance chunk iterator */
    if(sidx==0){
        m->chunk_idx=0; m->chunk_sample=0;
        m->chunk_spc=(m->n_stsc>0)?m->stsc_spc[0]:1;}
    uint32_t file_off=m->chunk_offsets[m->chunk_idx];
    /* compute offset within chunk */
    uint32_t off_in_chunk=0;
    for(uint32_t k=0;k<m->chunk_sample;k++){
        uint32_t prev=sidx-m->chunk_sample+k;
        off_in_chunk+=m->sample_sizes[prev];}
    file_off+=off_in_chunk;

    if((uint64_t)file_off+raw_sz>filesize)return 0;

    uint8_t* buf=(uint8_t*)malloc(raw_sz+7);
    if(!buf)return 0;
    ff_aac__make_adts(buf,raw_sz,m->sr_idx,m->channels);
    memcpy(buf+7,filedata+file_off,raw_sz);
    *out=buf;

    /* advance */
    m->sample_idx++; m->chunk_sample++;
    if(m->chunk_sample>=m->chunk_spc){
        m->chunk_idx++; m->chunk_sample=0;
        if(m->chunk_idx<m->n_chunks){
            /* look up new spc from stsc */
            uint32_t new_chunk=m->chunk_idx+1;
            uint32_t spc=m->stsc_spc[m->n_stsc-1];
            for(uint32_t i=0;i+1<m->n_stsc;i++)
                if(m->stsc_first[i]<=new_chunk&&new_chunk<m->stsc_first[i+1]){spc=m->stsc_spc[i];break;}
            m->chunk_spc=spc;}}
    return raw_sz+7;}

struct ff_aac__ctx {
    AVCodecContext  avctx;
    AACContext      ac;
    AVFrame         frame;
    const uint8_t*  mem_data; size_t mem_size; size_t mem_pos;
    int             is_file; FILE* fp;
    float*          pcm; size_t pcm_cap; size_t pcm_r; size_t pcm_w;
    ff_aac_allocation_callbacks alloc;
    ff_aac__mp4* mp4;
};
static void* ff_aac__xm(size_t sz,const ff_aac_allocation_callbacks* a){return (a&&a->onMalloc)?a->onMalloc(sz,a->pUserData):malloc(sz);}
static void* ff_aac__xr(void* p,size_t sz,const ff_aac_allocation_callbacks* a){return (a&&a->onRealloc)?a->onRealloc(p,sz,a->pUserData):realloc(p,sz);}
static void  ff_aac__xf(void* p,const ff_aac_allocation_callbacks* a){if(!p)return;if(a&&a->onFree)a->onFree(p,a->pUserData);else free(p);}
static int ff_aac__adts_len(const uint8_t* b,size_t n){
    if(n<7||b[0]!=0xFF||(b[1]&0xF6)!=0xF0)return 0;
    return ((b[3]&3)<<11)|(b[4]<<3)|(b[5]>>5);}
static int ff_aac__decode_one(ff_aac__ctx* ctx){
    AVPacket pkt;int flen,got,ret;uint8_t* fbuf;
    memset(&pkt,0,sizeof(pkt));
    if(ctx->mp4){
        /* M4A: get next raw frame, already wrapped in ADTS by mp4_next */
        uint8_t* raw=NULL;
        uint32_t sz=ff_aac__mp4_next(ctx->mp4,ctx->mem_data,ctx->mem_size,&raw);
        if(!sz||!raw)return 0;
        fbuf=(uint8_t*)calloc(sz+64,1);
        if(!fbuf){free(raw);return 0;}
        memcpy(fbuf,raw,sz); free(raw);
        flen=(int)sz;
    } else if(ctx->is_file){
        uint8_t hdr[7];
        if(fread(hdr,1,7,ctx->fp)<7)return 0;
        flen=ff_aac__adts_len(hdr,7);if(flen<=7)return 0;
        fbuf=(uint8_t*)calloc((size_t)flen+64,1);if(!fbuf)return 0;
        memcpy(fbuf,hdr,7);
        if((int)fread(fbuf+7,1,(size_t)(flen-7),ctx->fp)<flen-7){free(fbuf);return 0;}
    } else {
        while(ctx->mem_pos+1<ctx->mem_size){
            if(ctx->mem_data[ctx->mem_pos]==0xFF&&(ctx->mem_data[ctx->mem_pos+1]&0xF6)==0xF0)break;
            ctx->mem_pos++;}
        if(ctx->mem_pos+7>ctx->mem_size)return 0;
        flen=ff_aac__adts_len(ctx->mem_data+ctx->mem_pos,ctx->mem_size-ctx->mem_pos);
        if(flen<=0||(size_t)flen>ctx->mem_size-ctx->mem_pos)return 0;
        fbuf=(uint8_t*)calloc((size_t)flen+64,1);if(!fbuf)return 0;
        memcpy(fbuf,ctx->mem_data+ctx->mem_pos,(size_t)flen);
        ctx->mem_pos+=(size_t)flen;}
    pkt.data=fbuf;pkt.size=flen;ctx->frame.nb_samples=2048;got=0;
    ret=aac_decode_frame(&ctx->avctx,&ctx->frame,&got,&pkt);
    free(fbuf);if(ret<0||!got)return 1;
    {int nch=ctx->avctx.channels>0?ctx->avctx.channels:1,ns=ctx->frame.nb_samples,s,ch;
     size_t need=(size_t)(ns*nch),used=ctx->pcm_w-ctx->pcm_r;
     if(ctx->pcm_w+need>ctx->pcm_cap){
         if(ctx->pcm_r){memmove(ctx->pcm,ctx->pcm+ctx->pcm_r,used*sizeof(float));ctx->pcm_w=used;ctx->pcm_r=0;}
         if(ctx->pcm_w+need>ctx->pcm_cap){
             size_t nc=(ctx->pcm_w+need)*2;float* nb=(float*)realloc(ctx->pcm,nc*sizeof(float));
             if(!nb)return 0;ctx->pcm=nb;ctx->pcm_cap=nc;}}
     for(s=0;s<ns;s++)for(ch=0;ch<nch;ch++){float* b=(float*)ctx->frame.data[ch];ctx->pcm[ctx->pcm_w++]=b?b[s]:0.0f;}}
    return 1;}
static void ff_aac__ensure(ff_aac__ctx* ctx,size_t need){
    while((ctx->pcm_w-ctx->pcm_r)<need)if(!ff_aac__decode_one(ctx))break;}
static ff_aac_result ff_aac__open(ff_aac__ctx* ctx){
    int i,ret;
    /* Detect M4A: memory path only (file path handled below) */
    if(!ctx->is_file&&ctx->mem_data&&ctx->mem_size>=8){
        ctx->mp4=ff_aac__mp4_open(ctx->mem_data,ctx->mem_size);
        if(ctx->mp4){
            /* Pre-fill avctx channels/sample_rate from esds so API struct is valid */
            static const uint32_t sr_tab[]={96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350};
            ctx->avctx.channels=(int)ctx->mp4->channels;
            ctx->avctx.sample_rate=(int)(ctx->mp4->sr_idx<13?sr_tab[ctx->mp4->sr_idx]:44100);}}
    memset(&ctx->avctx.priv_data,0,sizeof(void*)); /* keep channels/sr */
    {AVCodecContext tmp; memcpy(&tmp,&ctx->avctx,sizeof(tmp));
     memset(&ctx->avctx,0,sizeof(ctx->avctx));
     ctx->avctx.channels=tmp.channels; ctx->avctx.sample_rate=tmp.sample_rate;}
    memset(&ctx->ac,0,sizeof(ctx->ac)); memset(&ctx->frame,0,sizeof(ctx->frame));
    ctx->avctx.priv_data=&ctx->ac;ctx->avctx.internal=(struct AVCodecInternal*)calloc(1,sizeof(AVCodecInternal));ctx->ac.avctx=&ctx->avctx;ctx->ac.frame=&ctx->frame;
    for(i=0;i<AV_NUM_DATA_POINTERS;i++)ctx->frame.data[i]=NULL;
    ctx->frame.extended_data=ctx->frame.data;
    ret=aac_decode_init(&ctx->avctx);if(ret<0)return FF_AAC_ERROR;
    ctx->pcm_cap=1024*16;ctx->pcm=(float*)calloc(ctx->pcm_cap,sizeof(float));
    if(!ctx->pcm)return FF_AAC_ERROR_OUT_OF_MEMORY;
    ff_aac__decode_one(ctx);return FF_AAC_SUCCESS;}
ff_aac_result ff_aac_init_memory(const void* data,size_t sz,const ff_aac_config* pCfg,ff_aac* pAAC){
    ff_aac__ctx* ctx;ff_aac_result r;
    if(!pAAC||!data)return FF_AAC_ERROR_INVALID_ARGS;
    memset(pAAC,0,sizeof(*pAAC));
    ctx=(ff_aac__ctx*)calloc(1,sizeof(*ctx));if(!ctx)return FF_AAC_ERROR_OUT_OF_MEMORY;
    (void)pCfg;
    ctx->mem_data=(const uint8_t*)data;ctx->mem_size=sz;ctx->is_file=0;pAAC->pInternal=ctx;
    r=ff_aac__open(ctx);
    if(r!=FF_AAC_SUCCESS){free(ctx);pAAC->pInternal=NULL;return r;}
    pAAC->channels=(uint32_t)(ctx->avctx.channels>0?ctx->avctx.channels:1);
    pAAC->sampleRate=(uint32_t)ctx->avctx.sample_rate;return FF_AAC_SUCCESS;}
#ifndef FF_AAC_NO_STDIO
ff_aac_result ff_aac_init_file(const char* path,const ff_aac_config* pCfg,ff_aac* pAAC){
    if(!pAAC||!path)return FF_AAC_ERROR_INVALID_ARGS;
    (void)pCfg;
    /* Slurp entire file into memory so M4A box tree can be scanned */
    FILE* fp_=fopen(path,"rb");if(!fp_)return FF_AAC_ERROR_OPEN_FAILED;
    if(fseek(fp_,0,SEEK_END)!=0){fclose(fp_);return FF_AAC_ERROR_OPEN_FAILED;}
    long fsz_=(long)ftell(fp_);rewind(fp_);
    if(fsz_<=0){fclose(fp_);return FF_AAC_ERROR_INVALID_FILE;}
    uint8_t* fbuf_=(uint8_t*)malloc((size_t)fsz_);
    if(!fbuf_){fclose(fp_);return FF_AAC_ERROR_OUT_OF_MEMORY;}
    if((long)fread(fbuf_,1,(size_t)fsz_,fp_)<fsz_){fclose(fp_);free(fbuf_);return FF_AAC_ERROR_OPEN_FAILED;}
    fclose(fp_);
    ff_aac_result r_=ff_aac_init_memory(fbuf_,(size_t)fsz_,pCfg,pAAC);
    if(r_!=FF_AAC_SUCCESS){free(fbuf_);return r_;}
    /* ctx now owns fbuf_ — mark so uninit frees it */
    ((ff_aac__ctx*)pAAC->pInternal)->is_file=1;
    return FF_AAC_SUCCESS;}
#endif
void ff_aac_uninit(ff_aac* pAAC){
    ff_aac__ctx* ctx;int ch;
    if(!pAAC||!pAAC->pInternal)return;ctx=pAAC->pInternal;
    aac_decode_close(&ctx->avctx);
    free(ctx->avctx.internal);
    if(ctx->mp4)ff_aac__mp4_free(ctx->mp4);
    for(ch=0;ch<AV_NUM_DATA_POINTERS;ch++)free(ctx->frame.data[ch]);
    if(ctx->is_file&&ctx->mem_data)free((void*)ctx->mem_data);
    free(ctx->pcm);free(ctx);memset(pAAC,0,sizeof(*pAAC));}
uint64_t ff_aac_read_pcm_frames_f32(ff_aac* pAAC,uint64_t nf,float* pOut){
    ff_aac__ctx* ctx;uint64_t done=0;uint32_t nch;
    if(!pAAC||!pAAC->pInternal||!pOut)return 0;
    ctx=pAAC->pInternal;nch=pAAC->channels?pAAC->channels:1;
    while(done<nf){
        size_t need=(size_t)((nf-done)*nch),batch=need<(size_t)(1024*nch)?(size_t)(1024*nch):need;
        ff_aac__ensure(ctx,batch);
        size_t avail=ctx->pcm_w-ctx->pcm_r;if(!avail)break;
        size_t take=avail<need?avail:need;
        memcpy(pOut+done*nch,ctx->pcm+ctx->pcm_r,take*sizeof(float));
        ctx->pcm_r+=take;done+=take/nch;}
    return done;}
#if defined(__SSE2__)||defined(_M_X64)||(defined(_M_IX86_FP)&&_M_IX86_FP>=2)
#include <emmintrin.h>
static void ff_aac__f32s16(const float* s,int16_t* d,size_t n){
    size_t i=0;
    for(;i+8<=n;i+=8){__m128 a=_mm_mul_ps(_mm_loadu_ps(s+i),_mm_set1_ps(32767.f)),b=_mm_mul_ps(_mm_loadu_ps(s+i+4),_mm_set1_ps(32767.f));
        _mm_storeu_si128((__m128i*)(d+i),_mm_packs_epi32(_mm_cvtps_epi32(a),_mm_cvtps_epi32(b)));}
    for(;i<n;i++){float v=s[i]*32767.f;d[i]=(int16_t)(v>32767.f?32767.f:v<-32768.f?-32768.f:v);}}
#else
static void ff_aac__f32s16(const float* s,int16_t* d,size_t n){
    size_t i;for(i=0;i<n;i++){float v=s[i]*32767.f;d[i]=(int16_t)(v>32767.f?32767.f:v<-32768.f?-32768.f:v);}}
#endif
uint64_t ff_aac_read_pcm_frames_s16(ff_aac* pAAC,uint64_t nf,int16_t* pOut){
    float tmp[4096];uint64_t done=0;uint32_t nch=pAAC->channels?pAAC->channels:1;
    while(done<nf){uint64_t b=nf-done;if(b>4096/nch)b=4096/nch;
        uint64_t got=ff_aac_read_pcm_frames_f32(pAAC,b,tmp);if(!got)break;
        ff_aac__f32s16(tmp,pOut+done*nch,(size_t)(got*nch));done+=got;}
    return done;}
ff_aac_result ff_aac_seek_to_pcm_frame(ff_aac* pAAC,uint64_t idx){(void)pAAC;(void)idx;return FF_AAC_ERROR;}
void ff_aac_free(void* p,const ff_aac_allocation_callbacks* a){if(a&&a->onFree)a->onFree(p,a->pUserData);else free(p);}
static float* ff_aac__read_all(ff_aac* a,uint64_t* pT,const ff_aac_allocation_callbacks* pa){
    size_t cap=4096,used=0;uint32_t nch=a->channels;
    float* buf=(float*)ff_aac__xm(cap*nch*sizeof(float),pa);if(!buf)return NULL;
    for(;;){uint64_t got;
        if(used+1024>cap){cap*=2;float* nb=(float*)ff_aac__xr(buf,cap*nch*sizeof(float),pa);if(!nb){ff_aac__xf(buf,pa);return NULL;}buf=nb;}
        got=ff_aac_read_pcm_frames_f32(a,1024,buf+used*nch);if(!got)break;used+=(size_t)got;}
    if(pT)*pT=(uint64_t)used;return buf;}
float* ff_aac_open_memory_and_read_pcm_frames_f32(const void* d,size_t sz,uint32_t* pCh,uint32_t* pSR,uint64_t* pT,const ff_aac_allocation_callbacks* pa){
    ff_aac a;float* out;if(ff_aac_init_memory(d,sz,NULL,&a)!=FF_AAC_SUCCESS)return NULL;
    if(pCh)*pCh=a.channels;if(pSR)*pSR=a.sampleRate;out=ff_aac__read_all(&a,pT,pa);ff_aac_uninit(&a);return out;}
int16_t* ff_aac_open_memory_and_read_pcm_frames_s16(const void* d,size_t sz,uint32_t* pCh,uint32_t* pSR,uint64_t* pT,const ff_aac_allocation_callbacks* pa){
    ff_aac a;float* f;int16_t* s;uint64_t n;uint32_t nch;
    if(ff_aac_init_memory(d,sz,NULL,&a)!=FF_AAC_SUCCESS)return NULL;
    if(pCh)*pCh=a.channels;nch=a.channels;if(pSR)*pSR=a.sampleRate;
    f=ff_aac__read_all(&a,&n,pa);ff_aac_uninit(&a);if(!f)return NULL;
    s=(int16_t*)ff_aac__xm(n*nch*sizeof(int16_t),pa);
    if(s){ff_aac__f32s16(f,s,n*nch);if(pT)*pT=n;}ff_aac__xf(f,pa);return s;}
#ifndef FF_AAC_NO_STDIO
float* ff_aac_open_file_and_read_pcm_frames_f32(const char* path,uint32_t* pCh,uint32_t* pSR,uint64_t* pT,const ff_aac_allocation_callbacks* pa){
    ff_aac a;float* out;if(ff_aac_init_file(path,NULL,&a)!=FF_AAC_SUCCESS)return NULL;
    if(pCh)*pCh=a.channels;if(pSR)*pSR=a.sampleRate;out=ff_aac__read_all(&a,pT,pa);ff_aac_uninit(&a);return out;}
int16_t* ff_aac_open_file_and_read_pcm_frames_s16(const char* path,uint32_t* pCh,uint32_t* pSR,uint64_t* pT,const ff_aac_allocation_callbacks* pa){
    ff_aac a;float* f;int16_t* s;uint64_t n;uint32_t nch;
    if(ff_aac_init_file(path,NULL,&a)!=FF_AAC_SUCCESS)return NULL;
    if(pCh)*pCh=a.channels;nch=a.channels;if(pSR)*pSR=a.sampleRate;
    f=ff_aac__read_all(&a,&n,pa);ff_aac_uninit(&a);if(!f)return NULL;
    s=(int16_t*)ff_aac__xm(n*nch*sizeof(int16_t),pa);
    if(s){ff_aac__f32s16(f,s,n*nch);if(pT)*pT=n;}ff_aac__xf(f,pa);return s;}
#endif
#if defined(__GNUC__)||defined(__clang__)
#pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif /* FF_AAC_IMPLEMENTATION */
