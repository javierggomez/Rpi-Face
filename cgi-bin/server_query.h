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

#ifndef SERVER_QUERY
#define SERVER_QUERY
void processMessage(char *message);
void renderMessage(char *message);
void setFace(int face);
int getValue(char *result, char *query, const char *key);
void parseValue(char *result, char *value);
void lowerCase(char *result, char *value);
unsigned char lowerChar(unsigned char c);
void writeParsed(char *result);
int savePosition(const unsigned char *position, const char *filename);
void render(const char *filename, ...);
void redirect(const char *url, const char *query);
char *string_replace(const char *string, const char *replace, const char *with);
#endif
