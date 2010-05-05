#ifndef __mips_asm_h__
#define __mips_asm_h__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

extern char* asm_disassemble(uint32_t inAddr, uint32_t inWord);
extern char* asm_disassemble_block(uint32_t* inPtr, uintptr_t inLength);

#ifdef __cplusplus
}
#endif

#endif
