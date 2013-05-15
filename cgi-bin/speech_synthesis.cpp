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
#include <cstdio>
#include <cstdarg>
#include "speech_synthesis.h"

#define COMMAND_LENGTH 2048

// Inicializa la librería Festival con los parámetros por defecto
// Hay que llamar a esta función antes de llamar a cualquier otra
// función de síntesis de texto.
void speech_initialize() {
	static int init=0;
	if (init) return;
	festival_initialize(true, 210000);
	init=1;
}
// Sintetiza y reproduce en español el mensaje contenido en un archivo de texto
// Entradas:
// - file: nombre del archivo
void say_file(const char *file) {
	festival_eval_command("(voice_el_diphone)"); // voz en español
	festival_say_file(file);
	//festival_wait_for_spooler(); // bloquear ejecución hasta que acabe de decirlo
}

// Sintetiza y reproduce en español el mensaje pasado como argumento
// Entradas:
// - text: texto a sintetizar
void say_text(const char *text) {
	festival_eval_command("(voice_el_diphone)"); // voz en español
	festival_say_text(text);
	//festival_wait_for_spooler(); // bloquear ejecución hasta que acabe de decirlo
}

// Genera las etiquetas de fonemas asociadas a un texto utilizando 
// Festival. Devuelve un archivo de etiquetas fonéticas y un archivo de segmentos
// Entradas: 
// - file_segs: Archivo donde devolver los segmentos
// - file_all: Archivo donde devolver las etiquetas
// - text: Texto a analizar
// - say: (Opcional) Si es true, se reproduce el fichero tras generar las etiquetas
void generate_labels(const char *name, const char *filename, const char *text, bool say) {
	// festival_eval_command("(voice_el_diphone)"); // voz en español
	// run_command("(set! utt1 (Utterance Text \"%s\"))", text); // crear Utterance
	// // run_command("(utt.synth utt1)"); // generar etiquetas
	// // 
	// // run_command("(utt.save.segs utt1 \"%s\")", file_segs); // guardar segmentos

	// // (voice_uvigo3_hts)
	// // (set! utt1 (Utterance Text "Hola Robin, qué tal.")");)");
	// run_command("(Initialize utt1)");
	// run_command("(Text utt1)");
	// run_command("(Token_POS utt1)");
	// run_command("(Token utt1)");
	// run_command("(POS utt1)");
	// run_command("(Phrasify utt1)");
	// run_command("(Word utt1)");
	// run_command("(Pauses utt1)");
	// run_command("(Intonation utt1)");
	// run_command("(PostLex utt1)");
	// run_command("(utt.save utt1 \"%s\")", file_all); // guardar todo
	// // run_command("(quit)

	// if (say) run_command("(utt.play utt1)"); // opcionalmente, reproducir
	FILE* file=fopen(filename, "w");
	fprintf(file, "( %s \"%s\" )\n", name, text);
	fclose(file);

	char command[256];
	sprintf(command, "./parser/generate_labels.sh %s", filename);
	system(command);
	if (say) say_text(text);
}

// Bloquea la ejecución hasta que se acabe de decir todo. Importante llamar a esto al
// final del programa.
void speech_close() {
	festival_wait_for_spooler();
}

// Ejecuta un comando Festival después de formatearlo al estilo de 
// printf.
// Entradas:
// - format: formato del comando (igual que en printf)
// - ...: (Opcional): variables para sustituir en el formato.
void run_command(const char *format, ...) {
	char *command=new char[COMMAND_LENGTH];
	va_list ap;
	va_start(ap, format);
	vsprintf(command, format, ap);
	va_end(ap);
	festival_eval_command(command);
	delete[] command;
}