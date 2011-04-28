C
C    Copyright (C) 1996  University of Washington, U.S.A.
C
C    Author:   Donald E. Marshall
C
C   This program is free software; you can redistribute it and/or modify
C   it under the terms of the GNU General Public License as published by
C   the Free Software Foundation; either version 1, or (at your option)
C   any later version.
C
C   This program is distributed in the hope that it will be useful,
C   but WITHOUT ANY WARRANTY; without even the implied warranty of
C   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
C   GNU General Public License for more details.
C
C   You should have received a copy of the GNU General Public License
C   along with this program; if not, write to the Free Software
C   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
C
C
C
C
c This program takes two columns of numbers. The output is
c written in blocks of 5 as follows: each group  of
c four from the input is written, followed by another copy of
c the first from the group, then a blank line is written.
c For use with xcm to draw images of 
c whitney boxes by conformal maps.
c
c output is whitney.sqo
c
      program block45
      implicit double precision(a-h,o-y),complex*16(z)
      character*55 filenm
      dimension x(30000),y(30000)
      write(*,*)' Name of file to be put in blocks of 5?'
      read(*,80)filenm
   80 format(a55)
      open(1,file=filenm,status='unknown')
      write(*,*)' Name of output file?'
      read(*,80)filenm
      open(2,file=filenm,status='unknown')
      do 1 j=1,30000
         read(1,*,end=2)x(j),y(j)
    1 continue
      write(*,*)' Exceeded 30000 pts, will use first 30000.'
    2 n=j-1
      n4=n/4
      do 4 j=0,n4-1
         do 3 i=1,4
            write(2,99)x(4*j+i),y(4*j+i)
    3    continue
         write(2,99)x(4*j+1),y(4*j+1)
         write(2,*)'   '   
    4 continue
   99 format(2f22.15)
      stop 
      end
