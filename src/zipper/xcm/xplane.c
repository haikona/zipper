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
/* Module for the Plane window object */
/**************************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module operates the plane window object, which is   */
/*                 set up to be a subwindow of another window.  The module  */
/*                 is responsible for handling the world transformations,   */
/*                 mouse control, keyboard shortcuts, grid lines, and       */
/*                 graphics functions.                                      */
/****************************************************************************/
/*  D. Marshall altered line 162 to start with hidden grid 1-14-94*/
/****/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include "globals.h"
#include "basic.h"
#include "complex.h"
#include "xplane.h"

#define DASH_LIST_SIZE 4
char dash_list[DASH_LIST_SIZE] = {1, 2, 5, 6};

static XPoint   PointBuffer[MAX_POINTS];
static XPoint   *PointBufferPos;

static XSegment SegmentBuffer[MAX_POINTS];
static XSegment *SegmentBufferPos;

/* The cursors used by the plane windows */
Cursor PlaneCursor;

/* The dashing */
char long_dash[2] = {4,8};
char short_dash[2] = {1,16};

/***********************/
/* Plane window object */
/***********************/

int rounds( source )
     double source;
{
  return((int) floor(source + 0.5));
}

char setup_plane_vars()
  /* PRE : 'display' is assigned.                                  */
  /* POST: Assigns the cursors, and fonts used by the plane_window */
  /*       object.  Returns True if succesful, False otherwise.    */
{
  PlaneCursor = XCreateFontCursor(display, XC_crosshair);
  return(1);
}

void map_plane_window( obj )
     plane_window *obj;
  /* POST: Maps 'obj->window' according the the parameters in the structure */
  /*       'obj->window' is unmapped each time its parent is resized.       */
{
  XSizeHints    hints;

/*  hints.flags = USPosition | USSize; 
  hints.x = obj->x;
  hints.y = obj->y;
  hints.width = obj->width;
  hints.height = obj->height;
  XSetNormalHints( display, obj->window, &hints);*/

  if ((obj->width > 0) || (obj->height > 0)) {
    XResizeWindow(display, obj->window, obj->width, obj->height);
/* The hints should also be changed! */
    XMapRaised(display, obj->window);
  }
  /* Otherwise the plane window remains unmapped until the size is */
  /* large enough to accomodate it.                                */
}

void open_plane_window( obj, parent, gc, x, y, width, height, x0, y0, x1, y1,
		        aspect, border, bd, bg, grid_color, subgrid_color,
		        show_grid )
     plane_window  *obj;
     Window        parent;
     GC            *gc;
     int           x, y;
     int           width, height;
     int           show_grid;
     double        x0, y0;
     double        x1, y1;
     double        aspect;
     unsigned int  border;
     unsigned long bd, bg;
     unsigned long grid_color;
     unsigned long subgrid_color;
  /* PRE : 'setup_plane_vars' has been called. */
  /* POST: Opens a new plane subwindow of 'parent' with the specified       */
  /*       parameters.  (x0, y0) and (x1,y1) are the suggested  lower-left  */
  /*       and upper-right world coordinates, repsectively.  The aspect     */
  /*       ratio of the window will be kept constant, so it the actual      */
  /*       corners may be different.  The world coordinates will be such    */ 
  /*       that the window is just large enough to contain the rectangle    */
  /*       of (x0,y0)-(x1,y1).                                              */
{
  unsigned long  event_mask;
  double         new_scale;

  event_mask = StructureNotifyMask | ButtonPressMask | ButtonReleaseMask |
    ButtonMotionMask | KeyPressMask | ExposureMask; 
  
  /* Create and map the window */
  obj->window = OpenSubwindow(parent, x, y, width, height, 
			      border, bd, bg,
			      UnmapGravity, event_mask);
  obj->parent = parent;
  obj->gc = gc;
  
  /* The actual y0 and y1 values will be reset when the window is mapped  */
  /* to ensure unit aspect.  The aritmetic mean of y0 and y1 will remain  */
  /* the center of the window in the y-direction.                         */

  obj->aspect = aspect;
  obj->center_x = (x1 + x0)/2.0;
  obj->center_y = (y1 + y0)/2.0;
  obj->scale = (x1 - x0);
  resize_plane_window(obj, width, height);

  obj->x = x;
  obj->y = y;
  obj->border_width = border;
  obj->bg = bg;
  obj->bd = bd;
  obj->grid_color = grid_color;
  obj->subgrid_color = subgrid_color;

  obj->drawable = obj->window;
/* The next line sets the default grid to be off--D. Marshall*/
  obj->show_grid = show_grid;

  /* The default mouse button functions are the Control functions */
  obj->button1 = plane_shift;
  obj->button2 = plane_translate;
  obj->button3 = plane_zoom;

  /* Now rescale the window so that (x0, y0) and (x1, y1) both appear */
  new_scale = 2*(y1 - obj->center_y)/obj->hw_ratio;
  if (new_scale > obj->scale)
    zoom_plane_window(obj, new_scale/obj->scale);

  obj->orig_x0 = obj->x0;
  obj->orig_x1 = obj->x1;
  obj->orig_y0 = obj->y0;
  obj->orig_y1 = obj->y1;

  XDefineCursor(display, obj->window, PlaneCursor);

  XFlush(display);
}

void destroy_plane_window( obj )
     plane_window *obj;
{
  XDestroyWindow(display, obj->window);
}

void assign_grid_values( obj )
     plane_window *obj;
  /* POST : Assigns the fields of 'obj' which control the appearance of */
  /*        the grid, if there is to be one.                            */
{
  double  size;
  double  decade;
  double  gx1, gy1;
  double  sub_gx0, sub_gy0;
  double  sub_gx1, sub_gy1;

  /* First determine the decade */
  size = MAX( fabs(obj->x0 - obj->x1), fabs(obj->y0 - obj->y1) );
  decade = log10(size);
  obj->log_decade = (int) floor(decade);
  obj->decade = pow(10.0, (double) obj->log_decade);
  
  /* Determine the first decade line in each direction */
  obj->gx0 = obj->decade*ceil(obj->x0/obj->decade);
  obj->gy0 = obj->decade*ceil(obj->y0/obj->decade);

  /* Determine the number of decade lines in each direction */
  obj->lines_x = (int) ceil((obj->x1 - obj->gx0)/obj->decade);
  obj->lines_y = (int) ceil((obj->y1 - obj->gy0)/obj->decade); 

  /* If there are too few lines, show the next decade down */
  if ( (decade - floor(decade)) < DECADE_SWITCH ) {
    obj->subgrid = 1;
    decade = obj->decade/10.0;
    
    sub_gx0 = decade*ceil(obj->x0/decade);
    sub_gy0 = decade*ceil(obj->y0/decade);

    sub_gx1 = decade*floor(obj->x1/decade);
    sub_gy1 = decade*floor(obj->y1/decade);

    obj->sub_xk0 = rounds((sub_gx0 - obj->gx0)/decade);
    obj->sub_yk0 = rounds((sub_gy0 - obj->gy0)/decade);

    obj->sub_xk1 = rounds((sub_gx1 - obj->gx0)/decade);
    obj->sub_yk1 = rounds((sub_gy1 - obj->gy0)/decade);
  }
  else {
    obj->subgrid = 0;

    obj->sub_xk0 = 0;
    obj->sub_yk0 = 0;

    obj->sub_xk1 = 10*obj->lines_x;
    obj->sub_yk1 = 10*obj->lines_y;
  }

}

void draw_plane_grid( obj )
     plane_window *obj;
{
  /* The grid shows lines at each integer multiple of some power of ten,   */
  /* called the "decade", then shows weaker lines subdividing each decade. */

  double  decade;
  double  x, y;
  int     k, k0;

  /* If there are too few lines, show the next decade down      */
  /* This is drawn first so the "brighter" lines appear on top. */

  if (obj->subgrid) {
    XSetForeground(display, *(obj->gc), obj->subgrid_color);
    decade = obj->decade/10.0;
    
    for (k = obj->sub_xk0; k <= obj->sub_xk1; k++)
      if (k % 10) 
	plane_draw_line2(obj, obj->gx0 + k*decade, obj->y0, 
			 obj->gx0 + k*decade, obj->y1);

    for (k = obj->sub_yk0; k <= obj->sub_yk1; k++)
      if (k % 10)
	plane_draw_line2(obj, obj->x0, obj->gy0 + k*decade,
			 obj->x1, obj->gy0 + k*decade); 
  }

  /* Draw the main grid */

  /* If the subgrid is the same color as the main grid, draw thick */
  if ( (obj->subgrid) && (obj->grid_color == obj->subgrid_color) )
    XSetLineAttributes(display, *(obj->gc), 
		       2, LineSolid, CapNotLast, JoinRound);

  /* Draw the lines */
  XSetForeground(display, *(obj->gc), obj->grid_color);
  for (k = 0; k <= obj->lines_x; k++) 
    plane_draw_line2(obj, obj->gx0 + k*obj->decade, obj->y0, 
		     obj->gx0 + k*obj->decade, obj->y1);

  for (k = 0; k <= obj->lines_y; k++) 
    plane_draw_line2(obj, obj->x0, obj->gy0 + k*obj->decade,
		     obj->x1, obj->gy0 + k*obj->decade);

  /* Reset the linewidth to 0, so the fastest algorithm is used */
  XSetLineAttributes(display, *(obj->gc), 
		     0, LineSolid, CapNotLast, JoinRound);

}

void clear_plane_window( obj )
     plane_window *obj;
{
  if (obj->drawable == obj->window)
    XClearWindow(display, obj->window);
  else {
    XSetForeground(display, *(obj->gc), obj->bg);
    XFillRectangle(display, obj->drawable, *(obj->gc), 
		   0, 0, obj->width, obj->height);
  }
}
  
void redraw_plane_window( obj )
     plane_window *obj;
{
  int k, s0, s1;
  XPoint point;
  complex z;
  XGCValues values;
  unsigned long mask;

  XSetForeground(display, (*obj->gc), WhitePixel(display, screen));
  XSetBackground(display, (*obj->gc), BlackPixel(display, screen));

  /* Draw the grid lines, if necessary */
  assign_grid_values(obj);

  clear_plane_window(obj);

  if (obj->show_grid)
    draw_plane_grid(obj);

  /* Now redraw the object */
  (*obj->redraw)(obj);
  XFlush(display); 
}

void resize_plane_window( obj, width, height)
     plane_window *obj;
     int         width, height;
  /* POST : resizes the obj parameters, but doesn't redraw the contents.    */
  /*        the values of y0 and y1 are modified so that the transformation */
  /*        has determinant one, i.e. squares look like sqaures.            */
{
  double   new_y0, new_y1;

  if (width <= 0)
    width = 1;
  if (height <= 0)
    height = 1;

  obj->hw_ratio = height/(1.0*width)*obj->aspect;
  new_y0 = obj->center_y - obj->hw_ratio*obj->scale/2.0;
  new_y1 = obj->center_y + obj->hw_ratio*obj->scale/2.0;

  obj->width = width;
  obj->height = height;
  obj->y0 = new_y0;
  obj->y1 = new_y1;

  obj->x0 = obj->center_x - obj->scale/2.0;
  obj->x1 = obj->center_x + obj->scale/2.0;
}

void zoom_plane_window( obj, scale )
     plane_window *obj;
     double       scale;
  /* POST : zooms in obj, by a factor of scale.  */
{
  double new_scale;
  double scale_x, scale_y;
  double min_x_scale, min_y_scale;

  new_scale = obj->scale*scale;

  /* Check to see that the new scale isn't too small, i.e. each point on */
  /* the window is a distinct value.                                     */

  min_x_scale = MAX(obj->center_x*EFFECTIVE_EPSILON*obj->width, MINIMUM_SCALE);
  min_y_scale = MAX(obj->center_y*EFFECTIVE_EPSILON*obj->width/obj->hw_ratio, 
		    MAX(MINIMUM_SCALE/obj->hw_ratio, 
			MINIMUM_SCALE*obj->hw_ratio));
  
  scale_x = MAX(new_scale, min_x_scale);
  scale_y = MAX(new_scale, min_y_scale);

  /* Check to see that the scale isn't too big */

  scale_x = MIN(scale_x, MAXIMUM_SCALE);
  scale_y = MIN(scale_y, MIN(MAXIMUM_SCALE/obj->hw_ratio, 
			     MAXIMUM_SCALE*obj->hw_ratio));

  obj->scale = MAX(scale_x, scale_y);

  obj->x0 = obj->center_x - obj->scale/2.0;
  obj->x1 = obj->center_x + obj->scale/2.0;
  obj->y0 = obj->center_y - obj->scale*obj->hw_ratio/2.0;
  obj->y1 = obj->center_y + obj->scale*obj->hw_ratio/2.0;
}

void recenter_plane_window( obj, x, y )
     plane_window  *obj;
     double        x, y;
  /* PRE  : obj->width and obj->height are nonzero                    */
  /* POST : translates obj so that the center of the window has world */
  /*        coordinates (x, y).  The contents are not redrawn.        */
  /* NOTE : This function is the same as the next, except that the    */
  /*        new center is given is world coordinates.                 */
{
  double world_dx, world_dy;
  
  world_dx = x - obj->center_x;
  world_dy = y - obj->center_y;

  obj->x0 += world_dx;
  obj->x1 += world_dx;
 
  obj->y0 += world_dy;
  obj->y1 += world_dy;

  obj->center_x += world_dx;
  obj->center_y += world_dy;
}

void v_recenter_plane_window( obj, x, y )
     plane_window  *obj;
     int           x, y;
  /* PRE  : obj->width and obj->height are nonzero                     */
  /* POST : translates obj so that the center of the window has window */
  /*        coordinates (x, y).  The contents are not redrawn.         */
{
  complex z;
  XPoint  p;
  
  p.x = (unsigned short) x;
  p.y = (unsigned short) y;
  p_view_to_world(obj, &z, p);

  recenter_plane_window(obj, z.x, z.y);
}

void translate_plane_window( obj, dx, dy )
     plane_window  *obj;
     int          dx, dy;
  /* PRE  : obj->width and obj->height are nonzero  */
  /* POST : translates obj by (dx, dy) pixels.      */
{
  double world_dx, world_dy;
  
  world_dx = dx*(obj->x1 - obj->x0)/(1.0*obj->width);
  world_dy = dy*(obj->y1 - obj->y0)/(1.0*obj->height);

  obj->x0 += world_dx;
  obj->x1 += world_dx;
 
  obj->y0 += world_dy;
  obj->y1 += world_dy;

  obj->center_x += world_dx;
  obj->center_y += world_dy;
}

/* The following three are suitable for the mouse button function pointers */

void plane_zoom( obj, event )
     plane_window *obj;
     XEvent      *event;
  /* PRE : event contains a ButtonPress event.                          */
  /* POST: zooms in and out obj, until a non-MotionNotify event occurs. */
{
  int x, y;
  double dx, dy;
  double zoom;

  x = event->xbutton.x;
  y = event->xbutton.y;

  /* translate to the point (x, y) */
/*  v_recenter_plane_window(obj, x, y);*/

  /* wait for an event, if there isn't one. */
  XPeekEvent(display, event);

  while (collect_motions(event)) {
    dy = event->xmotion.y - y;
    zoom_plane_window(obj, pow(ZOOM_FACTOR, dy/(1.0*obj->height)));
    redraw_plane_window(obj);
    x = event->xmotion.x;
    y = event->xmotion.y;
  } 

}      

void plane_translate( obj, event )
     plane_window *obj;
     XEvent      *event;
  /* PRE : event contains a ButtonPress event.                          */
  /* POST: translates obj, until a non MotionNotify event occurs.       */
{
  int x, y;
  int dx, dy;

  x = event->xbutton.x;
  y = event->xbutton.y;
/*  translate_plane_window(obj, obj->width/2 - x, obj->height/2 - y);
  redraw_plane_window(obj); */

  /* wait for an event, if there isn't one. */
  XPeekEvent(display, event);

  while (collect_motions(event)) {
    dx = x - event->xmotion.x;
    dy = -(y - event->xmotion.y);
    translate_plane_window(obj, dx, dy);
    redraw_plane_window(obj);
    x = event->xmotion.x;
    y = event->xmotion.y;
  }
}


void plane_shift( obj, event )
     plane_window *obj;
     XEvent       *event;
  /* PRE : event contains a ButtonPress event.                          */
  /* POST: translates the plane window so that the center of the window */
  /*       is located where the pointer is in 'event'.                  */
{
  v_recenter_plane_window(obj, event->xbutton.x, event->xbutton.y);
  redraw_plane_window(obj);
}



void handle_plane_key( obj, event )
     plane_window *obj;
     XEvent       *event;
  /* PRE : 'event' is a KeyPress event.             */
  /* POST: interprets the keystroke(s) in 'event'.  */
{
  int    length;
  int    buffer_max = 64;
  char   key_buffer[65];
  KeySym sym;
  XComposeStatus status;
  int    dx = 0, dy = 0;

  length = XLookupString(&(event->xkey), 
			 key_buffer, buffer_max, &sym, &status);

  /* Check for a numeric keystroke */
  if ((sym >= '1') && (sym <= '9')) {
    if (event->xkey.state & ControlMask)

      /* zoom out by a factor of 10^key */
      zoom_plane_window(obj, pow(10.0, (double) (sym - '0')));
    else

      /* zoom in by a factor of 10^key */
      zoom_plane_window(obj, pow(0.1, (double) (sym - '0')));
    redraw_plane_window(obj);
  }
  
  /* Otherwise check for the individual keystrokes */
  switch (sym) 
    {
    case 'u' :
    case 'U' :
      /* return to unit scale */
      zoom_plane_window(obj, 2.0/obj->scale);
      redraw_plane_window(obj);
      break;
      
    case 'o' :
    case 'O' :
      /* translate to the origin */
      recenter_plane_window(obj, 0.0, 0.0);
      redraw_plane_window(obj);
      break;

    case 'z' :
    case 'Z' :
      if (event->xkey.state & ControlMask)

	/* zoom out by a factor of 1.01 */
	zoom_plane_window(obj, KEY_ZOOM_FACTOR);
      else
	
	/* zoom in by a factor of 1.01 */
	zoom_plane_window(obj, 1.0/KEY_ZOOM_FACTOR);
      redraw_plane_window(obj);
      break;

    case 'x' :
    case 'X' :
      if (event->xkey.state & ControlMask)

	/* zoom out by a factor of 1.01 */
	zoom_plane_window(obj, KEY_FINE_ZOOM_FACTOR);
      else
	
	/* zoom in by a factor of 1.01 */
	zoom_plane_window(obj, 1.0/KEY_FINE_ZOOM_FACTOR);
      redraw_plane_window(obj);
      break;

    case XK_Up    : dy =  1; break;
    case XK_Down  : dy = -1; break;
    case XK_Left  : dx = -1; break;
    case XK_Right : dx =  1; break;
    }

  /* Handle a keyboard translate */

  if ( (dy != 0) || (dx != 0) ) {
    if (event->xkey.state & ControlMask) {
      dx *= obj->width/2.0;
      dy *= obj->height/2.0;
    }
    else if (event->xkey.state & ShiftMask) {
      dx *= 2;
      dy *= 2;
    }
    else {
      dx *= obj->width/10.0;
      dy *= obj->width/10.0;
    }
    translate_plane_window(obj, dx, dy);
    redraw_plane_window(obj);
  }
	
}
    
void handle_plane_event( obj, event )
     plane_window *obj;
     XEvent      *event;
{
  int            x, y;
  int            width, height;
  int            button;
  int            which;

  switch (event->type) 
    {
      /* Check for a Control-button */
    case ButtonPress : 
      if (event->xbutton.state & ControlMask) {

	if (event->xbutton.button == Button1) {
	  plane_shift(obj, event);
	}

	if (event->xbutton.button == Button3) {
	  plane_zoom(obj, event);
	}

	if (event->xbutton.button == Button2) {
	  plane_translate(obj, event);
	}
      }

      /* Check for a Shift button */
      else if (event->xbutton.state & ShiftMask) {

	if (event->xbutton.button == Button3) {
	  v_recenter_plane_window(obj, event->xbutton.x, event->xbutton.y);
	  zoom_plane_window(obj, 1/ZOOM_FACTOR);
	  redraw_plane_window(obj);
	}
      }

      else {
	if (event->xbutton.button == Button1) 
	  (*obj->button1)(obj, event);

	if (event->xbutton.button == Button2) 
	  (*obj->button2)(obj, event);

	if (event->xbutton.button == Button3) 
	  (*obj->button3)(obj, event);
      }

      break; 
    case KeyPress :
      handle_plane_key(obj, event);
      break;

    case Expose :
      redraw_plane_window(obj);
      break; 

    case UnmapNotify :
      /* ASSERT: obj is only unmapped in the case of a resize, or unmap     */
      /*         of its parent.  The parent must call 'resize_plane_window' */
      /*         if this has happened.                                      */
      map_plane_window(obj);

    }
}

/**********************/
/* Graphics Functions */
/**********************/

void c_exch( z1, z2 )
     complex *z1;
     complex *z2;
  /* POST: exchanges 'z1' and 'z2' */
{
  complex z;

  z.x = z2->x;
  z.y = z2->y;

  z2->x = z1->x;
  z2->y = z1->y;

  z1->x = z.x;
  z1->y = z.y;
}

void p_world_to_view( obj, z, pt)
     plane_window *obj;
     complex     z;
     XPoint      *pt;
  /* POST : converts z to the physical coordinates of obj.  */
{
  short x, y;

  x = (z.x - obj->x0)/obj->scale*obj->width;
  y = (z.y - obj->y0)/obj->scale/obj->hw_ratio*obj->height;

  pt->x = x;
  pt->y = obj->height - y;
}

void p_view_to_world( obj, z, pt)
     plane_window *obj;
     complex     *z;
     XPoint      pt;
{
  z->x = (double) pt.x*(obj->scale)/(1.0*obj->width) + obj->x0;
  z->y = (double) (obj->height - pt.y)*(obj->scale*obj->hw_ratio)
    /(1.0*obj->height) + obj->y0;
}

char p_in_view( obj, z )
     plane_window *obj;
     complex      z;
  /* POST: Returns True if 'z' is visible in 'obj', false otherwise */
{
  return( (z.x >= obj->x0) && (z.x <= obj->x1) &&
	  (z.y >= obj->y0) && (z.y <= obj->y1) );
}

char p_original_okay( obj )
     plane_window *obj;
  /* POST: True if the original coordiantes of the plane window will fall */
  /*       within the 'short' range.                                      */   
  /* NOTE: If the original coordinates bound all the lines, then if this  */
  /*       function returns true, X is probably capable of clipping the   */
  /*       lines properly.                                                */
{
  double x0, y0;
  double x1, y1;

  x0 = (obj->orig_x0 - obj->x0)/obj->scale*obj->width;
  y0 = obj->height - 
    (obj->orig_y0 - obj->y0)/obj->scale/obj->hw_ratio*obj->height;

  x1 = (obj->orig_x1 - obj->x0)/obj->scale*obj->width;
  y1 = obj->height - 
    (obj->orig_y1 - obj->y0)/obj->scale/obj->hw_ratio*obj->height;

  return( (x0 > (double) SHRT_MIN) && (y0 < (double) SHRT_MAX) &&
 	  (x1 < (double) SHRT_MAX) && (y1 > (double) SHRT_MIN) );

}

char p_original_in_view( obj )
     plane_window *obj;
  /* POST: True if the original world coordinates of 'obj' are currently */
  /*       visible.                                                      */
{
  return( (obj->orig_x0 >= obj->x0) && (obj->orig_x1 <= obj->x1) &&
	  (obj->orig_y0 >= obj->y0) && (obj->orig_y1 <= obj->y1) );
}

char p_line_in_view( obj, z1, z2 )
     plane_window *obj;
     complex      z1, z2;
  /* POST: Returns True if the line segment joining 'z1' and 'z2' is */
  /*       visible inside 'obj'.                                     */
{
  double m1, m2;
  double x;

  /* If either endpoint is visible, the line is also */
  if (p_in_view(obj, z1) || p_in_view(obj, z2))
    return(1);

  /* If both the y coordinates lie above or below the window, it's not */
  /* visible.  The same is true for the x coordinates.                 */
  if ( (MAX(z1.x, z2.x) <= obj->x0) || (MIN(z1.x, z2.x) >= obj->x1) )
    return(0);
  if ( (MAX(z1.y, z2.y) <= obj->y0) || (MIN(z1.y, z2.y) >= obj->y1) )
    return(0);

  /* Check for a vertical line */
  if (z1.x == z2.x) {
/*    return( ((z1.y <= obj->y0) && (z2.y >= obj->y1)) ||
	    ((z2.y <= obj->y0) && (z1.y >= obj->y1))    );*/
    return(1);
  }
  else {
    /* Find the intersection of the line (z1, z2) and the diagonal of obj */
    m1 = (z1.y - z2.y)/(z1.x - z2.x);
    if (m1 >= 0.0) {
      m2 = -(obj->y1 - obj->y0)/(obj->x1 - obj->x0);
      x = (z1.y - obj->y1 + m2*obj->x0 - m1*z1.x)/(m2-m1);
      return( (obj->x0 <= x) && (x <= obj->x1) );
    }
    else {
      m2 = (obj->y1 - obj->y0)/(obj->x1 - obj->x0);
      x = (z1.y - obj->y0 + m2*obj->x0 - m1*z1.x)/(m2-m1);
      return( (obj->x0 <= x) && (x <= obj->x1) );
    }
  }
}

char p_clip_line( obj, z1, z2, new_z1, new_z2 )
     plane_window *obj;
     complex      z1, z2;
     complex      *new_z1, *new_z2;  /* Return values */
  /* POST : If the line segment joining 'z1' and 'z2' is visible,         */
  /*        'new_z1' and 'new_z2' contain the endpoints of the visible    */
  /*        portion of that segment, i.e. clipped to the window of 'obj'. */
  /*        True is returned if the line is visible, False is returned    */
  /*        otherwise.                                                    */
  /*        The orientation of the segment is preserved.                  */
{
  double  slope;
  double  x_north, x_south;

  /* First check to see if the line is visible at all. */
  if (!p_line_in_view(obj, z1, z2)) 
    return(0);

  /* Check for a vertical line */
  if (z1.x == z2.x) {
    new_z1->x = z1.x;
    new_z2->x = z1.x;
    
    new_z1->y = MAX(obj->y0, MIN(z1.y, z2.y));
    new_z2->y = MIN(obj->y1, MAX(z1.y, z2.y));

    /* preserve the orientation */
    if (z2.y < z1.y)
      c_exch(new_z1, new_z2);
  }

  /* Check for a horizontal line */
  else if (z1.y == z2.y) {
    new_z1->x = MAX(obj->x0, MIN(z1.x, z2.x));
    new_z2->x = MIN(obj->x1, MAX(z1.x, z2.x));

    new_z1->y = z1.y;
    new_z2->y = z1.y;

    /* preserve the orientation */
    if (z2.x < z1.x)
      c_exch(new_z1, new_z2);
  }

  else {
    /* Calculate the slope of the line */
    slope = (z2.y - z1.y)/(z2.x - z1.x);
    
    /* Calculate the intersection of the line and the north and south */
    /* edges of the window.                                           */
    x_north = z1.x + (obj->y1 - z1.y)/slope;
    x_south = z1.x + (obj->y0 - z1.y)/slope;
    
    if (slope >= 0.0) {
      new_z1->x = MAX( MAX(obj->x0, x_south), MIN(z1.x, z2.x) );
      new_z2->x = MIN( MIN(obj->x1, x_north), MAX(z1.x, z2.x) );
    }
    else {
      new_z1->x = MAX( MAX(obj->x0, x_north), MIN(z1.x, z2.x) );
      new_z2->x = MIN( MIN(obj->x1, x_south), MAX(z1.x, z2.x) );
    }
    
    new_z1->y = z1.y + slope*(new_z1->x - z1.x);
    new_z2->y = z1.y + slope*(new_z2->x - z1.x);

    /* preserve the orientation */
    if (z2.x < z1.x)
      c_exch(new_z1, new_z2);
  }

  return(1);
}

void plane_draw_line( obj, z0, z1)
     plane_window *obj;
     complex     z0, z1;
{
  XPoint v0, v1;

  p_world_to_view(obj, z0, &v0);
  p_world_to_view(obj, z1, &v1);

  XDrawLine(display, obj->drawable, *(obj->gc), 
	    v0.x, v0.y, v1.x, v1.y);
}

void plane_draw_line2( obj, x0, y0, x1, y1 )
     plane_window *obj;
     double       x0, y0;
     double       x1, y1;
  /* POST : for convenience only */
{
  complex z0, z1;

  z0.x = x0;
  z0.y = y0;

  z1.x = x1;
  z1.y = y1;

  plane_draw_line(obj, z0, z1);
}

void plane_clip_line( obj, z0, z1 )
     plane_window *obj;
     complex      z0, z1;
{
  complex clip0, clip1;
  XPoint  v0, v1;

  if (p_clip_line(obj, z0, z1, &clip0, &clip1)) {
    p_world_to_view(obj, clip0, &v0);
    p_world_to_view(obj, clip1, &v1);
    
    XDrawLine(display, obj->drawable, *(obj->gc), 
	       v0.x, v0.y, v1.x, v1.y);
    XDrawPoint(display, obj->drawable, *(obj->gc), v1.x, v1.y);
  }
}

/*******************************************/
/* Point and Line Segment Buffer Functions */
/*******************************************/

void clear_PointBuffer()
  /* POST: Resets the counter PointBufferPos */
{
  PointBufferPos = PointBuffer;
}

void clear_SegmentBuffer()
{
  SegmentBufferPos = SegmentBuffer;
}

void dump_buffer_path( obj )
     plane_window *obj;
  /* POST: draws a connected path of line segments between the points of */
  /*       PointBuffer, moves the final point to the beginning of the    */
  /*       buffer, and sets the buffer position to the start + 1.        */
{
  if (PointBufferPos != PointBuffer) {
    XDrawLines(display, obj->drawable, *(obj->gc), PointBuffer, 
	       (int) (PointBufferPos - PointBuffer), CoordModeOrigin);
    *PointBuffer = *PointBufferPos;
    PointBufferPos = PointBuffer+1;
  }
}

void dump_segment_buffer( obj )
     plane_window *obj;
{
  if (SegmentBufferPos != SegmentBuffer) {
    XDrawSegments(display, obj->drawable, *(obj->gc), SegmentBuffer,
		  (int) (SegmentBufferPos - SegmentBuffer));
    SegmentBufferPos = SegmentBuffer;
  }
}

void draw_buffer_line( obj, z1, z2 )
     plane_window *obj;
     complex      z1, z2;
{
  XPoint  p1, p2;
  complex cz1, cz2;  /* For the clipped lines */

  /* First check to see if the entire segment is in view */

  if ((z1.x >= obj->x0) && (z1.x <= obj->x1) &&
      (z1.y >= obj->y0) && (z1.y <= obj->y1)  &&

      (z2.x >= obj->x0) && (z2.x <= obj->x1) &&
      (z2.y >= obj->y0) && (z2.y <= obj->y1)) {

    if (PointBufferPos != PointBuffer) {
      p_world_to_view(obj, z2, PointBufferPos++);
      if (PointBufferPos - PointBuffer >= MAX_POINTS)
	dump_buffer_path(obj);
    }
    else {
      p_world_to_view(obj, z1, PointBufferPos++);
      p_world_to_view(obj, z2, PointBufferPos++);
    }
  }

  else {
    dump_buffer_path(obj);
    clear_PointBuffer();

    if (p_clip_line(obj, z1, z2, &cz1, &cz2)) {
      p_world_to_view(obj, cz1, &p1);
      p_world_to_view(obj, cz2, &p2);
      
      SegmentBufferPos->x1 = p1.x;
      SegmentBufferPos->y1 = p1.y;
      
      SegmentBufferPos->x2 = p2.x;
      SegmentBufferPos->y2 = p2.y;
      
      SegmentBufferPos++;
      if (SegmentBufferPos - SegmentBuffer >= MAX_POINTS)
	dump_segment_buffer(obj);
    }
  }
}
    
    
void plane_draw_simple_lines( obj, lines, count )
     plane_window *obj;
     complex     lines[];
     int         count;
  /* POST: draws at most MAX_POINTS connected line segments from the array */
  /*       'lines' into obj.                                               */
  /* NOTE: No range checking or preclipping is performed before the array  */
  /*       is sent to the server, so the results will be unpredictable if  */
  /*       some of the endpoints cause an overflow when converted to the   */
  /*       window coordinates.                                             */
{
  static XPoint pts[MAX_POINTS];
  int           total_points;
  int           k;

  total_points = MIN(count, MAX_POINTS);

  for (k = 0; k < total_points; k++)
    p_world_to_view(obj, lines[k], pts+k);

  XDrawLines(display, obj->drawable, *(obj->gc), 
	     pts, total_points, CoordModeOrigin);
}

void plane_draw_lines( obj, lines, count )
     plane_window *obj;
     complex     lines[];
     int         count;
  /* POST: draws connected line segments from the array 'lines' into obj.  */
  /*       Lines which don't appear are not sent to the server.            */
{
  static XPoint pts[MAX_POINTS];
  int           k, n = 0;
  char          line_break = 1;

  while (n < count) {
    /* find the first visible line */
    while ((n < count-1) && !p_in_view(obj, lines[n], lines[n+1]))
      n++;

    /* There are no visible lines */
    if (n >= count-1)
      break;

    /* ASSERT: n < count-1, and lines[n]--lines[n+1] is a visible line */
    k = 1;
    p_world_to_view(obj, lines[n], pts);
    n++;
    while ( (k < MAX_POINTS) && (n < count) &&
	    p_line_in_view(obj, lines[n-1], lines[n])) {
      p_world_to_view(obj, lines[n], pts+k);
      k++;
      n++;
    }

    /* ASSERT: pts[0]...pts[k] is a sequence of connected, visible lines */
    XDrawLines(display, obj->drawable, *(obj->gc), pts, k+1, CoordModeOrigin);

    /* If k hits MAX_POINTS, and n < count, the previous didn't end because */
    /* an invisible line was encountered, so it is possible that the        */
    /* connected sequence didn't terminate.  To be certain, roll back one   */
    /* point and start again.                                               */

    if ((k == MAX_POINTS) && (n < count))
      n--;
  }
}

void plane_clip_lines0( obj, lines, count )
     plane_window *obj;
     complex     lines[];
     int         count;
  /* POST: Draws 'count' connected line segments from the array 'lines', */
  /*       where each line is clipped before being sent to the server.   */
{
  int k;

  for (k = 0; k < count-1; k++)
    plane_clip_line(obj, lines[k], lines[k+1]);
}


void plane_clip_lines( obj, lines, count )
     plane_window *obj;
     complex     lines[];
     int         count;
  /* POST: Draws 'count' connected line segments from the array 'lines', */
  /*       where each line is clipped before being sent to the server.   */
{
  int k, n = 0;
  complex z1, z2;
  XPoint  p1, p2;
  static XSegment line_seg[MAX_POINTS];

  for (k = 0; k < count-1; k++) {
    if (p_clip_line(obj, lines[k], lines[k+1], &z1, &z2)) {

/*if ( (lines[k].x >= obj->x0) && (lines[k].x <= obj->x1) &&
     (lines[k].y >= obj->y0) && (lines[k].y <= obj->y1)  &&

     (lines[k+1].x >= obj->x0) && (lines[k+1].x <= obj->x1) &&
     (lines[k+1].y >= obj->y0) && (lines[k+1].y <= obj->y1) ) */

      p_world_to_view(obj, z1, &p1);
      p_world_to_view(obj, z2, &p2);

      line_seg[n].x1 = p1.x;
      line_seg[n].y1 = p1.y;

      line_seg[n].x2 = p2.x;
      line_seg[n].y2 = p2.y;

      n++;
      if (n >= MAX_POINTS) {
	XDrawSegments(display, obj->drawable, *(obj->gc), line_seg, n);
	n = 0;
      }
    }
  }

  /* draw any leftover points */
  if (n > 0) {
    XDrawSegments(display, obj->drawable, *(obj->gc), line_seg, n);
  }
}

void plane_general_clip( obj, lines, count )
     plane_window *obj;
     complex      lines[];
     int          count;
{
  int k;

  clear_PointBuffer();
  clear_SegmentBuffer();

  for (k = 0; k <count-1; k++)
    draw_buffer_line(obj, lines[k], lines[k+1]);
  
  dump_buffer_path(obj);
  dump_segment_buffer(obj);
}

void plane_draw_point( obj, z )
     plane_window *obj;
     complex      z;
  /* POST: draws 'z' into 'obj. */
{
  XPoint p;

  p_world_to_view(obj, z, &p);
  XDrawPoint(display, obj->drawable, *(obj->gc), (int) p.x, (int) p.y);
}

void plane_draw_rect( obj, z )
     plane_window *obj;
     complex      z;
{
  XPoint p;

  p_world_to_view(obj, z, &p);
  XDrawRectangle(display, obj->drawable, *(obj->gc), 
		 (int) p.x - BOX_WIDTH/2, (int) p.y - BOX_HEIGHT/2, 
		 BOX_WIDTH, BOX_HEIGHT);
}

void plane_draw_points( obj, points, count )
     plane_window *obj;
     complex      points[];
     int          count;
  /* POST : draws at most MAX_POINTS from 'points' into 'obj'. */
{
  static XPoint pts[MAX_POINTS];
  int           k, n = 0;

  count = MIN(count, MAX_POINTS);
  for (k = 0; k < count; k++) {
    /* Only draw the points which appear in the window */

    if ( (points[k].x >= obj->x0) && (points[k].x <= obj->x1) &&
	 (points[k].y >= obj->y0) && (points[k].y <= obj->y1) ) {

      p_world_to_view(obj, points[k], pts+n);
      n++;
    }
  }

  XDrawPoints(display, obj->drawable, *(obj->gc), pts, n, CoordModeOrigin);
}

void plane_draw_rects( obj, points, count )
     plane_window *obj;
     complex      points[];
     int          count;
  /* POST : draws at most MAX_POINTS from 'points' into 'obj'. */
{
  static XRectangle rect[MAX_POINTS];
  int               k, n = 0;

  count = MIN(count, MAX_POINTS);
  for (k = 0; k < count; k++) {
    /* Only draw the points which appear in the window */

    if ( (points[k].x >= obj->x0) && (points[k].x <= obj->x1) &&
	 (points[k].y >= obj->y0) && (points[k].y <= obj->y1) ) {

      p_world_to_view(obj, points[k], rect+n);
      rect[n].x -= BOX_WIDTH/2;
      rect[n].y -= BOX_HEIGHT/2;
      rect[n].width = BOX_WIDTH;
      rect[n].height = BOX_HEIGHT;
      n++;
    }
  }

  XDrawRectangles(display, obj->drawable, *(obj->gc), rect, n);
}



