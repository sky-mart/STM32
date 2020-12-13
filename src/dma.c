#include "dma.h"

void dma_channel_init(DMA_Channel_TypeDef* dma_channel, const dma_transfer_t* dma_transfer)
{
	dma_channel->CCR &= ~DMA_CCR_EN;
	dma_channel->CPAR = (uint32_t)dma_transfer->periph_addr;
	dma_channel->CMAR = (uint32_t)dma_transfer->memory_addr;
	dma_channel->CNDTR = dma_transfer->count;
	dma_channel->CCR = *(uint32_t*)&dma_transfer->config;
	dma_channel->CCR |= DMA_CCR_EN;
}