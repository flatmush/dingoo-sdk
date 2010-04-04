#ifndef __jz4740_emc_h__
#define __jz4740_emc_h__

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	unsigned int tcl             : 2;
	unsigned int trc             : 3;
	unsigned int trwl            : 2;
	unsigned int reserved0       : 1;
	unsigned int tpc             : 3;
	unsigned int rcd             : 2;
	unsigned int tras            : 3;
	unsigned int reserved1       : 1;
	unsigned int cke_pin_ctrl    : 1;
	unsigned int pow_down_mode   : 1;
	unsigned int bank_addr_width : 1;
	unsigned int row_addr_width  : 2;
	unsigned int reserved2       : 1;
	unsigned int mode_reg_set    : 1;
	unsigned int refresh_control : 1;
	unsigned int refresh_mode    : 1;
	unsigned int col_addr_width  : 3;
	unsigned int reserved3       : 2;
	unsigned int bus_width       : 1;
} __attribute__((__packed__)) emc_sdram_ctrl_reg;

typedef struct {
	unsigned int cks       : 3;
	unsigned int reserved0 : 4;
	unsigned int cmf       : 1;
	unsigned int reserved1 : 8;
} __attribute__((__packed__)) emc_sdram_ref_stat_reg;

extern bool emc_sdram_timings_set(uintptr_t inTRAS, uintptr_t inRCD, uintptr_t inTPC, uintptr_t inTRWL, uintptr_t inTRC, uintptr_t inTCL, uintptr_t inTREF);
extern bool emc_sdram_timings_dump(const char* inPath);

#endif
