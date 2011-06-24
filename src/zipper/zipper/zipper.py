import numpy as np
import zipper_routines
import pylab

#def polygon(points, interior_point, number_of_points):

def load_data(infile="/Users/simonspicer/Desktop/cat.txt",intpt=0,N=200):
#    infile = "/Users/simonspicer/Desktop/cat.txt"

    A      = np.loadtxt(infile)
    B      = A[:,0] +1j*A[:,1]
    B      = B[:-1]

    X      = zipper_routines.polygon(B, intpt, N)
    Y      = np.trim_zeros(X)

    return Y

def plot(X):
    pylab.plot(pylab.real(X),pylab.imag(X),'r.')
    pylab.axes().set_aspect('equal')
    pylab.show()

def zipper(Y, intpt, preimage=False):
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

def inverse(data,params,abc):
    if len(data)>120000:
        print("Exceeded 10000 pts. Split file or recompile.")
        print("Will compute preimage of first 10000 points.")
        data = data[:10000]
    return zipper_routines.inverse(data,params,abc)
