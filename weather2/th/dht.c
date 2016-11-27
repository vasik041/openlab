//=============================================================================
//
// Reading DHT11
// LZ,2016
//
//=============================================================================

#include "main.h"
#include "dht.h"

#define MAX_CYCLES	200	// timeout 1ms
#define DHT_PIN		6


volatile uint8_t dht_data[5];


void dht_init(void)
{
	DDRB |= (1 << DHT_PIN);
}

//=============================================================================
/*
uint16_t expectPulse(uint8_t level)
{
uint16_t count = 0;
uint8_t portState = level ? (PINB & (1 << DHT_PIN)) : 0;

	while((PINB & (1 << DHT_PIN)) == portState) {
		if (count++ >= MAX_CYCLES) {
			return 0;
		}
	}
	return count;
}
*/

uint8_t expectPulse0()
{
uint8_t count = 0;

	while((PINB & (1 << DHT_PIN)) == 0) {
		if (count++ >= MAX_CYCLES) return 0;
	}
	return count;
}


uint8_t expectPulse1()
{
uint8_t count = 0;
//uint8_t portState = (PINB & (1 << DHT_PIN));

	while((PINB & (1 << DHT_PIN)) == (1 << DHT_PIN)) {
		if (count++ >= MAX_CYCLES) return 0;
	}
	return count;
}


//=============================================================================

uint8_t dht_read(void)
{
uint8_t cycleLow,cycleHigh,i;

	dht_data[0] = dht_data[1] = dht_data[2] = dht_data[3] = dht_data[4] = 0;
	DDRB |= (1 << DHT_PIN);
	PORTB |= (1 << DHT_PIN);
	_delay_ms(250);

	PORTB &= ~(1 << DHT_PIN);
	_delay_ms(20);

	// End the start signal by setting data line high for 40 microseconds.
	PORTB |= (1 << DHT_PIN);
	_delay_us(30);
	// Now start reading the data line to get the value from the DHT sensor.
	DDRB &= ~(1 << DHT_PIN);
//	_delay_us(10);  // Delay a bit to let sensor pull data line low.

	// First expect a low signal for ~80 microseconds followed by a high signal
	// for ~80 microseconds again.
	if (expectPulse0() == 0) {
		return 1;
	}
	if (expectPulse1() == 0) {
		return 2;
	}

// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
// microsecond low pulse followed by a variable length high pulse.  If the
// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
// then it's a 1.  We measure the cycle count of the initial 50us low pulse
// and use that to compare to the cycle count of the high pulse to determine
// if the bit is a 0 (high state cycle count < low state cycle count), or a
// 1 (high state cycle count > low state cycle count). Note that for speed all
// the pulses are read into a array and then examined in a later step.

	for(i=0; i < 40; i++) {
	      	cycleLow  = expectPulse0();
      		cycleHigh = expectPulse1();
		if(cycleLow == 0 || cycleHigh == 0) {
			return 3;
		}
    		dht_data[i/8] <<= 1;

		// Now compare the low and high cycle times to see if the bit is a 0 or 1.
		if (cycleHigh > cycleLow) {
      			// High cycles are greater than 50us low cycle count, must be a 1.
      			dht_data[i/8] |= 1;
		}
    	}

	// Check we read 40 bits and that the checksum matches.
	if (dht_data[4] == ((dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3]) & 0xFF)) {
		return 0;
	}
	return 4; // wrong checksum
}

//=============================================================================

uint8_t getTemperature(void)
{
	return dht_data[2];
}


uint8_t getHumidity(void)
{
	return dht_data[0];
}

