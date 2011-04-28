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
/************************************/
/* Functions for creating new paths */
/************************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* CREATION DATE:  May 17, 1993                                             */
/* PURPOSE:        This module contains the C functions for creating        */
/*                 new paths in a plane window.                             */
/****************************************************************************/

/****************************************************************************/
/* Specifically the program needs to have a way of creating new "regions",  */
/* that is, polygonal closed curves which are not self-intersecting.  	    */
/* However the code should be more general than this, in case the user	    */
/* wishes to create new collections of line segments, to be viewed with the */
/* viewer.								    */
/* 									    */
/* A "path" is a doubly-linked list of 'path_node' structures, each node of */
/* which contains a point, either internally in the node, or via an pointer */
/* to an external 'path_node'.  The latter case is for the case where the   */
/* path is joined to another path.                                          */
/****************************************************************************/

/* 3-95 Marshall fixed bug in fg and bg when redrawing after Move */
/*  point, delete line etc..(added XSetFunction...)*/


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "globals.h"
#include "path.h"
#include "file.h"
#include "xplane.h"
#include "primary.h"
#include "menu.h"

/* Necessary global variables */

extern unsigned long  PlaneFG;
extern unsigned long  PlaneBG;
extern unsigned long  PlaneXOR;

extern simple_menu FileDisplayMenu;
extern simple_menu EditDisplayMenu;

static path_node *Common[MAX_COMMON_POINTS];

/********************************/
/* Doubly-Linked List Functions */
/********************************/

path_node *new_path_node( ptr )
     path_node *ptr;
  /* POST: Creates a new path node and appends it to 'ptr'.  If 'ptr' is  */
  /*       NULL, the new node can be used as the start of a doubly-linked */
  /*       list.  The 'next' field is NULL.  The return value is a        */
  /*       pointer to the newly created node.                             */
{
  path_node *new_node;

  if ((new_node = (path_node*) malloc(sizeof(path_node))) == NULL) {
    fprintf(stderr, "%s: out of dynamic memory\n", NAME);
    exit(1);
  }

  new_node->next = NULL;
  new_node->external = NULL;

  if (ptr != NULL) {
    ptr->next = new_node;
    new_node->prev = ptr;
  }
  else 
    new_node->prev = NULL;
  
  return(new_node);
}

path_node *insert_path_node( pos )
     path_node *pos;
  /* PRE : 'pos' references a valid path node.                          */
  /* POST: Creates a new path node and inserts it immediately following */
  /*       the node referenced by 'pos'.                                */
{
  path_node *next;

  next = pos->next;
  pos->next = new_path_node(pos);
  pos->next->next = next;
  pos->next->prev = pos;
  if (next != NULL)
    next->prev = pos->next;

  return(pos->next);
}

path_node *preinsert_path_node( pos )
     path_node *pos;
  /* PRE : 'pos' references a valid path node.                           */
  /* POST: Creates a new path node and inserts it immediately preceeding */
  /*       the node referenced by 'pos'.                                 */
{
  path_node *prev;

  prev = pos->prev;
  pos->prev = new_path_node(pos->prev);
  pos->prev->next = pos;
  pos->prev->prev = prev;
  if (prev != NULL)
    prev->next = pos->prev;

  return(pos->prev);
}

void free_path_node( pos )
     path_node *pos;
{
  if (pos != NULL)
    free(pos);
}

void kill_path_node( pos )
     path_node *pos;
  /* PRE : 'pos' references a valid node of a proper list.       */
  /* POST: Removes the node 'pos' from the list in which it lies */
{
  path_node *doomed;

  if (pos == NULL)
    return;

  if (pos->next != NULL)
    pos->next->prev = pos->prev;
  if (pos->prev != NULL)
    pos->prev->next = pos->next;
  
  free_path_node(pos);
}

void kill_path( path )
     path_node *path;
  /* POST: Removes the entire list of 'path' */
{
  path_node *ptr;
  path_node *next;

  ptr = path;
  while (ptr != NULL) {
    next = ptr->next;
    free_path_node(ptr);
    ptr = next;
  }
}

complex path_node_point( node )
     path_node *node;
  /* PRE : 'node' is valid                                            */
  /* POST: Returns the coordiantes of the point referenced by 'node'. */
{
  if (node->external != NULL) 
    return(*(node->external));
  else 
    return(node->point);
}

complex *path_node_ref( node )
     path_node *node;
  /* PRE : 'node' is valid.
  /* POST: Returns a pointer to the point of 'node', regardless of whether */
  /*       the coordinates are internal or external.                       */
{
  if (node->external != NULL)
    return(node->external);
  else
    return(&(node->point));
}

char same_pointQ( node1, node2 )
     path_node *node1, *node2;
  /* PRE : 'node1' and 'node2' both exist.                           */
  /* POST: Returns true if 'node1' and 'node2' have the same point.  */
{
  complex z1, z2;

  z1 = path_node_point(node1);
  z2 = path_node_point(node2);
  
  return( (z1.x == z2.x) && (z1.y == z2.y) );
}

char single_lineQ( path )
     path_node *path;
  /* POST: Returns True if 'path' has less than three nodes. */
{
  if (path == NULL)
    return(1);
  if (path->prev == NULL)
    if (path->next == NULL)
      return(1);
    else
      return( path->next->next == NULL);
  if (path->next == NULL)
    if (path->prev == NULL)
      return(1);
    else
      return(path->prev->prev == NULL);
  return(0);
}

void copy_path_point( dest_node, source_node )
     path_node *source_node;
     path_node *dest_node;
  /* POST: Copies the coordinates of the point referenced by 'source_node' */
  /*       to 'dest_node'.                                                 */
{
  dest_node->point = source_node->point;
  dest_node->external = source_node->external;
}

path_node *terminal_node( path )
     path_node *path;
  /* POST: Returns a pointer to the temrinal ndoe of 'path', or NULL */
  /*       if the path is empty.                                     */
{
  path_node *ptr;

  if (path == NULL)
    return(NULL);
  
  for (ptr = path; ptr->next != NULL; ptr++);
  return(ptr);
}

path_node *concat_paths( path1, path2 )
     path_node *path1;
     path_node *path2;
  /* POST: Concatenates 'path1' and 'path2', and returns the first node */
  /*       of the new path.  'path1' preceeds 'path2', of course.       */
{
  path_node *end1;

  end1 = terminal_node(path1);
  end1->next = path2;
  path2->prev = end1;
  return(path1);
}

path_node *reverse_path( path )
     path_node *path;
  /* POST: Reverses 'path' and returns a pointer to the first node */
{
  path_node *tmp, *ptr, *last;
  
  for (ptr = path; ptr != NULL; ptr = ptr->prev) {
    /* exchange 'next' and 'prev' */
    tmp = ptr->prev;
    ptr->prev = ptr->next;
    ptr->next = tmp;
    last = ptr;
  }
  /* ASSERT: 'last' references what is now the first node of the list */
  
  return(last);
}

/***************************/
/* File to Path Conversion */
/***************************/

path_node *point_to_path( source )
     point_type *source;
{
  path_node  *ptr, *first;
  point_type *points;
  int        k;

  ptr = NULL;
  first = NULL;
  for (k = 0; k < source->count; k++) {
    ptr = new_path_node(ptr);
    if ( k == 0 )
      first = ptr;
    ptr->point = source->points[k];
    ptr->external = NULL;
  }
  return(first);
}

void file_to_graph( graph, file )
     graph_type *graph;
     point_type *file;
{
  point_type *ptr;
  int k = 0;

  for (ptr = file; ptr != NULL; ptr = ptr->next) {
    if ((graph->paths[k] = point_to_path(ptr)) != NULL)
      k++;
  }
  graph->path_count = k;
}


/********************************/
/* Graph Manipulation Functions */
/********************************/

char write_graph( graph, filename )
     graph_type *graph;
     char       filename[];
{
  int       k;
  path_node *node;
  complex   z;
  FILE      *OutFile;

  if ( (OutFile = fopen(filename, "w")) == NULL )
    return(0);

  for (k = 0; k < graph->path_count; k++) {
    for (node = graph->paths[k]; node != NULL; node = node->next) {
      z = path_node_point(node);
      fprintf(OutFile, "% 19.16E % 19.16E\n", z.x, z.y);
    }
    if (k != graph->path_count-1)
      fprintf(OutFile, "\n");
  }
  fclose(OutFile);
  return(1);
}

void store_path( obj )
     graph_type *obj;
  /* POST: Includes the current path into the path.  If either endpoint */
  /*       of the current path matches any endpoint of any of the other */
  /*       paths in 'obj', the current path is prepended or appended    */
  /*       to that endpoint.  Otherwise the current path is placed in   */
  /*       its own cell.                                                */
{
  path_node *end_node, *current_end_node;
  path_node *new_path;
  int       k;

  if (obj->current_path == NULL)
    return;

  current_end_node = terminal_node(obj->current_path);
  for (k = 0; k < obj->path_count; k++) {
    end_node = terminal_node(obj->paths[k]);
    
    /* Check if the current path starts at the start of the kth path */
    if (same_pointQ(obj->current_path, obj->paths[k])) {
      /* revese the current path, and prepend */
/*      new_path = reverse_path(obj->current_path);
      obj->paths[k] = concat_paths(new_path, obj->paths[k]);
      return;*/
    }

    /* Check if the current path starts at the end of the kth path */
    if (same_pointQ(obj->current_path, obj->paths[k])) {
/*      obj->paths[k] = concat_paths(obj->paths[k], obj->current_path);
      return*/
    }
  }

  obj->paths[obj->path_count] = obj->current_path;
  (obj->path_count)++;
  obj->current_path = NULL;
  obj->path_end = NULL;
}

void delete_path( graph, index )
     graph_type *graph;
     int        index;
  /* PRE : 'index' is a valid cell of 'graph->paths'.     */
  /* POST: Removes path with index 'index' from 'graph'.  */
{
  int k;

  kill_path(graph->paths[index]);
  for (k = index+1; k < graph->path_count; k++)
    graph->paths[k-1] = graph->paths[k];
  (graph->path_count)--;
  graph->paths[graph->path_count] = NULL;
}

/**************************************/  
/* Line and Point Searching Functions */
/**************************************/  

path_node *find_point( list, z, tol )
     path_node *list;
     complex   z;
     double    tol;
  /* POST: Searches the list for the first point which is within 'tol' in */
  /*       both directions of the point z = (x, y), and returns the first */
  /*       point in 'list' which matches, or NULL if node does.           */
  /* NOTE: This function only seaches the nodes for which the point is    */
  /*       explicitly included, not those with indirect references.       */
{
  path_node *ptr;

  for (ptr = list; ptr != NULL; ptr = ptr->next) {
    if (ptr->external == NULL) 
      if ( ( z.x-tol <= ptr->point.x) && (ptr->point.x <= z.x+tol) &&
	   ( z.y-tol <= ptr->point.y) && (ptr->point.y <= z.y+tol) )
	return(ptr);
  }
  return(NULL);
}

char match_lineQ( z, z1, z2, tol )
     complex z;
     complex z1;
     complex z2;
     double tol;
  /* POST: Returns True if z = (x,y) lies within 'tol' of the line segment */
  /*       joining z1 = (x1, y1) and z2 = (x2, y2).                        */
{
  double x0, y0;
  double dx, dy;
  double len;

  /* The method is to isometrically change coordinates so that the line */
  /* segment (x1,y1)-(x2,y2) lies along the x-axis with (x1,y1) at the  */
  /* origin.  Then the test is simple.  This method requires only one   */
  /* call to the sqrt function, and a few multiplications.              */

  dx = (z2.x - z1.x);
  dy = (z2.y - z1.y);

  len = sqrt(dx*dx + dy*dy);

  z.x -= z1.x;
  z.y -= z1.y;

  /* Now rotate z to (x0,y0) by the negative of the angle of the line */
  if (len > 0.0) {
    x0 = (dx*z.x + dy*z.y)/len;
    y0 = (dy*z.x - dx*z.y)/len;
  }

  return( (-tol <= x0) && (x0 <= len + tol) &&
	  (-tol <= y0) && (y0 <= tol)          );
}
  
path_node *match_line( list, z, tol )
     path_node *list;
     complex   z;
     double    tol;
  /* POST: Returns a pointer to the first line segment in 'list' which     */
  /*       lies withing 'tol' of the point (x,y), that is the first line   */
  /*       segment for which contains at least one point within a 'tol'-   */
  /*       neighborhood of (x,y).                                          */
  /*       The returned pointer references the first endpoint of the line. */
  /* NOTE: This function searches all segments of 'list'.                  */
{
  path_node *ptr;
  complex   z1, z2;

  if (list == NULL)
    return(NULL);

  for (ptr = list; ptr->next != NULL; ptr = ptr->next) {
    z1 = path_node_point(ptr);
    z2 = path_node_point(ptr->next);
    if (match_lineQ(z, z1, z2, tol))
      return(ptr);
  }
  return(NULL);
}
  
/******************************************/
/* High-Level Point/Line Search Functions */
/******************************************/

double pixel_tolerance( obj )
     plane_window *obj;
  /* POST: Returns the world size of PIXEL_TOL pixels. */
{
  return(PIXEL_TOL*(obj->scale)/(1.0*obj->width));
}

path_node *find_graph_point( obj, z )
     primary_window *obj;
     complex        z;
  /* POST: Searches each path of obj->graph (including the current path) */
  /*       for a node with coordinates "near" 'z', and returns the first */
  /*       match, or NULL if none is found.  "Near" means within         */
  /*       PIXEL_TOL of 'z' according to the current plane.              */
  /*       The point referenced by the end of the current path is not    */
  /*       matched.                                                      */
  /* NOTE: This function only seaches the nodes for which the point is   */
  /*       explicitly included, not those with external references.      */
{ 
  path_node *value = NULL;
  path_node *penultimate;
  double tol;
  int    k;

  tol = pixel_tolerance(&(obj->plane));
  for (k = 0; k < obj->graph.path_count; k++) {
    if ((value = find_point(obj->graph.paths[k], z, tol)) != NULL)
      return(value);
  }

  /* Check the current path as well */
  if (obj->graph.current_path != NULL) {
    /* Temporiarily remove the last node of the current path */
    penultimate = obj->graph.path_end->prev;
    penultimate->next = NULL;

    value = find_point(obj->graph.current_path, z, tol);
    penultimate->next = obj->graph.path_end;
  }
  
  return(value);
}

path_node *find_graph_line( obj, z )
     primary_window *obj;
     complex        z;
{
  int       k;
  double    tol;
  path_node *value;

  tol = pixel_tolerance(&(obj->plane));
  for (k = 0; k < obj->graph.path_count; k++) 
    if ((value = match_line(obj->graph.paths[k], z, tol)) != NULL)
      return(value);
  return(NULL);
}


/***************************/
/* Mode Changing Functions */
/***************************/

void set_graph_mode( obj, new_mode )
     primary_window  *obj;
     enum graph_mode new_mode;
  /* PRE : 'new_mode' is valid.                                           */
  /* POST: sets the graph modeof 'obj', and changes the function pointers */
  /*       to the appropriate values.                                     */
{
  obj->graph.mode = new_mode;

  switch (new_mode) {
    /* If no mode is active, return the mouse buttons to normal */
  case g_inactive :
    obj->plane.button1 = grab_translate;
    obj->plane.button2 = plane_translate;
    obj->plane.button3 = plane_zoom;
    break;
    
  case g_creating :
    obj->plane.button1 = handle_create_button;
    obj->plane.button2 = handle_create_button;
    obj->plane.button3 = handle_create_button;
    break;

  case g_move_point :
    obj->plane.button1 = move_path_point;
    obj->plane.button2 = plane_translate;
    obj->plane.button3 = plane_zoom;
    break;

  case g_kill_point :
    obj->plane.button1 = kill_path_point;
    obj->plane.button2 = plane_translate;
    obj->plane.button3 = plane_zoom;
    break;

  case g_kill_line :
    obj->plane.button1 = kill_path_line;
    obj->plane.button2 = plane_translate;
    obj->plane.button3 = plane_zoom;
    break;

  case g_break_line :
    obj->plane.button1 = break_path_line;
    obj->plane.button2 = plane_translate;
    obj->plane.button3 = plane_zoom;
    break;

  case g_break_point :
    obj->plane.button1 = break_path_point;
    obj->plane.button2 = plane_translate;
    obj->plane.button3 = plane_zoom;
    break;
  }
}


/**********************/
/* Graphics Functions */
/**********************/

double prec_round( source, precision )
     double source;
     double precision;
  /* PRE : 'precision' is positive.                                    */
  /* POST: rounds 'source' to the precision 'precision'.  For example, */
  /*       prec_round(3.141592654, 0.01) = 3.14.                       */
{
  return( precision*floor( (source/precision) + 0.5 ) );
}

void draw_path_line( obj, node )
     primary_window *obj;
     path_node      *node;
{
  XPoint p;

  if (node->next != NULL) {
    if ((EditDisplayMenu.index == 0) || (EditDisplayMenu.index == 3))
      plane_clip_line(&(obj->plane), 
		      path_node_point(node), path_node_point(node->next));
    else if ((EditDisplayMenu.index == 1) || (EditDisplayMenu.index == 2)) {
      plane_draw_point(&(obj->plane), path_node_point(node));
      plane_draw_point(&(obj->plane), path_node_point(node->next));
    }

    if ((EditDisplayMenu.index >= 2) && (EditDisplayMenu.index < 4)) {
      plane_draw_rect(&(obj->plane), path_node_point(node));
      plane_draw_rect(&(obj->plane), path_node_point(node->next));
    }
  }
}

void print_path_line( path_node *node )
{
  complex z1, z2;

  z1 = path_node_point(node);
  z2 = path_node_point(node->next);
  printf("(%f, %f)-(%f, %f)", z1.x, z1.y, z2.x, z2.y);

  if (node->external == NULL)
    printf("   internal");
  else
    printf("   external");

  if (node->next->external == NULL)
    printf(" internal");
  else
    printf(" external");
  printf("\n");
    
}

void draw_path( obj, path )
     primary_window *obj;
     path_node      *path;
  /* POST: Draws 'path' into the plane window of 'obj'.  */
{
  static complex points[MAX_POINTS];
  path_node *ptr;
  int k = 0;

  for (ptr = path; ptr != NULL; ptr = ptr->next) {
    points[k] = path_node_point(ptr);
    k++;
    if (k == MAX_POINTS) {
      /* dump and continue */
      if ((EditDisplayMenu.index == 0) || (EditDisplayMenu.index == 3))
	plane_general_clip(&(obj->plane), points, k);
      else if ((EditDisplayMenu.index == 1) || (EditDisplayMenu.index == 2)) 
	plane_draw_points(&(obj->plane), points, k);
      if ((EditDisplayMenu.index >= 2) && (EditDisplayMenu.index < 4)) 
	plane_draw_rects(&(obj->plane), points, k);

      k = 0;
    }
  }

  /* dump the remaining lines/points/rectangles */
  if (k > 0) {
    if ((EditDisplayMenu.index == 0) || (EditDisplayMenu.index == 3))
      plane_general_clip(&(obj->plane), points, k);
    else if ((EditDisplayMenu.index == 1) || (EditDisplayMenu.index == 2)) 
      plane_draw_points(&(obj->plane), points, k);
    if ((EditDisplayMenu.index >= 2) && (EditDisplayMenu.index < 4)) 
      plane_draw_rects(&(obj->plane), points, k);
  }
}

void draw_graph( obj )
     primary_window *obj;
{
  path_node *ptr;
  int       k;

  for (k = 0; k < obj->graph.path_count; k++)
    draw_path(obj, obj->graph.paths[k]);

  if (obj->graph.current_path != NULL) 
    draw_path(obj, obj->graph.current_path);
}
     
complex grab_path_point( node, primary, x, y)
     path_node      *node;
     primary_window *primary;     
     int            x, y;
  /* POST: Returns the world coordinates of the pointer, checking to see */
  /*       if the pointer is selecting any of the points in 'obj'.       */
  /* NOTE: The pointer coordinates are assumed to be relative to the     */
  /*       plane window.                                                 */
{
  complex   value, z, *z_ptr;
  XPoint    pointer;
  path_node *point;

  pointer.x = x;
  pointer.y = y;
  p_view_to_world(&(primary->plane), &value, pointer);

  /* round the coordinates of value to the nearest 100th decade */
/*  value.x = prec_round(value.x, primary->plane.decade/100);
  value.y = prec_round(value.y, primary->plane.decade/100);
  
  node->point = value;
  node->external = NULL; */

/*  if ((point = find_graph_point(primary, value)) != NULL) 
    node->external = &(point->point);
  else {
    node->point = value;
    node->external = NULL;
  } */

  if ((z_ptr = grab_point(primary, x, y, &z)) != NULL)
    node->external = z_ptr;
  else {
    node->point = z;
    node->external = NULL;
  } 

  return(value);
}
  

/* The ButtonPress events are received by the plane */

void handle_create_button( obj, event )
     plane_window *obj;
     XEvent       *event;
{
  XPoint p;
  primary_window *parent;
  unsigned long white;
  complex z;

  parent = (primary_window*) obj->parent_struct;

  switch (event->xbutton.button) {
    /* The left button gets a new point */
  case (Button1) :
    if (parent->graph.current_path == NULL) {
      /* start a new path */
      parent->graph.current_path = new_path_node(NULL);
      parent->graph.path_end = new_path_node(parent->graph.current_path);
      
      grab_path_point(parent->graph.current_path, parent,
		      event->xbutton.x, event->xbutton.y);
      copy_path_point(parent->graph.path_end, parent->graph.current_path);
    }
    
    else {
      /* erase the old line, with GXxor */
      XSetFunction(display, parent->plane_gc, GXxor);
      XSetForeground(display, parent->plane_gc, PlaneXOR);
      draw_path_line(parent, parent->graph.path_end->prev);
      
      /* Get the true coordinates of the end of the old line */
      grab_path_point(parent->graph.path_end, parent,
		      event->xbutton.x, event->xbutton.y);
      
      /* Draw the old line with GXcopy */
      XSetFunction(display, parent->plane_gc, GXcopy);
      XSetForeground(display, parent->plane_gc, PlaneFG);
      draw_path_line(parent, parent->graph.path_end->prev);
      
      /* create a new node */
      parent->graph.path_end = new_path_node(parent->graph.path_end);
      
      /* assign the new node */
      z = path_node_point(parent->graph.path_end->prev);
      parent->graph.path_end->point = z; /* the new node is always internal */
  
      /* ASSERT: the last two nodes of the current path have the same */
      /*         coordinates.                                         */
    }
    break;

    /* Button2 closes the path */
  case (Button2) :
    if (parent->graph.current_path != NULL) {
      /* erase the old line, with GXxor */
      XSetFunction(display, parent->plane_gc, GXxor);
      XSetForeground(display, parent->plane_gc, PlaneXOR);
      draw_path_line(parent, parent->graph.path_end->prev);
      
      /* set the 'external' field of the last node to reference the first */
      parent->graph.path_end->external = 
	path_node_ref(parent->graph.current_path);
      
      /* Draw the new line with GXcopy */
      XSetFunction(display, parent->plane_gc, GXcopy);
      XSetForeground(display, parent->plane_gc, PlaneFG);
      draw_path_line(parent, parent->graph.path_end->prev);
    }
/*    set_graph_mode(parent, g_inactive);*/
    store_path(&(parent->graph));
    break;
    
    /* Otherwise the path ends at the pointer */
  case (Button3) :
    
    if (parent->graph.current_path != NULL) {
      /* erase the old line, with GXxor */
      XSetFunction(display, parent->plane_gc, GXxor);
      XSetForeground(display, parent->plane_gc, PlaneXOR);
      draw_path_line(parent, parent->graph.path_end->prev);
      
      /* Get the true coordinates of the end of the old line */
      grab_path_point(parent->graph.path_end, parent,
		      event->xbutton.x, event->xbutton.y);
      
      /* Draw the last line with GXcopy */
      XSetFunction(display, parent->plane_gc, GXcopy);
      XSetForeground(display, parent->plane_gc, PlaneFG);
      
      draw_path_line(parent, parent->graph.path_end->prev);
    }
/*    set_graph_mode(parent, g_inactive);*/
    store_path(&(parent->graph));
    break;
  }
}  

/* The motion events are received by the primary */

void handle_create_motion( obj, event )
     primary_window *obj;
     XEvent         *event;
  /* PRE: Either the graph of 'obj' has current path NULL, or the current */
  /*      path contains at least two nodes.                               */
{
  XPoint p;
  unsigned long mask;

  if (obj->graph.current_path == obj->graph.path_end) {
    /* no startpoint has been set, so don't do anything */
  }
  else {
    /* erase the old line */
    XSetFunction(display, obj->plane_gc, GXxor);
    XSetForeground(display, obj->plane_gc, PlaneXOR);
    draw_path_line(obj, obj->graph.path_end->prev);

    p = w_primary_to_plane(obj, event->xmotion.x, event->xmotion.y);
    p_view_to_world(&(obj->plane), &(obj->graph.path_end->point), p);
    draw_path_line(obj, obj->graph.path_end->prev);
  }
}

void handle_motion( obj, event )
     primary_window *obj;
     XEvent         *event;
{
  XPoint p;

  switch (obj->graph.mode) 
    {
      case (g_creating) : {
	/* A new path is being created */
	handle_create_motion(obj, event);
      }
    }
}

/**************************************/
/* Functions to handle moving a point */
/**************************************/

void draw_common_lines( primary, points )
     primary_window *primary;
     path_node      *points[];
  /* PRE : 'points' is a NULL terminated array of node in the same graph   */
  /*       which share the point of the first element.                     */
  /* POST: Draws all the line segments in the graph which have an endpoint */
  /*       in 'points'.                                                    */
{
  int k;
  path_node *node;

  for (k = 0; points[k] != NULL; k++) {
    node = points[k];
    if (node->prev != NULL) 
      draw_path_line(primary, node->prev);
    if (node->next != NULL)
      draw_path_line(primary, node);
  }
}

void find_common_points( graph, points, node )
     graph_type *graph;
     path_node  *points[];
     path_node  *node;
  /* PRE : 'points' is an array containing at least MAX_COMMON_POINTS      */
  /*       cells, and 'node' has an internal point.                        */ 
  /* POST: Searches through all the points in 'graph' for nodes which have */
  /*       the point of 'node' as 'external', and places them in the array */
  /*       'points'.  The array is NULL terminated.                        */
  /*       The current path is not included in this search.                */
  /*       The first node of 'points' is 'node'.                           */
{
  path_node *ptr;
  int       path = 0;
  int       k = 1;

  points[0] = node;
  for (path = 0; path < graph->path_count; path++) {
    for (ptr = graph->paths[path]; ptr != NULL; ptr = ptr->next) {
      if ( (ptr->external != NULL) &&
	   (ptr->external->x == node->point.x) && 
	   (ptr->external->y == node->point.y) ) {
	if (k < MAX_COMMON_POINTS-1) {
	  points[k] = ptr;
	  k++;
	}
      }
    }
  }
  points[k] = NULL;
}

void move_path_point( obj, event )
     plane_window *obj;
     XEvent       *event;
  /* PRE : 'event' contains a ButtonPress event. */
{
  path_node        *primary_node;
  XPoint           pointer;
  complex          z;
  primary_window   *parent;
  int              k;

  parent = (primary_window*) obj->parent_struct;

  /* get the world coordinates of the pointer */
  pointer.x = event->xbutton.x;
  pointer.y = event->xbutton.y;
  p_view_to_world(obj, &z, pointer);

  /* Find the point in the graph the pointer is pointing to */
  if ( (primary_node = find_graph_point(parent, z)) == NULL )
    return;

  /* Now find all the other nodes which share this point */
  find_common_points(&(parent->graph), Common, primary_node);

  /* Erase the old lines joining this point */
  XSetFunction(display, parent->plane_gc, GXcopy);
  XSetForeground(display, parent->plane_gc, PlaneBG);
  draw_common_lines(parent, Common) ;

  /* Now draw the old lines, with exclusive or */
  XSetFunction(display, parent->plane_gc, GXxor);
  XSetForeground(display, parent->plane_gc, PlaneXOR);
  draw_common_lines(parent, Common);
  XFlush(display);

  /* Now loop until a non-ButtonMotion event appears */
  
  /* wait for an event, if there isn't one. */
  XPeekEvent(display, event);

  while (collect_motions(event)) {
    /* erase the old lines, with XOR */
    draw_common_lines(parent, Common);

    /* Calculate the new position */
    pointer.x = event->xmotion.x;
    pointer.y = event->xmotion.y;
    p_view_to_world(obj, &z, pointer);    
    primary_node->point = z;

    /* draw the new lines */
    draw_common_lines(parent, Common);
  } 

  /* Now grab the new point.  To prevent 'find_path_point' from matching */
  /* a the point of 'primary_node', we temporarily set it to non-null    */
  primary_node->external = &(primary_node->point);
  grab_path_point(primary_node, parent, (int) pointer.x, (int) pointer.y);
  
  /* Check to see if a new point was matched */
  if (primary_node->external != NULL) {
    for (k = 0; Common[k] != NULL; k++)
      Common[k]->external = primary_node->external;
  }
  else
    primary_node->external = NULL;

/*Marshall added this next crude patch to fix fg and bg*/
  XSetFunction(display, parent->plane_gc, GXcopy);
  redraw_plane_window(obj);
}

/************************************/
/* Functions for "Breaking" a Point */
/************************************/

void break_path_point(obj, event )
     plane_window *obj;
     XEvent       *event;
{
  path_node        *primary_node;
  XPoint           pointer;
  complex          z;
  primary_window   *parent;
  int              k;

  parent = (primary_window*) obj->parent_struct;

  /* get the world coordinates of the pointer */
  pointer.x = event->xbutton.x;
  pointer.y = event->xbutton.y;
  p_view_to_world(obj, &z, pointer);

  /* Find the point in the graph the pointer is pointing to */
  if ( (primary_node = find_graph_point(parent, z)) == NULL )
    return;

  /* Now find all the other nodes which share this point */
  find_common_points(&(parent->graph), Common, primary_node);

  /* change all the "common" nodes from external to internal */
  for (k = 1; Common[k] != NULL; k++) {
    Common[k]->point = *(Common[k]->external);
    Common[k]->external = NULL;
  }

  move_path_point(obj, event);
}


/**********************************/
/* Functions for Deleting a Point */
/**********************************/

void kill_path_point( obj, event )
     plane_window *obj;
     XEvent       *event;
{
  path_node        *primary_node, *ptr;
  int              path;
  XPoint           pointer;
  complex          z0, z;
  primary_window   *parent;

  parent = (primary_window*) obj->parent_struct;

  /* get the world coordinates of the pointer */
  pointer.x = event->xbutton.x;
  pointer.y = event->xbutton.y;
  p_view_to_world(obj, &z, pointer);

  /* Find the node which contains the point */
  if ( (primary_node = find_graph_point(parent, z)) == NULL )
    return;
  z0.x = primary_node->point.x;
  z0.y = primary_node->point.y;
  
  /* Delete all the lines which share the point */

  for (path = 0; path < parent->graph.path_count; path++) {
    for (ptr = parent->graph.paths[path]; ptr != NULL; ptr = ptr->next) {
      z = path_node_point(ptr);
      if ( (z.x == z0.x) && (z.y == z0.y) ) {

	/* The entire path has to be deleted if there are only two nodes */
	if (single_lineQ(ptr))
	  delete_path(&(parent->graph), path);

	/* If the node is the first in the path, the corresponding cell */
        /* of 'parent->graph.paths' must be changed.                    */
	else if (ptr->prev == NULL) {
	  parent->graph.paths[path] = ptr->next;
	  ptr->next->prev = NULL;
	  free_path_node(ptr);
	}

	/* otherwise just kill the node */
	else 
	  kill_path_node(ptr);
      }
    }
  }
  redraw_plane_window(obj);
}  

/*********************************/
/* Functions for deleting a line */
/*********************************/

void kill_path_line( obj, event )
     plane_window *obj;
     XEvent       *event;
{
  XPoint           pointer;
  complex          z;
  primary_window   *parent;
  path_node        *line;
  int              k;

  parent = (primary_window*) obj->parent_struct;

  /* get the world coordinates of the pointer */
  pointer.x = event->xbutton.x;
  pointer.y = event->xbutton.y;
  p_view_to_world(obj, &z, pointer);

  /* Find the line near 'z' */
  if ( (line = find_graph_line(parent, z)) == NULL )
    return;

  /* Find any lines which contain either endpoint of the deleted  */
  /* externally, and give them a direct (internal) reference.     */

  find_common_points(&(parent->graph), Common, line);

  for (k = 1; Common[k] != NULL; k++) {
    Common[k]->point = *(Common[k]->external);
    Common[k]->external = NULL;
  }

  find_common_points(&(parent->graph), Common, line->next);

  for (k = 1; Common[k] != NULL; k++) {
    Common[k]->point = *(Common[k]->external);
    Common[k]->external = NULL;
  }


  /* If the path has only one line, delete it */
  if (single_lineQ(line)) {
    for (k = 0; k < parent->graph.path_count; k++)
      if (parent->graph.paths[k] == line)
	delete_path(&(parent->graph), k);
  }

  /* If the line is the first in a path, the parent must be modified */
  else if (line->prev == NULL) {
    /* find the reference in graph.paths */
    k = 0;
    while ((k < parent->graph.path_count) && 
	   (parent->graph.paths[k] != line))
      k++;
    if (k == parent->graph.path_count)
      /* Something's wrong! */
      return;
    
    parent->graph.paths[k] = line->next;
    kill_path_node(line);
  }

  /* If the line is at the end of a path, delete that last node */
  else if (line->next->next == NULL) {
    kill_path_node(line->next);
  }

  /* otherwise split the path, and add a new cell to graph.paths */
  else {
    parent->graph.current_path = line->next;
    line->next->prev = NULL;
    store_path(&(parent->graph));
    line->next = NULL;
  }

/*Marshall added this next crude patch to fix fg and bg*/
  XSetFunction(display, parent->plane_gc, GXcopy);
  redraw_plane_window(obj);
}
    
  
/******************************************/
/* Functions to operate "breaking" a line */
/******************************************/

void break_path_line( obj, event )
     plane_window *obj;
     XEvent       *event;
{
  XPoint           pointer;
  complex          z;
  primary_window   *parent;
  path_node        *line, *new;

  parent = (primary_window*) obj->parent_struct;

  /* get the world coordinates of the pointer */
  pointer.x = event->xbutton.x;
  pointer.y = event->xbutton.y;
  p_view_to_world(obj, &z, pointer);

  /* Find the line near 'z' */
  if ( (line = find_graph_line(parent, z)) == NULL )
    return;

  /* Erase the line */
  XSetFunction(display, *(obj->gc), GXcopy);
  XSetForeground(display, *(obj->gc), PlaneBG);
  draw_path_line(parent, line);

  /* now "break" the line */
  new = insert_path_node(line);
  new->point = z;

/*  Common[0] = line;
  Common[1] = new;
  Common[2] = NULL;*/

  /* draw the two new lines, with XOR */
  XSetFunction(display, parent->plane_gc, GXxor);
  XSetForeground(display, parent->plane_gc, PlaneXOR);
  draw_path_line(parent, line);
  draw_path_line(parent, new);
  XFlush(display);

  /* Now loop until a non-ButtonMotion event appears */
  
  /* wait for an event, if there isn't one. */
  XPeekEvent(display, event);

  while (collect_motions(event)) {
    /* erase the old lines, with XOR */
    draw_path_line(parent, line);
    draw_path_line(parent, new);

    /* Calculate the new position */
    pointer.x = event->xmotion.x;
    pointer.y = event->xmotion.y;
    p_view_to_world(obj, &z, pointer);    
    new->point = z;

    /* draw the new lines */
    draw_path_line(parent, line);
    draw_path_line(parent, new);
  } 

  /* Now grab the new point.  To prevent 'find_path_point' from matching */
  /* a the point of 'new', we temporarily set its external field  to     */
  /* the value of its point.  If 'grab_path_line' doesn't match an       */
  /* external point, then the field will be returned to NULL.            */

  new->external = &(new->point);
  grab_path_point(new, parent, (int) pointer.x, (int) pointer.y);

/*Marshall added this next crude patch to fix fg and bg*/
  XSetFunction(display, parent->plane_gc, GXcopy);
  redraw_plane_window(obj);
}
  






