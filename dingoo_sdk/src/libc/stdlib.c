#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

uint32_t _rand_x = 0x12345678UL;
uint32_t _rand_a = 39916801UL;
uint32_t _rand_c = 479001599UL;



int abs(int n) {
	if(n >= 0)
		return n;
	return -n;
}

long int labs(long int n) {
	if(n >= 0)
		return n;
	return -n;
}



div_t div(int numerator, int denominator) {
	div_t tempOut;
	__asm__ volatile (
		"div     %2, %3\n"
		"mflo    %0\n"
		"mfhi    %1\n"
		: "=r"(tempOut.quot), "=r"(tempOut.rem) : "r"(numerator), "r"(denominator));
	return tempOut;
}

ldiv_t ldiv(long int numerator, long int denominator) {
	ldiv_t tempOut;
	__asm__ volatile (
		"div     %2, %3\n"
		"mflo    %0\n"
		"mfhi    %1\n"
		: "=r"(tempOut.quot), "=r"(tempOut.rem) : "r"(numerator), "r"(denominator));
	return tempOut;
}



bool _gcd_maxint32(uint32_t inArg) {
	if(inArg == 0)
		return false;

	uint32_t tempArg = 0 - inArg;

	while(tempArg != 0) {
		if(inArg > tempArg)
			inArg -= tempArg;
		else
			tempArg -= inArg;
	}

	return (inArg == 1);
}

uint32_t _coprime_maxint32() {
	uint32_t i, j;
	for(i = 1234567890, j = 1; !_gcd_maxint32(i); i -= j, j++);
	return i;
}

void srand(unsigned int inSeed) {
	_rand_x = inSeed;
}

int rand() {
	_rand_x *= _rand_a;
	_rand_x += _rand_c;
	return (_rand_x & RAND_MAX);
}
