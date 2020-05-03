//  Open BVM Ventilator - RepRapDiscount Full Graphic Smart Controller encoder support
//
//  See: https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller
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

#include "Encoder.h"

#define INT_A 0
#define INT_B 1
#define PIN_A 2
#define PIN_B 3

struct encoder_t encoder;

/*  This code uses the AVR hardware interrupts. There are only two available on
 *  the AtMega328AU chip. So digital pins 2 and 3 (chip pins 32 and 1) must be 
 *  assigned to the encoder for this code to work.
 */

void IsrA(){
  cli(); //stop interrupts happening before we read pin values
  encoder.c = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(encoder.c == B00001100 && encoder.a) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoder.dir = 1; //decrement the encoder's position count
    encoder.b = 0; //reset flags for the next turn
    encoder.a = 0; //reset flags for the next turn
  }
  else if (encoder.c == B00000100) encoder.b = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void IsrB(){
  cli(); //stop interrupts happening before we read pin values
  encoder.c = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if(encoder.c == B00001100 && encoder.b) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoder.dir = -1; //increment the encoder's position count
    encoder.b = 0; //reset flags for the next turn
    encoder.a = 0; //reset flags for the next turn
  }
  else if (encoder.c == B00001000) encoder.a = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void encoder_setup() {
  fastPinMode(PIN_A, INPUT_PULLUP); // set PinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  fastPinMode(PIN_B, INPUT_PULLUP); // set PinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  attachInterrupt(INT_A, IsrA, RISING); // set an interrupt on PIN_A, looking for a rising edge signal and executing the "IsrA" Interrupt Service Routine (below)
  attachInterrupt(INT_B, IsrB, RISING); // set an interrupt on PIN_B, looking for a rising edge signal and executing the "IsrB" Interrupt Service Routine (below)
}
