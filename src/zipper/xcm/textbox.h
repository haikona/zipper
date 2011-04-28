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
/* Header for the TextBox Object */
/*********************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __TEXTBOX
#define __TEXTBOX

#define MAX_TEXT_SIZE 256
#define TEXT_BOX_MARGIN 10
#define TEXT_BOX_BORDER_WIDTH 1

typedef struct text_box_struct {
  Window       window;
  Window       parent;

  char         visible;

  GC           *gc;
  XFontStruct  *font;

  int          width, height;
  int          x, y;

  unsigned long fg, bg, bd;
  
  char         text[MAX_TEXT_SIZE];
  char         text0[MAX_TEXT_SIZE];

  int          cursor;
  int          cursor0;

  char         cursor_char0;  /* The damaged character */
  int          cursor_x0;     /* The old physical cursor position */

  int          starting_pos;
} text_box;
  
text_box create_text_box();
char handle_text_box_event();

#endif
