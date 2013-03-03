
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "servo_controller.h"

#define DELIMITER " "
int main(void) {
	int fd=servo_initialize();
	if (fd<0) {
		perror("No se pudo abrir el puerto UART");
		return -1;
	}
	int exit=0; // flag de salida
	char input[64]; // datos a enviar
	char *temp;
	while (!exit) {
		gets(input);
		temp=strtok(input, DELIMITER);
		printf("%s\n", temp);
		if (!strcmp(temp, "q")) {
			exit=1;
		}
		else if (!strcmp(temp, "move")) {
			temp=strtok(NULL, DELIMITER);
			printf("%s\n", temp);
			int servo=atoi(temp);
			printf("%i\n", servo);
			temp=strtok(NULL, DELIMITER);
			printf("%s\n", temp);
			unsigned char position=(unsigned char)atoi(temp);
			printf("%i\n", (int)position);
			if (servo_setServoPosition(fd,servo,position)) {
				perror("No se recibió respuesta");
			}
		} else if (!strcmp(temp, "moveAll")) {
			unsigned char position[8];
			for (int i=0;i<8;i++) {
				temp=strtok(NULL, DELIMITER);
				position[i]=(unsigned char)atoi(temp);
			}
			if (servo_setAllServosPositions(fd,position)) {
				perror("No se recibió respuesta");
			}
		} else if (!strcmp(temp, "read")) {
			temp=strtok(NULL, DELIMITER);
			int servo=atoi(temp);
			int error=0;
			unsigned char position=servo_getPosition(fd,servo,&error);
			if (error){
				perror("No se recibió respuesta");
			} else {
				printf("%i\n",position);
			}
						
		} else {
			perror("El comando no existe");
		}
		
	}
	servo_close(fd);
	return 0;

}
