#include "FFT.h"
#include "logger.h"
#include "parser.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <vector>


int main(int argc, char** argv) {

    srand(time(NULL));
    Parser* ip = new Parser();
    ip->parse(argc, argv);

    int num_threads = ip->threads;
    int data_size = ip->length;
    ofstream out_file(ip->outfile);
    ifstream in_file(ip->infile);
    int runs = ip->runs;
    int dim = ip->dim;
    int out_data = ip->out_data;
    int integer = ip->integer;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < runs; i++) {
        if (integer) {
            std::vector<int> data1(ip->length);
            generate(data1.begin(), data1.end(), []{
                return rand() % 1000;
            });
            std::vector<int> data2(ip->length);
            generate(data2.begin(), data2.end(), []{
                return rand() % 1000;
            });

            vector<int> result = CUDA::multiply_poly(data1, data2, 4, num_threads);
            if (out_data) {
                for (int i = 0; i < result.size(); i++) {
                    out_file << result[i];
                    out_file << "\n";
                }
            }

            auto stop = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
            cout << "Cuda end to end Polynomial multiplication time (micros) - " << duration.count() << endl;
        } else {
            std::vector<double> data1(ip->length);
            generate(data1.begin(), data1.end(), []{
                return rand() % 1000;
            });
            std::vector<double> data2(ip->length);
            generate(data2.begin(), data2.end(), []{
                return rand() % 1000;
            });

            vector<double> result = CUDA::multiply_poly(data1, data2, 4, num_threads);
            if (out_data) {
                for (int i = 0; i < result.size(); i++) {
                    out_file << result[i];
                    out_file << "\n";
                }
            }

            auto stop = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
            cout << "Cuda end to end Polynomial multiplication time (micros) - " << duration.count() << endl;

        }

    }
    

}


