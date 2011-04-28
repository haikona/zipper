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
/******************************/
/* Plane Window Object Header */
/******************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __XPLANE
#define __XPLANE

#define LINES_PER_DECADE  10
#define DECADE_LINE_STEP   1
#define DECADE_SWITCH      0.6

#define BOX_WIDTH  4
#define BOX_HEIGHT 4

#define MINIMUM_SCALE 1000*DBL_MIN
#define MAXIMUM_SCALE DBL_MAX/1000

#define EFFECTIVE_EPSILON 10*DBL_EPSILON

/*#define ZOOM_FACTOR 2.71828*/
#define ZOOM_FACTOR 10.0

#define KEY_ZOOM_FACTOR      1.10
#define KEY_FINE_ZOOM_FACTOR 1.01

#define ZOOM_BUTTON           Button3
#define ZOOM_BUTTON_MASK      Button3Mask
#define TRANSLATE_BUTTON      Button2
#define TRANSLATE_BUTTON_MASK Button2Mask
#define POSITION_BUTTON       Button1
#define POSITION_BUTTON_MASK  Button1Mask

struct plane_window_struct {
  Window    window;
  Window    parent;
  void      *parent_struct; 
  Pixmap    pixmap;
  Drawable  drawable;
  GC        *gc;
  int       width, height;
  int       x, y; 
  unsigned int border_width;

  double    aspect;
  double    center_x, center_y; 
  double    hw_ratio;
  double    scale;
  double    x0, y0;
  double    x1, y1;

  double    orig_x0, orig_y0;
  double    orig_x1, orig_y1;

  unsigned long bg;
  unsigned long bd;
  unsigned long grid_color;
  unsigned long subgrid_color;

  char      show_grid;
  int       log_decade;
  double    decade;
  double    gx0, gy0;
  int       lines_x, lines_y;
  char      subgrid;
  int       sub_xk0, sub_xk1;
  int       sub_yk0, sub_yk1;

  void      (*redraw)(struct plane_window_struct *);
  void      (*update_pointer)(struct plane_window_struct *);

  void      (*button1)(struct plane_window_struct *, void *);  /* XEvent */
  void      (*button2)(struct plane_window_struct *, void *);  /* XEvent */
  void      (*button3)(struct plane_window_struct *, void *);  /* XEvent */
};

typedef struct plane_window_struct plane_window;

int rounds();
char setup_plane_vars();
void open_plane_window();
void destroy_plane_window();
void redraw_plane_window();
void resize_plane_window();
void zoom_plane_window();
void translate_plane_window();
void plane_zoom();
void plane_translate();
void plane_shift();
void plane_show_pos();
void plane_show_world();
void plane_pos();
void handle_plane_event();
void p_world_to_view();
void p_view_to_world();
char p_origial_okay();
char p_origial_in_view();
void plane_draw_line();
void plane_draw_line2();
void plane_draw_lines();
void plane_clip_lines();
void plane_draw_point();
void plane_draw_points();
void plane_draw_rect();
void plane_draw_rects();

void fill_image();

#endif
