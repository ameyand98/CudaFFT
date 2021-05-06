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

namespace Sequential {

    void bit_reverse(int size, vector<cmplx>& rev, vector<cmplx>& orig) {
        for (int i = 1, j = 0; i < size; ++i) {
            int bit = size >> 1;
            for (; j >= bit; bit >>= 1) {
                // cout << "shifting" << endl;
                j -= bit;
            }
            j += bit;
            if (i < j) {
                rev[i] = orig[j];
                rev[j] = orig[i];
                // swap(data[i], data[j]);
            }
        }
    }

    vector<cmplx> fft(vector<cmplx>& data_arr, bool invert) {
        int size = data_arr.size();

        // do bit reversal to group even and odd indices in data
        // Logger::log_info("Reversing bits", false);
        vector<cmplx> data(size);
        bit_reverse(size, data, data_arr);


        // Fft algorithm
        // bottom up algorithm that starts working on arr of len 2, 4, 8, ...
        Logger::log_info("Starting 1D FFT", false);
        for (int curr_length = 2; curr_length <= size; curr_length <<= 1) {
            // e^angle / curr_length
            double angle = ANGLE_MULT / (curr_length * (invert ? 1 : -1));
            cmplx wlen(cos(angle), sin(angle));

            for (int i = 0; i < size; i += curr_length) {
                cmplx w(1);
                // compute for even and odd halfs simutaneously
                for (int j = 0; j < curr_length / 2; j++) {
                    cmplx u = data[i + j];
                    cmplx v = data[i + j + curr_length / 2] * w;
                    data[i + j] = u + v;
                    data[i + j + curr_length / 2] = u - v;
                    w *= wlen;
                }
            }

        }

        // divide by the length of the array for inverse fft
        if (invert) {
            for (int i = 0; i < size; i++) {
                data[i] /= size;
            }
        }

        return data;

    }

    void fft_2D(vector<vector<cmplx>>& data, bool invert) {
        auto matrix = data;

        // first transform rows
        Logger::log_info("Transforming rows for 2D FFT", false);
        for (int i = 0; i < matrix.size(); i++) {
            cout << matrix.size() << endl;
            matrix[i] = fft(matrix[i], invert);
        }

        // now transform columns, by transposing the matrix
        Logger::log_info("Transposing the 2D matrix", false);
        data = matrix;
        matrix.resize(data[0].size());
        for (int i = 0; matrix.size(); i++) {
            matrix[i].resize(data.size());
        }

        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data[0].size(); j++) {
                matrix[j][i] = data[i][j];
            }
        }

        Logger::log_info("Transforming cols for 2D FFT", false);
        for (int i = 0; i < matrix.size(); i++) {
            matrix[i] = fft(matrix[i], invert);
        }

        // now we can restore the result by transposing again
        Logger::log_info("Restoring by re-transposing", false);
        for (int i = 0; i < data.size(); i++) {
            for (int j = 0; j < data[0].size(); j++) {
                data[i][j] = matrix[j][i];
            }
        }
    }

    void compress_img(vector<vector<uint8_t>>& image, double threshold) {
        Logger::log_info("Converting img into complex type", false);
        vector<vector<cmplx>> cmplx_img(image.size(), vector<cmplx>(image[0].size()));

        int rows = image.size();
        int cols = image[0].size();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                cmplx_img[i][j] = image[i][j];
            }
        }
        Logger::log_info("Complex img created", false);

        Logger::log_info("Performing FFT on image", false);
        fft_2D(cmplx_img, false);

        double max_val = 0; 
        for (int i = 0; i < cmplx_img.size(); i++) {
            for (int j = 0; j < cmplx_img.size(); j++) {
                max_val = max(max_val, (double) abs(cmplx_img[i][j]));
            }
        }
        Logger::log_info("Setting threshold for compression", false);
        threshold *= max_val;

        Logger::log_info("Clearing out values below threshold", false);
        Logger::log_info("Compressing the image", false);
        for (int i = 0; i < cmplx_img.size(); i++) {
            for (int j = 0; j < cmplx_img[0].size(); j++) {
                if (abs(cmplx_img[i][j]) < threshold) {
                    cmplx_img[i][j] = 0;
                }
            }
        }        

        Logger::log_info("Performing inverse fft to invert image", false);
        fft_2D(cmplx_img, true);

        Logger::log_info("Converting back to real img", false);
        for (int i = 0; i < cmplx_img.size(); i++) {
            for (int j = 0; j < cmplx_img[0].size(); j++) {
                // TODO need to justify the 0.5 here 
                image[i][j] = uint8_t(cmplx_img[i][j].real() + 0.5);
            }
        }

        Logger::log_info("Image compressed", false);

    }

    vector<int> multiply_poly(vector<int> poly1, vector<int> poly2) {
        vector<cmplx> f_cmplx(poly1.begin(), poly1.end()), s_cmplx(poly2.begin(), poly2.end());

        int size = 1;
        while (size < max(poly1.size(), poly2.size())) {
            size <<= 1;
        }
        size <<= 1;

        f_cmplx.resize(size), s_cmplx.resize(size);

        fft(f_cmplx, false);
        fft(s_cmplx, false);

        for (int i = 0; i < size; i++) {
            f_cmplx[i] *= s_cmplx[i];
        }

        fft(f_cmplx, true);

        vector<int> result;
        result.resize(size);

        for (int i = 0; i < size; i++) {
            result[i] = int(f_cmplx[i].real() + 0.5);
        }

        return result;
    }
}

int main(int argc, char** argv) {

    // Parser* ip = new Parser();
    // ip->parse(argc, argv);

    // ofstream out(ip->outfile);
    // std::cout.rdbuf(out.rdbuf());
    // vector<cmplx> result = {1, 2, 3, 4, 5, 6, 7, 8};


    // vector<cmplx> result(ip->length);
    // generate(result.begin(), result.end(), rand);

    // auto start = chrono::high_resolution_clock::now();
    // result = Sequential::fft(result, false);
    // auto stop = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    // cout << duration.count() << endl;

    // for (int i = 0; i < result.size(); i++) {
    //     cout << result[i] << " ";
    // }
    // cout << "\n";

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
        
    Sequential::compress_img(image, 0.000001);

    for (int i = 0; i < image_M.rows; ++i)
            for (int j = 0; j < image_M.cols; ++j)
                image_M.at<uint8_t>(i, j) = image[i][j];
    
    imwrite("compressed.jpg", image_M);

}


