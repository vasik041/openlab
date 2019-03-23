#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#include "main.h"
#include "uart.h"

//uint32_t PWM_Frequency = 100;
//uint32_t PWM_Steps = 100;


uint32_t get_timer_clock_frequency()
{
RCC_ClocksTypeDef RCC_Clocks;
uint32_t multiplier;

    RCC_GetClocksFreq(&RCC_Clocks);
    if (RCC_Clocks.PCLK1_Frequency == RCC_Clocks.SYSCLK_Frequency) {
	multiplier = 1;
    } else {
	multiplier = 2;
    }
    uart_printf("timer clock %d\r\n",multiplier * RCC_Clocks.PCLK1_Frequency);
    return multiplier * RCC_Clocks.PCLK1_Frequency;
}


void timer3_init()
{
    //PWM 3/1 PB4
    //PWM 3/2 PB5
    //PWM 3/3 PB0
    //PWM 3/4 PB1

    GPIO_InitTypeDef  gpioInitStruct;
    GPIO_StructInit(&gpioInitStruct);

    gpioInitStruct.GPIO_Pin = GPIO_Pin_0 + GPIO_Pin_1 + GPIO_Pin_4 + GPIO_Pin_5;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInitStruct);

    // PA6,7 -> PB4,5
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);

    //uint32_t TIMER_Frequency = get_timer_clock_frequency();
    //uint32_t COUNTER_Frequency = PWM_Steps * PWM_Frequency;
    uint32_t PSC_Value = 8; //(TIMER_Frequency / COUNTER_Frequency) - 1;
    uint16_t ARR_Value = 1000; //PWM_Steps - 1;
 
    // make sure the peripheral is clocked
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef timTimeBaseStruct;
    TIM_TimeBaseStructInit(&timTimeBaseStruct);

    timTimeBaseStruct.TIM_Period = ARR_Value;
    timTimeBaseStruct.TIM_Prescaler = PSC_Value;
    TIM_TimeBaseInit(TIM3, &timTimeBaseStruct);

//    uart_printf("ARR %d\r\n",ARR_Value);
//    uart_printf("PSC %d\r\n",PSC_Value);

    TIM_OCInitTypeDef  timOCInitStruct;
    TIM_OCStructInit(&timOCInitStruct);
 
    // Common settings for all channels
    timOCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    timOCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
//    timOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    timOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
    timOCInitStruct.TIM_Pulse = 0;
 
    TIM_OC1Init(TIM3, &timOCInitStruct);
    TIM_OC2Init(TIM3, &timOCInitStruct);
    TIM_OC3Init(TIM3, &timOCInitStruct);
    TIM_OC4Init(TIM3, &timOCInitStruct);

    TIM_SetCompare1(TIM3, 20);
    TIM_SetCompare2(TIM3, 40);
    TIM_SetCompare3(TIM3, 60);
    TIM_SetCompare4(TIM3, 80);

    TIM_Cmd(TIM3, ENABLE); 
}


void set_ch1(uint16_t v) {
    TIM_SetCompare1(TIM3, v);
}

void set_ch2(uint16_t v) {
    TIM_SetCompare2(TIM3, v);
}

void set_ch3(uint16_t v) {
    TIM_SetCompare4(TIM3, v);
}

void set_ch4(uint16_t v) {
    TIM_SetCompare3(TIM3, v);
}

void set_period(uint16_t psc, uint16_t arr)
{
    TIM_TimeBaseInitTypeDef timTimeBaseStruct;
    TIM_TimeBaseStructInit(&timTimeBaseStruct);
    timTimeBaseStruct.TIM_Period = arr;
    timTimeBaseStruct.TIM_Prescaler = psc;
    TIM_TimeBaseInit(TIM3, &timTimeBaseStruct);
}
