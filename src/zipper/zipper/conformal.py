"""
Conformal Mapping from a data-defined region in the complex plane
to the unit disk.

AUTHOR:

- Simon Spicer -- 2012

TESTS:

Pickling test::

    sage: import numpy as np
    sage: C = Conformal(np.array([1.,1.*1j,-1.,-1.*1j]),0)
    sage: C == loads(dumps(C))
    False
"""

########################################################################
#       Copyright (C) 2012 Simon Spicer  <haikona@gmail.com>
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
from copy import deepcopy

# Input data parsing: either a filename string or a numpy array
def load_data(data):
    """
    Loads and correctly formatting data into a 1-D complex Numpy array.

    INPUT:

    - ''data'' - Either the string of a file name of a saved Numpy 
                  array, or the Numpy array itself. The array must
                  either be a 1-D array of complex values, or an nx2
                  array of real values.

    OUTPUT:

     A 1-D complex Numpy array.

    EXAMPLES::

    """
    if type(data)==str:
        try:
            A      = np.loadtxt(data)
            if np.isrealobj(A) and len(A.shape)==2 and A.shape[1]==2:
                return A[:,0] +1j*A[:,1]
            elif np.iscomplexobj(data) and len(data.shape)==1:
                return A
            else:
                raise IOError("Data file must contain a 1-d complex array"\
                                      +" or an array of real pairs.")
        except:
            raise IOError("Please enter a valid file name.")
    elif type(data==np.ndarray):
        if np.isrealobj(data) and len(data.shape)==2 and data.shape[1]==2:
            return data[:,0] +1j*data[:,1]
        elif np.iscomplexobj(data) and len(data.shape)==1:
            return data
        else:
            raise IOError("Input must be a 1-d complex array"\
                          +" or an array of real pairs.")
    else:
        raise IOError("Input must be a valid file name or Numpy array.")


def grid_exterior(L,m=100,D=1.5):
    """
    Creates an array of evenly spaced points on the exterior of a list
     of data-defined regions.

    INPUT:

     - ''L'' - A list of 1-D complex Numpy arrays. The points in the
                arrays must be orientated in a clockwise direction, and
                an interior point must be supplied as the last point of
                each array.

    - ''m''  - Positive integer, default 100. The point density of the
                array (i.e. the array will be 100x100 before points are
                cut out).

    - ''D''  - Positive real number, default 1.5. The diameter of the 
                array of returned points relative to the diameter of
                the supplied data (i.e. the max vertical/horizontal
                distance between input data points).

    OUTPUT:

     A 1-D complex array of points.

    """
    if L==[]:
        raise InputError("Input list must be nonempty.")
    
    # Compute input data bounds
    xmin=np.real(L[0][0])
    xmax=xmin
    ymin=np.imag(L[0][0])
    ymax=ymin
    
    for A in L:
        X = np.real(A[:-1])
        Y = np.imag(A[:-1])
        x1 = np.min(X)
        x2 = np.max(X)
        y1 = np.min(Y)
        y2 = np.max(Y)
        
        xmin = np.min([xmin,x1])
        xmax = np.max([xmax,x2])
        ymin = np.min([ymin,y1])
        ymax = np.max([ymax,y2])

    # Output array radius
    r = D/2*np.max([xmax-xmin,ymax-ymin])
    
    # Output array bounds
    xleft  = (xmin+xmax)/2 - r
    xright = (xmin+xmax)/2 + r
    yleft  = (ymin+ymax)/2 - r
    yright = (ymin+ymax)/2 + r
    
    # Create array
    X = np.arange(xleft,xright,(xright-xleft)/m,dtype=np.float64)
    Y = np.arange(yleft,yright,(yright-yleft)/m,dtype=np.float64)
    XX,YY = np.meshgrid(X,Y)
    G = (XX+1j*YY).flatten()
    
    # Excise points
    for A in L:
        G = np.trim_zeros(zipper_routines.gridext(G,A[:-1],A[-1]))
        
    return G


def hue(X):
    """
    Returns an array of triples corresponding to the hue of the
    phase of the input array. Useful when plotting points on the
    complex plane coloured by phase.

    INPUT:

    - ''X'' - 1d complex Numpy array of points

    OUTPUT:

    - An n*3 real Numpy array of values in the range [0,1].

    EXAMPLES:: 

        sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
        C = Conformal(X,interior_point=0)
        sage: C.hue(X)
        array([[ 1. ,  0. ,  0. ],
        [ 0.5,  1. ,  0. ],
        [ 0. ,  1. ,  1. ],
        [ 0.5,  0. ,  1. ]])

    """
    V = np.angle(-X)
    V = (V/(2*np.pi) + 0.5)
    W = np.array((V,np.ones(V.shape),np.ones(V.shape))).T.reshape(-1,1,3)
    return hsv_to_rgb(W).reshape(-1,3)


class Conformal:
    """
    The Conformal class, containing a conformal map from the unit disk to a region
    defined by a Jordan curve passing through supplied data points.

    INPUT: 
        
        - ''data''            - Either a 1-d numpy array of complex points defining a
                                 simply connected polygon in the complex plane;
                                or a 2 x n numpy array of real pairs defining a simply
                                 connected polygon in RR^2;
                                or the name of a file containing data that can be
                                 parsed into the above format
        - ''interior_point''  - Either a point in the interior of the region, to
                                 which 0 is mapped in the forward conformal map;
                                or the string 'from_data', in which case the interior 
                                 point is taken to be last point in the data file/array
        - ''N''               - The approximate number of points to be used on the
                                 boundary of the region in the conformal map. Use more
                                 points for a curve that more closely approximates
                                 the polygon whose vertices are given by the supplied
                                 data.
        - ''preprocessor''    - Boolean flag, switching the preprocessor on or off
                                 (default is on). The preprocessor reorders the input
                                 and adds interpolating points for increased numerical
                                 stability.
        - ''normalization''   - Boolean flag; default is True.
                                If True, the map is normalized so that the derivative
                                 of the forward map at the origin is positive real;
                                if False, the point 1+0*1j is sent to the first point
                                 of the input array.

    EXAMPLES::

        sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
        sage: Conformal(X,0)
        Conformal map defined by supplied data with interior point 0j.
        sage: X = np.array([1.,1.*1j,-1.,-1.*1j,0+0.1*1j])
        sage: C = Conformal(X,interior_point="from_data",N=500); C
        Conformal map defined by supplied data with interior point 0.1j.
        sage: C.polygon(refined=False).shape, C.polygon(refined=True).shape
        ((4,), (400,))

    """
    def __init__(self, data, interior_point=None, N=200, \
                 preprocessor=True, normalization=True):
        """
        Initialises the Conformal class. See the class 'Conformal' for full
        documentation on the input of this initialization method.
        """


        B = load_data(data)

        # Interior point parsing
        self._interior_point = None
        if interior_point=="from_data":
            self._interior_point  = B[-1]
            self._polygon_raw = B[:-1]
        else:
            self._polygon_raw = B
            try:
                self._interior_point = \
                    np.complex128(interior_point)
            except:
                raise IOError("Invalid interior point.")

        # Format polygon using zipper_routines.polygon();
        # Obtain conformal map parameters and polygon preimages 
        if preprocessor:
            B,pindex,njs  = zipper_routines.polygon(self._polygon_raw,\
                                                    self._interior_point, N)
            self._polygon = B[:njs]
            self._preprocessor = True

            params1,ABC1,preimage1 = zipper_routines.zipper(self._polygon,\
                                                            self._interior_point)
            self._map_parameters       = (params1,ABC1)
            self._polygon_preimage     = preimage1
            self._polygon_preimage_raw = preimage1[pindex-1]
        else:
            self._preprocessor = False

            params1,ABC1,preimage1 = zipper_routines.zipper(self._polygon_raw,\
                                                            self._interior_point)

            self._map_parameters       = (params1,ABC1)
            self._polygon_preimage_raw = preimage1

        # Set normalization
        self.set_normalization(normalization)
                       
    def __repr__(self):
        """
        String representation of the Conformal class object.

        OUTPUT:

        String representation of the Conformal class object.

        EXAMPLES::
        
            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: Conformal(X,0)
            Conformal map defined by supplied data with interior point 0j.
        """
        s = "Conformal map defined by supplied data with interior point "
        s += str(self._interior_point) + "."
        return s
        
    def write_to_file(self, filename, data, interior_point=False):
        """
        Write numpy data attached to the map to file as a list of real and
        imaginary tuples. Allows for the inclusion of the interior point
        at the end if desired.

        INPUT:

            - ''filename''        - The name of the file to be written to.
                                     Extension must be supplied. If file already
                                     exists it is written over.

            - ''data''            - The data attached to self to be written to
                                     file.

            - ''interior_point''  - Boolean flag, default False. If set to True,
                                     a blank line followed by the interior point
                                     of self is added to the end of the written
                                     data file.

        OUTPUT:

        None.

        EXAMPLES::

            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: C = Conformal(X,0)
            sage: C.write_to_file('test.out',C.polygon())
            sage: D = np.loadtxt('test.out')
            sage: D = D[:,0] + 1j*D[:,1]
            sage: False in C.polygon() == D
            False
            sage: C.write_to_file('test.out',C.polygon(),interior_point=True)
            sage: D = np.loadtxt('test.out')
            sage: D = D[:,0] + 1j*D[:,1]
            sage: D[-1]
            0j
        """
        X = np.array([data.real,data.imag]).T
        np.savetxt(filename, X, delimiter='\t')
        
        if interior_point:
            f = open(filename, 'a')
            f.write('\n')
            f.write(str(self._interior_point.real) + '\t' + str(self._interior_point.imag))
            f.close()

    def set_normalization(self, state):
        """
        Turn the conformal mapnormalization on or off. Normalization is
        stored as a complex double of magnitude 1, by which data sent into
        the forward map is premultiplied.

        INPUT:

            - ''state''    - Boolean flag. If True, self._normalization is
                              set so that the derivative at the origin is
                              positive real. If False, self._normalization
                              is set so that 1+0j is sent to the first
                              input data point under the forward map.

        OUTPUT:

        None.

        EXAMPLES::
        
        """

        if state:
            # Get derivative of unnormalized conformal map at origin
            a1 = self._get_forward_map_derivative_at_origin()
            # Set normalization to the conjugate of a1/|a1| 
            self._normalization = np.conj(a1)/np.abs(a1)
        else:
            # Get the preimage of the first point of self._polygon
            b = self._polygon_preimage_raw[0]
            # Set normalization to the conjugate of b
            self._normalization = np.conj(b)

#    def _get_boundary_distance(self):
#        return np.min(np.abs(self.polygon(refined=True) - self.interior_point()))        
    
    def _get_forward_map_derivative_at_origin(self):
        """
        Helper function, used by self.set_normalization(), calculating the
        derivative of the unnormalized forward map at the origin.

        OUTPUT:

        A numpy.complex128 value.

        EXAMPLES::

        """
#        boundary_distance = self._get_boundary_distance()
        X = np.array([1.,1.*1j,-1.,-1.*1j])/1000
        # a1 approximates the derivative of the forward map at the origin
        a1 = np.sum(self._forward_map_unnormalized(X)/X)/4
        return a1
    
    def _forward_map_unnormalized(self, data): #, exterior_map=False):
        """
        The forward conformal map, mapping the unit disk to the input data-defined
        region, without any normalization.

        OUTPUT:

        A 1d complex Numpy array of points.

        EXAMPLES::

        """
        
        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute image of first 120000 points.")
            data = data[:120000]
        
#        if exterior_map:
#            Z = zipper_routines.forward(1./data,self._map_parameters[1][0],self._map_parameters[1][1])
#        else:
        Z = zipper_routines.forward(data,self._map_parameters[0],self._map_parameters[1])
        return Z

    def _inverse_map_unnormalized(self, data): #, exterior_map=False):
        """
        The inverse conformal map, mapping the input data_defined region
        to the unit disk, without any normalization.

        OUTPUT:

        A 1d complex Numpy array of points.
        
        EXAMPLES::

        """

        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute preimage of first 120000 points.")
            data = data[:120000]

#        if exterior_map:
#            Z = 1./zipper_routines.inverse(data,self._map_parameters[1][0],self._map_parameters[1][1])
#        else:
        Z = zipper_routines.inverse(data,self._map_parameters[0],self._map_parameters[1])

        return Z

    def forward_map(self, data): #, exterior_map=False):
        """
        The forward conformal map, mapping the unit disk to the input
        data-defined region.

        A 1d complex Numpy array of points.

        EXAMPLES::

        """
        X = data*self._normalization
        return self._forward_map_unnormalized(X) #,exterior_map=exterior_map)

    def inverse_map(self, data): #, exterior_map=False):
        """
        The inverse conformal map, mapping the input data-defined region
        to the unit disk.

        OUTPUT:

        A 1d complex Numpy array of points.

        EXAMPLES::
        """
        X = self._inverse_map_unnormalized(data) #,exterior_map=exterior_map)
        return X*np.conj(self._normalization)
        
    def interior_point(self):
        """
        Returns the point to which the origin is sent under the
        forward map. The is the point supplied by the user when
        the Conformal class object is constructed.

        OUTPUT:

        A numpy.complex128 value.

        EXAMPLES::

            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: C = Conformal(X,0)
            sage: C.interior_point()
            0j
        """
        return self._interior_point
        
    def polygon(self,refined=False):
        """
        Returns the data defining the region to which the unit disk
        is mapped.

        INPUT:

        - ''refined'' - Boolean flag, default is False. If False, returns
                         the input Numpy array defining the region
                         supplied by the user;
                        if True, returns a Numpy array of the polygon
                         with interpolating points as added by the
                         preprocessor.

        OUTPUT:

        A 1d complex Numpy array of points.

        EXAMPLES::

            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: C = Conformal(X,0)
            sage: C.polygon()
            array([ 1.+0.j,  0.+1.j, -1.+0.j, -0.-1.j])
        """
        if refined and self._preprocessor:
            return self._polygon
        else:
            return self._polygon_raw
        
    def polygon_preimage(self,refined=False):
        """
        Returns the preimage of the (preprocessed) polygon data as
        computed by the inverse map. These points all lie on the unit
        disk.

        INPUT:

        - ''refined'' - Boolean flag, default is False. If False, returns
                         the the computed preimage of the input data;
                        if True, returns the computed preimage of the
                         input data along with the added intorpolating
                         points

        OUTPUT:

        A 1d complex Numpy array of points.

        EXAMPLES::

            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: C = Conformal(X,interior_point=0)
            sage: D = C.polygon_preimage()
            sage: D.shape
            (202,)
            sage: np.max(np.abs(D))
            1.0000000000000002
            sage: np.min(np.abs(D))
            0.99999999999999489
        """
        if refined and self._preprocessor:
            return self._polygon_preimage
        else:
            return self._polygon_preimage_raw

        
    def grid(self, point_density=50):
        """
        Returns a 1d complex Numpy array of points evenly spaced in the
        interior or exterior of the polygon.

        INPUTS:

        - ''point_density'' - Positive integer. Point spacing will be 
                               D/point_density, where D is the max 
                               height/width of the polygon.

        OUTPUT:

        A 1d complex Numpy array of points.

        EXAMPLES::

            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: C = Conformal(X,interior_point=0)
            sage: D = C.grid()
            sage: D.shape
            (1200,)
            sage: np.max(np.abs(C.inverse_map(D)))
            0.99786414806575285
            sage: D = C.grid(point_density=200)
            sage: D.shape
            (19800,)
            sage: D = C.grid(point_density=-50)
            Traceback (most recent call last):
            ...
            ValueError: point_density must be an integer >= 1.
        """

        if point_density < 1:
            raise ValueError("point_density must be an integer >= 1.")

        Z = zipper_routines.imagegr(self._polygon,self._interior_point,point_density)
        return np.trim_zeros(Z)

    def carleson_grid(self, generations=6, sublevels=1): #, exterior_map=False):
        """
        Returns a 1d complex Numpy array of points defining a Carleson
        grid in the interior of the unit disk.

        INPUTS:

        - ''generations'' - Positive integer >= 2, default is 6. The number
                             of generations in the Carleson grid.
                            WARNING: The number of points in the returned
                             array is exponential in the number of generations,
                             so large values will result in large arrays.

        - ''sublevels''   - Positive integer, default is 1. The number of
                             of times each generation is repeated in the
                             Carleson grid.

        OUTPUT:

        A 1d complex Numpy array of points.

        EXAMPLES:: 

            sage: X = np.array([1.,1.*1j,-1.,-1.*1j])
            sage: C = Conformal(X,interior_point=0)
            sage: D = C.carleson_grid()
            sage: D.shape
            (4518,)
            sage: np.max(np.abs(D))
            0.99999999900000014
            sage: D = C.carleson_grid(generations=8,sublevels=2)
            sage: D.shape
            (73360,)
            sage: D = C.carleson_grid(generations=0)
            Traceback (most recent call last):
            ...
            ValueError: Number of generations must be an integer >= 2.
            sage: D = C.carleson_grid(sublevels=-1)
            Traceback (most recent call last):
            ...
            ValueError: Number of sublevels must be an integer >= 1.
        """
        
        if generations < 2:
            raise ValueError("Number of generations must be an integer >= 2.")
        if sublevels < 1:
            raise ValueError("Number of sublevels must be an integer >= 1.")

        Z = np.trim_zeros(zipper_routines.cgrids(generations,sublevels))
        Z = zipper_routines.corders(Z,generations,sublevels)
        
        return np.trim_zeros(Z)

# Not implemented yet
    def welding(self):
        return NotImplementedError("Weldings not yet implemented.")

    
###############################################################################

class ConformalChain:
    """
    Class representing a chain of Conformal maps that map the exterior
     of n simply connected bounded regions in the complex plane to the
     exterior of n circles. 
    """

    def _roundness_(self,A):
        """
        Helper function measuring how circular a bounded simply connected
         data-defined region is. This is given by rmax/rmin, where rmax
         is the maximum Euclidean distance between the region's vertices
         and the supplied interion point, and rmin the minumum such
         distance.

        INPUTS:

        - ''A'' - 1-D compled Numpy array defining the region to be
                   tested; the interior point must be the last element of
                   the array.

        OUTPUT:

        A nonnegative Numpy float.

        EXAMPLES::

        """

        c1 = np.min(np.abs(A[:-1]-A[-1]))
        c2 = np.max(np.abs(A[:-1]-A[-1]))
#        print(c1,c2)
        return c2/c1


    def __init__(self, data, preprocessor=True, N=200, tolerance=10e-2,\
                 map_limit=20):
        """
        Initialises the ConformalChain class. Data is stored as a chain of
         Conformal class object that iteratively map the exterior of one
         of the supplied regions to the interior of the unit disk.

        INPUTS:

        - ''data'' - A nonempty list of 1-D complex numpy array defining
                      the regions whose exterior is to be mapped. The last
                      point of each array must be a point on the interior
                      of said region, and the other points of that array
                      must wrap clockwise around that point.

        - ''preprocessor'' - Boolean flag, default on. Reorders the arrays
                              for increased numerical stability, and adds
                              interpolating points for increased precision.

        - ''N''            - Positive integer, default 200. The preprocessor
                              will attempt to add points up to this count
                              if there are less than that in a region.

        - ''tolerance''    - Positive real, default 0.01. The tolerance bound
                              for which, once all transformed regions are
                              this circular or more, iteration of conformal
                              maps halts

        - ''map_limit''    - Positive integer, default 20. The maximum number
                              of iterations of conformal maps.

        EXAMPLES:: 

        """
        
        # Set infinity to be a very larger number
        self._infinity = np.complex(2**300,0)

        # Load data
        self._input_data_raw = []
        for d in data:
            self._input_data_raw.append(load_data(d))

        # If preprocessor is invoked, format data using
        #  zipper_routines.polygon()
        if preprocessor:
            self._preprocessor = True

            self._input_data = []
            self._input_data_raw_indices = []        
            for d in self._input_data_raw:
                B,pindex,njs = zipper_routines.polygon(d[:-1],\
                                                       self._infinity,\
                                                       N)
                self._input_data.append(np.append(B[:njs],d[-1]))
                self._input_data_raw_indices.append(pindex-1)
        else:
            self._preprocessor = False

        # Store number of regions, initialize other parameters
        self._num_regions = len(self._input_data_raw)
        self._conformal_maps = []
        self._normalizations = []
        self._map_order = []

        # Decide which map to compute first
        R = [self._roundness_(d) for d in self.input_data(refined=True)]
        m = max(R)
        j = R.index(m)

        # Obtain copy of input data to be iteratively transformed
        if self._preprocessor:
            self._input_data_preimage = deepcopy(self._input_data)
        else:
            self._input_data_preimage = deepcopy(self._input_data_raw)

        # Iterate conformal maps until all transformed regions are
        #  sufficiently circular
        while m-1>=np.float64(tolerance) \
              and len(self._map_order)<=map_limit:
            
            # Compute conformal map mapping from exterior of region j
            #  to interior of disk
            C = Conformal(self._input_data_preimage[j][:-1],\
                          interior_point=np.complex(2**300,0),\
                          preprocessor=False,normalization=False)
            self._conformal_maps.append(C)

            # Compute normalization so that conformal map will map the
            #  exterior of the region to the exterior of disk, and the 
            #  map will look like the identity at infinity
            S0 = np.array([1e6,-1e6,1j*1e6,-1j*1e6],dtype=np.complex128)
            S1 = 1/C.inverse_map(S0)
            c0 = np.sum(S1)/4
            c1 = np.sum(S1/S0)/4
            self._normalizations.append((c0,c1))

            # Transform all boundary data according to this map
            for k in range(self._num_regions):
                if k==j:
                    d = np.append(1/C.polygon_preimage(),np.complex(0,0))
                else:
                    d = 1/C.inverse_map(self._input_data_preimage[k])
                d = (d - c0)/c1
                self._input_data_preimage[k] = d

            # Store which region's exterior was being mapped
            self._map_order.append(j)

            # Compute next region's whose exterior is to be mapped
            R = [self._roundness_(d) for d in self.input_data_preimage(refined=True)]
            m = max(R)
            j = R.index(m)

        # After all is done, store total number of maps
        self._num_maps = len(self._conformal_maps)


    def inverse_map(self,data):
        """
        Map points from the exterior of the defining regions to the exterior
         of the computed circles.

        INPUT:

         - ''data'' - 1-D complex Numpy array of data to be mapped.

        OUTPUT:

         A 1-D complex Numpy array of data.
         
        EXAMPLES::

        """
        
        # Iteratively map data using the normalized inverse maps of the
        #  computed Conformal objects
        d = data
        for j in range(self._num_maps):
            d = self._conformal_maps[j].inverse_map(d)
            d = (1/d - self._normalizations[j][0])/self._normalizations[j][1]
        
        return d

    def forward_map(self,data):
        """
        Map Points from the exterior of the computed circles to the exterior
         of the defining regions.

        INPUT:

         - ''data'' - 1-D complex Numpy array of data to be mapped.

        OUTPUT:

         A 1-D complex Numpy array of data.
         
        EXAMPLES::

        """
        # Iteratively map data using the normalized forward maps of the
        #  computed Conformal objects
        d = data
        for j in range(self._num_maps)[::-1]:
            d = 1/(d*self._normalizations[j][1] + self._normalizations[j][0])
            d = self._conformal_maps[j].forward_map(d)

        return d

    def input_data(self,refined=False):
        """
        Returns the boundary points of the defining regions.

        INPUT:

         - ''refined'' - Boolean flag, default False. If False, returns the
                          original unpreprocessed input data, including the
                          interior point at the end of each array.
                         If True, returns the preprocessed boundary data.

        OUTPUT:
        
         A list of 1-D complex Numpy arrays.

        EXAMPLES::

        """        
        if refined and self._preprocessor:
            return self._input_data
        else:
            return self._input_data_raw

    def input_data_preimage(self,refined=False):
        """
        Returns the preimages of the regions' boundary points.

        INPUT:

         - ''refined'' - Boolean flag, default False. If False, returns the
                          preimages of the unpreprocessed boundary data,
                          including the interior point at the end of each
                          array.
                         If True, returns the preimages of the preprocessed
                          boundary data.

        OUTPUT:
        
         A list of 1-D complex Numpy arrays.

        EXAMPLES::

        """
        if refined or not self._preprocessor:
            return self._input_data_preimage
        else:
            L = [0]*self._num_regions
            for j in range(self._num_regions):
                P = self._input_data_preimage[j]
                I = self._input_data_raw_indices[j]
                L[j] = np.append(P[I],P[-1])
            return L

    def num_maps(self):
        """
        Returns the number of individual maps computed in the
         creation of the ConformalChain object.

        OUTPUT:

         Positive integer.

        EXAMPLES::

        """
        return self._num_maps

    def num_regions(self):
        """
        Returns the number of regions whose exterior map the
         ConformalChain object computes.

        OUTPUT:

         Positive integer.

        EXAMPLES::

        """
        return self._num_regions

    def map_order(self):
        """
        Returns a list of indices indicating which conformal
         maps are computed at each step.

        OUTPUT:

         List of integers indexing the supplied regions.

        EXAMPLES::

        """
        return self._map_order

    def normalizations(self):
        """
        Returns the normalizations of each conformal map.

        OUTPUT:

         List of pairs of Numpy complex points parameterizing
          each normalization.

        EXAMPLES::

        """
        return self._normalizations

    def conformal_maps(self):
        """
        Returns the Conformal class objects used to compute each
         conformal map.

        OUTPUT:

         List of Conformal class objects.

        EXAMPLES::

        """
        return self._conformal_maps

