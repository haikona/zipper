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
/*************************/
/* Pull-Down Menu Object */
/*************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module contains the C functions for pulldown menus, */
/*                 which are transient windows.                             */
/****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "menu.h"

/*********************************************************/
/* Functions to operate the windows for the menu entries */
/*********************************************************/

void open_choice_window( obj, parent, gc, x, y, width, height, 
			 fore, back, hi_fore, hi_back, text, font, 
			 left_margin, index )
     choice_window *obj;
     Window        parent;
     GC            *gc;
     int           x, y;
     unsigned int  width, height;
     unsigned long fore, back;
     unsigned long hi_fore, hi_back;
     char          *text;
     XFontStruct   *font;
     int           left_margin;
     int           index;
{
  unsigned long event_mask;

  event_mask = ExposureMask | EnterWindowMask | LeaveWindowMask;
  obj->window = OpenSubwindow(parent, x, y, width, height, 
			      CHOICE_BORDER, back, back, 
			      NorthWestGravity, event_mask);
  strncpy(obj->text, text, MENU_ENTRY_LENGTH-1);
  obj->text[MENU_ENTRY_LENGTH-1] = '\0';
  obj->index = index;
  obj->font = font;

  obj->x = x;
  obj->y = y;
  obj->width = width;
  obj->height = height;
  
  obj->foreground = fore;
  obj->background = back;
  obj->hi_fore = hi_fore;
  obj->hi_back = hi_back;

  obj->parent = parent;
  obj->gc = gc;
  obj->left_margin = left_margin;
  obj->drawable = obj->window;
  obj->pointer_in = 0;
  obj->selected = 0;
}

void redraw_choice( obj )
     choice_window *obj;
{
  int x, y;
  Window root, child;
  int    root_x, root_y;
  unsigned int kb;

  XClearWindow(display, obj->window);
  XSetFont(display, *(obj->gc), obj->font->fid);
  /* Check to see if the choice is higlighted */

  x = obj->left_margin;
  y = obj->height - CHOICE_BORDER - obj->font->descent -MENU_MARGIN;

  if (obj->pointer_in) {
    XSetForeground(display, *(obj->gc), obj->hi_back);
    XFillRectangle(display, obj->drawable, *(obj->gc),
		   0, 0, obj->width, obj->height);
    XSetForeground(display, *(obj->gc), obj->hi_fore);
  }
  else {
    XClearWindow(display, obj->window);
    XSetForeground(display, *(obj->gc), obj->foreground);
  }

  if (obj->text[0])
    XDrawString(display, obj->drawable, *(obj->gc),
		x, y, obj->text, strlen(obj->text)); 

  if (obj->selected)
    XFillArc(display, obj->drawable, *(obj->gc), 
	     MENU_MARGIN, (obj->height - MENU_MARK_SIZE)/2,
	     MENU_MARK_SIZE, MENU_MARK_SIZE, 0, 23040); 
  XFlush(display);
}


char handle_choice_event( obj, event )
     choice_window *obj;
     XEvent        *event;
  /* POST: handles 'event' with respect to 'obj'.  Returns True if */
  /*       a button was pressed, false otherwise.                  */
{
  switch (event->type) {
  case Expose :
    redraw_choice(obj);
    break;

  case EnterNotify :
    obj->pointer_in = 1;
    redraw_choice(obj);
    break;

  case LeaveNotify :
    obj->pointer_in = 0;
    redraw_choice(obj);
    break;
  }
  return(0);
}


/******************************************************/
/* Functions to operate the windows for the main menu */
/******************************************************/

void create_simple_menu( obj, x, y, 
		         fore, back, bd, hi_fore, hi_back, title,
			 entry_text, entry_count, font, 
			 index)
     simple_menu   *obj;
     int           x, y;
     unsigned long fore, back, bd;
     unsigned long hi_fore, hi_back;
     char          title[];
     char          *entry_text[];
     int           entry_count;
     XFontStruct   *font;
     int           index;
  /* POST: Creates (but does not map) a simple menu window and all the */
  /*       choice windows contained therein.                           */
{
  unsigned long        mask;
  XSetWindowAttributes attr;
  XGCValues            values;
  XSizeHints           hints;

  int  width, height;
  int  entry_height;
  int  k;


  /* determine the width and height */
  entry_height = font->ascent + font->descent + 2*MENU_MARGIN;
  height = entry_height*entry_count;

  width = 0;
  for (k = 0; k < entry_count; k++)
    width = MAX(width, XTextWidth(font, entry_text[k], strlen(entry_text[k])));
  width += MENU_LEFT_MARGIN + MENU_MARGIN;

  if (title[0] != '\0') {
    height += entry_height;
    width = MAX(width, XTextWidth(font, title, strlen(title)));
    strcpy(obj->title, title);
    obj->has_title = 1;
  }
  else
    obj->has_title = 0;

  /* assign the parameters */

  obj->entry_count = entry_count;
  obj->index = index;
  obj->font = font;

  obj->x = x;
  obj->y = y;
  obj->width = width;
  obj->height = height;
  obj->entry_height = entry_height;
  
  obj->fg = fore;
  obj->bg = back;
  obj->bd = bd;
  obj->hi_fg = hi_fore;
  obj->hi_bg = hi_back;

  /*********************/
  /* Create the window */
  /*********************/

  attr.border_pixel = obj->bd;
  attr.background_pixel = obj->bg;
  attr.save_under = True;
  attr.override_redirect = True;

  mask = (CWBackPixel | CWBorderPixel | CWSaveUnder | CWOverrideRedirect);

  obj->window = XCreateWindow(display, RootWindow(display, screen),
			      x, y, width, height, MENU_BORDER, depth,
			      InputOutput, CopyFromParent, mask, &attr);

  /* Set the properties */

  hints.flags = PPosition | PSize; 
  hints.x = x;
  hints.y = y;
  hints.width = width;
  hints.height = height;
  XSetNormalHints( display, obj->window, &hints);

  mask = ButtonPressMask | ButtonReleaseMask | ExposureMask;
  XSelectInput(display, obj->window, mask);

  /* Create the GC */
  mask = 0;
  obj->gc = XCreateGC(display, obj->window, mask, &values);

  /***************/
  /* The Choices */
  /***************/

  obj->entries = (choice_window*) calloc(entry_count, sizeof(choice_window));

  for (k = 0; k < entry_count; k++) 
    open_choice_window(obj->entries+k, obj->window, &(obj->gc),
		       0, (k+obj->has_title)*(entry_height + CHOICE_BORDER),
		       width, entry_height + 2*CHOICE_BORDER, 
		       fore, back, hi_fore, hi_back,
		       entry_text[k], font, 
		       MENU_LEFT_MARGIN, k);
  if (index >= 0)
    obj->entries[index].selected = 1;

  XFlush(display);
}

int run_simple_menu( source, x, y )
     simple_menu *source;
     int         x, y;
  /* PRE:  'source' has been created.                                   */
  /* POST: Maps and operates a simple menu, and returns the index of    */
  /*       the entry which was selected, or -1 if none.  The 'index'    */
  /*       field of 'source' contains the new index.                    */
{
  XSizeHints     hints;
  XEvent         event;
  unsigned long  mask;
  int            k;

  /* Set the properties */
  XMoveWindow(display, source->window, x, y);

  hints.flags = PPosition | PSize; 
  hints.x = x;
  hints.y = y;
  hints.width = source->width;
  hints.height = source->height;
  XSetNormalHints( display, source->window, &hints);

  XMapRaised(display, source->window);
  XFlush(display);

  mask = EnterWindowMask | LeaveWindowMask |
    ButtonPressMask | ButtonReleaseMask;
  XGrabPointer(display, source->window, True, mask, 
	       GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

  do {
    XNextEvent(display, &event);
    for (k = 0; k < source->entry_count; k++) 
      if (event.xany.window == source->entries[k].window)
	handle_choice_event(source->entries+k, &event);

    if (event.xany.window == source->window) 
      switch (event.type) {
      case Expose :
	XSetForeground(display, source->gc, source->fg);
	position_string(source->window, source->gc, 
			source->width/2, MENU_MARGIN,
			0, 1, source->font, source->title, (char) 1);
	XDrawLine(display, source->window, source->gc, 
		  0, source->entry_height-1, 
		  source->width, source->entry_height-1);
	XFlush(display);
	break;
      }
  } while ((event.type != ButtonRelease) && (event.type != ButtonPress));

  XUngrabPointer(display, CurrentTime);
  XUnmapWindow(display, source->window);  
  XFlush(display);

  /* Now determine which option was chosen */
  for (k = 0; (k < source->entry_count); k++)
    if ((source->entries[k].pointer_in) && 
	(source->entries[k].text[0])) {
      source->entries[k].pointer_in = 0;
      source->entries[source->index].selected = 0;
      source->entries[k].selected = 1;
      source->index = k;

      return(k);
    }

  return(-1);
}
