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
static XrmOptionDescRec OptionTable[] = {
{"-display",      ".display",                XrmoptionSepArg, NULL},
{"=",             "*geometry",               XrmoptionIsArg,  NULL},
{"#",             "*iconGeometry",           XrmoptionIsArg,  NULL},
{"-geom",         "*geometry",               XrmoptionSepArg, NULL},
{"-geometry",     "*geometry",               XrmoptionSepArg, NULL},

{"-foreground",   "*foreground",             XrmoptionSepArg, NULL},
{"-fg"        ,   "*foreground",             XrmoptionSepArg, NULL},
{"-background",   "*background",             XrmoptionSepArg, NULL},
{"-bg",           "*background",             XrmoptionSepArg, NULL},
{"-bordercolor",  "*borderColor",            XrmoptionSepArg, NULL},
{"-bd",           "*borderColor",            XrmoptionSepArg, NULL},

{"-gridcolor",    "*plane.gridColor",        XrmoptionSepArg, NULL},
{"-subgridcolor", "*plane.subgridColor",     XrmoptionSepArg, NULL},
 
{"-planefg",       ".plane.foreground",      XrmoptionSepArg, NULL},
{"-planebg",       ".plane.background",      XrmoptionSepArg, NULL},
{"-planebd",       ".plane.borderColor",     XrmoptionSepArg, NULL},

{"-buttonfg",      ".button.foreground",     XrmoptionSepArg, NULL},
{"-buttonbg",      ".button.background",     XrmoptionSepArg, NULL},
{"-buttonbd",      ".button.borderColor",    XrmoptionSepArg, NULL},

/*{"-menufg",      ".menu.foreground",     XrmoptionSepArg, NULL},
{"-menubg",      ".menu.background",     XrmoptionSepArg, NULL},
{"-menubd",      ".menu.borderColor",    XrmoptionSepArg, NULL},

{"-dialogfg",      ".dialog.foreground",     XrmoptionSepArg, NULL},
{"-dialogbg",      ".dialog.background",     XrmoptionSepArg, NULL},
{"-dialogbd",      ".dialog.borderColor",    XrmoptionSepArg, NULL},*/

{"-borderwidth",   ".borderWidth",           XrmoptionSepArg, NULL},
{"-bw",            ".borderWidth",           XrmoptionSepArg, NULL},
{"-planebw",       ".plane.borderWidth",     XrmoptionSepArg, NULL},

{"-name",        ".name",                    XrmoptionSepArg, NULL},
{"-title",       ".title",                   XrmoptionSepArg, NULL},

{"-font",        "*font",                    XrmoptionSepArg, NULL},
{"-fn",          "*font",                    XrmoptionSepArg, NULL},
{"-buttonfont",  "*buttonFont",              XrmoptionSepArg, NULL},
{"-labelfont",   ".labelFont",               XrmoptionSepArg, NULL},
{"-posnfont",    ".positionFont",            XrmoptionSepArg, NULL},
{"-dialogfont",  ".dialogFont",              XrmoptionSepArg, NULL},

{"-colors", "*colors",  XrmoptionSepArg, NULL},
{"-c1",     "*color1",  XrmoptionSepArg, NULL},
{"-c2",     "*color2",  XrmoptionSepArg, NULL},
{"-c3",     "*color3",  XrmoptionSepArg, NULL},
{"-c4",     "*color4",  XrmoptionSepArg, NULL},
{"-c5",     "*color5",  XrmoptionSepArg, NULL},
{"-c6",     "*color6",  XrmoptionSepArg, NULL},
{"-c7",     "*color7",  XrmoptionSepArg, NULL},
{"-c8",     "*color8",  XrmoptionSepArg, NULL},
{"-c9",     "*color9",  XrmoptionSepArg, NULL},
{"-c10",    "*color10", XrmoptionSepArg, NULL},
{"-c11",    "*color11", XrmoptionSepArg, NULL},
{"-c12",    "*color12", XrmoptionSepArg, NULL},
{"-c13",    "*color13", XrmoptionSepArg, NULL},
{"-c14",    "*color14", XrmoptionSepArg, NULL},
{"-c15",    "*color15", XrmoptionSepArg, NULL},
{"-c16",    "*color16", XrmoptionSepArg, NULL},

{"-grays",  "*grays",   XrmoptionSepArg, NULL},

{"-aspect", ".aspect",  XrmoptionSepArg, NULL},

{"-psfile", ".postscriptfile", XrmoptionSepArg, NULL},
{"-outfile",   ".outfile",   XrmoptionSepArg, NULL},

{"-grid",   ".grid", XrmoptionNoArg, "on"}

};

#define OPTION_TABLE_SIZE 52

