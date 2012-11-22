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

#include "keyboard_device.h"
#include "usb_keyboard.h"
#include <util/delay.h>

// Structures
struct KeySlotCacheDatum
{
  uint8_t controllerStateIndex;
  uint8_t buttonBitMask;
};

// Variables
#define MAX_KEY_SLOTS (6)
static uint8_t previousPins[NUM_CONTROLLER_STATE_BYTES] = {0};
struct KeySlotCacheDatum keySlotCache[MAX_KEY_SLOTS];

// Forward Declarations
void ProcessInputPin(uint8_t pins[NUM_CONTROLLER_STATE_BYTES], uint8_t controllerStateIndex, uint8_t buttonBitMask, uint8_t logicalKeyID);
int8_t GetKeySlot(uint8_t controllerStateIndex, uint8_t buttonBitMask);
int8_t ClaimEmptyKeySlot(uint8_t controllerStateIndex, uint8_t buttonBitMask);
void ReleaseKeySlot(int8_t keySlotIndex);

void init_keyboard_device(void)
{
  /* Initialize the game pad USB interface */
  usb_init();
  while (!usb_configured());

	/* Wait an extra second for the OS to finish loading the USB driver */
	_delay_ms(1000);

  /* Initialize the key slot cache */
  for (uint8_t i = 0; i < MAX_KEY_SLOTS; ++i)
  {
    ReleaseKeySlot(i);
  }
}

void update_keyboard_device_pin_state(uint8_t pins[NUM_CONTROLLER_STATE_BYTES])
{
  /* Joysick */
  ProcessInputPin(pins, 1, D_LT, KEY_A);
  ProcessInputPin(pins, 1, D_RT, KEY_D);
  ProcessInputPin(pins, 1, D_UP, KEY_W);
  ProcessInputPin(pins, 1, D_DN, KEY_S);

  /* Buttons */
  ProcessInputPin(pins, 1, B_01, KEY_1);
  ProcessInputPin(pins, 1, B_02, KEY_2);
  ProcessInputPin(pins, 1, B_03, KEY_3);
  ProcessInputPin(pins, 1, B_04, KEY_4);
  ProcessInputPin(pins, 0, B_05, KEY_5);
  ProcessInputPin(pins, 0, B_06, KEY_6);
  ProcessInputPin(pins, 0, B_07, KEY_7);
  ProcessInputPin(pins, 0, B_08, KEY_8);
  ProcessInputPin(pins, 0, B_09, KEY_9);
  ProcessInputPin(pins, 0, B_10, KEY_0);
  ProcessInputPin(pins, 0, B_11, KEY_C);
  ProcessInputPin(pins, 0, B_12, KEY_ESC);
}

void ProcessInputPin(uint8_t pins[NUM_CONTROLLER_STATE_BYTES], uint8_t controllerStateIndex, uint8_t buttonBitMask, uint8_t logicalKeyID)
{
  if (pins[controllerStateIndex] & buttonBitMask)
  {
    if ((previousPins[controllerStateIndex] & buttonBitMask) == 0)
    {
      int8_t keySlotIndex = ClaimEmptyKeySlot(controllerStateIndex, buttonBitMask);
      if (keySlotIndex >= 0)
      {
        usb_keyboard_press(KEY_SLOT_0 + keySlotIndex, logicalKeyID, KEY_NONE);
      }
    }
    previousPins[controllerStateIndex] |= buttonBitMask;
  }
  else
  {
    int8_t keySlotIndex = GetKeySlot(controllerStateIndex, buttonBitMask);
    if (keySlotIndex >= 0)
    {
      ReleaseKeySlot(keySlotIndex);
      usb_keyboard_release(KEY_SLOT_0 + keySlotIndex);
    }
    previousPins[controllerStateIndex] &= ~buttonBitMask;
  }
}

int8_t GetKeySlot(uint8_t controllerStateIndex, uint8_t buttonBitMask)
{
  for (int8_t i = 0; i < MAX_KEY_SLOTS; ++i)
  {
    if (keySlotCache[i].controllerStateIndex == controllerStateIndex && keySlotCache[i].buttonBitMask == buttonBitMask)
    {
      return i;
    }
  }

  return -1;
}

int8_t ClaimEmptyKeySlot(uint8_t controllerStateIndex, uint8_t buttonBitMask)
{
  for (int8_t i = 0; i < MAX_KEY_SLOTS; ++i)
  {
    if (keySlotCache[i].controllerStateIndex == 0 && keySlotCache[i].buttonBitMask == 0)
    {
      keySlotCache[i].controllerStateIndex = controllerStateIndex;
      keySlotCache[i].buttonBitMask = buttonBitMask;
      return i;
    }
  }

  return -1;
}

void ReleaseKeySlot(int8_t keySlotIndex)
{
  keySlotCache[keySlotIndex].controllerStateIndex = 0;
  keySlotCache[keySlotIndex].buttonBitMask = 0;
}