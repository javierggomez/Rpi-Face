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

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdarg>
#include <signal.h>
#include <cstdlib>

#include "hardware.h"

// Tiempo entre comprobaciones
#define LOOP_DELAY 100000
// Tiempo antirrebotes
#define DEBOUNCE_DELAY 8000
// Umbral antirrebotes
#define DEBOUNCE_THRESHOLD 8
// Máximo de comprobaciones antirrebotes
#define DEBOUNCE_MAX_CHECKS 100
// GPIO para el botón
#define GPIO_BUTTON RPI_GPIO_P1_16
// GPIO para el LED rojo
#define GPIO_RED RPI_GPIO_P1_18
// GPIO para el LED amarillo
#define GPIO_YELLOW RPI_GPIO_P1_15

using namespace std;

void INThandler(int sig); // Controlador de interrupción Ctrl-C
void onButtonPressed();
void runInBackground(const char *format, ...);

bool g_main_running=true;
bool g_server_running=false;

int main(){
	if (!gpio_init()) {
		cerr << "No se pudo acceder a los puertos" << endl;
		return 1;
	}
    gpio_fsel(GPIO_BUTTON, true);
    gpio_fsel(GPIO_RED, false);
    gpio_fsel(GPIO_YELLOW, false);
    gpio_writePin(GPIO_RED, true);
    gpio_writePin(GPIO_YELLOW, false);
    cout << "init: Listo" << endl;
	// turnOnRed();
	while(g_main_running){
		if (gpio_readPin(GPIO_BUTTON) && gpio_debounce(GPIO_BUTTON)) {
			onButtonPressed();
			gpio_waitForRelease(GPIO_BUTTON);
		}
		usleep(LOOP_DELAY);
	}
    gpio_close();
	//Habría que poner que cuando se termine todo de cargar se encienda la
	//verde dentro de action manager probablemente
	//Y un sistema para que en vez de con control-c se pueda terminar apagando un interruptor
}

// Manejador de la señal SIGINT (recibida si el usuario pulsa Ctrl-C). 
// Termina el bucle software y permite que se ejecute el código de 
// cierre del programa.
// Entradas:
// - sig: señal recibida
void INThandler(int sig) {
	signal(sig, SIG_IGN);
	g_main_running=false;
}

// Ejecutado cuando se pulsa el botón. Enciende o apaga el servidor.
// Cambia el estado de los LEDs en función del nuevo estado del
// servidor.
void onButtonPressed() {
	if (g_server_running) {
		cout << "Parando servidor" << endl;
		runInBackground("./rpi-face-server -x");
		gpio_writePin(GPIO_YELLOW, false);
		gpio_writePin(GPIO_RED, true);
		g_server_running=false;
	} else {
		cout << "Iniciando servidor" << endl;
		runInBackground("./rpi-face-server -e");
		gpio_writePin(GPIO_YELLOW, true);
		gpio_writePin(GPIO_RED, false);
		g_server_running=true;
	}
}

// Ejecuta un comando en background llamando a un subproceso y
// vuelve inmediatamente.
void runInBackground(const char *format, ...) {
	// crear subproceso
	pid_t pid=fork();
	if (pid < 0) {
		perror("init: fallo al crear proceso: ");
		exit(1);
	} else if (pid==0) {
		// subproceso
		va_list ap;
		va_start(ap, format);
		char command[256];
		vsprintf(command, format, ap);
		system(command); // ejecutar comando
		va_end(ap);
		exit(0);
	} else {
		// proceso principal: vuelve inmediatamente
		return;
	}
}
