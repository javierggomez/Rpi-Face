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
#include <fstream>
#include <unistd.h>

#include "toggle_emotion.h"

// Archivo en el que se recibe el comando en formato XML
#define FILE_COMMAND "data/command.am"
// Archivo que actúa de semáforo
#define FILE_SEMAPHORE "data/semaphore.am"
// Tiempo de espera entre comprobaciones
#define DELAY 100000
// Etiqueta XML para cambiar estado emocional
#define KEY_EMOTION "emotion"

using namespace std;

// Cambia el estado emocional del servidor
// Valor de retorno: 1 si la operación tuvo éxito, 0 si no.
int toggle_emotion() {
	while (!access(FILE_SEMAPHORE, F_OK)) usleep(DELAY);
	ofstream out(FILE_COMMAND, ios::out);
	if (!out.is_open()){
		perror("toggle_emotion: error:");
		return 0;
	}
	out << "<" << KEY_EMOTION << ">" << "</" << KEY_EMOTION << ">" <<endl;
	out.close();
	out.open(FILE_SEMAPHORE, ios::out);
	out.close();
	return 1;
}