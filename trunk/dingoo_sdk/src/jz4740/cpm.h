#ifndef __jz4740_cpm_h__
#define __jz4740_cpm_h__

#include <stdbool.h>
#include <stdint.h>



typedef struct {
	unsigned int cdiv   : 4;
	unsigned int hdiv   : 4;
	unsigned int pdiv   : 4;
	unsigned int mdiv   : 4;
	unsigned int ldiv   : 5;
	unsigned int pcs    : 1;
	unsigned int ce     : 1;
	unsigned int udiv   : 6;
	unsigned int ucs    : 1;
	unsigned int clkoen : 1;
	unsigned int i2cs   : 1;
} __attribute__((__packed__)) cpm_clk_ctrl_reg;

typedef struct {
	unsigned int uart0    : 1;
	unsigned int tcu      : 1;
	unsigned int rtc      : 1;
	unsigned int i2c      : 1;
	unsigned int ssi      : 1;
	unsigned int aic_pclk : 1;
	unsigned int aic_bclk : 1;
	unsigned int msc      : 1;
	unsigned int sadc     : 1;
	unsigned int cim      : 1;
	unsigned int lcd      : 1;
	unsigned int udc      : 1;
	unsigned int dmac     : 1;
	unsigned int ipu      : 1;
	unsigned int uhc      : 1;
	unsigned int uart1    : 1;
	unsigned int reserved : 16;
} __attribute__((__packed__)) cpm_clk_gate_reg;

typedef struct {
	unsigned int stable_time : 8;
	unsigned int enable      : 1;
	unsigned int bypass      : 1;
	unsigned int stable      : 1;
	unsigned int reserved    : 5;
	unsigned int output_div  : 2;
	unsigned int n           : 5;
	unsigned int m           : 9;
} __attribute__((__packed__)) cpm_pll_ctrl_reg;

typedef struct {
	uintptr_t pll;
	uintptr_t pclk, mclk, hclk, cclk;
	uintptr_t cko;
	uintptr_t ldclk, lpclk;
	uintptr_t usbclk, i2sclk, mscclk;
} cpm_freq;



extern volatile cpm_clk_ctrl_reg* const cpm_cpccr;
extern volatile cpm_clk_gate_reg* const cpm_clkgr;
extern volatile cpm_pll_ctrl_reg* const cpm_cppcr;
extern volatile uint32_t*         const cpm_lpcdr;
extern volatile uint32_t*         const cpm_msccdr;
extern volatile uint32_t*         const cpm_i2scdr;



#define EXCLK_FREQ 12000000



extern cpm_freq  cpm_freq_get();
extern bool      cpm_freq_set(cpm_freq inFreq);
extern bool      cpm_freq_dump(const char* inPath);

extern uintptr_t cpm_pll_freq_get();
extern bool      cpm_pll_freq_calc(uintptr_t inFreq, uintptr_t* outM, uintptr_t* outN, uintptr_t* outDiv);
extern bool      cpm_pll_freq_set(uintptr_t inM, uintptr_t inN, uintptr_t inD);

extern bool      cpm_pll_ctrl_dump(const char* inPath);

#endif
