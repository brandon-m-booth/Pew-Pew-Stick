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

#include "game_pad_device.h"
#include "usb_gamepad.h"
#include <util/delay.h>

/* Axis values over USB */
#define DIR_NULL (128)
#define DIR_LEFT (0)
#define DIR_RIGHT (255)
#define DIR_UP (0)
#define DIR_DOWN (255)

/* Button values over USB (two bytes) */
#define BUTTON_01 (1<<0)
#define BUTTON_02 (1<<1)
#define BUTTON_03 (1<<2)
#define BUTTON_04 (1<<3)
#define BUTTON_05 (1<<4)
#define BUTTON_06 (1<<5)
#define BUTTON_07 (1<<6)
#define BUTTON_08 (1<<7)
#define BUTTON_09 (1<<0)
#define BUTTON_10 (1<<1)
#define BUTTON_11 (1<<2)
#define BUTTON_12 (1<<3)

void init_game_pad_device(void)
{
  /* Initialize the game pad USB interface */
  usb_init();
  while (!usb_configured());

  /* Wait an extra second for the OS to finish loading the USB driver */
	_delay_ms(1000);
}

void update_game_pad_device_pin_state(uint8_t pins[NUM_CONTROLLER_STATE_BYTES])
{
  /* Joystick motion */
  uint8_t x = DIR_NULL;
  uint8_t y = DIR_NULL;
  if (pins[1] & D_LT)
    x = DIR_LEFT;
  else if (pins[1] & D_RT)
    x = DIR_RIGHT;
  if (pins[1] & D_UP)
    y = DIR_UP;
  else if (pins[1] & D_DN)
    y = DIR_DOWN;

  /* Button presses */
  uint8_t b[2] = {0};
  if (pins[1] & B_01)
    b[0] |= BUTTON_01;
  if (pins[1] & B_02)
    b[0] |= BUTTON_02;
  if (pins[1] & B_03)
    b[0] |= BUTTON_03;
  if (pins[1] & B_04)
    b[0] |= BUTTON_04;
  if (pins[0] & B_05)
    b[0] |= BUTTON_05;
  if (pins[0] & B_06)
    b[0] |= BUTTON_06;
  if (pins[0] & B_07)
    b[0] |= BUTTON_07;
  if (pins[0] & B_08)
    b[0] |= BUTTON_08;
  if (pins[0] & B_09)
    b[1] |= BUTTON_09;
  if (pins[0] & B_10)
    b[1] |= BUTTON_10;
  if (pins[0] & B_11)
    b[1] |= BUTTON_11;
  if (pins[0] & B_12)
    b[1] |= BUTTON_12;

  usb_gamepad_action(x, y, b);
}