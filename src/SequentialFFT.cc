#include "include/SequentialFFT.h"
#include "include/logger.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

const double ANGLE_MULT = 2 * M_PI;

namespace Sequential {

    void bit_reverse(int size, vector<cmplx>& data) {
        for (int i = 1, j = 0; i < size; i++) {
            int bit = size >> 1;
            for (; j >= bit; bit >>= 1) {
                j -= bit;
            }
            if (i < j) {
                swap(data[i], data[j]);
            }
        }
    }

    void fft(vector<cmplx>& data, bool invert) {
        int size = data.size();

        // do bit reversal to group even and odd indices in data
        Logger::log_info("Reversing bits", false);
        bit_reverse(size, data);

        // Fft algorithm
        // bottom up algorithm that starts working on arr of len 2, 4, 8, ...
        Logger::log_info("Starting 1D FFT", false);
        for (int curr_length = 2; curr_length <= size; curr_length <<= 1) {
            // e^angle / curr_length
            double angle = ANGLE_MULT / curr_length * (invert ? 1 : -1);
            cmplx wlen(cos(angle), sin(angle));

            for (int i = 0; i < size; i += curr_length) {
                cmplx w(1);
                // compute for even and odd halfs simutaneously
                for (int j = 0; j < curr_length / 2; j++) {
                    data[i + j] = (data[i + j]) + (w * data[i + j + curr_length / 2]);
                    data[i + j + curr_length / 2] = (data[i + j]) - (w * data[i + j + curr_length / 2]);
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

    }

    void fft_2D(vector<vector<cmplx>>& data, bool invert) {
        auto matrix = data;

        // first transform rows
        Logger::log_info("Transforming rows for 2D FFT", false);
        for (int i = 0; i < matrix.size(); i++) {
            fft(matrix[i], invert);
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
            fft(matrix[i], invert);
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
}

int main() {
    Mat img_matrix;
    img_matrix = cv::imread("img/flower.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    Logger::log_info("Image read", false);
    cv::imwrite("orig.jpg", img_matrix);
    vector<vector<uint8_t>> image(img_matrix.rows, vector<uint8_t>(img_matrix.cols));
    for (int i = 0; i < img_matrix.rows; i++) {
        for (int j = 0; j < img_matrix.cols; j++) {
            image[i][j] = uint8_t(img_matrix.at<uint8_t>(i, j));
        }
    }

}


