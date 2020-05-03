//  Open BVM Ventilator - Stepper acceleration with position and RPM speed control
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
#include "options.h"
#include "Pins.h"
#include "StepperTimer.h"
#include "StepperSpeedControl.h"

stp_t stp;

void stepper_setup()
{
  fastPinMode(STEP_PIN,   OUTPUT);
  fastPinMode(DIR_PIN,    OUTPUT);
  fastPinMode(ENABLE_PIN, OUTPUT);

  memset(&stp, 0, sizeof(stp_t));
  set_acceleration(DEFAULT_ACCEL);
}

void enable_stepper()
{
  fastDigitalWrite(ENABLE_PIN, LOW);
}

void disable_stepper()
{
  fastDigitalWrite(ENABLE_PIN, HIGH);
}

void set_acceleration(unsigned long accel)
{
  stp.c0 = 0.676 * sqrt(2.0 / accel) * 1000000.0 * 0.1; // compute step count 0 delay
  stp.c1 = stp.c0 - ((2 * stp.c0) / (4 * 1 + 1));       // derive step count 1 delay
}

static void set_delay(long microseconds) {
  // check 
  if(microseconds > 0) {
    if(microseconds < MIN_DELAY) microseconds = MIN_DELAY;
  }
  else if(microseconds < 0) {
    if(microseconds > -MIN_DELAY) microseconds = -MIN_DELAY;
  }

  if(stp.ms != microseconds) {
    stp.ms = microseconds;
    if(stp.ms == 0) {
      stp.td = stp.c0;    
    }
    else {
#ifdef INVERT_STEP_DIRECTION      
      if(stp.ms < 0) {
        fastDigitalWrite(DIR_PIN, HIGH);
        stp.td = -stp.ms;
      }
      else {
        fastDigitalWrite(DIR_PIN, LOW);
        stp.td = stp.ms;
      }
#else
      if(stp.ms < 0) {
        fastDigitalWrite(DIR_PIN, LOW);
        stp.td = -stp.ms;
      }
      else {
        fastDigitalWrite(DIR_PIN, HIGH);
        stp.td = stp.ms;
      }
#endif
    }
    stp.atTargetRpm = false;
    if(stp.n == 0) {
      stp.d = stp.c0;
      set_timer1(stp.c0);
      timer1_start();
    }
  }
}

void set_rpm(float rpm, bool wait)
{
  long microseconds = rpm ? STEPS_PER_SECOND_TO_MICROS(RPM_TO_STEPS_PER_SECOND(rpm)) : 0;
  unsigned long t = abs(microseconds);
  bool d1 = false;
  bool d2 = false;
  bool r1 = false;
  bool r2 = false;

  // check for reversal of direction
  if(rpm && stp.ms) {
    d1 = stp.ms < 0;
    d2 = rpm < 0;
  }

  // check for instant rpm transition
  if(t && stp.td) {
    r1 = stp.td > stp.c1;
    r2 = t > stp.c1;
  }

  if(d1 != d2 || r1 != r2) {
    set_delay(0);
    while(!stp.atTargetRpm);
  }
  set_delay(microseconds);
  //DebugMessage("microseconds = ", stp.ms);
  while(wait && !stp.atTargetRpm); 
}

void home_stop()
{
  if(stp.ms < 0) {
    emergency_stop();
    stp.p = 0;
  }
}

void emergency_stop()
{
  if(stp.ms) {
     //IF_DEBUG( debug.println("emergency stop"); )
    timer1_stop();            // stop timer
    stp.ms = 0;               // zero millisecond delay
    stp.n = 0;                // zero ramp count
    stp.d = stp.td = stp.c0;  // delay equals c0
    stp.atTargetRpm = true;   // target speed reached
    stp.done = true;
  }
}

void move_n_steps(long steps, float rpm, bool wait)
{
  if(steps) {
    if(rpm > MAX_RPM) rpm = MAX_RPM;
    stp.i = steps > 0 ? 1 : -1;
    stp.c = 0;
    stp.tc = abs(steps);
    stp.done = false;
    set_rpm(steps > 0 ? rpm : -rpm, false);
    while(wait && !stp.done);
  }
}

void set_speed(float r, unsigned d)
{
  set_rpm(r); 
  delay(d);
}

void move_to_position(long p, float rpm, bool wait)
{ 
  move_n_steps(p - stp.p, rpm, wait); 
}
