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

// Servidor que utiliza Freeling para analizar un mensaje y 
// obtener las raíces de las palabras
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cctype>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iconv.h>
#include <cstring>
#include <signal.h>

using namespace std;

#include "freeling.h"
#include "freeling/morfo/traces.h"

// Puerto por defecto del servidor
#define PORT 25513
// Longitud máxima del mensaje
#define MESSAGE_MAX_LENGTH 1024

void INThandler(int sig);
void processAnalysis(char *result, list<sentence> &ls);
std::wstring array2wstring(char *array);

int sockfd;
int g_port;

// Inicia un servidor Freeling que recibe mensajes y los devuelve 
// analizados. El análisis consiste en obtener las raíces de las palabras,
// para facilitar la posterior aplicación del modelo
// Opciones: 
// -p PORT: puerto en el que escucha el servidor (por defecto, 25513)
int main (int argc, char **argv) {
  g_port=PORT;
  char c;
  int temp;
  // Opciones de línea de comandos
  while ((c=getopt(argc, argv, "p:"))!=EOF) {
    switch (c) {
      case 'p':
        // puerto del servidor
        temp=atoi(optarg);
        if (temp>=0&&temp<65536) {
          g_port=temp;
        } else {
          wcerr << "freeling_server: error: El número de puerto debe ser un entero entre 0 y 65535." << endl;
          return 1;
        }
        break;
      case '?':
        // opción desconocida
          switch (optopt) {
            case 'p': 
            wcerr << "freeling_server: error: la opción '-" << optopt << "' requiere un argumento." << endl;
            break;
          default:
            if (isprint (optopt)) {
              wcerr << "freeling_server: error: opción desconocida:  '" << optopt << "'." << endl;
            } else {
              wcerr << "freeling_server: error: opción desconocida." << endl;
            }
          }
    }
  }
  wstring text;
  list<word> lw;
  list<sentence> ls;

  util::init_locale(L"default");

  // path de las librerías Freeling
  wstring ipath;
  ipath=L"/usr/local";

  wstring path=ipath+L"/share/freeling/es/";
  
  // crear analizadores: divisor de frases en palabras
  // y generador de objetos sentence a partir de objetos word
  tokenizer tk(path+L"tokenizer.dat"); 
  splitter sp(path+L"splitter.dat");
  
  // opciones de los analizadores
  maco_options opt(L"es");  
  opt.UserMap=false;
  opt.QuantitiesDetection = false; 
  opt.AffixAnalysis = true; opt.MultiwordsDetection = true; opt.NumbersDetection = false; 
  opt.PunctuationDetection = true; opt.DatesDetection = false; opt.QuantitiesDetection = false; 
  opt.DictionarySearch = true; opt.ProbabilityAssignment = true; opt.NERecognition = true;   
  opt.UserMapFile=L"";
  opt.LocutionsFile=path+L"locucions.dat"; opt.AffixFile=path+L"afixos.dat";
  opt.ProbabilityFile=path+L"probabilitats.dat"; opt.DictionaryFile=path+L"dicc.src";
  opt.NPdataFile=path+L"np.dat"; opt.PunctuationFile=path+L"../common/punct.dat"; 

  // crear analizador morfológico
  maco morfo(opt); 
  // etiquetador
  hmm_tagger tagger(L"es", path+L"tagger.dat", true, FORCE_TAGGER); 
  
  // crear servidor
int newsockfd, portno;
socklen_t clilen;
char buffer[MESSAGE_MAX_LENGTH];
struct sockaddr_in serv_addr, cli_addr;
int n;
// crear socket
sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) 
   perror("Error al abrir el socket");
bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(g_port);
// asociar socket al puerto
if (bind(sockfd, (struct sockaddr *) &serv_addr,
         sizeof(serv_addr)) < 0) 
         perror("Error al asociar");
// Hacer que el servidor empiece a escuchar
listen(sockfd,5);
clilen = sizeof(cli_addr);
int message_length;
char result[MESSAGE_MAX_LENGTH];
signal(SIGINT, INThandler);
// procesar peticiones
while (true) {
  wcerr << "freeling_server: Listo: " << endl;
  // aceptar conexión
  newsockfd = accept(sockfd, 
            (struct sockaddr *) &cli_addr, 
            &clilen);
  if (newsockfd < 0) 
     perror("Error al aceptar conexión");
  bzero(buffer,256);
  // leer mensaje
  n = read(newsockfd,buffer,MESSAGE_MAX_LENGTH-1);
  if (n < 0) perror("Error al leer del socket");
  message_length=strlen(buffer);
  // procesar mensaje: convertir en wstring UTF-8
  while (buffer[message_length-1]==' ' || buffer[message_length-1]=='\n' || buffer[message_length-1]=='\r') message_length--;
  buffer[message_length++]='.';
  buffer[message_length]=0;
  text=array2wstring(buffer);
  // separar en palabras
  lw=tk.tokenize(text);
  // construir frases
  ls=sp.split(lw, false);
  // análisis morfológico
  morfo.analyze(ls);
  // etiquetado
  tagger.analyze(ls);
  // obtener datos relevantes del análisis
  processAnalysis(result, ls);
  // devolver resultado
  if (write(newsockfd,result,1+strlen(result))<0) perror("Error al escribir en el socket");
  close(newsockfd);
}


}

// Manejador de la señal SIGINT (recibida si el usuario pulsa Ctrl-C). 
// Termina el bucle software y permite que se ejecute el código de 
// cierre del programa.
// Entradas:
// - sig: señal recibida
void INThandler(int sig) {
  signal(sig, SIG_IGN);
  close(sockfd);
  wcerr << "Servidor cerrado." << endl;  
  exit(0);
}

// Convierte un array de char en ISO-8859-15 a un wstring en UTF-8
// Entradas:
// - array: array en ISO-8859-15
// Valor de retorno: wstring en UTF-8
std::wstring array2wstring(char *array) {
  // pasar a utf-8
  size_t inlength=1+strlen(array);
  size_t outlength=2*inlength;
  char *outbuf=new char[outlength];
  char *pin=array;
  char *pout=outbuf;
  iconv_t cd=iconv_open("UTF-8","ISO-8859-15");
  iconv(cd, &pin, &inlength, &pout, &outlength);
  iconv_close(cd);
  // pasar a string
  std::string temp(outbuf);
  delete[] outbuf;
  // pasar a wstring
  return util::string2wstring(temp);
}

// Procesa el análisis: obtiene la raíz de cada palabra y las devuelve en un string
// separadas por espacios.
// Entradas:
// - result: cadena donde almacenar el resultado
// - ls: resultado del análisis de Freeling
void processAnalysis(char *result, list<sentence> &ls) {
  // iterar en la lista de frases, que son listas de palabras
  word::const_iterator a;
  sentence::const_iterator w;
  char *analysis=new char[MESSAGE_MAX_LENGTH];
  char *pResult=(char*)analysis;
  for (list<sentence>::iterator is=ls.begin(); is!=ls.end(); is++) {
    for (w=is->begin(); w!=is->end(); w++) {
      // obtener la raíz de cada palabra y añadirla al final del resultado
      pResult+=(sprintf(pResult, "%ls ", (w->get_lemma()).c_str()));
    }
  }
  // pasar a ISO-8859-15
  size_t length=strlen(analysis);
  if (analysis[length-1]==' ') analysis[length-1]=0;
  char *pin=analysis;
  char *pout=result;
  iconv_t cd=iconv_open("ISO-8859-15", "UTF-8");
  iconv(cd, &pin, &length, &pout, &length);
  iconv_close(cd);
  delete[] analysis;
}


