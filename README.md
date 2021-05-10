# CudaFFT
Parallel Fast Fourier Transform implementation using CUDA and OpenCV

# Arguments 
-i - input file 
-o - output file
-n - number of input points in the dft
-t - number of threads for parallel fft
-d - input dimensions of fft calculation 
-w - output the data in the outfile (1 for output)
-x - type of input (1 for integer, 0 for float)
-r - number of times to run the algorithm
-p - input image for image compression

# Compilation
make seq_poly - compile sequential polynomial multiplication
make par_poly - compile CUDA polynomial multiplication
make par_img - compile CUDA image compression
make run_* - run the above mentioned compiled code
make clean 

Sequential - cd src && ./seq.o --arguments
Parallel - cd src && ./par.o --arguments
