//=============================================================================
//
// BME280 Humidity,temperature,pressure sensor
//
//=============================================================================

#include "main.h"
#include "bme280.h"
#include "i2c.h"

uint16_t dig_T1;
int16_t  dig_T2;
int16_t  dig_T3;

uint16_t dig_P1;
int16_t  dig_P2;
int16_t  dig_P3;
int16_t  dig_P4;
int16_t  dig_P5;
int16_t  dig_P6;
int16_t  dig_P7;
int16_t  dig_P8;
int16_t  dig_P9;

uint8_t  dig_H1;
int16_t  dig_H2;
uint8_t  dig_H3;
int16_t  dig_H4;
int16_t  dig_H5;
int8_t   dig_H6;

int32_t t_fine;

extern uint8_t e2err;
extern uint8_t e2plc;


void write8(uint8_t adr, uint8_t d)
{
	e2err = 0;
	e2plc = 0;

	bstart();
	tx(0xec);
	tx(adr);
	tx(d);
	bstop();
}


uint8_t read8(uint8_t adr)
{
uint8_t d;
	e2err = 0;
	e2plc = 0;

	bstart();
	tx(0xec);
	tx(adr);
	bstop();

	bstart();
	tx(0xed);
	d = rx(rxNO_ACK);
	bstop();
	return d;
}


uint16_t read16(uint8_t adr)
{
uint16_t d;
	e2err = 0;
	e2plc = 0;

	bstart();
	tx(0xec);
	tx(adr);
	bstop();

	bstart();
	tx(0xed);
	d = rx(rxACK);
	d <<= 8;
	d |= rx(rxNO_ACK);
	bstop();
	return d;
}


uint16_t read16_LE(uint8_t reg)
{
uint16_t temp = read16(reg);
	return (temp >> 8) | (temp << 8);
}

#define	readS16_LE(X)	(int16_t)read16_LE(X)


uint32_t read24(uint8_t adr)
{
uint32_t d;
	e2err = 0;
	e2plc = 0;

	bstart();
	tx(0xec);
	tx(adr);
	bstop();

	bstart();
	tx(0xed);
	d = rx(rxACK);
	d <<= 8;
	d |= rx(rxACK);
	d <<= 8;
	d |= rx(rxNO_ACK);
	bstop();

	return d;
}

//=============================================================================

void readCoefficients(void)
{
    dig_T1 = read16_LE(BME280_REGISTER_DIG_T1);
    dig_T2 = readS16_LE(BME280_REGISTER_DIG_T2);
    dig_T3 = readS16_LE(BME280_REGISTER_DIG_T3);

    dig_P1 = read16_LE(BME280_REGISTER_DIG_P1);
    dig_P2 = readS16_LE(BME280_REGISTER_DIG_P2);
    dig_P3 = readS16_LE(BME280_REGISTER_DIG_P3);
    dig_P4 = readS16_LE(BME280_REGISTER_DIG_P4);
    dig_P5 = readS16_LE(BME280_REGISTER_DIG_P5);
    dig_P6 = readS16_LE(BME280_REGISTER_DIG_P6);
    dig_P7 = readS16_LE(BME280_REGISTER_DIG_P7);
    dig_P8 = readS16_LE(BME280_REGISTER_DIG_P8);
    dig_P9 = readS16_LE(BME280_REGISTER_DIG_P9);

    dig_H1 = read8(BME280_REGISTER_DIG_H1);
    dig_H2 = readS16_LE(BME280_REGISTER_DIG_H2);
    dig_H3 = read8(BME280_REGISTER_DIG_H3);
    dig_H4 = (read8(BME280_REGISTER_DIG_H4) << 4) | (read8(BME280_REGISTER_DIG_H4+1) & 0xF);
    dig_H5 = (read8(BME280_REGISTER_DIG_H5+1) << 4) | (read8(BME280_REGISTER_DIG_H5) >> 4);
    dig_H6 = (int8_t)read8(BME280_REGISTER_DIG_H6);
}


uint8_t bme_init(void)
{
uint8_t dataToWrite;

	if(read8(BME280_REGISTER_CHIPID) != 0x60)
		return 0;

	readCoefficients();
	//Set before CONTROL_meas (DS 5.4.3)
//	write8(BME280_REGISTER_CONTROLHUMID, 0x05); //16x oversampling 
//	write8(BME280_REGISTER_CONTROL, 0xB7); // 16x ovesampling, normal mode

//	write8(BME280_REGISTER_CONFIG, 0xec); 
//	write8(BME280_REGISTER_CONTROLHUMID, 1); 
//	write8(BME280_REGISTER_CONTROL, 0x27);

	//Set the oversampling control words.
	//config will only be writeable in sleep mode, so first insure that.
	write8(BME280_REGISTER_CONTROL, 0x00);
	
	//Set the config word
	//dataToWrite = (settings.tStandby << 0x5) & 0xE0;
	//dataToWrite |= (settings.filter << 0x02) & 0x1C;
	write8(BME280_REGISTER_CONFIG, 0);
	
	//Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	//dataToWrite = settings.humidOverSample & 0x07; //all other bits can be ignored
	write8(BME280_REGISTER_CONTROLHUMID, 1);
	
	//set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (1 /*settings.tempOverSample*/ << 0x5) & 0xE0;
	//Next, pressure oversampling
	dataToWrite |= (1 /*settings.pressOverSample*/ << 0x02) & 0x1C;
	//Last, set mode
	dataToWrite |= (3 /*settings.runMode*/) & 0x03;
	//Load the byte
	write8(BME280_REGISTER_CONTROL, dataToWrite);

	return 1;
}


// x100
int32_t readTemperature(void)
{
int32_t var1, var2;
int32_t adc_T = read24(BME280_REGISTER_TEMPDATA);
	adc_T >>= 4;
	var1  = ((((adc_T>>3) - ((int32_t)dig_T1 <<1))) *
			((int32_t)dig_T2)) >> 11;
	var2  = (((((adc_T>>4) - ((int32_t)dig_T1)) *
			((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
	   		((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	return (((int32_t)t_fine * 5 + 128) >> 8) - 390; //WTF
}


//x1024
uint32_t readHumidity(void)
{
//	readTemperature(); // must be done first to get t_fine
	int32_t adc_H = read16(BME280_REGISTER_HUMIDDATA);
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) -
			(((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
	       		(((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
		    	(((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
		  	((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			((int32_t)dig_H1)) >> 4));
        v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	return  (v_x1_u32r>>12);
}



uint32_t readPressure()
{
int32_t var1, var2;
uint32_t p;
	int32_t adc_P = read24(BME280_REGISTER_PRESSUREDATA);
	adc_P >>= 4;

  var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
  var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)dig_P6);
  var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
  var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
  var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
  var1 =((((32768+var1))*((int32_t)dig_P1))>>15);
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
  if (p < 0x80000000) {
    p = (p << 1) / ((uint32_t)var1);
  } else {
    p = (p / (uint32_t)var1) * 2;
  }
  var1 = (((int32_t)dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
  var2 = (((int32_t)(p>>2)) * ((int32_t)dig_P8))>>13;
  p = (uint32_t)((int32_t)p + ((var1 + var2 + dig_P7) >> 4));
  return p;
}
