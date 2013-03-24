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
// Nombre de la variable asociada al mensaje en la QUERY_STRING
#define KEY_MESSAGE "message"
// Nombre de la variable asociada a comandos directos en la QUERY_STRING
#define KEY_FACE "face"
// Nombre del fichero donde irá el mensaje
#define FICHERO "data/fichero.raw"
// Nombre del fichero que actuará de semáforo
#define SEMAPHORE "data/semaphore.t3"
// Tiempo de espera para mover la cara tras inicializarla (us)
#define DELAY 100000
// Archivo donde se guarda la posición tras mover la cara
#define FILE_POSITION "data/savedata.mfc"
// Longitud de los datos de posición (8 caracteres)
#define POSITION_LENGTH 8

#define FILE_INDEX "/var/www/index.html"

// Programa que actúa de servidor web. Obtiene las variables de la QUERY_STRING.
// Si hay un mensaje, lo envía a t3. Si hay un comando directo, mueve la cara.
int main(int argc, char **argv, char **env) {
	
	int rendered=0;
	// Obtener método
	if (!strcmp(getenv("REQUEST_METHOD"), "GET")) {
		// Obtenemos QUERY_STRING y añadimos '&' al principio y al final
		char query[MAX_LENGTH+3]="&";
		strncat(query, getenv("QUERY_STRING"), MAX_LENGTH);
		int length=strlen(query);
		char *result=(char*)malloc(length); // array para almacenar valores de variables
		if (length>1) {
			strncat(query, "&", 1);
			if (getValue(result, query, KEY_FACE)) {
				// si hay comando directo, mover la cara
				int face=atoi(result);
				redirect("/", ""); // redirigir a la página principal
				setFace(face);
				rendered=1;
			} else if (getValue(result, query, KEY_MESSAGE)) {
				renderMessage(result); // mostrar mensaje en los campos
				rendered=1;
			}
		}
		free(result);
	} else if (!strcmp(getenv("REQUEST_METHOD"), "POST")) {
		// Obtenemos QUERY_STRING y añadimos '&' al principio y al final
		int length=1+atoi(getenv("CONTENT_LENGTH"));
		char *query=(char*)malloc(length+3);
		sprintf(query, "&");
		fgets(query+1, length, stdin);
		char *result=(char*)malloc(length); // array para almacenar valores de variables
		// si hay algo en la QUERY_STRING
		if (length>1) {
			strncat(query, "&", 1);
			if (getValue(result, query, KEY_MESSAGE)) {
				// si hay mensaje, procesarlo
				processMessage(result);
				query[strlen(query)-1]=0;
				// redirigir con GET para que no se reenvíe al recargar la página
				redirect("/", query+1);
				rendered=1;
			}
		}
		free(result);
		free(query);
	} 
	if (!rendered) {
		render(FILE_INDEX, "", "No se recibió ningún mensaje", NULL);
	}
	return 0;
}

// Procesa el mensaje: lo reproduce y mueve la cara
// Entradas: 
// - message: mensaje recibido.
void processMessage(char *message) {
	parseValue(message, message);
	// int length=strlen(message);
	// char *footer=(char*)malloc(length+25);
	// sprintf(footer, "<b>Mensaje recibido: </b>%s", message); // mostrarlo
	// render(FILE_INDEX, message, footer, NULL); // recargar el formulario
	lowerCase(message, message); // pasar a minúsculas y quitar caracteres extraños
	writeParsed(message); // escribirlo en el archivo de mensajes para pasárselo a t3
	FILE *semaphore;   
	semaphore = fopen(SEMAPHORE, "w"); //Se crea un semáforo para que t3 analice el fichero
	fclose(semaphore);
}

// Muestra la página con los campos rellenos con el mensaje
// Entradas:
// - message: mensaje a mostrar
void renderMessage(char *message) {
	parseValue(message, message); // decodificar caracteres especiales
	int length=strlen(message);
	char *footer=(char*)malloc(length+25);
	sprintf(footer, "<b>Mensaje recibido: </b>%s", message); // mostrarlo
	render(FILE_INDEX, message, footer, NULL); // recargar el formulario
	free(footer);
}

// Mueve la cara a una posición determinada
// Entradas:
// - face: indice de la posición en {FACE_HAPPY, FACE_SAD, FACE_SURPRISE, FACE_ANGRY, FACE_NEUTRAL}
void setFace(int face) {
	// Posiciones por defecto
	const unsigned char* POSITIONS[]={FACE_HAPPY, FACE_SAD, FACE_SURPRISE, FACE_ANGRY, FACE_NEUTRAL};
	// Mensajes asociados a las posiciones por defecto
	const char POSITIONS_MSGS[5][256]={"messages/FACE_HAPPY.txt", "messages/FACE_SAD.txt","messages/FACE_SURPRISE.txt","messages/FACE_ANGRY.txt","messages/FACE_NEUTRAL.txt",};
	if (face>=0&&face<5) {
		int fd=face_initialize();
		usleep(DELAY);
		face_setFace(fd, POSITIONS[face]);
		face_close(fd);
		speech_initialize();
		say_file(POSITIONS_MSGS[face]);
		speech_close();
		savePosition(POSITIONS[face], FILE_POSITION);
	}
}

// Obtiene el valor de una variable de URL y lo almacena en message. 
// Entradas:
// - message: cadena donde se almacenará el resultado; 
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

//Lee el fichero filename, sustituye cada cadena de <%ri%> por el argumento número i,
//aceptando un número variable de argumentos. Posteriormente se muestra el archivo.
//Entradas:
//	-filename: nombre del archivo a generar y mostrar.
//	-siguientes argumentos (opcionales): cadena de caracteres que sustituiran a <%ri%>. El último argumento
//	será NULL para indicar que se acaba la lista de argumentos.
void render(const char *filename, ...) {
	// Iniciar la salida HTML
	printf("Content-type:text/html\n\n");
	// Leer fichero y guardarlo en contents
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
	// Recorrer la lista de argumentos opcionales
	va_list ap;
	va_start(ap, filename);
	const char *arg;
	int i=1;
	char replace[16];
	while ((arg=va_arg(ap, const char*))!=NULL) {
		sprintf(replace, "<%%r%d%%>", i);
		// reemplazar "<%ri%>" con el argumento i
		pContents=string_replace(pContents, replace, arg);
		i++;
	}
	va_end(ap);
	printf("%s\n", pContents);
}

// Redirige al navegador a otra página.
// Entradas:
// - url: URL de la página (absoluta, relativa o relativa a la raíz del servidor)
// - query: QUERY_STRING a enviar con la URL.
void redirect(const char *url, const char *query) {
	printf("Status: 303 See Other\n");
	if (*query==0) {
		printf("Location: %s\n\n", url);
	} else {
		printf("Location: %s%c%s\n\n", url, '?', query);
	}
}

//Reemplaza en el contenido de la cadena string todas las coincidencias de replace con el 
//contenido de with sin modificar los string originales.
//Entradas:
//	-string: cadena original
//	-replace: patrón a encontrar y sustituir
//	-with: patrón que sustituirá a replace.
//Valor de retorno: cadena sustituida.
char *string_replace(const char *string, const char *replace, const char *with) {
	// Longitudes de las cadenas
	int sLength=strlen(string);
	int rLength=strlen(replace);
	int wLength=strlen(with);
	int count=0;
	// Primero, contar el número de ocurrencias de replace para calcular la longitud de
	// la nueva cadena
	const char *sPointer=string; // puntero para recorrer string
	const char *temp=string; // puntero auxiliar
	// Número de ocurrencias de replace en string
	while ((sPointer=strstr(sPointer, replace))!=NULL) {
		sPointer+=rLength;
		count++;
	}
	// Longitud destino
	int nLength=sLength+(wLength-rLength)*count;
	char *result=(char*)malloc(nLength+1); //reservar memoria
	char *pResult=result; // puntero para recorrer result
	sPointer=string;
	// Reemplazar las ocurrencias
	while ((temp=strstr(sPointer, replace))!=NULL) {
		// Para cada ocurrencia, copiar en result lo que hay en string antes de ella
		strncpy(pResult, sPointer, temp-sPointer);
		pResult+=(temp-sPointer);
		// y copiar el contenido de with después
		strcpy(pResult, with);
		pResult+=wLength;
		sPointer=temp+rLength; // poner sPointer al final de la ocurrencia
	}
	strcpy(pResult, sPointer); // copiar lo que queda tras la última ocurrencia
	return result;
}
