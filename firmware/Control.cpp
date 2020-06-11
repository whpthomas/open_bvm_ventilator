//  Open BVM Ventilator - Ventilator state machine and control functions
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
#include <EEPROM.h>
#include <FastIO.h>
#include "options.h"
#include "Pins.h"
#include "ButtonDebouncer.h"
#include "Control.h"
#include "Draw.h"
#include "StepperSpeedControl.h"

page_t page;
level_t level;
selection_t selection;
alarm_t alarm;
events_t events;
phase_t phase;
limit_t limit;
ctrl_t ctrl;
live_t live;

void live_setup();

void ctrl_setup()
{
  alarm = NO_ALARM;
  memset(&events, 0, sizeof(events_t));
  memset(&live, 0, sizeof(live_t));

  EEPROM.get(0, ctrl);
  EEPROM.get(sizeof(ctrl_t), limit);
  live_setup();
}

void live_setup()
{
  live_full_press_steps();
  live_breath_cycle_time();
  live_inspiratory_time();
  live_minute_ventilation();

  live.volume = ctrl.tidalVolume;
  live.pressure = ctrl.plateauAirwayPressure;
  live.audibleAlarm = ctrl.ventilationActive;
}

/* START POSITION
 * Step offset of start position
 */
 
void ctrl_start_position(unsigned value) {
  ctrl.startPosition = value;
  //DebugMessage("ctrl.startPosition = ", ctrl.startPosition);
  live_full_press_steps();
#ifdef TORQUE_RAMP
  live_first_position();
  live_second_position();
#endif
}

/* PRESS VOLUME
 * Volume of air in a full press
 * This value should be calibrated for each vendors bag
 */
 
void ctrl_full_press_volume(unsigned value) {
  ctrl.fullPressVolume = value;
  //DebugMessage("ctrl.fullPressVolume = ", ctrl.fullPressVolume);
  live_tital_end_position();
  live_volume_per_revolution();
#ifdef TORQUE_RAMP
  live_first_position();
  live_second_position();
#endif
}

/* TIDAL VOLUME
 * Volume of air in a breath 
 */

void ctrl_tidal_volume(unsigned value) {
  live.volume = ctrl.tidalVolume = value;
  //DebugMessage("ctrl.tidalVolume = ", ctrl.tidalVolume);
  live_tital_end_position();
  live_minute_ventilation();
}

/* RESPIRATORY RATE
 * Number of breaths per minute
 */

void ctrl_respiratory_rate(unsigned value) {
  ctrl.respiratoryRate = value;
  //DebugMessage("ctrl.respiratoryRate = ", ctrl.respiratoryRate);
  live_breath_cycle_time();
  live_inspiratory_time();
  live_minute_ventilation();
}

/* RESPIRATORY RATIO
 * Ratio of inspiratory to expiratory time
 */

void ctrl_respiratory_ratio(unsigned value) {
  ctrl.respiratoryRatio = value;
  //DebugMessage("ctrl.respiratoryRatio = ", ctrl.respiratoryRatio);
  live_inspiratory_time();
}

/* PLATEAU AIRWAY PRESSURE
 * Maximum inspiratory pressure
 */

void ctrl_plateau_airway_pressure(unsigned value) {
  ctrl.plateauAirwayPressure = value;
  //DebugMessage("ctrl.plateauAirwayPressure = ", ctrl.plateauAirwayPressure);
}

/* INSPIRATORY FLOW
 * Peak inspiratory flow of air 
 */

void ctrl_inspiratory_flow(unsigned value){
  ctrl.inspiratoryFlow = value;
  //DebugMessage("ctrl.inspiratoryFlow = ", ctrl.inspiratoryFlow);
  float volumePerRevolution = (unsigned long)ctrl.fullPressVolume * STEPS_PER_REVOLUTION / live.fullPressSteps ;
  //DebugMessage("volumePerRevolution = ", volumePerRevolution);
  live.inspiratoryRpm = (unsigned long)ctrl.inspiratoryFlow * 1000 / volumePerRevolution;
  //DebugMessage("live.inspiratoryRpm = ", live.inspiratoryRpm);
}

/* EXPIRATORY FLOW
 * Peak expiratory flow of air 
 */

void ctrl_expiratory_flow(unsigned value){
  ctrl.expiratoryFlow = value;
  //DebugMessage("ctrl.expiratoryFlow = ", ctrl.expiratoryFlow);
  float volumePerRevolution = (unsigned long)ctrl.fullPressVolume * STEPS_PER_REVOLUTION / live.fullPressSteps ;
  //DebugMessage("volumePerRevolution = ", volumePerRevolution);
  live.expiratoryRpm = (unsigned long)ctrl.expiratoryFlow * 1000 / volumePerRevolution;
  //DebugMessage("live.expiratoryRpm = ", live.expiratoryRpm);
}

/* PRESSURE TRIGGER
 * Inspiratory breath pressure trigger 
 */

void ctrl_pressure_trigger(unsigned value) {
  ctrl.triggerPressure = value;
  //DebugMessage("ctrl.triggerPressure = ", ctrl.triggerPressure);
}

/* VOLUME PER REVOLUTION
 * An interum value used to calculate stepper motor RPM
 */
 
void live_volume_per_revolution()
{
  float volumePerRevolution = (unsigned long)ctrl.fullPressVolume * STEPS_PER_REVOLUTION / live.fullPressSteps ;
  //DebugMessage("volumePerRevolution = ", volumePerRevolution);
  live.inspiratoryRpm = (unsigned long)ctrl.inspiratoryFlow * 1000 / volumePerRevolution;
  //DebugMessage("live.inspiratoryRpm = ", live.inspiratoryRpm);
  live.expiratoryRpm = (unsigned long)ctrl.expiratoryFlow * 1000 / volumePerRevolution;  
  //DebugMessage("live.expiratoryRpm = ", live.expiratoryRpm);
}

/* BREATH CYCLE TIME
 * Number of milliseconds for a breath cycle
 */

void live_breath_cycle_time() {
  live.breathCycleTime = 60000 / ctrl.respiratoryRate;
  //DebugMessage("live.breathCycleTime = ", live.breathCycleTime);
}

/* INSPIRATORY TIME
 * Number of milliseconds for inspiration
 */

void live_inspiratory_time() {
  live.inspiratoryTime = live.breathCycleTime / (ctrl.respiratoryRatio + 1);
  //DebugMessage("live.inspiratoryTime = ", live.inspiratoryTime);
}

/* INSPIRATORY TIME
 * Calculated tidal volume
 */
unsigned volume_ml() {
  unsigned v = ((stp.p - ctrl.startPosition) * ctrl.fullPressVolume) / live.fullPressSteps;
  return v > 225 ?  (v * 2) - 550 : 0;  
}

/* MINUTE VENTILATION
 * Volume of air per minute
 */

void live_minute_ventilation() {
  live.minuteVentilation = ctrl.tidalVolume * ctrl.respiratoryRate;
  //DebugMessage("live.minuteVentilation = ", live.minuteVentilation);
}

/* TIDAL STEPS
 * Number of steps in a breath
 */

unsigned tidal_steps(unsigned volume) {
  unsigned v = volume + 275 - (volume / 2);
  return ((unsigned long)live.fullPressSteps * v) / ctrl.fullPressVolume;
}

void live_tital_end_position() { 
  live.tidalEndPosition = ctrl.startPosition + tidal_steps(ctrl.tidalVolume); 
  DebugMessage("live.tidalEndPosition = ", live.tidalEndPosition);
}

#ifdef TORQUE_RAMP
void live_first_position() { 
  live.firstPosition = ctrl.startPosition + tidal_steps(FIRST_POSITION_VOLUME);
}

void live_second_position() { 
  live.secondPosition = ctrl.startPosition + tidal_steps(SECOND_POSITION_VOLUME);
}
#endif


/* PRESS STEPS
 * Number of steps in a full press 
 */

void live_full_press_steps() {
  live.fullPressSteps = END_POSITION - ctrl.startPosition;
  //DebugMessage("live.fullPressSteps = ", live.fullPressSteps);
  //DebugMessage("STEPS_PER_ROTATION = ", STEPS_PER_ROTATION); // 45454
  //DebugMessage("END_POSITION = ", END_POSITION); // 12626
  live_tital_end_position();
  live_volume_per_revolution();
#ifdef TORQUE_RAMP
  live_first_position();
  live_second_position();
#endif
}

int clamp_input_value(int value, int step, int8_t dir, int lo, int hi) {
  if(dir > 0) {
    value += step;
  }
  else {
    value -= step;
  }
  if(value < lo) value = lo;
  if(value > hi) value = hi;
  return value;
}

void trigger_alarm(alarm_t a)
{
  if(alarm != a) {
    alarm = a;
    events.list[4] = events.list[3];
    events.list[3] = events.list[2];
    events.list[2] = events.list[1];
    events.list[1] = events.list[0];
    events.list[0] = alarm;
    events.length++;
    if(events.length > MAX_EVENTS) events.length = MAX_EVENTS;
  }
  red_led();
}

void factory_reset()
{
  ctrl.startPosition = DEFAULT_START_POSITION;
  ctrl.fullPressVolume = DEFAULT_FULL_PRESS_VOLUME;
  ctrl.tidalVolume = DEFAULT_TIDAL_VOLUME;
  ctrl.respiratoryRate = DEFAULT_RESPIRATORY_RATE;
  ctrl.respiratoryRatio = DEFAULT_RESPIRATORY_RATIO;
  ctrl.plateauAirwayPressure = DEFAULT_PLATEAU_AIRWAY_PRESSURE;
  ctrl.inspiratoryFlow = DEFAULT_INSPIRATORY_FLOW;
  ctrl.expiratoryFlow = DEFAULT_EXPIRATORY_FLOW;
  ctrl.triggerPressure = DEFAULT_TRIGGER_PRESSURE;
  ctrl.ventilationActive = false;

  limit.minimum.pressure = DEFAULT_MINIMUM_PRESSURE;
  limit.maximum.pressure = DEFAULT_MAXIMUM_PRESSURE;
  limit.minimum.ventilation = DEFAULT_MINIMUM_MINUTE_VENTILATION;
  limit.maximum.ventilation = DEFAULT_MAXIMUM_MINUTE_VENTILATION;
  limit.minimum.volume = DEFAULT_MINIMUM_VOLUME;
  limit.maximum.volume = DEFAULT_MAXIMUM_VOLUME;

  live_setup();
  
  EEPROM.put(0, ctrl);
  EEPROM.put(sizeof(ctrl_t), limit);
}
