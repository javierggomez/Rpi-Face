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
