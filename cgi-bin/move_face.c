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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "move_face.h"
#include "face_controller.h"
#include "speech_synthesis.h"

// Tiempo de espera para mover la cara tras inicializarla
#define FACE_DELAY 100000
// Longitud de los datos de posición (8 caracteres)
#define POSITION_LENGTH 8
// Archivo donde se guarda la posición anterior
#define POSITION_FILE "data/savedata.mfc"
// Peso de la posición anterior en la ponderación
#define WEIGHT_PREVIOUS 0.5
// Peso de la posición calculada en la ponderación
#define WEIGHT_CURRENT 0.5
// Nota máxima de un mensaje
#define TAG_MAX 9
// Nota mínima de un mensaje
#define TAG_MIN 1
// Posición correspondiente a la nota máxima
#define POSITION_MAX FACE_HAPPY
// Posición correspondiente a la nota mínima
#define POSITION_MIN FACE_SAD
// Nota por defecto, si se llama al programa sin argumentos
#define DEFAULT_TAG TAG_MAX
// Archivo donde está guardado el mensaje
#define MESSAGE_FILE "data/fichero.raw"
// Mensaje cuando mejora el estado de ánimo
#define FILE_HAPPIER "messages/TEXT_HAPPIER.txt"
// Mensaje cuando empeora el estado de ánimo
#define FILE_SADDER "messages/TEXT_SADDER.txt"
// Mensaje cuando el cambio es pequeño
#define FILE_NOCHANGE "messages/TEXT_NOCHANGE.txt"
// Tolerancia de cambio de estado
#define MOOD_TOLERANCE 16

// Programa para calcular la nueva posición de la cara a partir de la antigua 
// (guardada en el archivo POSITION_FILE) y la valoración del último mensaje
// (pasada como argv[1]). Calcula la nueva posición, mueve la cara, reproduce
// el mensaje (que está en el archivo MESSAGE_FILE) y guarda la nueva posición.
int main(int argc, char **argv) {
	int exit_status=0;
	// Iniciar comunicación con la cabeza
	int fd=face_initialize();
	// Leer etiqueta 
	float tag=DEFAULT_TAG;
	if (argc>=2) {
		tag=atof(argv[1]);
		if (tag<TAG_MIN) {
			tag=TAG_MIN;
			fprintf(stderr, "%s: etiqueta no válida.\n", argv[1]);
		} else if (tag>TAG_MAX) {
			tag=TAG_MAX;
			fprintf(stderr, "%s: etiqueta no válida.\n", argv[1]);
		}
	}
	// Cargar última posición
	unsigned char last_position[POSITION_LENGTH];
	if (!loadPosition(last_position, POSITION_FILE)) weightedAverage(last_position, POSITION_MIN, POSITION_MAX, 0.5, 0.5, POSITION_LENGTH);
	// Obtener nueva posición
	unsigned char calculatedPosition[POSITION_LENGTH];
	calculatePosition(calculatedPosition, tag);
	unsigned char position[POSITION_LENGTH];
	weightedAverage(position, last_position, calculatedPosition, WEIGHT_PREVIOUS, WEIGHT_CURRENT, POSITION_LENGTH);
	// Mover la cabeza 
	usleep(FACE_DELAY);
	face_setFace(fd, position);
	// Guardar posición
	if (!savePosition(position, POSITION_FILE)) exit_status=1;
	face_close(fd);
	speech_initialize();
	// Reproducir mensaje
	say_file(MESSAGE_FILE);
	// Reproducir mensaje correspondiente al cambio de estado de ánimo
	usleep(FACE_DELAY);
	if (position[7]>last_position[7]+MOOD_TOLERANCE) say_file(FILE_HAPPIER);
	else if (position[7]<last_position[7]-MOOD_TOLERANCE) say_file(FILE_SADDER);
	else say_file(FILE_NOCHANGE);
	speech_close();
	return exit_status;
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
int savePosition(unsigned char *position, const char *filename) {
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
		fprintf(stderr, "%hhu ", result[i]);
	}
	fprintf(stderr, "\n");
}

// Calcula la posición correspondiente a la nota tag, utilizando
// una interpolación lineal entre POSITION_MIN y POSITION_MAX
// según el valor de tag entre TAG_MIN y TAG_MAX
// Entradas:
// - result: array donde se devolverá el resultado
// - tag nota del mensaje
void calculatePosition(unsigned char *result, float tag) {
	// Asegurar que tag está dentro de los límites
	if (tag<TAG_MIN) tag=TAG_MIN;
	else if (tag>TAG_MAX) tag=TAG_MAX;
	// interpolación lineal para cada elemento del array
	for (int i=0;i<POSITION_LENGTH;i++) {
		result[i]=(unsigned char)(POSITION_MIN[i]+(float)(POSITION_MAX[i]-POSITION_MIN[i])*(float)(tag-TAG_MIN)/(float)(TAG_MAX-TAG_MIN));
	}
}
