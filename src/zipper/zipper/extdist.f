C  combination of zipper, inverse,forward, rectl  
      program extdist      
c  slit map program with angles and bends.
c  The program will compute parameters and prevertices of the conformal map, 
c  and its inverse, which maps a Jordan region with the prescribed points on 
c  its boundary to the unit disk.
c        compile with integer*2 for best speed on a PC. Requires double 
c        precision.
c        input:   name of data file which contains:
c                         the boundary points in counterclockwise order,
c                         (the first point will be mapped to 1 on the 
c                         circle- for best accuracy, there should be a 
c                         tangent to the curve at the first 3 points),
c                         then the point in your region to be mapped to 0. 
c                         There should be an EVEN number of points, 
c                         with bends occurring at odd numbered points.
c                from standard input file: answer prompts
c        output:  prevertices (inverse images of vertices)
c                 parameters (data used to evaluate the map)
c
c        written by Donald Marshall 
c                   Mathematics Department
c                   University of Washington  
c                   marshall@math.washington.edu
c
c        This version is limited to 10000 boundary points including the
c          intermediate points.  To increase this limit, replace all
c          occurances of 10001 and 10000 with larger numbers.
c      program zipper
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      character*55 filenm
      common z(10001),a(10001),b(10001),c(10001),z1,z2,z3,zrot1,zto0,
     1zto1,angler,zrot2,mm,n
      dimension zend(4),zr(4),zn(7),index(4),ind2(4)
      write(*,*)' name of file with boundary of region and int. point?'
      read(*,80)filenm
   80 format(a55)
      open(1,file=filenm,status='old')
      open(3,file='poly.pre',status='unknown')
      open(4,file='poly.par',status='unknown')
      open(10,file='arcs1.dat',status='unknown')
      open(17,file='arcs2.dat',status='unknown')
      do 55 j=1,10000
          read(1,*,end=56)x,y
          z(j)=dcmplx(x,y)
   55 continue
      write(*,*)' more than 10000 points (Recompile)'
      stop
   56 n=j-2
      write(*,*)' number of data points=',n
      ntmp=n/2
      neven=ntmp*2
      if(neven.ne.n)then
          write(*,*)' You have an odd number of boundary points.'
          write(*,*)' The program requires an even number of points'
          write(*,*)' Also the bends should occur at odd numbered'
          write(*,*)' vertices for best accuracy'
          stop
      endif
c   Here we test the total change in the
c   argument of the tangent vector to the image of
c   the curve by the map z -> 1/(z-z0) where
c   z0 is the interior point. So it won't always
c   catch a non-Jordan curve. Also it can think
c   a curve is not Jordan if it has too few points,
c   i.e. if the polygonal line between the image
c   of the vertices by the above map is not Jordan.
      pi=3.14159265358979324d0
      wind=0.d0
      z0=z(n+1)
      zdo=1.d0/(z(1)-z0)
      zbo=zdo-1.d0/(z(n)-z0)
      do 993 j=1,n-1
         zd=1.d0/(z(j+1)-z0)
         zb=zd-zdo
         dthet=dimag(cdlog(zb/zbo))
         wind=wind+dthet
         zbo=zb
         zdo=zd
  993 continue
      dthet=dimag(cdlog((1.d0/(z(1)-z0)-zdo)/zbo))
      wind=wind+dthet
      tpi=2.d0*pi
      wind=-wind/tpi
  998 format(f10.1)
      if(dabs(wind*wind-1.d0).gt.0.1)then
          write(*,*)' Your curve is not a Jordan curve.'
          write(*,*)'   '
          write(*,*)' STOP the program and try again.'
          write(*,*)'   '
          stop
      elseif(dabs(wind-1.d0).gt.0.1)then
          nt=n/2
          do 980 j=1,nt
             ztemp=z(j)
             z(j)=z(n-j)
             z(n-j)=ztemp
  980     continue
      endif
      open(2,file='ends.dat',status='unknown')
      do 25 j=1,4
         read(2,*)x,y
         zzz=dcmplx(x,y)
         diff=99.d0
         do 26 k=1,n
            diff1=cdabs(zzz-z(k))
            if(diff1.lt.diff)then
               diff=diff1
               index(j)=k
            endif
   26 continue
   25 continue
cc put ends in proper order
      if(index(1).gt.index(2))then
         itemp=index(1)
         index(1)=index(2)
         index(2)=itemp
      endif
      if(index(2).gt.index(3))then
         itemp=index(2)
         index(2)=index(3)
         index(3)=itemp
      endif
      if(index(3).gt.index(4))then
         itemp=index(3)
         index(3)=index(4)
         index(4)=itemp
      endif
      if(index(1).gt.index(2))then
         itemp=index(1)
         index(1)=index(2)
         index(2)=itemp
      endif
      if(index(2).gt.index(3))then
         itemp=index(2)
         index(2)=index(3)
         index(3)=itemp
      endif
      if(index(1).gt.index(2))then
         itemp=index(1)
         index(1)=index(2)
         index(2)=itemp
      endif
c write out two nonadjacent ends.
      if(index(4).eq.n)then
         x=dreal(z(n))
         y=dimag(z(n))
         write(17,999)x,y
      endif
      do 39 k=1,n
         if((k.gt.index(1)).and.(k.lt.index(2)))goto 39
         if((k.gt.index(3)).and.(k.lt.index(4)))goto 39
         if((k.eq.index(2)).or.(k.eq.index(4)))then
            write(17,*)'   '
         endif
         x=dreal(z(k))
         y=dimag(z(k))
         write(17,999)x,y
   39 continue
      if(index(4).lt.n)then
         x=dreal(z(1))
         y=dimag(z(1))
         write(17,999)x,y
      endif
      do 29 k=index(1),index(4)
         if((k.gt.index(2)).and.(k.lt.index(3)))goto 29
         x=dreal(z(k))
         y=dimag(z(k))
         write(10,999)x,y
         if(k.eq.index(2))then
            write(10,*)'   '
         endif
   29 continue
c switch clockwise to cc
c      if((index(2).lt.index(1)).and.(index(3).lt.index(2)))then
c         indext=index(1)
c         index(1)=index(2)
c         index(2)=indext
c         indext=index(4)
c         index(4)=index(3)
c         index(3)=indext
c      endif
      call invers
      do 27 k=1,4
         zend(k)=z(index(k))
   27 continue
      write(*,*)'  '
      write(*,*)' Finished unzipping region'
      write(*,*)'  '
      write(4,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
      do 981 j=4,n-2,2
  981 write(4,999)a(j),b(j),c(j)
      rewind(4)
      do 982 j=1,n
         x=dreal(z(j))
         y=dimag(z(j))
  982 write(3,999)x,y
  999 format(3f25.15)
      write(*,*)' To map data points in the region to the disk,enter 1'
      write(*,*)' To skip this step, enter 0'
      read(*,*)idat
      if(idat.eq.0)goto 65
      write(*,*)' name of file with data points?'
      read(*,80)filenm
      open(12,file=filenm,status='unknown') 
      do 66 j=1,10000
          read(12,*,end=67)xdat,ydat
          z(j)=dcmplx(xdat,ydat)
   66 continue
      write(*,*)' truncated after 10000 points'    
   67 ndat=j-1
      mm=ndat
      write(*,*)'   '
      call invert
      open(13,file='curve.disk',status='unknown')
      do 68 j=1,ndat
         xdat=dreal(z(j))
         ydat=dimag(z(j))
         write(13,999)xdat,ydat
   68 continue
      write(*,*)' image of data points in disk is in file curve.disk'
   65 continue
c      Takes 4 points on unit circle & finds length of conformally
c      equivalent rectangle with height 1 (points corres. to vertices)
c      Then it computes a grid on the rectangle, suitable for use with 
c      inverse.f
c
      zr1=(zend(1)-zend(3))/(zend(1)-zend(4))
      zr2=(zend(2)-zend(4))/(zend(2)-zend(3))
      s=dreal(zr1*zr2)
      xk=2*s-1.d0+2*dsqrt(s*s-s)
      if(xk.gt.1.d0)xk=1.d0/xk
      ax=1.d0
      bx=xk
    1 tmp=(ax+bx)/2.d0
      bx=dsqrt(ax*bx)
      ax=tmp
      if(dabs(ax-bx).gt.1.d-13)goto 1
      xkk=ax
      xkp=dsqrt(1.-xk*xk)
      ax=1.d0
      bx=xkp
    3 tmp=(ax+bx)/2.d0
      bx=dsqrt(ax*bx)
      ax=tmp
      if(dabs(ax-bx).gt.1.d-13)goto 3
      xkkp=ax
      xlen=2.d0*xkk/xkkp
      write(*,*)' Extremal distance='
      write(*,*)'   length of equivalent rectangle of height 1 ='
      write(*,*)xlen
      write(*,*)'   '
      open(16,file='extlen',status='unknown')
      write(16,995)xlen
  995 format(f10.4)
      mapprox=n
      z1=dcmplx(0.d0,1.d0)
      z2=dcmplx(0.d0,0.d0)
      z3=dcmplx(xlen,0.d0)
      z4=z3+z1
      zint=(z2+z4)/2.d0
      zi=dcmplx(0.d0,1.d0)
      if(xlen.lt.1.d0)then
         zn(1)=dcmplx(0.d0,0.5d0)
         zn(2)=0.d0
         zn(3)=dcmplx(xlen,0.d0)
         zn(4)=zn(3)+zn(1)
         zn(5)=zn(3)+zi
         zn(6)=zi
         zint=zn(4)/2.d0
      else
         zn(1)=dcmplx(xlen/2.d0,0.d0)
         zn(2)=dcmplx(xlen,0.d0)
         zn(3)=zn(2)+zi
         zn(4)=zn(1)+zi
         zn(5)=zi
         zn(6)=0.d0
         zint=zn(3)/2.d0
      endif
      n=6
      perim=2.d0*xlen+2.d0
      zn(n+1)=zn(1)
      njsum=0
      do 4 j=1,n
         xl=cdabs(zn(j+1)-zn(j))
         nj=mapprox*xl/perim
         if(nj.lt.4)nj=4
         itest=nj/2
         itest=2*itest
         if(itest.lt.nj)nj=nj+1
         njsum=njsum+1
         zd=(zn(j+1)-zn(j))/float(nj)
         zz=zn(j)
         z(njsum)=zn(j)
         do 5 k=1,nj-1
            zz=zz+zd
            njsum=njsum+1
            z(njsum)=zz
    5    continue
    4 continue
      nj=njsum+1
      z(nj)=zint
      n=njsum
      write(*,*)' number of points on boundary of rectangle=',n
      zn(1)=zi
      zn(2)=0.d0
      zn(3)=dcmplx(xlen,0.d0)
      zn(4)=zn(3)+zi
      open(11,file='rect.ends',status='unknown')
      do 76 j=1,5
         x=dreal(zn(j))
         y=dimag(zn(j))
         if(j.eq.5)then
            x=dreal(zn(1))
            y=dimag(zn(1))
         endif
         write(11,999)x,y
   76 continue
      do 35 j=1,4
         diff=99.d0
         do 36 k=1,n
            diff1=cdabs(zn(j)-z(k))
            if(diff1.lt.diff)then
               diff=diff1
               ind2(j)=k
            endif
   36 continue
   35 continue
      call invers
      write(*,*)'  '
      write(*,*)' Finished unzipping rectangle'
      write(*,*)'   '
c      open(8,file='rect.par',status='unknown')
c      write(8,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
c      do 983 j=4,n-2,2
c  983 write(8,999)a(j),b(j),c(j)
      do 22 j=1,4
         zr(j)=z(ind2(j))
   22 continue
c  Once we have the map from the rectangle to the disk, zr(j)
c   denotes the points on the circle corresponding to the vertices
c in counter clockwise order, with the first two corresponding to the
c left edge and the last two corresponding to the right edge.
c  zend(j) will denote the points on the circle that correspond
c  via the inverse of the conformal map to the ends of the intervals 
c  for which we want the extremal distance.
c  next we find the l.f.t. mapping zr(j) to zend(j).
      za=(zr(3)-zr(2))/(zr(3)-zr(1))
      zwa=(zend(3)-zend(2))/(zend(3)-zend(1))
      zc1=zwa*zend(1)-za*zend(2)
      zc2=za*zend(2)*zr(1)-zwa*zend(1)*zr(2)
      zc3=zwa-za
      zc4=za*zr(1)-zwa*zr(2)
      z4=(zr(4)*zc1+zc2)/(zc3*zr(4)+zc4)
      diff=cdabs(z4-zend(4))
      if(diff.gt.1.d-4)then
         write(*,*)' vertices off by more than 1.d-4'
         write(*,*)' use more points on boundary of rectangle'
         write(*,*)' diff=',diff
         stop
      endif
      if(idat.eq.0)goto 60
      rewind(13)
      do 63 j=1,ndat
         read(13,*)xdat,ydat
         z(j)=dcmplx(xdat,ydat)
   63 continue
      do 62 j=1,ndat
         z(j)=(z(j)*zc4-zc2)/(-zc3*z(j)+zc1)
   62 continue
      mm=ndat
      call eval 
      open(14,file='curve.rect',status='unknown')
      do 61 j=1,ndat
         xdat=dreal(z(j))
         ydat=dimag(z(j))
         write(14,999)xdat,ydat
   61 continue
      write(14,*)'   '
      xz=0.d0
      xo=1.d0
      open(15,file='rect.dat',status='unknown')
      write(15,*)xz,xz
      write(15,*)xlen,xz
      write(15,*)xlen,xo
      write(15,*)xz,xo
      write(15,*)xz,xz
      write(*,*)' Image (in rect) of given curve is in file curve.rect'
      write(*,*)' Boundary of rectangle is in rect.dat'
      write(*,*)'   '
   60 continue
c    Look at z4 for lft.
c     program square
c  This program computes a grid on  rectangle
c   This was used in conjuction with a conformal map
c   to a rectangle, using 500 boundary points
c  The grid is scaled to have boundary 10^(-4) away
c   from edge of square, to keep points inside image
c   of disk so that can find inverse.  10^(-8) gives
c   errors at points closest to the corners.
c   If 500 is increased, you can make the grid closer.
c     read(*,*)xlen
c     write(*,*)' n = ?'
c     read(*,*)n
      write(*,*)' Enter number of subdivisions of rectangle'
      write(*,*)'  (at most about 40) grid will be approx squares'
      read(*,*)nn
      if(xlen.lt.1.d0)nn=nn/xlen
      xnn=nn
      dy=(1.d0/xnn)/(1.d0+2.d-3)
      m=idint(nn*xlen)
      xm=m
c    won't exactly be squares, but will fit evenly:
c  0.le.(1-dy/dx).le. 1/(nn*xlen)
      dx=xlen/(xm*(1.d0+(2.d-3)/xlen))
      ind=1
      z(1)=dcmplx(1.d-3,1.d-3)
      do 52 j=1,m+1
         do 53 k=1,nn
            ind=ind+1
            z(ind)=z(ind-1)+dcmplx(0.d0,dy)
   53    continue
         ind=ind+1
         z(ind)=z(ind-1)+dx
         dy=-dy
   52 continue
      mm=(m+1)*(nn+1)
      write(*,*)' number of grid points =',mm
      if(mm.gt.10000)then
         write(*,*)' Too many grid points on rectangle.'
         write(*,*)' reduce number of vertical subdiv. of rect.'
         stop
      endif
      open(8,file='rect.gridp',status='unknown')
      do 777 k=1,mm
         x=dreal(z(k))
         y=dimag(z(k))
         write(8,999)x,y
  777 continue
      call invert
      write(*,*)'  '
      write(*,*)' Finished pulling grid back to disk'
      write(*,*)'  '
      do 40 j=1,mm
         z(j)=(z(j)*zc1+zc2)/(zc3*z(j)+zc4)
   40 continue
      write(*,*)' Finished l.f.t. on disk'
c  slit map program with angles and bends.
c  The program will compute values of the conformal map (at prevertices
c  and at intermediate points or at interior points) which maps the disk
c  to a Jordan region with the prescribed points on its boundary. 
c        compile with integer*2 for best speed on a PC. Requires double 
c        precision.
c
c        written by Donald Marshall 
c                   Mathematics Department
c                   University of Washington  
c                   marshall@math.washington.edu
c
c              This version is limited to 10000 boundary points including the 
c              intermediate points.  To increase this limit, replace all
c              occurances of 10001 and 10000 with a larger number.
c      program forward
c      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
c      character*55 filenm
c      common z(10001),a(10001),b(10001),c(10001),z1,z2,z3,zrot1,zto0,
c     1zto1,angler,zrot2,mm,n
c   80 format(a55)
c     
c      read(*,80)filenm
c      open(4,file=filenm,status='unknown')
      read(4,*)z1,z2,z3,zrot1,zto0,zto1,angler,zrot2
      do 985 j=1,10000
         jj=j*2+2
  985 read(4,999,end=57)a(jj),b(jj),c(jj)
   57 n=jj
      call eval
      write(*,*)'  '
      write(*,*)' Finished mapping grid to region'
      write(*,*)'  '
      open(9,file='grid.img',status='unknown')
c Here reorder for graphing purposes
      in=nn+1
      do 12 k=1,m-1
         do 11 j=1,nn+1
            in=in+1
            x=dreal(z(in))
            y=dimag(z(in))
            write(9,999)x,y
   11    continue
         in=in-1
         x=dreal(z(in))
         y=dimag(z(in))
         write(9,999)x,y
         in=in+3
         x=dreal(z(in))
         y=dimag(z(in))
         write(9,999)x,y
         in=in-2
   12 continue
      nnp=nn+1
      in=in+2
      isign=-1
      jsign=-1
      modd=m/2
      moddt=modd*2
      if(moddt.eq.m)jsign=1
c      write(9,*)'m=',m,'nn=',nn
c      write(9,*)in
      in=in+(nnp+nnp-3)
      ik=1
      do 13 k=1,nn-1
         idx=(nnp-2*k-1)*ik
         do 14 j=1,m+1
c      write(9,*)in
            in=in+isign*(nnp+idx)
            idx=-idx
            x=dreal(z(in))
            y=dimag(z(in))
            write(9,999)x,y
   14    continue
c      write(9,*)in
         in=in-isign*(nnp-idx)
         x=dreal(z(in))
         y=dimag(z(in))
         write(9,999)x,y
c      write(9,*)in
         in=in-1 +(1-isign)*(1-jsign)/2
         x=dreal(z(in))
         y=dimag(z(in))
         write(9,999)x,y
         ik=1-(1+jsign)*(1-isign)/2
c      write(9,*)in
         in=in+isign*2*nnp
         isign=-isign
   13 continue
      write(*,*)' Finished reordering points for graphical purposes'
      write(*,*)'  '
      write(*,*)' Image of rectangle grid is in file grid.img '
      write(*,*)' plot it together with poly.dat (boundary)'
      stop
      end



      
c
c
c      computes map from region to disk and finds inverse image of vertices.
c        z(n+1) mapped to 0, z(1) mapped to 1
      subroutine invers
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(10001),a(10001),b(10001),c(10001),z1,z2,z3,zrot1,zto0,
     1zto1,angler,zrot2,mm,n
      pi=3.14159265358979324d0
      acc=1.d-12
c  acc should be at least machine eps **.75
      accr=1.d+12
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
      acc=1.d-12
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




c  test version of newton's method (upper-half plane)
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






c  test version of newton's method (upper-half plane)
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

c
c
c
c        computes map from open disk to region at vector z
c
      subroutine eval
      implicit double precision(a-h,o-y),integer*4(i-n),complex*16(z)
      common z(10001),a(10001),b(10001),c(10001),z1,z2,z3,zrot1,zto0,
     1zto1,angler,zrot2,mm,n
      pi=3.14159265358979324d0
      tpi=2.d0*pi
c-------map to sector of circle--------------------------------------
      zi=dcmplx(0.d0,1.d0)
      zto0c=dconjg(zto0)
      angle=1.d0/angler
      z1rot=zi**angle
      do 1 j=1,mm
         zt=zi*(zto0-z(j)*zto0c)/(z(j)-1.d0)
         zt=z1rot*(zt**angle)
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
            zr=zma/zmap
            zfr=zr**aa
            if(dimag(zfr).lt.0.d0)then
               if(dreal(zr).lt.0.d0)zfr=zfr*zrota
            endif
            zt=zmap*zfr
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

