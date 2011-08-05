"""
Conformal Mapping from an arbitrary polygon in the complex plane
to the unit disk.

AUTHOR:

- Simon Spicer -- 2011

TESTS:

Pickling test::

    sage: C = Conformal()
    sage: C == loads(dumps(C))
    False
"""

########################################################################
#       Copyright (C) 2011 William Stein <wstein@gmail.com>
#       Copyright (C) 2011 Simon Spicer  <haikona@gmail.com>
#
#  Distributed under the terms of the GNU General Public License (GPL)
#
#    This code is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    General Public License for more details.
#
#  The full text of the GPL is available at:
#
#                  http://www.gnu.org/licenses/
########################################################################


import numpy as np
import zipper_routines
import pylab
from matplotlib.colors import hsv_to_rgb

class Conformal:      
    def __init__(self, data="/Users/simonspicer/zipper/src/zipper/samples/cat.txt", \
                 intpt_from_file=False, interior_point=None, N=200,exterior_map=False):

        self._infinity = np.complex(2**128,0)
        self._interior_point = None

        # Input data parsing: either a filename string or a Numpy array
        if type(data)==str:
            try:
                A      = np.loadtxt(data)
                B      = A[:,0] +1j*A[:,1]
            except:
                raise IOError("Please enter a valid file name.")
        elif type(data==np.ndarray):
            if np.isrealobj(data) and len(data.shape)==2 and data.shape[1]==2:
                B      = data[:,0] +1j*data[:,1]
            elif np.iscomplexobj(data) and len(data.shape)==1:
                B      = data
            else:
                raise IOError("Input must be a 1-d complex array or an array of real pairs.")
        else:
            raise IOError("Input must be a valid file name or Numpy array.")

        # Interior point parsing
        if intpt_from_file:
            self._interior_point  = B[-1]
            self._polygon_raw = B[:-1]
        else:
            self._polygon_raw = B
        if interior_point is not None:
            try:
                self._interior_point = \
                    np.complex(np.real(interior_point),np.imag(interior_point))
            except:
                raise IOError("Invalid interior point.")
        if self._interior_point is None:
            self._interior_point = np.mean(self._polygon_raw)


        self._polygon_raw = B[:-1]
#        B      = zipper_routines.polygon(B[:-1], self._interior_point, N)
        B      = zipper_routines.polygon(self._polygon_raw, self._interior_point, N)
        self._polygon  = np.trim_zeros(B)

        params1,ABC1,preimage1 = zipper_routines.zipper(self._polygon,self._interior_point)
        params2,ABC2,preimage2 = zipper_routines.zipper(self._polygon[::-1],self._infinity)
        self._map_parameters = ((params1,ABC1),(params2,ABC2))
        self._polygon_preimage = (preimage1, preimage2)
                       
    def __repr__(self):
        return "Conformal map."
        
    def write_to_file(self, filename, data, interior_point=False):
        
        np.savetxt(filename, data, delimiter='\t')
        
        if interior_point:
            f = open(filename, 'a')
            f.write('\n')
            f.write(str(self._interior_point[0]) + '\t' + str(self._interior_point[1]))
            f.close()        
        
    def forward_map(self, data, exterior_map=False, phase=False):
        
        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute image of first 120000 points.")
            data = data[:120000]
        
        if exterior_map:
            Z = zipper_routines.forward(1./data,self._map_parameters[1][0],self._map_parameters[1][1])
        else:
            Z = zipper_routines.forward(data,self._map_parameters[0][0],self._map_parameters[0][1])

        if phase:
            angles = np.angle(data)
            return Z,angles
        else:
            return Z

    def inverse_map(self, data, exterior_map=False, phase=False):

        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute preimage of first 120000 points.")
            data = data[:120000]

        if exterior_map:
            Z = 1./zipper_routines.inverse(data,self._map_parameters[1][0],self._map_parameters[1][1])
        else:
            Z = zipper_routines.inverse(data,self._map_parameters[0][0],self._map_parameters[0][1])
        #Z = zipper_routines.inverse(data,self._map_parameters[0],self._map_parameters[1])

        if phase:
            angles = np.angle(data)
            return Z,angles
        else:
            return Z
        
    def interior_point(self):
        return self._interior_point
        
    def polygon(self,refined=False):
        if refined:
            return self._polygon
        else:
            return self._polygon_raw
        
    def polygon_preimage(self):
        return self._polygon_preimage
        
    def grid(self, point_density=50, exterior_map=False):
        if exterior_map:
            Z = zipper_routines.imagegr(self._polygon[::-1],self._infinity,point_density)
        else:
            Z = zipper_routines.imagegr(self._polygon,self._interior_point,point_density)
        return np.trim_zeros(Z)

    def carleson_grid(self, generations=6, sublevels=1, exterior_map=False):
        Z = np.trim_zeros(zipper_routines.cgrids(generations,sublevels))
        Z = zipper_routines.corders(Z,generations,sublevels)
        
        if exterior_map:
            return 1./np.trim_zeros(Z)
        else:
            return np.trim_zeros(Z)

    def welding(self):
        D = self.polygon_preimage()
        E1,E2 = np.log(D[0]).imag,np.log(D[1]).imag
        E1 = E1 + (E1<0)*2*np.pi
        E2 = E2 + (E2<0)*2*np.pi
        E = np.array((E1,E2))
        return E

    def hue(self, X):
        V = np.angle(-X)
        V = (V/(2*np.pi) + 0.5)
        W = np.array((V,np.ones(V.shape),np.ones(V.shape))).T.reshape(-1,1,3)
        return hsv_to_rgb(W).reshape(-1,3)
