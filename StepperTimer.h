//  Open BVM Ventilator - Stepper acceleration with position and RPM speed control timer
//
//  Created by WHPThomas <me(at)henri(dot)net> on 20/02/20.
//  Copyright (c) 2020 WHPThomas
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#pragma once

#define TIMER1_RESOLUTION 65536UL

static inline void set_timer1(unsigned long microseconds) { 
  const unsigned long cycles = (F_CPU / 1000000 * microseconds);
  unsigned short ticks;
  unsigned char selectBits;
  
  if (cycles < TIMER1_RESOLUTION) {
    selectBits  = (1 << CS10);
    ticks = cycles;
  }
  else if (cycles < TIMER1_RESOLUTION * 8) {
    selectBits  = (1 << CS11);
    ticks = cycles / 8;
  }
  else if (cycles < TIMER1_RESOLUTION * 64) {
    selectBits  = (1 << CS11) | (1 << CS10);
    ticks = cycles / 64;
    
  }
  else if (cycles < TIMER1_RESOLUTION * 256) {
    selectBits  = (1 << CS12);
    ticks = cycles / 256;
  }
  else if (cycles < TIMER1_RESOLUTION * 1024) {
    selectBits  = (1 << CS12) | (1 << CS10);
    ticks = cycles / 1024;
  } 
  else {
    selectBits  = (1 << CS12) | (1 << CS10);
    ticks = TIMER1_RESOLUTION - 1;
  }
  OCR1A = ticks;
  TCCR1B = selectBits | (1 << WGM12); // CTC mode
}

static inline void timer1_start()
{
  TIMSK1 |=  (1 << OCIE1A);
}

static inline void timer1_stop()
{
  TIMSK1 &= ~(1 << OCIE1A);
}

void timer1_setup();
