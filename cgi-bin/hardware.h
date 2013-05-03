//    Copyright (C) 2013 Javier García, Julio Alberto González

//    This file is part of Rpi-Face.
//    Rpi-Face is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    Rpi-Face is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with Rpi-Face.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __HARDWARE_H__
#define __HARDWARE_H__
#include <bcm2835.h>
bool gpio_init();
void gpio_close();
void gpio_fsel(uint8_t pin, bool in);
void gpio_writePin(uint8_t pin, bool on);
bool gpio_readPin(uint8_t pin);
bool gpio_debounce(uint8_t pin);
void gpio_waitForRelease(uint8_t pin);
#endif
