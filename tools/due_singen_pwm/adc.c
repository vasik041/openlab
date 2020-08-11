
#include "include/due_sam3x.h"
#include "adc.h"

/**
 * \brief Initialize the given ADC with the specified ADC clock and startup time.
 *
 * \param p_adc Pointer to an ADC instance.
 * \param ul_mck Main clock of the device (value in Hz).
 * \param ul_adc_clock Analog-to-Digital conversion clock (value in Hz).
 * \param uc_startup ADC start up time. Please refer to the product datasheet
 * for details.
 *
 * \return 0 on success.
 */
uint32_t adc_init2(Adc *p_adc, const uint32_t ul_mck, const uint32_t ul_adc_clock, const uint8_t uc_startuptime)
{
    uint32_t startup_table[] = { 0, 8, 16, 24, 64, 80, 96, 112, 512, 576, 640, 704, 768, 832, 896, 960 };
    uint32_t ul_prescal, ul_startup,  ul_mr_startup, ul_real_adc_clock;

    p_adc->ADC_CR = ADC_CR_SWRST;
    p_adc->ADC_MR = 0; /* Reset Mode Register. */
    
    p_adc->ADC_PTCR = (ADC_PTCR_RXTDIS | ADC_PTCR_TXTDIS); /* Reset PDC transfer. */
    p_adc->ADC_RCR = 0;
    p_adc->ADC_RNCR = 0;

    if (ul_mck % (2 * ul_adc_clock)) {
	// Division with reminder
	ul_prescal = ul_mck / (2 * ul_adc_clock);
    } else {
	// Whole division
	ul_prescal = ul_mck / (2 * ul_adc_clock) - 1;
    }
    ul_real_adc_clock = ul_mck / (2 * (ul_prescal + 1));

    // ADC clocks needed to get ul_startuptime uS
    ul_startup = (ul_real_adc_clock / 1000000) * uc_startuptime;

    // Find correct MR_STARTUP value from conversion table
    for (ul_mr_startup=0; ul_mr_startup<16; ul_mr_startup++) {
	if (startup_table[ul_mr_startup] >= ul_startup)
	    break;
    }
    if (ul_mr_startup==16)
	return -1;

    p_adc->ADC_MR |= ADC_MR_PRESCAL(ul_prescal) | ((ul_mr_startup << ADC_MR_STARTUP_Pos) & ADC_MR_STARTUP_Msk);
    return 0;
}


/**
 * \brief Configure ADC timing.
 *
 * \param p_adc Pointer to an ADC instance.
 * \param uc_tracking ADC tracking time = uc_tracking / ADC clock.
 * \param uc_settling Analog settling time = (uc_settling + 1) / ADC clock.
 * \param uc_transfer Data transfer time = (uc_transfer * 2 + 3) / ADC clock.
 */
void adc_configure_timing1(Adc *p_adc, const uint8_t uc_tracking,const enum adc_settling_time_t settling,const uint8_t uc_transfer)
{
    p_adc->ADC_MR |= ADC_MR_TRANSFER(uc_transfer) | settling | ADC_MR_TRACKTIM(uc_tracking);
}


/**
 * \brief Read the last ADC result data.
 *
 * \param p_adc Pointer to an ADC instance.
 *
 * \return ADC latest value.
 */
inline uint32_t adc_get_latest_value1(const Adc *p_adc)
{
    return p_adc->ADC_LCDR;
}

/**
 * \brief Get ADC interrupt and overrun error status.
 *
 * \param p_adc Pointer to an ADC instance.
 *
 * \return ADC status structure.
 */
inline uint32_t adc_get_status1(const Adc *p_adc)
{
    return p_adc->ADC_ISR;
}

/**
 * \brief Start analog-to-digital conversion.
 *
 * \note If one of the hardware event is selected as ADC trigger,
 * this function can NOT start analog to digital conversion.
 *
 * \param p_adc Pointer to an ADC instance.
 */

inline void adc_start1(Adc *p_adc)
{
    p_adc->ADC_CR = ADC_CR_START;
}

//==============================================================================

void adc_init1()
{
    pmc_enable_periph_clk(ID_ADC);

    adc_init2(ADC, SystemCoreClock, ADC_FREQ_MAX, 0 /*ADC_STARTUP_FAST*/);
//    adc_configure_timing1(ADC, 0, 0 ADC_SETTLING_TIME_0, 0 /*1*/);
    adc_configure_trigger(ADC, ADC_TRIG_SW, 0);		// Disable hardware trigger.
    adc_disable_interrupt(ADC, 0xFFFFFFFF);		// Disable all ADC interrupts.
    adc_disable_all_channel(ADC);

    PIO_Configure(PIOA, PIO_INPUT, PIO_PA16X1_AD7, PIO_DEFAULT);	//a0 pin
    PIO_Configure(PIOA, PIO_INPUT, PIO_PA24X1_AD6, PIO_DEFAULT);	//a1 pin
}


//    adc_enable_channel(ADC,ch);
//    adc_disable_channel(ADC,ch);

uint16_t adc_get1(uint8_t ch)
{
uint16_t val;
    adc_start1(ADC);
    while ((adc_get_status1(ADC) & ADC_ISR_DRDY) != ADC_ISR_DRDY);
    val = adc_get_latest_value1(ADC);
    return val;
}

