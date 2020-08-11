
#include "include/due_sam3x.h"
#include "pwm.h"

volatile const uint8_t sine[] = {
//sine
    128,130,133,136,139,142,145,148,151,154,157,160,162,165,168,171,
    173,176,179,181,184,187,189,192,194,197,199,201,204,206,208,210,
    212,214,216,218,220,222,224,226,227,229,230,232,233,235,236,237,
    238,239,240,241,242,243,244,245,245,246,246,247,247,247,247,247,
    247,247,247,247,247,247,246,246,245,245,244,243,242,241,240,239,
    238,237,236,235,233,232,230,229,227,226,224,222,220,218,216,214,
    212,210,208,206,204,201,199,197,194,192,189,187,184,181,179,176,
    173,171,168,165,162,160,157,154,151,148,145,142,139,136,133,130,
    128,125,122,119,116,113,110,107,104,101, 98, 95, 93, 90, 87, 84,
     82, 79, 76, 74, 71, 68, 66, 63, 61, 58, 56, 54, 51, 49, 47, 45,
     43, 41, 39, 37, 35, 33, 31, 29, 28, 26, 25, 23, 22, 20, 19, 18,
     17, 16, 15, 14, 13, 12, 11, 10, 10,  9,  9,  8,  8,  8,  8,  8,
      8,  8,  8,  8,  8,  8,  9,  9, 10, 10, 11, 12, 13, 14, 15, 16,
     17, 18, 19, 20, 22, 23, 25, 26, 28, 29, 31, 33, 35, 37, 39, 41,
     43, 45, 47, 49, 51, 54, 56, 58, 61, 63, 66, 68, 71, 74, 76, 79,
     82, 84, 87, 90, 93, 95, 98,101,104,107,110,113,116,119,122,125
};

volatile uint32_t step;
volatile uint32_t phase;

volatile uint16_t st0;
//volatile uint16_t st1;
volatile uint16_t len0;
//volatile uint16_t len1;

volatile uint16_t ph0;
//volatile uint16_t ph1;
volatile uint16_t pt0;
//volatile uint16_t pt1;

volatile uint16_t val0;

//==============================================================================
/**
 * \brief Configures a Timer Counter Channel
 *
 * Configures a Timer Counter to operate in the given mode. Timer is stopped
 * after configuration and must be restarted with TC_Start(). All the
 * interrupts of the timer are also disabled.
 *
 * \param pTc  Pointer to a Tc instance.
 * \param channel Channel number.
 * \param mode  Operating mode (TC_CMR value).
 */
void TC_Configure1(Tc *pTc, uint32_t dwChannel, uint32_t dwMode )
{
    TcChannel* pTcCh ;

    pTcCh = pTc->TC_CHANNEL+dwChannel ;

    /*  Disable TC clock */
    pTcCh->TC_CCR = TC_CCR_CLKDIS ;

    /*  Disable interrupts */
    pTcCh->TC_IDR = 0xFFFFFFFF ;

    /*  Clear status register */
    pTcCh->TC_SR ;

    /*  Set mode */
    pTcCh->TC_CMR = dwMode ;
}

/**
 * \brief Set RA on the selected channel.
 *
 * \param tc Pointer to a TC instance.
 * \param chan Channel to configure.
 * \param v New value for RA.
 */
inline void TC_SetRA1(Tc *tc, uint32_t chan, uint32_t v)
{
    tc->TC_CHANNEL[chan].TC_RA = v;
}


/**
 * \brief Set RB on the selected channel.
 *
 * \param tc Pointer to a TC instance.
 * \param chan Channel to configure.
 * \param v New value for RB.
 */
inline void TC_SetRB1(Tc *tc, uint32_t chan, uint32_t v)
{
    tc->TC_CHANNEL[chan].TC_RB = v;
}

/**
 * \brief Set RC on the selected channel.
 *
 * \param tc Pointer to a TC instance.
 * \param chan Channel to configure.
 * \param v New value for RC.
 */
inline void TC_SetRC1(Tc *tc, uint32_t chan, uint32_t v)
{
    tc->TC_CHANNEL[chan].TC_RC = v;
}

/**
 * \brief Reset and Start the TC Channel
 *
 * Enables the timer clock and performs a software reset to start the counting.
 *
 * \param pTc  Pointer to a Tc instance.
 * \param dwChannel Channel number.
 */
inline void TC_Start1(Tc *pTc, uint32_t dwChannel )
{
    TcChannel *pTcCh;

    pTcCh = pTc->TC_CHANNEL+dwChannel;
    pTcCh->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;
}

//==============================================================================

void pwm_init()
{
    PIO_Configure(PIOD, PIO_OUTPUT_1, PIO_PD0, PIO_DEFAULT);	/* Board pin 25 == PD0 */
    PIO_Configure(PIOD, PIO_OUTPUT_1, PIO_PD2, PIO_DEFAULT);	/* Board pin 27 == PD2 */

  pmc_enable_periph_clk(ID_TC0); // Use Timer/Counter 0 (enable its clock)
//  pmc_enable_periph_clk(ID_TC2); // Use Timer/Counter 0 (enable its clock)

  /* Configure as PWM using upward counter */
  TC_Configure1(TC0, 0, 0	   	/* Channel 0 on TC0               */
//  TC_Configure1(TC2, 2, 0	   	/* Channel 0 on TC0               */
    |TC_CMR_TCCLKS_TIMER_CLOCK2 	/* Use TCLK2 as source === MCLK/8 */
    |TC_CMR_WAVE                	/* Waveform mode ("PWM")          */
    |TC_CMR_WAVSEL_UP_RC        /* Count upwards to register C (==RC) */
    |TC_CMR_ACPA_CLEAR          /* Switch TIOA off when reaching RB Compare */
    |TC_CMR_ACPC_SET            /* Switch TIOA on  when reaching RC Compare */
    |TC_CMR_EEVT_XC0
  );
 
  PIO_Configure(PIOB, PIO_PERIPH_B, PIO_PB25B_TIOA0, PIO_DEFAULT);
//  PIO_Configure(PIOD, PIO_PERIPH_B, PIO_PD8B_TIOB8, PIO_DEFAULT);

//  TC_SetRC1(TC0, 0, 0xffff);	// Period: CLK/65536 == 10Hz
  TC_SetRC1(TC0, 0, 256);	// 25us
  TC_SetRA1(TC0, 0, 1);         // Initial duty cycle: 0
  TC_Start1(TC0, 0);            // Start timer (for channel 0)

  phase = 1;
  step = 1;

  REG_TC0_IER0 = 0b00010000;    // enable interrupt on counter=rc
  REG_TC0_IDR0 = 0b11101111;    // disable other interrupts
  NVIC_EnableIRQ(TC0_IRQn);     // enable TC0 interrupts
}


// pulse 1us = 10
// period 1ms = 
//void pwm_set(uint32_t period, uint32_t pulse)
//{
//  TC_SetRC1(TC0, 0, period);	 // Period: CLK/65536 == 10Hz
//  TC_SetRA1(TC0, 0, pulse);    // Initial duty cycle: 0
//}

void set_freq(uint32_t fq)
{
    step = fq;
}

// step 256..2560
// number of steps 65536/step = 256..25
// x 1..99%
uint16_t PC2T(uint16_t x)
{
uint32_t s = 0x40000/step;
uint32_t px = s*x/100;
    if(!px) px = 1;
    return px;
}

void set_pulse0(uint16_t start, uint16_t len)
{
    st0 = PC2T(start);
    len0 = PC2T(len);
}

//void set_pulse1(uint16_t start, uint16_t len)
//{
//    st1 = PC2T(start);
//    len1 = PC2T(len);
//}


void TC0_Handler()
{
    long dummy=REG_TC0_SR0; // vital - reading this clears some flag
                            // otherwise you get infinite interrupts
    (void)dummy;

    uint16_t val = sine[(phase >> 10) & 0xff];
    TC_SetRA1(TC0, 0, val);
    phase += step;

    if(val0 <= 128 && val > 128) {
        ph0 = st0;
    }
    if(val0 >= 128 && val < 128) {
        ph0 = st0;
    }
    val0 = val;

    if(ph0) {
        ph0--;
        if(!ph0) { pt0 = len0; out0(1); }
    }
//    if(ph1) {
//        ph1--;
//        if(!ph1) { pt1 = len1; out1(1); }
//    }
    if(pt0) {
        pt0--;
        if(!pt0) out0(0);
    }
//    if(pt1) {
//        pt1--;
//        if(!pt1) out1(0);
//    }
}


inline void out0(uint8_t st)
{
    if(st) {
        PIOD->PIO_SODR = PIO_PD0;
    } else {
        PIOD->PIO_CODR = PIO_PD0;
    }
}

inline void out1(uint8_t st)
{
    if(st) {
        PIOD->PIO_SODR = PIO_PD2;
    } else {
        PIOD->PIO_CODR = PIO_PD2;
    }
}

