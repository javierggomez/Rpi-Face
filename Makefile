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


CC=g++
CFLAGS=-Wall
FESTIVAL_FLAGS=-I/usr/include/festival -I/usr/lib/speech_tools/include -Lfestival/src/lib -leststring -lestools -lestbase -lFestival
EXECUTABLES=server_query.cgi move_face face_controller_test servo_controller_test servo_controllerTest rpi_uart_test

default: server_query.cgi move_face

all: $(EXECUTABLES) 

set_face.cgi: set_face.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o set_happy.cgi set_face.o face_controller.o servo_controller.o rpi_uart.o

server_query.cgi: server_query.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o server_query.cgi server_query.o face_controller.o servo_controller.o rpi_uart.o

move_face: move_face.o speech_synthesis.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) -o move_face move_face.o speech_synthesis.o face_controller.o servo_controller.o rpi_uart.o $(FESTIVAL_FLAGS)

move_face.o: move_face.c Makefile
	$(CC) -o move_face.o -c move_face.c

speech_synthesis.o: speech_synthesis.c speech_synthesis.h
	$(CC) -c speech_synthesis.c $(FESTIVAL_FLAGS) -o speech_synthesis.o

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

install: text_analyzer server_query.cgi move_face data acopost
	-mv -t /var/www/cgi-bin/ text_analyzer server_query.cgi move_face data acopost
