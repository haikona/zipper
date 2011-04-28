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
/*****************************/
/* Complex Arithmetic Module */
/*****************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/* PURPOSE:        This module contains C functions for operating on        */
/*                 an implementation of complex variables.                  */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "complex.h"

void c_assign( dest, x, y)
     complex *dest;
     double   x;
     double   y;
{
  dest->x = x;
  dest->y = y;
}

void c_infinity( dest )
     complex *dest;
{
  dest->x = FLT_MAX;
  dest->y = FLT_MAX;
}

void c_copy( dest, source )
     complex *dest;
     complex source;
{
  dest->x = source.x;
  dest->y = source.y;
}

void c_add( addend, augend, sum )
     complex addend;
     complex augend;
     complex *sum;
{
  sum->x = addend.x + augend.x;
  sum->y = addend.y + augend.y;
}

void c_sub( minuend, subtrahend, difference )
     complex minuend;
     complex subtrahend;
     complex *difference;
{
  difference->x = minuend.x - subtrahend.x;
  difference->y = minuend.y - subtrahend.y;
}

void c_neg( source )
     complex *source;
{
  source->x *= -1;
  source->y *= -1;
}

void c_mul( multiplicand, multiplier, product )
     complex multiplicand;
     complex multiplier;
     complex *product;
{
  complex z;

  z.x = multiplicand.x*multiplier.x - multiplicand.y*multiplier.y;
  z.y = multiplicand.x*multiplier.y + multiplicand.y*multiplier.x;

  c_copy(product, z);
}

void c_scale( scale, source )
     double scale;
     complex *source;
{
  source->x *= scale;
  source->y *= scale;
}

void c_conj( source )
     complex *source;
{
  source->y *= -1;
}

void c_copy_conj( dest, source )
     complex *dest;
     complex source;
{
  dest->x = source.x;
  dest->y = -source.y;
}

char c_zeroQ( source )
     complex source;
{
  return ( (source.x == 0) && (source.y == 0) );
}
  
char c_div( dividend, divisor, quotient )
     complex dividend;
     complex divisor;
     complex *quotient;
{
  complex z;

  if (c_zeroQ(divisor)) {
    c_infinity(quotient);
    return(0);
  }
  else {
    z.x = dividend.x*divisor.x + dividend.y*divisor.y;
    z.y = -dividend.x*divisor.y + dividend.y*divisor.x;  
    c_copy(quotient, z);
    c_scale(1/(divisor.x*divisor.x + divisor.y*divisor.y), quotient);
    return(1);
  }
}

double c_abs_sqr( source )
     complex source;
{
  return( source.x*source.x + source.y*source.y);
}

double c_abs( source )
     complex source;
{
  return( sqrt(c_abs_sqr(source)));
}

double c_arg( source )
     complex source;
{
  double arg;

  if (c_zeroQ(source))
    return(0.0);
  else {
    arg = atan2(source.y, source.x);
/*    if (arg < 0)
      arg += 6.2831853; */
    return(arg);
  }
}

double c_positive_arg( source )
     complex source;
{
  double arg;

  if (c_zeroQ(source))
    return(0.0);
  else {
    arg = atan2(source.y, source.x);
    if (arg < 0)
      arg += 6.2831853; 
    return(arg);
  }
}

char c_polar( source )
     complex *source;
{
  double abs, arg;

  abs = c_abs(*source);
  arg = c_positive_arg(*source);
  if (arg == C_ERROR)
    return(C_ERROR);
  else {
    source->x = abs;
    source->y = arg;
    return(1);
  }
}

void c_rect( source )
     complex *source;
{
  double x, y;

  x = source->x*cos(source->y);
  y = source->x*sin(source->y);

  source->x = x;
  source->y = y;
}
  
void c_zero( dest )
     complex *dest;
{
  dest->x = 0.0;
  dest->y = 0.0;
}

void c_one( dest )
     complex *dest;
{
  dest->x = 1.0;
  dest->y = 0.0;
}

void c_inv( source )
     complex *source;
{
  complex one;

  c_one(&one);
  c_div(one, *source, source);
}

/* Complex Elementary Functions */

void c_power( dest, source, power )
     complex *dest;
     complex source;
     int     power;
{
  int     k;
  complex z;

  if (power < 0) {
    c_power(dest, source, -power);
    c_one(&z);
    c_div(z, *dest, dest);
  }
  else {
    c_one(&z);
    for (k = 1; k <= power; k++) 
      c_mul(source, z, &z);
    c_copy(dest, z);
  }
}
    
void c_irreducible( dest, source, constant, coeff, power )
     complex *dest;
     complex source;
     complex constant;
     complex coeff;
     int     power;
{
  complex z1, z2;

  c_mul(coeff, source, &z1); 
  c_add(z1, constant, &z2);
  c_power(dest, z2, power);
}

/********************************/
/* complex elementary functions */
/********************************/

/* exponential and logarithmic functions */

void c_exp( dest, source )
     complex *dest;
     complex source;
{
  double mag;

  mag = exp(source.x);
  dest->x = cos(source.y)*mag;
  dest->y = sin(source.y)*mag;
}

void c_exp_iz( dest, source )
     complex *dest;
     complex source;
{
  complex z;

  z.x = -source.y;
  z.y = source.x;
  c_exp(dest, z);
}

void c_log( dest, source )
     complex *dest;
     complex source;
  /* Returns 0 if source lies on the usual branch cut */
{
  double arg;

  /* check for a branch cut */
  if ((source.y == 0) && (source.x <= 0)) {
    c_zero(dest);
    return;
  }
  
  dest->x = log(c_abs(source));
  dest->y = c_arg(source);
}
  
/* trig functions */

void c_sin( dest, source )
     complex *dest;
     complex source;
{
  complex z1, z2, z3;

  c_exp_iz(&z1, source);
  c_neg(&source);
  c_exp_iz(&z2, source);
  c_sub(z1, z2, &z1);
  c_assign(&z3, 0.0, 2.0);
  c_div(z1, z3, dest);
}
 
void c_cos( dest, source )
     complex *dest;
     complex source;
{
  complex z1, z2;

  c_exp_iz(&z1, source);
  c_neg(&source);
  c_exp_iz(&z2, source);
  c_add(z1, z2, dest);
  c_scale(0.5, dest);
}

void c_tan( dest, source )
     complex *dest;
     complex source;
{
  complex sin_z, cos_z;

  c_cos(&cos_z, source);
  if (c_zeroQ(cos_z)) {
    c_infinity(dest);
    return;
  }

  c_sin(&sin_z, source);
  c_div(sin_z, cos_z, dest);
}

void c_cot( dest, source )
     complex *dest;
     complex source;
{
  complex sin_z, cos_z;

  c_sin(&sin_z, source);
  if (c_zeroQ(sin_z)) {
    c_infinity(dest);
    return;
  }

  c_cos(&cos_z, source);
  c_div(cos_z, sin_z, dest);
}

void c_sec( dest, source )
     complex *dest;
     complex source;
{
  complex cos_z;

  c_cos(&cos_z, source);
  if (c_zeroQ(cos_z)) {
    c_infinity(dest);
    return;
  }
  c_copy(dest, cos_z);
  c_inv(dest);
}

void c_csc( dest, source )
     complex *dest;
     complex source;
{
  complex sin_z;

  c_sin(&sin_z, source);
  if (c_zeroQ(sin_z)) {
    c_infinity(dest);
    return;
  }
  c_copy(dest, sin_z);
  c_inv(dest);
}

/* hyperbolic functions */

void c_sinh( dest, source )
     complex *dest;
     complex source;
{
  complex z1, z2;

  c_exp(&z1, source);
  c_neg(&source);
  c_exp(&z2, source);
  c_sub(z1, z2, dest);
  c_scale(0.5, dest);
}

void c_cosh( dest, source )
     complex *dest;
     complex source;
{
  complex z1, z2;

  c_exp(&z1, source);
  c_neg(&source);
  c_exp(&z2, source);
  c_add(z1, z2, dest);
  c_scale(0.5, dest);
}

void c_tanh( dest, source )
     complex *dest;
     complex source;
{
  complex sinh_z, cosh_z;

  c_cosh(&cosh_z, source);
  if (c_zeroQ(cosh_z)) {
    c_infinity(dest);
    return;
  }

  c_sinh(&sinh_z, source);
  c_div(sinh_z, cosh_z, dest);
}

void c_coth( dest, source )
     complex *dest;
     complex source;
{
  complex sinh_z, cosh_z;

  c_sinh(&sinh_z, source);
  if (c_zeroQ(sinh_z)) {
    c_infinity(dest);
    return;
  }

  c_cosh(&cosh_z, source);
  c_div(cosh_z, sinh_z, dest);
}

void c_sech( dest, source )
     complex *dest;
     complex source;
{
  complex cosh_z;

  c_cosh(&cosh_z, source);
  if (c_zeroQ(cosh_z)) {
    c_infinity(dest);
    return;
  }
  c_copy(dest, cosh_z);
  c_inv(dest);
}

void c_csch( dest, source )
     complex *dest;
     complex source;
{
  complex sinh_z;

  c_sinh(&sinh_z, source);
  if (c_zeroQ(sinh_z)) {
    c_infinity(dest);
    return;
  }
  c_copy(dest, sinh_z);
  c_inv(dest);
}


/* complex power */
 
void c_complex_power(dest, source, power)
     complex *dest;
     complex source;
     complex power;
{
  complex ln;

  c_log(&ln, source);
  c_mul(ln, power, dest);
  c_exp(dest, *dest);
}

void c_sqrt( dest, source )
     complex *dest;
     complex source;
{
  complex power;

  power.x = 0.5;
  power.y = 0.0;
  c_complex_power(dest, source, power);
}
  
/* Inverse Trig Functions */

void c_arcsin( dest, source )
     complex *dest;
     complex source;
{
  complex z1, z2, z3, scale;

  /* calculate z1 = iz */
  c_assign(&z1, 0.0, 1.0);
  c_mul(z1, source, &z1);

  /* calculate z2 = sqrt(1 - z^2) */
  c_mul(source, source, &z2);
  z2.x = 1 - z2.x;
  z2.y = -z2.y;
  c_sqrt(&z2, z2);

  c_add(z1, z2, &z3);
  c_log(dest, z3);

  c_assign(&scale, 0.0, -1.0);
  c_mul(scale, *dest, dest);
}

void c_arccos( dest, source )
     complex *dest;
     complex source;
{
  complex z2, z3, scale;

  /* calculate z2 = sqrt(z^2 1 1) */
  c_mul(source, source, &z2);
  z2.x = z2.x - 1;
  c_sqrt(&z2, z2);

  c_add(source, z2, &z3);
  c_log(dest, z3);

  c_assign(&scale, 0.0, -1.0);
  c_mul(scale, *dest, dest);
}
  
void c_arctan( dest, source )
     complex *dest;
     complex source;
{
  complex iz, z1, z2, scale;

  iz.x = -source.y;
  iz.y = source.x;
  
  z1.x = 1 + iz.x;
  z1.y = iz.y;

  z2.x = 1 - iz.x;
  z2.y = -iz.y;

  c_div(z1, z2, dest);
  c_log(dest, *dest);
  c_assign(&scale, 0.0, 0.5);
  c_mul(scale, *dest, dest);
}

/* Inverse Hyperbolic Functions */

void c_arcsinh( dest, source )
     complex *dest;
     complex source;
{
  complex z2, z3, scale;

  /* calculate z2 = sqrt(1 + z^2) */
  c_mul(source, source, &z2);
  z2.x = 1 + z2.x;
  z2.y = z2.y;
  c_sqrt(&z2, z2);

  c_add(source, z2, &z3);
  c_log(dest, z3);
}

void c_arccosh( dest, source )
     complex *dest;
     complex source;
{
  complex z2, z3, scale;

  /* calculate z2 = sqrt(z^2 1 1) */
  c_mul(source, source, &z2);
  z2.x = z2.x - 1;
  c_sqrt(&z2, z2);

  c_add(source, z2, &z3);
  c_log(dest, z3);
}
  
void c_arctanh( dest, source )
     complex *dest;
     complex source;
{
  complex z1, z2;

  z1.x = 1 + source.x;
  z1.y = source.y;

  z2.x = 1 - source.x;
  z2.y = -source.y;

  c_div(z1, z2, dest);
  c_log(dest, *dest);
  c_scale(0.5, dest);
}

/* special functions */

/* Input and output functions */

void c_print( source )
     complex source;
{
  printf("(%f, %f)", source.x, source.y);
} 

