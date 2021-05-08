#!/bin/sh
# make par 
cd src

echo "hello" >> runtimes.txt
for i in 1..10
do 
    ./par.o -n 200000 -t 2048 >> runtimes.txt
done