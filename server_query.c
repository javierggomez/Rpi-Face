// Programa para procesar los parámetros de una URL (CGI
// método GET).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 65536
#define FORMAT "%c%s%c"
#define KEY_MESSAGE "message"
#define FICHERO fichero.raw

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
	if ((*result)!=0) {
		printf("<p><b>Mensaje recibido: </b>%s</p>", result);
		lowerCase(result, result);
		writeParsed(result);
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
		*pValue = tolower((unsigned char)*pValue);
		if(*pValue>96 && *pValue < 123 || *pValue>47 && *pValue < 58){
			*pResult=*pValue;
			pValue++;
			pResult++;
		}

	}
	*pResult=0; // terminar result con '\0'
	
}

//Escribe la cadena en un fichero

void writeParsed(char *result) {
	FILE *fichero;   
   	fichero = fopen(FICHERO, "w");
	unsigned char *s;
	for(s = cadena; *s; s++){
		*s = tolower((unsigned char)*s);
		if(s[0]=="32"){
			s[0]="13";
		}
		fputc(s, fichero);		
	}
	fclose(fichero);	
}


