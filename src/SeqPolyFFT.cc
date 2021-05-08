#include "FFT.h"
#include "logger.h"
#include "parser.h"
// #include <opencv4/opencv2/core/core.hpp>
// #include <opencv4/opencv2/highgui/highgui.hpp>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <vector>


int main(int argc, char** argv) {

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

    for (int i = 0; i < runs; i++) {
        if (integer) {
            std::vector<int> data1(ip->length);
            generate(data1.begin(), data1.end(), std::rand);
            std::vector<int> data2(ip->length);
            generate(data2.begin(), data2.end(), std::rand);

            vector<int> result = Sequential::multiply_poly(data1, data2);
            if (out_data) {
                for (int i = 0; i < result.size(); i++) {
                    out_file << result[i];
                    out_file << "\n";
                }
            }
        } else {
            std::vector<double> data1(ip->length);
            generate(data1.begin(), data1.end(), std::rand);
            std::vector<double> data2(ip->length);
            generate(data2.begin(), data2.end(), std::rand);

            vector<double> result = Sequential::multiply_poly(data1, data2);
            if (out_data) {
                for (int i = 0; i < result.size(); i++) {
                    out_file << result[i];
                    out_file << "\n";
                }
            }

        }



    }
    



    // Mat image_M;
    // image_M = imread("scene.jpg", IMREAD_GRAYSCALE);
    // if (!image_M.data) {
    //     cout << "Could not open or find the image" << std::endl;
    //     return -1;
    // }

    // imwrite("original.jpg", image_M);
    // vector<vector<uint8_t>> image(image_M.rows, vector<uint8_t>(image_M.cols));
    // for (int i = 0; i < image_M.rows; ++i)
    //     for (int j = 0; j < image_M.cols; ++j)
    //         image[i][j] = uint8_t(image_M.at<uint8_t>(i, j));
        
    // Sequential::compress_img(image, 0.000001);

    // for (int i = 0; i < image_M.rows; ++i)
    //         for (int j = 0; j < image_M.cols; ++j)
    //             image_M.at<uint8_t>(i, j) = image[i][j];
    
    // imwrite("compressed.jpg", image_M);

}


