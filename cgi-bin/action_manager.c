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

// Archivo en el que se recibe el comando
#define FILE_COMMAND "data/command.am"
// Archivo que actúa de semáforo
#define FILE_SEMAPHORE "data/semaphore.am"
#define FILE_MESSAGE "data/message.am"
#define KEY_MESSAGE "message"
#define KEY_POSITION "position"
#define KEY_SPEECH "speech"
#define POSITION_LENGTH 8
#define LOOP_DELAY 100000
// Tolerancia de cambio de estado
#define MOOD_TOLERANCE 16
#define OK_KO_RATIO 2
#define PORT_OK 31416
#define PORT_KO 27183
#define ANALYZER_OK_COMMAND "ngram -use-server %d -ppl %s"
#define ANALYZER_KO_COMMAND "ngram -use-server %d -ppl %s"
#define ANALYZER_LABEL "ppl= "
// Posición correspondiente a la nota máxima
#define POSITION_MAX FACE_HAPPY
// Posición correspondiente a la nota mínima
#define POSITION_MIN FACE_SAD
#define POSITION_WEIGHT 0.5
// Mensaje cuando mejora el estado de ánimo
#define FILE_HAPPIER "messages/TEXT_HAPPIER.txt"
// Mensaje cuando empeora el estado de ánimo
#define FILE_SADDER "messages/TEXT_SADDER.txt"
// Mensaje cuando el cambio es pequeño
#define FILE_NOCHANGE "messages/TEXT_NOCHANGE.txt"

int go_on;
int g_port_ok=PORT_OK;
int g_port_ko=PORT_KO;
void INThandler(int sig);
int main(int argc, char **argv) {
	go_on=1;
	signal(SIGINT, INThandler);
	char c;
	int temp;
	while ((c=getopt(argc, argv, "o:k:"))!=255) {
		switch (c) {
			case 'o':
				if(sscanf(optarg, "%d", &temp)&&temp>=0&&temp<65536) {
					g_port_ok=temp;
				} else {
					fprintf(stderr, "%s\n", "action_manager: error: el puerto debe ser un número entero entre 0 y 65535");
				}
				break;
			case 'k':
				if(sscanf(optarg, "%d", &temp)&&temp>=0&&temp<65536) {
					g_port_ko=temp;
				} else {
					fprintf(stderr, "%s\n", "action_manager: error: el puerto debe ser un número entero entre 0 y 65535");
				}
				break;
			case '?':
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
	speech_initialize();
	while (go_on) {
		if (!access(FILE_SEMAPHORE, F_OK)) {
			processCommand(FILE_COMMAND);
			unlink(FILE_SEMAPHORE);
		}
		usleep(LOOP_DELAY);
	}
	speech_close();
	return 0;
}

void INThandler(int sig) {
	signal(sig, SIG_IGN);
	go_on=0;
}

void processCommand(const char *filename) {
	FILE *file=fopen(filename, "r");
	char c;
	int i=0;
	fseek(file, 0, SEEK_END);
	int length=ftell(file);
	rewind(file);
	char *command=(char*)malloc(length+5);
	while ((c=fgetc(file))!=255) {
		command[i++]=c;
		if (i>(length+2)) {
			fprintf(stderr, "Error: comando demasiado largo\n");
			return;
		}
	}
	command[i]=0;
	fclose(file);
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

int getValue(char *value, const char* key, const char *string) {
	int keyLength=strlen(key);
	const char *pString=string;
	const char *pAux;
	char *temp=(char *)malloc((keyLength+5)*sizeof(char));
	sprintf(temp, "<%s>", key);
	if ((pString=strstr(string, temp))==NULL) {
		free(temp);
		return 0;
	}
	pString+=(keyLength+2);
	sprintf(temp, "</%s>", key);
	if ((pAux=strstr(string, temp))==NULL) {
		fprintf(stderr, "Error: comando mal formado\n");
		free(temp);
		return 0;
	}
	strncpy(value, pString, pAux-pString);
	value[pAux-pString]=0;
	free(temp);
	return 1;
}

void processMessage(const char *message) {
	fprintf(stderr, "%s\n", message);
	writeFile(FILE_MESSAGE, message);
	int length=strlen(FILE_MESSAGE);
	char *analyzerCommand=(char*)malloc(length+max(strlen(ANALYZER_OK_COMMAND), strlen(ANALYZER_KO_COMMAND))+5);
	sprintf(analyzerCommand, ANALYZER_OK_COMMAND, g_port_ok, FILE_MESSAGE);
	FILE *pipe_ok=popen(analyzerCommand, "r");
	float ppl_ok, ppl_ko;
	char line[256];
	char *aux;
	int labelLength=strlen(ANALYZER_LABEL);
	while (fgets(line, 255, pipe_ok)) {
		if ((aux=strstr(line, ANALYZER_LABEL))!=NULL) {
			sscanf(aux+labelLength, "%f", &ppl_ok);
		}
	}
	pclose(pipe_ok);
	sprintf(analyzerCommand, ANALYZER_KO_COMMAND, g_port_ko, FILE_MESSAGE);
	FILE *pipe_ko=popen(analyzerCommand, "r");
	while (fgets(line, 255, pipe_ko)) {
		if ((aux=strstr(line, ANALYZER_LABEL))!=NULL) {
			sscanf(aux+labelLength, "%f", &ppl_ko);
		}
	}
	pclose(pipe_ko);
	free(analyzerCommand);
	unsigned char position[8];
	calculatePosition(position, ppl_ok, ppl_ko);
	int change=setPosition(position, POSITION_WEIGHT);
	say_file(FILE_MESSAGE);
	if (change>MOOD_TOLERANCE) say_file(FILE_HAPPIER);
	else if (change<-MOOD_TOLERANCE) say_file(FILE_SADDER);
	else say_file(FILE_NOCHANGE);
}

void processPosition(const char *value) {
	fprintf(stderr, "%s\n", value);
	const char *pValue=value;
	int nRead;
	unsigned char position[POSITION_LENGTH];
	float weight;
	for (int i=0;i<POSITION_LENGTH;i++) {
		sscanf(pValue, "%hhu %n", position+i, &nRead);
		pValue+=nRead;
	}
	if (sscanf(pValue, "%f", &weight)){
		setPosition(position, weight);
	} else {
		setPosition(position);
	}
}

void processSpeech(const char *filename) {
	say_file(filename);
}

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

int max(int a, int b) {
	return (a<b?b:a);
}

void calculatePosition(unsigned char *result, float ppl_ok, float ppl_ko) {
	if (ppl_ko<1E-12) ppl_ko=1E-12;
	float ratio=ppl_ko/ppl_ok;
	if (ratio>OK_KO_RATIO) ratio=OK_KO_RATIO;
	else if (ratio<(1/OK_KO_RATIO)) ratio=1/OK_KO_RATIO;
	float tag=log(ratio*OK_KO_RATIO)/(2*log(OK_KO_RATIO));
	fprintf(stderr, "Tag: %f\n", tag);
	// interpolación lineal para cada elemento del array
	for (int i=0;i<POSITION_LENGTH;i++) {
		result[i]=(unsigned char)((float)POSITION_MIN[i]+(float)(POSITION_MAX[i]-POSITION_MIN[i])*tag);
	}
}