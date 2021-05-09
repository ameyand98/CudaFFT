#include "FFT.h"
#include "logger.h"
#include "parser.h"
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <cstdlib>
#include <fstream>
#include <math.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <vector>

using namespace cv;


int main(int argc, char** argv) {
    Mat image_M;
    image_M = imread("scene.jpg", IMREAD_GRAYSCALE);
    if (!image_M.data) {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }


    imwrite("original.jpg", image_M);
    vector<vector<uint8_t>> image(image_M.rows, vector<uint8_t>(image_M.cols));
    for (int i = 0; i < image_M.rows; ++i)
        for (int j = 0; j < image_M.cols; ++j)
            image[i][j] = uint8_t(image_M.at<uint8_t>(i, j));
        
    // cout << image.size() << " " << image[0].size() << endl;
    Sequential::compress_img(image, 0.010000);

    cout << image_M.rows << " " << image_M.cols << endl;

    // for (int i = 0; i < image_M.rows; ++i)
    //         for (int j = 0; j < image_M.cols; ++j)
    //             image_M.at<uint8_t>(i, j) = image[i][j];
    
    // imwrite("one_comp.jpg", image_M);
}