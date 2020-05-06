//  Open BVM Ventilator - UI redraw and LED control
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
#include <U8g2lib.h>
#include <SPI.h>

#include "ButtonDebouncer.h"
#include "Control.h"
#include "Draw.h"
#include "Pins.h"

bool redraw;
int8_t menu;
graph_t graph;

#define SMLR u8g2_font_helvR08_tr
#define SMLB u8g2_font_helvB08_tr
#define SMLH 8 // small font height
#define MNUH 9 // menu height
#define LRG u8g2_font_helvB14_tr
#define LRGH 14 // large font height

U8G2_ST7920_128X64_2_HW_SPI u8g2(U8G2_R0, /* CS */ CS_PIN);

void draw_setup()
{
  u8g2.begin();  
  u8g2.firstPage();

  fastPinMode(RED_PIN, OUTPUT);
  fastPinMode(GREEN_PIN, OUTPUT);
  fastPinMode(BLUE_PIN, OUTPUT);
  red_led();
}

void draw_update()
{
  switch(page) {
    case HOME_PAGE:
      draw_home_page();
      break;
    case SELECT_PAGE:
      draw_select_page();
      break;
    case CONTROLS_PAGE:
      draw_controls_page();
      break;
    case SETUP_PAGE:
      draw_setup_page();
      break;
    case LIMITS_PAGE:
      draw_limits_page();
      break;
    case EVENTS_PAGE:
      draw_events_page();
      break;
    case SYSTEM_PAGE:
      draw_system_page();
      break;
  }  
  if(!u8g2.nextPage()) {
    u8g2.firstPage();
    redraw = false;
  }
}

void draw_value(byte x, byte y, const __FlashStringHelper *key, unsigned value, const __FlashStringHelper *unit, bool selected)
{
  u8g2.setFont(selected ? SMLB : SMLR);    
  u8g2.setCursor(x, y + SMLH);
  u8g2.print(key);
  
  u8g2.setFont(LRG);
  u8g2.setCursor(u8g2.tx + 2, y + LRGH);
  u8g2.print(value);

  u8g2.setFont(SMLR);    
  u8g2.setCursor(u8g2.tx + 2, y + LRGH);
  u8g2.print(unit);
}

void draw_k_value(byte x, byte y, const __FlashStringHelper *key, unsigned value, const __FlashStringHelper *unit, bool selected)
{
  u8g2.setFont(selected ? SMLB : SMLR);    
  u8g2.setCursor(x, y + SMLH);
  u8g2.print(key);
  
  u8g2.setFont(LRG);
  u8g2.setCursor(u8g2.tx + 2, y + LRGH);
  unsigned exponent = value / 1000;
  value -= exponent * 1000;
  value /= 100;
  u8g2.print(exponent);
  u8g2.print('.');
  u8g2.print(value);

  u8g2.setFont(SMLR);    
  u8g2.setCursor(u8g2.tx + 2, y + LRGH);
  u8g2.print(unit);
}

void draw_message(byte x, byte y, const __FlashStringHelper *message)
{
  u8g2.setFont(SMLR);    
  u8g2.setCursor(x, y + SMLH);
  u8g2.print(message);
}

void draw_alert()
{
  const __FlashStringHelper *message = NULL;
  const __FlashStringHelper *dir = NULL;
  const __FlashStringHelper *key = NULL;
  
  u8g2.setFont(SMLB);    
  u8g2.setCursor(64, 18 + SMLH);
  
  switch(phase) {
    case HOME_PHASE:
      message = F("Started");
      break;
    case ENDSTOP_PHASE:
    case ZERO_PHASE:
      message = F("Homing");
      break;    
    case NO_PHASE:
      message = F("Ready");
      break;
    case TRIGGER_PHASE:
      if(stp.done) {
        message = F("Trigger");
      }
      else {
        message = F("Expiration");
      }
      break;
    case INSPIRATORY_PHASE:
      message = F("Inspiration");
      break;
    case CYCLING_PHASE:
      if(stp.done) {
        message = F("Cycling");
      }
      else {
        message = F("Inspiration");
      }
      break;
    case EXPIRATORY_PHASE:
      message = F("Expiration");
      break;
  }
  
  switch(alarm) {
    case POWER_FAILURE_ALARM:
      message = F("Power Failure");
      // fall through      
    case NO_ALARM:
      u8g2.print(message);
      return;
    case UNDER_PRESSURE_ALARM:
      dir = F("Under");
      key = F("Ip");
      break;
    case UNDER_VENTILATION_ALARM:
      dir = F("Under");
      key = F("Mv");
      break;
    case UNDER_VOLUME_ALARM:
      dir = F("Under");
      key = F("Vt");
      break;
    case OVER_PRESSURE_ALARM:
      dir = F("Over");
      key = F("Ip");
      break;
    case OVER_VENTILATION_ALARM:
      dir = F("Over");
      key = F("Mv");
      break;
    case OVER_VOLUME_ALARM:
      dir = F("Over");
      key = F("Vt");
      break;
  }
  u8g2.print(dir);
  u8g2.setCursor(u8g2.tx + 2, 18 + SMLH);
  u8g2.print(key);
}

void draw_graph(byte offset, int8_t *data)
{
  int8_t y, p = offset - data[63] - 1;
  byte x, i;
  for(i = 0; i < 64; i++) {
    x = 64 + (((63 - i) + graph.index + 1) & 63);
    y = offset - data[i];
    if(x == 64) {
      u8g2.drawVLine(x, p, 1);      
    }
    else if(y <= p) {
      u8g2.drawVLine(x, y, p - y + 1);
    }
    else {
      u8g2.drawVLine(x, p, y - p);      
    }
    p = y - 1;
  }
}

void draw_peak_pressure()
{
  draw_value(0, 50, F("Pip"), (live.peakPressure - 5) / 10, F("cm"), false);   
}

void draw_home_page()
{
  draw_value(0, 0, F("Vt"), ctrl.tidalVolume, F("ml"), selection == TIDAL_VOLUME);
  draw_k_value(64, 0, F("Mv"), live.minuteVentilation, F("l/m"), false);  
  draw_value(0, 25, F("Rr"), ctrl.respiratoryRate, F("bpm"), selection == RESPIRATORY_RATE);
  if(live.peakPressure && selection != PLATEAU_AIRWAY_PRESSURE) {
    draw_peak_pressure();
  }
  else {
    draw_value(0, 50, F("Paw"), ctrl.plateauAirwayPressure / 10, F("cm"), selection == PLATEAU_AIRWAY_PRESSURE);
  }
  draw_alert();
  draw_graph(42, graph.volume);
  draw_graph(59, graph.pressure);
}

void draw_item(byte column, byte row, const __FlashStringHelper *item, bool selected)
{
  byte y = row * MNUH;
  u8g2.setCursor(column, y + SMLH);
  u8g2.setFont(SMLR);    
  if(selected) {
    u8g2.print('>');
    u8g2.setFont(SMLB);    
  }
  u8g2.setCursor(column + 6, y + SMLH);
  u8g2.print(item);
}

void draw_select_page()
{
  draw_message(0, 0, F("Select page"));
  draw_item(20, 2, F("Controls"), menu == CONTROLS_PAGE);
  draw_item(20, 3, F("Setup"), menu == SETUP_PAGE);
  draw_item(20, 4, F("Limits"), menu == LIMITS_PAGE);
  draw_item(20, 5, F("Events"), menu == EVENTS_PAGE);
  draw_item(20, 6, F("System"), menu == SYSTEM_PAGE);
}

void draw_ratio(byte column, byte row, const __FlashStringHelper *control, unsigned value, bool selected)
{
  byte y = row * MNUH;
  u8g2.setCursor(column, y + SMLH);
  u8g2.setFont(SMLR);    
  if(selected) {
    u8g2.print('>');
    if(level == MENU_LEVEL) u8g2.setFont(SMLB);    
  }
  u8g2.setCursor(column + 6, y + SMLH);
  u8g2.print(control);
  u8g2.print(':');
  
  u8g2.setFont(selected && level == VALUE_LEVEL ? SMLB : SMLR);
  u8g2.setCursor(u8g2.tx + 2, y + SMLH);
  u8g2.print("1:");
  u8g2.print(value);
}

void draw_control(byte column, byte row, const __FlashStringHelper *control, unsigned value, const __FlashStringHelper *unit, bool selected)
{
  byte y = row * MNUH;
  u8g2.setCursor(column, y + SMLH);
  u8g2.setFont(SMLR);    
  if(selected) {
    u8g2.print('>');
    if(level == MENU_LEVEL) u8g2.setFont(SMLB);    
  }
  u8g2.setCursor(column + 6, y + SMLH);
  u8g2.print(control);
  u8g2.print(':');
  
  u8g2.setFont(selected && level != MENU_LEVEL ? SMLB : SMLR);
  u8g2.setCursor(u8g2.tx + 2, y + SMLH);
  u8g2.print(value);

  u8g2.setCursor(u8g2.tx + 2, y + SMLH);
  u8g2.print(unit);
}

void draw_controls_page()
{
  draw_message(0, 0, F("Controls Page"));
  draw_ratio(0,2, F("Respiratory Ratio"), ctrl.respiratoryRatio, selection == RESPIRATORY_RATIO);
  draw_control(0,3, F("Pressure Trigger"), ctrl.triggerPressure, F("mm"), selection == TRIGGER_PRESSURE);
  draw_control(0,4, F("Inspiratory Flow"), ctrl.inspiratoryFlow, F("l/m"), selection == INSPIRATORY_FLOW);
  draw_control(0,5, F("Expiratory Flow"), ctrl.expiratoryFlow, F("l/m"), selection == EXPIRATORY_FLOW);
}

void draw_setup_page()
{
  draw_message(0, 0, F("Controls Page"));
  draw_item(10, 1, ctrl.ventilationActive ? F("Stop Ventilation") : F("Start Ventilation"), selection == START_STOP_VENTILATION);
  if(!ctrl.ventilationActive) {
    draw_item(10, 2, F("Pressure Test"), selection == PRESSURE_TEST); 
    draw_peak_pressure();
    draw_graph(59, graph.pressure);
  }
}

void draw_limits_page()
{
  draw_message(0, 0, F("Limits Page"));
  if(level == MENU_LEVEL) {
    draw_item(10, 2, F("Pressure"), selection == PRESSURE_LIMIT);
    draw_item(10, 3, F("Minute Ventilation"), selection == MINUTE_VENTILATION_LIMIT);
    draw_item(10, 4, F("Tidal Volume"), selection == TIDAL_VOLUME_LIMIT);
  }
  else {
    switch(selection) {
      case PRESSURE_LIMIT:
        draw_message(10, 13, F("Airway Pressure"));
        draw_control(10,3, F("Minimum"), limit.minimum.pressure, F("mm"), level == OTHER_VALUE_LEVEL);
        draw_control(10,4, F("Maximum"), limit.maximum.pressure, F("mm"), level == VALUE_LEVEL);
        break;
      case MINUTE_VENTILATION_LIMIT:
        draw_message(10, 13, F("Minute Ventilation"));
        draw_control(10,3, F("Minimum"), limit.minimum.ventilation, F("ml/m"), level == OTHER_VALUE_LEVEL);
        draw_control(10,4, F("Maximum"), limit.maximum.ventilation, F("ml/m"), level == VALUE_LEVEL);
        break;
      case TIDAL_VOLUME_LIMIT:
        draw_message(10, 13, F("Tidal Volume"));
        draw_control(10,3, F("Minimum"), limit.minimum.volume, F("ml"), level == OTHER_VALUE_LEVEL);
        draw_control(10,4, F("Maximum"), limit.maximum.volume, F("ml"), level == VALUE_LEVEL);
        break;
      default:
        break;
    }
  }
}

void draw_events_page()
{
  byte y = MNUH + 4;
  draw_message(0, 0, F("Events Page"));
  if(events.length) {
    const __FlashStringHelper *message = NULL;
    const __FlashStringHelper *key = NULL;
    for(byte i = 0; i < events.length; i++, y += MNUH) {
      switch(events.list[i]) {
        case NO_ALARM:
          goto L_NO_ALARMS;
        case UNDER_PRESSURE_ALARM:
          message = F("Under");
          key = F("Pressure");
          break;
        case UNDER_VENTILATION_ALARM:
          message = F("Under");
          key = F("Ventilation");
          break;
        case UNDER_VOLUME_ALARM:
          message = F("Under");
          key = F("Volume");
          break;
        case OVER_PRESSURE_ALARM:
          message = F("Over");
          key = F("Pressure");
          break;
        case OVER_VENTILATION_ALARM:
          message = F("Over");
          key = F("Ventilation");
          break;
        case OVER_VOLUME_ALARM:
          message = F("Over");
          key = F("Volume");
          break;        
        case POWER_FAILURE_ALARM:
          message = F("Power");
          key = F("Failure");
          break;        
      }
      draw_message(0, y, message);  
      draw_message(u8g2.tx + 2, y, key);  
    }
  }
  else {
L_NO_ALARMS:
    draw_message(10, y, F("No Alarms!"));  
  }
}

void draw_system_page()
{
  draw_message(0, 0, F("System Page"));
  draw_item(0, 2, live.audibleAlarm ? F("Disable Alarm") : F("Enable Alarm"), selection == AUDIBLE_ALARM);
  draw_control(0, 3, F("Start Position"), ctrl.startPosition, F("steps"), selection == START_POSITION);
  draw_control(0, 4, F("Press Volume"), ctrl.fullPressVolume, F("ml"), selection == FULL_PRESS_VOLUME);
  draw_item(0, 5, F("Factory Reset"), selection == FACTORY_RESET);
  draw_item(0, 6, F("Restart"), selection == RESTART);
}

void red_led()
{
  fastDigitalWrite(RED_PIN, HIGH);
  fastDigitalWrite(GREEN_PIN, LOW);
  fastDigitalWrite(BLUE_PIN, LOW); 
}

void yellow_led()
{
  fastDigitalWrite(RED_PIN, HIGH);
  fastDigitalWrite(GREEN_PIN, HIGH);
  fastDigitalWrite(BLUE_PIN, LOW); 
}

void green_led()
{
  fastDigitalWrite(RED_PIN, LOW);
  fastDigitalWrite(GREEN_PIN, HIGH);
  fastDigitalWrite(BLUE_PIN, LOW); 
}

void cyan_led()
{
  fastDigitalWrite(RED_PIN, LOW);
  fastDigitalWrite(GREEN_PIN, HIGH);
  fastDigitalWrite(BLUE_PIN, HIGH); 
}

void blue_led()
{
  fastDigitalWrite(RED_PIN, LOW);
  fastDigitalWrite(GREEN_PIN, LOW);
  fastDigitalWrite(BLUE_PIN, HIGH); 
}

void magenta_led()
{
  fastDigitalWrite(RED_PIN, HIGH);
  fastDigitalWrite(GREEN_PIN, LOW);
  fastDigitalWrite(BLUE_PIN, HIGH); 
}
