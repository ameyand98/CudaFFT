#ifndef _FFT_H_
#include <vector_types.h>
#include <vector>
#include <complex>

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
            result[i] = int(f_cmplx[i].real() + 0.5);
        }

        return result;
    }

}


#endif