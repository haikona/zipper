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
       program circle
       implicit double precision(a-h,o-y),complex*16(z)
       character*55 filenm
       write(*,*)' number of points on the circle?'
       read(*,*)num
   80  format(a55)
       open(1,file='fftpts.dat',status='unknown')
       thet=0.
       pi=3.14159265358979324d0
       tpi=2.*pi
       dthet=tpi/dfloat(num)
       do 1 j=1,num+1
         thet=thet+dthet
         x=dcos(thet)
         y=dsin(thet)
         write(1,*)x,y
    1  continue
       stop
       end
