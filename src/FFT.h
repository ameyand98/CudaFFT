#ifndef _SEQUENTIALFFT_H_
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
}


#endif