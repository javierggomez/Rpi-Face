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

// Servidor de análisis y acciones. Consiste en un bucle software que se
// mantiene a la espera de comandos recibidos del servidor y analiza los 
// mensajes que llegan o ejecuta los comandos que recibe (movimiento de 
// la cabeza o reproducción de textos).

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

// Cliente para servidor Freeling
#include "freeling_client.h"
using namespace std;

#define MESSAGE_MAX_LENGTH 65536

// Envía un mensaje al servidor Freeling para que lo analice.
// Entradas:
// - result: cadena para almacenar el resultado
// message: mensaje a analizar
// - portno: número de puerto del servidor
int freeling_analyze(char *result, const char *message, int portno)
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    // abrir socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
		perror("Error al abrir el socket");
		return 0;

	}
    // definir el servidor como localhost en el puerto portno
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr,"Error: no se puede conectar con localhost\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    // conectar con el servidor
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		perror("Error al conectar con el servidor");
		return 0;
	}
    // mandar el mensaje por el socket
    n = write(sockfd,message,strlen(message));
    if (n < 0) {
		perror("Error al escribir en el socket");
		return 0;
	}
    // recibir la respuesta
    n = read(sockfd,result,MESSAGE_MAX_LENGTH);
    if (n < 0) {
		perror("Error al leer del socket");
		return 0;
	}
    close(sockfd);
    return 1;
}