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

// Programa para procesar los parámetros de una URL (CGI
// método GET).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdarg.h>

#include "server_query.h"
#include "face_controller.h"
#include "speech_synthesis.h"

// Longitud máxima de una QUERY_STRING
#define MAX_LENGTH 65536
// Formato para añadir un carácter antes y despues de una cadena
#define FORMAT "%c%s%c"
// Nombre de la variable asociada al mensaje en la QUERY_STRING
#define KEY_MESSAGE "message"
// Nombre de la variable asociada a comandos directos en la QUERY_STRING
#define KEY_FACE "face"
// Nombre del fichero donde irá el mensaje
#define FICHERO "data/fichero.raw"
// Nombre del fichero que actuará de semáforo
#define SEMAPHORE "data/semaphore.t3"
// Tiempo de espera para mover la cara tras inicializarla
#define DELAY 100000

#define FILE_POSITION "data/savedata.mfc"
// Longitud de los datos de posición (8 caracteres)
#define POSITION_LENGTH 8

#define FILE_INDEX "/var/www/index.html"

// Programa que actúa de servidor web. Obtiene las variables de la QUERY_STRING.
// Si hay un mensaje, lo envía a t3. Si hay un comando directo, mueve la cara.
int main(int argc, char **argv, char **env) {
	// Posiciones por defecto
	const unsigned char* POSITIONS[]={FACE_HAPPY, FACE_SAD, FACE_SURPRISE, FACE_ANGRY, FACE_NEUTRAL};
	// Mensajes asociados a las posiciones por defecto
	const char POSITIONS_MSGS[5][256]={"messages/FACE_HAPPY.txt", "messages/FACE_SAD.txt","messages/FACE_SURPRISE.txt","messages/FACE_ANGRY.txt","messages/FACE_NEUTRAL.txt",};
	// Iniciar la salida HTML
	printf("Content-type:text/html\n\n");
	// printf("<!Doctype html>\n\n");
	// printf("<html><head><meta charset='utf-8' /><title>Rpi-face</title><link rel='STYLESHEET' type='text/css' href='/style/style.css'></link></head><body>");
	
	
	// printf("<div class='center'><h1>Bienvenido a Rpi-face</h1><div class='text'>");
	// printf("Bienvenido a Rpi-face. Desde aquí podrás interactuar con el sistema. Por un lado dispones de la sección enviar mensajes, desde la cual le podrás enviar un mensaje al robot, y por otro lado podrás moverle la cara a la posición deseada");
	// printf("</div>");

	// Obtenemos QUERY_STRING y añadimos ''&' al principio y al final
	char query[MAX_LENGTH+3]="&";
	strncat(query, getenv("QUERY_STRING"), MAX_LENGTH);
	int length=strlen(query);
	char *result=(char*)malloc(length); // array para almacenar valores de variables
	char *message=(char*)malloc(length); // array para almacenar el mensaje
	result[0]=0;
	message[0]=0;
	// si hay algo en la QUERY_STRING
	if (length>1) {
		strncat(query, "&", 1);
		if (getValue(result, query, KEY_MESSAGE)) {
			// si la variable message existe, sustituir caracteres
			parseValue(result, result);
			strcpy(message, result);
			
		} else if (getValue(result, query, KEY_FACE)) {
			// si hay comando directo, mover la cara
			int face=atoi(result);
			if (face>=0&&face<5) {
				int fd=face_initialize();
				usleep(DELAY);
				face_setFace(fd, POSITIONS[face]);
				face_close(fd);
				say_file(POSITIONS_MSGS[face]);
				savePosition(POSITIONS[face], FILE_POSITION);

			}
		}
	}
	// Volver a mostrar el formulario
	// printf("<h1>Enviar mensaje</h1><div id='form'><span class='ext'>Texto a enviar: </span>"
	// 		"<form action=\"/cgi-bin/server_query.cgi\" method=\"GET\">"
	// 		"<textarea name=\"message\" cols=40 rows=2>%s</textarea>"
	// 		"<br/><input class='input' type=\"submit\" value=\"Enviar\"/></form>", message);
	// printf("<h1>Mover cara</h1><div id='actions'><ul class='menu'></li>");
	// printf("<li><a href='/cgi-bin/server_query.cgi?face=0' >Poner cara contenta</a><br/></li>");
	// printf("<li><a href='/cgi-bin/server_query.cgi?face=1' >Poner cara triste</a><br/></li>");
	// printf("<li><a href='/cgi-bin/server_query.cgi?face=2' >Poner cara sorprendida</a><br/></li>");
	// printf("<li><a href='/cgi-bin/server_query.cgi?face=3' >Poner cara enfadada</a><br/></li>");
	// printf("<li><a href='/cgi-bin/server_query.cgi?face=4' >Poner cara neutral</a><br/></li></ul></div>");

	if ((*message)!=0) {
		// Si hubo mensaje
		char *footer=(char*)malloc(length+25);
		sprintf(footer, "<b>Mensaje recibido: </b>%s", message); // mostrarlo
		render(FILE_INDEX, message, footer, NULL);
		lowerCase(message, message); // pasar a minúsculas y quitar caracteres extraños
		writeParsed(message); // escribirlo en el archivo de mensajes para pasárselo a t3
		FILE *semaphore;   
   		semaphore = fopen(SEMAPHORE, "w"); //Se crea un semáforo para que t3 analice el fichero
		fclose(semaphore);
		free(footer);
	} else {
		// printf("<p>No se recibi&oacute; ning&uacute;n mensaje</p>");
		render(FILE_INDEX, "", "No se recibió ningún mensaje", NULL);
	}
	// printf("</div></body></html>");
	free(result);
	free(message);
	return 0;
}

// Obtiene el valor de una variable de URL y lo almacena en result. 
// Entradas:
// - result: cadena donde se almacenará el resultado; 
// - query: QUERY_STRING de la petición, precedida y seguida por
// 	        el carácter '&'; key, nombre de la variable a obtener. 
// Valor de retorno: 1 si se encontró la variable, 0 si no. 
int getValue(char *result, char *query, const char *key) {
	// Buscamos el nombre de la variable precedido de '&' y seguido de '='
	int length=strlen(key);
	char *mod_key=(char*)malloc(length+3);
	sprintf(mod_key, "%c%s%c", '&', key, '=');
	char *position=strstr(query, mod_key);
	if ((position!=NULL)) {
		// si lo encontramos, obtenemos el valor, que empieza
		// en *(position+length+2) y acaba en el siguiente '&'
		sscanf(position+length+2, "%[^&]&", result);
		free(mod_key);
		return 1;
	}
	else {
		free(mod_key);
		return 0;
	}
}

// Sustituye los caracteres de control de value por sus equivalentes,
// y almacena el resultado en result. En particular, sustituye los 
// caracteres '+' por ' ', y las cadenas '%XX' por el carácter
// equivalente ASCII de 0xXX. result debe tener al menos el mismo
// espacio que la longitud de value. result y value pueden ser iguales,
// en cuyo caso el resultado se almacena sobreescribiendo value.
// Entradas:
// - result: array donde se almacenará el resultado
// - value: cadena a analizar. Puede ser la misma zona de memoria
//          que result, en cuyo caso el resultado se almacena
//          sobreescribiendo value
void parseValue(char *result, char *value) {
	unsigned int hexCode; // aquí se guardaran los códigos hexadecimales
	char *pValue=value; // puntero para recorrer value
	char *pResult=result; // puntero para recorrer result
	char *end=value+strlen(value); // dónde acabar la conversión
	while (pValue<end) {
		switch (*pValue) {
			case '+':
				*pResult=' ';
				pValue++;
				break;
			case '%':
				// obtener el valor hexadecimal de 2 cifras y convertirlo en un carácter
				sscanf(pValue+1, "%2x", &hexCode);
				*pResult=hexCode;
				pValue+=3;
				break;
			default:
				*pResult=*pValue;
				pValue++;
				break;
		}
		pResult++;
	}
	*pResult=0; // terminar result con '\0'
	
}

// Pasa la cadena a minúsculas y elimina los caractéres especiales
// Entradas:
// - result: array donde se almacenará el resultado
// - value: cadena a analizar. Puede ser la misma zona de memoria
//          que result, en cuyo caso el resultado se almacena
//          sobreescribiendo value
void lowerCase(char *result, char *value) {
	char *pValue=value; // puntero para recorrer value
	char *pResult=result; // puntero para recorrer result
	char *end=value+strlen(value); // dónde acabar la conversión
	while (pValue<end) {
		*pResult=lowerChar((unsigned char)*pValue);
		if((*pResult>=97 && *pResult <= 122) || (*pResult>=48 && *pResult <= 57) || (*pResult==' ') || (*pResult==225) || (*pResult==233) || (*pResult==237) || (*pResult==243) || (*pResult==250) || (*pResult==241)){//Minúsculas, números sin acentos
			pResult++;
		}
		pValue++;
			
	}
	*pResult=0; // terminar result con '\0'
	
}

// Pasa un carácter (ISO-8859-15) a minúsculas, incluyendo 'Ñ' y letras acentuadas
// Entradas:
// - c: carácter a transformar
// Valor de retorno: carácter transformado
unsigned char lowerChar(unsigned char c) {
	switch (c) {
		case 193:
		case 201:
		case 205:
		case 211:
		case 218:
		case 209:
			return c+32;
			break;
		default:
			return tolower(c);
	}
} 

// Escribe la cadena en el fichero de mensajes, sustituyendo
// espacios por saltos de línea para que t3 los entienda.
// Entradas:
// -result: cadena a escribir
void writeParsed(char *result) {
	FILE *fichero;   
   	fichero = fopen(FICHERO, "w"); // abrir fichero
	char *s;
	char temp;
	// Recorrer la cadena y escribir en el fichero
	for(s = result; *s; s++){
		temp=*s;
		if(temp==' '){
			temp='\n';
		}
		fputc(temp, fichero);		
	}
	fputc('\n', fichero); // salto de línea al final para que t3 lo entienda
	fclose(fichero);	
}

// Guarda la posición en el fichero
// Entradas:
// - position: posición a guardar
// - filename: nombre del fichero
// Valor de retorno: 0 si hay error, 1 si no
int savePosition(const unsigned char *position, const char *filename) {
	FILE *file=fopen(filename, "w"); // abrir o crear archivo
	if (file==NULL) {
		perror("server_query: No se pudo guardar la posición: ");
		return 0;
	}
	for (int i=0;i<POSITION_LENGTH;i++) {
		// Escribir 8 números entre 0 y 255
		if(fprintf(file, "%hhu ", position[i])<=0) {
			perror("server_query: No se pudo guardar la posición: ");
			fclose(file);
			return 0;
		}
	} 
	fclose(file);
	return 1;
}

void render(const char *filename, ...) {
	FILE *file=fopen(filename, "r");
	char contents[65536];
	char *pContents=(char*)contents;
	int c;
	while ((c=fgetc(file))!=EOF) {
		*(pContents++)=(char)c;
	}
	*pContents=0;
	pContents=(char*)contents;
	fclose(file);
	if (strlen(contents)==0) return;
	va_list ap;
	va_start(ap, filename);
	const char *arg;
	int i=1;
	char replace[16];
	while ((arg=va_arg(ap, const char*))!=NULL) {
		sprintf(replace, "<%%r%d%%>", i);
		pContents=string_replace(pContents, replace, arg);
		i++;
	}
	va_end(ap);
	printf("%s\n", pContents);
	fprintf(stderr, "%s\n", pContents);
}

char *string_replace(const char *string, const char *replace, const char *with) {
	int sLength=strlen(string);
	int rLength=strlen(replace);
	int wLength=strlen(with);
	int count=0;
	const char *sPointer=string;
	const char *temp=string;
	while ((sPointer=strstr(sPointer, replace))!=NULL) {
		sPointer+=rLength;
		count++;
	}
	int nLength=sLength+(wLength-rLength)*count;
	char *result=(char*)malloc(nLength+1);
	char *pResult=result;
	sPointer=string;
	while ((temp=strstr(sPointer, replace))!=NULL) {
		strncpy(pResult, sPointer, temp-sPointer);
		pResult+=(temp-sPointer);
		strcpy(pResult, with);
		pResult+=wLength;
		sPointer=temp+rLength;
	}
	strcpy(pResult, sPointer);
	return result;
}
