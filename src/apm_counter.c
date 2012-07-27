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

uint16_t apmCounter;
uint8_t previousInputState[NUM_CONTROLLER_STATE_BYTES];

void init_apm_counter(void)
{
  reset_apm_counter();
}

void reset_apm_counter(void)
{
  apmCounter = 0;
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

  apmCounter += numActions;

  set_seven_segment_display_number(apmCounter);
}