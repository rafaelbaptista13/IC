#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {


	if(argc < 3) {
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		return 1;
	}

	SndfileHandle sndOriginalFile { argv[argc-2] };
	if(sndOriginalFile.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    }

	if((sndOriginalFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if((sndOriginalFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}
    
    SndfileHandle sndQuantizatedFile { argv[argc-1] };
	if(sndQuantizatedFile.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    }

    if((sndQuantizatedFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	size_t nOriginalFrames, nQuantizatedFrames;
	vector<short> originalSamples(FRAMES_BUFFER_SIZE * sndOriginalFile.channels());
	vector<short> quantizatedSamples(FRAMES_BUFFER_SIZE * sndQuantizatedFile.channels());
	
	// Maximum per sample absolute error - vector to store the absolute error for each sample
	vector<short> samplesAbsoluteError;

	int originalAudioEnergy = 0;
	int noiseEnergy = 0;
	while((nOriginalFrames = sndOriginalFile.readf(originalSamples.data(), FRAMES_BUFFER_SIZE))) {

		nQuantizatedFrames = sndQuantizatedFile.readf(quantizatedSamples.data(), FRAMES_BUFFER_SIZE);
		
		originalSamples.resize(nOriginalFrames * sndOriginalFile.channels());
		quantizatedSamples.resize(nQuantizatedFrames * sndQuantizatedFile.channels());


		for (auto it = originalSamples.begin(); it != originalSamples.end(); ++it) {
            int index = distance(originalSamples.begin(), it);

			originalAudioEnergy = originalAudioEnergy + pow(abs( originalSamples[index] ), 2);
			noiseEnergy = noiseEnergy + pow(abs( (originalSamples[index] - quantizatedSamples[index]) ), 2);

			// Calculate absolute error and append to vector
			samplesAbsoluteError.push_back(abs(originalSamples[index] - quantizatedSamples[index]));
        }
	}

	int snr = 10*log10( originalAudioEnergy/noiseEnergy );
	cout << snr << endl;

	return 0;
}
