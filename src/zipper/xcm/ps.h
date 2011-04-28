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
/********************************/
/* Header for PostScript output */
/********************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef __PS
#define __PS

#define TIMES_CHAR_WIDTH 0.5

#define PS_CENTER_X 306.0
#define PS_CENTER_Y 396.0
#define PS_INCH      72.0
#define PS_WIDTH    540.0
#define PS_HEIGHT   720.0
#define PS_MARGIN    36.0

#define PS_FONT       "Times-Roman"
#define PS_FONT_SCALE 15.0

#define PS_LINE_WIDTH   0.01
#define PS_POINT_WIDTH  1.0

#define PS_GRID_WIDTH    0.01
#define PS_GRID_DASH     2.0
#define PS_SUBGRID_WIDTH 0.01
#define PS_SUBGRID_DASH  0.25

#define PS_BORDER_WIDTH 1.0

#define PS_MAX_LABEL    18

char PS_out();

#endif
