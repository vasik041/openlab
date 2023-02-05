/*
  TwoWire.h - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
           2023 by LZ to make plain C minimal version
*/

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

