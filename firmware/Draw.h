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

#pragma once

extern bool redraw;
extern int8_t menu;

#define GRAPH_MAX 64
#define GRAPH_MASK (GRAPH_MAX - 1)
#define GRAPH_SHIFT 1

struct graph_t {
  int8_t volume[64];
  int8_t pressure[64];
  byte index;
};

extern graph_t graph;

void draw_setup();
void draw_update();

void draw_home_page();
void draw_select_page();
void draw_controls_page();
void draw_setup_page();
void draw_limits_page();
void draw_events_page();
void draw_system_page();

void set_led(bool r, bool g, bool b);

inline void red_led()     { set_led(true, false, false); }
inline void yellow_led()  { set_led(true, true, false); }
inline void green_led()   { set_led(false, true, false); }
inline void cyan_led()    { set_led(false, true, true); }
inline void blue_led()    { set_led(false, false, true); }
inline void magenta_led() { set_led(true, false, true); }
