/*
 * This is free and unencumbered software released into the public domain.
 */

/* ADC1 DR register base address */
#define DR_ADDRESS                  ((uint32_t)0x4001244C)


void adc_init();
uint16_t adc_read(uint8_t ch);
void adc_read_n(uint8_t ch, uint16_t *p, uint16_t n);

void dma_init(uint8_t ch, uint16_t *p, uint16_t n);
void adc_read_dma();

#define DMA_ENABLE	DMA1_Channel1->CCR |= DMA_CCR1_EN		/* Enable the selected DMAy Channelx */
#define DMA_DISABLE     DMA1_Channel1->CCR &= (uint16_t)(~DMA_CCR1_EN)	/* Disable the selected DMAy Channelx */
