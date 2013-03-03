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
#include "face_controller.h"
#define DELAY 1000000

//Prueba de las caras sin comprobar errores
int main(void) {
	int fd = face_initialize();
	usleep(DELAY);
	printf("Iniciando\n");

	printf("Cara contenta\n");
	face_setFace(fd,FACE_HAPPY);
		usleep(5*DELAY);

	printf("Guardando posición por defecto\n");
	face_setAsHome(fd);
		usleep(5*DELAY);
	printf("Cara triste\n");
	face_setFace(fd,FACE_SAD);
		usleep(5*DELAY);
	printf("Cara sorprendida\n");
	face_setFace(fd,FACE_SURPRISE);
		usleep(5*DELAY);
	printf("Cara enfadada\n");
	face_setFace(fd,FACE_ANGRY);
		usleep(5*DELAY);
	printf("Cara neutra\n");
	face_setFace(fd,FACE_NEUTRAL);
		usleep(5*DELAY);
	printf("Posición por defecto\n");
	face_goToHome(fd);
		usleep(5*DELAY);
	printf("Apagando\n");
	face_turnOff(fd);
		usleep(5*DELAY);
	face_close(fd);

	return 0;
}
