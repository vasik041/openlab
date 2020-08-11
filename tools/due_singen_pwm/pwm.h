
#ifndef _PWM_H
#define _PWM_H

void pwm_init(void);
//void pwm_set(uint32_t period,uint32_t pulse);

void set_freq(uint32_t fq);
void set_pulse0(uint16_t start, uint16_t len);
//void set_pulse1(uint16_t start, uint16_t len);

void out0(uint8_t sw);
void out1(uint8_t sw);

#endif // _PWM_H
