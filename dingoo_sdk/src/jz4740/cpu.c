#include "cpu.h"

#include <stdlib.h>

#include <jz4740/cpm.h>
#include <jz4740/emc.h>
#include <jz4740/sdram.h>



#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif



bool cpu_clock_set(uintptr_t inCore) {
	uintptr_t tempM, tempN, tempD;
	if(!cpm_pll_freq_calc(inCore, &tempM, &tempN, &tempD))
		return false;

	uintptr_t i, j, k, l;
	for(i = 1, k = 0; (i <= 32) && ((inCore / i) > SDRAM_FREQ); i <<= 1, k = (k == 0 ? 1 : k + 2));
	for(j = 3, l = 2; (j <= 24) && ((inCore / j) > SDRAM_FREQ); j <<= 1, l += 2);

	uintptr_t tempMemDiv  = (i < j ? k : l);
	uintptr_t tempMemFreq = (inCore / min(i, j));
	uintptr_t tempMemCycle = (65536000000000ULL / tempMemFreq);

	cpm_freq tempFreq   = cpm_freq_get();
	bool tempMemClockUp = (tempMemFreq > tempFreq.mclk);

	uintptr_t tempPDiv = (tempMemFreq > 150000000 ? (tempMemDiv == 0 ? 1 : (tempMemDiv + 2)) : tempMemDiv);


	emc_sdram_timings tempTimings;
	tempTimings.tras = (((uint64_t)SDRAM_TRAS << 32ULL) / tempMemCycle);
	tempTimings.rcd  = (((uint64_t)SDRAM_RCD  << 32ULL) / tempMemCycle);
	tempTimings.tpc  = (((uint64_t)SDRAM_TPC  << 32ULL) / tempMemCycle);
	tempTimings.trwl = (((uint64_t)SDRAM_TRWL << 32ULL) / tempMemCycle);
	tempTimings.trc  = (((uint64_t)SDRAM_TRC  << 32ULL) / tempMemCycle);
	tempTimings.tcl  = SDRAM_TCL(tempMemFreq);
	tempTimings.tref = (((uint64_t)SDRAM_TREF << 32ULL) / tempMemCycle);

	tempTimings.tras = (tempTimings.tras + 0xFFFF) >> 16;
	tempTimings.rcd  = (tempTimings.rcd  + 0xFFFF) >> 16;
	tempTimings.tpc  = (tempTimings.tpc  + 0xFFFF) >> 16;
	tempTimings.trwl = (tempTimings.trwl + 0xFFFF) >> 16;
	tempTimings.trc  = (tempTimings.trc  + 0xFFFF) >> 16;
	tempTimings.tref = (tempTimings.tref + 0xFFFF) >> 16;

	tempTimings.tras = max(tempTimings.tras, 4);
	tempTimings.rcd  = max(tempTimings.rcd , 1);
	tempTimings.tpc  = max(tempTimings.tpc , 1);
	tempTimings.trwl = max(tempTimings.trwl, 1);
	tempTimings.trc += (~tempTimings.trc & 1);
	tempTimings.tcl  = max(tempTimings.tcl , 3);


	cpm_clk_ctrl_reg tempClkCtrl = *cpm_cpccr;

	tempClkCtrl.cdiv = 0;
	tempClkCtrl.mdiv = tempMemDiv;
	tempClkCtrl.hdiv = tempMemDiv;
	tempClkCtrl.pdiv = tempPDiv;
	tempClkCtrl.ldiv = (min(i, j) - 1); // Set LCD device clock to bus clock.

	cpm_cpccr->ce = 0;
	*cpm_cpccr = tempClkCtrl;

	cpm_clkgr->msc = 1;
	cpm_clkgr->lcd = 1;

	*cpm_msccdr = (((inCore + 14000000) / 28000000) - 1);

	if(tempMemClockUp) {
		emc_sdram_timings_set(tempTimings);
		emc_sdram_refresh_set(tempTimings.tref);
	}

	cpm_pll_freq_set(tempM, tempN, tempD);

	if(!tempMemClockUp) {
		emc_sdram_timings_set(tempTimings);
		emc_sdram_refresh_set(tempTimings.tref);
	}

	cpm_clkgr->lcd = 0;
	cpm_clkgr->msc = 0;

	return true;
}

/*bool cpu_clock_set(uintptr_t inCore, uintptr_t inMem) {
	uintptr_t tempM, tempN, tempD;
	if(!cpm_pll_freq_calc(inCore, &tempM, &tempN, &tempD))
		return false;

	if(inMem > SDRAM_FREQ)
		return false;
	if(inMem < (inCore >> 5))
		return false;

	cpm_clk_gate_reg tempClockGate    = *cpm_clkgr;
	cpm_clk_gate_reg tempClockGateOld = tempClockGate;

	tempClockGate.msc = 1;
	tempClockGate.lcd = 1;

	*cpm_clkgr = tempClockGate;


	cpm_freq tempFreq = cpm_freq_get();
	bool tempClockUp = (inCore > tempFreq.cclk);


	uintptr_t i, j, k, l;
	for(i = 1, k = 0; (i <= 32) && ((inCore / i) > inMem); i <<= 1, k = (k == 0 ? 1 : k + 2));
	for(j = 3, l = 2; (j <= 24) && ((inCore / j) > inMem); j <<= 1, l += 2);

	uintptr_t tempMemDiv  = (i < j ? k : l);
	uintptr_t tempMemFreq = (inCore / min(i, j));
	uintptr_t tempMemCycle = (65536000000000ULL / tempMemFreq);

	uintptr_t tempPDiv = (tempMemFreq > 150000000 ? (tempMemDiv == 0 ? 1 : (tempMemDiv + 2)) : tempMemDiv);


	emc_sdram_timings tempTimings;
	tempTimings.tras = (((uint64_t)SDRAM_TRAS << 32ULL) / tempMemCycle);
	tempTimings.rcd  = (((uint64_t)SDRAM_RCD  << 32ULL) / tempMemCycle);
	tempTimings.tpc  = (((uint64_t)SDRAM_TPC  << 32ULL) / tempMemCycle);
	tempTimings.trwl = (((uint64_t)SDRAM_TRWL << 32ULL) / tempMemCycle);
	tempTimings.trc  = (((uint64_t)SDRAM_TRC  << 32ULL) / tempMemCycle);
	tempTimings.tcl  = SDRAM_TCL(tempMemFreq);
	tempTimings.tref = (((uint64_t)SDRAM_TREF << 32ULL) / tempMemCycle);

	tempTimings.tras = (tempTimings.tras + 0xFFFF) >> 16;
	tempTimings.rcd  = (tempTimings.rcd  + 0xFFFF) >> 16;
	tempTimings.tpc  = (tempTimings.tpc  + 0xFFFF) >> 16;
	tempTimings.trwl = (tempTimings.trwl + 0xFFFF) >> 16;
	tempTimings.trc  = (tempTimings.trc  + 0xFFFF) >> 16;
	tempTimings.tref = (tempTimings.tref + 0xFFFF) >> 16;

	tempTimings.tras = max(tempTimings.tras, 4);
	tempTimings.rcd  = max(tempTimings.rcd , 1);
	tempTimings.tpc  = max(tempTimings.tpc , 1);
	tempTimings.trwl = max(tempTimings.trwl, 1);
	tempTimings.trc += (~tempTimings.trc & 1);
	tempTimings.tcl  = max(tempTimings.tcl , 3);


	cpm_clk_ctrl_reg tempClkCtrl = *cpm_cpccr;
	cpm_clk_ctrl_reg tempClkCtrlOld = tempClkCtrl;

	tempClkCtrl.cdiv = 0;
	tempClkCtrl.mdiv = tempMemDiv;
	tempClkCtrl.hdiv = tempMemDiv;
	tempClkCtrl.pdiv = tempPDiv;
	tempClkCtrl.ldiv = (min(i, j) - 1); // Set LCD device clock to bus clock.

	// TODO - Set remaining clock dividers correctly.

	cpm_cpccr->ce = 0;
	*cpm_cpccr = tempClkCtrl;

	//*cpm_lpcdr = (((inCore + 8400000) / 16800000) - 1); // Set LCD pixel clock as close to 16800000 as possible.
	*cpm_msccdr = (((inCore + 14000000) / 28000000) - 1); // Set MSC device clock as close to 2800000 as possible.

	// TODO - Clock other units correctly.

	if(tempClockUp && !emc_sdram_timings_set(tempTimings) && !emc_sdram_timings_safe(tempTimings)) {
		cpm_cpccr->ce = 0;
		*cpm_cpccr    = tempClkCtrlOld;
		*cpm_clkgr    = tempClockGateOld;
		return false;
	}

	if(!cpm_pll_freq_set(tempM, tempN, tempD)) {
		cpm_cpccr->ce = 0;
		*cpm_cpccr    = tempClkCtrlOld;
		*cpm_clkgr    = tempClockGateOld;
		return false;
	}

	*cpm_clkgr = tempClockGateOld;

	if(!tempClockUp && !emc_sdram_timings_set(tempTimings) && !emc_sdram_timings_safe(tempTimings)) {
		cpm_cpccr->ce = 0;
		*cpm_cpccr    = tempClkCtrlOld;
		cpm_pll_freq_calc(tempFreq.pll, &tempM, &tempN, &tempD);
		cpm_pll_freq_set(tempM, tempN, tempD);
		return false;
	}

	return true;
}*/

void cpu_clock_get(uintptr_t* outCore, uintptr_t* outMem) {
	cpm_freq tempFreq = cpm_freq_get();
	if(outCore != NULL)
		*outCore = tempFreq.cclk;
	if(outMem != NULL)
		*outMem = tempFreq.mclk;
}
