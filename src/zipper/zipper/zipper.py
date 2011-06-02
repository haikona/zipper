import numpy as np
import zipper_routines
import pylab

#def polygon(points, interior_point, number_of_points):

def load_data():
    infile = "/Users/simonspicer/Desktop/cat.txt"
    intpt  = 0
    N      = 200

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
