#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wav_quant.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {


	if(argc < 5) {
		cerr << "Usage: " << argv[0] << " <input file> <output file> <num_bits> <version> \n";
		return 1;
	}

	SndfileHandle sndFile { argv[argc-4] };
	if(sndFile.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    }

	if((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}
    
    SndfileHandle sfhOut { argv[argc-3], SFM_WRITE, sndFile.format(),
	  sndFile.channels(), sndFile.samplerate() };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

    int num_bits = -1;
    try {
        num_bits = stoi(argv[argc-2]);

        if(num_bits < 1 || num_bits > 15) {
            cerr << "Error: invalid number of bits requested\n";
            return 1;
        }
    } catch (invalid_argument const&) {	
        cerr << "Error: invalid number of bits requested\n";
        return 1;
    }

	int version = -1;
	try {
		version = stoi(argv[argc-1]);

	if(version != 1 && version != 2) {
		cerr << "Error: invalid version requested. Only version 1 or 2 are available\n";
		return 1;
	}
    } catch (invalid_argument const&) {	
        cerr << "Error: invalid version requested. Only version 1 or 2 are available\n";
        return 1;
    }

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	WAVQuant quant { num_bits, version };
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());
		samples = quant.quantization(samples);
		sfhOut.writef(samples.data(), nFrames);
	}

	return 0;
}

