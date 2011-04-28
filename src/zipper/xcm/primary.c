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
/*********************************************/
/* Module for Operating the 'primary' Window */
/*********************************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module has the jobs of creating and operating the   */
/*                 primary window, and its various subwindows.              */
/*                 'xplane.c' does most of the work for operating the plane */
/*                 subwindow, but this module is responsible for labeling   */
/*                 the plane grid, and reporting the position of the        */
/*                 pointer.                                                 */
/****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "globals.h"
#include "complex.h"
#include "basic.h"
#include "xplane.h"
#include "path.h"
#include "primary.h"

/* Globals this file requires */

extern unsigned int PrimaryWidth;
extern unsigned int PrimaryHeight;
extern int          PrimaryX;
extern int          PrimaryY;

extern int          LeftMargin;
extern int          RightMargin;
extern int          TopMargin;
extern int          BottomMargin;

/* correction by D. Marshall 5-95 */
/* extern unsigned long SizeHints;*/
extern XSizeHints SizeHints;

extern unsigned int PrimaryBorderWidth;
extern unsigned int PlaneBorderWidth;

extern unsigned long PrimaryFG;
extern unsigned long PrimaryBG;
extern unsigned long PrimaryBD;

extern unsigned long PlaneBG;
extern unsigned long PlaneBD;
extern unsigned long GridColor;
extern unsigned long SubgridColor;

extern double ScreenAspect;
/*Marshall:*/
extern int ShowGrid;

void open_primary( obj, x0, y0, x1, y1 )
     primary_window *obj;
     double         x0, y0;
     double         x1, y1;
  /* NOTE: y0 and y1 may be modified to ensure unit aspect ratio.  In any */
  /*       case, the arithmetic mean of 'y0' and 'y1' will be preserved.  */
{
  XSetWindowAttributes attr;
  XGCValues            gc_values;
  unsigned long        mask = 0;
  
  /* Set up the attributes for the new window */
  attr.border_pixel = PrimaryBD;
  attr.background_pixel = PrimaryBG;

  /* The mask indicates those attributes that have been specified. */
  mask = (CWBackPixel | CWBorderPixel);

  /* Now create the window (but this won't make it appear on the screen.) */
  obj->primary = XCreateWindow(display,  RootWindow(display, screen),
			       PrimaryX, PrimaryY, PrimaryWidth, PrimaryHeight,
			       PrimaryBorderWidth,
			       depth, InputOutput, CopyFromParent,
			       mask, &attr );
  
  /* Send the hints to the window manager. */
  XSetNormalHints( display, obj->primary, &SizeHints);

  /* Give the window a name (from p. 48, AXAP) */
  XStoreName(display, obj->primary, NAME);

  /* Now map the window to the screen, i.e. make it appear. */
  XMapRaised(display, obj->primary);
  
  obj->drawable = obj->primary;
  mask = GCForeground | GCBackground;
  gc_values.foreground = PrimaryFG;
  gc_values.foreground = PrimaryBG;
  obj->gc = XCreateGC(display, obj->primary, mask, &gc_values);
  obj->pos_width = 0;

  mask = 0;
  obj->plane_gc = XCreateGC(display, obj->primary, mask, &gc_values);
  open_plane_window(&(obj->plane), obj->primary, &obj->plane_gc,
		    LeftMargin, TopMargin,
		    PrimaryWidth - LeftMargin - RightMargin,
		    PrimaryHeight - TopMargin - BottomMargin,
		    x0, y0, x1, y1, ScreenAspect,
		    PlaneBorderWidth, PlaneBD,
		    PlaneBG, GridColor, SubgridColor,ShowGrid);
  obj->plane.parent_struct = obj;

  mask = StructureNotifyMask | ButtonPressMask | PointerMotionMask |
    ExposureMask | PointerMotionHintMask;
  XSelectInput(display, obj->primary, mask);

  /* The Buttons */
  mask = 0;
  obj->button_gc = XCreateGC(display, obj->primary, mask, &gc_values);
/*  create_button(&(obj->grid_button), obj->primary, "Hide Grid",
		&(obj->gc), ButtonFont, 720, 20, 70, 20,*/

  /* Setup the thing so that it's in new point mode */
/*  obj->graph.path_count = 0;*/
  obj->graph.current_path = NULL;
  obj->graph.path_end = NULL;
  set_graph_mode(obj, g_inactive);
		
  XFlush(display);
}

void destroy_primary( obj )
     primary_window *obj;
{
  destroy_plane_window(&(obj->plane));
  XDestroyWindow(display, obj->primary);
  XFreeGC(display, obj->gc);
}
  

void resize_primary( obj, width, height )
     primary_window *obj;
  /* POST : changes the parameters of 'obj' to accomodate a resizing */
  /*        event.  This function does not affect the physical size. */
{
  obj->width = width;
  obj->height = height;

  /* Now change the parameters of the plane, which has just been unmapped. */
  resize_plane_window(&(obj->plane),
		      width - RightMargin - LeftMargin,
		      height - TopMargin - BottomMargin);
}

XPoint w_primary_to_plane( obj, x, y )
     primary_window *obj;
     int            x, y;
  /* POST: returns the window coordinates of (x,y) releative to the  */
  /*       plane window.  The return value is suitable for use in    */
  /*       the function 'p_view_to_world".                           */
{
  XPoint value;

  value.x = x - obj->plane.border_width - obj->plane.x;
  value.y = y - obj->plane.border_width - obj->plane.y;

  return(value);
}

XPoint plane_to_primary( obj, x, y )
     plane_window *obj;
     double         x, y;
  /* POST: Returns the window coordinates of the world coordinates (x,y)  */
  /*       with respect to the *parent* window of 'obj'.                  */
  /*       This functions is primarily for use to calculate the positions */
  /*       of the labels on the axes.                                     */
{
  XPoint  p;
  complex z;
  
  z.x = x;
  z.y = y;

  p_world_to_view(obj, z, &p);
  p.x += obj->x;
  p.y += obj->y;

  return(p);
}

void to_string( string, value, precision )
     char   string[];
     double value;
     int    precision;
  /* POST: writes 'value' into 'string', with the precision given by */
  /*       'precision', indicating a common logarithm.               */
{
  static char format[16];

  if (precision < -MAX_DIGITS-2) 
    sprintf(string, "%e", value);
  else if (precision < 0) {
    format[0] = '%';
    format[1] = '.';
    sprintf(format+2, "%df", -precision);
    sprintf(string, format, value);
  }
  else if (precision <= 4) 
    sprintf(string, "%.0f", value);
  else {
    sprintf(string, "%0e", value);
  }
}


void show_position( obj )
     primary_window *obj;
  /* POST : Queries the pointer position, and if it is located in the */
  /*        child window obj->plane, displays the world coordinates   */
  /*        inside the plane subwindow.                               */
{
  static char  pos_string[256];
  static char  x_pos[128];
  static char  y_pos[128];
  int          width, overlap;
  Window       root, child;
  int          root_x, root_y;
  int          x, y;
  unsigned int state;
  XPoint       view;
  complex      world;

  XQueryPointer(display, obj->primary, &root, &child, 
		&root_x, &root_y, &x, &y, &state);
  if (child == obj->plane.window) {
    view = w_primary_to_plane(obj, x, y);
    p_view_to_world(&(obj->plane), &world, view);

    /* XDrawImageString will be used, so that the new position will erase */
    /* the old (to minimize the flickering).  If the new position fails   */
    /* to completely cover the old, the edges must be erased.             */

    to_string(x_pos, world.x, obj->plane.log_decade-4);
    to_string(y_pos, world.y, obj->plane.log_decade-4);
    sprintf(pos_string, "(%s, %s)", x_pos, y_pos);
    width = XTextWidth(PositionFont, pos_string, strlen(pos_string));

    x = LeftMargin + obj->plane.width/2 + obj->plane.border_width;
    y = obj->plane.y - AXES_LABEL_Y_OFFSET;

    /* Check to see if the new position covers the old */
    if (width < obj->pos_width) {
      overlap = (obj->pos_width - width)/2 + 1;
      XClearArea(display, obj->drawable, 
		 x - obj->pos_width/2, y - PositionFont->ascent,
		 overlap, PositionFont->ascent+PositionFont->descent, False);
      XClearArea(display, obj->drawable, 
		 x + width/2, y - PositionFont->ascent,
		 overlap, PositionFont->ascent+PositionFont->descent, False);
    }

    if (obj->current_font != PositionFont) {
      XSetFont(display, obj->gc, PositionFont->fid);
      obj->current_font = PositionFont;
      XSetForeground(display, obj->gc, PrimaryFG);
      XSetBackground(display, obj->gc, PrimaryBG);
    }

    XDrawImageString(display, obj->drawable, obj->gc, 
		     x - width/2, y, pos_string, strlen(pos_string));
    obj->pos_width = width;
  }
}

    
void label_grid( obj )
     primary_window *obj;
  /* PRE  : The gid fields of 'obj->plane' are assigned. */
  /* POST : Labels the grid, on the primary window.      */
{
  double       x, y;
  int          precision;
  int          k = 0;
  XPoint       view;
  XPoint       line1, line2;
  complex      world;
  int          width;
  int          decade_width;
  int          skip;
  static char  value[128];

  precision = obj->plane.log_decade;
  XSetFont(display, obj->gc, LabelFont->fid);
  obj->current_font = LabelFont;
  XSetForeground(display, obj->gc, PrimaryFG);

  /* Label the y-axis */
  x = obj->plane.x0;
  y = obj->plane.gy0;

  while (y <= obj->plane.y1) {
    to_string(value, y, precision-1);
    view = plane_to_primary(&(obj->plane), x, y);
    view.x -= AXES_LABEL_X_OFFSET;
    view.y += obj->plane.border_width;

    position_string(obj->drawable, obj->gc, view.x, view.y,
		  1, 0, LabelFont, value, (char) 0); 
    y += obj->plane.decade;
  }

  /* Label the x-axis */
  x = obj->plane.gx0;
  y = obj->plane.y0;

  /* Calculate the width of the first label */
  to_string(value, x, precision-1);
  width = XTextWidth(LabelFont, value, strlen(value));

  /* Calculate the separation between decade lines */
  line1 = plane_to_primary(&(obj->plane), obj->plane.gx0, obj->plane.y0);
  line2 = plane_to_primary(&(obj->plane), obj->plane.gx0 + obj->plane.decade,
			   obj->plane.y0);
  decade_width = (int) line2.x - line1.x;
  
  /* Now calculate how many decade lines to skip */
  skip = (width + X_LABEL_BORDER)/abs(decade_width) + 1;

  while (x <= obj->plane.x1) {
    if (!(k % skip)) {
      to_string(value, x, precision-1);
      view = plane_to_primary(&(obj->plane), x, y);
      view.y += 2*obj->plane.border_width;
      view.x += obj->plane.border_width;
      
      position_string(obj->drawable, obj->gc, view.x, view.y,
		      0, 1, LabelFont, value, (char) 0); 
    }
    x += obj->plane.decade;
    k++;
  }

  /* Label the subgrid, if necessary */
  x = obj->plane.x0;

  if ((obj->plane.lines_y <= 2) && (obj->plane.subgrid)) {
    for (k = obj->plane.sub_yk0; k <= obj->plane.sub_yk1; k++) 

      if ((k % 10) && !(k % 2)) {
	y = obj->plane.gy0 + k*obj->plane.decade/10;
	to_string(value, y, precision-1);
	view = plane_to_primary(&(obj->plane), x, y);
	view.x -= AXES_LABEL_X_OFFSET;
	view.y += obj->plane.border_width;

	position_string(obj->drawable, obj->gc, view.x, view.y,
			1, 0, LabelFont, value, (char) 0); 

      }
  }

}


void handle_primary_event( obj, event )
     primary_window *obj;
     XEvent         *event;
  /* POST: handles the event 'event' occuring in the window 'obj'.  */
{
  switch (event->type) {
  case ConfigureNotify :
    resize_primary(obj, event->xconfigure.width, event->xconfigure.height);
    break;
  case MotionNotify :
    show_position(obj);
    handle_motion(obj, event);
    break;
    
/*  case Expose :*/
  }
}
