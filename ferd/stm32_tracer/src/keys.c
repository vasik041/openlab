#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "main.h"


void keys_init()
{
    // initialize GPIO structure
    GPIO_InitTypeDef gpioInitStruct;
    GPIO_StructInit(&gpioInitStruct);

    gpioInitStruct.GPIO_Pin = GPIO_Pin_6;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpioInitStruct);

    gpioInitStruct.GPIO_Pin = GPIO_Pin_7;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpioInitStruct);

    gpioInitStruct.GPIO_Pin = GPIO_Pin_8;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpioInitStruct);

    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpioInitStruct);
}

uint8_t keys_get()
{
    uint8_t i0 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
    uint8_t i1 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
    uint8_t i2 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
    uint8_t i3 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
    return (i0 + (i1 << 1) + (i2 << 2) + (i3 << 3)) ^ 0x0f;
}
