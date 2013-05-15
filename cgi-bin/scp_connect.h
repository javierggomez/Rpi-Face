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

#ifndef SCP_CONNECT
#define SCP_CONNEC
int scp_transfer(const char *local, const char *user, const char *host, const char *dir, const char *authFile=0);
int scp_receive(const char *user, const char *host, const char *dir, const char *local, const char *authFile=0);
int ssh_command(const char *user, const char *host, const char *format, ...);
int run_system_command(const char *format, ...);
#endif