
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

// Programa para calcular la nueva posición y enviársela a la cara
// a partir de la posición anterior (guardada en savedata.mvf) y
// la valoración del último mensaje.

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

#include "move_face.h"
#include "face_controller.h"

#define FACE_DELAY 100000
#define POSITION_LENGTH 8
#define POSITION_FILE "savedata.mfc"
#define WEIGHT_PREVIOUS 0.5  // peso de la posición anterior
#define WEIGHT_CURRENT 0.5   //peso de la posición calculada
#define TAG_MAX 9  // máxima valoración
#define TAG_MIN 1  // mínima valoración
#define POSITION_MAX FACE_HAPPY  // posición correspondiente a TAG_MAX
#define POSITION_MIN FACE_SAD  // posición correspondiente a TAG_MIN
#define DEFAULT_TAG TAG_MAX

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
	// Obtener posición calculada
	unsigned char calculatedPosition[POSITION_LENGTH];
	calculatePosition(calculatedPosition, tag);
	// Obtener nueva posición como media ponderada entre la antigua y la calculada
	unsigned char position[POSITION_LENGTH];
	weightedAverage(position, last_position, calculatedPosition, WEIGHT_PREVIOUS, WEIGHT_CURRENT, POSITION_LENGTH);
	// Mover la cabeza 
	usleep(FACE_DELAY);
	face_setFace(fd, position);
	// Guardar posición
	if (!savePosition(position, POSITION_FILE)) exit_status=1;
	face_close(fd);
	return exit_status;
}

// Carga la posición anterior del archivo con nombre filename y
// la guarda en position. La posición se guarda con un formato 
// que consiste en 8 números enteros separados por espacios.
// Devuelve 1 si la operación tuvo 
// éxito; 0 si no. Si devuelve 0, lo que haya en position
// es indeterminado.
int loadPosition(unsigned char *position, const char *filename) {
	// Abrir archivo
	FILE *file=fopen(filename, "r");
	if (file==NULL) {
		perror("move_face: ");
		return 0;
	}
	// Leer datos
	for (int i=0;i<POSITION_LENGTH;i++) {
		if(fscanf(file, "%hhu ", position+i)<=0) {
			fprintf(stderr, "Formato de datos guardados no válido");
			fclose(file);
			return 0;
		}
	}
	fclose(file);
	return 1;
}

// Guarda la posición position en el archivo filename. El formato de
// guardado son 8 enteros separados por espacios. Devuelve 1 si tiene
// éxito y 0 si no. Si devuelve 0, el contenido del archivo de salida
// es indeterminado.
int savePosition(unsigned char *position, const char *filename) {
	// Abrir archivo
	FILE *file=fopen(filename, "w");
	if (file==NULL) {
		perror("move_face: No se pudo guardar la posición: ");
		return 0;
	}
	// Escribir datos
	for (int i=0;i<POSITION_LENGTH;i++) {
		if(fprintf(file, "%hhu ", position[i])<=0) {
			perror("move_face: No se pudo guardar la posición: ");
			fclose(file);
			return 0;
		}
	} 
	fclose(file);
	return 1;
}

// Calcula una media ponderada entre dos arrays de unsigned char de
// longitud length , con pesos weight1 y weight2. Devuelve el resultado
// en result.
void weightedAverage(unsigned char *result, const unsigned char *array1, const unsigned char *array2, float weight1, float weight2, int length) {
	for (int i=0;i<length;i++) {
		result[i]=(unsigned char)(weight1*(float)array1[i]+weight2*(float)array2[i]);
		printf("%hhu ", result[i]);
	}
	printf("\n");
}

// Calcula la nueva posición a partir de la nota tag. El procedimiento
// consiste en interpolar linealmente entre POSITION_MIN y POSITION_MAX
// según el valor de tag entre TAG_MIN y TAG_MAX. Devuelve el resultado 
// en result.
void calculatePosition(unsigned char *result, float tag) {
	if (tag<TAG_MIN) tag=TAG_MIN;
	else if (tag>TAG_MAX) tag=TAG_MAX;
	for (int i=0;i<POSITION_LENGTH;i++) {
		result[i]=(unsigned char)(POSITION_MIN[i]+(float)(POSITION_MAX[i]-POSITION_MIN[i])*(float)(tag-TAG_MIN)/(float)(TAG_MAX-TAG_MIN));
	}
}
