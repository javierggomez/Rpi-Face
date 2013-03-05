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

#include "face_controller.h"
#include "set_happy.h"


int main(int argc, char **argv, char **env) {
	int fd=face_initialize();
	// Iniciar la salida HTML
	printf("Content-type:text/html\n\n");
	printf("<html><head><title>Enviar mensaje</title></head><body>");
	printf("<h1>Enviar mensaje</h1>"
			"<form action=\"server_query.cgi\" method=\"GET\">"
			"<textarea name=\"message\" cols=40 rows=2></textarea>"
			"<br/><input type=\"submit\" value=\"Enviar\"/></form>");
	printf("<a href='set_happy.cgi' >Poner cara contenta</a>");
	printf("</body></html>");
	usleep(100000);
	face_setFace(fd, FACE_HAPPY);
	face_close(fd);
	return 0;
}

