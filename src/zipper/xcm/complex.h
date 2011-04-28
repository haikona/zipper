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
/***************************/
/* Complex Variable Header */
/***************************/

/****************************************************************************/
/* AUTHOR:         Michael Stark                                            */
/****************************************************************************/

#ifndef _COMPLEX_
#define _COMPLEX_ 

#define C_ERROR -10
#define LARGE_VALUE 1E20

typedef struct {
  double x;
  double y;
} complex;

void c_assign();
void c_infinity();
void c_copy();
void c_add();
void c_sub();
void c_neg();
void c_mul();
char c_div();
double c_abs();
double c_abs_sqr();
double c_arg();
double c_positive_arg();
void c_scale();
void c_conj();
void c_copy_conj();
char c_zeroQ();
char c_polar();
void c_rect();
void c_print();
void c_zero();
void c_one();
void c_power();
void c_irreducible();
void c_exp();
void c_inv();
void c_log();
void c_sin();
void c_cos();
void c_tan();
void c_cot();
void c_sec();
void c_csc();
void c_sinh();
void c_cosh();
void c_tanh();
void c_coth();
void c_sech();
void c_csch();
void c_arcsin();
void c_arccos();
void c_arctan();
void c_arcsinh();
void c_arccosh();
void c_arctanh();
void c_sqrt();
void c_complex_power();

#endif
