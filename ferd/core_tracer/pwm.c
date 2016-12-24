
#include "include/due_sam3x.h"
#include "pwm.h"

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
void TC_Configure1( Tc *pTc, uint32_t dwChannel, uint32_t dwMode )
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
void TC_SetRA1(Tc *tc, uint32_t chan, uint32_t v)
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
void TC_SetRB1(Tc *tc, uint32_t chan, uint32_t v)
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
void TC_SetRC1(Tc *tc, uint32_t chan, uint32_t v)
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
void TC_Start1( Tc *pTc, uint32_t dwChannel )
{
    TcChannel* pTcCh ;

    pTcCh = pTc->TC_CHANNEL+dwChannel ;
    pTcCh->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;
}

//==============================================================================

void pwm_init()
{
  pmc_enable_periph_clk(ID_TC0); // Use Timer/Counter 0 (enable its clock)

  /* Configure as PWM using upward counter */
  TC_Configure1(TC0, 0, 0	   	/* Channel 0 on TC0               */
    |TC_CMR_TCCLKS_TIMER_CLOCK2 	/* Use TCLK2 as source === MCLK/8 */
    |TC_CMR_WAVE                	/* Waveform mode ("PWM")          */
    |TC_CMR_WAVSEL_UP_RC        /* Count upwards to register C (==RC) */
    |TC_CMR_ACPA_CLEAR          /* Switch TIOA off when reaching RB Compare */
    |TC_CMR_ACPC_SET            /* Switch TIOA on  when reaching RC Compare */
    |TC_CMR_EEVT_XC0
  );
 
  PIO_Configure(PIOB, PIO_PERIPH_B, PIO_PB25B_TIOA0, PIO_DEFAULT);

  TC_SetRC1(TC0, 0, 0xffff );	// Period: CLK/65536 == 10Hz
  TC_SetRA1(TC0, 0, 1);         // Initial duty cycle: 0
  TC_Start1(TC0, 0);            // Start timer (for channel 0)
}



void pwm_set(uint32_t period,uint32_t pulse)
{
  TC_SetRC1(TC0, 0, period);	// Period: CLK/65536 == 10Hz
  TC_SetRA1(TC0, 0, pulse);     // Initial duty cycle: 0
}


