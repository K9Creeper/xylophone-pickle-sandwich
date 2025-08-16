#ifndef FREESTANDING_MATH_H
#define FREESTANDING_MATH_H

#include <stdint.h>
#include <float.h>

/* Constants */
#ifndef INFINITY
#define INFINITY (__builtin_inff())
#endif
#ifndef NAN
#define NAN (__builtin_nanf(""))
#endif
#ifndef HUGE_VAL
#define HUGE_VAL (INFINITY)
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

/* Classification */
static inline int isnan(double x)   { return __builtin_isnan(x); }
static inline int isnanf(float x)   { return __builtin_isnan(x); }
static inline int isnanl(long double x) { return __builtin_isnan((double)x); }

static inline int isinf(double x)   { return __builtin_isinf(x); }
static inline int isfinite(double x){ return __builtin_isfinite(x); }
static inline int isfinitef(float x){ return __builtin_isfinite(x); }
static inline int isfinitel(long double x) { return __builtin_isfinite((double)x); }

/* Absolute value / sign */
static inline double fabs(double x) { return __builtin_fabs(x); }
static inline float fabsf(float x)  { return __builtin_fabsf(x); }
static inline long double fabsl(long double x) { return __builtin_fabsl(x); }

static inline double copysign(double x, double y) { return __builtin_copysign(x,y); }
static inline float copysignf(float x, float y) { return __builtin_copysignf(x,y); }
static inline long double copysignl(long double x, long double y) { return __builtin_copysignl(x,y); }

/* Rounding / truncation */
static inline double floor(double x) { return __builtin_floor(x); }
static inline double ceil(double x)  { return __builtin_ceil(x); }
static inline double trunc(double x) { return __builtin_trunc(x); }
static inline double round(double x) { return __builtin_round(x); }

static inline float floorf(float x) { return __builtin_floorf(x); }
static inline float ceilf(float x)  { return __builtin_ceilf(x); }
static inline float truncf(float x) { return __builtin_truncf(x); }
static inline float roundf(float x) { return __builtin_roundf(x); }

/* Trigonometry */
static inline double sin(double x) { return __builtin_sin(x); }
static inline float sinf(float x) { return __builtin_sinf(x); }
static inline long double sinl(long double x) { return __builtin_sinl(x); }

static inline double cos(double x) { return __builtin_cos(x); }
static inline float cosf(float x) { return __builtin_cosf(x); }
static inline long double cosl(long double x) { return __builtin_cosl(x); }

static inline double tan(double x) { return __builtin_tan(x); }
static inline float tanf(float x) { return __builtin_tanf(x); }

/* Powers / exponentials / logs */
static inline double exp(double x) { return __builtin_exp(x); }
static inline float expf(float x) { return __builtin_expf(x); }

static inline double log(double x) { return __builtin_log(x); }
static inline float logf(float x) { return __builtin_logf(x); }

static inline double pow(double x, double y) { return __builtin_pow(x,y); }
static inline float powf(float x, float y) { return __builtin_powf(x,y); }

static inline double sqrt(double x) { return __builtin_sqrt(x); }
static inline float sqrtf(float x) { return __builtin_sqrtf(x); }
static inline long double sqrtl(long double x) { return __builtin_sqrtl(x); }

/* Min / max with NaN handling for floats */
static inline double fmin(double a, double b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a < b ? a : b;
}
static inline double fmax(double a, double b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a > b ? a : b;
}
static inline float fminf(float a, float b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a < b ? a : b;
}
static inline float fmaxf(float a, float b) {
    if (__builtin_isnan(a)) return b;
    if (__builtin_isnan(b)) return a;
    return a > b ? a : b;
}

/* Integer min/max */
static inline int imin(int a, int b) { return a < b ? a : b; }
static inline int imax(int a, int b) { return a > b ? a : b; }
static inline long lmin(long a, long b) { return a < b ? a : b; }
static inline long lmax(long a, long b) { return a > b ? a : b; }

/* Clamp helpers */
static inline int clampi(int val, int min, int max) {
    if(val < min) return min;
    if(val > max) return max;
    return val;
}
static inline long clampl(long val, long min, long max) {
    if(val < min) return min;
    if(val > max) return max;
    return val;
}

/* Integer absolute value */
static inline int abs(int x) { return x < 0 ? -x : x; }
static inline long labs(long x) { return x < 0 ? -x : x; }

/* Misc */
static inline double fmod(double x, double y) { return __builtin_fmod(x,y); }
static inline float fmodf(float x, float y) { return __builtin_fmodf(x,y); }

static inline double atan2(double y, double x) { return __builtin_atan2(y,x); }
static inline float atan2f(float y, float x) { return __builtin_atan2f(y,x); }

static inline double frexp(double x, int *exp) { return __builtin_frexp(x, exp); }
static inline float frexpf(float x, int *exp) { return __builtin_frexpf(x, exp); }

static inline double ldexp(double x, int exp) { return __builtin_ldexp(x, exp); }
static inline float ldexpf(float x, int exp) { return __builtin_ldexpf(x, exp); }

static inline double modf(double x, double *iptr) { return __builtin_modf(x, iptr); }
static inline float modff(float x, float *iptr) { return __builtin_modff(x, iptr); }

#endif /* FREESTANDING_MATH_H */