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
      program geodes
c This program takes two points as input and puts
c points along the circle thru them which is orthogonal
c to the unit circle. The points are the image of points
c on [0,1] by a l.f.t. The points on [0,1] have
c distance from 1 equal to .9 times the distance for the
c previous point.
      implicit double precision(a-h,o-y),complex*16(z)
      character*55 filenm
      open(1,file='twopts.pre',status='unknown')
      read(1,*)x,y
      z1=dcmplx(x,y)
      read(1,*)x,y
      z2=dcmplx(x,y)
      z1c=dconjg(z1)
      zc1=(z2-z1)/(1.d0-z1c*z2)
      dd=cdabs(zc1)
      zc2=zc1/dd
      open(2,file='ray.dat',status='unknown')
      np=300
      dxo=1.d0
      dd1=1.d0-dd
      do 2 j=1,np
         dx=(.9d0)**(j-1)
         if((dx.lt.dd1).and.(dxo.ge.dd1))then
             z=1.d0-dcmplx(dd1,0.d0)
             zw=(zc2*z+z1)/(1.d0+z1c*z*zc2)
             x=dreal(zw)
             y=dimag(zw)
             write(2,99)x,y
         endif
         z=1.d0-dcmplx(dx,0.d0)
         zw=(zc2*z+z1)/(1.d0+z1c*z*zc2)
         x=dreal(zw)
         y=dimag(zw)
         write(2,99)x,y
         dxo=dx
    2 continue
   99 format(2f22.15)
      stop
      end   
