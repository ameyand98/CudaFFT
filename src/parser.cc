#include "parser.h"
#include <bits/getopt_core.h>
#include <string>
#include <iostream>

void print_usage(int argc, char ** argv){
    std::cout << "usage: fft";
    std::cout << "\t -i \t \t input file " << std::endl;
    std::cout << "\t -o \t \t output file" << std::endl;
    std::cout << "\t -n \t \t number of input points in the dft" << std::endl;
    std::cout << "\t -t \t \t number of threads for parallel fft" << std::endl;
    std::cout << "\t -d \t \t input dimensions of fft calculation " << std::endl;
    std::cout << "\t -w \t \t output the data in the outfile" << std::endl;
    std::cout << "\t -x \t \t type of input (1 for integer, 0 for float)" << std::endl;
    std::cout << "\t -r \t \t number of times to run the algorithm" << std::endl;
}

bool Parser::parse(int argc, char **argv) {
    int input;

    while ((input = getopt(argc, argv, "i:o:n:r:t:d:w:x:h:")) != -1) {
        switch (input) {
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'n':
                length = stoi(optarg);
                break;
            case 'r':
                runs = stoi(optarg);
                break;
            case 't':
                threads = stoi(optarg);
                break;
            case 'd':
                dim = stoi(optarg);
                break;
            case 'w':
                out_data = stoi(optarg);
                break;
            case 'x':
                integer = stoi(optarg);
                break;
        }
    }
    return true;
}