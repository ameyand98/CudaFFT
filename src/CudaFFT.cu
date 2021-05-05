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

    // need to wait for all the bits to be reversed
    cudaDeviceSynchronize();

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


vector<int> multiply_poly(vector<int> first, vector<int> second, int thread_balance, int threads) {
    vector<cmplx> cmplx_poly1(first.begin(), first.end()), cmplx_poly2(second.begin(), second.end());

    int size = 1;
    int maximum = max(first.size(), second.size());
    while (size < maximum) {
        size <<= 1;
    }
    size <<= 1;

    cmplx_poly1.resize(size);
    cmplx_poly2.resize(size);

    fft(cmplx_poly1, false, thread_balance, threads);
    fft(cmplx_poly2, false, thread_balance, threads);

    for (int i = 0; i < size; i++) {
        cmplx_poly1[i] *= cmplx_poly2[i];
    }

    fft(cmplx_poly1, true, thread_balance, threads);

    vector<int> results;
    results.resize(size);
    for (int i = 0; i < size; i++) {
        results[i] = int(cmplx_poly1[i].real() + 0.5);
    }

    return results;
}


int main(int argc, char** argv) {

    Parser* ip = new Parser();
    ip->parse(argc, argv);
    vector<cmplx> result(ip->length);
    generate(result.begin(), result.end(), rand);

    auto start = chrono::high_resolution_clock::now();
    fft(result, false, 2, ip->threads);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    // cout << duration.count() << endl;

    // vector<int> result = multiply_poly(fa, fb, 2, 1024);
    // for (int i = 0; i < result.size(); i++) {
    //     cout << result[i] << " ";
    // }
    // cout << "\n";
    
}

