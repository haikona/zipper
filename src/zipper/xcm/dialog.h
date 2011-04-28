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
/*********************/
/* Dialog Box Header */
/*********************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __DIALOG
#define __DIALOG

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "button.h"
#include "textbox.h"

#define DIALOG_TEXT_LENGTH 256
#define DIALOG_LABEL_LENGTH 64

#define DIALOG_MARGIN 8
#define DIALOG_BORDER 2
#define DIALOG_MIN_TEXT_BOX_WIDTH 20
#define DIALOG_LABEL_SEP 8

struct dialog_struct {
  Window  window;
  GC      gc;

  char    alive;

  XFontStruct *font;
  XFontStruct *button_font;
  
  char text[DIALOG_TEXT_LENGTH];
  int  lines;

  unsigned int width;
  unsigned int height;

  button_type button1;
  button_type button2;
  button_type button3;

  char        include_textbox;
  text_box    textbox;
  int         textbox_y;
  char        textbox_label[DIALOG_LABEL_LENGTH];

  unsigned long fg;   /* The foreground color */
  unsigned long bg;   /* The background color */
  unsigned long bd;   /* The border color     */

};

typedef struct dialog_struct dialog_window;

enum dialog_return { d_no_return = 0, d_button1, d_button2, d_button3,
		     d_return_key};

void initialize_dialog();
void create_dialog();
void create_simple_dialog();
void create_text_dialog();
void redraw_diaglog();
enum dialog_return handle_dialog_event();
enum dialog_return run_dialog();

#endif
