import numpy as np
import zipper_routines
import pylab

class Conformal:      
    def __init__(self, infile="/Users/simonspicer/zipper/src/zipper/samples/cat.txt", \
                 interior_point=None, N=200,exterior_map=False):

        A      = np.loadtxt(infile)
        B      = A[:,0] +1j*A[:,1]

        self._interior_point  = B[-1]
        if interior_point is not None:
#            if interior_point.parent() == Infinity.parent():
#                self._interior_point = np.complex(2147483647, 2147483647)
#            else:
            self._interior_point  = np.complex(np.real(interior_point),np.imag(interior_point))

        self._polygon_raw = B[:-1]
        B      = zipper_routines.polygon(B[:-1], self._interior_point, N)
        self._polygon  = np.trim_zeros(B)

        params,ABC,preimage = zipper_routines.zipper(self._polygon,self._interior_point)
        self._map_parameters = [params,ABC]
        self._polygon_preimage = preimage
               
        self._grid = None
        self._grid_preimage = None
        self._carleson_grid = None
        self._carleson_grid_image = None
                
    def __repr__(self):
        return "Conformal map."
        
    def write_to_file(self, filename, data, interior_point=False):
        
        np.savetxt(filename, data, delimiter='\t')
        
        if interior_point:
            f = open(filename, 'a')
            f.write('\n')
            f.write(str(self._interior_point[0]) + '\t' + str(self._interior_point[1]))
            f.close()        
        
    def forward_map(self, data, phase=False):
        
        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute image of first 120000 points.")
            data = data[:120000]

        Z = zipper_routines.forward(data,self._map_parameters[0],self._map_parameters[1])

        if phase:
            angles = np.angle(data)
            return Z,angles
        else:
            return Z

    def inverse_map(self, data, phase=False):

        if len(data)>120000:
            print("Exceeded 120000 pts. Split file or recompile.")
            print("Will compute preimage of first 120000 points.")
            data = data[:120000]

        Z = zipper_routines.inverse(data,self._map_parameters[0],self._map_parameters[1])

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
        
    def grid(self, point_density=50):
        Z = zipper_routines.imagegr(self._polygon,self._interior_point,point_density)
        return np.trim_zeros(Z)

    def carleson_grid(self, generations=6, sublevels=1):
        Z = np.trim_zeros(zipper_routines.cgrids(generations,sublevels))
        Z = zipper_routines.corders(Z,generations,sublevels)
        return np.trim_zeros(Z)
