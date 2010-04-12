#ifndef __jz4740_sdram_h__
#define __jz4740_sdram_h__

#define SDRAM_HYNIX

#define SDRAM_TREF_CALC(cycles, time_ms) (((time_ms) * 1000000) / (cycles))

#if defined(SDRAM_HYNIX)
	#define SDRAM_FREQ      166666667
	#define SDRAM_TRAS      42 // RAS Active Time
	#define SDRAM_RCD       18 // RAS to CAS Delay
	#define SDRAM_TPC       18 // RAS Precharge Time
	#define SDRAM_TRWL       7 // Write Latency Time
	#define SDRAM_TREF      SDRAM_TREF_CALC(4096, 64) // Refresh period: 4096 refresh cycles/64ms
	#define SDRAM_TRC       60
	#define SDRAM_TCL(f)     2 // CAS Latency
#else
	#define SDRAM_FREQ      133333333
	#define SDRAM_TRAS      45 // RAS Active Time
	#define SDRAM_RCD       22 // RAS to CAS Delay
	#define SDRAM_TPC       22 // RAS Precharge Time
	#define SDRAM_TRWL       7 // Write Latency Time
	#define SDRAM_TREF      SDRAM_TREF_CALC(4096, 64) // Refresh period: 4096 refresh cycles/64ms
	#define SDRAM_TRC       68
	#define SDRAM_TCL(f) ((f) > 100000000 ? 3 : 2)
#endif

#endif
