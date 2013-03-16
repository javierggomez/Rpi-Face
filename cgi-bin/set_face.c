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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "face_controller.h"
#include "set_face.h"
#include "server_query.h" //MIRAR COMO SE HACE PARA QUE PARSE_VALUE UFUNCIONE AQUI

#define KEY_FACE "face"
#define MAX_LENGTH 65536

int main(int argc, char **argv, char **env) {
	int fd=face_initialize();
	// Iniciar la salida HTML
	printf("Content-type:text/html\n\n");
	printf("<html><head><title>Enviar mensaje</title></head><body>");
	printf("<h1>Enviar mensaje</h1>"
			"<form action=\"server_query.cgi\" method=\"GET\">"
			"<textarea name=\"FACE\" cols=40 rows=2></textarea>"
			"<br/><input type=\"submit\" value=\"Enviar\"/></form>");
	printf("<a href='set_face.cgi?face=0' >Poner cara contenta</a><br/>");
	printf("<a href='set_face.cgi?face=1' >Poner cara triste</a><br/>");
	printf("<a href='set_face.cgi?face=2' >Poner cara enfadada</a><br/>");
	printf("<a href='set_face.cgi?face=3' >Poner cara sorprendida</a><br/>");
	printf("<a href='set_face.cgi?face=4' >Poner cara neutral</a><br/>");
	

	char query[MAX_LENGTH+3]="&";
	strncat(query, getenv("QUERY_STRING"), MAX_LENGTH);
	int length=strlen(query);
	char *result=(char*)malloc(length);
	result[0]=0;
	if (length>1) {
		strncat(query, "&", 1);
		if (getValue(result, query, KEY_FACE)) {
			// si la variable FACE existe, sustituir caracteres
			parseValue(result, result);
			
		} 
	}

	const unsigned char *positions[5]={FACE_HAPPY, FACE_SAD, FACE_SURPRISE, FACE_ANGRY, FACE_NEUTRAL};
	char * pEnd;
	long int i = strtol (result,&pEnd,10);
  	if(i>=0 && i<5){
  		usleep(100000);
		face_setFace(fd, positions[i]);
		face_close(fd);
	}else{
		printf("<h1>Error en el comando</h1>");
	}
	printf("</body></html>");
	return 0;
}






// Obtiene el valor de una variable de URL y lo almacena en result. 
// Argumentos: result: cadena donde se almacenará el resultado; 
// query: QUERY_STRING de la petición, precedida y seguida por
// el carácter '&'; key, nombre de la variable a obtener. 
// Devuelve: 1 si se encontró la variable, 0 si no. 
int getValue(char *result, const char *query, const char *key) {
	// Buscamos el nombre de la variable precedido de '&' y seguido de '='
	int length=strlen(key);
	char *mod_key=malloc(length+3);
	sprintf(mod_key, "%c%s%c", '&', key, '=');
	char *position=strstr(query, mod_key);
	if ((position!=NULL)) {
		// si lo encontramos, obtenemos el valor, que empieza
		// en *(position+length+2) y acaba en el siguiente '&'
		sscanf(position+length+2, "%[^&]&*65536s", result);
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
