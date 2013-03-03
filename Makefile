#    Copyright (C) 2013 Javier García, Julio Alberto González

#    This file is part of Rpi-Face.
#    Rpi-Face is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    Rpi-Face is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with Rpi-Face.  If not, see <http://www.gnu.org/licenses/>.


CC=gcc
CFLAGS=-Wall -std=gnu99
EXECUTABLES=move_face face_controller_test servo_controller_test servo_controllerTest rpi_uart_test

all: $(EXECUTABLES) 

move_face: move_face.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o move_face move_face.o face_controller.o servo_controller.o rpi_uart.o
face_controller_test: face_controller_test.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o face_controller_test face_controller_test.o face_controller.o servo_controller.o rpi_uart.o

servo_controller_test: servo_controller_test.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o servo_controller_test servo_controller_test.o servo_controller.o rpi_uart.o

servo_controllerTest: servo_controllerTest.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o servo_controllerTest servo_controllerTest.o servo_controller.o rpi_uart.o

rpi_uart_test: rpi_uart_test.o rpi_uart.o
	$(CC) $(CFLAGS) -o rpi_uart_test rpi_uart_test.o rpi_uart.o

.PHONY = clean 
clean: 
	-rm -rf *.o $(EXECUTABLES) *~
