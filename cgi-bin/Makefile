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
CXX=g++
CFLAGS=-Wall
CXXFLAGS=-Wall
FESTIVAL_FLAGS=-lFestival -Lfestival/src/lib -lestools -lestbase -leststring -I/usr/include/festival -I/usr/lib/speech_tools/include
FREELING_FLAGS=-lfreeling -L/usr/local/lib -I/usr/local/include
GPIO_FLAGS=-lbcm2835 -lrt
EXECUTABLES=server_query.cgi action_manager vote_manager init move_face face_controller_test servo_controller_test servo_controllerTest rpi_uart_test freeling_server freeling_test UnigramModel_test festival_test

default: server_query.cgi action_manager vote_manager freeling_server

all: $(EXECUTABLES) 

server_query.cgi: server_query.o vote_handler.o 
	$(CC) $(CFLAGS) -o server_query.cgi server_query.o  vote_handler.o 

action_manager: action_manager.o freeling_client.o speech_synthesis.o position.o face_controller.o servo_controller.o rpi_uart.o
	$(CXX) $(CXXFLAGS) -o action_manager action_manager.o freeling_client.o speech_synthesis.o position.o face_controller.o servo_controller.o rpi_uart.o $(FREELING_FLAGS) $(FESTIVAL_FLAGS)

move_face: move_face.o speech_synthesis.o face_controller.o servo_controller.o rpi_uart.o
	$(CC) -o move_face move_face.o speech_synthesis.o face_controller.o servo_controller.o rpi_uart.o $(FESTIVAL_FLAGS)

vote_process.o: vote_process.c vote_process.h vote_handler.o speech_synthesis.o face_controller.o
	$(CC) $(CFLAGS) -c -o vote_process.o vote_process.c $(FESTIVAL_FLAGS)

speech_synthesis.o: speech_synthesis.cpp speech_synthesis.h
	$(CXX) $(CXXFLAGS) -c $(FESTIVAL_FLAGS) speech_synthesis.cpp -o speech_synthesis.o

face_controller_test: face_controller_test.o face_controller.o servo_controller.o rpi_uart.o
	$(CXX) $(CXXFLAGS) -o face_controller_test face_controller_test.o face_controller.o servo_controller.o rpi_uart.o

servo_controller_test: servo_controller_test.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o servo_controller_test servo_controller_test.o servo_controller.o rpi_uart.o

servo_controllerTest: servo_controllerTest.o servo_controller.o rpi_uart.o
	$(CC) $(CFLAGS) -o servo_controllerTest servo_controllerTest.o servo_controller.o rpi_uart.o

rpi_uart_test: rpi_uart_test.o rpi_uart.o
	$(CC) $(CFLAGS) -o rpi_uart_test rpi_uart_test.o rpi_uart.o

vote_manager: vote_manager.o vote_handler.o
	$(CC) $(CFLAGS) -o vote_manager vote_manager.o vote_handler.o

freeling_test: freeling_test.o freeling_client.o freeling_server
	$(CXX) $(CXXFLAGS) -o freeling_test freeling_test.o freeling_client.o $(FREELING_FLAGS)

freeling_translate: freeling_translate.o freeling_client.o freeling_server
	$(CXX) $(CXXFLAGS) -o freeling_translate freeling_translate.o freeling_client.o $(FREELING_FLAGS)

freeling_server: freeling_server.o
	$(CXX) $(CXXFLAGS) -o freeling_server freeling_server.o $(FREELING_FLAGS)

UnigramModel_test: UnigramModel_test.o UnigramModel.o
	$(CXX) $(CXXFLAGS) -o UnigramModel_test UnigramModel_test.o UnigramModel.o

festival_test: festival_test.o speech_synthesis.o
	$(CXX) $(CXXFLAGS) -o festival_test festival_test.o speech_synthesis.o $(FESTIVAL_FLAGS)

scp_connect_test: scp_connect_test.o scp_connect.o
	$(CXX) $(CXXFLAGS) -o scp_connect_test scp_connect_test.o scp_connect.o

init: init.o hardware.o
	$(CXX) $(CXXFLAGS) -o init init.o hardware.o $(GPIO_FLAGS)

.PHONY = clean 
clean: 
	-rm -rf *.o $(EXECUTABLES) *~

install: text_analyzer server_query.cgi move_face data acopost
	-mv -t /var/www/cgi-bin/ text_analyzer server_query.cgi move_face data acopost
