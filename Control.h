//  Open BVM Ventilator - Ventilator state machine and control functions
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
#include "StepperSpeedControl.h"

const float GEAR_RATIO = 5.33; // SUN = 9 Teeth, PLANETS = 15 Teeth
const float GANG_RATIO = GEAR_RATIO * GEAR_RATIO;
const float PRESS_RATIO = 90 / 360.0;
const unsigned END_POSITION = (STEPS_PER_REVOLUTION * GANG_RATIO * PRESS_RATIO);

enum page_t {
  HOME_PAGE,
  SELECT_PAGE,
  CONTROLS_PAGE,
  SETUP_PAGE,
  LIMITS_PAGE,
  EVENTS_PAGE,
  SYSTEM_PAGE
};

#define SELECT_MENU_MIN CONTROLS_PAGE
#define SELECT_MENU_MAX SYSTEM_PAGE

extern page_t page;

enum level_t {
  OTHER_VALUE_LEVEL = -1,
  VALUE_LEVEL,
  MENU_LEVEL,
};

extern level_t level;

enum selection_t {
  /* HOME_PAGE */
  TIDAL_VOLUME,
  RESPIRATORY_RATE,
  PLATEAU_AIRWAY_PRESSURE,
  /* CONTROLS_PAGE */
  RESPIRATORY_RATIO,
  TRIGGER_PRESSURE,
  INSPIRATORY_FLOW,
  EXPIRATORY_FLOW,
  /* SETUP PAGE */
  START_STOP_VENTILATION,
  PRESSURE_TEST,
  /* LIMITS_PAGE */
  PRESSURE_LIMIT,
  MINUTE_VENTILATION_LIMIT,
  TIDAL_VOLUME_LIMIT,
  /* SYSTEM_PAGE */
  AUDIBLE_ALARM,
  START_POSITION,
  FULL_PRESS_VOLUME,
  FACTORY_RESET,
  RESTART,
};

extern selection_t selection;

#define HOME_SELECTION_MIN TIDAL_VOLUME
#define HOME_SELECTION_MAX PLATEAU_AIRWAY_PRESSURE

#define CONTROLS_SELECTION_MIN RESPIRATORY_RATIO
#define CONTROLS_SELECTION_MAX EXPIRATORY_FLOW

#define SETUP_SELECTION_MIN START_STOP_VENTILATION
#define SETUP_SELECTION_MAX PRESSURE_TEST

#define LIMITS_SELECTION_MIN PRESSURE_LIMIT
#define LIMITS_SELECTION_MAX TIDAL_VOLUME_LIMIT

#define SYSTEM_SELECTION_MIN AUDIBLE_ALARM
#define SYSTEM_SELECTION_MAX RESTART

enum alarm_t {
  NO_ALARM,
  
  UNDER_PRESSURE_ALARM,
  UNDER_VENTILATION_ALARM,
  UNDER_VOLUME_ALARM,
  
  OVER_PRESSURE_ALARM,
  OVER_VENTILATION_ALARM,
  OVER_VOLUME_ALARM,

  POWER_FAILURE_ALARM,
};

extern alarm_t alarm;

#define MAX_EVENTS 5

struct events_t {
  alarm_t list[MAX_EVENTS];
  byte length;
};

extern events_t events;

enum phase_t {
  HOME_PHASE = -3,
  ENDSTOP_PHASE = -2,
  ZERO_PHASE = -1,
  NO_PHASE,
  TRIGGER_PHASE,
  INSPIRATORY_PHASE,
  CYCLING_PHASE,
  EXPIRATORY_PHASE,
};

extern phase_t phase;

struct limit_t {
  struct {
    int pressure;         // 5 cm
    unsigned ventilation; // 3.0 l/min
    unsigned volume;      // 180 ml
  } minimum;
  struct {
    int pressure;         // 40 cm
    unsigned ventilation; // 8.0 l/min
    unsigned volume;      // 750
  } maximum; 
};

extern limit_t limit;

inline void limit_minimum_pressure(int value) { limit.minimum.pressure = value; }
inline void limit_minimum_ventilation(unsigned value)  { limit.minimum.ventilation = value; }
inline void limit_minimum_volume(unsigned value)  { limit.minimum.volume = value; }

inline void limit_maximum_pressure(int value) { limit.maximum.pressure = value; }
inline void limit_maximum_ventilation(unsigned value)  { limit.maximum.ventilation = value; }
inline void limit_maximum_volume(unsigned value)  { limit.maximum.volume = value; }

struct ctrl_t {
  unsigned startPosition;   // steps
  unsigned fullPressVolume; // (850 ml)
  unsigned tidalVolume;     // Vt (200-750 ml [6-8cc per kg])
  unsigned respiratoryRate; // Rr (12-20 bpm)
  unsigned respiratoryRatio;// 1:1-4
  int plateauAirwayPressure;// Paw (10-35cm)
  unsigned inspiratoryFlow; // 60-100 lpm
  unsigned expiratoryFlow;  // 60-100 lpm
  int triggerPressure;      // 5cm
  bool ventilationActive;
};

extern ctrl_t ctrl;

struct live_t {
  unsigned long breathCycleTime; // millis
  unsigned long inspiratoryTime; // millis

  int pressure;
  unsigned volume;
  unsigned minuteVentilation; // 3000-8000 ml/min

  int zeroPressure;
  int basePressure;
  int peakPressure;

  unsigned inspiratoryRpm;
  unsigned expiratoryRpm;
  unsigned fullPressSteps;
  unsigned tidalSteps;
  unsigned _400Steps;
  unsigned _600Steps;
  byte rpmRamp;
  bool audibleAlarm;
};

extern live_t live;

void ctrl_setup();

void ctrl_start_position(unsigned value);
void ctrl_full_press_volume(unsigned value);
void ctrl_tidal_volume(unsigned value);
void ctrl_respiratory_rate(unsigned value);
void ctrl_respiratory_ratio(unsigned value);
void ctrl_plateau_airway_pressure(unsigned value);
void ctrl_inspiratory_flow(unsigned value);
void ctrl_expiratory_flow(unsigned value);
void ctrl_pressure_trigger(unsigned value);

void live_volume_per_revolution();
void live_breath_cycle_time();
void live_inspiratory_time();
void live_minute_ventilation();
void actual_minute_ventilation();
void live_full_press_steps();
unsigned tidal_steps(unsigned volume);
inline void live_tital_steps() { live.tidalSteps = tidal_steps(ctrl.tidalVolume); }
inline void live_400_steps() { live._400Steps = ctrl.startPosition + tidal_steps(400); }
inline void live_600_steps() { live._600Steps = ctrl.startPosition + tidal_steps(600); }

int clamp_input_value(int value, int step, int8_t dir, int lo, int hi);

void alarm_event(alarm_t alarm);
void factory_reset();
