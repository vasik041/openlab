#include <stdio.h>
#include <stdlib.h>

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "main.h"
#include "i2c.h"

void i2c_init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    
    // PB10 SCL
    // PB11 SDA
    GPIO_InitTypeDef gpioInitStruct;
    GPIO_StructInit(&gpioInitStruct);
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10 + GPIO_Pin_11;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInitStruct);

    I2C_InitTypeDef i2cInitStruct;
    I2C_StructInit(&i2cInitStruct);

    i2cInitStruct.I2C_Mode = I2C_Mode_I2C;
    i2cInitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    i2cInitStruct.I2C_OwnAddress1 = 0x38;
    i2cInitStruct.I2C_Ack = I2C_Ack_Disable;
    i2cInitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2cInitStruct.I2C_ClockSpeed = 100000;
//    I2C_DeInit(I2C2);
    I2C_Init(I2C2, &i2cInitStruct);
    I2C_Cmd(I2C2, ENABLE);	
}


int i2c_read(uint16_t address)
{
    uint32_t timeout = I2C_TIMEOUT_MAX;
    uint8_t d = 0;
    
    I2C_GenerateSTART(I2C2, ENABLE);
    
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
	if(timeout-- == 0) return -1;
    }
    I2C_Send7bitAddress(I2C2, 0xA0, I2C_Direction_Transmitter);
    
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
	if(timeout-- == 0) return -1;
    } 
    
    I2C_SendData(I2C2, address >> 8);
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
	if (timeout-- == 0) return -1;
    }

    I2C_SendData(I2C2, address);
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
	if (timeout-- == 0) return -1;
    }
    
    I2C_GenerateSTART(I2C2, ENABLE);
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
	if (timeout-- == 0) return -1;
    }

    I2C_Send7bitAddress(I2C2, 0xA0, I2C_Direction_Receiver);
    
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
	if (timeout-- == 0) return -1;
    }
    
    I2C_AcknowledgeConfig(I2C2, DISABLE);
    
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))  {
	if (timeout-- == 0) return -1;
    }
 
    I2C_GenerateSTOP(I2C2, ENABLE);
    d = I2C_ReceiveData(I2C2);
    
    return d;
}

int i2c_write(uint16_t address, uint8_t data)
{
    uint32_t timeout = I2C_TIMEOUT_MAX;
    
    I2C_GenerateSTART(I2C2, ENABLE);
    
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
	if (timeout-- == 0) return -1;
    }
    I2C_Send7bitAddress(I2C2, 0xA0, I2C_Direction_Transmitter);
    
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
	if (timeout-- == 0) return -1;
    } 
    
    I2C_SendData(I2C2, address >> 8);
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
	    if(timeout-- == 0) return -1;
    }

    I2C_SendData(I2C2, address);
    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
	    if(timeout-- == 0) return -1;
    }
    
    I2C_SendData(I2C2, data);

    timeout = I2C_TIMEOUT_MAX;
    while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
	    if (timeout-- == 0) return -1;
    }

    I2C_GenerateSTOP(I2C2, ENABLE);
    delay_ms(5);
    return 0;	
}


uint16_t i2c_read16(uint16_t addr)
{
    return i2c_read(addr) + (i2c_read(addr+1) << 8);
}


void i2c_write16(uint16_t addr, uint16_t d)
{
    i2c_write(addr, d);
    i2c_write(addr+1, d >> 8);
}


