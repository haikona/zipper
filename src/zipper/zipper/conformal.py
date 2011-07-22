import numpy as np
import zipper_routines
import pylab

class Conformal:      
    def __init__(self, infile="/Users/simonspicer/zipper/src/zipper/samples/cat.txt", interior_point=None, N=200):

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
        
    def forward_map(self, input_data, parm_1=None, parm_2=None, parm_3=None):
        
        self.write_to_file(self._outputdir+"input.tmp", input_data)
        
        f = open(self._cwdir + 'input/params.tmp', 'w')
        f.write(str(0) + '\n')
        f.write(self._outputdir + 'input.tmp' + '\n')
        f.write(self._outputdir + 'output.tmp' + '\n')
        f.write(str(0) + '\n')
        f.close()
        
        self._execute_zipper("forward","params.tmp")       
        output_data = np.loadtxt(self._outputdir+"output.tmp")
        
        return output_data
        
    def inverse_map(self, input_data):
        
        self.write_to_file(self._outputdir+"input.tmp", input_data)
        
        f = open(self._cwdir + 'input/params.tmp', 'w')
        f.write(self._outputdir + 'input.tmp' + '\n')
        f.write(self._outputdir + 'output.tmp' + '\n')
        f.close()
        
        self._execute_zipper("inverse","params.tmp")       
        output_data = np.loadtxt(self._outputdir+"output.tmp")
        
        return output_data
        
    def input_data(self):
        return self._input_data
        
    def interior_point(self):
        return self._interior_point
        
    def polygon(self):
        return self._polygon
        
    def polygon_preimage(self):
        if self._polygon_preimage is None:
            self._execute_zipper("origpre")
            self._polygon_preimage = np.loadtxt(self._outputdir+"orig.pre")
        return self._polygon_preimage
        
    def polygon_hd(self):
        if self._polygon_hd is None:
            self._polygon_hd = np.genfromtxt(self._outputdir+"poly.dat",skip_footer=1)
        return self._polygon_hd
        
    def polygon_hd_preimage(self):
        if self._polygon_hd_preimage is None:
            self._polygon_hd_preimage = np.loadtxt(self._outputdir+"poly.pre")
        return self._polygon_hd_preimage
        
    def grid(self, point_density=50):
        if self._grid is None or self._grid[1] != point_density:
            f = open(self._cwdir + 'input/params.tmp', 'w')
            f.write(str(point_density) + '\n')
            f.close()
            
            self._execute_zipper("imagegr","params.tmp")
            self._grid = [np.loadtxt(self._outputdir+"grid.dat"), point_density]
        return self._grid[0]
        
    def grid_preimage(self, point_density=50):
        if self._grid_preimage is None or self._grid_preimage[1] != point_density:
            self._grid_preimage = [self.inverse_map(self.grid(point_density)), point_density]
        return self._grid_preimage[0]
        
    def carleson_grid(self, generations=6, sublevels=1):
        if self._carleson_grid is not None \
            and self._carleson_grid[1] == generations \
            and self._carleson_grid[2] == sublevels: pass
        else:
            f = open(self._cwdir + 'input/params1.tmp', 'w')
            f.write(str(generations) + '\n')
            f.write(str(sublevels) + '\n')
            f.write(self._outputdir + 'cgrid.dat' + '\n')
            f.close()
            
            f = open(self._cwdir + 'input/params2.tmp', 'w')
            f.write(str(generations) + '\n')
            f.write(str(sublevels) + '\n')
            f.write(self._outputdir + 'cgrid.dat' + '\n')
            f.write(self._outputdir + 'poly.grid' + '\n')
            f.close()
            
            self._execute_zipper("cgrids","params1.tmp")
            self._execute_zipper("corders","params2.tmp") 
                    
            self._carleson_grid = [np.loadtxt(self._outputdir+"poly.grid"), generations, sublevels]
        return self._carleson_grid[0]
        
    def carleson_grid_image(self, generations=6, sublevels=1):
        if self._carleson_grid is not None \
            and self._carleson_grid[1] == generations \
            and self._carleson_grid[2] == sublevels: pass
        else:
            A = self.carleson_grid(generations, sublevels)
        
        if self._carleson_grid_image is not None \
            and self._carleson_grid_image[1] == generations \
            and self._carleson_grid_image[2] == sublevels: pass
        else:
            f = open(self._cwdir + 'input/params1.tmp', 'w')
            f.write(str(1) + '\n')
            f.write(str(4) + '\n')
            f.write(str(1) + '\n')
            f.write(self._outputdir + 'cgrid.dat' + '\n')
            f.write(self._outputdir + 'cgrid.img' + '\n')
            f.write(str(0) + '\n')
            f.close()
            
            f = open(self._cwdir + 'input/params2.tmp', 'w')
            f.write(str(generations) + '\n')
            f.write(str(sublevels) + '\n')
            f.write(self._outputdir + 'cgrid.img' + '\n')
            f.write(self._outputdir + 'poly.gridi' + '\n')
            f.close()
            
            self._execute_zipper("forward","params1.tmp")
            self._execute_zipper("corders","params2.tmp") 
                    
            self._carleson_grid_image = [np.loadtxt(self._outputdir+"poly.gridi"), generations, sublevels]
        return self._carleson_grid_image[0]
