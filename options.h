//  Open BVM Ventilator - Compile time options
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

//#define DEBUG

#ifdef DEBUG
# define debug Serial 
# define DebugMessage(a, b) debug.print(F(a)); debug.println(b);
# define IF_DEBUG(block) block
# define NO_DEBUG(block)
#else
# define DebugMessage(a, b)
# define IF_DEBUG(block)
# define NO_DEBUG(block) block
#endif
