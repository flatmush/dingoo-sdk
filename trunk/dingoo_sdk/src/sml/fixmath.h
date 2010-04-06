#ifndef __fixmath_h__
#define __fixmath_h__
/*!
	\file fixmath.h
	\brief Functions to perform fast fixed-point math operations.
*/
#include <stdint.h>
#include <stdbool.h>

#define fix16_MAX (fix16_t)0x7FFFFFFF /*!< the maximum value of fix16_t */
#define fix16_MIN (fix16_t)0x80000000 /*!< the minimum value of fix16_t */

typedef uint32_t fract32_t;
typedef int32_t fix16_t;

extern const fix16_t fix16_pi; /*!< fix16_t value of pi */
extern const fix16_t fix16_e; /*!< fix16_t value of e */
extern const fix16_t fix16_one; /*!< fix16_t value of 1 */

/*! Performs an unsigned log-base2 on the specified unsigned integer and returns the result.
*/
extern uint32_t uint32_log2(uint32_t inVal);

/*! Coverts a fix16_t to a double and returns the result.
*/
extern double  fix16_to_dbl(const fix16_t inVal);

/*! Converts a double to a fix16_t and returns the result.
*/
extern fix16_t fix16_from_dbl(const double inVal);

/*! Converts a fix16_t to a float and returns the result.
*/
extern float   fix16_to_float(const fix16_t inVal);

/*! Converts a float to a fix16_t and returns the result.
*/
extern fix16_t fix16_from_float(const float inVal);

/*! Converts a fix16_t to a signed integer and returns the result.
*/
extern int32_t fix16_to_int(const fix16_t inVal);

/*! Converts a signed integer to a fix16_t and returns the result.
*/
extern fix16_t fix16_from_int(const int32_t inVal);

/*! Performs a saturated addition (overflow-protected) of the two given fix16_t's and returns the result.
*/
extern fix16_t fix16_sadd(fix16_t inArg0, fix16_t inArg1);

/*! Multiplies the two given fix16_t's and returns the result.
*/
extern fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1);

/*! Performs a saturated multiplication (overflow-protected) of the two given fix16_t's and returns the result.
*/
extern fix16_t fix16_smul(fix16_t inArg0, fix16_t inArg1);

/*! Divides the first given fix16_t by the second and returns the result.
*/
extern fix16_t fix16_div(fix16_t inArg0, fix16_t inArg1);

/*! Performs a saturated division (overflow-protected) of the first fix16_t by the second and returns the result.
*/
extern fix16_t fix16_sdiv(fix16_t inArg0, fix16_t inArg1);

/*! Returns the sine of the given fix16_t.
*/
extern fix16_t fix16_sin(fix16_t inAngle);

/*! Returns the cosine of the given fix16_t.
*/
extern fix16_t fix16_cos(fix16_t inAngle);

/*! Returns the tangent of the given fix16_t.
*/
extern fix16_t fix16_tan(fix16_t inAngle);

/*! Returns the arcsine of the given fix16_t.
*/
extern fix16_t fix16_asin(fix16_t inValue);

/*! Returns the arccosine of the given fix16_t.
*/
extern fix16_t fix16_acos(fix16_t inValue);

/*! Returns the arctangent of the given fix16_t.
*/
extern fix16_t fix16_atan(fix16_t inValue);

/*! Returns the arctangent of inY/inX.
*/
extern fix16_t fix16_atan2(fix16_t inY, fix16_t inX);

/*! Returns the square root of the given fix16_t.
*/
extern fix16_t fix16_sqrt(fix16_t inValue);

/*! Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
*/
extern fix16_t fix16_lerp16(fix16_t inArg0, fix16_t inArg1, uint16_t inFract);

/*! Creates a fraction using unsigned integers.
	\param inNumerator the unsigned integer numerator
	\param inDenominator the unsigned integer denominator
	\return a fraction using the given numerator and denominator
*/
extern fract32_t fract32_create(uint32_t inNumerator, uint32_t inDenominator);

/*! Inverts the given fraction, swapping the numerator and the denominator.
*/
extern fract32_t fract32_invert(fract32_t inFract);

/*! Performs unsigned saturated (overflow-protected) multiplication with the two given fractions and returns the result as an unsigned integer.
*/
extern uint32_t  fract32_usmul(uint32_t inVal, fract32_t inFract);

/*! Performs saturated (overflow-protected) multiplication with the two given fractions and returns the result as a signed integer.
*/
extern int32_t   fract32_smul(int32_t inVal, fract32_t inFract);

#endif
