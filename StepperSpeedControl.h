//  Open BVM Ventilator - Stepper acceleration with position and RPM speed control
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

#define INVERT_STEP_DIRECTION

const float STEP_ANGLE = 1.8;
const float MICRO_STEPS = 8;
const float STEPS_PER_REVOLUTION = (360.0 / STEP_ANGLE * MICRO_STEPS);

#define DEFAULT_ACCEL 300
#define MIN_DELAY 100
#define MAX_RPM 400

#define RPM_TO_STEPS_PER_SECOND(rpm) (STEPS_PER_REVOLUTION / 60.0 * rpm)
#define STEPS_PER_SECOND_TO_MICROS(sps) (1000000.0 / sps)

struct stp_t {
  volatile int8_t i;        // direction (1 or -1)
  volatile long p;          // current position
  volatile unsigned long c; // current step count
  volatile unsigned long tc;// target step count
  volatile bool done;       // true when stepper is at target position

  volatile float d;         // step delay
  unsigned int c0;          // step count zero delay
  float c1;                 // step count one delay
  volatile unsigned long n; // ramp count
  volatile unsigned long td;// absolute target millisecond delay
  volatile long ms;         // signed target millisecond delay
  volatile bool atTargetRpm;// true when stepper is at target RPM
};

extern stp_t stp;

void stepper_setup();
void enable_stepper();
void disable_stepper();

void set_acceleration(unsigned long accel);

void set_rpm(float rpm, bool wait = true);
inline void update_rpm(float rpm, bool wait) { if(stp.ms) set_rpm(rpm, wait); }

void set_speed(float r, unsigned d = 1000);

void home_stop();
void emergency_stop();

void move_n_steps(long steps, float rpm, bool wait = true);
void move_to_position(long p, float rpm, bool wait = true);
