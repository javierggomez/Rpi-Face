//    Copyright (C) 2013 Javier García, Julio Alberto González

//    This file is part of Rpi-Face.
//    Rpi-Face is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    Foobar is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


#ifndef RPI_UART
#define RPI_UART
int uart_initialize(int read, int write, int flags);
void uart_close(int fd);
void uart_setBaudRate(int fd, int baudRate);
void uart_setBlockForInput(int fd, int nChars);
int uart_read(int fd, char *buffer, int nChars);
int uart_write(int fd, char *string, int nChars);

#endif
