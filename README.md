# CudaFFT
Parallel Fast Fourier Transform implementation using CUDA and OpenCV

# Arguments 
-i - input file <br/>
-o - output file <br/>
-n - number of input points in the dft <br/>
-t - number of threads for parallel fft <br/>
-d - input dimensions of fft calculation <br/>
-w - output the data in the outfile (1 for output) <br/>
-x - type of input (1 for integer, 0 for float) <br/>
-r - number of times to run the algorithm <br/>
-p - input image for image compression <br/>

# Compilation
make seq_poly - compile sequential polynomial multiplication <br/>
make par_poly - compile CUDA polynomial multiplication <br/>
make par_img - compile CUDA image compression <br/>
make run_* - run the above mentioned compiled code <br/>
make clean <br/>

Sequential - cd src && ./seq.o --arguments <br/>
Parallel - cd src && ./par.o --arguments <br/>
