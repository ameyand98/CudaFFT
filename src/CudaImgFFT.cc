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
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

using namespace cv;


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
    string img = ip->image;

    Mat image_M;
    image_M = imread(img, IMREAD_GRAYSCALE);

    cv::imwrite("original.jpg", image_M);
    vector<vector<uint8_t>> image(image_M.rows, vector<uint8_t>(image_M.cols));
    for (int i = 0; i < image_M.rows; ++i)
        for (int j = 0; j < image_M.cols; ++j)
            image[i][j] = uint8_t(image_M.at<uint8_t>(i, j));
    
    CUDA::compress_image(image, 0.010000, 2, num_threads);

    for (int i = 0; i < image_M.rows; ++i)
            for (int j = 0; j < image_M.cols; ++j)
                image_M.at<uint8_t>(i, j) = image[i][j];
    
    cv::imwrite("img_comp.jpg", image_M);

}