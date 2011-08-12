"""
Conformal Mapping from an arbitrary polygon in the complex plane
to the unit disk.

AUTHOR:

- Simon Spicer -- 2011

TESTS:

Pickling test::

    sage: import numpy as np
    sage: C = Conformal(np.array([1.,1.*1j,-1.,-1.*1j]),0)
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
    """
    The Conformal class, containing a conformal map from the interior of the unit disk
    to a given polygon, and a map from the exterior of the disk to the exterior of the
    polygon.
    """
    def __init__(self, data, intpt_from_file=False, interior_point=None, N=200, \
                 preprocess=True, normalization="default"):
        """
        INPUT: 
        
        - ''data''            - Either a 1-d numpy array of complex points defining a
                                simply connected polygon in the complex plane;
                                or a 2 x n numpy array of real pairs defining a simply
                                connected polygon in RR^2;
                                or the name of a file containing data that can be
                                parsed into the above format
        - ''interior_point''  - Either a point in the interior of the polygon, to
                                which 0 is mapped in the forward conformal map;
                                or the string 'from_data', in which case the interior 
                                point is taken to be last point in the data file/array
        - ''N''               - The approximate number of points to be used on the
                                boundary of the polygon in the conformal map. Use more
                                points for higher precision

    EXAMPLES::

        sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
        sage: Conformal(X,0)
        Conformal map defined by supplied data with interior point 0j.
        sage: X = np.array([1.,1.*1j,-1.,-1.*1j,0+0.1*1j])
        sage: Conformal(X,interior_point="from_data",N=500)
        Conformal map defined by supplied data with interior point 0.1j.
    """

        self._infinity = np.complex(2**128,0)
        self._interior_point = None

        # Input data parsing: either a filename string or a numpy array
        if type(data)==str:
            try:
                A      = np.loadtxt(data)
                if np.isrealobj(A) and len(A.shape)==2 and A.shape[1]==2:
                    B      = A[:,0] +1j*A[:,1]
                elif np.iscomplexobj(data) and len(data.shape)==1:
                    B      = A
                else:
                    raise IOError("Data file must contain a 1-d complex array"\
                                      +" or an array of real pairs.")
            except:
                raise IOError("Please enter a valid file name.")
        elif type(data==np.ndarray):
            if np.isrealobj(data) and len(data.shape)==2 and data.shape[1]==2:
                B      = data[:,0] +1j*data[:,1]
            elif np.iscomplexobj(data) and len(data.shape)==1:
                B      = data
            else:
                raise IOError("Input must be a 1-d complex array"\
                              +" or an array of real pairs.")
        else:
            raise IOError("Input must be a valid file name or Numpy array.")

        # Interior point parsing
        if interior_point=="from_data":
            self._interior_point  = B[-1]
            self._polygon_raw = B[:-1]
        else:
            self._polygon_raw = B
            try:
                self._interior_point = \
                    np.complex(np.real(interior_point),np.imag(interior_point))
            except:
                raise IOError("Invalid interior point.")

        # Format polygon using zipper_routines.polygon()
        if preprocess:
            B,njs    = zipper_routines.polygon(self._polygon_raw, self._interior_point, N)
            self._polygon  = B[:njs]
        else: self._polygon = self._polygon_raw

        # Obtain conformal map parameters and polygon preimages
        params1,ABC1,preimage1 = zipper_routines.zipper(self._polygon,self._interior_point)
        params2,ABC2,preimage2 = zipper_routines.zipper(self._polygon[::-1],self._infinity)
        self._map_parameters = ((params1,ABC1),(params2,ABC2))
        self._polygon_preimage = (preimage1, preimage2)

        # Set normalization
        self.set_normalization(normalization)
                       
    def __repr__(self):
        s = "Conformal map defined by supplied data with interior point "
        s += str(self._interior_point) + "."
        return s
        
    def write_to_file(self, filename, data, interior_point=False):
        
        np.savetxt(filename, data, delimiter='\t')
        
        if interior_point:
            f = open(filename, 'a')
            f.write('\n')
            f.write(str(self._interior_point[0]) + '\t' + str(self._interior_point[1]))
            f.close()

    def set_normalization(self, s):

        if s=="default":
            # Get derivative of unnormalized conformal map at origin
            a1=self._get_forward_map_derivative_at_origin()
            # Set normalization to the argument of a1 
            self._normalization = np.mod(-np.log(a1).imag, 2*np.pi)
        elif s=="None":
            self._normalization = np.float64(0)
        else:
            self._normalization = np.mod(s,2*np.pi)

    def _get_boundary_distance(self):
        return np.min(np.abs(self.polygon(refined=True) - self.interior_point()))        
    
    def _get_forward_map_derivative_at_origin(self):
        boundary_distance = self._get_boundary_distance()
        X = np.array([1.,1.*1j,-1.,-1.*1j])
        X = X*0.001*boundary_distance
        # a is the derivative of the inverse map at the interior point
        a = 0.25*sum(self._inverse_map_unnormalized(X + self._interior_point)/X)
        return 1/a
    
    def _forward_map_unnormalized(self, data, exterior_map=False):
        
        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute image of first 120000 points.")
            data = data[:120000]
        
        if exterior_map:
            Z = zipper_routines.forward(1./data,self._map_parameters[1][0],self._map_parameters[1][1])
        else:
            Z = zipper_routines.forward(data,self._map_parameters[0][0],self._map_parameters[0][1])
        return Z

    def _inverse_map_unnormalized(self, data, exterior_map=False):

        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute preimage of first 120000 points.")
            data = data[:120000]

        if exterior_map:
            Z = 1./zipper_routines.inverse(data,self._map_parameters[1][0],self._map_parameters[1][1])
        else:
            Z = zipper_routines.inverse(data,self._map_parameters[0][0],self._map_parameters[0][1])

        return Z

    def forward_map(self, data, exterior_map=False):
        X = np.exp(1j*self._normalization)*data
        return self._forward_map_unnormalized(X,exterior_map=exterior_map)

    def inverse_map(self, data, exterior_map=False):
        X = self._inverse_map_unnormalized(data,exterior_map=exterior_map)
        return np.exp(-1j*self._normalization)*X
        
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
