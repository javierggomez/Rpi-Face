//    Copyright (C) 2013 Javier García, Julio Alberto González

//    This file is part of Rpi-Face.
//    Rpi-Face is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    Foobar is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MOVE_FACE
#define MOVE_FACE
int loadPosition(unsigned char *position, const char *filename);
int savePosition(unsigned char *position, const char *filename);
void weightedAverage(unsigned char *result, const unsigned char *array1, const unsigned char *array2, float weight1, float weight2, int length);
void calculatePosition(unsigned char *result, float tag);
#endif
