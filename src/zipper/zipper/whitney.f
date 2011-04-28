      implicit double precision(a-h,o-y),complex*16(z)
      dimension z(10000),zc(10000),zcn(10000),zv(5)
      character*55 filenm
      write(*,*)' Name of file with boundary of region?'
      read(*,80)filenm
   80 format(a55)
      open(1,file=filenm,status='unknown')
      write(*,*)'  '
      write(*,*)' Name of file with whitney squares?'
      read(*,80)filenm
      open(2,file=filenm,status='unknown')
      xmax=-1.d99
      xmin=1.d99
      ymax=-1.d99
      ymin=1.d99
      do 1 j=1,9999
         read(1,*,end=2)x,y
         xmax=dmax1(xmax,x)
         xmin=dmin1(xmin,x)
         ymax=dmax1(ymax,y)
         ymin=dmin1(ymin,y)
         z(j)=dcmplx(x,y) 
    1 continue
      write(*,*)' Exceeded 10,000 pts, will use first 10,000'
    2 npts=j-2
      nptsp=npts+1
      z(nptsp)=z(1)
      if(cdabs(z(npts)-z(1)).lt. 1.d-12)then
         npts=npts-1
         nptsp=nptsp-1
      endif
      zc(1)=dcmplx((xmax+xmin)/2.d0,(ymax+ymin)/2.d0)
      tl=dmax1(xmax-xmin,ymax-ymin)
      mn=dlog(tl)/dlog(2.d0)
      xll=2.d0**(mn)
      nsq=1
      zv(1)=dcmplx(xll,xll)
      zv(2)=dcmplx(-xll,xll)
      zv(3)=dcmplx(-xll,-xll)
      zv(4)=dcmplx(xll,-xll)
      zv(5)=zv(1)
c      do 19 jj=1,5
c         xx=dreal(zc(1)+zv(jj))
c         yy=dimag(zc(1)+zv(jj))
c         write(2,99)xx,yy
c   19 continue
c      write(2,*)'  '
      write(*,*)' Number of levels?'
      read(*,*)nlevels 
      do 3 m=1,nlevels
         do 33 j=1,5
            zv(j)=zv(j)/2.d0
   33 continue
         nsqn=0
         do 4 jsq=1,nsq
            do 5 jp=1,4
               zcen=zc(jsq)+zv(jp)
               zvert=zcen+4*zv(1)
               do 6 jv=1,4
                  jvp=jv+1
                  zvertp=zcen+4.d0*zv(jvp)
                  xm=dreal(zvertp)
                  ym=dimag(zvertp)
                  if((xmax.le.xm).and.(ymax.le.ym))then
                     xm=dreal(zcen-4.d0*zv(1))
                     ym=dimag(zcen-4.d0*zv(1))
                     if((xmin.ge.xm).and.(ymin.ge.ym))then
                         nsqn=nsqn+1
                         zcn(nsqn)=zcen
                         goto 5
                     endif
                  endif
                  do 7 j=1,npts
                     if(cdabs(zvert-z(j)).lt.1.d-8)then
                           nsqn=nsqn+1
                           zcn(nsqn)=zcen
                           goto 5
                     elseif(cdabs(zvertp-z(j)).lt.1.d-8)then
                           nsqn=nsqn+1
                           zcn(nsqn)=zcen
                           goto 5
                     endif      
                     s1=dimag(dconjg(z(j+1)-z(j))*(zvert-z(j)))
                     s2=dimag(dconjg(z(j+1)-z(j))*(zvertp-z(j)))
                     if(s1*s2.lt.0.d0)then
                        s3=dimag(dconjg(zvertp-zvert)*(z(j)-zvert))
                        s4=dimag(dconjg(zvertp-zvert)*(z(j+1)-zvert))
                        if(s3*s4.lt.0.)then
                           nsqn=nsqn+1
                           zcn(nsqn)=zcen
                           goto 5
                        endif
                     endif
    7             continue
                  zvert=zvertp
    6          continue
               wind=0.d0
               do 8 ii=1,npts
                  z1=z(ii)-zcen
                  z2=dconjg(z(ii+1)-zcen)
                  wind=wind+dimag(cdlog(z2*z1))
    8          continue 
               if(dabs(wind).ge.1.d-6)then
                  do 9 kv=1,4
                     zwv=zcen+zv(kv)
                     xx=dreal(zwv)
                     yy=dimag(zwv)
                     write(2,99)xx,yy
    9             continue
               endif
    5       continue
    4    continue
         nsq=nsqn
         do 11 jnsq=1,nsq
            zc(jnsq)=zcn(jnsq)
   11    continue
    3 continue
   99 format(2f22.15)
      stop 
      end


