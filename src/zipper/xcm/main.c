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
/***************/
/* Main Module */
/***************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This is the module which contains the C function 'main'. */
/*                 The primary purposes are to create the various windows,  */
/*                 parse the command-line options, merge the resource       */
/*                 databases, and operate the event loop.  The event loop   */
/*                 includes the control of the various modes of operation.  */
/****************************************************************************/
/* D. Marshall altered 1-14-94 to begin with hidden grid: line 271*/
/*     and ShowGrid now tells ps.c there is a grid or not.*/    
/*     Changed Kill to Delete and some Quits to Cancel */
/* D. Marshall altered 1-17-95 XrmParseCommandLine so that can use */
/*     path to command in argv[0], by replacing it with "xcm" in line 990*/
/*    If xcm was invoked using the path as part of the command*/
/*  this line wouldn't work so that the command line options were not parsed */
/*    Also fixed -outfile to be the Write output file (EditFileName)*/
/*  changed printfile to postscript file (psfile) here and in options.h*/
/*  3-95 Also Edit file begins with lines and boxed points now. */
/*  3-95 fixed -grid option so that can start with grid using xcm -grid*/
/*  8-95 fixed quit so that prompts for output file if created*/
/*  lines 1179 and 1180 commented out: caused errors*/
/*    but needs to be properly fixed*/
/*       path or used option -e to input an edit file.*/
/*  D. Marshall: 6-00 line 1222 changed CommandLineDatabase to FinalDatabase */
/*    suggestion of  Eric Schwartz to eliminate error Red Hat Linux */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xresource.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "globals.h"
#include "basic.h"
#include "file.h"
#include "xplane.h"
#include "button.h"
#include "primary.h"
#include "path.h"
#include "textbox.h"
#include "dialog.h"
#include "menu.h"
#include "ps.h"
#include "options.h"
#include "defaults.h"

/************************/
/* Program-Wide Globals */
/************************/

Display   *display;
int       screen;
int       depth;
Colormap  colormap;

XFontStruct *PositionFont;
XFontStruct *LabelFont;
XFontStruct *ButtonFont;
XFontStruct *DialogFont;

unsigned long Colors[MAX_COLORS];
int           ColorCount = 1;

char        *argv2[MAX_FILES];

int		ShowGrid = 0;
int          CreatePath = 0;

unsigned long Black, White; /* temporary */

GC GrayGC;
Pixmap GrayStipple;

primary_window Primary;
struct button_struct GridButton;
struct button_struct TestButton;
struct button_struct PointOptionsButton;
struct button_struct DisplayFormatButton;
struct button_struct EditFormatButton;
struct button_struct PrintButton;
struct button_struct OutputButton;
struct button_struct RedisplayButton;
struct button_struct QuitButton;

button_type CreatePathButton;
button_type KillPointButton;
button_type MovePointButton;
button_type BreakPointButton;
button_type KillLineButton;
button_type BreakLineButton;

GC                   ButtonGC;
complex LowerLeft, UpperRight;
complex Middle;

dialog_window Dialog;

/****************************************/
/* Globals for the command-line options */
/****************************************/

char DisplayName[256];

unsigned int PrimaryWidth  = 800;
unsigned int PrimaryHeight = 600;
int          PrimaryX      = 0;
int          PrimaryY      = 0;

unsigned int ButtonWidth;
unsigned int ButtonHeight;

/* The margins are dependent on the sizes of the fonts, and the buttons */
int          TopMargin;
int          BottomMargin = BOTTOM_MARGIN;
int          LeftMargin;
int          RightMargin = RIGHT_MARGIN;

XSizeHints SizeHints;

unsigned int PrimaryBorderWidth = 2;
unsigned int PlaneBorderWidth = 2;

/* The Colors */
unsigned long PrimaryFG;
unsigned long PrimaryBG;
unsigned long PrimaryBD;

unsigned long PlaneFG;
unsigned long PlaneBG;
unsigned long PlaneBD;
unsigned long PlaneXOR;

unsigned long GridColor;
unsigned long SubgridColor;

unsigned long PathFG;

unsigned long ButtonFG;
unsigned long ButtonBG;
unsigned long ButtonBD;
unsigned long ButtonHiFG;
unsigned long ButtonHiBG;

unsigned long MenuFG;
unsigned long MenuBG;
unsigned long MenuBD;

unsigned long DialogFG;
unsigned long DialogBG;
unsigned long DialogBD;

double ScreenAspect = 1.0;

/**********************/
/* Resource Databases */
/**********************/

XrmDatabase CommandLineDatabase;
XrmDatabase FinalDatabase;

/******************************************/
/* Globals for the source files of points */
/******************************************/

/* each "file" is a linked list of type point_type */
point_type **PointFile;
char       **PointFileName;
int        PointFileCount = 0;

point_type *EditFile = NULL;
char       EditFileName[256] = "output.dat";


point_type *Main_File;

char PrintFilename[256] = "output.ps";
char GraphFilename[256] = "output.dat";
char Colorprint[256] = "b&w";
int  Printcolor = 0;

/*************/
/* The Menus */
/*************/

simple_menu PointOptionsMenu;
char *PointOptionsMenuText[] = 
{"Arbitrary", "Decade", "Decade/10", "Decade/100", 
 "Static Points", "Edit Points"};
int  PointOptionsMenuCount = 6;

simple_menu FileDisplayMenu;
char *FileDisplayMenuText[] = 
{"Lines", "Points", "Boxed Points", "Lines & Boxed Points", "Invisible"};
int FileDisplayMenuCount = 5;

simple_menu EditDisplayMenu;
char *EditDisplayMenuText[] = 
{"Lines", "Points", "Boxed Points", "Lines & Boxed Points", "Invisible"};
int EditDisplayMenuCount = 5;

void quit();
char parse_args();
void parse_remaining_args();
void load_fonts();
void redraw_plane();
void operate_grid_button();
void operate_print();
void operate_output();
void operate_redisplay();
void get_option();
void file_display_menu();
void edit_display_menu();
void operate_create_path();
void operate_kill_point();
void operate_move_point();
void operate_break_point();
void operate_kill_line();
void operate_break_line();
void main_loop();
void load_files();
void global_bounds();
void merge_options();
void get_databases();
void setup_globals();

main( int argc, char *argv[] )
{
  char line[80];
  double width, height;
  unsigned int x,y;
  int          dy;

  XSizeHints hints;
  point_type *test;

  XrmInitialize();  /* Initialize the Resource Manager */
  parse_args(&argc, argv);
  InitX(DisplayName);
  colormap = DefaultColormap(display, screen);

  get_databases();
  merge_options();
  load_fonts();
  setup_globals();

  parse_remaining_args(&argc, argv);
  load_files();
  global_bounds(&LowerLeft, &UpperRight);

  setup_plane_vars();
  open_primary(&Primary, LowerLeft.x, LowerLeft.y, UpperRight.x, UpperRight.y);
  Primary.plane.redraw = redraw_plane;

			
  /* Create the gray stipple and the gray gc */

  GrayGC = DefaultGC(display, screen);
  XSetForeground(display, GrayGC, WhitePixel(display, screen));
  XSetBackground(display, GrayGC, BlackPixel(display, screen));
  XSetFillStyle(display, GrayGC, FillStippled);  
  GrayStipple = create_gray_stipple(Primary.primary);
  XSetStipple(display, GrayGC, GrayStipple);


  x = PrimaryWidth - BUTTON_OUTER_MARGIN - ButtonWidth;
  y = TopMargin;
  dy = ButtonHeight + BUTTON_SPACE;

  create_button(&GridButton, Primary.primary, "Show Grid", &Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, NorthEastGravity);
  GridButton.apply = operate_grid_button;
  y += dy;

  create_button(&DisplayFormatButton, Primary.primary, "Static Options", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, NorthEastGravity);
  DisplayFormatButton.apply = file_display_menu;
  y += dy;

  create_button(&EditFormatButton, Primary.primary, "Edit Options", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, NorthEastGravity);
  EditFormatButton.apply = edit_display_menu;
  y += dy;

  create_button(&PointOptionsButton, Primary.primary, "Grab Options", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, NorthEastGravity);
  PointOptionsButton.apply = get_option;
  y += 2*dy;



  create_button(&CreatePathButton, Primary.primary, "Create Path", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_inactive, NorthEastGravity);
  CreatePathButton.apply = operate_create_path;
  y += dy;

  create_button(&KillPointButton, Primary.primary, "Delete Point", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_inactive, NorthEastGravity);
  KillPointButton.apply = operate_kill_point;
  y += dy;

  create_button(&MovePointButton, Primary.primary, "Move Point", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_inactive, NorthEastGravity);
  MovePointButton.apply = operate_move_point;
  y += dy;

  create_button(&BreakPointButton, Primary.primary, "Break Point", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_inactive, NorthEastGravity);
  BreakPointButton.apply = operate_break_point;
  y += dy;

  create_button(&KillLineButton, Primary.primary, "Delete Line", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_inactive, NorthEastGravity);
  KillLineButton.apply = operate_kill_line;
  y += dy;

  create_button(&BreakLineButton, Primary.primary, "Break Line", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_inactive, NorthEastGravity);
  BreakLineButton.apply = operate_break_line;
  y += dy;

  create_button(&OutputButton, Primary.primary, "Write Output", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, NorthEastGravity);
  OutputButton.apply = operate_output;
  y += 2*dy;


  create_button(&PrintButton, Primary.primary, "Postscript", &Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, NorthEastGravity);
  PrintButton.apply = operate_print;
  y += dy;


  y = PrimaryHeight - ButtonHeight - BottomMargin;
  create_button(&QuitButton, Primary.primary, "Quit", &Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, SouthEastGravity);
  QuitButton.apply = quit;

  y -= dy;
  create_button(&RedisplayButton, Primary.primary, "Re-read Files", 
		&Primary.button_gc,
		ButtonFont, x, y, ButtonWidth, ButtonHeight,
		ButtonBD, ButtonFG, ButtonBG, ButtonBG, ButtonFG,
		button_active, SouthEastGravity);
  RedisplayButton.apply = operate_redisplay;

  /*  TextBox = create_text_box(Primary.primary, 20, 20, 50, 20,
                            &Primary.button_gc,
			    WhitePixel(display, screen), 
			    BlackPixel(display, screen),
			    WhitePixel(display, screen),
			    ButtonFont, "", WestGravity);*/

  White = WhitePixel(display, screen);
  Black = BlackPixel(display, screen);
  initialize_dialog(Black, White, Black, White, Black, 
		    PositionFont, ButtonFont);


  create_simple_menu(&PointOptionsMenu, 20, 20, 
		     WhitePixel(display, screen), BlackPixel(display, screen),
		     WhitePixel(display, screen), 
		     BlackPixel(display, screen), WhitePixel(display, screen),
		     "Grab Options", 
		     PointOptionsMenuText, PointOptionsMenuCount, 
		     ButtonFont, 0);

  create_simple_menu(&FileDisplayMenu, 20, 20, 
		     WhitePixel(display, screen), BlackPixel(display, screen),
		     WhitePixel(display, screen), 
		     BlackPixel(display, screen), WhitePixel(display, screen),
		     "Static Display Options", 
		     FileDisplayMenuText, FileDisplayMenuCount, 
		     ButtonFont, 0);

  create_simple_menu(&EditDisplayMenu, 20, 20, 
		     WhitePixel(display, screen), BlackPixel(display, screen),
		     WhitePixel(display, screen), 
		     BlackPixel(display, screen), WhitePixel(display, screen),
		     "Edit Display Options", 
		     EditDisplayMenuText, EditDisplayMenuCount, 
		     ButtonFont, 3);
/* Marshall changed 0 to 3 in line above 3-9-95 */
/*  so that default on Edit options is lines and boxed points. */

  main_loop();
  
/*10/04 Marshall commented out next line. Caused warning on compile*/
/* the function gets is called dangerous */
/* I don't see what the line is for*/
/*  gets(line);*/
}

void redraw_plane( obj )
     plane_window *obj;
{
  point_type *ptr;
  int        k;
  primary_window *parent;

  parent = (primary_window*) obj->parent_struct;
  XSetFunction(display, *(obj->gc), GXcopy);
  XClearWindow(display, parent->primary);

  /* Draw the graph */
  XSetForeground(display, *(obj->gc), PlaneFG);
  draw_graph(parent);

  for (k = 0; k < PointFileCount; k++) {

    XSetForeground(display, *(obj->gc), Colors[k%ColorCount]);

    /* Draw the lines, if that is what the option is */
    if ((FileDisplayMenu.index == 0) || (FileDisplayMenu.index == 3)) {

      /* Check to see if all the lines are in view */
      if (p_original_in_view(obj)) 
	for (ptr = PointFile[k]; ptr != NULL; ptr = ptr->next) 
	  plane_draw_simple_lines(&(Primary.plane), ptr->points, ptr->count);
      
      else 
	for (ptr = PointFile[k]; ptr != NULL; ptr = ptr->next) 
	  plane_general_clip(&(Primary.plane), ptr->points, ptr->count);
    }
    
    /* Otherwise, draw only the points */

    else if ((FileDisplayMenu.index == 1) || (FileDisplayMenu.index == 2)) {
      for (ptr = PointFile[k]; ptr != NULL; ptr = ptr->next) 
	plane_draw_points(&(Primary.plane), ptr->points, ptr->count);
    }

    /* Finally, check to see if they should be boxed */

    if ((FileDisplayMenu.index >= 2) && (FileDisplayMenu.index < 4))
      for (ptr = PointFile[k]; ptr != NULL; ptr = ptr->next) 
	plane_draw_rects(&(Primary.plane), ptr->points, ptr->count);
  }

  label_grid(&Primary);
  XFlush(display);
}

void quit()
{
/*  XFreeCursor */
/* Marshall did:  */
/* it will prompt for output filename if Create path selected */
/* or started xcm with option -e and output not written */
/*  when the quit buttton is selected*/
/* 10-04 removed again- causes errors in redhat linnux. The only purpose*/
/* was to remind users to write their output*/
/*    if (( CreatePath >= 1 ) || (EditFile != NULL))*/
/*	operate_output();*/
  XFreeFont(display, PositionFont);
  XFreeFont(display, ButtonFont);
  destroy_primary(&Primary);
  XCloseDisplay(display);
  exit(0);
}

void load_fonts()
  /* POST: Tries to load the fonts used in the program. */
{
  PositionFont = load_font_with_alt(PositionFontString, "fixed", NULL);
  LabelFont = load_font_with_alt(LabelFontString, "fixed", NULL);
  ButtonFont = load_font_with_alt(ButtonFontString, "fixed", NULL);
  DialogFont = load_font_with_alt(DialogFontString, "fixed", NULL);
}

complex *grab_point( primary, x, y, z )
     primary_window *primary;
     int            x, y;
     complex        *z;
{
  double    tol;
  complex   z0, *value;
  XPoint    p;
  path_node *node;
  int       k;

  tol = pixel_tolerance(&(primary->plane));

  p.x = x;
  p.y = y;
  p_view_to_world(&(primary->plane), &z0, p);
  
  switch (PointOptionsMenu.index) 
    {
    case 0 :
      /* arbitrary */
      *z = z0;
      break;

    case 1 :
      /* Decade */
      z->x = prec_round(z0.x, primary->plane.decade);
      z->y = prec_round(z0.y, primary->plane.decade);
      break;

    case 2 :
      /* Decade/10 */
      z->x = prec_round(z0.x, primary->plane.decade/10);
      z->y = prec_round(z0.y, primary->plane.decade/10);
      break;

    case 3 :
      /* Decade/100 */
      z->x = prec_round(z0.x, primary->plane.decade/100);
      z->y = prec_round(z0.y, primary->plane.decade/100);
      break;

    case 4 :
      /* File points */
      for (k = 0; k < PointFileCount; k++) {
	value = find_file_point(PointFile[k], z0, tol);
	if (value != NULL) {
	  *z = *value;
	  return(NULL);
	}
      }
      *z = z0;
      break;

    case 5:
      /* Graph points */
      node = find_graph_point(primary, z0);
      if (node != NULL) {
	*z = z0;
	return(&(node->point));
      }
      *z = z0;
      break;
    }
  return(NULL);
}

void grab_translate( obj, event )
     plane_window  *obj;
     XEvent        *event;
{
  complex        z;
  primary_window *parent;

  parent = (primary_window*) obj->parent_struct;
  grab_point(parent, event->xbutton.x, event->xbutton.y, &z);
  recenter_plane_window(obj, z.x, z.y);
  redraw_plane_window(obj);
}
  
void operate_redisplay( obj )
     struct button_struct *obj;
{
  int k;

  XClearWindow(display, Primary.primary);
  XClearWindow(display, Primary.plane.window);
  XFlush(display);
  
  for (k = 0; k < PointFileCount; k++)
    kill_file(PointFile[k]);
  load_files();

  redraw_plane_window(&(Primary.plane));
  release_button(obj);
}

void operate_grid_button( obj )
     struct button_struct *obj;
{
  if (Primary.plane.show_grid) {
    ShowGrid = 0;
    Primary.plane.show_grid = 0;
    strcpy(obj->text, "Show Grid");
  }
  else {
    ShowGrid = 1;
    Primary.plane.show_grid = 1;
    strcpy(obj->text, "Hide Grid");
  }
  redraw_plane_window(&(Primary.plane));
  XFlush(display);
  release_button(obj);
  XFlush(display);
}

void operate_print( obj )
     struct button_struct *obj;
{
  enum dialog_return value;
  if ( Printcolor == 0){
      strcpy(Colorprint, "b&w");
  }
  if ( Printcolor == 1){
      strcpy(Colorprint, "color");
  }
  create_simple_text_dialog(&Dialog, 100, 100, 
       "Please specify the filename for the PostScript output.",
			    "Filename:", PrintFilename,
			    "Okay", Colorprint, "Cancel", "Postscript Filename");
  value = run_dialog(&Dialog);
  
/* Marshall added, so that still changes name if click on new button*/
/* need to reopen with new button = color and need to be able to*/
/* cycle*/
  if (value == d_button2){
      Printcolor = 1 - Printcolor;
      strcpy(PrintFilename, Dialog.textbox.text);
       operate_print( obj );
      return;
  }
   
  if ((value == d_button1) || (value == d_return_key)) {
    strcpy(PrintFilename, Dialog.textbox.text);
    while (!PS_out(&Primary, PrintFilename)) {
      XBell(display, 100);
      XFlush(display);
      create_simple_text_dialog(&Dialog, 100, 100, 
         "Unable to open this file.\nPlease specify new file.",
				"Filename:", PrintFilename,
				"Okay", "", "Cancel", "Postscript Filename");
      value = run_dialog(&Dialog);
      if ((value != d_button1) && (value != d_return_key))
/* Marshall asks: shouldn't PostScript button be released here?*/
	return;
      strcpy(PrintFilename, Dialog.textbox.text);
    }
  }
  release_button(obj);
}

void operate_output( obj )
     struct button_struct *obj;
{
  enum dialog_return value;
  create_simple_text_dialog(&Dialog, 100, 100, 
			    "Please specify the filename for the data.",
			    "Filename:", GraphFilename,
			    "Okay", "", "Cancel", "Output Filename");
  value = run_dialog(&Dialog);
  
  if ((value == d_button1) || (value == d_return_key)) {
    strcpy(GraphFilename, Dialog.textbox.text);
    while (!write_graph(&(Primary.graph), GraphFilename)) {
      XBell(display, 100);
      XFlush(display);
      create_simple_text_dialog(&Dialog, 100, 100, 
         "Unable to open this file.\nPlease specify new file.",
				"Filename:", GraphFilename,
				"Okay", "", "Cancel", "Output Filename");
      value = run_dialog(&Dialog);
      if ((value != d_button1) && (value != d_return_key))
	return;
      strcpy(GraphFilename, Dialog.textbox.text);
    }
  }
  CreatePath = 0;
  release_button(obj);
}
  

void get_option( struct button_struct *obj )
{
  int          k;
  Window       root, child;
  int          root_x, root_y;
  int          x, y;
  unsigned int state;

  XQueryPointer(display, RootWindow(display, screen), &root, &child, 
		&root_x, &root_y, &x, &y, &state);

  k = run_simple_menu(&PointOptionsMenu, x-10, y-10);
  release_button(obj);
}

void file_display_menu( struct button_struct *obj )
{
  int          k;
  Window       root, child;
  int          root_x, root_y;
  int          x, y;
  unsigned int state;

  XQueryPointer(display, RootWindow(display, screen), &root, &child, 
		&root_x, &root_y, &x, &y, &state);

  k = run_simple_menu(&FileDisplayMenu, x-10, y-10);
  release_button(obj);
  redraw_plane_window(&(Primary.plane));
}

void edit_display_menu( struct button_struct *obj )
{
  int          k;
  Window       root, child;
  int          root_x, root_y;
  int          x, y;
  unsigned int state;

  XQueryPointer(display, RootWindow(display, screen), &root, &child, 
		&root_x, &root_y, &x, &y, &state);

  k = run_simple_menu(&EditDisplayMenu, x-10, y-10);
  release_button(obj);
  redraw_plane_window(&(Primary.plane));
}

void create_path( struct button_struct *obj )
{
  set_graph_mode(&Primary, g_break_line);

}

void release_other_modes()
{
  release_button(&CreatePathButton);
  release_button(&KillPointButton);
  release_button(&MovePointButton);
  release_button(&KillLineButton);
  release_button(&BreakLineButton);
  release_button(&BreakPointButton);
  set_graph_mode(&Primary, g_inactive);
}

void operate_create_path( struct button_struct *obj )
{
  if (obj->status == button_active) {
    set_graph_mode(&Primary, g_inactive);
  }
  else {
    release_button(&KillPointButton);
    release_button(&MovePointButton);
    release_button(&KillLineButton);
    release_button(&BreakLineButton);
    release_button(&BreakPointButton);
    set_graph_mode(&Primary, g_creating);
    CreatePath = 1;
  }
}

void operate_kill_point( struct button_struct *obj )
{
  /* Check to see if the button was released by the mouse */
  if (obj->status == button_active) 
    set_graph_mode(&Primary, g_inactive);
  else {
    release_button(&CreatePathButton);
    release_button(&MovePointButton);
    release_button(&KillLineButton);
    release_button(&BreakLineButton);
    release_button(&BreakPointButton);
    set_graph_mode(&Primary, g_kill_point);
  }
}

void operate_move_point( struct button_struct *obj )
{
  if (obj->status == button_active) 
    set_graph_mode(&Primary, g_inactive);
  else {
    release_button(&CreatePathButton);
    release_button(&KillPointButton);
    release_button(&KillLineButton);
    release_button(&BreakLineButton);
    release_button(&BreakPointButton);
    set_graph_mode(&Primary, g_move_point);
  }
}

void operate_kill_line( struct button_struct *obj )
{
  if (obj->status == button_active) 
    set_graph_mode(&Primary, g_inactive);
  else {
    release_button(&CreatePathButton);
    release_button(&KillPointButton);
    release_button(&MovePointButton);
    release_button(&BreakLineButton);
    release_button(&BreakPointButton);
    set_graph_mode(&Primary, g_kill_line);
  }
}

void operate_break_point( struct button_struct *obj )
{
  if (obj->status == button_active) 
    set_graph_mode(&Primary, g_inactive);
  else {
    release_button(&CreatePathButton);
    release_button(&KillPointButton);
    release_button(&MovePointButton);
    release_button(&BreakLineButton);
    release_button(&KillLineButton);
    set_graph_mode(&Primary, g_break_point);
  }
}

void operate_break_line( struct button_struct *obj )
{
  if (obj->status == button_active) 
    set_graph_mode(&Primary, g_inactive);
  else {
    release_button(&CreatePathButton);
    release_button(&KillPointButton);
    release_button(&MovePointButton);
    release_button(&BreakPointButton);
    release_button(&KillLineButton);
    set_graph_mode(&Primary, g_break_line);
  }
}
    
/**********************/
/* Primary Event Loop */
/**********************/

void main_loop()
{
  XEvent event;
  enum dialog_return dialog_value;

  while (1) {
    XNextEvent(display, &event);

    if (event.xany.window == Primary.primary) 
      handle_primary_event(&Primary, &event); 
    if (event.xany.window == Primary.plane.window) 
      handle_plane_event(&(Primary.plane), &event);

    if (event.xany.window == GridButton.window)
      handle_button_event(&GridButton, &event);
    if (event.xany.window == QuitButton.window)
      handle_button_event(&QuitButton, &event);
    if (event.xany.window == PointOptionsButton.window)
      handle_button_event(&PointOptionsButton, &event);

    if (event.xany.window == CreatePathButton.window)
      handle_button_event(&CreatePathButton, &event);
    if (event.xany.window == MovePointButton.window)
      handle_button_event(&MovePointButton, &event);
    if (event.xany.window == KillPointButton.window)
      handle_button_event(&KillPointButton, &event);
    if (event.xany.window == BreakPointButton.window)
      handle_button_event(&BreakPointButton, &event);
    if (event.xany.window == KillLineButton.window)
      handle_button_event(&KillLineButton, &event);
    if (event.xany.window == BreakLineButton.window)
      handle_button_event(&BreakLineButton, &event);

    if (event.xany.window == DisplayFormatButton.window)
      handle_button_event(&DisplayFormatButton, &event);
    if (event.xany.window == EditFormatButton.window)
      handle_button_event(&EditFormatButton, &event);
    if (event.xany.window == PrintButton.window)
      handle_button_event(&PrintButton, &event);
    if (event.xany.window == OutputButton.window)
      handle_button_event(&OutputButton, &event);
    if (event.xany.window == RedisplayButton.window)
      handle_button_event(&RedisplayButton, &event);

  }
}

/****************************/
/* High-Level File Handling */
/****************************/

void load_files()
  /* PRE : 'PointFileCount' and the array 'PointFileName' are assigned.  */
  /* POST: Reads in all the files from PointFileName.                    */
{
  int k;

  for (k = 0; k < PointFileCount; k++)
    PointFile[k] = parse_file(PointFileName[k]);
}

void global_bounds( LL, UR )
     complex *LL;
     complex *UR;
  /* POST: 'LL' and 'UR' contain the corners of the smallest rectangle */
  /*       which bound all the points in all the lists in 'PointFile'. */
  /*       If 'PointFile' is empty, an appropriate value is returned.  */
{
  int k;
  complex LL1, UR1;

  if ((PointFileCount == 0) && (EditFile != NULL)) {
    file_frame(EditFile, LL, UR);
    return;
  }
    
  if ((PointFileCount == 0) && (EditFile == NULL)) {
    LL->x = -1.2;
    LL->y = -1.2;
    
    UR->x = 1.2;
    UR->y = 1.2;
  }
  else {
    if (PointFileCount > 0) {
      file_frame(PointFile[0], LL, UR);
      for (k = 1; k < PointFileCount; k++) {
	if (file_frame(PointFile[k], &LL1, &UR1)) {
	  
	  LL->x = MIN(LL1.x, LL->x);
	  LL->y = MIN(LL1.y, LL->y);
	  
	  UR->x = MAX(UR1.x, UR->x);
	  UR->y = MAX(UR1.y, UR->y);
	}
      }
    }
    if (EditFile != NULL) {
      if (file_frame(EditFile, &LL1, &UR1)) {
	LL->x = MIN(LL1.x, LL->x);
	LL->y = MIN(LL1.y, LL->y);
	
	UR->x = MAX(UR1.x, UR->x);
	UR->y = MAX(UR1.y, UR->y);
      }
    }
  }
}


/************************************************/
/* Command-Line Options, and Resource Databases */
/************************************************/

void echo_usage()
{
  fprintf(stderr, "usage: xcm\n");
  exit(1);
}

void crash( message )
     char message[];
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

unsigned long parse_color( color_name )
     char color_name[];
  /* POST: Attempts to parse the color named in 'color_name', and if      */
  /*       successful, tries to allocate a new color cell in 'colormap'   */
  /*       for the color.  The pixel value of the color is returned if    */
  /*       'color_name' contains a valid color, otherwise an error        */
  /*       message is printed and the program exits with code 1.          */
{
  XColor         rgb;
  unsigned long  pixel;


  if (!XParseColor(display, colormap, color_name, &rgb)) {
    fprintf(stderr, "%s: invalid color specification: %s\n", NAME, color_name);
    exit(1);
  }
  XAllocColor(display, colormap, &rgb);
  return(rgb.pixel);
}
  

char parse_args( int *argc, char *argv[] )
  /* POST: Parses the command line arguments found in 'OptionsTable', and */
  /*       assigns the global 'DisplayName', but doesn't open the         */
  /*       connection.  The parsed arguments are returned from the array  */
  /*       'argv' and 'argc' contains the new count, i.e. the command-    */
  /*       line arguments which were not parsed.                          */
{
  int  k = 1;
  char *arg;
  char *str_type[32];
  XrmValue value;

  /* Parse the command-line options which appear in the option table */
  /* defined in "options.h".                                         */

  XrmParseCommand(&CommandLineDatabase, OptionTable, OPTION_TABLE_SIZE,
		  "xcm", argc, argv);

/* above line caused a  problem if use pathname to executible file */
/* Here is the previous version.   */
/*                 argv[0], argc, argv); */
/* There is a further problem: large aspect yields no labels */
/* on the horizontal axes. */
/* D.Marshall 1-17-95              */

  /* ASSERT: 'argv' contains the command line parameters which were not */
  /*         included in the option table, and 'argc' is the count.     */

  
  /* Get the display name, as it will be required to get all the other */
  /* resource files.                                                   */

  if (XrmGetResource(CommandLineDatabase, "xcm.display",
		     "Xcm.Display", str_type, &value) == True) 
    strncpy(DisplayName, value.addr, (int) value.size);

}

void remove_arg( int *argc, char *argv[], int index )
  /* POST: Removes the argument of index 'index' from the argument array, */
  /*       and decrements 'argc' if the index exists.                     */
{
  int k;

  if ( (index >= 0) && (index < *argc) ) {
    for (k = index+1; k < *argc; k++) 
      argv[k-1] = argv[k];
    (*argc)--;
  }
}

void parse_remaining_args( int *argc, char *argv[] )
{
  int k = 1;
  int n;

XColor    rgb;

  /* Parse any argument preceeded by a '-' */
  while (k < *argc) {

    /* COLORS:  One can force the colors here */
    if (!strcmp(argv[k], "-use")) {
      remove_arg(argc, argv, k);
      if (k >= *argc) 
	crash("inapropriate '-use' specification.");

      /* The first argument is the count */      
      if (!sscanf(argv[k], "%d", &ColorCount))
	crash("argument following '-use' option must be an positive integer.");
      remove_arg(argc, argv, k);
      if (ColorCount <= 0)
	crash("positive number of colors required.");

      /* Now get the colors */
      for (n = 0; n < ColorCount; n++) {
	if (k >= *argc) 
	  crash("too few colors following '-use' specification.");


/* Marshall: (saves colors for ps.c)*/
   argv2[n]=argv[k];
   XParseColor(display, colormap, argv2[n], &rgb);
/*              */

 	Colors[n] = parse_color(argv[k]);
	remove_arg(argc, argv, k);
      }
    }
    
    /* Check for an '-e' flag, indicating "edit" */
    else if (!strcmp(argv[k], "-e")) {
      remove_arg(argc, argv, k);
      if (k >= *argc) 
	crash("'-e' option requires a filename.");

      strcpy(EditFileName, argv[k+1]);
      remove_arg(argc, argv, k);
      EditFile = parse_file(argv[k+1]);
      file_to_graph(&(Primary.graph), EditFile);
      
    }

    else 
      k++;
  }

  /* ASSERT: The remaining strings in 'argv' are not preceeded by '-', */
  /*         and all the options are parsed.                           */


  /* Allocate space for the files */

  PointFileCount = *argc - 1;
  PointFile = (point_type**) calloc(PointFileCount, sizeof(point_type*));
  PointFileName = (char**) calloc(PointFileCount, sizeof(char*));

  /* Now load in the files.  The program exits if one isn't found. */
  for (k = 0; k < PointFileCount; k++) {
    PointFileName[k] = argv[k+1];
/*    PointFile[k] = Parse_File(argv[k+1]); */
  }
}


/********************/
/* Resource Manager */
/********************/

void get_databases()
  /* PRE : the function 'parse_args' has been called, so that FinalDatabase */
  /*       is assigned, and the display is open.                            */
  /*       Also the global 'Colormap' is assigned and installed.            */
  /* POST: Reads and merges the user databases in the following order:      */
  /*                                                                        */
  /*      1) the application database in /usr.lib/X11/app-defaults/         */
  /*                                                                        */
  /*      2) the server defaults from the string referenced by the          */
  /*         display structure.                                             */
  /*                                                                        */
  /*      3) the file referenced by the environment variable XENVIRONMENT,  */
  /*         or if there is none, the file .Xdefaults in the user's home    */
  /*         directory, referenced by the environment variable HOME.        */
  /*                                                                        */
  /*       'FinalDatabase' contains the merger of these three databases,    */
  /*       In the specified order.                                          */
{
  XrmDatabase ServerDatabase;
  XrmDatabase HomeDatabase;

  char filename_buffer[1024];
  char name[256];
  char *xenvironment;
  char *home;

  /* Get the "app-defaults/" database, if there is one */
  sprintf(name, "/usr/lib/X11/app-defaults/%s", CLASS_NAME);

  FinalDatabase = XrmGetFileDatabase(name);
  
  /* Get the database from the resource manager string */
/* D. Marshall commented out next 2 lines because they*/
/* caused error messages...will fix later*/
/*  ServerDatabase = XrmGetStringDatabase(XResourceManagerString(display));*/
/*  XrmMergeDatabases(ServerDatabase, &FinalDatabase);*/


  /* Check for an XENVIRONMENT file, otherwise use the .XDefaults file */

  if ( (xenvironment = getenv("XENVIRONMENT")) == NULL ) {
  
    /* Set environment to the name ".Xdefaults" in the user's home directory */
    home = getenv("HOME");
    strcpy(name, home);
    strcat(name, "/.Xdefaults");
    xenvironment = name;
  }

  /* Otherwise use the name in the XENVIRONMENT environment variable */
  HomeDatabase = XrmGetFileDatabase(xenvironment);
  XrmMergeDatabases(HomeDatabase, &FinalDatabase);
}

void merge_options()
  /* POST: This functions merges the resource databases, and provides   */
  /*       all the defaults if some of the resources aren't found.      */
  /*       Most of the defaults are contained in 'defaults.h' as string */
  /*       literals.                                                    */
{
  char     *str_type[32];
  char     buffer[256];
  char     r_name[128], c_name[128];
  XrmValue value;
  XColor   rgb;
  int      k, graylevel;
  double   x, y;

  /* Merge the command-line options with the final database */
  XrmMergeDatabases(CommandLineDatabase, &FinalDatabase);

  /* Geometry */
/* change suggested for RedHat Linux 6/00 (Marshall): */
/*  if (XrmGetResource(CommandLineDatabase, "xcm.geometry", "Xcm.Geometry",*/
  if (XrmGetResource(FinalDatabase, "xcm.geometry", "Xcm.Geometry",
		     str_type, &value) == True)
    strncpy(GeomString, value.addr, value.size);

  /* Icon Geometry */
  if (XrmGetResource(FinalDatabase, "xcm.iconGeometry", "Xcm.IconGeometry",
		     str_type, &value) == True)
    strncpy(IconGeomString, value.addr, value.size);

  /* foreground color */
  if (XrmGetResource(FinalDatabase, "xcm.foreground", "Xcm.Foreground",
		     str_type, &value) == True) 
    strncpy(FGString, value.addr, value.size);
  PrimaryFG = parse_color(FGString);

  /* background color */
  if (XrmGetResource(FinalDatabase, "xcm.background", "Xcm.Background",
		     str_type, &value) == True)
    strncpy(BGString, value.addr, value.size);
  PrimaryBG = parse_color(BGString);

  /* border color */
  if (XrmGetResource(FinalDatabase, "xcm.borderColor", "Xcm.BorderColor",
		     str_type, &value) == True)
    strncpy(BDString, value.addr, value.size);
  PrimaryBD = parse_color(BDString);

  /* border width */
  if (XrmGetResource(FinalDatabase, "xcm.borderWidth", "Xcm.BorderWidth",
		     str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    if (!sscanf(buffer, "%d", &PrimaryBorderWidth))
      echo_usage();
  }


  /***********************/
  /* The plane resources */
  /***********************/

  /* plane foreground color */
  if (XrmGetResource(FinalDatabase, "xcm.plane.foreground", 
		     "Xcm.Plane.Foreground", str_type, &value) == True) {
    strncpy(PlaneFGString, value.addr, value.size);
  }
/*  else*/
/*    PlaneFG = PrimaryFG;*/
  PlaneFG = parse_color(PlaneFGString);

  /* plane background color */
  if (XrmGetResource(FinalDatabase, "xcm.plane.background", 
		     "Xcm.Plane.Background", str_type, &value) == True) {
    strncpy(PlaneBGString, value.addr, value.size);
  }
/*  else*/
/*    PlaneBG = PrimaryBG;*/
  PlaneBG = parse_color(PlaneBGString);

  /* plane border color */
  if (XrmGetResource(FinalDatabase, "xcm.plane.borderColor", 
		     "Xcm.Plane.borderColor", str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    PlaneBD = parse_color(buffer);
  }
  else
    PlaneBD = PrimaryFG;

  /* plane border width */
  if (XrmGetResource(FinalDatabase, "xcm.plane.borderWidth", 
		     "Xcm.Plane.BorderWidth",
		     str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    if (!sscanf(buffer, "%d", &PlaneBorderWidth))
      echo_usage();
  }

  /* The grid colors */
  if (XrmGetResource(FinalDatabase, "xcm.plane.gridColor", 
		     "Xcm.Plane.GridColor", str_type, &value) == True) {
    strncpy(GridColorString, value.addr, value.size);
    /* If a grid color is specified, the subgrid color defaults to that */
    strcpy(SubgridColorString, GridColorString);
  }
  GridColor = parse_color(GridColorString);

  if (XrmGetResource(FinalDatabase, "xcm.plane.subgridColor", 
		     "Xcm.Plane.SubgridColor", str_type, &value) == True) 
    strncpy(SubgridColorString, value.addr, value.size);
  SubgridColor = parse_color(SubgridColorString);

  if (depth == 1) {
    GridColor = PrimaryFG;
    SubgridColor = GridColor;
  }


  /* Fonts */
  if (XrmGetResource(FinalDatabase, "xcm.labelFont", "Xcm.LabelFont",
		     str_type, &value) == True)
    strncpy(LabelFontString, value.addr, value.size);

  if (XrmGetResource(FinalDatabase, "xcm.positionFont", "Xcm.PositionFont",
		     str_type, &value) == True)
    strncpy(PositionFontString, value.addr, value.size);

  if (XrmGetResource(FinalDatabase, "xcm.buttonFont", "Xcm.ButtonFont",
		     str_type, &value) == True)
    strncpy(ButtonFontString, value.addr, value.size);

  if (XrmGetResource(FinalDatabase, "xcm.dialogFont", "Xcm.DialogFont",
		     str_type, &value) == True)
    strncpy(DialogFontString, value.addr, value.size);


  /****************************/
  /* The colors for the files */
  /****************************/

  /* A "-grays" option overrides colors */
  if (XrmGetResource(FinalDatabase, "xcm.grays", "Xcm.Grays",
		     str_type, &value) == True) {
    if (!sscanf(value.addr, "%d", &ColorCount))
      crash("'-grays' must have an positive integer argument.");
    if (ColorCount <= 0)
      crash("positive number of colors is required.");
    ColorCount = MIN(ColorCount, MAX_COLORS);

    /* allocate the gray levels, from white (k=0) to half white */
    for (k = 0; k < ColorCount; k++) {
      graylevel = 4095 - k*2048/ColorCount;
      sprintf(buffer, "#%03x%03x%03x", graylevel, graylevel, graylevel);
      Colors[k] = parse_color(buffer);
    }
  }

  /* otherwise check for specific colors */

  else if (XrmGetResource(FinalDatabase, "xcm.colors", "Xcm.Colors",
		     str_type, &value) == True) {
    if (!sscanf(value.addr, "%d", &ColorCount))
      crash("a positive number of colors is required.");
    if (ColorCount <= 0)
      crash("a positive number of colors is required.");
    ColorCount = MIN(ColorCount, MAX_COLORS);

    /* Now allocate the specified colors -c1, -c2, . . . . */
    for (k = 0; k < ColorCount; k++) {
      sprintf(r_name, "xcm.color%d", k);
      sprintf(c_name, "Xcm.Color%d", k);
      if (XrmGetResource(FinalDatabase, r_name, c_name,
			 str_type, &value) == True) {
	strncpy(buffer, value.addr, value.size);
	Colors[k] = parse_color(buffer);
      }    
      else
	Colors[k] = PrimaryFG;
    }
  }

  /* Otherwise use the plane forground color for all the files */
  else {
    ColorCount = 1;
    Colors[0] = PlaneFG;
  }

  /* Finally, assign the pixel value for XOR drawing.  PlaneXOR when */
  /* exclusive OR with Plane BG yields PlaneFG.                      */

  PlaneXOR = PlaneFG ^ PlaneBG;

  /*****************/
  /* Button Colors */
  /*****************/

  /* button foreground color */
  if (XrmGetResource(FinalDatabase, "xcm.button.foreground", 
		     "Xcm.Button.Foreground", str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    ButtonFG = parse_color(buffer);
  }
  else
    ButtonFG = PrimaryFG;

  /* Button background color */
  if (XrmGetResource(FinalDatabase, "xcm.button.background", 
		     "Xcm.Button.Background", str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    ButtonBG = parse_color(buffer);
  }
  else
    ButtonBG = PrimaryBG;

  /* Button border color */
  if (XrmGetResource(FinalDatabase, "xcm.button.borderColor", 
		     "Xcm.Button.borderColor", str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    ButtonBD = parse_color(buffer);
  }
  else
    ButtonBD = PrimaryFG;

  /********************/
  /* The Aspect, Etc. */
  /********************/
  
  if (XrmGetResource(FinalDatabase, "xcm.aspect", "Xcm.Aspect",
		     str_type, &value) == True) {
    strncpy(buffer, value.addr, value.size);
    ScreenAspect = atof(buffer);
  }
  else {
    x = DisplayWidthMM(display, screen)/(1.0*DisplayWidth(display, screen));
    y = DisplayHeightMM(display, screen)/(1.0*DisplayHeight(display, screen));
    ScreenAspect = y/x;
  }

  if (XrmGetResource(FinalDatabase, "xcm.postscriptfile", "Xcm.PostScriptFile",
		     str_type, &value) == True ) 
    strncpy(PrintFilename, value.addr, value.size);
  if (XrmGetResource(FinalDatabase, "xcm.outfile", "Xcm.OutFile",
		     str_type, &value) == True ) 
    strncpy(GraphFilename, value.addr, value.size);
  if (EditFileName[0] != '\0');
/* Marshall switched 1-19-95  strcpy(GraphFilename, EditFileName);*/
  strcpy(EditFileName, GraphFilename);

  if (XrmGetResource(FinalDatabase, "xcm.grid", "Xcm.Grid",
		     str_type, &value) == True ) {
    ShowGrid = 1;
    Primary.plane.show_grid = 1;
    }
}

/**********************/
/* Additional Globals */
/**********************/

void setup_globals()
  /* PRE : 'merge_options' has been called, and the fonts have been loaded. */
  /* POST: Assigns the global variables, such as the colors, which depend   */
  /*       on the command-line options or the resources.                    */
{
  int geom_flags;

  /* parse the geometry */
  if (*GeomString != '\0') {
    geom_flags = XParseGeometry(GeomString, &PrimaryX, &PrimaryY, 
				&PrimaryWidth, &PrimaryHeight);
    SizeHints.width = PrimaryWidth;
    SizeHints.height = PrimaryHeight;

    /* Check for a negative x value */
    if (XValue & geom_flags) {
      if (XNegative & geom_flags)
	PrimaryX = DisplayWidth(display, screen) +
	  PrimaryX - PrimaryWidth;

      /* otherwise change the SizeHints so the user defines the position */
      SizeHints.flags |= USPosition;
      SizeHints.x = PrimaryX;
    }

    /* Check for a negative y value */
    if (YValue & geom_flags) {
      if (XNegative & geom_flags)
	PrimaryY = DisplayHeight(display, screen) +
	  PrimaryY - PrimaryHeight;

      /* otherwise change the SizeHints so the user defines the position */
      SizeHints.flags |= USPosition;
      SizeHints.y = PrimaryY;
    }
  }

  /* Parse the icon geometry */

  /* Determine the sizes of the buttons */
  ButtonWidth = XTextWidth(ButtonFont, "Static Options", 14) + 
    2*BUTTON_INNER_MARGIN + 2*BUTTON_BORDER_WIDTH;
  ButtonHeight = ButtonFont->ascent + 
    2*BUTTON_INNER_MARGIN + 2*BUTTON_BORDER_WIDTH;;

  RightMargin = XTextWidth(ButtonFont, "Grab Options", 12) +
    2*(BUTTON_INNER_MARGIN + BUTTON_OUTER_MARGIN + BUTTON_BORDER_WIDTH);

  /* Determine the sizes of the margins */
  LeftMargin = XTextWidth(LabelFont, "-0.000000000000000", 18) + 
    AXES_LABEL_X_OFFSET;
  TopMargin = PositionFont->ascent + PositionFont->descent + 
    AXES_LABEL_Y_OFFSET;

  RightMargin = ButtonWidth + 2*BUTTON_OUTER_MARGIN;
  BottomMargin = 2*(LabelFont->ascent + LabelFont->descent);

}

