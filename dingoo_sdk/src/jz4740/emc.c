#include "emc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <jz4740/jz4740.h>



#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif



volatile emc_sdram_ctrl_reg*     const emc_dmcr  = ((emc_sdram_ctrl_reg*)EMC_DMCR);
volatile emc_sdram_ref_stat_reg* const emc_rtcsr = ((emc_sdram_ref_stat_reg*)EMC_RTCSR);
volatile uint16_t*               const emc_rtcor = ((uint16_t*)EMC_RTCOR);
volatile uint16_t*               const emc_rtcnt = ((uint16_t*)EMC_RTCNT);



uintptr_t _emc_sdram_refresh_order[8] = { 0, 2, 4, 6, 8, 10, 11, 12 };



bool emc_sdram_refresh_set(uintptr_t inTREF) {
	emc_sdram_ref_stat_reg tempStat = *emc_rtcsr;

	if(inTREF == 0) {
		tempStat.cks = 0;
		emc_rtcsr->cks = tempStat.cks;
		if(emc_rtcsr->cks != tempStat.cks)
			return false;
		return true;
	}

	uintptr_t tempTREF = ((inTREF + ((1 << _emc_sdram_refresh_order[tempStat.cks]) - 1)) >> _emc_sdram_refresh_order[tempStat.cks]);
	tempTREF = (tempTREF == 0 ? 0 : (tempTREF - 1));
	bool tempValid = (tempTREF <= 0xFF);

	uintptr_t tempDiv;
	for(tempDiv = 1; ((inTREF >> _emc_sdram_refresh_order[tempDiv]) > 0xFF) && (tempDiv < 8); tempDiv++);

	if(tempDiv >= 8)
		return false;

	tempStat.cks = 0;
	emc_rtcsr->cks = tempStat.cks;
	if(emc_rtcsr->cks != tempStat.cks) {
		if(tempValid) {
			*emc_rtcor = tempTREF;
			*emc_rtcnt = *emc_rtcor;
			return true;
		}
		return false;
	}

	tempTREF = ((inTREF + ((1 << _emc_sdram_refresh_order[tempStat.cks]) - 1)) >> _emc_sdram_refresh_order[tempStat.cks]);
	tempTREF = (tempTREF == 0 ? 0 : (tempTREF - 1));
	*emc_rtcor = tempTREF;
	*emc_rtcnt = *emc_rtcor;

	return true;
}

uintptr_t emc_sdram_refresh_get() {
	emc_sdram_ref_stat_reg tempStat = *emc_rtcsr;
	if(tempStat.cks == 0)
		return 0;

	uintptr_t tempTREF = *emc_rtcor;
	tempTREF  += 1;
	tempTREF <<= _emc_sdram_refresh_order[tempStat.cks];
	return tempTREF;
}



emc_sdram_timings emc_sdram_timings_get() {
	emc_sdram_ctrl_reg tempCtrl = *emc_dmcr;
	emc_sdram_timings  tempOut;

	tempOut.tras = (tempCtrl.tras + 4);
	tempOut.rcd  = (tempCtrl.rcd  + 1);
	tempOut.tpc  = (tempCtrl.tpc  + 1);
	tempOut.trwl = (tempCtrl.trwl + 1);
	tempOut.trc  = ((tempCtrl.trc << 1) + 1);
	tempOut.tcl  = ((tempCtrl.tcl == 0 || tempCtrl.tcl == 3) ? 0 : (tempCtrl.tcl + 1));
	tempOut.tref = emc_sdram_refresh_get();

	return tempOut;
}

bool emc_sdram_timings_set(emc_sdram_timings inTimings) {
	if((inTimings.tras < 4) || (inTimings.tras > 11))
		return false;
	if((inTimings.rcd == 0) || (inTimings.rcd > 4))
		return false;
	if((inTimings.tpc == 0) || (inTimings.tpc > 8))
		return false;
	if((inTimings.trwl == 0) || (inTimings.trwl > 4))
		return false;
	if(((inTimings.trc & 1) == 0) || (inTimings.trc > 15))
		return false;
	if((inTimings.tcl == 1) || (inTimings.tcl > 3))
		return false;

	emc_sdram_ctrl_reg tempCtrl = *emc_dmcr;

	tempCtrl.tras = (inTimings.tras - 4);
	tempCtrl.rcd  = (inTimings.rcd  - 1);
	tempCtrl.tpc  = (inTimings.tpc  - 1);
	tempCtrl.trwl = (inTimings.trwl - 1);
	tempCtrl.trc  = (inTimings.trc >> 1);
	tempCtrl.tcl  = (inTimings.tcl == 0 ? 0 : (inTimings.tcl - 1));

	emc_sdram_ctrl_reg tempMask;
	tempMask.mask = 0;

	tempMask.tras = 7;
	tempMask.rcd  = 3;
	tempMask.tpc  = 7;
	tempMask.trwl = 3;
	tempMask.trc  = 7;
	tempMask.tcl  = 3;

	// TODO - Work out why DMCR is read-only and write it properly.
	*emc_dmcr = tempCtrl;
	if((emc_dmcr->mask & tempMask.mask) != (tempCtrl.mask & tempMask.mask))
		return false;

	emc_sdram_refresh_set(inTimings.tref);

	return true;
}

bool emc_sdram_timings_safe(emc_sdram_timings inTimings) {
	emc_sdram_timings tempTimings = emc_sdram_timings_get();
	if(inTimings.tras > tempTimings.tras)
		return false;
	if(inTimings.rcd > tempTimings.rcd)
		return false;
	if(inTimings.tpc > tempTimings.tpc)
		return false;
	if(inTimings.trwl > tempTimings.trwl)
		return false;
	if(inTimings.trc > tempTimings.trc)
		return false;
	if(inTimings.tcl > tempTimings.tcl)
		return false;
	if(inTimings.tref < tempTimings.tref)
		return false;
	return true;
}

bool emc_sdram_timings_dump(const char* inPath, uintptr_t inFreq) {
	emc_sdram_timings tempTimingsCycles = emc_sdram_timings_get();

	inFreq /= 1000;
	if(inFreq == 0)
		inFreq = 1;

	emc_sdram_timings tempTimingsTime;
	tempTimingsTime.tras = ((tempTimingsCycles.tras * 1000000) / inFreq);
	tempTimingsTime.rcd  = ((tempTimingsCycles.rcd  * 1000000) / inFreq);
	tempTimingsTime.tpc  = ((tempTimingsCycles.tpc  * 1000000) / inFreq);
	tempTimingsTime.trwl = ((tempTimingsCycles.trwl * 1000000) / inFreq);
	tempTimingsTime.trc  = ((tempTimingsCycles.trc  * 1000000) / inFreq);
	tempTimingsTime.tcl  = ((tempTimingsCycles.tcl  * 1000000) / inFreq);
	tempTimingsTime.tref = ((tempTimingsCycles.tref * 1000000) / inFreq);

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	char tempString[256];

	sprintf(tempString, "tras: RAS Assertion Time:   %lu cycles (%lu ns)\n", tempTimingsCycles.tras, tempTimingsTime.tras);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "rcd:  RAS-CAS Delay:        %lu cycles (%lu ns)\n", tempTimingsCycles.rcd, tempTimingsTime.rcd);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "tpc:  RAS Precharge Time:   %lu cycles (%lu ns)\n", tempTimingsCycles.tpc, tempTimingsTime.tpc);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "trwl: Write Precharge Time: %lu cycles (%lu ns)\n", tempTimingsCycles.trwl, tempTimingsTime.trwl);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "trc:  RAS Cycle Time:       %lu cycles (%lu ns)\n", tempTimingsCycles.trc, tempTimingsTime.trc);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "tcl:  CAS Latency:          %lu cycles (%lu ns)\n", tempTimingsCycles.tcl, tempTimingsTime.tcl);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "tref: Refresh Time:         %lu cycles (%lu ns)\n", tempTimingsCycles.tref, tempTimingsTime.tref);
	fwrite(tempString, 1, strlen(tempString), tempFile);

	fclose(tempFile);
	return true;
}



bool emc_sdram_ctrl_reg_dump(const char* inPath) {
	emc_sdram_ctrl_reg tempCtrl = *emc_dmcr;

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	char tempString[256];

	sprintf(tempString, "Bus Width:            %lu\n", (tempCtrl.bus_width ? 32 : 16));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	if(tempCtrl.col_addr_width > 4)
		sprintf(tempString, "Column Address Width: Reserved\n");
	else
		sprintf(tempString, "Column Address Width: %lu\n", (tempCtrl.col_addr_width + 8));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Refresh Mode:         %s\n", (tempCtrl.refresh_mode ? "Self" : "Auto"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Refresh Control:      %s\n", (tempCtrl.refresh_control ? "Refresh" : "None"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Mode Register Set:    %s\n", (tempCtrl.mode_reg_set ? "Mode Register" : "All-Bank Pre-Charge"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	if(tempCtrl.row_addr_width >= 3)
		sprintf(tempString, "Row Address Width:    Reserved\n");
	else
		sprintf(tempString, "Row Address Width:    %lu\n", (tempCtrl.row_addr_width + 11));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Bank Address Width:   %lu\n", (tempCtrl.bank_addr_width + 1));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Power Down Mode:      %s\n", (tempCtrl.pow_down_mode ? "On" : "Off"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "CKE Pin Control:      %s\n", (tempCtrl.cke_pin_ctrl ? "High" : "Low"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "RAS Assertion Time:   %lu cycles\n", (tempCtrl.tras + 4));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "RAS-CAS Delay:        %lu cycles\n", (tempCtrl.rcd + 1));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "RAS Precharge Time:   %lu cycles\n", (tempCtrl.tpc + 1));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Write Precharge Time: %lu cycles\n", (tempCtrl.trwl + 1));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "RAS Cycle Time:       %lu cycles\n", ((tempCtrl.trc << 1) + 1));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	if((tempCtrl.tcl == 0) || (tempCtrl.tcl >= 3))
		sprintf(tempString, "CAS Latency:          Inhibit%s\n", (tempCtrl.tcl == 0 ? " (Initial)" : ""));
	else
		sprintf(tempString, "CAS Latency:          %lu cycles\n", (tempCtrl.tcl + 1));
	fwrite(tempString, 1, strlen(tempString), tempFile);

	fclose(tempFile);
	return true;
}
