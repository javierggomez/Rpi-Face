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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "servo_controller.h"

#define DELIMITER " "
int main(void) {
	int fd=servo_initialize();
	if (fd<0) {
		perror("No se pudo abrir el puerto UART");
		return -1;
	}
	int exit=0; // flag de salida
	char input[64]; // datos a enviar
	char *temp;
	while (!exit) {
		gets(input);
		temp=strtok(input, DELIMITER);
		printf("%s\n", temp);
		if (!strcmp(temp, "q")) {
			exit=1;
		}
		else if (!strcmp(temp, "move")) {
			temp=strtok(NULL, DELIMITER);
			printf("%s\n", temp);
			int servo=atoi(temp);
			printf("%i\n", servo);
			temp=strtok(NULL, DELIMITER);
			printf("%s\n", temp);
			unsigned char position=(unsigned char)atoi(temp);
			printf("%i\n", (int)position);
			if (servo_setServoPosition(fd,servo,position)) {
				perror("No se recibió respuesta");
			}
		} else if (!strcmp(temp, "moveAll")) {
			unsigned char position[8];
			for (int i=0;i<8;i++) {
				temp=strtok(NULL, DELIMITER);
				position[i]=(unsigned char)atoi(temp);
			}
			if (servo_setAllServosPositions(fd,position)) {
				perror("No se recibió respuesta");
			}
		} else if (!strcmp(temp, "read")) {
			temp=strtok(NULL, DELIMITER);
			int servo=atoi(temp);
			int error=0;
			unsigned char position=servo_getPosition(fd,servo,&error);
			if (error){
				perror("No se recibió respuesta");
			} else {
				printf("%i\n",position);
			}
						
		} else {
			perror("El comando no existe");
		}
		
	}
	servo_close(fd);
	return 0;

}
