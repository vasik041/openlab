
/* ADC1 DR register base address */
#define DR_ADDRESS                  ((uint32_t)0x4001244C)


void adc_init();
uint16_t adc_read(uint8_t ch);
void adc_read_n(uint8_t ch, uint16_t *p, uint16_t n);
void adc_read_n_dma(uint8_t ch, uint16_t *p, uint16_t n);
