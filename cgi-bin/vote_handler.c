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

#include "vote_handler.h"
// Archivo con el recuento de votos
#define FILE_VOTE "data/vote.vth"

// Añade un voto al recuento.
// Entradas:
// - vote: voto a añadir (1 positivo, 0 negativo)
void addVote(int vote) {
	int plus, minus;
	if (access(FILE_VOTE, F_OK)) {
		// si no existe el archivo de recuento, empezar de 0
		plus=0;
		minus=0;
		if (vote) plus++; else minus++;
		// crear archivo y guardar recuento
		FILE* file=fopen(FILE_VOTE, "w"); 
		fprintf(file, "%d\n%d\n", plus, minus);
		fclose(file);
	} else {
		// si existe archivo de recuento
		FILE* file=fopen(FILE_VOTE, "r+");
		// leer recuento
		fscanf(file, "%d\n%d\n", &plus, &minus);
		// incrementar
		if (vote) plus++; else minus++;
		// volver a guardar
		fseek(file, 0, SEEK_SET);
		fprintf(file, "%d\n%d\n", plus, minus);
		fclose(file);
	}
}

// Obtiene el recuento de votos en una estructura.
// Valor de retorno: puntero a una estructura de tipo VoteCount,
// con campos plus: int (votos positivos) y minus: int (votos 
// negativos).
VoteCount *getVoteCount() {
	// reservar memoria
	VoteCount *result=(VoteCount*)malloc(sizeof(VoteCount));
	// obtener datos
	FILE *file = fopen (FILE_VOTE, "r");
	fscanf(file, "%d\n%d\n", &(result->plus), &(result->minus));
	fclose(file);
	return result;
}

// Pone a 0 el recuento de votos
void resetVotes() {
	FILE* file=fopen(FILE_VOTE, "w");
	fprintf(file, "%d\n%d\n", 0, 0);
	fclose(file);
}