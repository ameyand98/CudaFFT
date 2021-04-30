default:
	sudo docker run --rm -it -v $$PWD:$$PWD -u`id -u`:`id -g` --gpus all -w $$PWD nvcr.io/nvidia/cuda:10.2-devel-ubuntu18.04
	g++ -o seq.o logger.cc SequentialFFT.cc 

clean:
	rm -rf *.o

run:
	./seq.o