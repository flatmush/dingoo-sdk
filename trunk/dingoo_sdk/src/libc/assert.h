#ifndef __assert_h__
#define __assert_h__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef NDEBUG
	#define assert(expression) ((void)0)
#else
	extern char* _app_name;
	extern void _assert(char* inProgram, char* inFile, unsigned int inLine, char* inFunction, char* inExpressionString, int inExpression);
	#define assert(inExpression) _assert(_app_name, __FILE__, __LINE__, __FUNCTION__, #inExpression, inExpression)
#endif

#ifdef __cplusplus
}
#endif

#endif
