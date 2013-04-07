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

// Programa encargado de mover la cara y guardar la última posición
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "position.h"
#include "face_controller.h"

// Tiempo de espera para mover la cara tras inicializarla
#define FACE_DELAY 100000
// Longitud de los datos de posición (8 caracteres)
#define POSITION_LENGTH 8
// Archivo donde se guarda la posición anterior
#define POSITION_FILE "data/savedata.mfc"
// Posición correspondiente a la nota máxima
#define POSITION_MAX FACE_HAPPY
// Posición correspondiente a la nota mínima
#define POSITION_MIN FACE_SAD

// Coloca la cara en una posición y la guarda en el archivo
// Entradas:
// - position: posición a colocar
void setPosition(const unsigned char *position) {
	int fd=face_initialize();
	usleep(FACE_DELAY);
	face_setFace(fd, position);
	face_close(fd);
	// Guradar en archivo
	savePosition(position, POSITION_FILE);
}

// Coloca la cara en una posición calculada a partir de la anterior
// y de la pasada como argumento. Guarda en el archivo la nueva
// posición.
// Entradas:
// - position: posición con la que hacer media
// - previousWeight: peso de la posición anterior en la ponderación
//	 (entre 0 y 1) 
// Valor de retorno: diferencia entre la posición anterior del servo
// 8 y la actual.
int setPosition(const unsigned char *position, float previousWeight) {
	// Cargar última posición
	unsigned char last_position[POSITION_LENGTH];
	if (!loadPosition(last_position, POSITION_FILE)) weightedAverage(last_position, POSITION_MIN, POSITION_MAX, 0.5, 0.5, POSITION_LENGTH);
	unsigned char finalPosition[POSITION_LENGTH];
	weightedAverage(finalPosition, last_position, position, previousWeight, 1-previousWeight, POSITION_LENGTH);
	// Mover la cabeza 
	setPosition(finalPosition);
	return (int)(finalPosition[7])-(int)(last_position[7]);
}

// Carga la posición anterior del fichero
// Entradas:
// - position: array donde se devolverá el resultado
// - filename: nombre del archivo donde está guardada la posición
// Valor de retorno: 0 si hay error, 1 si no
int loadPosition(unsigned char *position, const char *filename) {
	FILE *file=fopen(filename, "r"); // abrir archivo
	if (file==NULL) {
		perror("move_face: ");
		return 0;
	}
	for (int i=0;i<POSITION_LENGTH;i++) {
		// Leer 8 números entre 0 y 255 (unsigned char, %hhu)
		if(fscanf(file, "%hhu ", position+i)<=0) {
			fprintf(stderr, "Formato de datos guardados no válido");
			fclose(file);
			return 0;
		}
	}
	fclose(file); // cerrar archivo
	return 1;
}

// Guarda la posición en el fichero
// Entradas:
// - position: posición a guardar
// - filename: nombre del fichero
// Valor de retorno: 0 si hay error, 1 si no
int savePosition(const unsigned char *position, const char *filename) {
	FILE *file=fopen(filename, "w"); // abrir o crear archivo
	if (file==NULL) {
		perror("move_face: No se pudo guardar la posición: ");
		return 0;
	}
	for (int i=0;i<POSITION_LENGTH;i++) {
		// Escribir 8 números entre 0 y 255
		if(fprintf(file, "%hhu ", position[i])<=0) {
			perror("move_face: No se pudo guardar la posición: ");
			fclose(file);
			return 0;
		}
	} 
	fclose(file);
	return 1;
}

// Calcula la media ponderada de dos arrays de unsigned char
// Entradas:
// - result: array donde se devolverá el resultado
// - array1: primer array de datos
// - array2: segundo array de datos
// - weight1: peso del primer array en la media
// - weight2: peso del segundo array en la media
// - length: longitud de los arrays
void weightedAverage(unsigned char *result, const unsigned char *array1, const unsigned char *array2, float weight1, float weight2, int length) {
	for (int i=0;i<length;i++) {
		result[i]=(unsigned char)(weight1*(float)array1[i]+weight2*(float)array2[i]);
	}
}

