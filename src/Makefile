seq:
	g++ -o seq.o logger.cc SequentialFFT.cc 
	# sudo docker run --rm -it -v $$PWD/..:$$PWD/.. -u`id -u`:`id -g` --gpus all -w $$PWD/.. nvcr.io/nvidia/cuda:10.2-devel-ubuntu18.04 

clean:
	rm -rf *.o *.txt

run_seq:
	./seq.o

par:
	nvcc -o par.o CudaFFT.cu logger.cc -std=c++11

run_par:
	./par.o