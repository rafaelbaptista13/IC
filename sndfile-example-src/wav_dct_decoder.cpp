#include <iostream>
#include <vector>
#include <cmath>
#include <fftw3.h>
#include <sndfile.hh>
#include "BitStream.h"
#include <bitset>

using namespace std;

int main(int argc, char *argv[]) {

	size_t bs { 1024 };

	if(argc < 3) {
		cerr << "Usage: wav_dct_decoder\n";
		cerr << "               [ -bs blockSize (def 1024) ]\n";
		cerr << "               inputCodecFile wavFileOut\n";
		cerr << "Example: ./wav_dct_decoder bits_file output.wav\n";
		return 1;
	}

	for(int n = 1 ; n < argc ; n++)
		if(string(argv[n]) == "-bs") {
			bs = atoi(argv[n+1]);
			break;
		}

	// BitStream to read coded file
	BitStream bitStreamRead { argv[argc-2], "r" };
	// Get wavFileInput format
	std::string str_format = bitStreamRead.get_n_bits(32);
	int format = (int32_t) std::bitset<32>(str_format).to_ulong();
	// Get wavFileInput channels
	std::string str_channels = bitStreamRead.get_n_bits(32);
	int channels = (int32_t) std::bitset<32>(str_channels).to_ulong();
	// Get wavFileInput frames
	std::string str_frames = bitStreamRead.get_n_bits(32);
	int frames = (int32_t) std::bitset<32>(str_frames).to_ulong();
	// Get wavFileInput sampleRate
	std::string str_sample_rate = bitStreamRead.get_n_bits(32);
	int sample_rate = (int32_t) std::bitset<32>(str_sample_rate).to_ulong();
	// Get dctFrac
	std::string str_dctFrac = bitStreamRead.get_n_bits(32);
	double dctFrac = (int32_t) std::bitset<32>(str_dctFrac).to_ulong();
	dctFrac = dctFrac/1000;

	SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, format,
	  channels, sample_rate };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

	size_t nChannels { static_cast<size_t>(channels) };
	size_t nFrames { static_cast<size_t>(frames) };

	// Read all samples: c1 c2 ... cn c1 c2 ... cn ...
	// Note: A frame is a group c1 c2 ... cn
	vector<short> samples(nChannels * nFrames);

	size_t nBlocks { static_cast<size_t>(ceil(static_cast<double>(nFrames) / bs)) };

	// Do zero padding, if necessary
	samples.resize(nBlocks * bs * nChannels);

	// Vector for holding DCT coefficients
	vector<double> x(bs);

	// Inverse DCT
	fftw_plan plan_i = fftw_plan_r2r_1d(bs, x.data(), x.data(), FFTW_REDFT01, FFTW_ESTIMATE);
	for(size_t n = 0 ; n < nBlocks ; n++)
		for(size_t c = 0 ; c < nChannels ; c++) {
			for(size_t k = 0 ; k < bs ; k++) {
				if (k < bs * dctFrac) {
					std::string value = bitStreamRead.get_n_bits(32);
					x[k] = (int32_t) std::bitset<32>(value).to_ulong();
				} else {
					x[k] = 0;
				}
			}
			fftw_execute(plan_i);
			for(size_t k = 0 ; k < bs ; k++)
				samples[(n * bs + k) * nChannels + c] = static_cast<short>(round(x[k]));
		}

	sfhOut.writef(samples.data(), frames);

	return 0;
}

