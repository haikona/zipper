      implicit double precision(a-h,o-z)
      dimension x(80000),y(80000)
      open(1,file='./output/data.d',status='unknown')
      pi=3.14159265358979323d0
      npts=0
   2  write(*,*)' radius of circle (will stop if lt 0)'
      read(*,*)rad
      if(rad.lt.0)goto 3
      write(*,*)' number of points on circle?'
      read(*,*)n
      xn=n
      tpin=pi*2.0d0/xn
      np1=n+1
      do 1 j=1,np1
      xj=j-1
      x(npts+j)=rad*dcos(tpin*xj)
    1 y(npts+j)=rad*dsin(tpin*xj)
      npts=npts+np1
      go to 2
    3 nptc=npts
      write(*,*)' number of rays?'
      read(*,*)nrays
      if(nrays.eq.0)go to 5
      xnrays=nrays
      tpin=pi*2.0d0/xnrays
      write(*,*)' number of points on each ray?'
      read(*,*)nrpts
      write(*,*)' beginning radius?'
      read(*,*)rrb
      dx=(1.-1.d-13-rrb)/(nrpts)
      do 7 k=1,nrays
      co=dcos(tpin*k)
      si=dsin(tpin*k)
      rr=rrb
      do 6 j=1,nrpts
      rr=rr+dx
      x(npts+j)=rr*co
    6 y(npts+j)=rr*si
      npts=npts+nrpts
      do 8 j=1,nrpts
      rr=rr-dx
      x(npts+j)=rr*co
    8 y(npts+j)=rr*si
    7 npts=npts+nrpts
    5 length=8*npts+4
      write(*,*)' no. points =',npts
      write(*,*)' output will be in file data.d'
  999 format(2f25.15)
      do 10 j=1,nptc
          write(1,999)x(j),y(j)
   10 continue
      write(1,*)'   '
      do 11 j=nptc+1,npts
          write(1,999)x(j),y(j)
   11 continue
      stop
      end

