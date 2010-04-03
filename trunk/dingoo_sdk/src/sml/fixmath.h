#ifndef __fixmath_h__
#define __fixmath_h__

#include <stdint.h>
#include <stdbool.h>

extern uint32_t uint32_log2(uint32_t inVal);

typedef int32_t fix16_t;
#define fix16_MAX (fix16_t)0x7FFFFFFF
#define fix16_MIN (fix16_t)0x80000000
extern const fix16_t fix16_pi;
extern const fix16_t fix16_e;
extern const fix16_t fix16_one;

extern double  fix16_to_dbl(const fix16_t inVal);
extern fix16_t fix16_from_dbl(const double inVal);

extern float   fix16_to_float(const fix16_t inVal);
extern fix16_t fix16_from_float(const float inVal);

extern int32_t fix16_to_int(const fix16_t inVal);
extern fix16_t fix16_from_int(const int32_t inVal);

extern fix16_t fix16_sadd(fix16_t inArg0, fix16_t inArg1);
extern fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1);
extern fix16_t fix16_smul(fix16_t inArg0, fix16_t inArg1);
extern fix16_t fix16_div(fix16_t inArg0, fix16_t inArg1);
extern fix16_t fix16_sdiv(fix16_t inArg0, fix16_t inArg1);

extern fix16_t fix16_sin(fix16_t inAngle);
extern fix16_t fix16_cos(fix16_t inAngle);
extern fix16_t fix16_tan(fix16_t inAngle);

extern fix16_t fix16_asin(fix16_t inValue);
extern fix16_t fix16_acos(fix16_t inValue);
extern fix16_t fix16_atan(fix16_t inValue);
extern fix16_t fix16_atan2(fix16_t inY, fix16_t inX);

extern fix16_t fix16_sqrt(fix16_t inValue);

extern fix16_t fix16_lerp16(fix16_t inArg0, fix16_t inArg1, uint16_t inFract);

typedef uint32_t fract32_t;

extern fract32_t fract32_create(uint32_t inNumerator, uint32_t inDenominator);
extern fract32_t fract32_invert(fract32_t inFract);
extern uint32_t  fract32_usmul(uint32_t inVal, fract32_t inFract);
extern int32_t   fract32_smul(int32_t inVal, fract32_t inFract);

#endif
