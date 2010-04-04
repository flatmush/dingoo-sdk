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



bool emc_sdram_timings_set(uintptr_t inTRAS, uintptr_t inRCD, uintptr_t inTPC, uintptr_t inTRWL, uintptr_t inTRC, uintptr_t inTCL, uintptr_t inTREF) {
	if((inTRAS < 4) || (inTRAS > 11))
		return false;
	if((inRCD == 0) || (inRCD > 4))
		return false;
	if((inTPC == 0) || (inTPC > 8))
		return false;
	if((inTRWL == 0) || (inTRWL > 4))
		return false;
	if(((inTRC & 1) == 0) || (inTRC > 15))
		return false;
	if((inTCL == 1) || (inTCL > 3))
		return false;

	uintptr_t tempTREF = max(((inTREF + 63) >> 6), 0xFF);

	emc_sdram_ctrl_reg tempCtrl = *((emc_sdram_ctrl_reg*)EMC_DMCR);

	tempCtrl.tras = (inTRAS - 4);
	tempCtrl.rcd  = (inRCD - 1);
	tempCtrl.tpc  = (inTPC - 1);
	tempCtrl.trwl = (inTRWL - 1);
	tempCtrl.trc  = ((inTRC - 1) >> 1);
	tempCtrl.tcl  = (inTCL == 0 ? 0 : (inTCL - 1));

	*((emc_sdram_ctrl_reg*)EMC_DMCR) = tempCtrl;
	if(*((uint32_t*)EMC_DMCR) != *((uint32_t*)&tempCtrl))
		return false;

	// TODO - Allow for different refresh resolutions.
	*((uint16_t*)EMC_RTCOR) = tempTREF;
	*((uint16_t*)EMC_RTCNT) = tempTREF;

	return true;
}

bool emc_sdram_timings_dump(const char* inPath) {
	emc_sdram_ctrl_reg tempCtrl = *((emc_sdram_ctrl_reg*)EMC_DMCR);

	char tempString[256];

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

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
