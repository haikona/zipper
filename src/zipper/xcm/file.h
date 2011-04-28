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
/***********************/
/* Input/Output Header */
/***********************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __FILE
#define __FILE

#include "complex.h"

#define MAX_LINE_LENGTH  256

/* Each file (containing pairs of points is read into a linked list,  */
/* each node of which contains an array (dynamically allocated) of    */
/* connected line segments.  The largest size of the array allowed is */
/* MAX_POINTS, so if a collection of connected segments exceeds this  */
/* value, it must be split into multiple nodes in the list, where the */
/* last point in one node is the first in the next.                   */

struct point_struct {
  int                   count;
  complex               *points;
  struct point_struct   *next;
};

typedef struct point_struct point_type;

point_type *parse_file();
void kill_file();
void show_file();
void file_bounds();
char file_frame();
complex *find_file_point();

#endif
