import numpy as np
import zipper_routines
import pylab

#def polygon(points, interior_point, number_of_points):

def load_data(infile="/Users/simonspicer/Desktop/cat.txt",intpt=0,N=200):
#    infile = "/Users/simonspicer/Desktop/cat.txt"

    A      = np.loadtxt(infile)
    B      = A[:,0] +1j*A[:,1]
    B      = B[:-1]

    intpt  = np.complex(np.real(intpt),np.imag(intpt))
    X      = zipper_routines.polygon(B, intpt, N)
    Y      = np.trim_zeros(X)

    return Y

def plot(X):
    pylab.plot(pylab.real(X),pylab.imag(X),'r.')
    pylab.axes().set_aspect('equal')
    pylab.show()

def zipper(Y, intpt, preimage=False):
    intpt = np.complex(np.real(intpt),np.imag(intpt))
    params,ABC,Y_preimage = zipper_routines.zipper(Y,intpt)

    if preimage==False:
        return params,ABC
    else:
        return params,ABC,Y_preimage

def forward(data,params,abc):
    if len(data)>120000:
        print("Exceeded 120000 pts. Split file or recompile.")
        print("Will compute image of first 120000 points.")
        data = data[:120000]
    return zipper_routines.forward(data,params,abc)

def inverse(data,params,abc,phase=False):
    if len(data)>120000:
        print("Exceeded 10000 pts. Split file or recompile.")
        print("Will compute preimage of first 10000 points.")
        data = data[:10000]
    Y = zipper_routines.inverse(data,params,abc)
#    Z = np.array([r for r in Y if np.abs(r) < 1])
    TF = abs(Y)<1
    Z = Y[TF]

    if phase:
        angles = np.angle(data)
        angles = angles[TF]
        return Z,angles
    else:
        return Z

def imagegr(polygon,intpt,dpi):
    intpt = np.complex(np.real(intpt),np.imag(intpt))
    Z = zipper_routines.imagegr(polygon,intpt,dpi)
    return np.trim_zeros(Z)

def carleson_grid(generations=5,sublevels=0):
    Z = zipper_routines.cgrids(generations,sublevels)
    return np.trim_zeros(Z)

def corders(generations=5,sublevels=0):
    Z = np.trim_zeros(zipper_routines.cgrids(generations,sublevels))
    Z = zipper_routines.corders(Z,generations,sublevels)
    return np.trim_zeros(Z)
