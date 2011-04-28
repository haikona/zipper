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
/****************************************/
/* Header for the Primary Window Module */
/****************************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __PRIMARY
#define __PRIMARY

#include "button.h"
#include "path.h"

/* Declaraction for the primary window */
#define AXES_LABEL_X_OFFSET 2
#define AXES_LABEL_Y_OFFSET 5
#define X_LABEL_BORDER      8

#define MAX_DIGITS          16

#define POSITION_Y          10

struct primary_struct {
  Window                      primary;
  Drawable                    drawable;
  GC                          gc;
  GC                          plane_gc;
  GC                          button_gc;
  int                         width, height;
  int                         pos_width;

  struct plane_window_struct  plane;

  button_type                 grid_button;
  button_type                 lines_button;

  XFontStruct                 *current_font;
  
  graph_type  graph;
};

typedef struct primary_struct primary_window;

void open_primary();
XPoint w_primary_to_plane();
void label_grid();
void handle_primary_event();

#endif
