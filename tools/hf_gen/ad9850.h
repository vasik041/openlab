/*=========================================================================*/
/**
 * AD9850 serial driver
 * This is free and unencumbered software released into the public domain.
 * LZ,2013
 */
/*=========================================================================*/

#define	SDATA 2 //PA2
#define	W_CLK 1	//PA1
#define	FQ_UD 0 //PA0

#define	RESET 1 //PB1

void ad9850_reset();
void ad9850_wr(uint8_t w0,uint32_t freq);




