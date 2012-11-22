/*
Pew Pew Stick Microcontroller Code
Copyright (c) 2012, Matt Stine
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

#include "pins.h"
#include "macros.h"
#include "controller.h"
#include "device.h"
#include "input_filter.h"
#include "seven_segment_display.h"
#include "apm_counter.h"

uint8_t pins[NUM_CONTROLLER_STATE_BYTES];

int main(void)
{
  /* Set 16 MHz clock */
  CPU_PRESCALE(CPU_16MHz);
  LED_CONFIG;
  //LED_ON;
  DDRC |= PIN_07;

  struct Device device;
  struct Controller controller;
  struct InputFilter inputFilter;

  /* Initialize device */
  init_device(&device, GAME_PAD_DEVICE_TYPE);

  /* Initialize controller */
  init_controller(&controller, PARALLEL_TYPE);

  /* Initialize controller input filter */
  init_input_filter(&inputFilter);

  /* Initialize seven segment display */
  init_seven_segment_display();

  /* Initialize apm (actions per minute) counter */
  init_apm_counter();

  //LED_OFF;
  
  /* Main loop. */
  for(;;)
  {
    /* Get the current input state of the controller */
    get_controller_state(&controller, pins);

    /* Filter the raw input data */
    filter_input(&inputFilter, pins);

    /* Update the APM counter */
    update_apm_counter(pins);

    /* Update the device state and transmit the input state */
    update_device_pin_state(&device, pins);

    /* Turn on the LED if any button is pressed, otherwise turn it off */
    int i = 0;
    for (; i < NUM_CONTROLLER_STATE_BYTES; ++i)
    {
      if (pins[i] != 0)
      {
        //LED_ON;
        PORTC |= PIN_07;
        break;
      }
    }
    if (i == NUM_CONTROLLER_STATE_BYTES)
    {
      //LED_OFF;
      PORTC &= ~PIN_07;
    }
  }

}
