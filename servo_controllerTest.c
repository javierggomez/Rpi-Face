#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "servo_controller.h"

int main (void) {
//inicializa
	int fd=servo_initialize();
	if (fd<0) {
		perror("No se pudo abrir el puerto UART");
		return -1;
	}
//pone todos los servos a la mitad
	usleep(100000);
	for (int i=1;i<9;i++) {
		servo_setServoPosition(fd, i, (unsigned char)200);
		usleep(100000);
		//sleep(10/1000);
	}
	sleep(1);

//pone todos los servos a 0
	for (int i=1;i<9;i++) {
		servo_setServoPosition(fd, i, (unsigned char)50);
		usleep(100000);
	}
	sleep(1);

//pone todos los servos al máximo con una sola orden
	unsigned char *pPositions;
	unsigned char positions[8] = {255,255, 255, 255, 255, 255, 255,255};
	pPositions = positions;
	servo_setAllServosPositions(fd, pPositions); 
	sleep(1);


//apaga todos los servos
	for (int i=1;i<9;i++) {
		servo_turnOff(fd, i);
	}
	sleep(1);

//pone todos los servos a la mitad y lo pone como la posición HOME
	for (int i=1;i<9;i++) {
		servo_setServoPosition(fd, i, 127);
		servo_setAsHome(fd, i);
	}
	sleep(1);
//apaga todos los servos
	for (int i=1;i<9;i++) {
		servo_turnOff(fd, i);
	}
	sleep(1);

//regresa al home de todos los servos, que debería ser la mitad
	for (int i=1;i<9;i++) {
		servo_goToHome(fd, i);
	}
	sleep(1);
}
