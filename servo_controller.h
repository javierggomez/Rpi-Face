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


#ifndef SERVO_CONTROLLER
#define SERVO_CONTROLLER
int servo_read(int fd, char *buffer, int nChars, int useconds);
int servo_initialize();
int servo_setServoPosition(int fd, int servo, unsigned char position);
int servo_setAllServosPositions(int fd, unsigned const char *positions);
int servo_turnOff(int fd, int servo);
int servo_setAsHome(int fd, int servo);
int servo_goToHome(int fd, int servo);
int servo_setWidthResolution(int fd, int servo, char resolution);
unsigned char servo_getPosition(int fd, int servo, int *error);
int servo_getTorque(int fd, int servo, int *error);
int servo_setBaudRate(int fd, int rate);
void servo_close(int fd);
#endif
