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
/* Header for Resource Defaults */
/********************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This header contains string literals to be taken as the  */
/*                 defaults for the various X resources, in the event that  */
/*                 the resources can't be found.  Some of the resources     */
/*                 have hard-wired defaults, as given in the main module.   */
/****************************************************************************/

#ifndef __DEFAULTS
#define __DEFAULTS

#define DEFAULT_STRING_SIZE 128

typedef char default_string[DEFAULT_STRING_SIZE];

default_string  Display_String    = "";
default_string  GeomString       = "";
default_string  IconGeomString   = "";

default_string  FGString         = "brown";
default_string  BGString         = "white";
default_string  BDString         = "black";
default_string  PlaneFGString    = "blue";
default_string  PlaneBGString    = "white";

/* These colors default to the global colors if not sepcified */
default_string  PlaneBDString    = "";

default_string  ButtonFGString   = "";
default_string  ButtonBGString   = "";
default_string  ButtonBDString   = "";

default_string  BorderWidthString = "2";
default_string  PlaneBorderString = "2";

default_string  PositionFontString = "fixed";
default_string  LabelFontString    = "fixed";
default_string  ButtonFontString   = "variable";
default_string  DialogFontString   = "variable";

default_string  TitleString      = "Xcm";

/* Grid colors */
/*default_string GridColorString = "#888";*/
default_string GridColorString = "orange";
default_string SubgridColorString = "yellow";
/*default_string SubgridColorString = "#444";*/

/* Options specific for this program */

default_string  CenterString  = "0 0";
default_string  ScaleString   = "1.2";

default_string  GridString    = "on";
default_string  NoGridString  = "off";

/* Colors for use in the paths */
default_string Color1String = "#fff";

#endif
