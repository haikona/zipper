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
c  This program orders points from cgrids.f for plotting (can be applied
c   to images via a conformal map.)
      program corders
      implicit double precision(a-h,o-y),complex*16(z)
      dimension x(200000),y(200000),x1(200000),y1(200000)
      character*55 filenm
      write(*,*)' number of generations?'
      read(*,*)ngen
      write(*,*)' number of sublevels?'
      read(*,*)nsl
      write(*,*)' name of file with data?'
      read(*,80)filenm
   80 format(a55)
      open(2,file=filenm,status='unknown')
      write(*,*)' name of file for ordered data?'
      read(*,80)filenm
      open(3,file=filenm,status='unknown')
      do 11 i=1,200000
         read(2,*,end=12)x(i),y(i)
   11 continue
   12 nnn=i-1
      write(*,*)' no. data points',nnn
      i=0
      ngen1=ngen-1
      lend=32*nsl
      kend=nsl
      nnsl=nsl
      if(nnsl.eq.0)then
          kend=1
          lend=16
          nsl=1
      endif
      lp=0
      do 1 j=1,ngen1
         if((j.eq.ngen1).and.(nnsl.ne.0))kend=2*nsl
         do 2 k=1,kend
            lnxt=lend
            i=i+1
            x1(i)=x(lp+lend)
            y1(i)=y(lp+lend)
            do 3 l=1,lend
               i=i+1
               lp=lp+1
               x1(i)=x(lp)
               y1(i)=y(lp)
               i=i+1
               if((k.eq.kend).and.(j.lt.ngen1))lnxt=lnxt+1
               if(nsl.eq.0)lnxt=lend
               x1(i)=x(lp+lnxt)
               y1(i)=y(lp+lnxt)
               i=i+1
               x1(i)=x(lp)
               y1(i)=y(lp)
    3       continue
    2    continue
         lend=lend*2
    1 continue
      nnn=i
      write(*,*)'no. pts. =',nnn
      write(3,99)(x1(i),y1(i),i=1,nnn)
   99 format(2f25.15)
      stop
      end
       


