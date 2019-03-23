#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#include "main.h"
#include "adc.h"


void adc_init()
{
    GPIO_InitTypeDef gpioInitStruct;
    GPIO_StructInit(&gpioInitStruct);
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3 + GPIO_Pin_4;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpioInitStruct);

    ADC_InitTypeDef adcInitStruct;
    ADC_StructInit(&adcInitStruct);
    /* PCLK2 is the APB2 clock */
    /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /* Enable ADC1 clock so that we can talk to it */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    /* Put everything back to power-on defaults */
    ADC_DeInit(ADC1);

    /* ADC1 Configuration */
    /* ADC1 and ADC2 operate independently */
    adcInitStruct.ADC_Mode = ADC_Mode_Independent;
    /* Disable the scan conversion so we do one at a time */
    adcInitStruct.ADC_ScanConvMode = DISABLE;
    /* Don't do continuous conversions - do them on demand */
    adcInitStruct.ADC_ContinuousConvMode = ENABLE;
    /* Start conversin by software, not an external trigger */
    adcInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    /* Conversions are 12 bit - put them in the lower 12 bits of the result */
    adcInitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    /* Say how many channels would be used by the sequencer */
    adcInitStruct.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &adcInitStruct);
    ADC_Cmd(ADC1, ENABLE);

    /* Enable ADC1 reset calibaration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));
    /* Start ADC1 calibaration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));
}


uint16_t adc_read(uint8_t channel)
{
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
  // Start the conversion
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  // Wait until conversion completion
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  // Get the conversion value
  return ADC_GetConversionValue(ADC1);
}


void adc_read_n(uint8_t channel, uint16_t *buf, uint16_t n)
{
uint16_t i;
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    for(i=0; i < n; i++) {
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        buf[i] = ADC_GetConversionValue(ADC1);
    }
}

//==============================================================================


volatile uint32_t status = 0;

void DMA1_Channel1_IRQHandler(void)
{
    //Test on DMA1 Channel1 Transfer Complete interrupt
    if(DMA_GetITStatus(DMA1_IT_TC1)) {
	status = 1;
	//Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits
	DMA_ClearITPendingBit(DMA1_IT_GL1);
    }
}



void adc_read_n_dma(uint8_t channel, uint16_t *buf, uint16_t n)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_1Cycles5);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_InitTypeDef dmaInitStruct;
    
    /* DMA channel1 configuration */
    DMA_DeInit(DMA1_Channel1);
    dmaInitStruct.DMA_PeripheralBaseAddr = DR_ADDRESS;
    dmaInitStruct.DMA_MemoryBaseAddr = (u32)buf;
    dmaInitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    dmaInitStruct.DMA_BufferSize = n;
    dmaInitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaInitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaInitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dmaInitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dmaInitStruct.DMA_Mode = DMA_Mode_Normal;
    dmaInitStruct.DMA_Priority = DMA_Priority_High;
    dmaInitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dmaInitStruct);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    //Enable DMA1 channel IRQ Channel */
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    status = 0;
    while(status == 0);
    DMA_Cmd(DMA1_Channel1, DISABLE);
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
}


