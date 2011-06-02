zipper: zipper.f forward.f inverse.f polygon.f cgrids.f corders.f\
imagegr.f polyline.f blankl.f block45.f extdist.f fluid.f stagpt.f\
whitney.f geodes.f
	f77  -O0 -o zipper zipper.f
	f77  -O0 -o forward forward.f
	f77  -O0 -o inverse inverse.f
	f77  -O0 -o polygon polygon.f
	f77  -O0 -o cgrids cgrids.f
	f77  -O0 -o corders corders.f
	f77  -O0 -o imagegr imagegr.f
	f77  -O0 -o polyline polyline.f
	f77  -O0 -o blankl blankl.f
	f77  -O0 -o block45 block45.f
	f77  -O0 -o extdist extdist.f
	f77  -O0 -o fluid fluid.f
	f77  -O0 -o stagpt stagpt.f
	f77  -O0 -o whitney whitney.f
	f77  -O0 -o geodes geodes.f
	f77  -O0 -o circle circle.f
	f77  -O0 -o origpre origpre.f
	chmod a+x ../demo*

clean:
	rm -f zipper forward inverse polygon cgrids corders polyline\
imagegr geodes blankl block45 extdist fluid stagpt whitney circle core\
*.dat *.img *.int *.pre *.grid* *.par *.o *.ps
