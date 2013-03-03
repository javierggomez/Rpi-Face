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


// Prueba de rpi_uart.c. Envía por UART lo que escribe el usuario por stdin
// Después de cada entrada del usuario, lee el buffer de recepción UART y 
// muestra los datos leídos. Salir con Ctrl-C o escribiendo cualquier cosa que 
// empiece por q.

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "rpi_uart.h"

int main(void) {
	int fd=uart_initialize(1,1,0); // iniciar UART con acceso RW
	if (fd<0) {
		perror("No se pudo abrir el puerto UART");
		return -1;
	}
	int exit=0; // flag de salida
	int readChars=0; // número de caracteres leídos
	char input[64]; // datos a enviar
	char rdBuffer[64]; // datos recibidos
	while (!exit) {
		gets(input);
		if (input[0]=='q') exit=1;
		// Enviar datos
		if (uart_write(fd, input, strlen(input))<0) perror("Error de escritura");
		// Leer datos
		readChars=uart_read(fd,rdBuffer, 255);
		if (readChars<0) perror("Error de lectura");
		else if (readChars>0) {
			rdBuffer[readChars]=0;
			printf("%s\n", rdBuffer);
		}
		usleep(1000);
	}
	return 0;

}
