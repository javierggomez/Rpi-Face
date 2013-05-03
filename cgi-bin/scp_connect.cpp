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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "scp_connect.h"

#define SIZE 300

using namespace std;
// Se encarga de transferir un archivo mediante scp a una máquina remota
// Entradas: 
// - local: Fichero de la máquina local a transmitir
// - user: Usuario de la máquina remota al que conectarse
// - host: Máquina a la que conectarse
// - dir: Directorio de la máquina remota donde situar el archivo enviado
// - authFile: Archivo donde se encuentra la clave privada
// Salidas: 1 si el archivo se ha transmitido con éxito, 0 si no ha sido así
int scp_transfer(const char *local, const char *user, const char *host, const char *dir, const char *authFile){

	char command[SIZE];
	int n=sprintf (command, "scp %s %s@%s:%s", local,user, host, dir);
	if (authFile!=NULL) {
		sprintf(command+n," -i %s", authFile);
	}
	cerr << command << endl;
	return system(command);
}

// Se encarga de transferir un archivo mediante scp desde una máquina remota
// Entradas: 
// - local: Fichero de la máquina local a transmitir
// - user: Usuario de la máquina remota al que conectarse
// - host: Máquina a la que conectarse
// - dir: Directorio de la máquina remota donde situar el archivo enviado
// - authFile: Archivo donde se encuentra la clave privada
// Salidas: 1 si el archivo se ha transmitido con éxito, 0 si no ha sido así
int scp_receive(const char *user, const char *host, const char *dir, const char *local, const char *authFile){

	char command[SIZE];
	int n=sprintf (command, "scp %s@%s:%s %s", user, host, dir ,local);
	if (authFile!=NULL) {
		sprintf(command+n," -i %s", authFile);
	}
	cerr << command << endl;
	return system(command);
}