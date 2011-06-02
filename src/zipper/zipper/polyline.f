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
      program polyline
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
c  As written, this program is limited to 10000 vertices
c This program is intended for use with inverse. Input is a sequence 
c of points in a region.  This program places additional points along
c the polygonal line through the given points.
      dimension z(10002),zp(10000),xl(10002)
      write(*,*)'   '
      write(*,*)' Boundary of region should be in poly.dat'
      open(3,file='poly.dat',status='unknown')
      pi=3.14159265358979324d0
      test=pi
      do 11 j=1,10000
         read(3,*,end=12)x,y
         z(j)=dcmplx(x,y)
   11 continue
   12 n=j-2
      wind0=0.d0
      z0=z(n+1)
      zd0=z(1)-z0
      if(cdabs(zd0).lt.1.d-13)then
         n=n-1
         z0=z(n+1)
         zd0=z(1)-z0
      endif
      do 25 k=2,n
         zd1=z(k)-z0
         if(cdabs(zd1).lt.1.d-13)goto 25
         dthet=dimag(cdlog(zd1/zd0))
         zd0=zd1
         wind0=wind0+dthet
   25 continue
      if(wind0.lt.pi)test=-pi
      write(*,*)' File with data points should be called path.pts'
      open(1,file='path.pts',status='old')
      do 1 j=1,10001
         read(1,*,end=2)x,y
         zp(j)=dcmplx(x,y)
    1 continue
      write(*,*)' exceeded 10000 vertices'
      stop
    2 nn=j-1
      write(*,*)' The program will put at least 5 points per edge'
      write(*,*)' (counting endpoints) and only use points inside'
      write(*,*)' region, if part of polygon leaves region'
      write(*,*)'  '
      write(*,*)' Approx. lower bound on total number of points?'
      write(*,*)'    '
      read(*,*)mapprox
      perim=0.d0
      do 3 j=1,nn-1
         xl(j)=cdabs(zp(j+1)-zp(j))
         perim=perim+xl(j)
    3 continue
      njsum=0
      write(*,*)' Output will be in path.dat'
      open(2,file='path.dat',status='unknown')
      do 4 j=1,nn-1
         nj=mapprox*xl(j)/perim
         if(nj.lt.4)nj=4
         zd=(zp(j+1)-zp(j))/float(nj)
         if(j.eq.nn-1)nj=nj+1
         njsum=njsum+nj
         zz=zp(j)
         x=dreal(zz)
         y=dimag(zz)
         do 5 k=1,nj
            wind=0.d0
            zmo=z(1)-zz
            if(cdabs(zmo).lt.1.d-13)goto 5
            do 15 m=1,n-1
               zmp=z(m+1)-zz
               if(cdabs(zmp).lt.1.d-13)goto 5
               dthet=dimag(cdlog(zmp/zmo))
               if(dabs(dabs(dthet)-pi).lt.1.d-12)goto 5
               wind=wind+dthet
               zmo=zmp
   15       continue
            zmp=z(1)-zz
            if(cdabs(zmp).lt.1.d-13)goto 5
            dthet=dimag(cdlog(zmp/zmo))
            if(dabs(dabs(dthet)-pi).lt.1.d-12)goto 5
            wind=wind+dthet
            if(wind.gt.test)write(2,999)x,y
            zz=zz+zd
            x=dreal(zz)
            y=dimag(zz)
    5    continue
    4 continue
      write(*,*)' Total number of data points =',njsum
  999 format(2f25.15)
      stop 
      end
