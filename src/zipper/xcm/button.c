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
/************************/
/* Screen Button Object */
/************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module contains the C functions for screen buttons, */
/*                 which are simple subwindows of a parent window.          */
/****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>
#include "globals.h"
#include "basic.h"
#include "button.h"

/***********************/
/* Screen Button Stuff */
/***********************/

void no_op()
  /* For buttons wihout specific functions */
{
}

void create_button( obj, parent, text, gc, font, x, y, width, height, 
		    border, foreground, background, hi_fore, hi_back,
		    initial_state, gravity )
     button_type   *obj;
     Window        parent;
     char          text[];
     GC            *gc;
     XFontStruct   *font;
     int           x, y;
     unsigned int  width, height;
     unsigned long border, foreground, background;
     unsigned long hi_fore, hi_back;
     enum button_status initial_state;
     int           gravity;
  /* PRE : 'parent' exists, and is presumably mapped.  'font' is assigned */
  /*        completely, i.e. by XLoadQueryFont.                           */
  /* POST: Opens a subwindow to be used as a 'button' with the specified  */
  /*       parameters.  The subwindow is created and mapped, but not      */
  /*       drawn.                                                         */
{
  unsigned long event_mask;

  obj->parent = parent;
  obj->font = font;
  obj->gc = gc;
  obj->width = width;
  obj->height = height;
  obj->status = initial_state;
  obj->foreground = foreground;
  obj->background = background;
  obj->hi_fore = hi_fore;
  obj->hi_back = hi_back;
  strncpy(obj->text, text, BUTTON_TEXT_SIZE-1);
  obj->text[BUTTON_TEXT_SIZE-1] = '\0';

  obj->status = button_active;

  event_mask = StructureNotifyMask | ButtonPressMask | ExposureMask |
               EnterWindowMask | LeaveWindowMask;

  obj->window = OpenSubwindow(parent, x, y, width, height, 
			      BUTTON_BORDER_WIDTH, border, background,
			      gravity, event_mask);

  XFlush(display);
}

void draw_button( obj )
     button_type   *obj;
  /* PRE : 'obj' is created.                                              */
  /* POST: Draws the background and text of 'obj', according to the state */
  /*       of the 'active' and 'pressed' fields of 'obj'.                 */
{
  switch (obj->status) {
  case (button_inactive) :
    XClearWindow(display, obj->window);
    center_string(obj->window, GrayGC, obj->width/2, obj->height/2,
		  obj->font, obj->text);
    break;
  case (button_active) :
      XSetBackground(display, *(obj->gc), obj->background);
      XSetForeground(display, *(obj->gc), obj->foreground);
      XClearWindow(display, obj->window);
      center_string(obj->window, *(obj->gc), 
		    obj->width/2, obj->height/2,
		    obj->font, obj->text);
      break;
    case (button_pressed) :
      XSetForeground(display, *(obj->gc), obj->hi_back);
      XFillRectangle(display, obj->window, *(obj->gc),
		     0, 0, obj->width, obj->height);
      XSetBackground(display, *(obj->gc), obj->hi_back);
      XSetForeground(display, *(obj->gc), obj->hi_fore);
      center_string(obj->window, *(obj->gc), 
		    obj->width/2, obj->height/2,
		    obj->font, obj->text);
      break;
  }
  XFlush(display);
}

void release_button( obj )
     button_type *obj;
  /* PRE : obj->status == button_pressed.                          */
  /* POST: returns 'obj' to the "active" (but not "pressed" state) */
{
  obj->status = button_active;
  draw_button(obj);
}

void change_button_border( obj )
     button_type *obj;
  /* POST: Draws a rectangle inside 'obj' so that the border appears larger */
{
  if (obj->status >= button_active) {
    XSetForeground(display, *(obj->gc), obj->foreground);
    XDrawRectangle(display, obj->window, *(obj->gc), 
		   0, 0, obj->width-1, obj->height-1);
    XDrawRectangle(display, obj->window, *(obj->gc), 
		   1, 1, obj->width-3, obj->height-3);
  }
}

  
void handle_button_event( obj, event )
     button_type *obj;
     XEvent      *event;
  /* POST: Parses a "button" event. */
{
  switch (event->type) 
    {
    case ButtonPress :
      if ((event->xbutton.button == Button1) && 
	  (obj->status >= button_active)) {

	/* The "button" was "pressed" */
	if (obj->status == button_pressed) {
	  release_button(obj);
	  (*(obj->apply))(obj);
	  /* it's up to the clien function to handle this case */
	}
	else {
	  obj->status = button_pressed;
	  draw_button(obj);
	  (*(obj->apply))(obj);
	  /* It's up to the client function to release the button */
	}
      }
      break;

    case EnterNotify :
      change_button_border(obj);
      break;

    case LeaveNotify :
      draw_button(obj);
      break;
      
    case ConfigureNotify :
    case Expose :
      draw_button(obj);
    }
}

void activate_button( obj )
     button_type *obj;
  /* POST: activates 'obj', by changing it's status to "button_active" and */
  /*       redrawing the window.                                           */
{
  obj->status = button_active;
  draw_button(obj);
  XFlush(display);
}

void deactivate_button( obj )
     button_type *obj;
  /* POST: activates 'obj', by changing it's status to "button_inactive" and */
  /*       redrawing the window.                                             */
{
  obj->status = button_inactive;
  draw_button(obj);
  XFlush(display);
}

