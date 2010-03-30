#include "dma.h"
#include <dingoo/cache.h>
#include <dingoo/jz4740.h>
#include <dingoo/ucos2.h>

uintptr_t _dma_dest_width[3]   = { DMAC_DCMD_DWDH_8, DMAC_DCMD_DWDH_16, DMAC_DCMD_DWDH_32 };
uintptr_t _dma_src_width[3]    = { DMAC_DCMD_SWDH_8, DMAC_DCMD_SWDH_16, DMAC_DCMD_SWDH_32 };
uintptr_t _dma_block_width[6]  = { 0, 1, 2, 2, 4, 5 };
uintptr_t _dma_block_const[6]  = { DMAC_DCMD_DS_8BIT, DMAC_DCMD_DS_16BIT, DMAC_DCMD_DS_32BIT, DMAC_DCMD_DS_32BIT, DMAC_DCMD_DS_16BYTE, DMAC_DCMD_DS_32BYTE };



bool dma_wait(uintptr_t inChannel, uintptr_t inTimeout) {
	if(inChannel >= MAX_DMA_NUM)
		return false;
	uintptr_t i;
	for(i = 0; !(REG_DMAC_DCCSR(inChannel) & DMAC_DCCSR_TT); i++) { // Wait for transfer terminated bit
		OSTimeDly(1);
		if((inTimeout != 0) && (i >= inTimeout))
			return false;
	}
	return true;
}

bool dma_copy(uintptr_t inChannel, void* inDest, void* inSource, uintptr_t inSize) {
	if(inChannel >= MAX_DMA_NUM)
		return false;
	if(!(REG_DMAC_DCCSR(inChannel) & DMAC_DCCSR_TT))
		return false;
	if(inSize == 0)
		return true;

	uintptr_t i;

	if(inDest == (void*)SLCD_FIFO) {
		while(REG_SLCD_STATE & 0x01);
		REG_SLCD_CTRL = 0x01;
	}

	bool tempSourceFIFO = false;
	tempSourceFIFO |= (inSource == (void*)MSC_RXFIFO);
	tempSourceFIFO |= (inSource == (void*)CIM_RXFIFO);

	bool tempDestFIFO = false;
	tempDestFIFO |= (inDest == (void*)MSC_TXFIFO);
	tempDestFIFO |= (inDest == (void*)SLCD_FIFO);

	uintptr_t tempSourceMode   = (tempSourceFIFO ? 0 : DMAC_DCMD_SAI);
	uintptr_t tempDestMode     = (tempDestFIFO   ? 0 : DMAC_DCMD_DAI);
	uintptr_t tempTransferMode = DMAC_DCMD_TM;

	if(tempDestFIFO) {
		if(inDest == (void*)SLCD_FIFO)
			tempDestMode |= DMAC_DCMD_DWDH_16;
		else
			return false; // TODO - Setup dest mode for remaining FIFO destinations.
	} else {
		for(i = 1; (((uintptr_t)inDest & ((1 << i) - 1)) == 0) && (i < 3); i++);
		tempDestMode |= _dma_dest_width[i - 1];
	}

	if(tempSourceFIFO) {
		return false; // TODO - Setup source mode for FIFO sources.
	} else {
		for(i = 1; (((uintptr_t)inSource & ((1 << i) - 1)) == 0) && (i < 3); i++);
		tempSourceMode |= _dma_src_width[i - 1];
	}

	for(i = 1; ((inSize & ((1 << i) - 1)) == 0) && (i < 6); i++);
	if((inDest == (void*)SLCD_FIFO) && (i >= 6))
		i = 5;
	tempTransferMode |= _dma_block_const[i - 1];

	REG_DMAC_DCCSR(inChannel) = 0; // Disable DMA channel while configuring.

	// Set DMA into correct transfer mode.
	if(tempDestFIFO || tempSourceFIFO) {
		if(inDest == (void*)SLCD_FIFO) REG_DMAC_DRSR(inChannel) = DMAC_DRSR_RS_SLCD;
		else return false;
	} else REG_DMAC_DRSR(inChannel) = DMAC_DRSR_RS_AUTO;

	// Modify memory mapping for sources/destinations.
	inDest   = (void*)((uintptr_t)inDest   & 0x1FFFFFFF);
	inSource = (void*)((uintptr_t)inSource & 0x1FFFFFFF);

	REG_DMAC_DSAR(inChannel) = (uintptr_t)inSource;
	REG_DMAC_DTAR(inChannel) = (uintptr_t)inDest;
	REG_DMAC_DTCR(inChannel) = (inSize >> _dma_block_width[i - 1]);

	REG_DMAC_DCMD(inChannel) = tempSourceMode | tempDestMode | tempTransferMode;
	if(!tempSourceFIFO) __dcache_writeback_all();
	REG_DMAC_DCCSR(inChannel) |= DMAC_DCCSR_NDES; // No DMA descriptor used.
	REG_DMAC_DCCSR(inChannel) |= DMAC_DCCSR_EN; // Set enable bit to start DMA.

	return true;
}

bool dma_set(uintptr_t inChannel, void* inDest, uintptr_t inDestSize, void* inSource, uintptr_t inSourceSize) {
	if(inChannel >= MAX_DMA_NUM)
		return false;
	if(!(REG_DMAC_DCCSR(inChannel) & DMAC_DCCSR_TT))
		return false;
	if((inSourceSize == 0) || (inSourceSize == 3) || (inSourceSize > 4))
		return false;
	if(inDestSize == 0)
		return true;

	uintptr_t i;

	if(inDest == (void*)SLCD_FIFO) {
		while(REG_SLCD_STATE & 0x01);
		REG_SLCD_CTRL = 0x01;
	}

	bool tempSourceFIFO = false;
	tempSourceFIFO |= (inSource == (void*)MSC_RXFIFO);
	tempSourceFIFO |= (inSource == (void*)CIM_RXFIFO);

	bool tempDestFIFO = false;
	tempDestFIFO |= (inDest == (void*)MSC_TXFIFO);
	tempDestFIFO |= (inDest == (void*)SLCD_FIFO);

	uintptr_t tempSourceMode   = 0;
	uintptr_t tempDestMode     = (tempDestFIFO   ? 0 : DMAC_DCMD_DAI);
	uintptr_t tempTransferMode = DMAC_DCMD_TM;

	if(tempDestFIFO) {
		if(inDest == (void*)SLCD_FIFO)
			tempDestMode |= DMAC_DCMD_DWDH_16;
		else
			return false; // TODO - Setup dest mode for remaining FIFO destinations.
	} else {
		for(i = 1; (((uintptr_t)inDest & ((1 << i) - 1)) == 0) && (i < 3); i++);
		tempDestMode |= _dma_dest_width[i - 1];
	}

	if(tempSourceFIFO) {
		return false; // TODO - Setup source mode for FIFO sources.
	} else {
		switch(inSourceSize) {
			case 1:
				tempSourceMode |= _dma_src_width[0];
				break;
			case 2:
				tempSourceMode |= _dma_src_width[1];
				break;
			case 4:
				tempSourceMode |= _dma_src_width[2];
				break;
		}

	}

	for(i = 1; ((inDestSize & ((1 << i) - 1)) == 0) && (i < 6); i++);
	if((inDest == (void*)SLCD_FIFO) && (i >= 6))
		i = 5;
	tempTransferMode |= _dma_block_const[i - 1];

	REG_DMAC_DCCSR(inChannel) = 0; // Disable DMA channel while configuring.

	// Set DMA into correct transfer mode.
	if(tempDestFIFO || tempSourceFIFO) {
		if(inDest == (void*)SLCD_FIFO) REG_DMAC_DRSR(inChannel) = DMAC_DRSR_RS_SLCD;
		else return false;
	} else REG_DMAC_DRSR(inChannel) = DMAC_DRSR_RS_AUTO;

	// Modify memory mapping for sources/destinations within RAM.
	inDest   = (void*)((uintptr_t)inDest   & 0x1FFFFFFF);
	inSource = (void*)((uintptr_t)inSource & 0x1FFFFFFF);

	REG_DMAC_DSAR(inChannel) = (uintptr_t)inSource;
	REG_DMAC_DTAR(inChannel) = (uintptr_t)inDest;
	REG_DMAC_DTCR(inChannel) = (inDestSize >> _dma_block_width[i - 1]);

	REG_DMAC_DCMD(inChannel) = tempSourceMode | tempDestMode | tempTransferMode;
	if(!tempSourceFIFO) __dcache_writeback_all();
	REG_DMAC_DCCSR(inChannel) |= DMAC_DCCSR_NDES; // No DMA descriptor used.
	REG_DMAC_DCCSR(inChannel) |= DMAC_DCCSR_EN; // Set enable bit to start DMA.

	return true;
}
