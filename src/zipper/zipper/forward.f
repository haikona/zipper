C
C    Copyright (C) 1996  University of Washington, U.S.A.
C
C    Author:   Donald E. Marshall
C
C   This program is free software; you can redistribute it and/Or modify
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
c  The program will compute values of the conformal map (at prevertices
c  and at intermediate points or at interior points) which maps the disk
c  to a Jordan region with the prescribed points on its boundary. 
c
c  Program requires double precision. On some machines, compiling
c    with integer*2 may increase speed (such as an old PC)
c
c        written by Donald Marshall 
c                   Mathematics Department
c                   University of Washington  
c                   marshall@math.washington.edu
c
c              This version is limited to 120000 boundary points including the 
c              intermediate points.  To increase this limit, replace all
c              occurances of 120001 and 120000 with a larger number.
      program forward
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      character*55 filenm
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,m,n
   80 format(a55)
c  file with mapping parameters is called poly.par
      open(4,file='./output/poly.par',status='unknown')
      read(4,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
      do 981 j=1,120000
         jj=j*2+2
  981 read(4,999,end=57)a(jj),b(jj),c(jj)
   57 n=jj
      write(*,*)' number of vertices =',n
      write(*,*)' type 1 if you wish to evaluate at prevertices and',
     1' at intermediate points'
      write(*,*)' type 0 to skip'
      write(*,*)'  '
      read(*,*)nchoice
      if(nchoice.lt.1)goto 50
c file with preimages of vertices is called poly.pre
      open(1,file='./output/poly.pre',status='old')
c file with image of unit circle is poly.img
      open(7,file='./output/poly.img',status='unknown')
      zm=dcmplx(0.d0,0.d0)
      do 55 j=1,120000
         read(1,999,end=56)x,y
         z(j)=dcmplx(x,y)
         if(z(j).eq.zm)then
            jm=j-1
            write(*,*)' WARNING: prevertices',j,' and',jm,' are equal'
            write(*,*)' eliminate two vertices and redo zipper program'
         endif
         zm=z(j)
   55 continue
   56 n=j-1
      write(*,*)' number of data points=',n
      write(*,*)' Enter m. The map will be evaluated at n*m points  '
      read(*,*)m
      call intmed
      mn=n*m
      open(8,file='./output/poly.int',status='unknown')
      do 980 j=1,mn
         x=dreal(z(j))
         y=dimag(z(j))
  980 write(8,999)x,y
      write(*,*)' poly.int has prevert. and interm. pts.'
      call bndry
      do 983 j=1,mn
         x=dreal(z(j))
         y=dimag(z(j))
  983 write(7,999)x,y
      x=dreal(z(1))
      y=dimag(z(1))
      write(7,999)x,y
      write(*,*)' file with image of circle is poly.img'
   49 write(*,*)' type 1 if you wish to evaluate map at points in the',
     1' interior of the disk'
      write(*,*)' type 0 to skip'
      write(*,*)'  '
      read(*,*)nchoice
      if(nchoice.lt.1)goto 51
  50  write(*,*)' name of file with points in the open disk?  '
      read(*,80)filenm
      open(2,file=filenm,status='old')
      write(*,*)' name of file for image of these points?  '
      read(*,80)filenm
      open(3,file=filenm,status='unknown')
      do 65 j=1,120000
          read(2,*,end=66)x,y
          z(j)=dcmplx(x,y)
   65 continue
      write(*,*)' Exceeded 120000 pts. Split file or recompile'
      write(*,*)' Will compute image of first 120000 points'
   66 mm=j-1
      write(*,*)' number of data points=',mm
      call eval(mm)
      do 984 j=1,mm
         x=dreal(z(j))
         y=dimag(z(j))
  984 write(3,999)x,y
      go to 49
  999 format(3f25.15)
   51 stop
      end
c
      subroutine intmed
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,m,n
      dimension zt(120001)
      pi=3.14159265358979324d0
      tpi=2.d0*pi
      if(m.le.1)return
      if(m*n.gt.120000)then
         write(*,*)' m is too big. Either decrease m or increase'
         write(*,*)'   the dimensions in the program and recompile'
         stop
      endif
      do 1 j=1,n
         zt(j)=z(j)
    1 continue
      np=n+1
      zt(np)=z(1)
      j=0
      xp=1.d0/dfloat(m)
      do 19 kk=1,n
          thet=dimag(cdlog(zt(kk+1)/zt(kk)))
          if(thet.lt.0.d0)thet=thet+tpi
          dt=thet/dfloat(m)
          zd=cdexp(dcmplx(0.d0,dt))
C          zd=(zd/cdabs(zd))**xp
          j=j+1
          z(j)=zt(kk)
          do 18 k=1,m-1
              j=j+1
              z(j)=z(j-1)*zd
   18     continue
   19 continue
      return
      end
c
c
c        computes map from circle to boundary of region at vertices and
c        intermediate points.
c
      subroutine bndry
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,m,n
c-------map to sector of circle--------------------------------------
      pi=3.14159265358979324d0
      zi=dcmplx(0.d0,1.d0)
      zto0c=dconjg(zto0)
      angle=1.d0/angler
      z1rot=zi**angle
      mn=m*n
      do 1 j=2,mn
         zt=zi*(zto0-z(j)*zto0c)/(z(j)-1.d0)
         if(cdabs(zt).gt.1.d-14)then
            zt=z1rot*(zt**angle)
         else
            zt=dcmplx(0.d0,0.d0)
         endif
         z(j)=zt/(zrot2+zt/zto1)
    1 continue
         z(1)=zto1
c---push up curve-----------------------------------------------------
      do 2 k=n-2,4,-2
         aa=a(k)
         bb=1.d0/b(k)
         cc=c(k)*bb
c----use real arithmetic on points that remain on real line----------
         mend1=m*(k-2)
         mend2=mend1+m+m
         pia=pi*aa
         do 3 j=1,mend1
            xma=dreal(z(j))-aa
            xmap=xma+1.d0
            xratio=xma/xmap
            if(xratio.lt.1.d-20)xratio=dabs(xratio)
            xfract=(xratio)**aa
            x=xmap*xfract
            z(j)=x/(bb-cc*x)
    3    continue
         z(mend1+1)=dcmplx(0.d0,0.d0)
c----force correct branch for points to be mapped to slit--------------
         zdirec=dcmplx(dcos(pia),dsin(pia))
         do 4 j=mend1+2,mend2+1
            xzj=dreal(z(j))
            if((xzj.gt.-1.d-20).and.(j.le.mend2))xzj=-1.d-20
            xma=xzj-aa
            xmap=xma+1.d0
            if(xmap.lt.1.d-20)xmap=1.d-20
            xfract=(-xma/xmap)**aa
            zx=xmap*xfract*zdirec
            z(j)=zx/(bb-cc*zx)
    4    continue
c----complex arithmetic for points in the upper half plane-------------
         do 5 j=mend2+2,mn
            zma=z(j)-aa
            zmap=zma+1.d0
            zfract=(zma/zmap)**aa
            zt=zmap*zfract
            z(j)=zt/(bb-cc*zt)
    5    continue
    2 continue
c-----map upper half plane to complement of circular arc---------------
      do 6 j=1,mn
         zt=-z(j)*z(j)/zrot1
         z(j)=(z3-z1*zt)/(1.d0-zt)
    6 continue
c----Estimate relative error-------------------------------------------
      if(m.lt.4)return
      xmax=dreal(z(1))
      xmin=dreal(z(1))
      ymax=dimag(z(1))
      ymin=dimag(z(1))
      do 7 j=2,mn
         x=dreal(z(j))
         y=dimag(z(j))
         if(x.gt.xmax)xmax=x
         if(x.lt.xmin)xmin=x
         if(y.gt.ymax)ymax=y
         if(y.gt.ymin)ymin=y
    7 continue
      scale=dsqrt((xmax-xmin)*(xmax-xmin)+(ymax-ymin)*(ymax-ymin))
      z(mn+1)=z(1)   
      error=0.d0
      do 8 k=1,n
         z0=z((k-1)*m+1)
         zrot=z(k*m+1)-z0
         azrot=cdabs(zrot)
         if(azrot.lt.1.d-20)goto 8
         zf=azrot/(zrot*scale)
         do 9 j=2,m-1
            et=dabs(dimag((z((k-1)*m+j)-z0)*zf))
            if(et.gt.error)error=et
    9    continue
    8 continue
      write(*,*)
      write(*,*)' Estimate of the relative error is in the file'
      write(*,*)' called error.est.'
      write(*,*)
      open(9, file='./output/error.est',status='unknown')
      write(9,99)error
   99 format(f10.7)
      return
      end
c
c
c        computes map from open disk to region at vector z
c
      subroutine eval(mm)
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,m,n
      pi=3.14159265358979324d0
      tpi=2.d0*pi
c-------map to sector of circle--------------------------------------
      zi=dcmplx(0.d0,1.d0)
      zto0c=dconjg(zto0)
      angle=1.d0/angler
      z1rot=zi**angle
      do 1 j=1,mm
         zm=z(j)-1.d0
         if(cdabs(zm).lt.1.d-14)then
            zt=1.d+30
            zt=zt*z1rot
         else
            zt=zi*(zto0-z(j)*zto0c)/zm
            zt=z1rot*(zt**angle)
         endif
         z(j)=zt/(zrot2+zt/zto1)
    1 continue
c---push up curve-----------------------------------------------------
      do 2 k=n-2,4,-2
         aa=a(k)
         zrota=cdexp(dcmplx(0.d0,tpi*aa))
         bb=1.d0/b(k)
         cc=c(k)*bb
         do 5 j=1,mm
            zma=z(j)-aa
            zmap=zma+1.d0
C  9-13-2009 changed 1.d-14 to 1.d-28
            if(cdabs(zma*zmap).lt.1.d-28)then
               zt=0.d0
            else
               zr=zma/zmap
               zfr=zr**aa
c   get the branch right:
               if(dimag(zfr).lt.0.d0)then
                  if(dreal(zr).lt.0.d0)zfr=zfr*zrota
               endif
               zt=zmap*zfr
            endif
            z(j)=zt/(bb-cc*zt)
    5    continue
    2 continue
c-----map upper half plane to complement of circular arc---------------
      do 6 j=1,mm
         zt=-z(j)*z(j)/zrot1
         z(j)=(z3-z1*zt)/(1.d0-zt)
    6 continue
      return
      end

