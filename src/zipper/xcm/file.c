/*
    Copyright (C) 1993  University of Washington, U.S.A.

    Author:   Michael Stark

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/**************************************/
/* Module for Input/Output Operations */
/**************************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module contains the C functions for the input and   */
/*                 output operations, except the PostScript functions.      */
/****************************************************************************/

#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "globals.h"
#include "complex.h"
#include "file.h"

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

int CurrentLine;


char blank_lineQ( source )
     char source[];
  /* POST: True if source contains only whitespace charaters, False */
  /*       otherwise.                                               */
{
  char *ptr;
  
  ptr = source;
  while ( (*ptr != '\0') && (isspace(*ptr)) )
    ptr++;

  return( *ptr == '\0');
}
  
point_type *new_node()
  /* POST: Allocates a new variable of type point_type, and returns a      */
  /*       pointer thereto.  If unsuccesfull, an error message is printed  */
  /*       to stderr, and exit(1) is called.                               */
  /*       The 'next' field of the new node is initialized to NULL.        */
{
  point_type *ptr;

  if ( (ptr = (point_type*) malloc(sizeof(point_type))) == NULL) {
    fprintf(stderr, "Insufficient memory.\n");
    exit(1);
  }
  ptr->next = NULL;
  ptr->count = 0;
  return(ptr);
}


complex *new_point_array( count )
     int count;
  /* POST: Tries to allocate a dynamic array of 'count' cells of type   */
  /*       complex type, and returns a pointer to the first cell of the */
  /*       array if successful.  Otherwise an error message is printed  */
  /*       and the program exits with code 1.                           */
{
  complex *ptr;

  if ((ptr = (complex*) calloc(count, sizeof(complex))) == NULL) {
    fprintf(stderr, "Insufficient memory (probably too many points).\n");
    exit(1);
  }
  return(ptr);
}

char next_line( line, stream )
     char line[];
     FILE *stream;
  /* POST: Reads the next line from stream, and increments           */
  /*       The global variable CurrentLine (for error messages).     */
  /*       Returns -1 if EOF(stream), 0 if a blank line is read, and */
  /*       1 if the next line is not blank.                          */
{
  if (feof(stream))
    return(-1);

  /* Assign 'line' to a blank line */
  line[0] = '\0';
  fgets(line, MAX_LINE_LENGTH, stream);
  CurrentLine++;

  if (blank_lineQ(line))
    return(0);
  else
    return(1);
}

void parse_point( point, line )
     complex *point;
     char line[];
  /* POST : Scans 'line' for two double-precision constants, and assigns  */
  /*        them to 'point'.  If unsuccesful, an error message is printed */
  /*        to stderr, and exit(1) is called.                             */
{
  double x, y;

  if (sscanf(line, "%lf%lf", &x, &y) != 2) {
    fprintf(stderr, "Error reading values at line %d\n%s\n",
	    CurrentLine, line);
    exit(1);
  }
  point->x = x;
  point->y = y;
}

char parse_points0( line, obj, stream )
     char       line[];
     point_type *obj;
     FILE       *stream;
  /* PRE : 'stream' is open for reading text, 'obj' is assigned.      */
  /* POST: Parses a sequence of at most MAX_POINTS lines from stream, */
  /*       each containing a pair of floating-point constants.        */
  /*       'line' is the first line in the sequence, and upon         */
  /*       completion is the last line parsed.                        */
  /*       The point values are copied into 'obj->lines', and         */
  /*       'obj->count' is assigned accordingly.                      */
  /*       Returns True if a blank line is read before MAX_POINTS is  */
  /*       attained.                                                  */
{
  static complex  buffer[MAX_POINTS];
  int             k = 0;

  parse_point(buffer, line);
  k++;

  while( (k < MAX_POINTS) && (next_line(line, stream) == 1) ) {
    parse_point(buffer+k, line);
    k++;
  }
  obj->points = new_point_array(k);
  memcpy(obj->points, buffer, k*sizeof(complex));
  obj->count = k;

  return(blank_lineQ(line));
}

struct point_struct *parse_connected_points( obj, stream )
     point_type *obj;
     FILE       *stream;
  /* PRE : 'obj' is allocated, 'stream' is open for reading text.    */
  /* POST: Reads a sequence of non-blank lines of text from 'stream' */
  /*       and places into 'obj->points'.  If the sequence exceeds   */
  /*       MAX_POINTS, new nodes are created.                        */
  /*       Returns a pointer to the terminal node of the list.       */
{
  static char line[MAX_LINE_LENGTH];
  int         pieces = 0;
  point_type  *ptr;

  ptr = obj;

  /* skip any leading blank lines */
  while ( next_line(line, stream) == 0 );
  if (feof(stream))
    return(ptr);

  /* Parse each contiguous chunk (of size MAX_POINTS) */
  while (!feof(stream) && (!parse_points0(line, ptr, stream))) {
    ptr->next = new_node();
    ptr = ptr->next;
    pieces++;
  }

  ptr->next = new_node();
  ptr = ptr->next;

  return(ptr);
}
	     
point_type *parse_file( name )
     char name[];
  /* POST: opens 'name' for reading, and reads the points into a linked   */
  /*       list of point_type, each node containing a sequence of         */
  /*       connected line segments.  A pointer to the first node is      */
  /*       returned.  If 'name' is unavailable for reading as a text     */
  /*       file, or some line of 'name' is neither a pair of floating-   */
  /*       point numbers or blank, an error message is printed to stderr */
  /*       and the program is exited with code 1.                        */
{
  FILE                 *source_file;
  point_type           *ptr, *first;
  static char          line[MAX_LINE_LENGTH];
  int                  k;

  /* Open the file */
  if ( (source_file = fopen(name, "r")) == NULL ) {
    fprintf(stderr, "Unable to open file %s.\n", name);
    exit(1);
  }
  CurrentLine = 0;

  /* Allocate the first node in the list */
  ptr = new_node();
  first = ptr;

  /* Now parse the file */
  while (!feof(source_file)) 
    ptr = parse_connected_points(ptr, source_file);

  return(first);
}

void kill_file( obj )
     point_type *obj;
  /* POST: Frees each node of 'obj'.  */
{
  point_type *ptr, *next;

  ptr = obj;
  while (ptr != NULL) {
    next = ptr->next;
    free(ptr);
    ptr = next;
  }
}

void show_file( obj )
     point_type *obj;
{
  int k;
  point_type *ptr;

  for (ptr = obj; ptr != NULL; ptr = ptr->next ) {
    if (ptr->count <= 0) 
      printf("Empty node! (%d)\n", ptr->count);
    else {
      for (k = 0; k < ptr->count; k++) 
	printf("%d: %f %f\n", k, ptr->points[k].x, ptr->points[k].y);
      puts(" ");
    }
  }
}

/***********************************************************/
/* Functions to determine the natural boundaries of a file */
/***********************************************************/

void ctd_bounds( base, count, min, max )
     complex base[];
     int     count;
     complex *min, *max; /* Return values */
  /* POST: 'min' and 'max' contains the lower-left and upper-right   */
  /*       corners of a rectangle bounding 'count' points in 'base'. */
{
  int k;

  *min = base[0];
  *max = base[0];

  for (k = 1; k < count; k++) {
    min->x = MIN(min->x, base[k].x);
    min->y = MIN(min->y, base[k].y);

    max->x = MAX(max->x, base[k].x);
    max->y = MAX(max->y, base[k].y);
  }
}

void file_bounds( source, min, max )
     point_type *source;
     complex    *min, *max;  /* Return values */
  /* PRE : 'source' references a valid node of points.                    */
  /* POST: 'min' and 'max' contain the lower-left and upper-right corners */
  /*       of a rectangle bounding all the points in the linked-list      */
  /*       starting at 'source.                                           */
{
  complex local_min, local_max;
  point_type *ptr;

  ctd_bounds(source->points, source->count, min, max);
  for (ptr = source->next; ptr != NULL; ptr = ptr->next) {
    if (ptr->count > 0) {
      ctd_bounds(ptr->points, ptr->count, &local_min, &local_max);
      
      min->x = MIN(min->x, local_min.x);
      min->y = MIN(min->y, local_min.y);
      
      max->x = MAX(max->x, local_max.x);
      max->y = MAX(max->y, local_max.y);
    }
  }
}

char file_frame( source, LL, UR )
     point_type *source;
     complex    *LL, *UR;  /* Return values */
  /* PRE : 'source' is a valid list.                                    */
  /* POST: Determines the lower-left (LL) and upper-right (UR) corners  */
  /*       of the bounds of a lists of point nodes, including the       */
  /*       border given by INNER_BORDER_FACTOR.  The return value is    */
  /*       false if either of the coordinates are the same.             */
{
  complex Middle;
  double  width, height;

  file_bounds(source, LL, UR);
  
  /* Calculate the dimensions and the middle of the window */
  Middle.x = (LL->x + UR->x)/2;
  Middle.y = (LL->y + UR->y)/2;
  width = -(LL->x - UR->x);
  height = -(LL->y - UR->y);
  
  /* Now rescale the corners so a visible border appears */
  LL->x = Middle.x - width*INNER_BORDER_FACTOR;
  UR->x = Middle.x + width*INNER_BORDER_FACTOR;
  
  LL->y = Middle.y - height*INNER_BORDER_FACTOR;
  UR->y = Middle.y + height*INNER_BORDER_FACTOR;

  return( (LL->x != UR->x) && (LL->y != UR->y) );
}


/**********************************/
/* Functions to Search for Points */
/**********************************/

complex *find_f_point( source, z, tol )
     point_type source;
     complex    z;
     double     tol;
  /* POST: Searches through the array of 'source' for a point within 'tol' */
  /*       in both coordinates of 'z', and returns a pointer to the first  */
  /*       such point found, or NULL if none is found.                     */
{
  int k;

  for (k = 0; k < source.count; k++) 
    if ( ( z.x-tol <= source.points[k].x) && (source.points[k].x <= z.x+tol) &&
	 ( z.y-tol <= source.points[k].y) && (source.points[k].y <= z.y+tol) )
      return(source.points + k);
  return(NULL);
}

complex *find_file_point( source, z, tol )
     point_type *source;
     complex    z;
     double     tol;
  /* POST: Searches through all the points in 'source' for the first point  */
  /*       which lies within 'tol' of 'z', in both coordinates, and returns */
  /*       a pointer to the first match, or NULL if none is found.          */
{
  complex    *value;
  point_type *ptr;
  
  for (ptr = source; ptr != NULL; ptr = ptr->next)
    if ( (value = find_f_point(*ptr, z, tol)) != NULL)
      return(value);
  return(NULL);
}
