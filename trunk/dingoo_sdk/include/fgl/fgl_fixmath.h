#ifndef __fgl_fixmath_h__
#define __fgl_fixmath_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

extern uint32_t uint32_log2(uint32_t inVal);

typedef int32_t fgl_fix16_t;
#define fgl_fix16_MAX (fgl_fix16_t)0x7FFFFFFF
#define fgl_fix16_MIN (fgl_fix16_t)0x80000000
extern const fgl_fix16_t fgl_fix16_pi;
extern const fgl_fix16_t fgl_fix16_e;
extern const fgl_fix16_t fgl_fix16_one;

extern double      fgl_fix16_to_dbl(const fgl_fix16_t inVal);
extern fgl_fix16_t fgl_fix16_from_dbl(const double inVal);

extern float       fgl_fix16_to_float(const fgl_fix16_t inVal);
extern fgl_fix16_t fgl_fix16_from_float(const float inVal);

extern int32_t fgl_fix16_to_int(const fgl_fix16_t inVal);
extern fgl_fix16_t fgl_fix16_from_int(const int32_t inVal);

extern fgl_fix16_t fgl_fix16_sadd(fgl_fix16_t inArg0, fgl_fix16_t inArg1);
extern fgl_fix16_t fgl_fix16_mul(fgl_fix16_t inArg0, fgl_fix16_t inArg1);
extern fgl_fix16_t fgl_fix16_smul(fgl_fix16_t inArg0, fgl_fix16_t inArg1);
extern fgl_fix16_t fgl_fix16_div(fgl_fix16_t inArg0, fgl_fix16_t inArg1);
extern fgl_fix16_t fgl_fix16_sdiv(fgl_fix16_t inArg0, fgl_fix16_t inArg1);

extern fgl_fix16_t fgl_fix16_sin(fgl_fix16_t inAngle);
extern fgl_fix16_t fgl_fix16_cos(fgl_fix16_t inAngle);
extern fgl_fix16_t fgl_fix16_tan(fgl_fix16_t inAngle);

extern fgl_fix16_t fgl_fix16_asin(fgl_fix16_t inValue);
extern fgl_fix16_t fgl_fix16_acos(fgl_fix16_t inValue);
extern fgl_fix16_t fgl_fix16_atan(fgl_fix16_t inValue);
extern fgl_fix16_t fgl_fix16_atan2(fgl_fix16_t inY, fgl_fix16_t inX);

extern fgl_fix16_t fgl_fix16_sqrt(fgl_fix16_t inValue);

extern fgl_fix16_t fgl_fix16_lerp16(fgl_fix16_t inArg0, fgl_fix16_t inArg1, uint16_t inFract);

typedef uint32_t fgl_fract32_t;

extern fgl_fract32_t fgl_fract32_create(uint32_t inNumerator, uint32_t inDenominator);
extern fgl_fract32_t fgl_fract32_invert(fgl_fract32_t inFract);
extern uint32_t  fgl_fract32_usmul(uint32_t inVal, fgl_fract32_t inFract);
extern int32_t   fgl_fract32_smul(int32_t inVal, fgl_fract32_t inFract);

#ifdef __cplusplus
}
#endif

#endif
