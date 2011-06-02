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
C    You should have received a copy of the GNU General Public License
C    along with this program; if not, write to the Free Software
C    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
C
C
C
C
      program fluid
      implicit double precision(a-h,o-y),complex*16(z)
      character*55 filenm
      dimension zz(10000)
      pi=3.14159265358979324d0
c  stagpt.pre should contain the preimage of the stagnation point
c  infty.pre should contain the preimages of 10**7 and 10**8
c  fluid.inp2 should contain the number of points on each half
c         flow line and the number of flow lines (odd)
c         then 0 for evenly spaced flowlines
c         and  1 for flowlines approaching the boundary geometrically
      open(1,file='stagpt.pre',status='unknown')
      open(4,file='infty.pre',status='unknown')
      open(2,file='input/fluid.inp2',status='unknown')
      read(1,*)x,y
      zs=dcmplx(x,y)
      read(4,*)x,y
      z1=dcmplx(x,y)
      read(4,*)x,y
      z2=dcmplx(x,y)
      read(2,*)nflowpt
      read(2,*)nflows
      read(2,*)flch
      zl1=(z2-z1)/(1.d0-dconjg(z1)*z2)
      zl2=zl1/cdabs(zl1)
c  tau \circ f: oo-> 0 and pos deriv. at oo is given by
c    tau=(zl2)(1-conjg(z1)z)/(z-z1) 
      zss=zl2*(1.d0-dconjg(z1)*zs)/(zs-z1)
c circulation
      circul=2.d0*dimag(zss)
      ccc=dabs(circul*2.d0*pi)
      write(*,*)' circulation/speed = ',ccc
      write(*,*)' stagnation point on circle', zss
c for convenience of computing flow lines from the standard
c region, rotate z-> -z if necessary so that circulation >0
c  (won't affect flow lines)
      if(circul.lt.0)then
          circul=-circul
          zl2=-zl2
      endif
      xc=circul*pi
      xt1=dsqrt(4.d0-circul*circul)
      xt=xt1+circul*dasin(circul/2.d0)+xc/2.d0
c construct horizontal lines on the region {x>0,y<0}U{x>xc,y>0}U
c       {x>xt, y=0}. Then reflect about Re z =0.
      xsize=2.0d0
      ysize=2.0d0
c  increase xsize, ysize  to get bigger picture
      nnfpt=2*nflowpt
      nflowsl=nflows/2
      nflows=2*nflowsl+1
      write(*,*)' output of flow past circle will be in file circflow'
      open(3,file='circflow',status='unknown')
      y=-ysize
      dy=ysize/nflowsl
      dx=xsize*xt/(nflowpt-1)
      z0=dcmplx(xt*xsize,y)-xc/2.d0
      zt=(z0+cdsqrt(z0*z0-4.d0))/2.d0
      zz(1)=(zl2/zt+z1)/(1.d0+dconjg(z1)*zl2/zt)
      do 1 j=1,nflows
         z=zl2*(1.d0-dconjg(z1)*zz(1))/(zz(1)-z1) 
         x=xt*xsize
         if(j.le.nflowsl)then
            dx=xsize*xt/(nflowpt-1)
         elseif(j.eq.nflowsl+1)then
            y=0.d0
            dx=(xsize-1.d0)*xt/(nflowpt-1)
         else
            dx=(xsize*xt-xc)/(nflowpt-1)
         endif
         do 2 k=1,nflowpt
            if((j.eq.nflowsl+1).and.(k.eq.nflowpt))then
               xx=dabs(dreal(zss))
               yy=dabs(dimag(zss))
               zzs=dcmplx(xx,yy)
               rr=1.d0-1.d-13
               zz(nflowpt)=rr*(zl2/zzs+z1)/(1.d0+dconjg(z1)*zl2/zzs)
               zzsc=-dconjg(zzs)
               zz(nflowpt+1)=rr*(zl2/zzsc+z1)/(1.d0+dconjg(z1)*zl2/zzsc)
            else
               zw=dcmplx(x,y)
c       using previous z as initial guess:
               call newt2(zw,circul,z)
               zz(k)=(zl2/z+z1)/(1.d0+dconjg(z1)*zl2/z)
               zc=-dconjg(z)
               nfk=nnfpt-k
               if(j.eq.nflowsl+1)nfk=nfk+1
               zz(nfk)=(zl2/zc+z1)/(1.d0+dconjg(z1)*zl2/zc)
               x=x-dx
            endif
    2    continue
         if(j.eq.nflowsl+1)then
            do 3 k=1,nflowpt
               xz=dreal(zz(k))
               yz=dimag(zz(k))
               write(3,*)xz,yz
    3       continue
            write(3,*)'  '
            do 4 k=1,nflowpt
               xz=dreal(zz(k+nflowpt))
               yz=dimag(zz(k+nflowpt))
               write(3,*)xz,yz
    4       continue
            write(3,*)'  '
         else
            do 5 k=1,nnfpt-1 
               xz=dreal(zz(k))
               yz=dimag(zz(k))
               write(3,*)xz,yz
    5       continue
            write(3,*)'  '
         endif
c code for flow lines approaching boundary geometrically.
c  problem: quickly end up with data crossing boundary
c          need to fix inverse.f so that data points outside
c          the region are replaced by points on boundary.
c          
         if(flch.ne.0)then
            if(j.lt.nflowsl)then
               y=y*0.5d0
            elseif(j.eq.nflowsl)then
               ysmall=y
               y=0.d0
            elseif(j.eq.nflowsl+1)then
               y=-ysmall
            else
               y=y/0.5d0
            endif   
         else
            y=y+dy
         endif
    1 continue
      stop
      end

      subroutine newt2(zw,circul,z)
      implicit double precision(a-h,o-y),complex*16(z)
      zic=dcmplx(0.d0,circul)
      pi=3.14159265358979324d0
      cp2=circul*pi/2.d0
      do 11 j=1,20
         zfz=z+1.d0/z-zic*cdlog(z)+cp2-zw
         if(cdabs(zfz).lt.1.d-13)go to 12
         zfzp=1.d0-(zic+1.d0/z)/z
         z=z-zfz/zfzp
   11 continue
      write(*,*)' More than 20 newton iterations required.'
      write(*,*)' Need better initial guess.'
      write(*,*)zw,z
      stop
   12 return 
      end
