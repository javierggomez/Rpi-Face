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

#ifndef FACE_CONTROLLER
#define FACE_CONTROLLER
extern const unsigned char FACE_HAPPY[8];
extern const unsigned char FACE_SAD[8];
extern const unsigned char FACE_SURPRISE[8];
extern const unsigned char FACE_ANGRY[8];
extern const unsigned char FACE_NEUTRAL[8];
int face_initialize();
int face_setAsHome(int fd);
int face_goToHome(int fd);
int face_setFace(int fd, unsigned const char *positions);
int face_turnOff(int fd);
int face_blink(int fd, int times, unsigned char current);
void face_close(int fd);
#endif
