
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "face_controller.h"
#define DELAY 1000000

//Prueba de las caras sin comprobar errores
int main(void) {
	int fd = face_initialize();
	usleep(DELAY);
	printf("Iniciando\n");

	printf("Cara contenta\n");
	face_setFace(fd,FACE_HAPPY);
		usleep(5*DELAY);

	printf("Guardando posición por defecto\n");
	face_setAsHome(fd);
		usleep(5*DELAY);
	printf("Cara triste\n");
	face_setFace(fd,FACE_SAD);
		usleep(5*DELAY);
	printf("Cara sorprendida\n");
	face_setFace(fd,FACE_SURPRISE);
		usleep(5*DELAY);
	printf("Cara enfadada\n");
	face_setFace(fd,FACE_ANGRY);
		usleep(5*DELAY);
	printf("Cara neutra\n");
	face_setFace(fd,FACE_NEUTRAL);
		usleep(5*DELAY);
	printf("Posición por defecto\n");
	face_goToHome(fd);
		usleep(5*DELAY);
	printf("Apagando\n");
	face_turnOff(fd);
		usleep(5*DELAY);
	face_close(fd);

	return 0;
}
