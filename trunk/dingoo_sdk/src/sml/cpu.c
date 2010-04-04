#include "cpu.h"

#include <stdlib.h>

#include <jz4740/cpm.h>
#include <jz4740/emc.h>
#include <jz4740/jz4740.h>



#define SDRAM_HYNIX

#ifdef SDRAM_ELPIDA
#define SDRAM_FREQ      133333333
#define SDRAM_TRAS      45 // RAS Active Time */
#define SDRAM_RCD       22 // RAS to CAS Delay */
#define SDRAM_TPC       22 // RAS Precharge Time */
#define SDRAM_TRWL       7 // Write Latency Time */
#define SDRAM_TREF   15625 // Refresh period: 4096 refresh cycles/64ms
#define SDRAM_TRC       68
#define SDRAM_TCL(f) ((f) > 100000000 ? 3 : 2)
#endif

#ifdef SDRAM_HYNIX
#define SDRAM_FREQ      166666667
#define SDRAM_TRAS      42 // RAS Active Time
#define SDRAM_RCD       18 // RAS to CAS Delay
#define SDRAM_TPC       18 // RAS Precharge Time
#define SDRAM_TRWL       7 // Write Latency Time
#define SDRAM_TREF   15625 // Refresh period: 4096 refresh cycles/64ms
#define SDRAM_TRC       60
#define SDRAM_TCL(f)     2 // CAS Latency
#endif


#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif



bool cpu_clock_set(uintptr_t inCore, uintptr_t inMem) {
	uintptr_t tempM, tempN, tempD;
	if(!cpm_pll_freq_calc(inCore, &tempM, &tempN, &tempD))
		return false;

	if(inMem > SDRAM_FREQ)
		return false;
	if(inMem < (inCore >> 5))
		return false;

	cpm_freq tempFreq = cpm_freq_get();
	bool tempClockUp = (inCore > tempFreq.cclk);

	uintptr_t i, j, k, l;
	for(i = 1, k = 0; (i <= 32) && ((inCore / i) > inMem); i <<= 1, k = (k == 0 ? 1 : k + 2));
	for(j = 3, l = 2; (j <= 24) && ((inCore / j) > inMem); j <<= 1, l += 2);

	uintptr_t tempMemDiv  = (i < j ? k : l);
	uintptr_t tempMemFreq = (inCore / min(i, j));
	uintptr_t tempMemCycle = (65536000000000ULL / tempMemFreq);

	uintptr_t tempTRAS = (((uint64_t)SDRAM_TRAS << 32ULL) / tempMemCycle);
	uintptr_t tempRCD  = (((uint64_t)SDRAM_RCD  << 32ULL) / tempMemCycle);
	uintptr_t tempTPC  = (((uint64_t)SDRAM_TPC  << 32ULL) / tempMemCycle);
	uintptr_t tempTRWL = (((uint64_t)SDRAM_TRWL << 32ULL) / tempMemCycle);
	uintptr_t tempTRC  = (((uint64_t)SDRAM_TRC  << 32ULL) / tempMemCycle);
	uintptr_t tempTCL  = SDRAM_TCL(tempMemFreq);
	uintptr_t tempTREF = (((uint64_t)SDRAM_TREF << 32ULL) / tempMemCycle);

	tempTRAS = (tempTRAS + 0xFFFF) >> 16;
	tempRCD  = (tempRCD  + 0xFFFF) >> 16;
	tempTPC  = (tempTPC  + 0xFFFF) >> 16;
	tempTRWL = (tempTRWL + 0xFFFF) >> 16;
	tempTRC  = (tempTRC  + 0xFFFF) >> 16;
	tempTREF = (tempTREF + 0xFFFF) >> 16;

	tempTRAS = max(tempTRAS, 4);

	cpm_clk_ctrl_reg tempClkCtrl = *((cpm_clk_ctrl_reg*)CPM_CPCCR);

	tempClkCtrl.cdiv = 0;
	tempClkCtrl.mdiv = tempMemDiv;
	tempClkCtrl.hdiv = tempMemDiv;
	tempClkCtrl.pdiv = tempMemDiv;
	tempClkCtrl.ldiv = (min(i, j) - 1); // Set LCD device clock to bus clock.

	// TODO - Set remaining clock dividers correctly.

	((cpm_clk_ctrl_reg*)CPM_CPCCR)->ce = 0;
	*((cpm_clk_ctrl_reg*)CPM_CPCCR) = tempClkCtrl;

	//*((uint32_t*)CPM_LPCDR) = (((inCore + 8400000) / 16800000) - 1); // Set LCD pixel clock as close to 16800000 as possible.
	//*((uint32_t*)CPM_MSCCDR) = (((inCore + 14000000) / 28000000) - 1); // Set MSC device clock as close to 2800000 as possible.

	// TODO - Clock other units correctly.

	if(tempClockUp) emc_sdram_timings_set(tempTRAS, tempRCD, tempTPC, tempTRWL, tempTRC, tempTCL, tempTREF);

	cpm_pll_freq_set(tempM, tempN, tempD);

	if(!tempClockUp) emc_sdram_timings_set(tempTRAS, tempRCD, tempTPC, tempTRWL, tempTRC, tempTCL, tempTREF);

	return true;
}

void cpu_clock_get(uintptr_t* outCore, uintptr_t* outMem) {
	cpm_freq tempFreq = cpm_freq_get();
	if(outCore != NULL)
		*outCore = tempFreq.cclk;
	if(outMem != NULL)
		*outMem = tempFreq.mclk;
}
