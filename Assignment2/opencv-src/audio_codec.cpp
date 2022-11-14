#include <iostream>
#include <sndfile.hh>
#include "golomb_codec.h"
#include "BitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

void encodePredictor0(vector<short> samples, string mode, BitStream &bitStream) {
    GOLOMBCodec codec {6};

    int residual;

    for (auto it = samples.begin(); it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);

        residual = samples[index];

        string encoded_residual = codec.encode(residual);

        bitStream.write_n_bits(encoded_residual);
    }
}

void encodePredictor1(vector<short> samples, string mode, BitStream &bitStream) {
    GOLOMBCodec codec {6};

	// The residual for the first bit is 0
    int residual = samples[0];
	string encoded_residual = codec.encode(residual);
	bitStream.write_n_bits(encoded_residual);

    for (auto it = samples.begin() + 1; it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);

		residual = samples[index] - samples[index-1];

        encoded_residual = codec.encode(residual);

        bitStream.write_n_bits(encoded_residual);
    }
}

void encodePredictor2(vector<short> samples, string mode, BitStream &bitStream) {
    GOLOMBCodec codec {6};

	// The residual for the first element is the element
    int residual = samples[0];
	string encoded_residual = codec.encode(residual);
    bitStream.write_n_bits(encoded_residual);
	// The residual for the second element is value - 2 * first_elem
	residual = samples[1] - 2 * samples[0];
	encoded_residual = codec.encode(residual);
    bitStream.write_n_bits(encoded_residual);

    for (auto it = samples.begin() + 2; it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);

        residual = samples[index] - 2 * samples[index-1] - samples[index-2];

        encoded_residual = codec.encode(residual);

        bitStream.write_n_bits(encoded_residual);
    }
}

void encodePredictor3(vector<short> samples, string mode, BitStream &bitStream) {
    GOLOMBCodec codec {6};

    // The residual for the first element is the element
    int residual = samples[0];
	string encoded_residual = codec.encode(residual);
    bitStream.write_n_bits(encoded_residual);
	// The residual for the second element is value - 3 * first_elem
	residual = samples[1] - 3 * samples[0];
	encoded_residual = codec.encode(residual);
    bitStream.write_n_bits(encoded_residual);
	// The residual for the third element is value - 3 * second_elem - 3 * first elem
	residual = samples[2] - 3 * samples[1] - 3 * samples[0];
	encoded_residual = codec.encode(residual);
    bitStream.write_n_bits(encoded_residual);

    for (auto it = samples.begin() + 3; it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);

        residual = samples[index] - 3 * samples[index-1] - 3 * samples[index-2] + samples[index-3];

        encoded_residual = codec.encode(residual);

        bitStream.write_n_bits(encoded_residual);
    }
}


void encodeMonoAudio(vector<short> samples, int predictor_type, BitStream &bitStream) {

    if (predictor_type == 0) {
        encodePredictor0(samples, "mono", bitStream);
    } else if (predictor_type == 1) {
		encodePredictor1(samples, "mono", bitStream);
    } else if (predictor_type == 2) {
		encodePredictor2(samples, "mono", bitStream);
    } else if (predictor_type == 3) {
		encodePredictor3(samples, "mono", bitStream);
    }

}

/*
void encodeStereoAudio(vector<short> samples, int predictor_type, BitStream &bitStream) {
    std::cout << "Stereo" << std::endl;
}
*/

int main(int argc,const char** argv) {

    int predictor_type = 0;

    if(argc < 3) {
		cerr << "Usage: ./audio_codec [ -p predictor (def 0) ]\n";
		cerr << "                      input_file output_file\n";
		cerr << "Example: ./audio_codec -p 3 sample.wav\n";
		return 1;
	}

	SndfileHandle sndFile { argv[argc-2] };
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

    for(int n = 1 ; n < argc ; n++)
		if(string(argv[n]) == "-p") {
			predictor_type = atoi(argv[n+1]);
			break;
		}

    BitStream bitStream { argv[argc-1], "w" };
    
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        if (sndFile.channels() == 2) encodeMonoAudio(samples, predictor_type, bitStream);
        //else if (sndFile.channels() == 2) encodeStereoAudio(samples, predictor_type, bitStream);
        
	}

    return 0;
}

