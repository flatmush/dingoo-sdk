#include "cpm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <jz4740/jz4740.h>



cpm_freq cpm_freq_get() {
	cpm_freq tempFreq;
	cpm_clk_ctrl_reg tempClkCtrl = *((cpm_clk_ctrl_reg*)CPM_CPCCR);

	tempFreq.pll = cpm_pll_freq_get();

	uintptr_t tempDiv[16] = {
		 1,  2,  3,  4,
		 6,  8, 12, 16,
		24, 32,  1,  1,
		 1,  1,  1,  1
		};

	tempFreq.pclk   = tempFreq.pll / tempDiv[tempClkCtrl.pdiv];
	tempFreq.mclk   = tempFreq.pll / tempDiv[tempClkCtrl.mdiv];
	tempFreq.hclk   = tempFreq.pll / tempDiv[tempClkCtrl.hdiv];
	tempFreq.cclk   = tempFreq.pll / tempDiv[tempClkCtrl.cdiv];

	tempFreq.cko    = (tempClkCtrl.clkoen ? tempFreq.mclk : 0);

	tempFreq.ldclk  = ((tempFreq.pll >> (1 - tempClkCtrl.pcs)) / (tempClkCtrl.ldiv + 1));
	tempFreq.lpclk  = ((tempFreq.pll >> (1 - tempClkCtrl.pcs)) / ((*((uint32_t*)CPM_LPCDR) & 0x000001FF) + 1));

	tempFreq.usbclk = (tempClkCtrl.ucs ? ((tempFreq.pll >> (1 - tempClkCtrl.pcs)) / (tempClkCtrl.udiv + 1)) : EXCLK_FREQ);
	// TODO - Get other USB frequencies.

	tempFreq.mscclk = ((tempFreq.pll >> (1 - tempClkCtrl.pcs)) / (*((uint32_t*)CPM_MSCCDR) & 0x0000001F));

	tempFreq.i2sclk = (tempClkCtrl.i2cs ? ((tempFreq.pll >> (1 - tempClkCtrl.pcs)) / ((*((uint32_t*)CPM_I2SCDR) & 0x000001FF) + 1)) : EXCLK_FREQ);

	// TODO - Get other frequencies.

	return tempFreq;
}

bool cpm_freq_set(cpm_freq inFreq) {
	if(inFreq.mclk % inFreq.pclk)
		return false;
	if((inFreq.hclk != inFreq.mclk) && (inFreq.hclk != (inFreq.mclk << 1)))
		return false;
	if(inFreq.cclk % inFreq.hclk)
		return false;

	if(inFreq.ldclk > 150000000)
		return false;

	uintptr_t tempM, tempN, tempD;
	if(!cpm_pll_freq_calc(inFreq.cclk, &tempM, &tempN, &tempD))
		return false;

	uintptr_t tempLookup[33];
	uintptr_t i;
	for(i = 0; i <= 32; i++)
		tempLookup[i] = 0xFFFFFFFF;
	tempLookup[1] = 0;
	tempLookup[2] = 1;
	tempLookup[3] = 2;
	tempLookup[4] = 3;
	tempLookup[6] = 4;
	tempLookup[8] = 5;
	tempLookup[12] = 6;
	tempLookup[16] = 7;
	tempLookup[24] = 8;
	tempLookup[32] = 9;

	uintptr_t tempCDiv, tempMDiv, tempHDiv, tempPDiv;

	tempCDiv = 0;

	tempMDiv = (inFreq.cclk / inFreq.mclk);
	if(tempMDiv > 32) return false;
	tempMDiv = tempLookup[inFreq.cclk / inFreq.mclk];
	if(tempMDiv > 9) return false;

	tempHDiv = (inFreq.cclk / inFreq.hclk);
	if(tempHDiv > 32) return false;
	tempHDiv = tempLookup[inFreq.cclk / inFreq.hclk];
	if(tempHDiv > 9) return false;

	tempPDiv = (inFreq.cclk / inFreq.pclk);
	if(tempPDiv > 32) return false;
	tempPDiv = tempLookup[inFreq.cclk / inFreq.pclk];
	if(tempPDiv > 9) return false;

	((cpm_clk_ctrl_reg*)CPM_CPCCR)->ce = 0;

	cpm_clk_ctrl_reg tempClkCtrl = *((cpm_clk_ctrl_reg*)CPM_CPCCR);

	tempClkCtrl.cdiv = tempCDiv;
	tempClkCtrl.mdiv = tempMDiv;
	tempClkCtrl.hdiv = tempHDiv;
	tempClkCtrl.pdiv = tempPDiv;

	tempClkCtrl.ldiv = (inFreq.cclk / inFreq.ldclk) - 1;

	// TODO - Set other clocks correctly.

	*((cpm_clk_ctrl_reg*)CPM_CPCCR) = tempClkCtrl;

	cpm_pll_freq_set(tempM, tempN, tempD);

	return false;
}

bool cpm_freq_dump(const char* inPath) {
	cpm_freq tempCpmFreq = cpm_freq_get();

	char tempString[256];

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	sprintf(tempString, "PLL Freq.:        %lu Hz\n", tempCpmFreq.pll);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Core Clock:       %lu Hz\n", tempCpmFreq.cclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "System Clock:     %lu Hz\n", tempCpmFreq.hclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Memory Clock:     %lu Hz\n", tempCpmFreq.mclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Peripheral Clock: %lu Hz\n", tempCpmFreq.pclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "SDRAM Clock:      %lu Hz\n", tempCpmFreq.cko);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "LCD Device Clock: %lu Hz\n", tempCpmFreq.ldclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "LCD Pixel Clock:  %lu Hz\n", tempCpmFreq.lpclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "USB Source Clock: %lu Hz\n", tempCpmFreq.usbclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "MSC Clock:        %lu Hz\n", tempCpmFreq.mscclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "I2S Clock:        %lu Hz\n", tempCpmFreq.i2sclk);
	fwrite(tempString, 1, strlen(tempString), tempFile);

	fclose(tempFile);
	return true;
}



uintptr_t cpm_pll_freq_get() {
	cpm_pll_ctrl_reg tempCtrl = *((cpm_pll_ctrl_reg*)CPM_CPPCR);

	uintptr_t tempFreq = EXCLK_FREQ;

	if(tempCtrl.enable && !tempCtrl.bypass) {
		tempFreq *= (tempCtrl.m + 2);
		tempFreq /= (tempCtrl.n + 2);
		tempFreq >>= (tempCtrl.output_div > 0 ? (tempCtrl.output_div == 3 ? 2 : 1) : 0);
	}

	return (uintptr_t)tempFreq;
}

bool cpm_pll_freq_calc(uintptr_t inFreq, uintptr_t* outM, uintptr_t* outN, uintptr_t* outDiv) {
	uintptr_t tempNew = EXCLK_FREQ;
	intptr_t  tempNewDiff = ((inFreq > tempNew) ? (inFreq - tempNew) : (tempNew - inFreq));

	uintptr_t tempCurM = 0;
	uintptr_t tempCurN = 0;
	uintptr_t tempCurD = 0;
	intptr_t  tempCurDiff = tempNewDiff;

	uintptr_t n, m, d;
	for(d = 0; (d < 3) && (tempCurDiff != 0); d += 1) {
		for(n = 0; (n < 32) && (tempCurDiff != 0); n++) {
			for(m = 0; (m < 512) && (tempCurDiff != 0); m++) {
				tempNew = ((EXCLK_FREQ * (m + 2)) / (n + 2)) >> d;
				tempNewDiff = ((inFreq > tempNew) ? (inFreq - tempNew) : (tempNew - inFreq));
				if(tempNewDiff < tempCurDiff) {
					tempCurM = m;
					tempCurN = n;
					tempCurD = (d + (d >> 1));
					tempCurDiff = tempNewDiff;
				}
			}
		}
	}

	if(tempCurDiff != 0)
		return false;

	if(outM != NULL)
		*outM   = tempCurM;
	if(outN != NULL)
		*outN   = tempCurN;
	if(outDiv != NULL)
		*outDiv = tempCurD;

	return true;
}

bool cpm_pll_freq_set(uintptr_t inM, uintptr_t inN, uintptr_t inD) {
	if((inM >= 512) || (inN >= 32) || (inD > 3))
		return false;
	inD += (inD >> 1);

	bool tempNoDiv = ((inM + 2) == ((inN + 2) << inD));

	if(tempNoDiv) {
		((cpm_pll_ctrl_reg*)CPM_CPPCR)->enable = 0;
	} else {
		//((cpm_pll_ctrl_reg*)CPM_CPPCR)->bypass = 1;
		//((cpm_pll_ctrl_reg*)CPM_CPPCR)->enable = 0;

		cpm_pll_ctrl_reg tempCtrl = *((cpm_pll_ctrl_reg*)CPM_CPPCR);
		tempCtrl.n          = inN;
		tempCtrl.m          = inM;
		tempCtrl.output_div = inD;
		tempCtrl.enable     = 1;
		*((cpm_pll_ctrl_reg*)CPM_CPPCR) = tempCtrl;

		//while(((cpm_pll_ctrl_reg*)CPM_CPPCR)->stable == 0);

		//((cpm_pll_ctrl_reg*)CPM_CPPCR)->bypass = 0;
	}

	return true;
}



bool cpm_pll_ctrl_dump(const char* inPath) {
	cpm_pll_ctrl_reg tempPllCtrl = *((cpm_pll_ctrl_reg*)CPM_CPPCR);

	char tempString[256];

	FILE* tempFile = fopen(inPath, "wb");
	if(tempFile == NULL)
		return false;

	sprintf(tempString, "Feedback Divider: %lu\n", (tempPllCtrl.m + 2));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Input Divider:    %lu\n", (tempPllCtrl.n + 2));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Output Divider    %lu\n", (tempPllCtrl.output_div == 2 ? 2 : (tempPllCtrl.output_div + 1)));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Stable:           %s\n", (tempPllCtrl.stable ? "Stable" : "Unstable"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Bypass:           %s\n", (tempPllCtrl.bypass ? "Bypass" : "Use"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Enable:           %s\n", (tempPllCtrl.enable ? "Enable" : "Disable"));
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Stable Time:      %lu\n", tempPllCtrl.stable_time);
	fwrite(tempString, 1, strlen(tempString), tempFile);
	sprintf(tempString, "Output Freq.:     %lu\n", cpm_pll_freq_get());
	fwrite(tempString, 1, strlen(tempString), tempFile);

	fclose(tempFile);
	return true;
}
