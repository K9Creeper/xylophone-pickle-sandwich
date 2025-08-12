/* freestanding_math.h
 *
 * A small freestanding math.h replacement (C99-compatible).
 * Uses compiler builtins (__builtin_*) when available.
 *
 * Copyright: Public domain (use as you like)
 */

#ifndef FREESTANDING_MATH_H
#define FREESTANDING_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/* --- basic types / macros --- */
#include <stdint.h> /* allowed in freestanding environments under C99 */
#include <float.h>

/* constants */
#ifndef INFINITY
#define INFINITY (__builtin_inff())
#endif

#ifndef NAN
#define NAN (__builtin_nanf(""))
#endif

#ifndef HUGE_VAL
#define HUGE_VAL (INFINITY)
#endif

/* Common math constants (optional) */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#ifndef M_E
#define M_E 2.71828182845904523536028747135266250
#endif

/* --- classification helpers --- */
/* We forward to builtins if available. These are usually supported by clang/gcc. */

static inline int isnan_f(double x) { return __builtin_isnan(x); }
static inline int isinf_f(double x) { return __builtin_isinf_sign(x) ? 1 : (__builtin_isinf(x) ? 1 : 0); }
static inline int isfinite_f(double x) { return __builtin_isfinite(x); }

/* Provide C-style names */
static inline int isnan(double x) { return isnan_f(x); }
static inline int isinf(double x) { return __builtin_isinf(x); }
static inline int isfinite(double x) { return isfinite_f(x); }

/* float / long double variants */
static inline int isnanf(float x) { return __builtin_isnan(x); }
static inline int isnanl(long double x) { return __builtin_isnan((double)x); }

static inline int isfinitef(float x) { return __builtin_isfinite(x); }
static inline int isfinitel(long double x) { return __builtin_isfinite((double)x); }

/* --- basic ops --- */
static inline double fabsd(double x) { return __builtin_fabs(x); }
static inline float fabsf(float x) { return __builtin_fabsf(x); }
static inline long double fabsl(long double x) { return __builtin_fabsl(x); }

static inline double copysignd(double x, double y) { return __builtin_copysign(x, y); }
static inline float copysignf(float x, float y) { return __builtin_copysignf(x, y); }
static inline long double copysignl(long double x, long double y) { return __builtin_copysignl(x, y); }

/* fmin / fmax with NaN handling similar to POSIX: if one arg is NaN, return the other */
static inline double fmin_d(double a, double b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a < b ? a : b;
}
static inline double fmax_d(double a, double b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a > b ? a : b;
}
static inline float fminf_f(float a, float b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a < b ? a : b;
}
static inline float fmaxf_f(float a, float b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a > b ? a : b;
}

/* fma */
static inline double fma_d(double a, double b, double c) { return __builtin_fma(a,b,c); }
static inline float fmaf_f(float a, float b, float c) { return __builtin_fmaf(a,b,c); }
static inline long double fmal_l(long double a, long double b, long double c) { return __builtin_fmal(a,b,c); }

/* --- rounding & truncation --- */
static inline double floor_d(double x) { return __builtin_floor(x); }
static inline double ceil_d(double x) { return __builtin_ceil(x); }
static inline double trunc_d(double x) { return __builtin_trunc(x); }
static inline double round_d(double x) { return __builtin_round(x); }
static inline float floorf_f(float x) { return __builtin_floorf(x); }
static inline float ceilf_f(float x) { return __builtin_ceilf(x); }

/* --- basic transcendental functions --- */
/* These are thin wrappers around compiler builtins which map to libm or intrinsics.
 * In a true freestanding environment you may need to provide your own implementations.
 */

static inline double sin_d(double x) { return __builtin_sin(x); }
static inline float sinf_f(float x) { return __builtin_sinf(x); }
static inline long double sinl_l(long double x) { return __builtin_sinl(x); }

static inline double cos_d(double x) { return __builtin_cos(x); }
static inline float cosf_f(float x) { return __builtin_cosf(x); }
static inline long double cosl_l(long double x) { return __builtin_cosl(x); }

static inline double tan_d(double x) { return __builtin_tan(x); }
static inline float tanf_f(float x) { return __builtin_tanf(x); }

static inline double atan2_d(double y, double x) { return __builtin_atan2(y,x); }
static inline float atan2f_f(float y, float x) { return __builtin_atan2f(y,x); }

static inline double sqrt_d(double x) { return __builtin_sqrt(x); }
static inline float sqrtf_f(float x) { return __builtin_sqrtf(x); }
static inline long double sqrtl_l(long double x) { return __builtin_sqrtl(x); }

static inline double exp_d(double x) { return __builtin_exp(x); }
static inline float expf_f(float x) { return __builtin_expf(x); }
static inline double log_d(double x) { return __builtin_log(x); }
static inline float logf_f(float x) { return __builtin_logf(x); }

static inline double pow_d(double x, double y) { return __builtin_pow(x,y); }
static inline float powf_f(float x, float y) { return __builtin_powf(x,y); }

/* fmod */
static inline double fmod_d(double x, double y) { return __builtin_fmod(x,y); }
static inline float fmodf_f(float x, float y) { return __builtin_fmodf(x,y); }

/* --- mantissa/exponent helpers --- */
/* frexp, ldexp, modf — use builtins */
static inline double frexp_d(double x, int *exp) { return __builtin_frexp(x, exp); }
static inline float frexpf_f(float x, int *exp) { return __builtin_frexpf(x, exp); }

static inline double ldexp_d(double x, int exp) { return __builtin_ldexp(x, exp); }
static inline float ldexpf_f(float x, int exp) { return __builtin_ldexpf(x, exp); }

static inline double modf_d(double x, double *iptr) { return __builtin_modf(x, iptr); }
static inline float modff_f(float x, float *iptr) { return __builtin_modff(x, iptr); }

/* --- shortcuts: traditional names similar to <math.h> --- */

#define fabs(x) \
    _Generic((x), \
        float: fabsf_f, \
        double: fabsd, \
        long double: fabsl \
    )(x)

#define sin(x) \
    _Generic((x), \
        float: sinf_f, \
        double: sin_d, \
        long double: sinl_l \
    )(x)

#define cos(x) \
    _Generic((x), \
        float: cosf_f, \
        double: cos_d, \
        long double: cosl_l \
    )(x)

#define tan(x) \
    _Generic((x), \
        float: tanf_f, \
        double: tan_d \
    )(x)

#define sqrt(x) \
    _Generic((x), \
        float: sqrtf_f, \
        double: sqrt_d, \
        long double: sqrtl_l \
    )(x)

#define exp(x) \
    _Generic((x), \
        float: expf_f, \
        double: exp_d \
    )(x)

#define log(x) \
    _Generic((x), \
        float: logf_f, \
        double: log_d \
    )(x)

#define pow(x,y) \
    _Generic((x), \
        float: powf_f, \
        double: pow_d \
    )(x,y)

#define fmod(x,y) \
    _Generic((x), \
        float: fmodf_f, \
        double: fmod_d \
    )(x,y)

#define isnan(x) \
    _Generic((x), \
        float: isnanf, \
        double: isnan, \
        long double: isnanl \
    )(x)

#define isfinite(x) \
    _Generic((x), \
        float: isfinitef, \
        double: isfinite, \
        long double: isfinitel \
    )(x)

/* Provide fallback simple macros for min/max on integers if desired */
#define IMIN(a,b) ((a) < (b) ? (a) : (b))
#define IMAX(a,b) ((a) > (b) ? (a) : (b))

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FREESTANDING_MATH_H */
