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
/*********************************/
/* Module for the Textbox Object */
/*********************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module contains the C functions for creating and    */
/*                 operating a texbox window.  A textbox window is a simple */
/*                 window which serves as a line editor for a single line   */
/*                 of text.  The object was created to be part of a dialog  */
/*                 box.                                                     */
/****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"
#include "basic.h"
#include "textbox.h"

text_box create_text_box( parent, x, y, width, height, gc, fg, bg, bd,
			  font, text, gravity)
     Window        parent;
     int           x, y;
     unsigned int  width, height;
     GC            *gc;
     unsigned long fg, bg, bd;
     XFontStruct   *font;
     char          *text;
     int           gravity;
  /* POST: creates and maps a new "text box" window with the specified */
  /*       parameters.                                                 */
{
  unsigned long  event_mask;
  text_box       value;

  value.parent = parent;
  value.font = font;
  value.gc = gc;
  value.x = x;
  value.y = y;
  value.width = width;
  value.height = height;
  value.fg = fg;
  value.bg = bg;
  value.bd = bd;
  strcpy(value.text, text);
  strcpy(value.text0, text);
  value.cursor = strlen(text);
  value.cursor0 = value.cursor;
  value.starting_pos = 0;
  value.visible = 1;

  event_mask = StructureNotifyMask | ExposureMask | EnterWindowMask | 
               LeaveWindowMask | KeyPressMask;
  
  value.window = OpenSubwindow(parent, x, y, width, height, 
			       TEXT_BOX_BORDER_WIDTH, bd, bg, 
			       gravity, event_mask);
  XFlush(display);
  return(value);
}

void map_text_box( obj )
     text_box *obj;
  /* PRE : the 'x', 'y', 'width' and 'height' fields of 'obj' are */
  /*       assigned.
  /* POST: Maps 'obj->window'.  This function may be called after its */
  /*       parent has been resized.                                   */
{
  XSizeHints    hints;

  hints.flags = PPosition | PSize; 
  hints.x = obj->x;
  hints.y = obj->y;
  hints.width = obj->width;
  hints.height = obj->height;
  XSetNormalHints( display, obj->window, &hints);

  if ((obj->width > 0) || (obj->height > 0)) {
    XResizeWindow(display, obj->window, obj->width, obj->height);
    XMapRaised(display, obj->window);
  }
}


void cursor_text_pos( obj, cursor_x, char_x )
     text_box *obj;
     int      *cursor_x; /* RETURN */
     int      *char_x;   /* RETURN */
  /* POST: 'cursor_x' and 'char_x' contain the physical positions of the */
  /*       cursor, and the last character referenced by the cursor.      */
{
  *cursor_x = TEXT_BOX_MARGIN + 
    XTextWidth(obj->font, obj->text + obj->starting_pos,
	       obj->cursor - obj->starting_pos);
  
  if (obj->text[obj->cursor])
    *char_x = *cursor_x + XTextWidth(obj->font, obj->text + obj->cursor, 1);
  else
    *char_x = *cursor_x;
}  

int text_pos( obj, pos )
     text_box *obj;
     int      pos;
  /* POST: Returns the physical position of the character at 'pos' */
{
  int x;

  x = TEXT_BOX_MARGIN + 
    XTextWidth(obj->font, obj->text + obj->starting_pos,
	       pos - obj->starting_pos);
  return(x);
}
    
char adjust_starting_pos( obj )
     text_box *obj;
  /* PRE : The 'text' and 'cursor' fields of 'obj' are properly assigned.  */
  /* POST: adjusts the 'starting_pos' field of 'obj' so that the cursor as */
  /*       well as the character it references appears in the window.      */
  /*       If the cursor is off the right side of the window, the new      */
  /*       starting position will be such that the character of cursor     */
  /*       is the last (complete) character in the box.                    */
  /*       If the cursor is already visible, True is returned.             */
{
  int cursor_x;
  int char_x;

  /* If the cursor is off the left side, move left until it appears */
  if (obj->cursor < obj->starting_pos) {
    obj->starting_pos = MAX(0, obj->cursor-1);
    return(0);
  }

  /* Find the position of the cursor according to the current window */
  cursor_text_pos(obj, &cursor_x, &char_x);
  if (char_x <= obj->width - TEXT_BOX_MARGIN)
    return(1);

  /* A while loop is a bit wasteful here, but really it should only loop */
  /* a couple of times in most cases.  The text shouldn't bee too long   */
  /* anyway.                                                             */

  while(char_x > obj->width - TEXT_BOX_MARGIN) {
    (obj->starting_pos)++;
    cursor_text_pos(obj, &cursor_x, &char_x);
  }
  return(0);
}
  
void draw_text_cursor( obj ) 
     text_box *obj;
  /* POST: Draws the text cursor of 'obj', and set the 'cursor_x0' field */
  /*       of 'obj' to the position at which the cursor is drawn.        */
{
  int cursor_x, char_x;

  XSetForeground(display, *(obj->gc), obj->fg);
  cursor_text_pos(obj, &cursor_x, &char_x);
  XDrawRectangle(display, obj->window, *(obj->gc), 
		 cursor_x, (obj->height - obj->font->ascent)/2,
		 1, obj->font->ascent + obj->font->descent);
  obj->cursor_x0 = cursor_x;
  obj->cursor_char0 = obj->text[obj->cursor];
}

void erase_text_cursor( obj )
     text_box *obj;
  /* POST: Erases the text cursor, and repairs any damage.  The cursor is */
  /*       erased at the position 'cursor0'.                              */
{
  char character[2];

  XSetForeground(display, *(obj->gc), obj->bg);
  XDrawRectangle(display, obj->window, *(obj->gc), 
		 obj->cursor_x0, (obj->height - obj->font->ascent)/2,
		 1, obj->font->ascent + obj->font->descent);

  XSetForeground(display, *(obj->gc), obj->fg);
  if (obj->cursor_char0 != '\0') {
    XSetFont(display, *(obj->gc), obj->font->fid);
    character[0] = obj->cursor_char0;
    character[1] = '\0';
    position_string(obj->window, *(obj->gc), obj->cursor_x0, obj->height/2,
		    -1, 0, obj->font, character, 1);
  }
}
  
void redraw_text_box( obj )
     text_box *obj;
  /* POST: This should only be called in case of an Exposure event, or a */
  /*       total redraw.  update_text_box is the general function.       */
{
  XSetForeground(display, *(obj->gc), obj->fg);
  XClearWindow(display, obj->window);
  position_string(obj->window, *(obj->gc), TEXT_BOX_MARGIN, obj->height/2,
		  -1, 0, obj->font, obj->text + obj->starting_pos, 1);
  draw_text_cursor(obj);
  XFlush(display);
}

void update_text_box( obj )
     text_box *obj;
  /* POST: redraws the text in 'obj' according to the previous draw, which */
  /*       is specified in the fields 'text0' and 'cursor0'.               */
{
  int k, x;

  erase_text_cursor(obj);
  
  /* If the starting position of the text has changed, i.e. the cursor has */
  /* moved off the window, redraw the entire text.                         */
  
  if (!adjust_starting_pos(obj)) {
    redraw_text_box(obj);
    return;
  }
  else {

    /* determine how much of the text needs to be redrawn */
    if (strcmp(obj->text, obj->text0)) {

      /* Find the first different character (there is one!) */
      for (k = 0; obj->text[k] == obj->text0[k]; k++);

      XSetFont(display, *(obj->gc), obj->font->fid);
      XSetForeground(display, *(obj->gc), obj->fg);

      x = text_pos(obj, k);
      XClearArea(display, obj->window, 
		 x, 0, 
		 /*XTextWidth(obj->font, obj->text+k, strlen(obj->text+k)),*/
		 obj->width, obj->height,
		 False);
      position_string(obj->window, *(obj->gc), x, obj->height/2, -1, 0,
		      obj->font, obj->text+k, 0);
    }
    draw_text_cursor(obj);
  }
}

char add_text_box_char( obj, sym )
     text_box *obj;
     KeySym    sym;
  /* POST: Inserts the letter 'sym' at the cursor position in 'obj' and */
  /*       increments the cursor.  If the cursor references the last    */
  /*       possible cell of 'obj->text', the character isn't            */
  /*       inserted, the bell is rung, and False is returned.           */
{
  size_t count;

  if ((obj->cursor >= MAX_TEXT_SIZE-1) || 
      (strlen(obj->text) >= MAX_TEXT_SIZE-1)) {
    XBell(display, 10);
    return(0);
  }
  else {
    count = strlen(obj->text) - obj->cursor + 1;
    memcpy(obj->text + obj->cursor+1, obj->text + obj->cursor, count);

    /* ASSERT: The null-terminator is moved as well */
    obj->text[obj->cursor] = sym;
    (obj->cursor)++;
    return(1);
  }
}

char parse_text_box_key( obj, event )
     text_box *obj;
     XEvent   *event;
  /* PRE : 'event' is a KeyPress event, and the appropriate fields of 'obj' */
  /*       are properly defined.                                            */
  /* POST: Interprets the keystroke(s) in 'event', and adjusts 'obj->text'  */
  /*       and 'obj->cursor' according to the keystroke.  'obj->text0' and  */
  /*       'obj->cursor0' contain the text and cursor position at the time  */
  /*       of the function call.                                            */
  /*       True is returned if the "return" key was pressed.                */
{
  int    length;
  int    buffer_max = 64;
  char   key_buffer[65];
  KeySym sym;
  XComposeStatus status;
  int    count;

/* correction by D. Marshall 5-95*/
/*length = XLookupString(event, key_buffer, buffer_max, &sym, &status);*/
  length = XLookupString(&(event->xkey), key_buffer, buffer_max, &sym, &status);

  strcpy(obj->text0, obj->text);
  obj->cursor0 = obj->cursor;

  /* Check for a normal keystroke */
  
  if ((sym >= ' ') && (sym <= '~'))
    add_text_box_char(obj, sym);

  /* Check for special keys */

  count = strlen(obj->text) - obj->cursor + 1;

  if (sym == XK_BackSpace) {  /* Backspace */
    if (obj->cursor > 0) {
      (obj->cursor)--;
      memcpy(obj->text+obj->cursor, obj->text+obj->cursor+1, count);
    }
    else
      XBell(display, 10);
  }

  if (sym == XK_Delete) {    /* Delete */ 
    if (obj->text[obj->cursor] != '\0') {
      memcpy(obj->text+obj->cursor, obj->text+obj->cursor+1, count-1);
    }
    else
      XBell(display, 10);
  }

  if (sym == XK_Left)        /* Left Arrow */
    obj->cursor = MAX(0, obj->cursor-1);

  if (sym == XK_Right)       /* Right Arrow */
    obj->cursor = MIN(obj->cursor+1, strlen(obj->text));

/*  if (sym == XK_Home)      
    obj->cursor = 0;

  if (sym == XK_Home)        
    obj->cursor = strlen(obj->text); */

  if (sym == XK_Return)      /* Return */
    return(1);

  update_text_box(obj);
  return(0);
}
    

char handle_text_box_event( obj, event )
     text_box *obj;
     XEvent   *event;
  /* POST: True if the Return key was seen */
{
/*printf("Handling %s event in text box\n", event_name[event->type]);*/
  switch (event->type) 
    {
    case KeyPress :
      return(parse_text_box_key(obj, event));
      break;

    case Expose :
      redraw_text_box(obj);
      break; 

    }
  return(0);
}

  
