#ifndef __jz4740_emc_h__
#define __jz4740_emc_h__

#include <stdbool.h>
#include <stdint.h>



typedef union {
	struct {
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
	} __attribute__((__packed__));
	uint32_t mask;
} __attribute__((__packed__)) emc_sdram_ctrl_reg;

typedef union {
	struct {
		unsigned int cks       : 3;
		unsigned int reserved0 : 4;
		unsigned int cmf       : 1;
		unsigned int reserved1 : 8;
	} __attribute__((__packed__));
	uint16_t mask;
} __attribute__((__packed__)) emc_sdram_ref_stat_reg;

typedef struct {
	uintptr_t tras, rcd, tpc, trwl, trc, tcl, tref;
} emc_sdram_timings;



extern volatile emc_sdram_ctrl_reg*     const emc_dmcr;
extern volatile emc_sdram_ref_stat_reg* const emc_rtcsr;
extern volatile uint16_t*               const emc_rtcor;
extern volatile uint16_t*               const emc_rtcnt;



extern uintptr_t emc_sdram_refresh_get();
extern bool      emc_sdram_refresh_set(uintptr_t inTREF);

extern emc_sdram_timings emc_sdram_timings_get();
extern bool              emc_sdram_timings_set(emc_sdram_timings inTimings);
extern bool              emc_sdram_timings_safe(emc_sdram_timings inTimings);
extern bool              emc_sdram_timings_dump(const char* inPath, uintptr_t inFreq);

extern bool emc_sdram_ctrl_reg_dump(const char* inPath);

#endif
