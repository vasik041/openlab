#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "main.h"
#include "twi.h"
#include "wire.h"

// Initialize Class Variables //////////////////////////////////////////////////

//uint8_t rxBuffer[BUFFER_LENGTH];
//uint8_t rxBufferIndex = 0;
//uint8_t rxBufferLength = 0;

uint8_t txAddress = 0;
uint8_t txBuffer[BUFFER_LENGTH];
uint8_t txBufferIndex = 0;
uint8_t txBufferLength = 0;

uint8_t transmitting = 0;
//void (*user_onRequest)(void);
//void (*user_onReceive)(int);


// Public Methods //////////////////////////////////////////////////////////////

void wire_begin_1(void)
{
//  rxBufferIndex = 0;
//  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;

  twi_init();
}

//void wire_begin(uint8_t address)
//{
//  twi_setAddress(address);
//  twi_attachSlaveTxEvent(onRequestService);
//  twi_attachSlaveRxEvent(onReceiveService);
//  wire_begin_1();
//}

void wire_setClock(uint32_t frequency)
{
  TWBR = ((F_CPU / frequency) - 16) / 2;
}

//uint8_t wire_requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
//{
  // clamp to buffer length
//  if(quantity > BUFFER_LENGTH){
//    quantity = BUFFER_LENGTH;
//  }
  // perform blocking read into buffer
//  uint8_t read = twi_readFrom(address, rxBuffer, quantity, sendStop);
  // set rx buffer iterator vars
//  rxBufferIndex = 0;
//  rxBufferLength = read;
//  return read;
//}

void wire_beginTransmission(uint8_t address)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to 
//	perform a repeated start. 
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t wire_endTransmission(uint8_t sendStop)
{
  // transmit buffer (blocking)
  int8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop);
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
  // indicate that we are done transmitting
  transmitting = 0;
  return ret;
}

//	This provides backwards compatibility with the original
//	definition, and expected behaviour, of endTransmission
//
//uint8_t wire_endTransmission(void)
//{
//  return wire_endTransmission(true);
//}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t wire_write_1(uint8_t data)
{
  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(txBufferLength >= BUFFER_LENGTH){
//      wire_setWriteError();
      return 0;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer   
    txBufferLength = txBufferIndex;
  }else{
  // in slave send mode
    // reply to master
    twi_transmit(&data, 1);
  }
  return 1;
}

// must be called in:
// slave tx event callback
// or after beginTransmission(address)
//size_t wire_write(const uint8_t *data, size_t quantity)
//{
//  if(transmitting){
  // in master transmitter mode
//    for(size_t i = 0; i < quantity; ++i){
//      wire_write_1(data[i]);
//    }
//  }else{
  // in slave send mode
    // reply to master
//    twi_transmit(data, quantity);
//  }
//  return quantity;
//}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
//int wire_available(void)
//{
//  return rxBufferLength - rxBufferIndex;
//}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
//int wire_read(void)
//{
//  int value = -1;
    // get each successive byte on each call
//  if(rxBufferIndex < rxBufferLength){
//    value = rxBuffer[rxBufferIndex];
//    ++rxBufferIndex;
//  }
//  return value;
//}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
//int wire_peek(void)
//{
//  int value = -1;
//  if(rxBufferIndex < rxBufferLength){
//    value = rxBuffer[rxBufferIndex];
//  }
//  return value;
//}

//void wire_flush(void)
//{
  // XXX: to be implemented.
//}

// behind the scenes function that is called when data is received
//void wire_onReceiveService(uint8_t* inBytes, int numBytes)
//{
  // don't bother if user hasn't registered a callback
//  if(!user_onReceive){
//    return;
//  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
//  if(rxBufferIndex < rxBufferLength){
//    return;
//  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
//  for(uint8_t i = 0; i < numBytes; ++i){
//    rxBuffer[i] = inBytes[i];    
//  }
  // set rx iterator vars
//  rxBufferIndex = 0;
//  rxBufferLength = numBytes;
  // alert user program
//  wire_user_onReceive(numBytes);
//}

// behind the scenes function that is called when data is requested
//void wire_onRequestService(void)
//{
  // don't bother if user hasn't registered a callback
//  if(!user_onRequest){
//    return;
//  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
//  txBufferIndex = 0;
//  txBufferLength = 0;
  // alert user program
//  user_onRequest();
//}

// sets function called on slave write
//void wire_onReceive( void (*function)(int) )
//{
//  user_onReceive = function;
//}

// sets function called on slave read
//void wire_onRequest( void (*function)(void) )
//{
//  user_onRequest = function;
//}


