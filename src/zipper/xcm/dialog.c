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
/* Dialog Window Object */
/************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module contains the C functions for the creation    */
/*                 and operation of dialog boxes, which are transient       */
/*                 windows used for brief communication with the user.      */
/****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "basic.h"
#include "button.h"
#include "textbox.h"
#include "dialog.h"

/* Variables global to this module */

static unsigned long DialogFG;
static unsigned long DialogBG;
static unsigned long DialogBD;
static unsigned long DialogHiFG;
static unsigned long DialogHiBG;

static XFontStruct *DialogButtonFont;


/***********************/
/* Dialog Window Stuff */
/***********************/

void initialize_dialog( fg, bg, bd, hi_fg, hi_bg, font, button_font )
     unsigned long  fg, bg, bd;
     unsigned long  hi_fg, hi_bg;
     XFontStruct    *font, *button_font;
  /* POST: Initializes the dialog globals above, with the specified      */
  /*       parameters.                                                   */
  /* NOTE: This function must be called before any of the "simple"       */
  /*       dialog window functions are used, but isn't necessary         */
  /*       if the dialog window are created with 'create_dialog_window'. */
{
  DialogFG = fg;
  DialogBG = bg;
  DialogBD = bd;

  DialogHiFG = hi_fg;
  DialogHiBG = hi_bg;  

  DialogFont = font;
  DialogButtonFont = button_font;
}

int line_count( source )
     char source[];
  /* POST: Returns the number of carriage returns in source plus one. */
{
  int k = 0;
  char *pos;

  pos = source;
  while ( (pos = strchr(pos, '\n')) != NULL ) {
    k++;
    pos++;
  }

  return(k+1);
}

char *eol( source )
     char source[];
  /* POST: Returns the position of the next newline character or null */
  /*       terminator.                                                */
{
  char *pos;

  for (pos = source; ((*pos != '\n') && (*pos != '\0')); pos++);
  return(pos);
}
  
void create_dialog( obj, x, y,
		    width, height, fg, bg, bd, hi_fg, hi_bg,
		    font, button_font,
		    main_text, button1_text, button2_text, button3_text,
		    include_text_box, text_box_label, text_box_text,
		    dialog_name)

     dialog_window  *obj;
     int            x, y;
     unsigned int   width, height;
     unsigned long  fg, bg, bd;
     unsigned long  hi_fg, hi_bg;
     XFontStruct    *font;
     XFontStruct    *button_font;
     char           *main_text;
     char           *button1_text, *button2_text, *button3_text;
     char           include_text_box;
     char           *text_box_label;
     char           *text_box_text;
     char           *dialog_name;
  /* POST: Creates a dialog_window and assigns it to 'obj'.  The actual */
  /*       width and height of the window will be large enough to hold  */
  /*       the message text in 'main_text' and all the buttons.         */
{
  XSetWindowAttributes attr;
  unsigned long   mask;
  XGCValues       values;
  XSizeHints      hints;
  int             text_height, button_height, total_height;
  int             button1_width = 0;
  int             button2_width = 0;
  int             button3_width = 0;
  int             width1 = 0, width2, width3;
  int             label_width;
  char            *pos, *pos0;

  obj->font = font;
  obj->button_font = button_font;

  obj->width = width;
  obj->height = width;
  obj->fg = fg;
  obj->bg = bg;
  obj->bd = bd;

  strncpy(obj->text, main_text, DIALOG_TEXT_LENGTH);
  obj->text[DIALOG_TEXT_LENGTH] = '\0';
  obj->lines = line_count(obj->text);

  strncpy(obj->textbox_label, text_box_label, DIALOG_LABEL_LENGTH);
  obj->textbox_label[DIALOG_LABEL_LENGTH] = '\0';

  /* Determine the width of each button */
  if ((button1_text != NULL) && (strlen(button1_text) > 0))
    button1_width = 
      XTextWidth(button_font, button1_text, strlen(button1_text)) +
	2*BUTTON_SIDE_MARGIN;

  if ((button2_text != NULL) && (strlen(button2_text) > 0))
    button2_width = 
      XTextWidth(button_font, button2_text, strlen(button2_text)) +
	2*BUTTON_SIDE_MARGIN;

  if ((button3_text != NULL) && (strlen(button3_text) > 0))
    button3_width = 
      XTextWidth(button_font, button3_text, strlen(button3_text)) +
	2*BUTTON_SIDE_MARGIN;

  /* Find the total width of the buttons, including the space between */
  width2 = button1_width + button2_width + button3_width;
  width2 += (1 + MIN(1, button1_width) + MIN(1, button2_width) + 
    MIN(1, button3_width))*DIALOG_MARGIN;

  /* Find the maximum width of the lines of text */
  pos = obj->text;
  pos0 = pos;
  while (*pos != '\0') {
    pos = eol(pos0);
    width1 = MAX(width1, XTextWidth(obj->font, pos0, pos - pos0));
    pos0 = pos+1;
  }
  width1 += 2*DIALOG_MARGIN;

  /* Find the width of the text box with its label */
  if ((text_box_label != NULL) && (strlen(text_box_label) > 0)) {
    label_width = XTextWidth(font, text_box_label, strlen(text_box_label))
      + DIALOG_LABEL_SEP;
    width3 = label_width + DIALOG_MIN_TEXT_BOX_WIDTH;
  }
  else {
    label_width = 0;
    width3 = DIALOG_MIN_TEXT_BOX_WIDTH;
  }
  width3 += 2*DIALOG_MARGIN;


  /* Determine the width of the window */

  width = MAX(MAX(width, width3), MAX(width1, width2));

  /* Determine the height of the window */

  text_height = obj->lines*(font->ascent + font->descent) + DIALOG_MARGIN;
  button_height = (button_font->ascent + button_font->descent) + 
    2*BUTTON_TOP_MARGIN;
  total_height = text_height + button_height + 2*DIALOG_MARGIN;
  if (include_text_box)
    total_height += DIALOG_MARGIN + button_height;

  height = MAX(height, total_height);

  obj->width = width;
  obj->height = height;


  /* Set up the attributes */
  attr.border_pixel = obj->bd;
  attr.background_pixel = obj->bg;
  attr.save_under = True;
  attr.override_redirect = False;

  /* The mask indicates those attributes that have been specified. */
  mask = (CWBackPixel | CWBorderPixel | CWSaveUnder | CWOverrideRedirect);

  /*************************************/  
  /* Create and Open the Dialog Window */
  /*************************************/  

  obj->window = XCreateWindow(display, RootWindow(display, screen),
			      x, y, width, height, DIALOG_BORDER, depth,
			      InputOutput, CopyFromParent, mask, &attr);
  
  XStoreName(display, obj->window, dialog_name);

  hints.flags = PPosition | PSize; 
  hints.x = x;
  hints.y = y;
  hints.width = width;
  hints.height = obj->height;
  XSetNormalHints( display, obj->window, &hints);

  mask = StructureNotifyMask | ExposureMask | KeyPressMask;
  XSelectInput(display, obj->window, mask);

  /* Map the window */
  XMapRaised(display, obj->window);

  /* Create the GC */
  mask = 0;
  obj->gc = XCreateGC(display, obj->window, mask, &values);


  /***************/
  /* The Buttons */
  /***************/

  if (button1_width > 0) 
    create_button(&(obj->button1), obj->window, button1_text, &(obj->gc),
		  button_font,
		  DIALOG_MARGIN, height - DIALOG_MARGIN - button_height,
		  button1_width, button_height,
		  bd, fg, bg, hi_fg, hi_bg, button_active, SouthWestGravity);

  if (button2_width > 0) 
    create_button(&(obj->button2), obj->window, button2_text, &(obj->gc),
		  button_font,
		  (button1_width + width - button3_width - button2_width)/2, 
		  height - DIALOG_MARGIN - button_height,
		  button2_width, button_height,
		  bd, fg, bg, hi_fg, hi_bg, button_active, SouthGravity);

  if (button3_width > 0) 
    create_button(&(obj->button3), obj->window, button3_text, &(obj->gc),
		  button_font,
		  width - DIALOG_MARGIN - button3_width, 
		  height - DIALOG_MARGIN - button_height,
		  button3_width, button_height,
		  bd, fg, bg, hi_fg, hi_bg, button_active, SouthEastGravity);

  obj->button1.apply = no_op;
  obj->button2.apply = no_op;
  obj->button3.apply = no_op;

  /*******************/
  /* The Text Window */
  /*******************/

  if (include_text_box) {
    obj->textbox = create_text_box(obj->window, 
				   DIALOG_MARGIN + label_width, 
				   text_height + DIALOG_MARGIN,
				   width - 2*DIALOG_MARGIN - label_width, 
				   button_height,
				   &(obj->gc),
				   fg, bg, bd, button_font, 
				   text_box_text, WestGravity);
    obj->textbox_y = text_height + DIALOG_MARGIN + 
      button_height/2 + font->ascent/2;
    obj->include_textbox = 1;
  }

  obj->alive = 1;

  XFlush(display);
}

void create_simple_dialog( obj, x, y, 
			   message,
			   button1_text, button2_text, button3_text,
			   dialog_name)
     dialog_window *obj;
     int           x, y;
     char          *message;
     char          *button1_text, *button2_text, *button3_text;
     char          *dialog_name;
  /* PRE : 'intialize_dialog' has been called.                       */
  /* POST: Creates a simple dialog box, with the global parameters   */
  /*       specified at the beginning of this file.                  */
  /*       The width and height are calculated from the sizes of the */
  /*       message text and the buttons.                             */
{
  create_dialog(obj, x, y, (unsigned int) 0, (unsigned int) 0, 
		DialogFG, DialogBG, DialogBD, DialogHiFG, DialogHiBG,
		DialogFont, DialogButtonFont, message,
		button1_text, button2_text, button3_text,
		(char) 0, "", "", dialog_name);
}

void create_simple_text_dialog( obj, x, y, 
			        message, textbox_label, text,
			        button1_text, button2_text, button3_text,
			        name)
     dialog_window *obj;
     int           x, y;
     char          *message;
     char          *textbox_label;
     char          *text;
     char          *button1_text, *button2_text, *button3_text;
     char          *name;
  /* PRE : 'initialize_dialog' has been called.                           */
  /* POST: Creates a dialog box, with the global parameters specified     */
  /*       at the beginning of this file.  The width and height are       */
  /*       calculated from the sizes of the message text and the buttons. */
{
  create_dialog(obj, x, y, (unsigned int) 0, (unsigned int) 0, 
		DialogFG, DialogBG, DialogBD, DialogHiFG, DialogHiBG,
		DialogFont, DialogButtonFont, message,
		button1_text, button2_text, button3_text,
		(char) 1, textbox_label, text, name);
}

void destroy_dialog( obj )
     dialog_window *obj;
  /* POST: Destroys 'obj->window' and all the subwindows thereof */
{
  XDestroyWindow(display, obj->window);
  obj->alive = 0;
  XFlush(display);
}
  
void redraw_dialog_window( obj ) 
     dialog_window *obj;
  /* POST: redraws the message text in the dialog window.  The window is */
  /*       not cleared.                                                  */
{
  char *pos, *pos0;
  int  y;

  pos = obj->text;
  pos0 = pos;

  XSetFont(display, obj->gc, obj->font->fid);

  y = DIALOG_MARGIN + obj->font->ascent;

  /* If there is just one line of text, center the line */
  if (obj->lines == 1) 
    position_string(obj->window, obj->gc, obj->width/2, y, 0, -1,
		    obj->font, obj->text, (char) 0);

  else {
    while (*pos != '\0') {
      pos = eol(pos0);
      XDrawString(display, obj->window, obj->gc, 
		  (int) DIALOG_MARGIN, y, pos0, pos - pos0);
      
      y += obj->font->ascent + obj->font->descent;
      pos0 = pos+1;
    }
  }

  /* If the text box is labeled, draw the label */
  if ((obj->include_textbox) && (strlen(obj->textbox_label) > 0))
    XDrawString(display, obj->window, obj->gc,
		(int) DIALOG_MARGIN, obj->textbox_y,
		obj->textbox_label, strlen(obj->textbox_label));

/*XDrawRectangle(display, obj->window, obj->gc, 
	       DIALOG_MARGIN, DIALOG_MARGIN, 
	       obj->width - 2*DIALOG_MARGIN, obj->height - 2*DIALOG_MARGIN);
XDrawLine(display, obj->window, obj->gc, obj->width/2, 0, obj->width/2, obj->height);*/

  XFlush(display);
}

void resize_dialog( obj, width, height )
     dialog_window *obj;
     int           width, height;
{
  obj->textbox.width += (width - obj->width);
  obj->textbox_y += (height - obj->height)/2;
  obj->width = width;
  obj->height = height;
  map_text_box(&(obj->textbox));
}

enum dialog_return handle_dialog_event( obj, event )
     dialog_window *obj;
     XEvent        *event;
  /* POST: Checks to see if 'event' contains an event in 'obj' and     */
  /*       handles it accordingly.  The return value indicates whether */
  /*       a screen button was pressed, or they return key was presed. */
{
  if (!obj->alive)
    return(d_no_return);

  if (event->xany.window == obj->textbox.window) 
    if (handle_text_box_event(&(obj->textbox), event))
      return(d_return_key);

  if (event->xany.window == obj->button1.window)
    handle_button_event(&(obj->button1), event);
  if (event->xany.window == obj->button2.window)
    handle_button_event(&(obj->button2), event);
  if (event->xany.window == obj->button3.window)
    handle_button_event(&(obj->button3), event);

  if (event->xany.window == obj->window)
    switch(event->type) {

    case ConfigureNotify :
      resize_dialog(obj, event->xconfigure.width, event->xconfigure.height);

    case Expose :
      redraw_dialog_window(obj);
      break;

    case KeyPress :
      if (parse_text_box_key( &(obj->textbox), event))
	return(d_return_key);
    }

  /* I really shouldn't do this */

  if (obj->button1.status == button_pressed)
    return(d_button1);
  if (obj->button2.status == button_pressed)
    return(d_button2);
  if (obj->button3.status == button_pressed)
/*  Error Marshall found 4-95   return(d_button2);*/
    return(d_button3);

  return(d_no_return);
}

enum dialog_return run_dialog( obj )
     dialog_window *obj;
{
  unsigned long      mask;
  enum dialog_return value;
  XEvent             event;

/*  mask = EnterWindowMask | LeaveWindowMask |
    ButtonPressMask | ButtonReleaseMask;
  XGrabPointer(display, obj->window, True, mask, 
	       GrabModeAsync, GrabModeAsync, None, None, CurrentTime);*/

  mask = EnterWindowMask | LeaveWindowMask | StructureNotifyMask |
    KeyPressMask | ButtonPressMask | ExposureMask;
  do {
/*    if (XCheckWindowEvent(display, obj->button1.window, mask, &event)) {
      XWindowEvent(display, obj->button1.window, mask, &event);
      value = handle_dialog_event(obj, &event);
    }
    else if (XCheckWindowEvent(display, obj->button2.window, mask, &event)) {
      XWindowEvent(display, obj->button2.window, mask, &event);
      value = handle_dialog_event(obj, &event);
    }
    else if (XCheckWindowEvent(display, obj->button3.window, mask, &event)) {
      XWindowEvent(display, obj->button3.window, mask, &event);
      value = handle_dialog_event(obj, &event);
    }
    else if (XCheckWindowEvent(display, obj->textbox.window, mask, &event)) {
      XWindowEvent(display, obj->textbox.window, mask, &event);
      value = handle_dialog_event(obj, &event);
    }

    else {      
      XWindowEvent(display, obj->window, mask, &event);
      value = handle_dialog_event(obj, &event);
    }*/

    XNextEvent(display, &event);
    value = handle_dialog_event(obj, &event);
  } while (value == d_no_return);
  destroy_dialog(obj);

/*  XUngrabPointer(display, CurrentTime);*/
  XFlush(display);

  return(value);
}
     
