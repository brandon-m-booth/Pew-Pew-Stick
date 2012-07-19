/*
  Pew Pew Stick Microcontroller Code
  Copyright (c) 2012, Matt Stine, Brandon Booth
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met: 

  1. Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer. 
  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution. 

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "seven_segment_display.h"
#include "macros.h"

void init_seven_segment_display(void)
{
  // Set SS, SCLK, and PIND_0 as output.
  DDRB = PORT_CONFIG_OUTPUT;
  DDRD = PORT_CONFIG_OUTPUT;
  //DDRB |= PIN_00|PIN_01|PIN_02;
  //DDRD |= PIN_00|PIN_01;

  // Enable SPI, set to Master mode, set clock rate to fck/16
  //SPCR |= (1<<SPE)|(1<<MSTR)|(1<<SPR0);
  //SPCR &= ~(1<<CPOL);

  // Set SCK frequency to fOSC/2.
  //SPSR |= (1<<SPI2X);

  // Clear the serial shift registers
  PORTD &= ~PIN_01;
  PORTD |= PIN_01;

  // Enable the slave SPI device set the parallel load low.
  PORTB &= ~PIN_00;
  PORTD &= ~PIN_00;

  // Load the serial bits into the parallel output pins
  PORTD |= PIN_00;

  // Return the parallel load pin to low
  PORTD &= ~PIN_00;
}

void set_seven_segment_display_number(uint16_t number)
{
  // TODO - Get the current state and translate into binary stream for the display

  //PORTB |= PIN_00; // Deselect slave device

  // Enable the slave SPI device
  //PORTB &= ~PIN_00;

  // Write out the current seven-segment state
  //SPDR = 0xFF;

  // Wait for transmission to complete
  //while(!(SPSR & (1<<SPIF))) {}

  PORTB |= PIN_02; // Write out serial bit high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high
  PORTB &= ~PIN_01; // Clock low
  PORTB |= PIN_01; // Clock high

  // Load the serial bits into the parallel output pins
  PORTD |= PIN_00;

  // Return the parallel load pin to low
  PORTD &= ~PIN_00;

  // Disable the slave SPI device
  //PORTB |= PIN_00;
}