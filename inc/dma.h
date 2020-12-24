#ifndef DMA_H
#define DMA_H

#include "stm32f3xx.h"

typedef struct {
	uint32_t EN : 1;
	uint32_t TCIE : 1;
	uint32_t HTIE : 1;
	uint32_t TEIE : 1;
	uint32_t DIR : 1;		// read from memory
	uint32_t CIRC : 1;
	uint32_t PINC : 1;
	uint32_t MINC : 1;
	uint32_t PSIZE : 2;
	uint32_t MSIZE : 2;
	uint32_t PL : 2;
	uint32_t MEM2MEM: 1;
	uint32_t reserved: 17;
} dma_ccr_t;

typedef struct {
	void* periph_addr;
	void* memory_addr;
	uint32_t count;
	dma_ccr_t config;
} dma_transfer_t;

void dma_channel_init(DMA_Channel_TypeDef* dma_channel, const dma_transfer_t* dma_transfer);

#endif // DMA_H