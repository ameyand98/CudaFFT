seq_poly:
	g++ logger.cc SequentialFFT.cc parser.cc SeqPolyFFT.cc -o seq.o 
	
seq_img:
	g++ logger.cc SequentialFFT.cc parser.cc SeqImgFFT.cc -o seq.o -I/usr/local/include/opencv4 -lopencv_imgcodecs -lopencv_core

run_seq_poly:
	./seq.o -o outs.txt -n 8192 -r 1 -d 1 -w 1 -x 1

par_poly:
	nvcc CudaPolyFFT.cc CudaFFT.cu logger.cc parser.cc -o par.o -std=c++11
	# sudo docker run --rm -it -v $$PWD/..:$$PWD/.. -u`id -u`:`id -g` --gpus all -w $$PWD/.. nvcr.io/nvidia/cuda:10.2-devel-ubuntu18.04

run_par_poly:
	./par.o -o outp.txt -n 15 -t 128 -r 1 -d 1 -w 1 -x 0

clean:
	rm -rf *.o *.txt *_comp.jpg