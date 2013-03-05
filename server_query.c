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

#include "server_query.h"

#define MAX_LENGTH 65536
#define FORMAT "%c%s%c"
#define KEY_MESSAGE "message"
#define FICHERO "/var/www/cgi-bin/data/fichero.raw"
#define COMMAND "/var/www/cgi-bin/text_analyzer /var/www/cgi-bin/data/fichero.raw"

int main(int argc, char **argv, char **env) {
	// Iniciar la salida HTML
	printf("Content-type:text/html\n\n");
	printf("<html><head><title>Enviar mensaje</title></head><body>");
	
	// Obtenemos QUERY_STRING y añadimos ''&' al principio y al final
	char query[MAX_LENGTH+3]="&";
	strncat(query, getenv("QUERY_STRING"), MAX_LENGTH);
	int length=strlen(query);
	char *result=(char*)malloc(length);
	result[0]=0;
	if (length>1) {
		strncat(query, "&", 1);
		if (getValue(result, query, KEY_MESSAGE)) {
			// si la variable message existe, sustituir caracteres
			parseValue(result, result);
			
		} 
	}
	// Volver a mostrar el formulario
	printf("<h1>Enviar mensaje</h1>"
			"<form action=\"server_query.cgi\" method=\"GET\">"
			"<textarea name=\"message\" cols=40 rows=2>%s</textarea>"
			"<br/><input type=\"submit\" value=\"Enviar\"/></form>", result);
	printf("<a href='set_happy.cgi' >Poner cara contenta</a>");
	if ((*result)!=0) {
		printf("<p><b>Mensaje recibido: </b>%s</p>", result);
		lowerCase(result, result);
		writeParsed(result);
		system(COMMAND);
	} else {
		printf("<p>No se recibi&oacute; ning&uacute;n mensaje</p>");
	}
	printf("</body></html>");
	free(result);
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

// Pasa la cadena a minúsculas y elimina los caractéres especiales
void lowerCase(char *result, char *value) {
	char *pValue=value; // puntero para recorrer value
	char *pResult=result; // puntero para recorrer result
	char *end=value+strlen(value); // dónde acabar la conversión
	while (pValue<end) {
		*pResult=tolower((unsigned char)*pValue);
		if(*pValue==195) {
			switch (*(++pValue)) {
				case 145:
				case 177:
					*pResult=195;
					pResult[1]=177;
					pResult+=2;
					break;
				case 129:
				case 137:
				case 141:
				case 147:
				case 154:
					*pResult=195;
					pResult[1]=(*pValue)+32;
					pResult+=2;
					break;
			}
			
		} if((*pResult>=97 && *pResult <= 122) || (*pResult>=48 && *pResult <= 57) || (*pResult==' ')){//Minúsculas, números sin acentos
			pResult++;
		}
		pValue++;
			
	}
	*pResult=0; // terminar result con '\0'
	
}

//Escribe la cadena en un fichero

void writeParsed(char *result) {
	FILE *fichero;   
   	fichero = fopen(FICHERO, "w");
	char *s;
	char temp;
	for(s = result; *s; s++){
		temp=*s;
		if(temp==' '){
			temp='\n';
		}
		fputc(temp, fichero);		
	}
	fputc('\n', fichero);
	fclose(fichero);	
}


