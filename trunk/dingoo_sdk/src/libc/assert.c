#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void _assert(char* inProgram, char* inFile, unsigned int inLine, const char* inFunction, char* inExpressionString, int inExpression) {
	if(!inExpression) {
		//fprintf(stdout, "%s: %s:%lu: %s: Assertion '%s' failed.", inPath, inFile, inLine, inFunction, inExpressionString);
		abort();
	}
}

/* This prints an "Assertion failed" message and aborts.  */
void __assert_fail (const char *__assertion, const char *__file, unsigned int __line, const char *__function) {
  abort();
}
