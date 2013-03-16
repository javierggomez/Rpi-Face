/*
  One-dimensional Arrays

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
#include <string.h>
#include "array.h"
#include "mem.h"

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
array_pt array_new(unsigned int size)
{
  array_pt a=(array_pt)mem_malloc(sizeof(array_t));

  a->size=size;
  a->count=0;
  a->v=(void **)mem_malloc(size*sizeof(void *));
  memset(a->v, 0, sizeof(void *)*size);
  return a;
}

/* ------------------------------------------------------------ */
void array_fill(array_pt a, void *p)
{
  int i;
  for (i=0; i<a->size; i++) { a->v[i]=p; }
}

/* ------------------------------------------------------------ */
array_pt array_new_fill(unsigned int size, void *p)
{
  array_pt a=array_new(size);

  a->count=size;
  array_fill(a, p);
  return a;
}

/* ------------------------------------------------------------ */
void array_free(array_pt a)
{
  mem_free(a->v);
  mem_free(a);
}

/* ------------------------------------------------------------ */
void array_clear(array_pt a)
{
  a->count=0;
}

/* ------------------------------------------------------------ */
array_pt array_clone(array_pt a)
{
  array_pt b=array_new(a->size);

  b->size=a->size;
  b->count=a->count;
  b->v=(void **)mem_malloc(b->size*sizeof(void *));
  (void)memcpy(b->v, a->v, b->size*sizeof(void *));
  return b;
}

/* ------------------------------------------------------------ */
unsigned int array_add(array_pt a, void *p)
{
  if (a->size<=a->count)
    {
      if (a->size<=0) { a->size=8; } else { a->size*=2; }
      a->v=(void **)mem_realloc(a->v, a->size*sizeof(void *));
    }
  a->v[a->count]=p;
  return a->count++;
}

/* ------------------------------------------------------------ */
unsigned int array_add_unique(array_pt a, void *p)
{
  unsigned int k;
  
  for (k=0; k<a->count; k++) { if (a->v[k]==p) { return k; } }
  return array_add(a, p);
}

/* ------------------------------------------------------------ */
void array_delete_item(array_pt a, void *p)
{
  int i, j, oldcount=a->count;
  
  for (i=0, j=0; i<oldcount; i++)
    {
      if (a->v[i]==p) { a->count--; }
      else { a->v[j]=a->v[i]; j++; }
    }
}

/* ------------------------------------------------------------ */
void array_delete_duplicates(array_pt a)
{
  int i, j, k, oldcount;
  
  for (k=0; k<a->count; k++)
    {
      oldcount=a->count;
      for (i=k+1, j=i; i<oldcount; i++)
	{
	  if (a->v[i]==a->v[k]) { a->count--; }
	  else { a->v[j]=a->v[i]; j++; }
	}
    }
}

/* ------------------------------------------------------------ */
void *array_delete_index(array_pt a, unsigned int no)
{
  void *p=a->v[no];
  int i;

  for (i=no+1; i<a->count; i++) { a->v[i-1]=a->v[i]; }
  a->count--;
  return p;
}

/* ------------------------------------------------------------ */
void array_trim(array_pt a)
{
  if (a->size>a->count)
    {
      a->size=a->count;
      a->v=(void **)mem_realloc(a->v, a->size*sizeof(void *));
    }
}

/* ------------------------------------------------------------ */
void array_filter_with(array_pt a, int (*func)(void *, void *), void *data)
{
  int i, j;
  int oldcount=a->count;
  
  for (i=0, j=0; i<oldcount; i++)
    {
      if (func(a->v[i], data)) { a->count--; }
      else { a->v[j]=a->v[i]; j++; }
    }
}

/* ------------------------------------------------------------ */
void array_filter(array_pt a, int (*func)(void *))
{
  int i, j;
  int oldcount=a->count;
  
  for (i=0, j=0; i<oldcount; i++)
    {
      if (func(a->v[i])) { a->count--; }
      else { a->v[j]=a->v[i]; j++; }
    }
}

/* ------------------------------------------------------------ */
void array_map(array_pt a, void (*func)(void *))
{
  int i;
  
  for (i=0; i<a->count; i++) { func(a->v[i]); }
}

/* ------------------------------------------------------------ */
void array_map1(array_pt a, void (*func)(void *, void *), void *p)
{
  int i;
  
  for (i=0; i<a->count; i++) { func(a->v[i], p); }
}

/* ------------------------------------------------------------ */
void array_map2(array_pt a, void (*func)(void *, void *, void *), void *p1, void *p2)
{
  int i;
  
  for (i=0; i<a->count; i++) { func(a->v[i], p1, p2); }
}

/* ------------------------------------------------------------ */
void *array_set(array_pt a, int i, void *p)
{
  void *old;
  if (a->size<=i)
    {
      if (a->size<=0) { a->size=8; }
      while (a->size<=i) { a->size*=2; }
      a->v=(void **)mem_realloc(a->v, a->size*sizeof(void *));
      memset(&a->v[a->count], 0, sizeof(void *)*(a->size-a->count));
    }
  old=a->v[i];
  a->v[i]=p;
  if (a->count<=i) { a->count=i+1; }
  return old;  
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */

/* previously inlined */
int array_size (array_pt a)
{
    return a->size;
}

/* previously inlined */
int array_count (array_pt a)
{
    return a->count;
}

/* previously inlined */
void *array_get (array_pt a, int i)
{
    return a->v[i];
}
