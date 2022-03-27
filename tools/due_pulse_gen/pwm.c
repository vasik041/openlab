
#include "include/due_sam3x.h"
#include "pwm.h"

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
  pmc_enable_periph_clk(ID_TC0); // Use Timer/Counter 0 (enable its clock)
  pmc_enable_periph_clk(ID_TC8);

  /* Configure as PWM using upward counter */
  TC_Configure1(TC0, 0, 0	   	/* Channel 0 on TC0               */
    |TC_CMR_TCCLKS_TIMER_CLOCK2 	/* Use TCLK2 as source === MCLK/8 */
    |TC_CMR_WAVE                	/* Waveform mode ("PWM")          */
    |TC_CMR_WAVSEL_UP_RC        /* Count upwards to register C (==RC) */
    |TC_CMR_ACPA_CLEAR          /* Switch TIOA off when reaching RA Compare */
    |TC_CMR_ACPC_SET            /* Switch TIOA on  when reaching RC Compare */
    |TC_CMR_EEVT_XC0            /* disable external trigger mode (external trigger to XCO) */
  );
 
  PIO_Configure(PIOB, PIO_PERIPH_B, PIO_PB25B_TIOA0, PIO_DEFAULT); // digital pin 2
  TC_SetRC1(TC0, 0, 500);	// 50us
  TC_SetRA1(TC0, 0, 10);        // Initial duty cycle: 10
//  TC_Start1(TC0, 0);            // Start timer (for channel 0)



  /* TC2 channel 2 */
  /* Configure as PWM using upward counter */
  TC_Configure1(TC2, 2, 0	   	/* Channel 2 on TC2               */
    |TC_CMR_TCCLKS_TIMER_CLOCK2 	/* Use TCLK2 as source === MCLK/8 */
    |TC_CMR_WAVE                	/* Waveform mode ("PWM")          */
    |TC_CMR_WAVSEL_UP_RC        /* Count upwards to register C (==RC) */
    |TC_CMR_ACPA_CLEAR          /* Switch TIOA off when reaching RA Compare */
    |TC_CMR_ACPC_SET            /* Switch TIOA on  when reaching RC Compare */
    |TC_CMR_BCPB_CLEAR          /* Switch TIOB off when reaching RB Compare */
    |TC_CMR_BCPC_SET            /* Switch TIOB on  when reaching RC Compare */
    |TC_CMR_EEVT_XC0
  );
 
  PIO_Configure(PIOD, PIO_PERIPH_B, PIO_PD7B_TIOA8, PIO_DEFAULT);
  PIO_Configure(PIOD, PIO_PERIPH_B, PIO_PD8B_TIOB8, PIO_DEFAULT);
  TC_SetRC1(TC2, 2, 500);
  TC_SetRA1(TC2, 2, 20);
  TC_SetRB1(TC2, 2, 30);

  TC_Start1(TC0, 0);            // Start timer (for channel 0)
  TC_Start1(TC2, 2);
}


// pulse 1us = 10
// period 1us = 10
void pwm_set(uint8_t timer, uint32_t period, uint32_t pulse)
{
  switch(timer) {
    case 0:
      TC_SetRC1(TC0, 0, period);
      TC_SetRA1(TC0, 0, pulse);
      break;
    case 1:
      TC_SetRC1(TC2, 2, period);
      TC_SetRA1(TC2, 2, pulse); 
      break;
    case 2:
      TC_SetRC1(TC2, 2, period);
      TC_SetRB1(TC2, 2, pulse); 
      break;
    default:;
  }
}

