import numpy as np
import polytest

#def polygon(points, interior_point, number_of_points):

def load_data():
    infile = "/Users/simonspicer/Desktop/cat.txt"
    intpt  = 0
    N      = 300

    A      = np.loadtxt(infile)
    B      = A[:,0] +1j*A[:,1]
    B      = B[:-1]

    X      = polytest.polygon(B, intpt, N)
    Y      = np.trim_zeros(X)

    return Y
