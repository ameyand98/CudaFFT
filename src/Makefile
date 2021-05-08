seq:
	g++ logger.cc SequentialFFT.cc parser.cc SeqPolyFFT.cc -o seq.o
	# g++ -o seq.o logger.cc SequentialFFT.cc parser.cc -I/usr/local/include/opencv4 -lopencv_imgcodecs -lopencv_core
	# sudo docker run --rm -it -v $$PWD/..:$$PWD/.. -u`id -u`:`id -g` --gpus all -w $$PWD/.. nvcr.io/nvidia/cuda:10.2-devel-ubuntu18.04 

clean:
	rm -rf *.o *.txt

run_seq:
	@./seq.o -o outs.txt -n 8 -r 10 -d 1 -w 1 -x 0

par:
	@nvcc -o par.o CudaFFT.cu logger.cc parser.cc -std=c++11

run_par:
	@./par.o -o outp.txt -n 100000 -t 128