#ifndef _SEQUENTIALFFT_H_

#include <vector>
#include <complex>

using namespace std;
using cmplx = complex<float>;

namespace Sequential {

    void fft(vector<cmplx>& data, bool invert);
    void 2Dfft(vector<vector<cmplx>>& a, bool invert);
    void compress_img(vector<vector<uint8_t>>& image, double threshold);
}






















#endif