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
/* Header for Pull-Down Menus */
/******************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __MENU
#define __MENU

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define CHOICE_BORDER       0
#define MENU_BORDER         2
#define MENU_LEFT_MARGIN   20
#define MENU_MARGIN         4
#define MENU_MARK_SIZE      8

#define MENU_ENTRY_LENGTH  64

typedef struct choice_struct {
  Window   parent;
  Window   window;
  GC       *gc;
  Drawable drawable;
  
  unsigned int width, height;
  int          x, y;

  unsigned long foreground, background;
  unsigned long hi_fore, hi_back;

  char        text[MENU_ENTRY_LENGTH];
  XFontStruct *font;
  int         left_margin;
  int         index;

  char        pointer_in;
  char        selected;

} choice_window;


typedef struct simple_menu_struct {
  Window  window;
  GC      gc;

  unsigned int width;
  unsigned int height;
  int          entry_height;


  int      x, y;

  XFontStruct *font;

  unsigned long  fg, bg, bd;
  unsigned long  hi_fg, hi_bg;

  char           title[256];
  int            has_title;
  choice_window  *entries;
  int            entry_count;

  int            index;
} simple_menu;

void redraw_choice_window();
char handle_choice_event();
void open_choice_window();
void create_simple_menu();
int run_simple_menu();

#endif
