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
/* Screen Button Header */
/************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __BUTTON
#define __BUTTON

#define BUTTON_BORDER_WIDTH    1
#define BUTTON_TEXT_SIZE      24

#define BUTTON_SIDE_MARGIN  8
#define BUTTON_TOP_MARGIN   4

#define BUTTON_INNER_MARGIN  4
#define BUTTON_OUTER_MARGIN 10
#define BUTTON_SPACE        10

enum button_status { button_invisible = 0, button_inactive, 
		     button_active, button_pressed };

struct button_struct {
  Window        window;   /* The window identity */
  Window        parent;   /* The parent window */

  char          text[BUTTON_TEXT_SIZE];  /* The visual text */

  unsigned int  width;    /* The apparent width, here for convenience */
  unsigned int  height;   /* The apparent height */

  GC            *gc;      /* The GC of the parent window */
  XFontStruct   *font;    /* The font for the button text */

  unsigned long foreground;  /* the pixel for the foreground */
  unsigned long background;  /* the pixel for the background */

  unsigned long hi_fore;   /* the pixel for the highlighted foreground */
  unsigned long hi_back;   /* the pixel for the highlighted background */

  enum button_status  status; /* indicates the status of the button */

  void          (*apply)(void *);  /* C function to exectute when pressed */
};

typedef struct button_struct button_type;

void no_op();
void create_button();
void handle_button_event();
void activate_button();
void deactivate_button();

#endif
