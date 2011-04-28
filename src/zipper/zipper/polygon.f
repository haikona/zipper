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
c This program makes sure that the data is in the correct form
c for zipper. 
c  The curve needs to be a Jordan curve. 
c  If you have a smooth curve,
c  or a curve which is piecewise smooth (relatively few bends),
c  it might be wise to create your own data file satisfying the 
c  requirements of zipper and avoid using this program, resulting in 
c  far fewer data points and hence greater speed with zipper.
c
c 
c 
      program polygon
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
c  As written, this program is limited to 10000 vertices
      dimension z(10002),zn(10002),xl(10002),zz(10002),jindex(10002)
      character*55 filenm
      write(*,*)' name of file with vertices then interior point?  '
      read(*,80)filenm
   80 format(a55)
   99 format(2g25.15)
   98 format(i5)
      open(1,file=filenm,status='old')
      do 1 j=1,10001
         read(1,*,end=2)x,y
         z(j)=dcmplx(x,y)
    1 continue
      write(*,*)' exceeded 10000 vertices'
      stop
    2 n=j-2
C remove last point if the curve is closed
      zint=z(n+1)
      if(cdabs(z(1)-z(n)).lt.1.d-14)then
         n=n-1
      endif
      norg=n
      np=n+1
      write(*,*)' The program will put at least 5 points per edge'
      write(*,*)' (counting endpoints) with vertices at odd points.' 
      write(*,*)'  '
      write(*,*)' Approx. lower bound on number of points on boundary?'
      write(*,*)'    '
      read(*,*)mapprox
      z(np)=z(1)
      zm=(z(2)+z(1))/2.d0
      distm=cdabs(zm-zint)
      jclose=1
      do 6 j=2,n
         zc=(z(j+1)+z(j))/2.d0
         dd=cdabs(zc-zint)
         if(dd.lt.distm)then
            distm=dd
            jclose=j
            zm=zc
         endif
    6 continue
      zn(1)=zm
      do 7 j=2,n-jclose+1
         zn(j)=z(jclose+j-1)
    7 continue
      do 8 j=n-jclose+2,n+1
         zn(j)=z(j-n+jclose-1)
    8 continue
      npp=n+2
      zn(npp)=zm
      perim=0.d0
      do 3 j=1,np
         xl(j)=cdabs(zn(j+1)-zn(j))
         perim=perim+xl(j)
    3 continue
      write(*,*)' name of file for points?  '
      read(*,80)filenm
      open(2,file=filenm,status='unknown')
      njs=0
      do 4 j=1,np
         nj=mapprox*xl(j)/perim
         if(nj.lt.4)nj=4
         itest=nj/2
         itest=2*itest
         if(itest.lt.nj)nj=nj+1
         zd=(zn(j+1)-zn(j))/float(nj)
         if(cdabs(zd).lt.1.d-16)goto 4
         njs=njs+1
         if(j.le.n-jclose+1)then
            m=jclose-1+j
         else
            m=jclose-1-n+j
         endif
         jindex(m)=njs
         zz(njs)=zn(j)
         do 5 k=1,nj-1
            njs=njs+1
            zz(njs)=zz(njs-1)+zd
    5    continue
    4 continue
      write(*,*)' number of boundary points =',njs
      n=njs
      np=n+1
C here we test that the interior point corresponds to the region
C complementary to the curve (correct boundary orientation).
      zz(np)=zz(1)
      ncross=0
      zrot1=(zz(1)-zz(2))/(zz(2)-zz(3))
      zt=(zint-zz(3))/(zint-zz(1))*zrot1
      atest=dimag(zt)
      if(atest.ge.0.d0)then
C   a point in the region bounded by the curve:
         z0=dcmplx(-1.d0,1.d-3)
      else
C   a point not in the region bounded by the curve:
         z0=dcmplx(-1.d0,-1.d-3)
      endif
C count signed crossings of [z0,f(zint)] where f is an lft.
      zt=zt-z0
      rota=dimag(cdlog(zt))
      zrot2=cdexp(dcmplx(0.d0,-rota))
      zold=-z0*zrot2
      xf=dreal(zt*zrot2)
      do 199 j=4,np
        znew=(((zz(j)-zz(3))/(zz(j)-zz(1)))*zrot1 -z0)*zrot2
        aimg=dimag(zold)
        bimg=dimag(znew)
        if(aimg*bimg.gt.0.d0)then
          go to 198
        endif
        t=aimg/(aimg-bimg)
        x=(1.d0-t)*dreal(zold)+t*dreal(znew)
        if((x.gt.0.d0).and.(x.lt.xf))then
          if(aimg.lt.0.d0)then
             ncross=ncross+1
          else
             ncross=ncross-1
          endif
        endif
  198   zold=znew
  199 continue
      if((atest.ge.0.d0).and.(ncross.eq.0))then
        go to 983
      endif
      if((atest.lt.0.d0).and.(ncross.eq.-1))then
        go to 983
      endif
      write(*,*)' atest=',atest,' ncross=',ncross
      write(*,*)' curve is traced in wrong direction.'
      write(*,*)' Program will reverse the order.'
      nt=n/2
      do 980 j=1,nt
         ztemp=zz(j)
         zz(j)=zz(n-j)
         zz(n-j)=ztemp
  980 continue
      do 981 jj=1,norg
         jindex(jj)=n-jindex(jj)
  981 continue
  983 continue
      zz(np)=zint
      do 9 j=1,njs
          x=dreal(zz(j))
          y=dimag(zz(j))
          write(2,99)x,y
    9 continue
      write(2,*)'  '
      x=dreal(zint)
      y=dimag(zint)
      write(2,99)x,y
      write(*,*)' index.dat=file with index for original data pts.'
      open(3,file='index.dat',status='unknown')
      do 19 j=1,norg
         write(3,98)jindex(j)
   19 continue
      stop
      end
