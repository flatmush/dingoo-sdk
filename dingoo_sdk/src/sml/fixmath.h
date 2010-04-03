#ifndef __fixmath_h__
#define __fixmath_h__
/**
=sml/fixmath.h=
Functions to perform fast fixed-point math operations.
*/
#include <stdint.h>
#include <stdbool.h>

#define fix16_MAX (fix16_t)0x7FFFFFFF
#define fix16_MIN (fix16_t)0x80000000
/**
==Types==
  * *fract32`_`t*: Just a _uint32`_`t_.
  * *fix16`_`t*: Just an _int32`_`t_.
*/
typedef uint32_t fract32_t;
typedef int32_t fix16_t;

/**
==Global constants==
  * *fix16`_`pi*: Pi (3.14... TODO)
  * *fix16`_`e*: e (... TODO)
  * *fix16`_`one*: 1.0000000.. TODO
*/
extern const fix16_t fix16_pi;
extern const fix16_t fix16_e;
extern const fix16_t fix16_one;

/**
==Functions==
  * _uint32`_`t_ *uint32`_`log2*( _uint32`_`t_ ): Performs a log-base2 on the given number.
  * _double_ *fix16`_`to`_`dbl*( [#Types fix16_t] ): Converts the given fixed-point to a double.
  * [#Types fix16_t] *fix16`_`from`_`dbl*( _double_ ): Converts the given double to a fixed-point.
  * _int32`_`t_ *fix16`_`to`_`int*( [#Types fix16_t] ): Converts the given fixed-point to an integer.
  * [#Types fix16_t] *fix16`_`from`_`int*( _int32`_`t_ ): Converts the given integer to a fixed-point.
  * [#Types fix16_t] *fix16`_`sadd*( [#Types fix16_t], [#Types fix16_t] ): Adds the two fixed-points together.
  * [#Types fix16_t] *fix16`_`mul*( [#Types fix16_t], [#Types fix16_t] ): Multiplies the two fixed-points together.
  * [#Types fix16_t] *fix16`_`smul*( [#Types fix16_t], [#Types fix16_t] ): Multiplies the two _signed_ fixed-points together.
  * [#Types fix16_t] *fix16`_`div*( [#Types fix16_t], [#Types fix16_t] ): Divides the first fixed-point by the second fixed-point.
  * [#Types fix16_t] *fix16`_`sdiv*( [#Types fix16_t], [#Types fix16_t] ): Divides the first _signed_ fixed-point by the second _signed_ fixed-point.
  * [#Types fix16_t] *fix16`_`sin*( [#Types fix16_t] ): Performs a sine on the given fixed-point.
  * [#Types fix16_t] *fix16`_`cos*( [#Types fix16_t] ): Performs a cosine on the given fixed-point.
  * [#Types fix16_t] *fix16`_`tan*( [#Types fix16_t] ): Performs a tangent on the given fixed-point.
  * [#Types fix16_t] *fix16`_`asin*( [#Types fix16_t] ): Performs an inverse sine on the given fixed-point.
  * [#Types fix16_t] *fix16`_`scos*( [#Types fix16_t] ): Performs an inverse cosine on the given fixed-point.
  * [#Types fix16_t] *fix16`_`atan*( [#Types fix16_t] ): Performs an inverse tangent on the given fixed-point.
  * [#Types fix16_t] *fix16`_`atan2*( [#Types fix16_t] ): Performs an inverse base2-tangent on the given fixed-point. TODO
  * [#Types fix16_t] *fix16`_`sqrt*( [#Types fix16_t] ): Performs a square root on the given fixed-point.
  * [#Types fix16_t] *fix16`_`lerp16*( [#Types fix16_t] ): TODO
  * [#Types fract32_t] *fract32`_`create*( _uint32`_`t_, _uint32`_`t_ ): Creates a fraction with the first integer as the numerator, and the second as the denominator.
  * [#Types fract32_t] *fract32`_`invert*( [#Types fract32_t] ): Inverts the given fraction.
  * _uint32`_`t_  *fract32`_`usmul*( _uint32`_`t_, [#Types fract32_t] ): Multiplies the given fraction by the given unsigned integer.
  * _uint32`_`t_  *fract32`_`smul*( _int32`_`t_, [#Types fract32_t] ): Multiplies the given fraction by the given signed integer. 
*/
extern uint32_t uint32_log2(uint32_t inVal);

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

extern fract32_t fract32_create(uint32_t inNumerator, uint32_t inDenominator);
extern fract32_t fract32_invert(fract32_t inFract);
extern uint32_t  fract32_usmul(uint32_t inVal, fract32_t inFract);
extern int32_t   fract32_smul(int32_t inVal, fract32_t inFract);

#endif
