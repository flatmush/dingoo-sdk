#include "asm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <inttypes.h>



#define PRIu8  "d"

#define PRIu16 "d"
#define PRIi16 "d"
#define PRIX16 "X"

#define PRIu32 "d"
#define PRIi32 "d"
#define PRIX32 "X"

#define _USED_O (63 << 26)
#define _USED_S (31 << 21)
#define _USED_T (31 << 16)
#define _USED_D (31 << 11)
#define _USED_H (31 << 6)
#define _USED_F 0x3F
#define _USED_I 0xFFFF
#define _USED_A 0xFFFFFFFF



char* _asm_reg_name[32] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
	};


char* asm_disassemble(uint32_t inAddr, uint32_t inWord) {
	char str[64];

	uint8_t  s = (inWord >> 21) & 31;
	uint8_t  t = (inWord >> 16) & 31;
	uint8_t  d = (inWord >> 11) & 31;
	uint8_t  h = (inWord >>  6) & 31;

	char* s_name = _asm_reg_name[s];
	char* t_name = _asm_reg_name[t];
	char* d_name = _asm_reg_name[d];

	uint8_t  op = (inWord >> 26);
	uint16_t ui = (inWord & 0x0000FFFF);
	int16_t  si = (int16_t)ui;
	uint8_t  function = (inWord & 0x3F);
	uint32_t laddr = (((inWord << 2) & 0x0FFFFFFF) | (inAddr & 0xF0000000));
	int32_t  raddrb = (si << 0);
	int32_t  raddrh = (si << 1);
	int32_t  raddrw = (si << 2);
	uint32_t code   = ((inWord >> 6) & 0x000FFFFF);

	uint32_t umask = _USED_O;

	switch(op) {
		case 0x00: // 0b000000: SPECIAL R-Type
			umask |= _USED_F;
			switch(function) {
				case 0x00: // 0b000000
					sprintf(str, "sll $%s, $%s, %"PRIu8, d_name, t_name, h);
					umask |= _USED_D | _USED_T | _USED_H;
					break;
				case 0x02: // 0b000010:
					if((s & 1) == 0) sprintf(str, "srl $%s, $%s, %"PRIu8, d_name, t_name, h);
					else             sprintf(str, "rotr $%s, $%s, %"PRIu8, d_name, t_name, h);
					umask |= _USED_D | _USED_T | _USED_H | (_USED_S & 1);
					break;
				case 0x03: // 0b000011
					sprintf(str, "sra $%s, $%s, %"PRIu8, d_name, t_name, h);
					umask |= _USED_D | _USED_T | _USED_H;
					break;
				case 0x04: // 0b000100
					sprintf(str, "sllv $%s, $%s, $%s", d_name, t_name, s_name);
					umask |= _USED_D | _USED_T | _USED_S;
					break;
				case 0x06: // 0b000110
					if((h & 1) == 0) sprintf(str, "srlv $%s, $%s, $%s", d_name, t_name, s_name);
					else             sprintf(str, "rotrv $%s, $%s, $%s", d_name, t_name, s_name);
					umask |= _USED_D | _USED_T | _USED_S | (_USED_H & 1);
					break;
				case 0x07: // 0b000111
					sprintf(str, "srav $%s, $%s, $%s", d_name, t_name, s_name);
					umask |= _USED_D | _USED_T | _USED_S;
					break;
				case 0x08: // 0b001000
					sprintf(str, "jr $%s", s_name);
					umask |= _USED_S;
					break;
				case 0x09: // 0b001001
					sprintf(str, "jalr $%s, $%s", d_name, s_name);
					umask |= _USED_D | _USED_S;
					break;
				case 0x0A: // 0b001010
					sprintf(str, "movz $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x0B: // 0b001011
					sprintf(str, "movn $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x0C: // 0b001100
					sprintf(str, "syscall %"PRIu32, code);
					umask = _USED_A;
					break;
				case 0x0D: // 0b001101
					sprintf(str, "break %"PRIu32, code);
					umask = _USED_A;
					break;
				case 0x0F: // 0b001111
					sprintf(str, "sync %"PRIu8, h);
					umask |= _USED_H;
					break;
				case 0x10: // 0b010000
					sprintf(str, "mfhi $%s", d_name);
					umask |= _USED_D;
					break;
				case 0x11: // 0b010001
					sprintf(str, "mthi $%s", s_name);
					umask |= _USED_S;
					break;
				case 0x12: // 0b010010
					sprintf(str, "mflo $%s", d_name);
					umask |= _USED_D;
					break;
				case 0x13: // 0b010011
					sprintf(str, "mtlo $%s", s_name);
					umask |= _USED_S;
					break;
				case 0x18: // 0b011000
					sprintf(str, "mult $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x19: // 0b011001
					sprintf(str, "multu $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x1A: // 0b011010
					sprintf(str, "div $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x1B: // 0b011011
					sprintf(str, "divu $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x20: // 0b100000
					sprintf(str, "add $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x21: // 0b100001
					sprintf(str, "addu $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x22: // 0b100000
					sprintf(str, "sub $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x23: // 0b100001
					sprintf(str, "subu $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x24: // 0b100100
					sprintf(str, "and $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x25: // 0b100101
					sprintf(str, "or $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x26: // 0b100110
					sprintf(str, "xor $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x27: // 0b100111
					sprintf(str, "nor $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x2A: // 0b101010
					sprintf(str, "slt $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x2B: // 0b101011
					sprintf(str, "sltu $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				default:
					umask = 0;
					break;
			}
			break;
		case 0x01: // 0b000001: REGIMM I-Type
			umask |= _USED_T;
			switch(t) {
				case 0x00:
					sprintf(str, "bltz $%s, %"PRIi16, s_name, raddrw);
					umask |= _USED_S | _USED_I;
					break;
				case 0x01:
					sprintf(str, "bgez $%s, %"PRIi16, s_name, raddrw);
					umask |= _USED_S | _USED_I;
					break;
				case 0x1F: //0b11111:
					sprintf(str, "synci %"PRIi16"($%s)", si, s_name);
					umask = _USED_A;
					break;
				default:
					umask = 0;
					break;
			}
			break;
		case 0x1C: // 0b011100: SPECIAL2 R-Type
			umask |= _USED_F;
			switch(function) {
				case 0x00: // 0b000000
					sprintf(str, "madd $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x01: // 0b000001
					sprintf(str, "maddu $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x02: // 0b000010
					sprintf(str, "mul $%s, $%s, $%s", d_name, s_name, t_name);
					umask |= _USED_D | _USED_S | _USED_T;
					break;
				case 0x04: // 0b000100
					sprintf(str, "msub $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x05: // 0b000101
					sprintf(str, "msubu $%s, $%s", s_name, t_name);
					umask |= _USED_S | _USED_T;
					break;
				case 0x20: // 0b100000
					sprintf(str, "clz $%s, $%s", d_name, s_name);
					umask |= _USED_D | _USED_S;
					break;
				case 0x21: // 0b100001
					sprintf(str, "clo $%s, $%s", d_name, s_name);
					umask |= _USED_D | _USED_S;
					break;
				case 0x3F: // 0b111111
					sprintf(str, "sdbbp %"PRIu32, code);
					umask = _USED_A;
					break;
				default:
					umask = 0;
					break;
			}
			break;
		case 0x1F: // 0b101111: SPECIAL3 R-Type
			umask |= _USED_F;
			switch(function) {
				case 0x00: // 0b000000:
					sprintf("EXT $%s, $%s, %"PRIu8", %"PRIu8, t_name, s_name, h, (d + 1));
					umask = _USED_A;
					break;
				case 0x04: // 0b000100:
					sprintf("INS $%s, $%s, %"PRIu8", %"PRIu8, t_name, s_name, h, ((d + 1) - h));
					umask = _USED_A;
					break;
				case 0x20: // 0b100000:
					umask |= _USED_H;
					switch(h) {
						case 0x02: // 0b00010:
							sprintf("WSBH $%s, $%s", d_name, t_name);
							umask |= _USED_D | _USED_T;
							break;
						case 0x10: // 0b10000:
							sprintf("SEB $%s, $%s", d_name, t_name);
							umask |= _USED_D | _USED_T;
							break;
						case 0x18: // 0b11000:
							sprintf("SEH $%s, $%s", d_name, t_name);
							umask |= _USED_D | _USED_T;
							break;
						default:
							umask = 0;
							break;
					}
					break;
				case 0x3B: // 0b111011:
					sprintf("RDHWR $%s, $%"PRIu8, t_name, d);
					umask |= _USED_T | _USED_D;
					break;
				default:
					umask = 0;
					break;
			}
			break;
		case 0x02: //0b000010:
			sprintf(str, "j 0x%08"PRIX32, laddr);
			umask |= _USED_I;
			break;
		case 0x03: //0b000011:
			sprintf(str, "jal 0x%08"PRIX32, laddr);
			umask |= _USED_I;
			break;
		case 0x04: //0b000100:
			sprintf(str, "beq $%s, $%s, %"PRIi16, s_name, t_name, raddrw);
			umask |= _USED_S | _USED_T | _USED_I;
			break;
		case 0x05: //0b000101:
			sprintf(str, "bne $%s, $%s, %"PRIi16, s_name, t_name, raddrw);
			umask |= _USED_S | _USED_T | _USED_I;
			break;
		case 0x06: //0b000110:
			sprintf(str, "blez $%s, %"PRIi16, s_name, raddrw);
			umask |= _USED_S | _USED_I;
			break;
		case 0x07: //0b000111:
			sprintf(str, "bgtz $%s, %"PRIi16, s_name, raddrw);
			umask |= _USED_S | _USED_I;
			break;
		case 0x08: //0b001000:
			sprintf(str, "addi $%s, $%s, %"PRIi16, t_name, s_name, si);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x09: //0b001001:
			sprintf(str, "addiu $%s, $%s, %"PRIu16, t_name, s_name, ui);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x0A: //0b001010:
			sprintf(str, "slti $%s, $%s, %"PRIi16, t_name, s_name, si);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x0B: //0b001011:
			sprintf(str, "sltiu $%s, $%s, %"PRIu16, t_name, s_name, ui);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x0C: //0b001100:
			sprintf(str, "andi $%s, $%s, 0x%04"PRIX16, t_name, s_name, ui);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x0D: //0b001101:
			sprintf(str, "ori $%s, $%s, 0x%04"PRIX16, t_name, s_name, ui);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x0E://0b001110:
			sprintf(str, "xori $%s, $%s, 0x%04"PRIX16, t_name, s_name, ui);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x0F: //0b001111:
			sprintf(str, "lui $%s, %"PRIu16, t_name, ui);
			umask |= _USED_T | _USED_I;
			break;
		case 0x10: //0b010000:
			if((s >> 4) != 0) {
				umask |= ((_USED_S & 1) << 4);
				umask |= _USED_F;
				switch(function) {
					case 0x01: // 0b000001:
						sprintf(str, "tlbr");
						break;
					case 0x02: // 0b000010:
						sprintf(str, "tlbwi");
						break;
					case 0x06: // 0b000110:
						sprintf(str, "tlbwr");
						break;
					case 0x08: // 0b001000:
						sprintf(str, "tlbp");
						break;
					case 0x18: // 0b011000:
						sprintf(str, "eret");
						break;
					case 0x1F: // 0b011111:
						sprintf(str, "deret");
						break;
					case 0x20: // 0b100000:
						sprintf(str, "wait %"PRIu32, ((inWord >> 6) & 0x0007FFFF));
						umask = _USED_A;
						break;
					default:
						umask = 0;
						break;
				}
			} else {
				umask |= _USED_S;
				switch(s) {
					case 0x00: // 0b00000
						sprintf(str, "mfc0 $%s, $%"PRIu8", %"PRIu16, t_name, d, (ui & 7));
						umask |= _USED_D | _USED_T | (_USED_I & 7);
						break;
					case 0x04: // 0b00100
						sprintf(str, "mtc0 $%s, $%"PRIu8", %"PRIu16, t_name, d, (ui & 7));
						umask |= _USED_D | _USED_T | (_USED_I & 7);
						break;
					case 0x0A: // 0b01010
						sprintf(str, "rdpgpr $%s, $%"PRIu8, d_name, t);
						umask |= _USED_D | _USED_T;
						break;
					case 0x0B: // 0b01011
						umask |= _USED_F;
						if(ui == 0x6010) {
							sprintf(str, "EI $%s", t_name);
							umask = _USED_A;
						} else if(ui == 0x6000) {
							sprintf(str, "DI $%s", t_name);
							umask = _USED_A;
						} else {
							umask = 0;
						}
						break;
					case 0x0E: // 0b01110
						sprintf(str, "wrpgpr $%s, $%s", d_name, t_name);
						umask |= _USED_D | _USED_T;
						break;
					default:
						umask = 0;
						break;
				}
			}
			break;
		case 0x20: //0b100000:
			sprintf(str, "lb $%s, %"PRIi16"($%s)", t_name, raddrb, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x21: //0b100001:
			sprintf(str, "lh $%s, %"PRIi16"($%s)", t_name, raddrh, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x23: //0b100011:
			sprintf(str, "lw $%s, %"PRIi16"($%s)", t_name, raddrw, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x24: //0b100100:
			sprintf(str, "lbu $%s, %"PRIi16"($%s)", t_name, raddrb, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x25: //0b100101:
			sprintf(str, "lhu $%s, %"PRIi16"($%s)", t_name, raddrh, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x28: //0b101000:
			sprintf(str, "sb $%s, %"PRIi16"($%s)", t_name, raddrb, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x29: //0b101001:
			sprintf(str, "sh $%s, %"PRIi16"($%s)", t_name, raddrh, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x2B: //0b101011:
			sprintf(str, "sw $%s, %"PRIi16"($%s)", t_name, raddrw, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x2F: //0b101111:
			sprintf(str, "cache %"PRIu8", %"PRIi16"($%s)", t, si, s_name);
			umask = _USED_A;
			break;
		case 0x31: //0b110001:
			sprintf(str, "lwc1 $%s, %"PRIi32"($%s)", t_name, raddrw, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x32: //0b110010:
			sprintf(str, "lwc2 $%s, %"PRIi32"($%s)", t_name, raddrw, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x39: //0b111001:
			sprintf(str, "swc1 $%s, %"PRIi32"($%s)", t_name, raddrw, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		case 0x3A: //0b111010:
			sprintf(str, "swc2 $%s, %"PRIi32"($%s)", t_name, raddrw, s_name);
			umask |= _USED_T | _USED_S | _USED_I;
			break;
		default:
			umask = 0;
			break;
	}

	if(inWord == 0)
		sprintf(str, "nop");
	else if(inWord == 1)
		sprintf(str, "ssnop");
	else if(inWord == 0x00000140)
		sprintf(str, "pause");
	else if((inWord & ~umask) != 0)
		sprintf(str, ".word 0x%08"PRIX32, inWord);

	char* tempOut = (char*)malloc(strlen(str) + 1);
	if(tempOut == NULL)
		return NULL;
	strcpy(tempOut, str);
	return tempOut;
}

char* asm_disassemble_block(uint32_t* inPtr, uintptr_t inLength) {
	if((inPtr == NULL) || (inLength == 0))
		return NULL;
	if(inLength == 1)
		return asm_disassemble((uint32_t)inPtr, *inPtr);

	uintptr_t tempLen = (64 * inLength);
	char* tempOut = (char*)malloc(tempLen);
	if(tempOut == NULL)
		return NULL;

	uintptr_t i, j;
	char* s;
	for(i = 0, j = 0; i < inLength; i++) {
		s = asm_disassemble((uint32_t)&inPtr[i], inPtr[i]);
		if(s == NULL) {
			free(tempOut);
			return NULL;
		}
		strcpy(&tempOut[j], s);
		j += strlen(s);
		free(s);
		tempOut[j] = '\n';
		j++;
	}
	tempOut[j] = '\0';

	char* tempRealloc = (char*)realloc(tempOut, (j + 1));
	if(tempRealloc == NULL)
		return tempOut;
	return tempRealloc;
}
