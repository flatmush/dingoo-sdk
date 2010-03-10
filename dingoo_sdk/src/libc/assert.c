#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void _assert(char* inProgram, char* inFile, unsigned int inLine, char* inFunction, char* inExpressionString, int inExpression) {
	if(!inExpression) {
		//fprintf(stdout, "%s: %s:%lu: %s: Assertion '%s' failed.", inPath, inFile, inLine, inFunction, inExpressionString);
		abort();
	}
}
