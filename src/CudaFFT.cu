#include "FFT.h"
#include "logger.h"
#include "parser.h"
#include <cmath>
#include <crt/host_defines.h>
#include <cstdlib>
#include <cuda_device_runtime_api.h>
#include <cuda_runtime_api.h>
#include <driver_types.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>

using namespace std;

template <typename T>
ostream &operator<<(ostream &o, vector<T> v) {
    if (v.size() > 0) {
        o << v[0];
    }
    for (unsigned i = 1; i < v.size(); i++) {
        o << " " << v[i];
    }

    return o << endl;
}

static __device__ __host__ inline cmplx_struct add(cmplx_struct a, cmplx_struct b) {
    cmplx_struct num;
    num.x = a.x + b.x;
    num.y = a.y + b.y;
    return num;
}

static __device__ __host__ inline cmplx_struct inverse(cmplx_struct number) {
    cmplx_struct inverse;
    inverse.x = -number.x;
    inverse.y = -number.y;
    return inverse;
}

static __device__ __host__ inline cmplx_struct multiply(cmplx_struct first, cmplx_struct second) {
    cmplx_struct mult;
    mult.x = first.x * second.x - first.y * second.y;
    mult.y = first.y * second.x + first.x * second.y;
    return mult;
}

__global__ void inverse_divide(cmplx_struct* numbers, int n, int threads) {
    int index = blockIdx.x * threads + threadIdx.x;
    // bounds check
    if (index < n) {
        numbers[index].x /= n;
        numbers[index].y /= n;
    } 
}

// TODO pointer aliasing optimization must go in the report
// https://developer.nvidia.com/blog/cuda-pro-tip-optimize-pointer-aliasing/
__global__ void reorder_array(cmplx_struct* __restrict__ rev, cmplx_struct* __restrict__ orig, int s, int threads, int n) {
    unsigned int index = blockIdx.x * threads + threadIdx.x;
    if (index < n && (__brev(index) >> (32 - s)) < n) {
        // reversed
        rev[__brev(index) >> (32 - s)] = orig[index];
    }
}

__device__ void fft_inner_loop(cmplx_struct* __restrict__ numbers, int row, int col, int len, int n, bool invert) {
    if (row + col + len / 2 < n && col < len / 2) {
        cmplx_struct first, second;
        float angle = (ANGLE_MULT * col) / (len * (invert ? 1 : -1));
        second.x = cos(angle);
        second.y = sin(angle);

        first = numbers[row + col];
        second = multiply(numbers[row + col + len / 2], second);

        numbers[row + col] = add(first, second);
        numbers[row + col + len / 2] = add(first, inverse(second));
    }
}


__global__ void compute_fft(cmplx_struct* __restrict__ numbers, int row, int len, int n, int threads, bool invert) {
    int col = blockIdx.x * threads + threadIdx.x;
    fft_inner_loop(numbers, row, col, len, n, invert);
}

__global__ void fft_outer_loop(cmplx_struct* __restrict__ numbers, int len, int n, int threads, bool invert) {
    int row = (blockIdx.x * threads + threadIdx.x) * len;

    for (int col = 0; col < len / 2; col++) {
        fft_inner_loop(numbers, row, col, len, n, invert);
    }
}

void real_fft(int size, int threads, cmplx_struct* reversed_nums, cmplx_struct* nums, int balance, bool invert) {
    int power = log2(size);

    auto start = chrono::high_resolution_clock::now();

    reorder_array<<<ceil(float(size) / threads), threads>>>(reversed_nums, nums, power, threads, size);

    // no need to wait for all the bits to be reversed (implicit sync)
    // cudaDeviceSynchronize();

    // parallel fft
    for (int len = 2; len <= size; len <<= 1) {
        if (size / len > balance) {
            fft_outer_loop<<<ceil((float) size / threads / len), threads>>>(reversed_nums, len, size, threads, invert);
        } else {
            for (int row = 0; row < size; row += len) {
                float repeat = (float) len / 2;
                compute_fft<<<ceil(repeat / threads), threads>>>(reversed_nums, row, len, size, threads, invert);
            }
        }
    }

    if (invert) {
        inverse_divide<<<ceil((float) size / threads), threads>>>(reversed_nums, size, threads);
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << duration.count() << endl;

}


void fft(vector<cmplx>& array, bool invert, int balance, int threads) {
    int size = (int) array.size();
    cmplx_struct* data = (cmplx_struct*)malloc(sizeof(cmplx_struct) * size);
    for (int i = 0; i < size; i++) {
        data[i].x = array[i].real();
        data[i].y = array[i].imag();
    }

    cmplx_struct *reversed_nums, *nums;
    cudaMalloc((void **)&reversed_nums, sizeof(cmplx_struct) * size);
    cudaMalloc((void **)&nums, sizeof(cmplx_struct) * size);
    cudaMemcpy(nums, data, sizeof(cmplx_struct) * size, cudaMemcpyHostToDevice);

    real_fft(size, threads, reversed_nums, nums, balance, invert);

    cmplx_struct* results;
    results = (cmplx_struct*)malloc(sizeof(cmplx_struct) * size);
    cudaMemcpy(results, reversed_nums, sizeof(cmplx_struct) * size, cudaMemcpyDeviceToHost);

    for (int i = 0; i < size; i++) {
        array[i] = cmplx(results[i].x, results[i].y);
    }

    // cleanup
    free(data);
    cudaFree(reversed_nums);
    cudaFree(nums);
}

void fft_2D(vector<vector<cmplx> >& data, bool invert, int thread_balance, int threads) {
    vector<vector<cmplx> >& matrix = data;
    for (int i = 0; i < matrix.size(); i++) {
        fft(matrix[i], invert, thread_balance, threads);
    }

    data = matrix;
    matrix.resize(data[0].size());
    for (int i = 0; i < matrix.size(); i++) {
        matrix[i].resize(data.size());
    }

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[0].size(); j++) {
            matrix[j][i] = data[i][j];
        }
    }

    for (int i = 0; i < matrix.size(); i++) {
        fft(matrix[i], invert, thread_balance, threads);
    }

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < data[0].size(); j++) {
            data[j][i] = matrix[i][j];
        }
    }

}


void compress_image(vector<vector<uint8_t>> &image, double threshold, int balance, int threads) {
    //Convert image to complex type
    vector<vector<cmplx>> complex_image(image.size(), vector<base>(image[0].size()));
    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[0].size(); j++) {
            complex_image[i][j] = image[i][j];
        }
    }

    //Perform 2D fft on image
    fft_2D(complex_image, false, balance, threads);

    //Threshold the fft

    double maximum_value = 0.0;
    for (int i = 0; i < complex_image.size(); i++) {
        for (int j = 0; j < complex_image[0].size(); j++) {
            maximum_value = max(maximum_value, abs(complex_image[i][j]));
        }
    }
    threshold *= maximum_value;

    for (int i = 0; i < complex_image.size(); i++) {
        for (int j = 0; j < complex_image[0].size(); j++) {
            if (abs(complex_image[i][j]) < threshold) {
                complex_image[i][j] = 0;
            }
        }
    }
    int zeros_count = 0;
    for (int i = 0; i < complex_image.size(); i++) {
        for (int j = 0; j < complex_image[0].size(); j++) {
            if (abs(complex_image[i][j]) == 0) {
                zeros_count++;
            }
        }
    }
    cout << "Components removed: " << ((zeros_count*1.00/(complex_image.size()*complex_image[0].size())))*100 << endl;

    // Perform inverse FFT
    fft_2D(complex_image, true, balance, threads);

    // We will consider only the real part of the image
    for (int i = 0; i < complex_image.size(); i++) {
        for (int j = 0; j < complex_image[0].size(); j++) {
            image[i][j] = uint8_t(complex_image[i][j].real() + 0.5);
        }
    }

}


int main(int argc, char** argv) {

    // Parser* ip = new Parser();
    // ip->parse(argc, argv);
    // vector<cmplx> result(ip->length);
    // generate(result.begin(), result.end(), rand);

    // auto start = chrono::high_resolution_clock::now();
    vector<cmplx> result = {1, 2, 3, 4, 5, 6, 7, 8};
    fft(result, false, 2, 4);
    // auto stop = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    // cout << duration.count() << endl;

    // vector<int> result = multiply_poly(fa, fb, 2, 1024);
    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << " ";
    }
    cout << "\n";

    // std::vector<int> fa(ip->length);
    // std::generate(fa.begin(), fa.end(), std::rand);
    // std::vector<int> fb(ip->length);
    // std::generate(fb.begin(), fb.end(), std::rand);

    // auto start = chrono::high_resolution_clock::now(); 
    // auto result_parallel = multiply_poly(fa, fb, 4, ip->threads);
    // auto stop = chrono::high_resolution_clock::now();
    // auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    // cout << duration.count() << endl;


    
}

