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

// Interfaz de acceso a los GPIO de la RPI.

#include <cstdio>

#include "hardware.h"

// Tiempo antirrebotes
#define DEBOUNCE_DELAY 8000
// Umbral antirrebotes
#define DEBOUNCE_THRESHOLD 8
// Máximo de comprobaciones antirrebotes
#define DEBOUNCE_MAX_CHECKS 100
// GPIO para el botón

using namespace std;

// Inicializa la librería BCM2835 de acceso a los GPIO
bool gpio_init() {
	return bcm2835_init()!=0;
}

// Cierra el acceso a los GPIO
void gpio_close() {
	bcm2835_close();
}

// Selecciona un GPIO como entrada o salida
// Entradas:
// - pin: puerto a seleccionar
// - in: true para entrada, false para salida
void gpio_fsel(uint8_t pin, bool in) {
	bcm2835_gpio_fsel(pin, in?BCM2835_GPIO_FSEL_INPT:BCM2835_GPIO_FSEL_OUTP);
}

// Escribe en un GPIO
// Entradas:
// - pin: puerto
// - on: true para escribir un 1, false para un 0
void gpio_writePin(uint8_t pin, bool on){
    bcm2835_gpio_write(pin, on?HIGH:LOW);
}

// Lee de un puerto
// Entradas:
// - pin: puerto para leer
// Valor de retorno: true si hay un 1, false si hay un 0
bool gpio_readPin(uint8_t pin){
	uint8_t value = bcm2835_gpio_lev(pin);
	return (value==HIGH);
}

// Antirrebotes software. Comprueba que el botón se mantenga pulsado durante
// más de DEBOUNCE_THRESHOLD*DEBOUNCE_DELAY microsegundos. Si esto ocurre, 
// devuelve true. Si se mantiene sin pulsar durante más de este tiempo, 
// devuelve false. Si después de DEBOUNCE_MAX_CHECKS*DEBOUNCE_DELAY microsegundos
// no ha ocurrido ninguna de las dos cosas, devuelve false.
// Entradas:
// - pin: puerto
// Valor de retorno: true si el botón se pulsó durante el tiempo umbral.
bool gpio_debounce(uint8_t pin) {
	int time_on=0; // tiempo seguido encendido
	int time_off=0; // tiempo seguido apagado
	int checks=0; // número de comprobaciones
	while (time_on<DEBOUNCE_THRESHOLD && time_off<DEBOUNCE_THRESHOLD && (++checks)<DEBOUNCE_MAX_CHECKS) {
		bcm2835_delayMicroseconds(DEBOUNCE_DELAY);
		if (bcm2835_gpio_lev(pin)==HIGH) {
			time_on++;
			time_off=0;
		} else {
			time_off++;
			time_on=0;
		}
	}
	return (time_on>=DEBOUNCE_THRESHOLD);
}

// Bloquea ejecución hasta que el puerto esté a 0 durante 
// DEBOUNCE_DELAY*DEBOUNCE_THRESHOLD microsegundos.
// Entradas:
// - pin: puerto
void gpio_waitForRelease(uint8_t pin) {
	int time_off=0;
	while (time_off<DEBOUNCE_THRESHOLD) {
		bcm2835_delayMicroseconds(DEBOUNCE_DELAY);
		if (bcm2835_gpio_lev(pin)==HIGH) time_off=0;
		else time_off++;
	}
}
