#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <setjmp.h>
#include <ctype.h>



uint32_t _rand_x = 0x12345678UL;
uint32_t _rand_a = 39916801UL;
uint32_t _rand_c = 479001599UL;

extern jmp_buf _libc_exit_jmp_buf;
extern int     _libc_exit_status;


typedef void (*_atexit_func_t)(void);
uintptr_t       _atexit_count = 0;
_atexit_func_t* _atexit_table = NULL;


void _abort() {
	exit(EXIT_FAILURE);
}

void exit(int status) {
	_libc_exit_status = status;
	longjmp(_libc_exit_jmp_buf, 1);
}

int atexit(void (*func)(void)) {
	_atexit_func_t* tempRealloc = (_atexit_func_t*)realloc(_atexit_table, (sizeof(_atexit_func_t) * (_atexit_count + 1)));
	if(tempRealloc == NULL)
		return -1;
	_atexit_table = tempRealloc;
	_atexit_table[_atexit_count] = (_atexit_func_t)func;
	_atexit_count++;
	return 0;
}



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



int atoi(const char* str) {
	return atol(str);
}

long atol(const char* str) {
	if(str == NULL)
		return 0;
	long out = 0;
	uintptr_t i;
	for(i = 0; isdigit(str[i]); i++) {
		out *= 10;
		out += (str[i] - '0');
	}
	return out;
}

long long atoll(const char* str) {
	if(str == NULL)
		return 0;
	long long out = 0;
	uintptr_t i;
	for(i = 0; isdigit(str[i]); i++) {
		out *= 10;
		out += (str[i] - '0');
	}
	return out;
}

double atof(const char* str) {
	if(str == NULL)
		return 0;
	double out = 0.0;
	uintptr_t i;
	for(i = 0; isdigit(str[i]); i++) {
		out *= 10.0;
		out += (str[i] - '0');
	}
	if(str[i] == '.') {
		double j;
		for(i++, j = 10; isdigit(str[i]); i++, j *= 10)
			out += (double)(str[i] - '0') / j;
	}
	return out;
}
