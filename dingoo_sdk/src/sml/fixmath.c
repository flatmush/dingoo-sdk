#include "fixmath.h"



uint32_t uint32_log2(uint32_t inVal) {
	if(inVal == 0)
		return 0;
	uint32_t tempOut = 0;
	if(inVal >= (1 << 16)) { inVal >>= 16; tempOut += 16; }
	if(inVal >= (1 <<  8)) { inVal >>=  8; tempOut +=  8; }
	if(inVal >= (1 <<  4)) { inVal >>=  4; tempOut +=  4; }
	if(inVal >= (1 <<  2)) { inVal >>=  2; tempOut +=  2; }
	if(inVal >= (1 <<  1)) {               tempOut +=  1; }
	return tempOut;
}




fix16_t _fix16_sin_cache_index[4096] = { 0 };
fix16_t _fix16_sin_cache_value[4096] = { 0 };
fix16_t _fix16_atan_cache_index[4096] = { 0 };
fix16_t _fix16_atan_cache_value[4096] = { 0 };
fix16_t _fix16_sqrt_cache_index[4096] = { 0 };
fix16_t _fix16_sqrt_cache_value[4096] = { 0 };



const fix16_t fix16_pi  = 205887;
const fix16_t fix16_e   = 178145;
const fix16_t fix16_one = 0x00010000;



double fix16_to_dbl(const fix16_t inVal) {
	return ((double)inVal / 65536.0);
}

fix16_t fix16_from_dbl(const double inVal) {
	return (fix16_t)(inVal * 65536.0);
}

float fix16_to_float(const fix16_t inVal) {
	return ((float)inVal / 65536.0f);
}

fix16_t fix16_from_float(const float inVal) {
	return (fix16_t)(inVal * 65536.0f);
}

int32_t fix16_to_int(const fix16_t inVal) {
	return ((inVal + 0x00008000) >> 16);
}

fix16_t fix16_from_int(const int32_t inVal) {
	return (inVal << 16);
}



fix16_t fix16_sadd(fix16_t inArg0, fix16_t inArg1) {
	fix16_t tempResult = (inArg0 + inArg1);
	if((tempResult > 0) && (inArg0 < 0) && (inArg1 < 0))
		return 0x80000000;
	if((tempResult < 0) && (inArg0 > 0) && (inArg1 > 0))
		return 0x7FFFFFFF;
	return tempResult;
}

fix16_t fix16_mul(fix16_t inArg0, fix16_t inArg1) {
	int64_t tempResult = ((int64_t)inArg0 * (int64_t)inArg1) + 0x00008000;
	tempResult >>= 16;
	return tempResult;
}

fix16_t fix16_smul(fix16_t inArg0, fix16_t inArg1) {
	int64_t tempResult = ((int64_t)inArg0 * (int64_t)inArg1) + 0x00008000;
	tempResult >>= 16;
	if(tempResult < fix16_MIN)
		return fix16_MIN;
	if(tempResult > fix16_MAX)
		return fix16_MAX;
	return tempResult;
}

fix16_t fix16_div(fix16_t inArg0, fix16_t inArg1) {
	int64_t tempResult = inArg0;
	tempResult <<= 16;
	tempResult += (inArg1 >> 1);
	tempResult /= inArg1;
	return tempResult;
}

fix16_t fix16_sdiv(fix16_t inArg0, fix16_t inArg1) {
	if(inArg1 == 0) {
		if(inArg0 < 0)
			return fix16_MIN;
		return fix16_MAX;
	}
	int64_t tempResult = inArg0;
	tempResult <<= 16;
	tempResult += (inArg1 >> 1);
	tempResult /= inArg1;
	if(tempResult < fix16_MIN)
		return fix16_MIN;
	if(tempResult > fix16_MAX)
		return fix16_MAX;
	return tempResult;
}



fix16_t fix16_sin(fix16_t inAngle) {
	fix16_t tempAngle = inAngle % (fix16_pi << 1);
	if(tempAngle > fix16_pi)
		tempAngle -= (fix16_pi << 1);
	else if(tempAngle < -fix16_pi)
		tempAngle += (fix16_pi << 1);

	fix16_t tempIndex = ((inAngle >> 5) & 0x00000FFF);
	if(_fix16_sin_cache_index[tempIndex] == inAngle)
		return _fix16_sin_cache_value[tempIndex];

	fix16_t tempAngleSq = fix16_mul(tempAngle, tempAngle);

	fix16_t tempOut = tempAngle;
	tempAngle = fix16_mul(tempAngle, tempAngleSq);
	tempOut -= (tempAngle / 6);
	tempAngle = fix16_mul(tempAngle, tempAngleSq);
	tempOut += (tempAngle / 120);
	tempAngle = fix16_mul(tempAngle, tempAngleSq);
	tempOut -= (tempAngle / 5040);
	tempAngle = fix16_mul(tempAngle, tempAngleSq);
	tempOut += (tempAngle / 362880);
	tempAngle = fix16_mul(tempAngle, tempAngleSq);
	tempOut -= (tempAngle / 39916800);

	_fix16_sin_cache_index[tempIndex] = inAngle;
	_fix16_sin_cache_value[tempIndex] = tempOut;

	return tempOut;
}

fix16_t fix16_cos(fix16_t inAngle) {
	return fix16_sin(inAngle + (fix16_pi >> 1));
}

fix16_t fix16_tan(fix16_t inAngle) {
	return fix16_div(fix16_sin(inAngle), fix16_cos(inAngle));
}

fix16_t fix16_asin(fix16_t inValue) {
	if((inValue > 1) || (inValue < -1))
		return 0;
	fix16_t tempOut;
	tempOut = (1 - fix16_mul(inValue, inValue));
	tempOut = (inValue / fix16_sqrt(tempOut));
	tempOut = fix16_atan(tempOut);
	return tempOut;
}

fix16_t fix16_acos(fix16_t inValue) {
	return ((fix16_pi >> 1) - fix16_asin(inValue));
}

fix16_t _fix16_atan(fix16_t inValue) {
	fix16_t tempOut;
	if(inValue > 29736) {
		tempOut = (1 + fix16_mul(inValue, inValue));
		tempOut = (1 + fix16_sqrt(tempOut));
		tempOut = (inValue / tempOut);
		tempOut = _fix16_atan(tempOut);
		return (tempOut << 1);
	}

	fix16_t tempValue = inValue;
	fix16_t tempValSq = fix16_mul(inValue, inValue);
	tempOut = inValue;
	tempValue = fix16_mul(tempValue, tempValSq);
	tempOut += (tempValue / 3);
	tempValue = fix16_mul(tempValue, tempValSq);
	tempOut += (tempValue / 5);
	tempValue = fix16_mul(tempValue, tempValSq);
	tempOut += (tempValue / 7);
	tempValue = fix16_mul(tempValue, tempValSq);
	tempOut += (tempValue / 9);
	tempValue = fix16_mul(tempValue, tempValSq);
	tempOut += (tempValue / 11);

	return tempOut;
}

fix16_t fix16_atan(fix16_t inValue) {
	fix16_t tempIndex = (((inValue >> 16) ^ (inValue >> 4)) & 0x00000FFF);
	if(_fix16_atan_cache_index[tempIndex] == inValue)
		return _fix16_atan_cache_value[tempIndex];
	fix16_t tempOut = _fix16_atan(inValue);
	_fix16_atan_cache_index[tempIndex] = inValue;
	_fix16_atan_cache_value[tempIndex] = tempOut;
	return tempOut;
}

fix16_t fix16_atan2(fix16_t inY, fix16_t inX) {
	if(inX == 0) {
		if(inY == 0)
			return 0;
		if(inY > 0)
			return (fix16_pi >> 1);
		return -(fix16_pi >> 1);
	}
	if(inY > 0)
		return fix16_atan(fix16_div(inY, inX));
	if(inY >= 0)
		return (fix16_pi + fix16_atan(fix16_div(inY, inX)));
	return (-fix16_pi + fix16_atan(fix16_div(inY, inX)));
}



fix16_t fix16_sqrt(fix16_t inValue) {
	fix16_t tempIndex = (((inValue >> 16) ^ (inValue >> 4)) & 0x00000FFF);
	if(_fix16_sqrt_cache_index[tempIndex] == inValue)
		return _fix16_sqrt_cache_value[tempIndex];

	int64_t tempOp = inValue; tempOp <<= 16;
	int64_t tempOut = 0;
	int64_t tempOne = 0x4000000000000000ULL;

	while(tempOne > tempOp)
		tempOne >>= 2;

	while(tempOne != 0) {
		if (tempOp >= tempOut + tempOne) {
			tempOp -= tempOut + tempOne;
			tempOut += tempOne << 1;
		}
		tempOut >>= 1;
		tempOne >>= 2;
	}

	_fix16_sqrt_cache_index[tempIndex] = inValue;
	_fix16_sqrt_cache_value[tempIndex] = tempOut;

	return tempOut;
}

fix16_t fix16_lerp16(fix16_t inArg0, fix16_t inArg1, uint16_t inFract) {
	int64_t tempOut;
	tempOut   = ((int64_t)inArg0 * (fix16_one - inFract));
	tempOut  += ((int64_t)inArg1 * inFract);
	tempOut >>= 16;
	return (fix16_t)tempOut;
}






fract32_t fract32_create(uint32_t inNumerator, uint32_t inDenominator) {
	if(inDenominator <= inNumerator)
		return 0xFFFFFFFF;
	uint32_t tempMod = (inNumerator % inDenominator);
	uint32_t tempDiv = (0xFFFFFFFF / (inDenominator - 1));
	return (tempMod * tempDiv);
}

fract32_t fract32_invert(fract32_t inFract) {
	return (0xFFFFFFFF - inFract);
}

uint32_t fract32_usmul(uint32_t inVal, fract32_t inFract) {
	return (uint32_t)(((uint64_t)inVal * (uint64_t)inFract) >> 32);
}

int32_t fract32_smul(int32_t inVal, fract32_t inFract) {
	if(inVal < 0)
		return -fract32_usmul(-inVal, inFract);
	return fract32_usmul(inVal, inFract);
}
