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
c  This computes the Carleson grid with refined squares in the top half
c   of each box. Points are not in correct order for plotting.
      subroutine cgrids(ngen,nsl,zout)
      implicit double precision(a-h,o-y),complex*16(z)
      dimension zout(100000)
      dimension x(100000),y(100000)
Cf2py intent(in) ngen,nsl
Cf2py intent(out) zout
c
c      character*55 filenm
c      write(*,*)' number of generations?'
c      read(*,*)ngen
c      write(*,*)' number of sublevels?'
c      read(*,*)nsl
      nnsl=nsl
c      if(nsl.eq.0)then
c          write(*,*)'  Program requires sublevels geq 1'
c          stop
c      endif
c      write(*,*)' name of file for data?'
c      read(*,80)filenm
c   80 format(a55)
c      open(2,file=filenm,status='unknown')
      pi=3.1415926535898d0
      rad=1.d0-pi/4.d0
      i=0
      ngen1=ngen-1
      do 1 j=1,ngen1
         rad=(1.d0+rad)/2.d0
         lend=16*(2**j)*nsl
         kend=nsl
         if(nnsl.eq.0)then
            nsl=1
            lend=8*(2**j)
            kend=1
         endif
         drad=(1.d0-rad)/(2.d0*nsl)
         dtheta=2.d0*pi/lend
         rad1=rad-drad
         if((j.eq.ngen1).and.(nnsl.ne.0))kend=2*nsl
         do 2 k=1,kend
            rad1=rad1+drad
            do 3 l=1,lend
               i=i+1
               x(i)=rad1*dcos(dtheta*l)
               y(i)=rad1*dsin(dtheta*l)
    3       continue
    2    continue
    1 continue
      rad1=.999999999d0
      do 4 l=1,lend
         i=i+1
         x(i)=rad1*dcos(dtheta*l)
         y(i)=rad1*dsin(dtheta*l)
    4 continue
      nnn=i
c
      do 5 j=1,nnn
         zout(j)=dcmplx(x(j),y(j))
    5 continue
c
c      write(*,*)' number of points =',nnn
c      write(2,99)(x(i),y(i),i=1,nnn)
c   99 format(2f25.15)
c      stop
      end
       


