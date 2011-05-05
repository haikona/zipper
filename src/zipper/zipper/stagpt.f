C
C    Copyright (C) 1996  University of Washington, U.S.A.
C
C    Author:   Donald E. Marshall
C
C   This program is free software; you can redistribute it and/or modify
C   it under the terms of the GNU General Public License as published by
C   the Free Software Foundation; either version 1, or (at your option)
C   any later version.

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
      program stagpt
      implicit double precision(a-h,o-y),complex*16(z)
      open(1,file='./output/stagpt.dat',status='unknown')
      read(1,*)x,y
      zstag=dcmplx(x,y)
      open(3,file='./output/poly.dat',status='unknown')
      diff=99.d0
      do 1 k=1,10000
         read(3,*,end=3)x,y
         z=dcmplx(x,y)
         diff1=cdabs(zstag-z)
         if(diff1.lt.diff)then
            diff=diff1
            index=k
         endif
    1 continue
    3 continue
      open(4,file='./output/poly.pre',status='unknown')
      do 2 k=1,index
         read(4,*)x,y
    2 continue
      open(2,file='./output/stagpt.pre',status='unknown')
      write(2,*)x,y
      stop
      end
