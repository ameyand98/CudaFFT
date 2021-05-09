#ifndef _FFT_H_
#include <vector_types.h>
#include <vector>
#include <complex>
#include <chrono>
#include <iostream>

using namespace std;

using cmplx = complex<float>;
using cmplx_struct = float2; // needed for cuda array

const double ANGLE_MULT = 2 * M_PI;

namespace Sequential {

    extern vector<cmplx> fft(vector<cmplx>& data, bool invert);
    extern void fft_2D(vector<vector<cmplx> >& data, bool invert);
    extern void compress_img(vector<vector<uint8_t> >& image, double threshold);
    template <typename T>
    vector<T> multiply_poly(vector<T> poly1, vector<T> poly2) {
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

        vector<T> result;
        result.resize(size);

        for (int i = 0; i < size; i++) {
            result[i] = (f_cmplx[i].real() + 0.5);
        }

        return result;
    }
    extern bool is_power2(int x);

}

namespace CUDA {

    extern void fft_2D(vector<vector<cmplx> >& data, bool invert, int thread_balance, int threads);
    extern void fft(vector<cmplx>& array, bool invert, int balance, int threads);
    extern void real_fft(int size, int threads, cmplx_struct* reversed_nums, cmplx_struct* nums, int balance, bool invert);
    extern void compress_image(vector<vector<uint8_t> > &image, double threshold, int balance, int threads);

    template <typename T>
    vector<T> multiply_poly(vector<T> first, vector<T> second, int thread_balance, int threads) {
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

        vector<T> results;
        results.resize(size);
        for (int i = 0; i < size; i++) {
            results[i] = (cmplx_poly1[i].real() + 0.5);
        }

        return results;
    }
}


#endif