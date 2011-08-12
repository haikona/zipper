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
      subroutine forward(zpts,k1,zparams,abc,k2,zout)
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
c      character*55 filenm
      dimension zpts(k1),zparams(8),abc(k2,3),zout(k1)
      common z(120001),a(120001),b(120001),c(120001),z1,z2,z3,zrot1,
     1zto0,zto1,angler,zrot2,m,n
Cf2py intent(in) zpts,k1,zparams,abc,k2
Cf2py intent(out) zout 
c   80 format(a55)
c  file with mapping parameters is called poly.par
c      open(4,file='poly.par',status='unknown')
c      read(4,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
c
c Copyin parameters from zparams
      z1=zparams(1)
      z2=zparams(2)
      z3=zparams(3)
      zrot1=zparams(4)
      zto0=zparams(5)
      zto1=zparams(6)
      angler=zparams(7)
      zrot2=zparams(8)
c
c      do 981 j=1,120000
c         jj=j*2+2
c  981 read(4,999,end=57)a(jj),b(jj),c(jj)
c
c Copying parameters from abc
      do 57 j=1,k2
         jj=j*2+2
         a(jj)=abc(j,1)
         b(jj)=abc(j,2)
         c(jj)=abc(j,3)
   57 continue
c
c The number of vertices of the polygon is 2*(length of abc)+4
      n=k2*2+4
c
c      write(*,*)' number of vertices =',n
c
c      write(*,*)' type 1 if you wish to evaluate at prevertices and',
c     1' at intermediate points'
c      write(*,*)' type 0 to skip'
c      write(*,*)'  '
c      read(*,*)nchoice
c
c
c      nchoice=0
c      if(nchoice.lt.1)goto 50
cc file with preimages of vertices is called poly.pre
c      open(1,file='poly.pre',status='old')
cc file with image of unit circle is poly.img
c      open(7,file='poly.img',status='unknown')
c      zm=dcmplx(0.d0,0.d0)
c      do 55 j=1,120000
c         read(1,999,end=56)x,y
c         z(j)=dcmplx(x,y)
c         if(z(j).eq.zm)then
c            jm=j-1
c            write(*,*)' WARNING: prevertices',j,' and',jm,' are equal'
c            write(*,*)' eliminate two vertices and redo zipper program'
c         endif
c         zm=z(j)
c   55 continue
c   56 n=j-1
c      write(*,*)' number of data points=',n
c      write(*,*)' Enter m. The map will be evaluated at n*m points  '
c      read(*,*)m
c      call intmed
c      mn=n*m
c      open(8,file='poly.int',status='unknown')
c      do 980 j=1,mn
c         x=dreal(z(j))
c         y=dimag(z(j))
c  980 write(8,999)x,y
c      write(*,*)' poly.int has prevert. and interm. pts.'
c      call bndry
c      do 983 j=1,mn
c         x=dreal(z(j))
c         y=dimag(z(j))
c  983 write(7,999)x,y
c      x=dreal(z(1))
c      y=dimag(z(1))
c      write(7,999)x,y
c
c
c      write(*,*)' file with image of circle is poly.img'
c   49 write(*,*)' type 1 if you wish to evaluate map at points in the',
c     1' interior of the disk'
c      write(*,*)' type 0 to skip'
c      write(*,*)'  '
c      read(*,*)nchoice
c      if(nchoice.lt.1)goto 51
c
c
c  50  continue 
c
c
c  50  write(*,*)' name of file with points in the open disk?  '
c      read(*,80)filenm
c      open(2,file=filenm,status='old')
c      write(*,*)' name of file for image of these points?  '
c      read(*,80)filenm
c      open(3,file=filenm,status='unknown')
c      do 65 j=1,120000
c          read(2,*,end=66)x,y
c          z(j)=dcmplx(x,y)
c   65 continue
c      k3 = k1
c      if(k3.lt.120000)goto 64
c         k3=120000
c         write(*,*)' Exceeded 120000 pts. Split file or recompile'
c         write(*,*)' Will compute image of first 120000 points'
c   64 continue  
c
c Copying input data from zpts
      do 65 j=1,k1
          z(j)=zpts(j)
   65 continue
c
c      write(*,*)' Exceeded 120000 pts. Split file or recompile'
c      write(*,*)' Will compute image of first 120000 points'
c   66 mm=j-1
c      write(*,*)' number of data points=',k1
      call eval(k1)
c      do 984 j=1,k1
c         x=dreal(z(j))
c         y=dimag(z(j))    
c  984 write(3,999)x,y
c      go to 49
c  999 format(3f25.15)
c   51 stop
c  51  continue
c
c Copying output to zout
      do 67 j=1,k1
         zout(j)=z(j)
  67  continue    
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
      open(9, file='error.est',status='unknown')
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

