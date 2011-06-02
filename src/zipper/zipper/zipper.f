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
c  slit map program with angles and bends.
c  The program will compute parameters and prevertices of the conformal map, 
c  and its inverse, which maps a Jordan region with the prescribed points on 
c  its boundary to the unit disk.
c
c  Program requires double precision. On some machines, compiling 
c    with integer*2 may increase speed (such as an old PC)
c     
c        input:  poly.dat  data file which contains:
c                         the boundary points in counterclockwise order,
c                         then the point in your region to be mapped to 0. 
c                         (the first point will be mapped to 1 on the 
c                         circle- for best accuracy, there should be a 
c                         tangent to the curve at the first 3 points),
c                         There should be an EVEN number of boundary points, 
c                         with bends occurring at odd numbered points.
c                         Format: each line consists of two numbers,
c                         the x and y coordinates of a point.
c
c                         It is possible to have infinity as an interior
c                         point of the region, but the orientation of
c                         the boundary must be positive: in this case
c                         the boundary points will be in clockwise order.
C                         This program does not test for correct
C                         input. Testing is done in polygon.f - use
C                         polygon if you are trying these programs for
C                         the first time and experiencing
C                         difficulties.
c
c        output:poly.pre   prevertices (inverse images of vertices)
c               poly.par   parameters (data used to evaluate the map)
c
c        written by Donald Marshall 
c                   Mathematics Department
c                   University of Washington  
c                   marshall@math.washington.edu
c
c              This version is limited to 120000 boundary points including the 
c              intermediate points.  To increase this limit, replace all
c              occurances of 120001 and 120000 with larger numbers.
      program zipper

      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,n
c  file containing data is called poly.dat
      open(1,file='./output/poly.dat',status='old')
c  file containing mapping parameters is called poly.par
      open(4,file='./output/poly.par',status='unknown')
c  file containing preimages of data on the unit circle is poly.pre
      open(3,file='./output/poly.pre',status='unknown')
      do 55 j=1,120000
          read(1,*,end=56)x,y
          z(j)=dcmplx(x,y)
   55 continue
      write(*,*)' more than 120000 points (Recompile)'
      stop
   56 n=j-2     
C
C the tests below have been moved to polygon.f
C remove last point if the curve is closed
C      zint=z(n+1)
C     if(cdabs(z(1)-z(n)).lt.1.d-14)then
C        n=n-1
C      endif
C      np=n+1
C      write(*,*)' number of data points=',n
C      ntmp=n/2
C      neven=ntmp*2
C      if(neven.ne.n)then
C          write(*,*)' You have an odd number of distinct boundary'
C          write(*,*)' points. The program requires an even number.'
C          write(*,*)' Also the bends should occur at odd numbered'
C          write(*,*)' vertices for best accuracy'
C          stop
C      endif
CC here we test that the interior point corresponds to the region
CC complementary to the curve (correct boundary orientation).
C      z(np)=z(1)
C      ncross=0
C      zrot1=(z(1)-z(2))/(z(2)-z(3))
C      zt=(zint-z(3))/(zint-z(1))*zrot1
C      atest=dimag(zt)
C      if(atest.ge.0.d0)then
CC   a point in the region bounded by the curve:
C         z0=dcmplx(-1.d0,1.d-3)
C      else
CC   a point not in the region bounded by the curve:
C         z0=dcmplx(-1.d0,-1.d-3)
C      endif
CC count signed crossings of [z0,f(zint)] where f is an lft.
C      zt=zt-z0
C      rota=dimag(cdlog(zt))
C      zrot2=cdexp(dcmplx(0.d0,-rota))
C      zold=-z0*zrot2
C      xf=dreal(zt*zrot2)
C      do 199 j=4,np
C        znew=(((z(j)-z(3))/(z(j)-z(1)))*zrot1 -z0)*zrot2
C        aimg=dimag(zold)
C        bimg=dimag(znew)
C        if(aimg*bimg.gt.0.d0)then
C          go to 198
C        endif
C        t=aimg/(aimg-bimg)
C        x=(1.d0-t)*dreal(zold)+t*dreal(znew)
C        if((x.gt.0.d0).and.(x.lt.xf))then
C          if(aimg.lt.0.d0)then
C             ncross=ncross+1
C          else
C             ncross=ncross-1
C          endif
C        endif
C  198   zold=znew
C  199 continue
C      if((atest.ge.0.d0).and.(ncross.eq.0))then
C        go to 983
C      endif
C      if((atest.lt.0.d0).and.(ncross.eq.-1))then
C        go to 983
C      endif
C      write(*,*)' atest=',atest,' ncross=',ncross
C      write(*,*)' curve is traced in wrong direction.'
C      write(*,*)' zipper will reverse the order.'
C      nt=n/2
C      do 980 j=1,nt
C         ztemp=z(j)
C         z(j)=z(n-j)
C         z(n-j)=ztemp
C  980 continue
C  983 continue
C      z(np)=zint
      call invers
      write(4,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
      do 981 j=4,n-2,2
  981 write(4,999)a(j),b(j),c(j)
      zm=dcmplx(0.d0,0.d0)
      ierr=0
      do 982 j=1,n
      if(cdabs(zm-z(j)).lt.1.d-16)then
         ierr=1
         jm=j-1
         write(*,*)' WARNING: prevertices',j,' and',jm,' are equal'
      endif
      zm=z(j)
      x=dreal(z(j))
      y=dimag(z(j))
  982 write(3,999)x,y
      if(ierr.eq.1)then
            write(*,*)' This occurred because two points are too close'
            write(*,*)' on the boundary (poly.dat). Their preimages on '
            write(*,*)' the unit circle differ by less than 10**(-16)'
            write(*,*)'    '
            write(*,*)'    '
            write(*,*)' Stop the demo and restart. You can modify'
            write(*,*)' your previous curve by: xcm -e init.dat'
            write(*,*)' Select Lines & Boxed points under Edit Options.'
            write(*,*)' Use Kill Point and Move Point and Write Output'
            write(*,*)' Then restart demo, but quit at the first step'
            write(*,*)' without doing Write Output. If the new points'
            write(*,*)' are sufficiently separated, the demo will work.'
      endif
  999 format(3f25.15)
      stop
      end
c
c
c      computes map from region to disk and finds inverse image of vertices.
c        z(n+1) mapped to 0, z(1) mapped to 1
      subroutine invers
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,n
      pi=3.14159265358979324d0
      acc=1.d-14
c  acc should be at least machine eps **.75
      accr=1.d+14
      zi=dcmplx(0.d0,1.d0)
c-----Map complement of arc of circle through z(1),z(2),z(3) to the 
c-----upper half plane-------------------------------------------------
c-----------------------------------------------------------------------
      z1=z(1)
      z2=z(2)
      z3=z(3)
      zrot1=(z(1)-z(2))/(z(2)-z(3))
      do 1 j=4,n+1
         z(j)=zi*cdsqrt(zrot1*(z(j)-z(3))/(z(j)-z(1)))
    1 continue
      z(1)=dcmplx(-1.d+20,0.d0)
      z(2)=dcmplx(-1.d0,0.d0)
      z(3)=dcmplx(0.d0,0.d0)
c    abs(w) gt r1*length of slit will be outer region 
      r1=1.125d0 
c    radius of region around tip=r2*ytip 
      r2=.25d0 
c-------------pull back curve----------------------------------
      do 2 j=4,n-2,2
         z1rc=dconjg(1.d0/z(j))
         z2r=1.d0/z(j+1)
         b1=-dimag(z1rc*z2r)/dimag(z1rc+z2r)
         zt=z2r+b1
         pia=-datan2(dimag(zt),dreal(zt))
         a(j)=pia/pi
         aa=a(j)
         oma=1.d0-aa 
         omar=1.d0/oma 
         azt=cdabs(zt)
         xl=(aa**aa)*(oma**oma) 
         b(j)=1.d0/(xl*azt)
         c(j)=b1*b(j)
         ar=1.d0/aa 
         zangc=cdexp(dcmplx(0.d0,-pia)) 
         zrota=zangc*zangc
         x=xl*dreal(zangc) 
         y=-xl*dimag(zangc) 
         ztip=dcmplx(x,y) 
         rr1=1.d0/(r1*xl) 
         rr2=r2*y 
         a0=aa-oma 
         c2=-2.d0*a0/3.d0
         a1=aa*oma/2.d0
         c1=4.d0*a1
         a2=c2*a1 
         zc1=dcmplx(0.d0,dsqrt(c1)) 
         do 3 k=j+2,n+1
            zwr=c(j)+b(j)/z(k)
            xlzw=cdabs(zwr)
            if(xlzw.lt.rr1)then
               zt=1.d0+(a0+(a1+a2*zwr)*zwr)*zwr
               call newton(zwr,aa,zt,zrota)
               go to 8
            endif
            zw=1.d0/zwr
            xzw=dreal(zw)
            yzw=dimag(zw)
            if(cdabs(zw-ztip).lt.rr2)then
               zwi=cdsqrt(zw/ztip-1.d0)
c  to be sure of branch:
              if((dreal(zwi).lt.1.d-4).and.(dimag(zwi).lt.0.d0))zwi=-zwi
c               zt=zwi*zc1
               zt=(zc1+c2*zwi)*zwi
c               zt=(zc1+c2*(1.d0+zc3*zwi)*zwi)*zwi
               call newt1(zwi,aa,c1,zt,zrota)
            elseif(xlzw.gt.accr)then
               zt=-oma
            elseif(datan2(yzw,xzw).ge.pia)then
               zwt=(zangc*zw)**omar
               zt=-oma+zwt
               test=cdabs(zwt)
               test=test*test
               if(test.lt.acc)go to 8
               zt=zt*zwr
               call newton(zwr,aa,zt,zrota)
            else
               zwt=zw**ar
               zt=aa+zwt
c               zt=(aa+(1.d0+a3*zwt)*zwt) 
c               zt=(aa+(1.d0+a3*(1.d0+a4*zwt)*zwt)*zwt)
               test=cdabs(zwt)
               test=test*test
               if(test.lt.acc)go to 8 
               zt=zt*zwr
               call newton(zwr,aa,zt,zrota)
            endif
    8       z(k)=zt
    3    continue
         z(j+1)=dcmplx(0.d0,0.d0)
         z(j-1)=dcmplx(aa-1.d0,0.d0)
         zwr=c(j)+b(j)/z(j)
         zw=1.d0/zwr
         if(cdabs(zw-ztip).lt.rr2)then
            zwi=dcmplx(0.d0,dsqrt(dreal(1.d0-zw/ztip)))
c            zt=zwi*zc1
            zt=(zc1+c2*zwi)*zwi
c            zt=(zc1+c2*(1.d0+zc3*zwi)*zwi)*zwi
            call newt1(zwi,aa,c1,zt,zrota)
         elseif(cdabs(zwr).gt.accr)then
            zt=-oma
         else
            zwt=(zangc*zw)**omar
            zt=-oma+zwt
            test=cdabs(zwt)
            test=test*test
            if(test.lt.acc)go to 28
            zt=zt*zwr
            call newton(zwr,aa,zt,zrota)
         endif
   28    z(j)=zt
         do 4 k=1,j-2
            xw=dreal(z(k))
            xwr=c(j)+b(j)/xw
            xlzw=dabs(xwr)
            if(xlzw.lt.rr1)then
               xt=1.d0+(a0+(a1+a2*xwr)*xwr)*xwr
               call rnewt(xwr,aa,xt)
               go to 18
            endif
            xw=1.d0/xwr
            if(xlzw**omar.gt.accr)then
               xt=-oma
            elseif(xw.le.0.d0)then
               xwt=-(-xw)**omar
               xt=-oma+xwt
               test=xt*xt
               if(test.lt.acc)go to 18
               xt=xt*xwr
               call rnewt(xwr,aa,xt)
            else
               xwt=xw**ar
               xt=aa+xwt
c               xt=(aa+(1.d0+a3*xwt)*xwt) 
c               xt=(aa+(1.d0+a3*(1.d0+a4*xwt)*xwt)*xwt)
               test=xwt*xwt
               if(test.lt.acc)go to 18 
               xt=xt*xwr
               call rnewt(xwr,aa,xt)
            endif
   18       z(k)=dcmplx(xt,0.d0)
    4    continue
    2 continue
      z(n-1)=dcmplx(0.d0,0.d0)
c------------- map circular sector to disk---------------------------
      zrot2=1.d0/z(n)-1.d0/z(1)
      zto1=z(1)
      angler=pi/datan2(dimag(-zrot2),dreal(-zrot2))
      zto0=(z(n+1)*zrot2/(1.d0-z(n+1)/z(1)))**angler
      zto0c=dconjg(zto0)
      do 5 j=2,n-2
         zt=(z(j)*zrot2/(1.d0-z(j)/z(1)))**angler
         z(j)=(zt-zto0)/(zt-zto0c)
    5 continue
      z(n-1)=zto0/zto0c
      zt=(z(n)*zrot2/(1.d0-z(n)/z(1)))**angler
      z(n)=(zt-zto0)/(zt-zto0c)
      z(1)=dcmplx(1.d0,0.d0)
      z(n+1)=dcmplx(0.d0,0.d0)
      return
      end
c
c
cc    version of newton's method (upper-half plane)
cc    iterating on x/w  (xwr=1/w) -- REAL arithmetic
c     x=output and initial guess
      subroutine rnewt(xwr,aa,x)  
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      acc=1.d-14
c  acc should be at least machine eps **.75
      xb1=-aa*xwr
      xb2=xb1+xwr
      do 10 i=1,20
         xma=x+xb1
         xmap=x+xb2
         xfract=(xmap/xma)**aa
         xd=(xfract-xmap)*xma/x
         x=x+xd
         test=dabs(xd)
         if(test.lt.acc)goto 20
   10 continue
      write(*,*)' more than 20 iterations required in rnewt'
      write(*,*)' xwr=',xwr,' a=',aa,' x=',x,' test=',test
   20 x=x/xwr
      return
      end
c
c



c    version of newton's method (upper-half plane)
c    iterating on z/w
      subroutine newton(zwr,aa,z,zrota)
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      acc=1.d-14
c  acc should be at least machine eps **.75
      zb1=-aa*zwr
      zb2=zb1+zwr
      do 10 i=1,20
         zma=z+zb1
         zmap=z+zb2
         zr=zmap/zma
         zfr=zr**aa
         if(dimag(zfr).gt.0.d0)then
            if(dreal(zr).lt.0.d0)zfr=zfr*zrota
         endif
         zd=(zfr-zmap)*zma/z
         z=z+zd
         test=cdabs(zd)
         if(test.lt.acc)goto 20
   10 continue
      write(*,*)' more than 20 iterations required in newton'
      zw=1.d0/zwr
      write(*,*)' zw=',zw
      write(*,*)' z =',z
      write(*,*)' test=',test
      write(*,*)' a=',aa
   20 z=z/zwr
      return
      end






c    version of newton's method (upper-half plane) near tip
c    iterating on z
      subroutine newt1(zwi,aa,c1,z,zrota)
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      acc=1.d-14
c  acc should be at least machine eps **.75
      oma=1.d0-aa
      do 10 i=1,20
         z1=1.d0-z/aa
         z2=1.d0+z/oma
         zr=z1/z2
         zfr=zr**aa
c  Analytic continuation across slit:
         if(dimag(zfr).gt.0.d0)then
            if(dreal(zr).lt.0.d0)zfr=zfr*zrota
            z3=cdsqrt(zfr*z2-1.d0)
            if(dreal(zr).gt.0.d0)z3=-z3
         else
            z3=cdsqrt(zfr*z2-1.d0)
         endif
         zfcn=z3-zwi
         zq=z3/z
         zd=zq*c1*z1*zfcn/zfr
         z=z+zd
         test=cdabs(zd)
         if(test.lt.acc)goto 20
   10 continue
      write(*,*)' more than 20 iterations required in newt1'
      write(*,*)' zwi=',zwi,' z=',z,' test=',test,' a=',aa
   20 return
      end


