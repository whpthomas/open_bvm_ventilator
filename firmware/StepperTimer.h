//  Open BVM Ventilator - Stepper acceleration with position and RPM speed control timer
//
//  Created by WHPThomas <me(at)henri(dot)net> on 20/02/20.
//  Copyright (c) 2020 WHPThomas
//
//  See: AVR446: Linear speed control of stepper motor
//       8-bit AVR Microcontrollers Application Note
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

#pragma once

#define TIMER1_RESOLUTION 65536UL

static inline void timer1_start()
{
  TIMSK1 |=  (1 << OCIE1A);
}

static inline void timer1_stop()
{
  TIMSK1 &= ~(1 << OCIE1A);
}

void timer1_setup();
void set_timer1(unsigned long microseconds);
