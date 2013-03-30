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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vote_process.h"
#include "vote_handler.h"
#include "face_controller.h"
#include "speech_synthesis.h"

// Tiempo de espera para mover la cara tras inicializarla
#define FACE_DELAY 100000
// Longitud de los datos de posición (8 caracteres)
#define POSITION_LENGTH 8
// Archivo donde se guarda la posición anterior
#define POSITION_FILE "data/savedata.mfc"
// Posición más positiva
#define POSITION_MAX FACE_HAPPY
// Posición más negativa
#define POSITION_MIN FACE_SAD
// Archivo con el mensaje a sintetizar si se recibe un voto positivo
#define FILE_MAX "messages/vote_plus.txt"
// Archivo con el mensaje a sintetizar si se recibe un voto negativo
#define FILE_MIN "messages/vote_minus.txt"

int savePosition(unsigned char *position, const char *filename);
// Procesa un voto obtenido del servidor: mueve la cara a la
// posición correspondiente al voto, reproduce un mensaje, 
//guarda el voto, vuelve a mover la cara a la posición correspondiente
// al recuento de votos y guarda la posición de la cara.
// Entradas:
// - vote: voto (1 positivo, 0 negativo)
void processVote(int vote) {
	addVote(vote); // añadir voto al recuento
	// Mover cara a posición correspondiente al voto
	int fd=face_initialize();
	usleep(FACE_DELAY);
	face_setFace(fd, vote?POSITION_MAX:POSITION_MIN);
	// Reproducir mensaje
	speech_initialize();
	say_file(vote?FILE_MAX:FILE_MIN);
	speech_close();
	// Mover a posición correspondiente al recuento de votos
	VoteCount *count=getVoteCount();
	unsigned char position[POSITION_LENGTH];
	calculatePosition(position, count->plus, count->minus);
	face_setFace(fd, position);
	face_close(fd);
	savePosition(position, POSITION_FILE);
}

// Guarda la posición en el fichero
// Entradas:
// - position: posición a guardar
// - filename: nombre del fichero
// Valor de retorno: 0 si hay error, 1 si no
int savePosition(unsigned char *position, const char *filename) {
	FILE *file=fopen(filename, "w"); // abrir o crear archivo
	if (file==NULL) {
		perror("vote_process: No se pudo guardar la posición: ");
		return 0;
	}
	for (int i=0;i<POSITION_LENGTH;i++) {
		// Escribir 8 números entre 0 y 255
		if(fprintf(file, "%hhu ", position[i])<=0) {
			perror("vote_process: No se pudo guardar la posición: ");
			fclose(file);
			return 0;
		}
	} 
	fclose(file);
	return 1;
}
// Calcula la posición correspondiente al recuento de votos, utilizando
// una interpolación lineal entre POSITION_MIN y POSITION_MAX
// Entradas:
// - result: array donde se devolverá el resultado
// - plus: número de votos positivos
// - minus: número de votos negativos
void calculatePosition(unsigned char *result, int plus, int minus) {
	float ratio=(plus+minus)?((float)plus/(float)(plus+minus)):0.5;
	// interpolación lineal para cada elemento del array
	for (int i=0;i<POSITION_LENGTH;i++) {
		result[i]=(unsigned char)(POSITION_MIN[i]+(float)(POSITION_MAX[i]-POSITION_MIN[i])*ratio);
	}
}