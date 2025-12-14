#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>

#define BUFFER_LENGTH 32

void wire_begin_1();
void wire_begin(uint8_t);
void wire_setClock(uint32_t);
void wire_beginTransmission(uint8_t);
uint8_t wire_endTransmission(uint8_t);
uint8_t wire_requestFrom(uint8_t, uint8_t, uint8_t);
size_t wire_write_1(uint8_t);
size_t wire_write(const uint8_t *, size_t);
int wire_available(void);
int wire_read(void);
int wire_peek(void);
void wire_flush(void);
void wire_onReceive( void (*)(int) );
void wire_onRequest( void (*)(void) );

#endif

