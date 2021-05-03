#ifndef _SEQUENTIALFFT_H_

#include <vector>
#include <complex>

using namespace std;
using cmplx = complex<float>;

namespace Sequential {

    extern void fft(vector<cmplx>& data, bool invert);
    extern void fft_2D(vector<vector<cmplx>>& data, bool invert);
    extern void compress_img(vector<vector<uint8_t>>& image, double threshold);
}


#endif