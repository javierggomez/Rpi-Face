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


/*
Código de control de los servos srv8-t a través de una UART
*/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>

#include "servo_controller.h"
#include "rpi_uart.h"

#define HEADER '>'
#define MODULE '1'
#define STANDARD '0'
#define EXTENDED '1'
#define ACK 13
#define ACK_TIMEOUT 100000

/*
Inicializa el acceso a los servos
*/
int servo_initialize() {
	return uart_initialize(1,1,0);
}

/*
Lee datos que presumiblemente haya enviado la controladora de servos.
Se utiliza para control de errores y para consultar posiciones de servos.
Entradas:
- fd: descriptor de archivo obtenido de uart_initialize.
- buffer: array donde se almacenarán losdatos leídos.
- nChars: número máximo de datos que se pretende leer.
- useconds: número de microsegundos que se espera para leer (para dejar
            tiempo a la controladora a que reaccione)
Valor de retorno: -1 si hubo error de lectura, o el número de datos
leídos realmente.
*/
int servo_read(int fd, char *buffer, int nChars, int useconds) {
	usleep(useconds);
	return uart_read(fd, buffer, nChars);
	
}

/*
Mueve un servo a una posición determinada.
Entradas:
- fd: descriptor de archivo obtenido de uart_initialize.
- servo: número de servo a mover.
- position: posición en la que se quiere poner.
Valor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_setServoPosition(int fd, int servo, unsigned char position) {
	if (!(servo==1||(servo>4&&servo<9))) {
		perror("Número de servo no válido");
		return 1;
	}
	// Construir el comando '>1[servo]a[posición]'
	char command[6];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=servo+48;
	command[3]='a';
	command[4]=position;
	command[5]=0;
	return uart_write(fd, command, 5); // enviar el comando
	
	
}

/*
Mueve todos los servos a posiciones determinadas.
Entradas:
- fd: descriptor de archivo obtenido de uart_initialize.
- positions: array con las posiciones en la que se quieren poner.
Calor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_setAllServosPositions(int fd, unsigned const char *positions) {
	// Construir el comando '>1m[posiciones]'
	char command[13];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=49;
	command[3]='m';
	for (int i=0;i<8;i++) {
		command[i+4]=positions[i];
	}
	command[12]=0;

	return uart_write(fd, command, 12); // enviar el comando
	
}

/*
Apaga un servo.
Entradas:
- fd: descriptor de archivo obtenido de uart_initialize.
- servo: número de servo a mover.
Valor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_turnOff(int fd, int servo){
	return servo_setServoPosition(fd,servo,0);
}

/*
Pone la posición actual del servo como home.
Entradas: 
- fd: descriptor de archivo obtenido de uart_initialize.
- servo: número de servo.
Valor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_setAsHome(int fd, int servo){
	// Construir el comando '>1[servo]c'
	char command[5];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=servo+48;
	command[3]='c';
	command[4]=0;
	int w = uart_write(fd, command, 4); // Enviar comando
	if (w ==-1) return 1;
	char response=0;
	if (servo_read(fd, &response, 1, ACK_TIMEOUT)==1&&response==ACK) {
		return 0;
	} else {
		return 1;
	}	
}

/*
Mueve un servo a su posición home.
Entradas: 
- fd: descriptor de archivo obtenido de uart_initialize.
- servo: número de servo.
Valor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_goToHome(int fd, int servo){
	// Construir el comando '>1[servo]h'
	char command[5];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=servo+48;
	command[3]='h';
	command[4]=0;
	int w = uart_write(fd, command, 4); // Enviar comando
	if (w ==-1) return 1;
	char response=0;
	if (servo_read(fd, &response, 1, ACK_TIMEOUT)==1&&response==ACK) {
		return 0;
	} else {
		return 1;
	}
}

/*
Fija la resolución de un servo en standard o extended.
Entradas:
- fd: descriptor de archivo obtenido de uart_initialize.
- servo: número de servo.
- resolution:resolución deseada.
Valor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_setWidthResolution(int fd, int servo, char resolution){
	if (resolution!=STANDARD && resolution!=EXTENDED) {
		perror("Resolución inválida");
		return 1;
	}
	// Construir el comando '>1[servo]w[resolución]'
	char command[6];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=servo+48;
	command[3]='w';	
	command[4]=resolution;
	command[5]=0;
	int w = uart_write(fd, command, 5); // Enviar comando
	if (w ==-1) return 1;
	char response=0;
	if (servo_read(fd, &response, 1, ACK_TIMEOUT)==1&&response==ACK) {
		return 0;
	} else {
		return 1;
	}
}

/*
Consulta a la controladora la posición actual de un servo.
Entradas: fd: descriptor de archivo obtenido de 
uart_initialize. servo: número de servo.
error: flag que indica si todo ha ido bien o ha habido errores
Valor de retorno: 0 si ha habido algún error, o la posición si todo ha
ido correctamente
*/
unsigned char servo_getPosition(int fd, int servo, int *error){
	*error=1;
	char command[5];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=servo+48;
	command[3]='g';
	command[4]=0;
	int w = uart_write(fd, command, 4);
	if (w ==-1) return 0;
	char response[2];
	if (servo_read(fd, response, 2, ACK_TIMEOUT)==2&&response[1]==ACK) {
		*error=0;		
		return response[0];
	} else {
		return 0;
	}
}

/*
Consulta a la controladora de servos la torque de un servo.
Entradas: fd: descriptor de archivo obtenido de 
uart_initialize. servo: número de servo.
error: flag que indica si todo ha ido bien o ha habido errores
Valor de retorno: 0 si ha habido algún error, o la torque si todo ha
ido correctamente
*/
int servo_getTorque(int fd, int servo, int *error){
	*error=1;
	char command[5];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=servo+48;
	command[3]='t';
	command[4]=0;
	return uart_write(fd, command, 4);
	char response[3];
	if (servo_read(fd, response, 3, ACK_TIMEOUT)==3&&response[2]==ACK) {
		*error=0;		
		return (int)(response[1])*256+(int)(response[0]);
	} else {
		return 0;
	}	
}

/*
Configura la tasa de símbolo tanto en la controladora como en la Rapsberri
Entradas: fd: descriptor de archivo obtenido de 
uart_initialize. rate: tasa de símbolo.
Valor de retorno: 1 si ha habido algún error, o 0 si todo ha
ido correctamente
*/
int servo_setBaudRate(int fd, int rate){
	char rateCode;
	switch (rate) {
		case B2400:
			rateCode='3';
			break;
		case B4800:
			rateCode='2';
			break;
		case B9600:
			rateCode='1';
			break;
		case B19200:
			rateCode='0';
			break;
		default:
			perror("Resolución inválida");
			return 1;	
	}
	
	char command[6];
	command[0]=HEADER;
	command[1]=MODULE;
	command[2]=49;
	command[3]='b';	
	command[4]=rateCode;
	command[5]=0;
	int w = uart_write(fd, command, 5);
	if (w ==-1) return 1;
	char response=0;
	if (servo_read(fd, &response, 1, ACK_TIMEOUT)==1&&response==ACK) {
		
		uart_setBaudRate(fd, rate);
		return 0;
	} else {
		return 1;
	}
}

/*
Cierra la conexión con la UART
argumentos: fd: descriptor de archivo obtenido de 
uart_initialize.
*/
void servo_close(int fd) {
	uart_close(fd);
}




