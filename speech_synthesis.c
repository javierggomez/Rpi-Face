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

#include <festival.h>
#include <stdio.h>
#include "speech_synthesis.h"

// Sintetiza y reproduce en español el mensaje contenido en un archivo de texto
// Entradas:
// - file: nombre del archivo
void say_file(const char *file) {
	festival_initialize(true, 210000);
	festival_eval_command("(voice_el_diphone)"); // voz en español
	festival_say_file(file);
	festival_wait_for_spooler(); // bloquear ejecución hasta que acabe de decirlo
}

// Sintetiza y reproduce en español el mensaje pasado como argumento
// Entradas:
// - text: texto a sintetizar
void say_text(const char *text) {
	festival_initialize(true, 210000);
	festival_eval_command("(voice_el_diphone)"); // voz en español
	festival_say_text(text);
	festival_wait_for_spooler(); // bloquear ejecución hasta que acabe de decirlo
}