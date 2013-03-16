/*
  Memory management

  Copyright (C) 2001 Ingo Schröder

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* ------------------------------------------------------------ */
#include <stdlib.h>
#include <stdio.h>
#include "mem.h"

/* ------------------------------------------------------------ */
void mem_free(void *p)
{
  free(p);
}

/* ------------------------------------------------------------ */
void *mem_malloc(unsigned int size)
{
  void *p=malloc(size);
  if (!p) { fprintf(stderr, "no memory left, abort\n"); exit(0); }
  return p;
}

/* ------------------------------------------------------------ */
void *mem_realloc(void *p, unsigned int size)
{
  p=realloc(p, size);
  if (!p) { fprintf(stderr, "no memory left, abort\n"); exit(0); }
  return p;
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
