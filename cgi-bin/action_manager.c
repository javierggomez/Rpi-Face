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

// Servidor de análisis y acciones. Consiste en un bucle software que se
// mantiene a la espera de comandos recibidos del servidor y analiza los 
// mensajes que llegan o ejecuta los comandos que recibe (movimiento de 
// la cabeza o reproducción de textos).

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>

#include "action_manager.h"
#include "speech_synthesis.h"
#include "face_controller.h"
#include "position.h"

// Archivo en el que se recibe el comando en formato XML
#define FILE_COMMAND "data/command.am"
// Archivo que actúa de semáforo
#define FILE_SEMAPHORE "data/semaphore.am"
// Archivo donde se escribe el mensaje para poder analizarlo
#define FILE_MESSAGE "data/message.am"
// Etiqueta XML para el mensaje
#define KEY_MESSAGE "message"
// Etiqueta XML para la posición
#define KEY_POSITION "position"
// Etiqueta XML para el texto a reproducir
#define KEY_SPEECH "speech"
// Longitud del array que define una posición
#define POSITION_LENGTH 8
// Tiempo entre comprobaciones
#define LOOP_DELAY 100000
// Tolerancia de cambio de estado
#define MOOD_TOLERANCE 16
// Relación máxima entre la valoración positiva y la negativa para
// movar la cabeza a su posición máxima. Debe ser un float.
#define OK_KO_RATIO 2.0
// Puerto predeterminado del servidor positivo
#define PORT_OK 31416
// Puerto predeterminado del servidor negativo
#define PORT_KO 27183
// Comando para el analizador positivo
#define ANALYZER_OK_COMMAND "ngram -use-server %d -ppl %s"
// Comando para el analizador negativo
#define ANALYZER_KO_COMMAND "ngram -use-server %d -ppl %s"
// Etiqueta a buscar en la respuesta del analizador
#define ANALYZER_LABEL "ppl= "
// Posición correspondiente a la nota máxima
#define POSITION_MAX FACE_HAPPY
// Posición correspondiente a la nota mínima
#define POSITION_MIN FACE_SAD
// Peso de la posición anterior por defecto
#define POSITION_WEIGHT 0.5
// Mensaje cuando mejora el estado de ánimo
#define FILE_HAPPIER "messages/TEXT_HAPPIER.txt"
// Mensaje cuando empeora el estado de ánimo
#define FILE_SADDER "messages/TEXT_SADDER.txt"
// Mensaje cuando el cambio es pequeño
#define FILE_NOCHANGE "messages/TEXT_NOCHANGE.txt"

int go_on; // variable para mantener ejecución del bucle
int g_port_ok=PORT_OK; // puerto del servidor positivo
int g_port_ko=PORT_KO; // puerto del servidor negativo
void INThandler(int sig);

// Programa principal. Inicia el bucle software.
// Entradas: argumentos de línea de comandos
// Opciones:
// -o PORT_OK: define el puerto del servidor positivo
// -k PORT_KO: define el puerto del servidor negativo
// Valor de retorno: 0 si todo fue bien.
int main(int argc, char **argv) {
	go_on=1;
	// Instalar manejador de la señal SIGINT
	signal(SIGINT, INThandler);
	char c;
	int temp;
	// Opciones de línea de comandos
	while ((c=getopt(argc, argv, "o:k:"))!=255) {
		switch (c) {
			case 'o':
				if(sscanf(optarg, "%d", &temp)&&temp>=0&&temp<65536) {
					g_port_ok=temp; // definir puerto positivo
				} else {
					fprintf(stderr, "%s\n", "action_manager: error: el puerto debe ser un número entero entre 0 y 65535");
				}
				break;
			case 'k':
				if(sscanf(optarg, "%d", &temp)&&temp>=0&&temp<65536) {
					g_port_ko=temp; // definir puerto negativo
				} else {
					fprintf(stderr, "%s\n", "action_manager: error: el puerto debe ser un número entero entre 0 y 65535");
				}
				break;
			case '?':
				// opción desconocida
		    	switch (optopt) {
		    		case 'o': case 'k': 
			    	fprintf (stderr, "action_manager: error: la opción '-%c' requiere un argumento.\n", optopt);
				    break;
			    default:
			    	if (isprint (optopt)) {
	 	        	fprintf (stderr, "action_manager: error: opción desconocida:  '-%c'.\n", optopt);
			    	} else {
			        fprintf (stderr, "action_manager: error: opción desconocida.");
			    	}
			    }
		}
	}
	// Iniciar Festival
	speech_initialize();
	// inicio del bucle. Se ejecuta hasta que se recibe la señal SIGINT
	// (Ctrl-C)
	while (go_on) {
		if (!access(FILE_SEMAPHORE, F_OK)) {
			// Cuando aparezca el semáforo, procesar comando
			processCommand(FILE_COMMAND);
			unlink(FILE_SEMAPHORE);
		}
		usleep(LOOP_DELAY);
	}
	// Cerrar Festival
	speech_close();
	return 0;
}

// Manejador de la señal SIGINT (recibida si el usuario pulsa Ctrl-C). 
// Termina el bucle software y permite que se ejecute el código de 
// cierre del programa.
// Entradas:
// - sig: señal recibida
void INThandler(int sig) {
	signal(sig, SIG_IGN);
	go_on=0;
}

// Procesa un comando recibido
// Entradas:
// - filename: archivo donde llega el comando.
void processCommand(const char *filename) {
	FILE *file=fopen(filename, "r");
	char c;
	int i=0;
	// obtener longitud del comando
	fseek(file, 0, SEEK_END);
	int length=ftell(file);
	rewind(file);
	char *command=(char*)malloc(length+5);
	// leer el comando tal cual en el array command
	while ((c=fgetc(file))!=255) {
		command[i++]=c;
		if (i>(length+2)) {
			fprintf(stderr, "Error: comando demasiado largo\n");
			return;
		}
	}
	command[i]=0;
	fclose(file);
	// procesar comando según las variables que haya
	char value[length];
	if (getValue(value, KEY_MESSAGE, command)) {
		processMessage(value);
	}
	if (getValue(value, KEY_POSITION, command)) {
		processPosition(value);
	}
	if (getValue(value, KEY_SPEECH, command)) {
		processSpeech(value);
	}
	free(command);
}

// Obtiene lo que hay dentro de una etiqueta XML en un string.
// Es decir, dado un string del tipo "*<key>value</key>*", obtiene
// value a partir de key. Si no encuentra la etiqueta en el string, 
// devuelve 0 y no modifica value.
// Entradas:
// - value: array donde se almacenará el resultado (lo que hay dentro
//    de la etiqueta)
// - key: nombre de la etiqueta
// - string: cadena a procesar, donde se encuentra la etiqueta
// Valor de retorno: 1 si se encontró la etiqueta, 0 si no.
int getValue(char *value, const char* key, const char *string) {
	int keyLength=strlen(key);
	const char *pString=string;
	const char *pAux;
	char *temp=(char *)malloc((keyLength+5)*sizeof(char));
	// buscar inicio de la etiqueta
	sprintf(temp, "<%s>", key);
	if ((pString=strstr(string, temp))==NULL) {
		// si no se encuentra, devolver 0
		free(temp);
		return 0; 
	}
	// sumar longitud de la etiqueta (longitud de "<key>"")
	pString+=(keyLength+2);
	// buscar fin de etiqueta
	sprintf(temp, "</%s>", key);
	if ((pAux=strstr(string, temp))==NULL) {
		fprintf(stderr, "Error: comando mal formado\n");
		free(temp);
		return 0;
	}
	// copiar lo que hay entre <key> y </key> en value
	strncpy(value, pString, pAux-pString);
	value[pAux-pString]=0;
	free(temp);
	return 1;
}


// Procesa un mensaje recibido. Lo analiza, mueve la cabeza y reproduce
// el mensaje y el estado de ánimo.
// Entradas:
// - message: mensaje recibido
void processMessage(const char *message) {
	fprintf(stderr, "%s\n", message); // debug
	writeFile(FILE_MESSAGE, message);
	int length=strlen(FILE_MESSAGE);
	// formar comando de análisis positivo
	char *analyzerCommand=(char*)malloc(length+max(strlen(ANALYZER_OK_COMMAND), strlen(ANALYZER_KO_COMMAND))+5);
	sprintf(analyzerCommand, ANALYZER_OK_COMMAND, g_port_ok, FILE_MESSAGE);
	// abrir un pipe con la ejecución del comando
	FILE *pipe_ok=popen(analyzerCommand, "r");
	float ppl_ok, ppl_ko;
	char line[256];
	char *aux;
	int labelLength=strlen(ANALYZER_LABEL);
	// obtener perplejidad en ppl_ok
	while (fgets(line, 255, pipe_ok)) {
		if ((aux=strstr(line, ANALYZER_LABEL))!=NULL) {
			sscanf(aux+labelLength, "%f", &ppl_ok);
		}
	}
	pclose(pipe_ok);
	// formar comando de análisis negativo
	sprintf(analyzerCommand, ANALYZER_KO_COMMAND, g_port_ko, FILE_MESSAGE);
	// abrir un pipe con la ejecución del comando
	FILE *pipe_ko=popen(analyzerCommand, "r");
	// obtener la perplejidad en ppl_ko
	while (fgets(line, 255, pipe_ko)) {
		if ((aux=strstr(line, ANALYZER_LABEL))!=NULL) {
			sscanf(aux+labelLength, "%f", &ppl_ko);
		}
	}
	pclose(pipe_ko);
	free(analyzerCommand);
	unsigned char position[8];
	// calcular posición correspondiente a la valoración
	// del mensaje
	calculatePosition(position, ppl_ok, ppl_ko);
	int change=setPosition(position, POSITION_WEIGHT);
	say_file(FILE_MESSAGE); // reproducir mensaje
	// reproducir mensaje según el cambio de ánimo
	if (change>MOOD_TOLERANCE) say_file(FILE_HAPPIER);
	else if (change<-MOOD_TOLERANCE) say_file(FILE_SADDER);
	else say_file(FILE_NOCHANGE);
}

// Ejecuta un comando de colocar posición directa
// Entradas:
// - value: posición expresada como un string con 8 números
//    entre 0 y 255 separados por espacios, y opcionalmente 
//    un noveno número real entre 0 y 1 que indica el peso
//    de la posición anterior (por defecto, 0).
void processPosition(const char *value) {
	fprintf(stderr, "%s\n", value);  // debug
	const char *pValue=value;
	int nRead;
	unsigned char position[POSITION_LENGTH];
	float weight;
	// leer posición
	for (int i=0;i<POSITION_LENGTH;i++) {
		sscanf(pValue, "%hhu %n", position+i, &nRead);
		pValue+=nRead;
	}
	// colocar posición
	if (sscanf(pValue, "%f", &weight)){
		setPosition(position, weight);
	} else {
		setPosition(position);
	}
}
// Procesa un comando de reproducir texto
// Entradas:
// - filename: nombre del archivo con el texto a reproducir
void processSpeech(const char *filename) {
	say_file(filename);
}

// Escribe el contenido de un string en un archivo
// Entradas:
// - filename: nombre de archivo
// - string: cadena de caracteres a escribir en el archivo
// Valor de retorno: 1 si la operación tiene éxito; 0 si no.
int writeFile(const char *filename, const char *string) {
	FILE *file=fopen(filename, "w");
	if (file==NULL) return 0;
	const char *pString=string;
	while ((*pString)) {
		fputc(*(pString++), file);
	}
	fclose(file);
	return 1;
}

// Devuelve el máximo de dos enteros
// Entradas:
// - a: entero 1
// - b: entero 2
// Valor de retorno: máximo entre a y b
int max(int a, int b) {
	return (a<b?b:a);
}

// Calcula la posición correspondiente a los valores de perplejidad
// positiva y negativa obtenidos. El procedimiento es una interpolación 
// logarítmica entre la posición mínima y la máxima según el valor
// de ppl_ko/ppl_ok entre OK_KO_RATIO y 1/OK_KO_RATIO
// Entradas:
// - result: array donde se almacenará el resultado
// - ppl_ok: perplejidad positiva
// - ppl_ko: perplejidad negativa
void calculatePosition(unsigned char *result, float ppl_ok, float ppl_ko) {
	if (ppl_ko<1E-12) ppl_ko=1E-12; // evitar divisiones por cero
	float ratio=ppl_ko/ppl_ok;
	// saturar entre OK_KO_RATIO y 1/OK_KO_RATIO
	if (ratio>OK_KO_RATIO) ratio=OK_KO_RATIO;
	else if (ratio<(1/OK_KO_RATIO)) ratio=1/OK_KO_RATIO;
	// interpolación logarítmica
	float tag=log(ratio*OK_KO_RATIO)/(2*log(OK_KO_RATIO));
	fprintf(stderr, "Tag: %f\n", tag); // debug
	for (int i=0;i<POSITION_LENGTH;i++) {
		result[i]=(unsigned char)((float)POSITION_MIN[i]+(float)(POSITION_MAX[i]-POSITION_MIN[i])*tag);
	}
}