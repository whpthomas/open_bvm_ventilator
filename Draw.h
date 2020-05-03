//  Open BVM Ventilator - UI redraw and led control
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

void red_led();
void yellow_led();
void green_led();
void cyan_led();
void blue_led();
void magenta_led();
