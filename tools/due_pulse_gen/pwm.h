#ifndef _PWM_H
#define _PWM_H

void pwm_init(void);
void pwm_set(uint8_t channel, uint32_t period,uint32_t pulse);

#endif // _PWM_H
