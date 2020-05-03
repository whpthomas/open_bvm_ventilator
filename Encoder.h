//  Open BVM Ventilator - RepRapDiscount Full Graphic Smart Controller encoder support
//
//  See: https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller
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

struct encoder_t {
  volatile bool a;
  volatile bool b;
  volatile byte c;
  volatile int8_t dir;
};

extern struct encoder_t encoder;

void encoder_setup();
