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
/***********************************/
/* Header for Program-Wide Globals */
/***********************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This header contains the declarations for the global     */
/*                 globals variables available to every module.  It is      */
/*                 included in every module which uses any of the X         */
/*                 library functions.                                       */
/****************************************************************************/

#ifndef __GLOBALS
#define __GLOBALS

#define NAME       "xcm"
#define CLASS_NAME "Xcm"

/* MAX_POINTS is the largest number allowed in each single block */
#define MAX_POINTS 1000

#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

#define PRIMARY_WIDTH  800
#define PRIMARY_HEIGHT 600

#define LEFT_MARGIN   100
#define RIGHT_MARGIN  90
#define TOP_MARGIN    20
#define BOTTOM_MARGIN 100

#define INNER_BORDER_FACTOR 0.55

#define OUT_OF_VIEW_FACTOR 10

#define MAX_COLORS 16

#define MAX_FILES 30

/* window object for complex functions */
struct cw_struct {
  Drawable drawable;  /* the window, in the general case. */
  GC       gc;
  int      width;
  int      height;
  double   x0, y0;
  double   x1, y1;
  void     *func;
  char     active;
  char     name[100];
};

typedef struct cw_struct complex_window;

/*********************************/
/* Program-Wide Global Variables */
/*********************************/

extern Display  *display;
extern int      screen;
extern int      depth;
extern Colormap colormap;

/* Fonts */

extern XFontStruct *PositionFont;
extern XFontStruct *ButtonFont;
extern XFontStruct *LabelFont;
extern XFontStruct *DialogFont;

/* global GC's, for various operations */

extern GC GrayGC;

#endif
