//  Open BVM Ventilator - firmware
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
#include <BME280I2C.h>
#include <Wire.h>
#include <avr/wdt.h>

#include "Pins.h"
#include "ButtonDebouncer.h"
#include "Control.h"
#include "Draw.h"
#include "Encoder.h"
#include "StepperTimer.h"
#include "StepperSpeedControl.h"

ButtonDebouncer rotaryButton;
ButtonDebouncer stopButton;
ButtonDebouncer homeEndstop;

BME280I2C::Settings settings(
   BME280::OSR_X2,
   BME280::OSR_X1,
   BME280::OSR_X4,
   BME280::Mode_Forced,
   BME280::StandbyTime_500us,
   BME280::Filter_2,
   BME280::SpiEnable_False,
   BME280I2C::I2CAddr_0x76
);

BME280I2C bme(settings);

void select_home_page();
int read_pressure();
void graph_pressure();
void check_pressure();

void setup() {
  IF_DEBUG( debug.begin(9600); )
  stepper_setup();
  timer1_setup();
  ctrl_setup();
  draw_setup();  
  encoder_setup();

  Wire.begin();
  if(!bme.begin()) {
    Serial.println("Could not find BME280I2C sensor!");
    delay(1000);    
  }
  delay(500);
  live.zeroPressure = read_pressure();

  IF_DEBUG( debug.println("Started"); )
  fastPinMode(ROTARY_BUTTON_PIN, INPUT_PULLUP);
  fastPinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
  fastPinMode(HOME_ENDSTOP_PIN, INPUT_PULLUP);
  phase = HOME_PHASE;

  if(ctrl.ventilationActive) trigger_alarm(POWER_FAILURE_ALARM);
}

void loop()
{
  unsigned long now = millis();
  static unsigned long redrawTimer;
  static unsigned long inspiratoryTimer;
  static unsigned long breathCycleTimer;
  static unsigned long lastBreathCycle;
  static bool makeSound = false;
  
  /* ROTARY ENCODER BEHAVIOR
   * If value level, change the selection input value within range
   * If menu level, change the selection on the current page
   */

  if(encoder.dir) {
     //IF_DEBUG( debug.println(encoder.dir > 0 ? "clockwise" : "counter clockwise"); )
    switch(level) {
      default: // encoder changes control value 
        switch(selection) {
          /* HOME_PAGE */
          case RESPIRATORY_RATE:
            ctrl_respiratory_rate(clamp_input_value(ctrl.respiratoryRate, 1, encoder.dir, MINIMUM_RESPIRATORY_RATE, MAXIMUM_RESPIRATORY_RATE));
            break;
          case TIDAL_VOLUME:
            ctrl_tidal_volume(clamp_input_value(ctrl.tidalVolume, VOLUME_INCREMENT, encoder.dir, limit.minimum.volume, limit.maximum.volume < ctrl.fullPressVolume ? limit.maximum.volume : ctrl.fullPressVolume));
            break;
          case PLATEAU_AIRWAY_PRESSURE:
            ctrl_plateau_airway_pressure(clamp_input_value(ctrl.plateauAirwayPressure, PLATEAU_AIRWAY_PRESSURE_INCREMENT, encoder.dir, limit.minimum.pressure, limit.maximum.pressure));
            break;

          /* CONTROLS_PAGE */
          case RESPIRATORY_RATIO:
            ctrl_respiratory_ratio(clamp_input_value(ctrl.respiratoryRatio, 1, encoder.dir, MINIMUM_RESPIRATORY_RATIO, MAXIMUM_RESPIRATORY_RATIO));
            break;
          case TRIGGER_PRESSURE:
            ctrl_pressure_trigger(clamp_input_value(ctrl.triggerPressure, 1, encoder.dir, MINIMUM_TRIGGER_PRESSURE, MAXIMUM_TRIGGER_PRESSURE));
            break;
          case INSPIRATORY_FLOW:
            ctrl_inspiratory_flow(clamp_input_value(ctrl.inspiratoryFlow, 1, encoder.dir, MINIMUM_INSPIRATORY_FLOW, MAXIMUM_INSPIRATORY_FLOW));
            break;
          case EXPIRATORY_FLOW:
            ctrl_expiratory_flow(clamp_input_value(ctrl.expiratoryFlow, 1, encoder.dir, MINIMUM_EXPIRATORY_FLOW, MAXIMUM_EXPIRATORY_FLOW));
            break;

          /* SETUP PAGE */
          case START_STOP_VENTILATION:
            break;
          case PRESSURE_TEST:
            break;

          /* LIMITS_PAGE */
          case PRESSURE_LIMIT:
            if(level == OTHER_VALUE_LEVEL) {
              limit_minimum_pressure(clamp_input_value(limit.minimum.pressure, 1, encoder.dir, MINIMUM_PRESSURE, MAXIMUM_MINIMUM_PRESSURE));
            }
            else {
              limit_maximum_pressure(clamp_input_value(limit.maximum.pressure, 1, encoder.dir, MINIMUM_MAXIMUM_PRESSURE, MAXIMUM_PRESSURE));              
            }
            break;
          case MINUTE_VENTILATION_LIMIT:
            if(level == OTHER_VALUE_LEVEL) {
              limit_minimum_ventilation(clamp_input_value(limit.minimum.ventilation, MINUTE_VENTILATION_INCREMENT, encoder.dir, MINIMUM_MINUTE_VENTILATION, MAXIMUM_MINIMUM_MINUTE_VENTILATION));
            }
            else {
              limit_maximum_ventilation(clamp_input_value(limit.maximum.ventilation, MINUTE_VENTILATION_INCREMENT, encoder.dir, MINIMUM_MAXIMUM_MINUTE_VENTILATION, MAXIMUM_MINUTE_VENTILATION));              
            }
            break;
          case TIDAL_VOLUME_LIMIT:
            if(level == OTHER_VALUE_LEVEL) {
              limit_minimum_volume(clamp_input_value(limit.minimum.volume, VOLUME_INCREMENT, encoder.dir, MINIMUM_VOLUME, ctrl.fullPressVolume));
            }
            else {
              limit_maximum_volume(clamp_input_value(limit.maximum.volume, VOLUME_INCREMENT, encoder.dir, MINIMUM_MAXIMUM_VOLUME, ctrl.fullPressVolume));              
            }
            break;

          /* SYSTEM_PAGE */
          case AUDIBLE_ALARM:
            break;
          case START_POSITION:
            ctrl_start_position(clamp_input_value(ctrl.startPosition, START_POSITION_INCREMENT, encoder.dir, MINIMUM_START_POSITION, MAXIMUM_START_POSITION));
            if(phase == NO_PHASE) {
              enable_stepper();
              move_to_position(ctrl.startPosition, live.expiratoryRpm, true);
              disable_stepper();
            }
            break;
          case FULL_PRESS_VOLUME:
            ctrl_full_press_volume(clamp_input_value(ctrl.fullPressVolume, FULL_PRESS_VOLUME_INCREMENT, encoder.dir, MINIMUM_FULL_PRESS_VOLUME, MAXIMUM_FULL_PRESS_VOLUME));
            break;
          case FACTORY_RESET:
            break;
          case RESTART:
            break;
        }
        break;

      case MENU_LEVEL: // encoder changes menu selection
        switch(page) {
          case HOME_PAGE:
            level = VALUE_LEVEL;
            break;
          case SELECT_PAGE:
            menu = clamp_input_value(menu, 1, encoder.dir, SELECT_MENU_MIN, SELECT_MENU_MAX);
            break;
          case EVENTS_PAGE:
            break;
          case CONTROLS_PAGE:
            selection = (selection_t)clamp_input_value(selection, 1, encoder.dir, CONTROLS_SELECTION_MIN, CONTROLS_SELECTION_MAX);
            break;
          case SETUP_PAGE:
            selection = (selection_t)clamp_input_value(selection, 1, encoder.dir, SETUP_SELECTION_MIN, ctrl.ventilationActive ? SETUP_SELECTION_MIN : SETUP_SELECTION_MAX);
            break;
          case LIMITS_PAGE:
            selection = (selection_t)clamp_input_value(selection, 1, encoder.dir, LIMITS_SELECTION_MIN, LIMITS_SELECTION_MAX);
            break;
          case SYSTEM_PAGE:
            selection = (selection_t)clamp_input_value(selection, 1, encoder.dir, SYSTEM_SELECTION_MIN, SYSTEM_SELECTION_MAX);
            break;
        }
    }
    encoder.dir = 0; // clear encoder pulse
    redraw = true;
  }
  
  /* ROTARY BUTTON BEHAVIOR
   * If home page, increment selection
   * If select page, select page
   * Else select value
   */
  
  rotaryButton.update(fastDigitalRead(ROTARY_BUTTON_PIN) ? 0 : 1);
  if(rotaryButton.pressed()) {
     //IF_DEBUG( debug.println("rotary button"); )
    switch(page) {
      case HOME_PAGE: // rotary button changes home page selection
        selection = (selection_t)(selection + 1);
        if(selection > HOME_SELECTION_MAX) {
          selection = HOME_SELECTION_MIN;
        }
        break;
      case SELECT_PAGE: // rotary button selects page
        page = (page_t)menu;
        switch(page) {
          case CONTROLS_PAGE:
            selection = CONTROLS_SELECTION_MIN;
            break;
          case SETUP_PAGE:
            selection = SETUP_SELECTION_MIN;
            break;
          case LIMITS_PAGE:
            selection = LIMITS_SELECTION_MIN;
            break;
          case SYSTEM_PAGE:
            selection = SYSTEM_SELECTION_MIN;
            break;
          default:
            break;
        }
        break;
      case EVENTS_PAGE:
        events.length = 0;
        green_led();
        select_home_page();
        break;
      case LIMITS_PAGE: // rotary button toggles minimum and maximum limits
        switch(level) {
          case OTHER_VALUE_LEVEL:
            level = VALUE_LEVEL;
            break;
          case VALUE_LEVEL:
            level = MENU_LEVEL;
            break;
          case MENU_LEVEL:
            level = OTHER_VALUE_LEVEL;
            break;
        }
        break;
      case SETUP_PAGE:
        if(selection == START_STOP_VENTILATION) {
          ctrl.ventilationActive = !ctrl.ventilationActive;
          select_home_page();
        }
        else if(!ctrl.ventilationActive && selection == PRESSURE_TEST) {
          live.rpmRamp = 0;
          live.peakPressure = 0;
          enable_stepper();
          move_to_position(live.tidalEndPosition, live.inspiratoryRpm, false);
          while(!stp.done) {
            live.pressure = read_pressure();
            check_pressure();
            if(redrawTimer < now) {
              redrawTimer = now + 200;
              graph_pressure();
            }
            if(redraw) {
              draw_update();
            }
          }
          DebugMessage("stp.p = ", stp.p);  
          move_to_position(ctrl.startPosition, live.expiratoryRpm, false);
        }
        break;
      case SYSTEM_PAGE:
        if(selection == AUDIBLE_ALARM) {
          live.audibleAlarm = !live.audibleAlarm;
          select_home_page();
          break;
        }
        else if(selection == FACTORY_RESET) {
          factory_reset();
          page = HOME_PAGE;
          level = VALUE_LEVEL;
          selection = HOME_SELECTION_MIN;
          break;
        }
        else if(selection == RESTART) {
          wdt_disable();
          wdt_enable(WDTO_15MS);
          while(1) {}
        }
        // fall through
      default: // rotary button toggles between menu item and control value
        switch(level) {
          default:
            level = MENU_LEVEL;
            break;
          case MENU_LEVEL:
            level = VALUE_LEVEL;
            break;
        }
        break;
    }
    redraw = true;
  }

  /* STOP BUTTON BEHAVIOR
   * If alarm, disable alarm
   * If homing, emergency stop
   * Else, toggle home and select pages
   */

  stopButton.update(fastDigitalRead(STOP_BUTTON_PIN) ? 0 : 1); 
  if(stopButton.pressed()) {
     //IF_DEBUG( debug.println("stop button"); )
    if(alarm) { // stop button disables alarm
      alarm = NO_ALARM;
    }
    if(phase < NO_PHASE) {
      emergency_stop();
      disable_stepper();
      phase = NO_PHASE;
    }
    else {
      switch(page) {
        case SELECT_PAGE:
          select_home_page();
          break;
        case HOME_PAGE:
          menu = SELECT_MENU_MIN;
          // fall through
        default: // stop button returns to previous menu
          page = SELECT_PAGE;
          level = MENU_LEVEL;
          break;
      }
    }
    redraw = true;
  }
  
  /* ENDSTOP BEHAVIOR
   * If homing, stop steppers and change phase to NO_PHASE
   * Else, stop steppers
   */

  homeEndstop.update(fastDigitalRead(HOME_ENDSTOP_PIN) ? 0 : 1); 
  if(homeEndstop.pressed()) {
    //IF_DEBUG( debug.println("home endstop"); )
    switch(phase) {
      case HOME_PHASE:
      case ENDSTOP_PHASE:
      case TRIGGER_PHASE:
        if(!alarm) green_led();
        phase = ZERO_PHASE;
        enable_stepper();
        home_stop();
        live.zeroPressure = read_pressure();
        move_to_position(ctrl.startPosition, live.expiratoryRpm, false);
        break;
      case NO_PHASE:
        phase = HOME_PHASE;
        break;
      default:
        break;
    }
    redraw = true;
  }

  live.pressure = read_pressure();
  if(redrawTimer < now) {
    redrawTimer = now + 200;
    graph_pressure();
  }

  /* RESPIRATION CYCLE
   * Cycle the stepper motor movement through the phases to support mechanical breath
   */

  switch(phase) {
    case HOME_PHASE:
      //IF_DEBUG( debug.println("Homing"); )
      enable_stepper();
      move_to_position(-STEPS_PER_ROTATION, 200); // home stepper motor until endstop
      phase = ENDSTOP_PHASE;
      redraw = true;
      break;
 
    case ENDSTOP_PHASE: 
    case ZERO_PHASE:
      if(stp.done) {
        //IF_DEBUG( debug.println("Zero"); )
        disable_stepper();
        phase = NO_PHASE;
        redraw = true;
      }
      break;

    case NO_PHASE:
      if(!ctrl.ventilationActive) break;
      enable_stepper();
      lastBreathCycle = now - live.breathCycleTime;
      if(!alarm) green_led();
      redraw = true;
      // fall through

    case TRIGGER_PHASE:
      if(stp.done) {
        live.rpmRamp = 0;
        if(!alarm) green_led();
        if(makeSound) {
           tone(BEEPER_PIN, 8000, 500);
           makeSound = false;
        }
        if(live.zeroPressure - live.pressure >= ctrl.triggerPressure || breathCycleTimer < now) {  // if negative trigger pressure or timed breath
          if(!ctrl.ventilationActive) { // check ventilation still active
            disable_stepper();
            phase = NO_PHASE;
            redraw = true;
            break;
          }
          inspiratoryTimer = now + live.inspiratoryTime;
          breathCycleTimer = now + live.breathCycleTime;
          live.minuteVentilation = 60000 * ctrl.tidalVolume / (now - lastBreathCycle);
          lastBreathCycle = now;
          phase = INSPIRATORY_PHASE;
          redraw = true;
        }
      }
      break;

    case INSPIRATORY_PHASE:
      if(!alarm) blue_led();
      else if(live.audibleAlarm) makeSound = true;
      move_to_position(live.tidalEndPosition, live.inspiratoryRpm, false);
      phase = CYCLING_PHASE;
      live.peakPressure = 0;
      redraw = true;
      break;

    case CYCLING_PHASE:
      check_pressure();
      if(stp.done) {
        if(!alarm) cyan_led();
        if(inspiratoryTimer < now) {
          if(!alarm) magenta_led();
          live_volume();
          phase = EXPIRATORY_PHASE;
          redraw = true;
          int ip = live.pressure - live.zeroPressure;
          if(ip >= 0 && ip < limit.minimum.pressure) {
            trigger_alarm(UNDER_PRESSURE_ALARM);
          }
        }
      }
      break;

    case EXPIRATORY_PHASE:
      move_to_position(ctrl.startPosition, live.expiratoryRpm, false);
      phase = TRIGGER_PHASE;
      redraw = true;
      break;
  }

  /* CHECK LIMITS
   * If operating outside limits trigger alarm
   */  
  
  if(live.peakPressure > limit.maximum.pressure) {
    trigger_alarm(OVER_PRESSURE_ALARM);
  }

  if(live.volume < limit.minimum.volume) {
    trigger_alarm(UNDER_VOLUME_ALARM);
  }
  else if(live.volume > limit.maximum.volume) {
    trigger_alarm(OVER_VOLUME_ALARM);
  }

  if(live.minuteVentilation < limit.minimum.ventilation) {
    trigger_alarm(UNDER_VENTILATION_ALARM);
  }
  else if(live.minuteVentilation > limit.maximum.ventilation) {
    trigger_alarm(OVER_VENTILATION_ALARM);
  }
  
  /* REDRAW
   * If a display update is flagged, redraw the current page
   */

  if(redraw) {
    draw_update();
  }
}

void select_home_page()
{
  page = HOME_PAGE;
  level = VALUE_LEVEL;
  selection = HOME_SELECTION_MIN;
  EEPROM.put(0, ctrl);
  EEPROM.put(sizeof(ctrl_t), limit);
}

int read_pressure()
{
  float temp(NAN), hum(NAN), pres(NAN);
  bme.read(pres, temp, hum, BME280::TempUnit_Celsius, BME280::PresUnit_Pa);
  pres *= 0.101972;
  return round(pres);
}

#define RAMP_MAX 3

void check_pressure()
{
  int pressure = live.pressure - live.zeroPressure;
  if(live.peakPressure < pressure) live.peakPressure = pressure;
  if(live.rpmRamp < RAMP_MAX && pressure >= ctrl.plateauAirwayPressure) { // if plateau airway pressure reached
    //DebugMessage("live.peakPressure = ", live.peakPressure);  
    //DebugMessage("microseconds = ", stp.ms);
    update_rpm(0); // ramp down stepper motor  
    live.rpmRamp = RAMP_MAX;
  }
#ifdef TORQUE_RAMP
  else {
    switch(live.rpmRamp) {
      case 0:
        if(pressure > FIRST_POSITION_PRESSURE || stp.p > live.firstPosition) {
          update_rpm(live.inspiratoryRpm * 2 / 3);
          live.rpmRamp++;
        }
        break;
      case 1:
        if(pressure > SECOND_POSITION_PRESSURE || stp.p > live.secondPosition) {
          update_rpm(live.inspiratoryRpm / 2);
          live.rpmRamp++;
        }
        break;
    }
  }
#endif
}

void graph_pressure()
{
  unsigned v = volume_ml() / 50;
  if(v > 10) v = 10;
  graph.volume[graph.index & GRAPH_MASK] = v;

  int p = (live.pressure - live.zeroPressure) / 12;
  if(p > 30) p = 30;
  if(p < -2) p = -2;
  graph.pressure[graph.index & GRAPH_MASK] = p;
  //DebugMessage("pressure = ", p);
  redraw = true;
  graph.index++;
}
