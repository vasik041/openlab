
#include "main.h"
#include "rf24l01.h"

// In sending mode.
uint8_t PTX;

// Channel 0 - 127 or 0 - 84 in the US.
uint8_t channel;

// Payload width in bytes default 16 max 32.
uint8_t payload;

//=============================================================================

void spiBegin()
{
	DDRA |= (1 << SCK_PIN);
	DDRA |= (1 << MOSI_PIN);
	DDRA &= ~(1 << MISO_PIN);
	DDRA |= (1 << CE_PIN);
	DDRA |= (1 << CSN_PIN);
}


uint8_t spiTransfer(uint8_t dout)
{
uint8_t msk;
uint8_t din = 0;
	for(msk=0x80; msk; msk >>= 1) {
		if(dout & msk) {
      			PORTA |= (1 << MOSI_PIN);
		} else {
      			PORTA &= ~(1 << MOSI_PIN);
		}
		if(PINA & (1 << MISO_PIN)) din |= msk;
		PORTA |= (1 << SCK_PIN);
		PORTA &= ~(1 << SCK_PIN);
	}
	return din;
}


void transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len)
{
uint8_t i;
	for(i=0; i < len; i++) {
		datain[i] = spiTransfer(dataout[i]);
	}
}

void transmitSync(uint8_t *dataout,uint8_t len)
{
uint8_t i;
	for(i=0; i < len; i++){
		spiTransfer(dataout[i]);
	}
}


// Initializes pins to communicate with the MiRF module
// Should be called in the early initializing phase at startup.
void rf_init() 
{   
    DDRA |= (1 << CE_PIN);
    DDRA |= (1 << CSN_PIN);
    ceLow();
    csnHi();
    spiBegin(); // Initialize spi module
}


// Sets the important registers in the MiRF module and powers the module
// in receiving mode
// NB: channel and payload must be set now.
void config() 
{
    	// Set RF channel
	configRegister(RF_CH,channel);
	configRegister(RF_SETUP,0x09);	// RF_PWR = 00 -18dBm

	// Set length of incoming payload 
	configRegister(RX_PW_P0, payload);
	configRegister(RX_PW_P1, payload);

	// Start receiver 
	powerUpRx();
	flushRx();
}


// Sets the receiving address
void setRADDR(uint8_t * adr) 
{
	ceLow();
	writeRegister(RX_ADDR_P1,adr,mirf_ADDR_LEN);
	ceHi();
}


// Sets the transmitting address
void setTADDR(uint8_t * adr)
{
	/*
	 * RX_ADDR_P0 must be set to the sending addr for auto ack to work.
	 */
	writeRegister(RX_ADDR_P0,adr,mirf_ADDR_LEN);
	writeRegister(TX_ADDR,adr,mirf_ADDR_LEN);
}


uint8_t dataReady() 
// Checks if data is available for reading
{
// See note in getData() function - just checking RX_DR isn't good enough
uint8_t status = getStatus();

    // We can short circuit on RX_DR, but if it's not set, we still need
    // to check the FIFO for any pending packets
    if ( status & (1 << RX_DR) ) return 1;
    return !rxFifoEmpty();
}


uint8_t rxFifoEmpty()
{
uint8_t fifoStatus;
	readRegister(FIFO_STATUS,&fifoStatus,sizeof(fifoStatus));
	return (fifoStatus & (1 << RX_EMPTY));
}



// Reads payload bytes into data array
void getData(uint8_t * data) 
{
    csnLow();                               // Pull down chip select
    spiTransfer( R_RX_PAYLOAD );            // Send cmd to read rx payload
    transferSync(data,data,payload); // Read payload
    csnHi();                               // Pull up chip select
    // NVI: per product spec, p 67, note c:
    //  "The RX_DR IRQ is asserted by a new packet arrival event. The procedure
    //  for handling this interrupt should be: 1) read payload through SPI,
    //  2) clear RX_DR IRQ, 3) read FIFO_STATUS to check if there are more 
    //  payloads available in RX FIFO, 4) if there are more data in RX FIFO,
    //  repeat from step 1)."
    // So if we're going to clear RX_DR here, we need to check the RX FIFO
    // in the dataReady() function
    configRegister(STATUS,(1<<RX_DR));   // Reset status register
}

// Clocks only one byte into the given MiRF register
void configRegister(uint8_t reg, uint8_t value)
{
    csnLow();
    spiTransfer(W_REGISTER | (REGISTER_MASK & reg));
    spiTransfer(value);
    csnHi();
}

// Reads an array of bytes from the given start position in the MiRF registers.
void readRegister(uint8_t reg, uint8_t *value, uint8_t len)
{
    csnLow();
    spiTransfer(R_REGISTER | (REGISTER_MASK & reg));
    transferSync(value,value,len);
    csnHi();
}

// Writes an array of bytes into inte the MiRF registers.
void writeRegister(uint8_t reg, uint8_t * value, uint8_t len) 
{
    csnLow();
    spiTransfer(W_REGISTER | (REGISTER_MASK & reg));
    transmitSync(value,len);
    csnHi();
}

// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
void send(uint8_t * value) 
{
    uint8_t status;
    status = getStatus();

    while (PTX) {
	    status = getStatus();

	    if((status & ((1 << TX_DS)  | (1 << MAX_RT)))){
		    PTX = 0;
		    break;
	    }
    }                  // Wait until last paket is send

    ceLow();
    
    powerUpTx();       // Set to transmitter mode , Power up
    
    csnLow();                    // Pull down chip select
    spiTransfer( FLUSH_TX );     // Write cmd to flush tx fifo
    csnHi();                    // Pull up chip select
    
    csnLow();                    // Pull down chip select
    spiTransfer( W_TX_PAYLOAD ); // Write cmd to write payload
    transmitSync(value,payload);   // Write payload
    csnHi();                    // Pull up chip select

    ceHi();                     // Start transmission
}

/**
 * isSending.
 *
 * Test if chip is still sending.
 * When sending has finished return chip to listening.
 */
uint8_t isSending()
{
uint8_t status;
	if(PTX){
		status = getStatus();
		/*
		 *  if sending successful (TX_DS) or max retries exceded (MAX_RT).
		 */
		if((status & ((1 << TX_DS)  | (1 << MAX_RT)))){
			powerUpRx();
			return 0; 
		}
		return 1;
	}
	return 0;
}


uint8_t getStatus()
{
uint8_t rv;
	readRegister(STATUS,&rv,1);
	return rv;
}

void powerUpRx()
{
	PTX = 0;
	ceLow();
	configRegister(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );
	ceHi();
	configRegister(STATUS,(1 << TX_DS) | (1 << MAX_RT)); 
}


void flushRx()
{
    csnLow();
    spiTransfer( FLUSH_RX );
    csnHi();
}


void powerUpTx()
{
	PTX = 1;
	configRegister(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) );
}


void ceHi()
{
	PORTA |= (1 << CE_PIN);
}


void ceLow()
{
	PORTA &= ~(1 << CE_PIN);
}


void csnHi()
{
	PORTA |= (1 << CSN_PIN);
}


void csnLow()
{
	PORTA &= ~(1 << CSN_PIN);
}


void powerDown()
{
	ceLow();
	configRegister(CONFIG, mirf_CONFIG);
}

