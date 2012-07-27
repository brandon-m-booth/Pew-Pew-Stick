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

#define MAX_NUM_DIGITS (3)
uint8_t digitPins[MAX_NUM_DIGITS];
void set_digit_bits(uint8_t digitIndex, uint8_t numberInHex);

void init_seven_segment_display(void)
{
  // Setup output pins
  DDRB |= PIN_00|PIN_01|PIN_02;
  DDRD |= PIN_00|PIN_01;

  // Enable SPI, set to Master mode, set clock rate to fck/16
  SPCR |= (1<<SPE)|(1<<MSTR);//|(1<<SPR0);

  // Set SCK frequency to fOSC/2.
  //SPSR |= (1<<SPI2X);

  // Clear the serial shift registers
  PORTD &= ~PIN_01;
  PORTD |= PIN_01;

  // Enable the slave SPI device set the parallel load low.
  PORTB &= ~PIN_00;
  PORTD &= ~PIN_00;

  clear_seven_segment_display();
}

void clear_seven_segment_display(void)
{
  for (int i = 0; i < MAX_NUM_DIGITS; ++i)
  {
    digitPins[i] = 0b00000000;
  }

  // Write out the current seven-segment state
  SPDR = digitPins[0];

  // Wait for transmission to complete
  while(!(SPSR & (1<<SPIF))) {}

  // Load the serial bits into the parallel output pins
  PORTD |= PIN_00;

  // Return the parallel load pin to low
  PORTD &= ~PIN_00;
}

void set_seven_segment_display_number(uint16_t number)
{
  int8_t digitsCounter = 0;
  for (uint16_t digitsLeft = number; digitsCounter < MAX_NUM_DIGITS; digitsLeft /= 10)
  {
    set_digit_bits(digitsCounter, digitsLeft%10);
    ++digitsCounter;
  }

  for (int i = 0; i < MAX_NUM_DIGITS; ++i)
  {
	  // Write out the current seven-segment state
	  SPDR = digitPins[i];

	  // Wait for transmission to complete
	  while(!(SPSR & (1<<SPIF))) {}
  }

  // Load the serial bits into the parallel output pins
  PORTD |= PIN_00;

  // Return the parallel load pin to low
  PORTD &= ~PIN_00;
}

// Helper function to set the bits in the serial stream to make the target digit at the given
// index display the decimal equivalent of the input hex number.
//
// Assumes the pins are mapped to the seven segment display in the following way:
//
// Bit order:    0, 1, 2, 3, 4, 5, 6, 7
// Pin mapping:  H, G, F, E, D, C, B, A
// Pin arrangement:
//                    _c__
//                  b|    |d
//                   |_a__|
//                  g|    |e
//                   |_f__| .h
//
void set_digit_bits(uint8_t digitIndex, uint8_t numberInHex)
{
    if (digitIndex >= MAX_NUM_DIGITS)
        return;

    switch(numberInHex)
    {
    case 0x0:
        digitPins[digitIndex] = 0b01111110;
        break;
    case 0x1:
        digitPins[digitIndex] = 0b00011000;
        break;
    case 0x2:
        digitPins[digitIndex] = 0b01101101;
        break;
    case 0x3:
        digitPins[digitIndex] = 0b00111101;
        break;
    case 0x4:
        digitPins[digitIndex] = 0b00011011;
        break;
    case 0x5:
        digitPins[digitIndex] = 0b00110111;
        break;
    case 0x6:
        digitPins[digitIndex] = 0b01110111;
        break;
    case 0x7:
        digitPins[digitIndex] = 0b00011100;
        break;
    case 0x8:
        digitPins[digitIndex] = 0b01111111;
        break;
    case 0x9:
        digitPins[digitIndex] = 0b00111111;
        break;
    case 0xA:
        digitPins[digitIndex] = 0b01011111;
        break;
    case 0xB:
        digitPins[digitIndex] = 0b01110011;
        break;
    case 0xC:
        digitPins[digitIndex] = 0b01100110;
        break;
    case 0xD:
        digitPins[digitIndex] = 0b01111001;
        break;
    case 0xE:
        digitPins[digitIndex] = 0b01100111;
        break;
    case 0xF:
        digitPins[digitIndex] = 0b01000111;
        break;
    default:
        // Turn them all on (why not?)!
        digitPins[digitIndex] = 0b11111111;
        break;
    }
}