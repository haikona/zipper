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
      subroutine imagegr(zpoly,k1,zintpt,m,zout)
c This program reads in the boundary data from poly.dat and computes 
c a grid of mxm  points for mapping by inverse,
c but deletes points outside the region.
      implicit double precision(a-h,o-y),complex*16(z)
      dimension z(100000),zpoly(k1),zout(100000)
Cf2py intent(in) zpoly,k1,m
Cf2py intent(out) zout
c
c      write(*,*)' Input should be in poly.dat'
c      open(1,file='poly.dat',status='unknown')
cc  The grid is (np+1) by (np+1) 
c      write(*,*)' input m. The grid will be mxm.'
c      read(*,*)m
c
      np=m-1
      xnp=np
      pi=3.14159265358979324d0
c      do 1 j=1,10000
c         read(1,*,end=2)x,y
c         z(j)=dcmplx(x,y)
c    1 continue
c    2 n=j-2
c      z0=z(n+1)
c
      do 1 j=1,k1
         z(j)=zpoly(j)
    1 continue
      n=k1
      z0=zintpt
c
      wind0=0.d0
      xmin=1.d+99
      xmax=-1.d+99
      ymin=1.d+99
      ymax=-1.d+99
      do 15 k=1,n-1
         dthet=dimag(cdlog((z(k+1)-z0)/(z(k)-z0)))
         wind0=wind0+dthet
         x=dreal(z(k))
         y=dimag(z(k))
         xmax=dmax1(x,xmax)
         ymax=dmax1(y,ymax)
         xmin=dmin1(x,xmin)
         ymin=dmin1(y,ymin)
   15 continue
      dthet=dimag(cdlog((z(1)-z0)/(z(n)-z0)))
      wind0=wind0+dthet
      x=dreal(z(n))
      y=dimag(z(n))
      xmax=dmax1(x,xmax)
      ymax=dmax1(y,ymax)
      xmin=dmin1(x,xmin)
      ymin=dmin1(y,ymin)
      if(wind0.gt.pi)then
         test=pi
         xl=dmax1(xmax-xmin,ymax-ymin)
         y0=ymin
         x0=xmin
         ym=ymax
         xm=xmax
      else
         test=-pi
         xd=dmax1(xmax-xmin,ymax-ymin)
         xl=2.d0*xd
         xc=(xmin+xmax)/2.d0
         yc=(ymin+ymax)/2.d0
         zc=dcmplx(xc,yc)
         x0=xc-xd
         y0=yc-xd
         xm=x0+xl
         ym=y0+xl
      endif
      dx=xl/xnp
      dy=dx
      y=y0-dy
c      write(*,*)' Output will be in grid.dat'
c      open(2,file='grid.dat',status='unknown')
c
      k2=1
c
      do 3 k=1,np+1
         y=y+dy
         if(y.gt.ym)goto 50
         x=x0-dx
         do 4 j=1,np+1
             x=x+dx
             if(x.gt.xm)goto 3
             z0=dcmplx(x,y)
             if(test.lt.0)then
                dd=cdabs(z0-zc)
                if(dd.gt.xd)goto 4
             endif
             wind=0.d0
             zmo=z(1)-z0
             if(cdabs(zmo).lt.1.d-13)goto 4
             do 5 m=1,n-1
                zmp=z(m+1)-z0
                if(cdabs(zmp).lt.1.d-13)goto 4
                dthet=dimag(cdlog(zmp/zmo))
                if(dabs(dabs(dthet)-pi).lt.1.d-12)goto 4
                wind=wind+dthet
                zmo=zmp
    5        continue
         zmp=z(1)-z0
         if(cdabs(zmp).lt.1.d-13)goto 4
         dthet=dimag(cdlog(zmp/zmo))
         if(dabs(dabs(dthet)-pi).lt.1.d-12)goto 4
         wind=wind+dthet
c         if(wind.gt.test)write(2,999)x,y
c
         if(wind.gt.test)then
            zout(k2)=dcmplx(x,y)
            k2=k2+1
         endif
c
    4    continue
    3 continue
c   50 stop
c  999 format(2f25.15)
   50 continue
      end

