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
c  this reads in the output of forward and adds blank lines
c  so that flows are graphed separately
      program blankl
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      open(1,file='input/fluid.inp2',status='unknown')
      read(1,*)nflowpt
      nnfpt=2*nflowpt
      read(1,*)nflows
      nflowl=nflows/2
      nflows=nflowl*2 + 1
      open(6,file='infty.pre',status='unknown')
      open(7,file='circflow',status='unknown')
      open(2,file='circflow.dsk',status='unknown')
      open(3,file='circflow.img',status='unknown')
      open(4,file='cflow.lines',status='unknown')
      read(6,*)x,y
      z1=dcmplx(x,y)
      read(6,*)x,y
      z2=dcmplx(x,y)
      zi1=(z2-z1)/(1.d0-dconjg(z1)*z2)
      zi2=zi1/cdabs(zi1)
      do 1 j=1,nflows
         kend=nnfpt-1
         if(j.eq.nflowl+1)kend=nflowpt
         do 2 k=1,kend
            read(7,*)x,y
            z=dcmplx(x,y)
            zw=zi2*(1.d0-dconjg(z1)*z)/(z-z1)
            x=dreal(zw)
            y=dimag(zw)
            write(2,*)x,y
            read(3,*)x,y
            write(4,*)x,y
    2    continue
         write(2,*)'  '
         write(4,*)'  '
         if(j.eq.nflowl+1)then
            do 3 k=1,kend
               read(7,*)x,y
               z=dcmplx(x,y)
               zw=zi2*(1.d0-dconjg(z1)*z)/(z-z1)
               x=dreal(zw)
               y=dimag(zw)
               write(2,*)x,y
               read(3,*)x,y
               write(4,*)x,y
    3       continue
         endif
         write(2,*)'  '
         write(4,*)'  '
    1 continue
      end
