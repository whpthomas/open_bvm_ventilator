//  Open BVM Ventilator - Stepper acceleration with position and RPM speed control timer ISR
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
#include <FastIO.h>
#include "Pins.h"
#include "StepperTimer.h"
#include "StepperSpeedControl.h"

void timer1_setup()
{
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  interrupts();
}

ISR(TIMER1_COMPA_vect)
{
  fastDigitalWrite(STEP_PIN, HIGH);
  fastDigitalWrite(STEP_PIN, LOW);

  stp.c++;
  stp.p += stp.i;
  
  if(stp.td >= stp.c1 && stp.d >= stp.c1) { // instant rpm
    // if target delay zero
    if(stp.ms == 0) { // instant stop
      stp.n = 0;      // zero ramp count
      stp.d = stp.c0; // delay equals c0
      stp.atTargetRpm = true; // target speed reached
      stp.done = true;
      timer1_stop();  // stop timer
      return;
    }
    else if(stp.d != stp.td) { // instant start
      stp.n = 1;      // set ramp count 
      stp.d = stp.td;  // delay equals target delay
      stp.atTargetRpm = true; // target speed reached
    }    
  }
  else if(stp.d > stp.td) { // ramp up phase
    stp.n++;          // increment ramp count
    stp.d = stp.d - ((2 * stp.d) / (4 * stp.n + 1)); // update delay
    if(stp.d <= stp.td) { // if delay less than target delay
      stp.d = stp.td;  // delay equals target delay
      stp.atTargetRpm = true; // target speed reached
    }
  }
  else if(stp.d < stp.td) { // ramp down phase
    stp.n--;          // decrement ramp count
    stp.d = (stp.d * (4 * stp.n + 1)) / (4 * stp.n - 1); // update delay
    if(stp.d >= stp.td) { // if delay greater than target delay
      stp.d = stp.td;  // delay equals target delay
      stp.atTargetRpm = true; // target speed reached
    }
  }
  set_timer1(stp.d);
  
  if(stp.n == 0) {  // if ramp count zero
    stp.atTargetRpm = true; // target speed reached
    stp.done = true;
    timer1_stop();  // stop timer
  }
  else if(stp.ms && stp.c >= stp.tc - stp.n) { // ramp down step count
    stp.ms = 0; // signal ramp down
    stp.td = stp.c0; // target delay is c0
    stp.atTargetRpm = false;
  }
}
