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
#include "toggle_emotion.h"

// Tiempo entre comprobaciones
#define LOOP_DELAY 100000
// GPIO para el botón
#define GPIO_BUTTON RPI_GPIO_P1_16
// GPIO para el LED rojo
#define GPIO_RED RPI_GPIO_P1_18
// GPIO para el LED amarillo
#define GPIO_YELLOW RPI_GPIO_P1_15
// GPIO para el LED verde
#define GPIO_GREEN RPI_GPIO_P1_22
// GPIO para el botón de cambiar estado emocional
#define GPIO_EMOTION RPI_GPIO_P1_11

using namespace std;

void INThandler(int sig); // Controlador de interrupción Ctrl-C
void onButtonPressed();
void onEmotionPressed();
void runInBackground(const char *format, ...);

bool g_main_running=true;
bool g_server_running=false;

int main(){
	if (!gpio_init()) {
		cerr << "No se pudo acceder a los puertos" << endl;
		return 1;
	}
    gpio_fsel(GPIO_BUTTON, true);
    gpio_fsel(GPIO_EMOTION, true);
    gpio_fsel(GPIO_RED, false);
    gpio_fsel(GPIO_YELLOW, false);
    gpio_fsel(GPIO_GREEN, false);
    gpio_writePin(GPIO_RED, true);
    gpio_writePin(GPIO_YELLOW, false);
    gpio_writePin(GPIO_GREEN, false);
    cout << "init: Listo" << endl;
	// turnOnRed();
	while(g_main_running){
		if (gpio_readPin(GPIO_BUTTON) && gpio_debounce(GPIO_BUTTON)) {
			onButtonPressed();
			gpio_waitForRelease(GPIO_BUTTON);
		}
		if (gpio_readPin(GPIO_EMOTION) && gpio_debounce(GPIO_EMOTION)) {
			onEmotionPressed();
			gpio_waitForRelease(GPIO_EMOTION);
		}
		usleep(LOOP_DELAY);
	}
	gpio_writePin(GPIO_RED, false);
	gpio_writePin(GPIO_YELLOW, false);
	gpio_writePin(GPIO_GREEN, false);	
    gpio_close();
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
		gpio_writePin(GPIO_GREEN, false);
		gpio_writePin(GPIO_YELLOW, false);
		gpio_writePin(GPIO_RED, true);
		runInBackground("./rpi-face-server -x");
		g_server_running=false;
	} else {
		cout << "Iniciando servidor" << endl;
		gpio_writePin(GPIO_GREEN, false);
		gpio_writePin(GPIO_YELLOW, true);
		gpio_writePin(GPIO_RED, false);
		runInBackground("./rpi-face-server -e");
		g_server_running=true;
	}
}

// Ejecutado al pulsar el botón de cambio de emoción. 
// Cambia el estado emocional del servidor, si éste esá encendido
void onEmotionPressed() {
	if (!g_server_running) return;
	cout << "Cambiando estado emocional" << endl;
	toggle_emotion();
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
		va_end(ap);
		system(command); // ejecutar comando
		_Exit(0);
	} else {
		// proceso principal: vuelve inmediatamente
		return;
	}
}
