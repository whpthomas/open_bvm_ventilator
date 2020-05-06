//  Open BVM Ventilator - Reliable button input debouncer
//
// See: https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
//
//  Created by WHPThomas <me(at)henri(dot)net> on 20/02/20.
//  Copyright (c) 2020 WHPThomas
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <Arduino.h>
#include "ButtonDebouncer.h"

ButtonDebouncer::ButtonDebouncer() {
  timer = 0;
  history = 0;
}

void ButtonDebouncer::update(bool state)
{
  unsigned long now = millis();
  if(now > timer) {
    history <<= 1;
    history |= state;
    timer = now + 30;
  }
}

bool ButtonDebouncer::pressed()
{
  return ((history & 0b11110001) == 0b00000001) ? (history = 0b11111111, true) : false;
}

bool ButtonDebouncer::released()
{
  return ((history & 0b10001111) == 0b10000000) ? (history = 0b00000000, true) : false;
}

bool ButtonDebouncer::down()
{
  return history == 0b11111111;
}

bool ButtonDebouncer::up()
{
  return history == 0b00000000;
}
