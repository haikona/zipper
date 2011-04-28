C
C    Copyright (C) 1993  University of Washington, U.S.A.
C
C    Author:   Donald E. Marshall
C
C    This program is free software; you can redistribute it and/or modify
C   it under the terms of the GNU General Public License as published by
C    the Free Software Foundation; either version 1, or (at your option)
C    any later version.
C
C    This program is distributed in the hope that it will be useful,
C    but WITHOUT ANY WARRANTY; without even the implied warranty of
C    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
C    GNU General Public License for more details.
C
C    You should have received a copy of the GNU General Public License
C    along with this program; if not, write to the Free Software
C    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
C
C
C
C
c  slit map program with angles and bends.
c  The program will evaluate the inverse map from the Jordan region to the 
c  disk. Warning: currently it only works on points in the *interior*
c   of the region bounded by the (computed) image of the unit circle. 
c   so it won't work for boundary points, and may fail for some points
c   very near the boundary, inside your region, but outside the 
c   computed boundary.
c
c  Program requires double precision. On some machines, compiling
c    with integer*2 may increase speed (such as an old PC)
c
c        written by Donald Marshall 
c                   Mathematics Department
c                   University of Washington  
c                   marshall@math.washington.edu
c
c              This version is limited to 10000 boundary points including the 
c              intermediate points.  To increase this limit, replace all
c              occurances of 10001 with a larger number.
      program inverse
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      character*55 filenm
      common z(10001),a(10001),b(10001),c(10001),z1,z2,z3,zrot1,zto0,
     1zto1,angler,zrot2,mm,n
   80 format(a55)
c file with mapping parameters is poly.par
      open(4,file='./output/poly.par',status='unknown')
      read(4,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
      do 981 j=1,10000
      jj=j*2+2
  981 read(4,999,end=57)a(jj),b(jj),c(jj)
   57 n=jj
      write(*,*)' number of vertices =',n
  50  write(*,*)' name of file with points in the open region?  '
      read(*,80)filenm
      open(2,file=filenm,status='old')
      write(*,*)' name of file for image of these points?  '
      read(*,80)filenm
      open(3,file=filenm,status='unknown')
      do 65 j=1,10000
          read(2,*,end=66)x,y
          z(j)=dcmplx(x,y)
   65 continue
      write(*,*)' Exceeded 10000 pts. Split file or recompile'
      write(*,*)' Will compute image of first 10000 points'
   66 mm=j-1
      write(*,*)' number of data points=',mm
      call invert
      do 984 j=1,mm
         x=dreal(z(j))
         y=dimag(z(j))
c points outside the region will be mapped to points outside the disk.
c But the map is not quite 1-1: the last step of mapping part of
c a circle to the disk is not 1-1 on the outside.
c thus we will just delete these points.
         if(x*x+y*y.gt.(1.d0+1.d-8))then
            write(*,*)'  '
            write(*,*)' z(j) outside region, so pullback outside disk,'
            write(*,*)' and will be eliminated from output.'
            write(*,*)' j=',j,'   inverse of z(j)=', z(j)
            goto 984
         endif
         write(3,999)x,y
  984 continue
  999 format(3f25.15)
      stop
      end
c
c
c      computes map from region to disk 
c
      subroutine invert
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(10001),a(10001),b(10001),c(10001),z1,z2,z3,zrot1,zto0,
     1zto1,angler,zrot2,mm,n
      pi=3.14159265358979324d0
      acc=1.d-12
      zi=dcmplx(0.d0,1.d0)
c   abs(w) gt r1*length of slit will be outer region
      r1=1.125d0
c   radius of region around tip=r2*ytip
      r2=.25d0
c-----Map complement of arc of circle through z1,z2,z3 to the upper half
c-----plane-------------------------------------------------
      do 1 j=1,mm
         zd=z(j)-z1
         if(cdabs(zd).lt.1.d-20)then
            z(j)=dcmplx(1.d+20,0.d0)
            go to 1
         endif
         z(j)=zi*cdsqrt(zrot1*(z(j)-z3)/(z(j)-z1))
    1 continue
c-------------pull back points----------------------------------
      do 2 j=4,n-2,2
         aa=a(j)
         oma=1.d0-aa 
         omar=1.d0/oma 
         xl=(aa**aa)*(oma**oma) 
         pia=pi*aa 
         ar=1.d0/aa 
         zangc=cdexp(dcmplx(0.d0,-pia)) 
         zrota=zangc*zangc
         x=xl*dreal(zangc) 
         y=-xl*dimag(zangc) 
         ztip=dcmplx(x,y) 
         rr1=r1*xl 
         rr2=r2*y 
         a0=aa-oma 
         a1=aa*oma/2.d0 
         c2=-2.d0*a0/3.d0
         a2=c2*a1
         c1=4.d0*a1 
         zc1=dcmplx(0.d0,dsqrt(c1)) 
         do 3 k=1,mm
            if(cdabs(z(k)).lt.1.d-4)then
               zt=-oma
               go to 8
            endif 
            zwr=c(j)+b(j)/z(k)
            xlzw=1.d0/cdabs(zwr)
            if(xlzw.gt.rr1)then
               zt=1.d0+(a0+(a1+a2*zwr)*zwr)*zwr
               call newton(zwr,aa,zt,zrota)
               go to 8
            endif
            zw=1.d0/zwr
            if(cdabs(zw-ztip).lt.rr2)then
               zwi=cdsqrt(zw/ztip-1.d0)
c     to be sure of branch...:
               if((dreal(zwi).lt.1.d-4).and.(dimag(zwi).lt.0.d0))zwi=-zwi
c               zt=zwi*zc1
               zt=(zc1+c2*zwi)*zwi
c               zt=(zc1+c2*(1.d0+zc3*zwi)*zwi)*zwi
               test=cdabs(zwi)
               test=test*test*test
               if(test.lt.acc)then
                  if(dreal(zt).gt.0.d0)zt=-zt
                  if(dimag(zt).lt.0.d0)zt=dconjg(zt)
                  go to 8
               endif
               call newt1(zwi,aa,c1,zt,zrota)
               go to 8
            endif
            zwrot=zangc*zw
c the next test allows analytic continuation in case you evaluate the 
c   inverse at points outside the computed region.  If the wrong 
c   branch is chosen, you might alter this test.
            if(dimag(zwrot).gt.-1.d-3)then
               zwt=zwrot**omar
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
    2 continue
c------------- map circular sector to disk---------------------------
      zto0c=dconjg(zto0)
      do 5 j=1,mm
         zt=(z(j)*zrot2/(1.d0-z(j)/zto1))**angler
         z(j)=(zt-zto0)/(zt-zto0c)
    5 continue
      return
      end




c   newton's method (upper-half plane)
c    iterating on z/w
      subroutine newton(zwr,aa,z,zrota)
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      acc=1.d-12
c   acc depends upon machine epsilon... you may have to increase it.
c    it should be at least eps**.75
      zb1=-aa*zwr
      zb2=zb1+zwr
      do 10 i=1,20
c         if(i.gt.5)write(*,*)' newton iteration',i
         zma=z+zb1
         zmap=z+zb2
         zr=zmap/zma
         zfr=zr**aa
c  analytic continuation across the slit (for w near slit):
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
      zz=z/zwr
      write(*,*)' z =',zz
      write(*,*)' test=',test
      write(*,*)'  a=',aa
   20 z=z/zwr
      return
      end






c   newton's method (upper-half plane)
c    iterating on z.  Used near tip of the slit.
      subroutine newt1(zwi,aa,c1,z,zrota)
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      acc=1.d-12
c acc depends upon machine epsilon...you may have to increase it.
c    it should be at least eps**.75
      oma=1.d0-aa
      do 10 i=1,20
c         if(i.gt.5)write(*,*)' newt1 iteration',i
         z1=1.d0-z/aa
         z2=1.d0+z/oma
         zr=z1/z2
         zfr=zr**aa
c Analytic continuation across slit:
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

