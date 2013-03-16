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

// Código para controlar la cara.
#include <unistd.h>

#include "face_controller.h"
#include "servo_controller.h"

const unsigned char FACE_HAPPY[8]= {127,0, 0, 0, 127, 127, 240,240};
const unsigned char FACE_SAD[8]= {127,0, 0, 0, 30, 210, 10,10};
const unsigned char FACE_SURPRISE[8]= {200,0, 0, 0, 20, 170, 127,127};
const unsigned char FACE_ANGRY[8]= {80,0, 0,0, 215, 30, 127,127};
const unsigned char FACE_NEUTRAL[8]= {127,0, 0,0, 127, 127, 127,127};

/*
Inicializa el acceso a la cara
*/
int face_initialize() {
	return servo_initialize();
}


/*
Pone la cara actual como cara por defecto
Argumentos: fd: descriptor de archivo obtenido de uart_initialize.
Devuelve: 1 si ha habido algún error, y 0 si todo ha ido bien.
*/
int face_setAsHome(int fd){
	int e=0;
	e+=servo_setAsHome(fd, 1);
	for (int i=5;i<9;i++) {
		e+=servo_setAsHome(fd, i);
	}
	if(e>0){
		return 1;
	}else{
		return 0;
	}
}


/*
Retorna al estado de cara por defecto
Argumentos: fd: descriptor de archivo obtenido de uart_initialize.
Devuelve: 1 si ha habido algún error, y 0 si todo ha ido bien.
*/
int face_goToHome(int fd){
	int e=0;
	e+=servo_goToHome(fd, 1);
	for (int i=5;i<9;i++) {
		e+=servo_goToHome(fd, i);
	}
	if(e>0){
		return 1;
	}else{
		return 0;
	}
}

/*
Pone una cara determinada
Argumentos: fd: descriptor de archivo obtenido de uart_initialize.
Devuelve: 1 si ha habido algún error, y 0 si todo ha ido bien.
*/
int face_setFace(int fd, unsigned const char *positions){
	return servo_setAllServosPositions(fd, positions); 
}

/*
Hace que la cara parpadee un número de veces
Argumentos: fd: descriptor de archivo obtenido de uart_initialize.
times: número de veces a parpadear
current: posición actual de los ojos
Devuelve: 1 si ha habido algún error, y 0 si todo ha ido bien.
//Hay que probar si puede mantener el ritmo el robot (incluso si no necesita el tiempo ese de espera y lo almacena en un buffer todo)
*/
int face_blink(int fd, int times, unsigned char current){
	for (int i=0;i<=times;i++) {
		servo_setServoPosition(fd, 1, 0);
		usleep(10000);
		servo_setServoPosition(fd, 1, current);
	}
	return 1;
}

/*
Apaga todos los servos de la cara
Argumentos: fd: descriptor de archivo obtenido de uart_initialize.
Devuelve: 1 si ha habido algún error, y 0 si todo ha ido bien.
*/
int face_turnOff(int fd){
	int e=0;
	e+=servo_turnOff(fd, 1);
	for (int i=5;i<9;i++) {
		e+=servo_turnOff(fd, i);
	}
	if(e>0){
		return 1;
	}else{
		return 0;
	}
	
}

/*
Termina el acceso a la cara
Argumentos: fd: descriptor de archivo obtenido de uart_initialize.
*/
void face_close(int fd){
	servo_close(fd);
}

