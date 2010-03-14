#ifndef __fixmath_h__
#define __fixmath_h__

#include <stdint.h>



extern uint32_t uint32_log2(uint32_t inVal);



typedef int32_t fix16_t;

extern const fix16_t fix16_pi;
extern const fix16_t fix16_e;

extern int32_t fix16_to_int(fix16_t inVal);
extern fix16_t fix16_from_int(int32_t inVal);

extern fix16_t fix16_sadd(fix16_t inArg0, fix16_t inArg1);
extern fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1);
extern fix16_t fix16_div(fix16_t inArg0, fix16_t inArg1);

extern fix16_t fix16_sin(fix16_t inAngle);
extern fix16_t fix16_cos(fix16_t inAngle);
extern fix16_t fix16_tan(fix16_t inAngle);

extern fix16_t fix16_asin(fix16_t inValue);
extern fix16_t fix16_acos(fix16_t inValue);
extern fix16_t fix16_atan(fix16_t inValue);
extern fix16_t fix16_atan2(fix16_t inY, fix16_t inX);

extern fix16_t fix16_sqrt(fix16_t inValue);



typedef uint32_t fract_t;

extern fract_t  fract_create(uint32_t inNumerator, uint32_t inDenominator);
extern fract_t  fract_invert(fract_t inFract);
extern uint32_t fract_usmul(uint32_t inVal, fract_t inFract);
extern int32_t  fract_smul(int32_t inVal, fract_t inFract);

#endif
