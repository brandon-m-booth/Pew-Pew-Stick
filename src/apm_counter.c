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

#include "apm_counter.h"
#include "seven_segment_display.h"
#include "macros.h"
#include "pins.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t actionCounter;
volatile uint16_t apm; // Actions per second
const uint16_t runningAvgSamplePercent = 2; // Expressed as a whole number from [0-100]
uint8_t previousInputState[NUM_CONTROLLER_STATE_BYTES];

ISR(TIMER1_COMPA_vect)
{
  apm = (runningAvgSamplePercent*60*actionCounter + (100-runningAvgSamplePercent)*apm)/100;
  actionCounter = 0;
}

void init_apm_counter(void)
{
  reset_apm_counter();

  // Initialize timer counter 1 (16-bit) and enable a timer interrupt every second
  //GTCCR = 0x81;   // Reset timer prescaler
  TCCR1A = 0xC0;  // Set timer 1 to compare match A
  TCCR1B = 0x0C;  // Use clk/256 prescaler and CTC timer counting mode
  OCR1A = 0xF424; // Count to 62500 to generate a timer match
  GTCCR = 0x80;   // Set timer control register to not reset prescaler

  TIFR1 |= (1<<OCF1A);   // Reset interrupt trigger for timer1 compare A
  TIMSK1 |= (1<<OCIE1A); // Enable interrupts for timer1 compare A
}

void reset_apm_counter(void)
{
  TIMSK1 &= ~(1<<OCIE1A); // Disable timer 1 compare A interrupts
  actionCounter = 0;
  apm = 0;
  TIMSK1 |= (1<<OCIE1A);  // Enable the interrupt again
  for (int i = 0; i < NUM_CONTROLLER_STATE_BYTES; ++i)
  {
    previousInputState[i] = 0x00;
  }
}

void update_apm_counter(uint8_t* inputStates)
{
  uint8_t numActions = 0;
  for (int i = 0; i < NUM_CONTROLLER_STATE_BYTES; ++i)
  {
    uint8_t inputStatePresses = (inputStates[i] ^ previousInputState[i]) & inputStates[i];

    for (uint8_t mask = 0x01; mask != 0; mask = mask << 1)
    {
      numActions += (inputStatePresses & mask) ? 1 : 0;
    }

    previousInputState[i] = inputStates[i];
  }

  TIMSK1 &= ~(1<<OCIE1A); // Disable timer 1 compare A interrupts
  actionCounter += numActions;
  uint16_t displayApm = apm;
  TIMSK1 |= (1<<OCIE1A);  // Enable the interrupt again

  set_seven_segment_display_number(displayApm);
}