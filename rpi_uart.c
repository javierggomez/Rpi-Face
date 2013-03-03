/*
Código para acceso sencillo a la UART de la Raspberry Pi
(GPIO 14: TX y 15: RX). Antes, desbloquear UART siguiendo
estos pasos: 
http://raspberrypihobbyist.blogspot.com.es/2012/08/raspberry-pi-serial-port.html
y añadir permisos con el siguiente comando:
 sudo chmod a+rw /dev/ttyAMA0
*/
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

#include "rpi_uart.h"

#define UART_PATH "/dev/ttyAMA0"
#define DEFAULT_BAUDRATE B19200

/*
Configuración inicial de la UART. Por defecto, 9600bps,
8 bits de datos, sin paridad, 1 bit de parada y no bloquear
en lectura.
Argumentos: read: acceso de lectura (1) o no (0), 
write: acceso de escritura (1) o no (0), flags: otros flags
de acceso a archivos (ver fcntl.h)
Devuelve: descriptor de archivo, para ser usado como 
parámetro en todas las demás funciones de UART, o -1
si hay algún error.
*/
int uart_initialize(int read, int write, int flags) {
	// obtener flag de lectura/escritura
	int rw_flag;
	if (read&&write) rw_flag=O_RDWR;
	else if (read) rw_flag=O_RDONLY;
	else if (write) rw_flag=O_WRONLY;
	else return -1;
	
	flags|=rw_flag;
	// O_NOCTTY: no controlado por terminal.
	// O_NDELAY: abrir inmediatamente
	flags=flags|O_NOCTTY|O_NDELAY; 
	int result=open(UART_PATH, flags);
	if (result>-1) {
		uart_setBaudRate(result, DEFAULT_BAUDRATE);
		uart_setBlockForInput(result, 0);
	}
	return result;
}

// Cerrar el puerto UART.
void uart_close(int fd) {
	close(fd);
}

/*
Define si el método uart_read debe bloquearse esperando datos.
Argumentos: fd: descriptor de archivo obtenido de 
uart_initialize. nChars: número de caracteres que espera el 
método uart_read para desbloquearse. Poner a 0 para que no
se bloquee nunca.
*/
void uart_setBlockForInput(int fd, int nChars) {
	struct termios options;
	tcgetattr(fd, &options);
	// Modificar la opción c_cc[VMIN]: mínimo número de caracteres
	// de una lectura.
	options.c_cc[VMIN]=nChars; 
	tcsetattr(fd, TCSANOW, &options);
}

/*
Define la velocidad del puerto serie. 
Argumentos: fd: descriptor de archivo obtenido de 
uart_initialize. baudRate: constante definida en termios.h
para la velocidad deseada (ejemplos: B9600, B115200, etc).
*/
void uart_setBaudRate(int fd, int baudRate) {
	struct termios options;
	tcgetattr(fd, &options);
	// CS8: 8 bits de datos, sin paridad, 1 bit de parada.
	// CLOCAL: conexión local, sin control externo.
	// CREAD: habilitar recepción de datos.
	options.c_cflag=baudRate|CS8|CLOCAL|CREAD;
	// IGNPAR: ignorar paridad
	// ICRNL: igualar retorno de carro y salto de línea.
	options.c_iflag=IGNPAR|ICRNL;
	options.c_oflag=0;
	options.c_lflag=0;
	// Vaciar buffer de datos
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, &options);
}

/*
Transmite datos por UART. 
Argumentos: fd: descriptor de archivo obtenido de 
uart_initialize. string: buffer o cadena de caracteres a
enviar. nChars: número de caracteres que se quiere enviar
Devuelve: número de caracteres enviados realmente (menor
o igual que nChars, puede ser menor debido a limitaciones
de la velocidad o a que no haya suficientes en string).
Devuelve -1 si hay error de escritura.
*/
int uart_write(int fd, char *string, int nChars) {
	return write(fd, string, nChars);
}

/*
Lee datos del buffer de recepción de UART.
Argumentos: fd: descriptor de archivo obtenido de 
uart_initialize. buffer: array donde se almacenarán los
datos leídos. nChars: número máximo de datos que se pretende
leer.
Devuelve: -1 si hubo error de lectura, o el número de datos
leídos realmente.
*/
int uart_read(int fd, char *buffer, int nChars) {
	return read(fd, buffer, nChars);
}
