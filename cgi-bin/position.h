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

#ifndef __POSITION_H__
#define __POSITION_H__
void setPosition(const unsigned char *position);
int setPosition(const unsigned char *position, float previousWeight);
int loadPosition(unsigned char *position, const char *filename);
int savePosition(const unsigned char *position, const char *filename);
void weightedAverage(unsigned char *result, const unsigned char *array1, const unsigned char *array2, float weight1, float weight2, int length);
#endif