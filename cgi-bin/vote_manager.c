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

// Programa para que el administrador del servidor pueda controlar los votos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "vote_handler.h"

// Control del sistema de votaciones. Sin argumentos, muestra el recuento.
// Con -a plus o -a minus, añade un voto positivo o negativo. Con -r 
// reinicia el recuento.
int main(int argc, char **argv) {
	// obtener opciones
	char c;
	int exitStatus=0;
	while ((c=getopt(argc, argv, "a:r"))!=255) {
		switch (c) {
			case 'a':
				// añadir voto
				switch (optarg[0]) {
					case 'p':
					case 'P':
					case '+':
						printf("Voto positivo.\n");
						addVote(1);
						break;
					case 'm':
					case 'M':
					case 'n':
					case 'N':
						printf("Voto negativo.\n");
						addVote(0);
						break;
					default:
						fprintf(stderr, "Error: no se reconoce el voto: %s\n", optarg);
						exitStatus=1;
				}
				break;
			case 'r':
				// reiniciar votos
				printf("Votos reiniciados.\n");
				resetVotes();
				break;
			case '?':
		    	if (optopt == 'a') {
			    	fprintf (stderr, "vote_manager: error: la opción '-%c' requiere un argumento.\n", optopt);
				    exitStatus=1;
			    } else if (isprint (optopt)) {
	 	        	fprintf (stderr, "vote_manager: error: opción desconocida:  '-%c'.\n", optopt);
	 	        	exitStatus=1;
			    } else {
			        fprintf (stderr, "vote_manager: error: opción desconocida.");
			        exitStatus=1;
			    }
		}
	}
	// mostrar votos
	VoteCount *count=getVoteCount();
	printf("Votos positivos: %d\n", count->plus);
	printf("Votos negativos: %d\n", count->minus);
	free(count);
	return exitStatus;
}