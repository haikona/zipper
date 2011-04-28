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
/****************************/
/* Basic X Window Functions */
/****************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* CREATION DATE:  March 31, 1993                                           */
/* PURPOSE:        This module contains the C functions for basic X         */
/*                 operations, such as opening windows, setting up the      */
/*                 connection to the server, etc.                           */
/****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "basic.h"

void InitX( display_name )
     char display_name[];
  /* POST: Tries to open a connection to the X server with hardware     */
  /*       display name 'display_name'.  If NULL is sent, the name will */
  /*       be the name of the DISPLAY environment variable.  The global */
  /*       variables 'display', 'screen' and 'depth' are all assigned   */
  /*       accordingly.  If the connection cannot be made, an error     */
  /*       message is printed to stderr, and exit(1) is called.         */
{
  /* Establish a connection to the X server */
  display = XOpenDisplay(display_name); 
  if (display == NULL) {
    fprintf(stderr, "ERROR: cannot establish connection.\n");
    exit(1);
  }

  screen = DefaultScreen(display);
  depth = DefaultDepth(display, screen);
}  

XFontStruct *load_font( name )
     char name[];
  /* POST: Tries to load the X Window font with name 'name' and returns */
  /*       the XFontStruct if successful.  Otherwise an error results   */
  /*       and the program exits with code 1.                           */
{
  XFontStruct *font;

  if ( (font = XLoadQueryFont(display, name)) == NULL ) {
    fprintf(stderr, "Unable to find font %s\n", name);
    exit(1);
  }
  return(font);
}
 
XFontStruct *load_font_with_alt( name, alternate1, alternate2 )
     char name[];
     char alternate1[];
     char alternate2[];
  /* POST: Tries to load the X Window font with name 'name', and if  */
  /*       unsuccessful, tries with 'alternate1' and 'alternate2'.   */
  /*       If none of the three are available, an error message is   */
  /*       printed and the program exits with code 1.                */
  /*       A message is printed to stderr if any of the fonts fail.  */
  /*       Both the alternates may be NULL, in which case no load is */
  /*       attempted.                                                */
{
  XFontStruct *font;

  if ( (font = XLoadQueryFont(display, name)) == NULL ) {
    fprintf(stderr, "Unable to find font '%s', trying '%s'\n",
	    name, alternate1);
    if ( (alternate1 != NULL) && 
	((font = XLoadQueryFont(display, alternate1)) == NULL ) ) {
      fprintf(stderr, "Unable to find font '%s', trying '%s'\n",
	      alternate1, alternate2);
      if ( (alternate2 != NULL) && 
	  ((font = XLoadQueryFont(display, alternate2)) == NULL ) ) {
	fprintf(stderr, "Unable to find font '%s'. Sorry.\n", alternate2);
	exit(1);
      }
    }
  }
  return(font);
}
      
Window OpenWindow(x, y, width, height, name)
     int     x, y;
     int     width, height;
     char    *name;
{
  XSetWindowAttributes attr;
  unsigned long        mask;
  Window               window;
  XSizeHints           hints;

  /* Set up the attributes for the new window */
  attr.border_pixel = WhitePixel( display, screen );
  attr.background_pixel =  BlackPixel( display, screen );
  attr.save_under = True;

  /* The mask indicates those attributes that have been specified. */
  mask = (CWBackPixel | CWBorderPixel | CWSaveUnder);

  /* Now create the window (but this won't make it appear on the screen.) */
  window = XCreateWindow( display, 
			  RootWindow(display, screen),
			  x, y,
			  width, height, 
			  2,
			  depth,
			  InputOutput,
			  CopyFromParent,
			  mask,
			  &attr );
  
  /* Set up the hints to send to the window manager about window. */
  hints.flags = PPosition | USSize; 
  hints.x = x;
  hints.y = y;
  hints.width = width;
  hints.height = height;

  /* Send the hints to the window manager. */
  XSetNormalHints( display, window, &hints);

  /* Give the window a name (from p. 48, AXAP) */
  XStoreName(display, window, name);

  /* Now map the window to the screen, i.e. make it appear. */
  XMapRaised(display, window);
  
  /* Flush out all the queued requests to the server. */
  XFlush(display); 

  /* Wait for the window to be mapped. */
/*  wait_for_map(window); */

  return(window);
}

Window OpenSubwindow( parent, x, y, width, height, border_width,
		      border, background, gravity, event_mask )
     Window        parent;
     int           x, y;
     unsigned int  width, height;
     unsigned int  border_width;
     unsigned long border;
     unsigned long background;
     int           gravity;
     unsigned long event_mask;
{
  Window               window;
  XSizeHints           hints;
  unsigned long        mask;
  XSetWindowAttributes attr;

  /* Now create the window (but this won't make it appear on the screen.) */
  window = XCreateSimpleWindow(display, 
			       parent,
			       x, y,
			       width, height, 
			       border_width,
			       border, background);
  
  /* change the attributes */
  mask = CWWinGravity;
  attr.win_gravity = gravity;
  XChangeWindowAttributes(display, window, mask, &attr);

  XSelectInput(display, window, event_mask);

  /* Set up the hints to send to the window manager about window. */
  hints.flags = PPosition | USSize; 
  hints.x = x;
  hints.y = y;
  hints.width = width;
  hints.height = height;

  /* Send the hints to the window manager. */
  XSetNormalHints( display, window, &hints);

  /* Now map the window to the screen, i.e. make it appear. */
  XMapRaised(display, window);
  
  /* Flush out all the queued requests to the server. */
  XFlush(display); 

  return(window);
}

void center_string( drawable, gc, x, y, font, string )
     Drawable drawable;
     GC       gc;
     int      x, y;
     XFontStruct *font;
     char     *string;
  /* PRE  : font is assigned, including the ascent, descent, and width */
  /*        fields.                                                    */
  /* POST : centers string horizontally and vertically at (x,y).       */
{
  int length, width;

  length = strlen(string);
  width = XTextWidth(font, string, length);

  XSetFont(display, gc, font->fid);
  XDrawString(display, drawable, gc, 
	      x - width/2, y + (font->ascent - font->descent)/2,
	      string, length);
}

void right_center_text( drawable, gc, x, y, font, string )
     Drawable drawable;
     GC       gc;
     int      x, y;
     XFontStruct *font;
     char     *string;
{
  int length, width;

  length = strlen(string);
  width = XTextWidth(font, string, length);

  XSetFont(display, gc, font->fid);
  XDrawImageString(display, drawable, gc, 
	      x - width, y + (font->ascent - font->descent)/2,
	      string, length);
}

void position_string( drawable, gc, x, y, x_pos, y_pos, font, string, new_font)
     Drawable    drawable;
     GC          gc;
     int         x, y;
     int         x_pos, y_pos;
     XFontStruct *font;
     char        *string;
     char        new_font;
  /* POST: Draws 'string' at (x,y) with positioning specified by 'x_pos' */
  /*       and 'y_pos'.  x_pos = -1, y_pos = -1 positions the text so    */
  /*       that (x,y) is the upper-left corner, (0,0) centers the text,  */
  /*       etc.  (0,1) centers the text horizontally at the baseline.    */
  /*       If 'new_font' is True (nonzero) the font of GC is changed,    */
  /*       otherwise it isn't.                                           */
{
  int length, width;
  int x0, y0;

  length = strlen(string);
  width = XTextWidth(font, string, length);
  
  if (new_font)
    XSetFont(display, gc, font->fid);

  /* Calculate the relative coordiantes (x0, y0) */
  x0 = -width/2*(x_pos + 1);
  if (y_pos > 0)
    y0 = font->ascent;
  else if (y_pos == 0)
    y0 = font->ascent/2; /*(font->ascent - font->descent)/2;*/
  else
    y0 = 0;

  XDrawString(display, drawable, gc, x + x0, y + y0, string, length);
}


char collect_motions( event )
     XEvent *event;
  /* POST : waits for an event (if there are none queued), then skips   */
  /*        through a series of contiguous MotionNotify events, placing */
  /*        the last one into event.  
  /*        Returns False if there are no MotionNotify events.          */
{
  static XEvent event2;

  XPeekEvent(display, &event2);
  if (event2.type != MotionNotify) {
    return(False);
  }
  while (event2.type == MotionNotify) {
    XNextEvent(display, event);
    XFlush(display);
    if (XPending(display) == 0) {
      return(True);
    }
    XPeekEvent(display, &event2);
  }
  return(True);
}


Pixmap create_gray_stipple( drawable ) 
     Drawable drawable;
  /* POST: Returns a bitmap which is the best stipple for the server. */
{
  Pixmap        gray_stipple;
  unsigned int  width, height;
  char          first_row = 0x55;
  char          last_byte_mask;
  int           full_bytes, bits_remaining;
  int           bytes;
  int           x, y;
  char          *bits;

  if (!XQueryBestStipple(display, drawable, 8, 8, &width, &height)) {
    /* The call failed, use 8x8 */
    width = 8;
    height = 8;
  }
  
  full_bytes = width / 8;
  bits_remaining = width % 8;

  last_byte_mask = ~(~0 >> bits_remaining);
  bytes = full_bytes + MIN(bits_remaining, 1);
  
  bits = (char*) calloc(bytes*height, sizeof(char));
  for (y = 0; y < height; y++) 
    for (x = 0; x < bytes; x++) 
      bits[y*bytes + x] = first_row * (1 + (y % 2));

  /* Chop off the extra bits */
  if (bits_remaining > 0)
    for (y = bytes-1; y < height*bytes; y += bytes)
      bits[y] = bits[y] & last_byte_mask;

  gray_stipple = XCreateBitmapFromData(display, drawable, bits, width, height);

  free(bits);
  return(gray_stipple);
}

