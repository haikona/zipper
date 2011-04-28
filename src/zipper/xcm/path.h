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
/***********************************/
/* Header for Path Creation Module */
/***********************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __PATH
#define __PATH

#include "complex.h"

#define MAX_PATHS 100
#define PIXEL_TOL   4

#define MAX_COMMON_POINTS 100

typedef struct path_node_struct {
  complex point;      /* The coordinates of the point */
  
  complex *external;  /* the address of the point node */
                      /* of the point if the point is  */
                      /* not given by z = (x,y).       */

  struct path_node_struct *next;      /* the next point     */
  struct path_node_struct *prev;      /* the previous point */
} path_node;

/* a "graph" is a collection of "paths" */

enum graph_mode {no_graph = -1, g_inactive, g_creating, 
		 g_move_point, g_kill_point, g_kill_line, g_break_line,
		 g_break_point};

typedef struct graph_struct {
  path_node *paths[MAX_PATHS]; /* an array of the first nodes of the paths */
  int       path_count;        /* the total number of paths                */

  path_node *current_path;     /* The current "selected" path */
 
  path_node *path_end;         /* The endpoint of the current path */

  enum graph_mode  mode;
} graph_type;


path_node *new_path_node();
path_node *insert_path_node();
path_node *preinsert_path_node();
void free_path_node();
void kill_path_node();
void prekill_path_node();
complex path_node_point();
path_node *find_point();
char math_lineQ();
path_node *match_line();
path_node *find_graph_point();
double pixel_tolerance();
double prec_round();
char write_graph();
void draw_graph();
void handle_create_button();
void handle_create_motion();
void handle_motion();
void kill_path_point();
void kill_path_line();
void move_path_point();
void break_path_line();
void break_path_point();

/* This lies in function main */
complex *grab_point();
void grab_translate();

#endif
