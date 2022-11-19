#include <iostream>
#include <sndfile.hh>
#include "golomb_codec.h"
#include "BitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames


void encodePredictor0MonoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

    int residual;

    for (auto it = samples.begin(); it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);

        residual = samples[index];

        string encoded_residual = codec.encode(residual);

        bitStream.write_n_bits(encoded_residual);
    }
}


void encodePredictor0StereoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

    int lastSample = 0;
    int meanValue;
    int diffValue;
    int midChannelResidual;
    int sideChannelResidual;

    for (auto it = samples.begin(); it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);
			
        if (index % 2 == 0) {
            lastSample = samples[index];
        } else {

            meanValue = (lastSample + samples[index]) / 2;
            diffValue = (lastSample - samples[index]) / 2;
            midChannelResidual = meanValue;
            sideChannelResidual = diffValue;

            string encoded_mid_channel_residual = codec.encode(midChannelResidual);
            string encoded_side_channel_residual = codec.encode(sideChannelResidual);

            bitStream.write_n_bits(encoded_mid_channel_residual);
            bitStream.write_n_bits(encoded_side_channel_residual);

        }

    }
}

void encodePredictor1MonoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

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


void encodePredictor1StereoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

    int lastSample = 0;
    int meanValue;
    int lastMeanValue = 0;
    int diffValue;
    int lastDiffValue = 0;
    
    // The residual for the first bit is 0
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    for (auto it = samples.begin(); it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);
			
        if (index % 2 == 0) {
            lastSample = samples[index];
        } else {

            meanValue = (lastSample + samples[index]) / 2;
            diffValue = (lastSample - samples[index]) / 2;
            midChannelResidual = meanValue - lastMeanValue;
            sideChannelResidual = diffValue - lastDiffValue;

            string encoded_mid_channel_residual = codec.encode(midChannelResidual);
            string encoded_side_channel_residual = codec.encode(sideChannelResidual);

            bitStream.write_n_bits(encoded_mid_channel_residual);
            bitStream.write_n_bits(encoded_side_channel_residual);

            lastMeanValue = meanValue;
            lastDiffValue = diffValue;

        }

    }
}

void encodePredictor2MonoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

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


void encodePredictor2StereoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

    int lastSample = 0;
    int meanValue;
    int lastMeanValues[] = {0,0};
    int diffValue;
    int lastDiffValues[] = {0,0};
    
    // The residual for the first bit is 0
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    for (auto it = samples.begin(); it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);
			
        if (index % 2 == 0) {
            lastSample = samples[index];
        } else {

            meanValue = (lastSample + samples[index]) / 2;
            diffValue = (lastSample - samples[index]) / 2;
            midChannelResidual = meanValue -  (2 * lastMeanValues[0]) - lastMeanValues[1];
            sideChannelResidual = diffValue - (2 * lastDiffValues[0]) - lastDiffValues[1];

            string encoded_mid_channel_residual = codec.encode(midChannelResidual);
            string encoded_side_channel_residual = codec.encode(sideChannelResidual);

            bitStream.write_n_bits(encoded_mid_channel_residual);
            bitStream.write_n_bits(encoded_side_channel_residual);

            lastMeanValues[1] = lastMeanValues[0];
            lastMeanValues[0] = meanValue;
            lastDiffValues[1] = lastDiffValues[0];
            lastDiffValues[0] = diffValue;

        }

    }
}


void encodePredictor3MonoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

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


void encodePredictor3StereoChannel(vector<short> samples, BitStream &bitStream, GOLOMBCodec codec) {

    int lastSample = 0;
    int meanValue;
    int lastMeanValues[] = {0,0,0};
    int diffValue;
    int lastDiffValues[] = {0,0,0};
    
    // The residual for the first bit is 0
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    for (auto it = samples.begin(); it != samples.end(); ++it) {
        int index = std::distance(samples.begin(), it);
			
        if (index % 2 == 0) {
            lastSample = samples[index];
        } else {

            meanValue = (lastSample + samples[index]) / 2;
            diffValue = (lastSample - samples[index]) / 2;
            midChannelResidual = meanValue - (3 * lastMeanValues[0]) - (3 * lastMeanValues[1]) + lastMeanValues[2];
            sideChannelResidual = diffValue - (3 * lastDiffValues[0]) - (3 * lastDiffValues[1]) + lastDiffValues[2];

            string encoded_mid_channel_residual = codec.encode(midChannelResidual);
            string encoded_side_channel_residual = codec.encode(sideChannelResidual);

            bitStream.write_n_bits(encoded_mid_channel_residual);
            bitStream.write_n_bits(encoded_side_channel_residual);

            lastMeanValues[2] = lastMeanValues[1];
            lastMeanValues[1] = lastMeanValues[0];
            lastMeanValues[0] = meanValue;
            lastDiffValues[2] = lastDiffValues[1];
            lastDiffValues[1] = lastDiffValues[0];
            lastDiffValues[0] = diffValue;

        }

    }
}


void encodeMonoAudio(vector<short> samples, int predictor_type, BitStream &bitStream, GOLOMBCodec codec) {

    if (predictor_type == 0) {
        encodePredictor0MonoChannel(samples, bitStream, codec);
    } else if (predictor_type == 1) {
		encodePredictor1MonoChannel(samples, bitStream, codec);
    } else if (predictor_type == 2) {
		encodePredictor2MonoChannel(samples, bitStream, codec);
    } else if (predictor_type == 3) {
		encodePredictor3MonoChannel(samples, bitStream, codec);
    }

}


void encodeStereoAudio(vector<short> samples, int predictor_type, BitStream &bitStream, GOLOMBCodec codec) {
    
    if (predictor_type == 0) {
        encodePredictor0StereoChannel(samples, bitStream, codec);
    } else if (predictor_type == 1) {
		encodePredictor1StereoChannel(samples, bitStream, codec);
    } else if (predictor_type == 2) {
		encodePredictor2StereoChannel(samples, bitStream, codec);
    } else if (predictor_type == 3) {
		encodePredictor3StereoChannel(samples, bitStream, codec);
    }
}


int main(int argc,const char** argv) {

    int predictor_type = 0;
    int golomb_m_parameter = 6;

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
            try {
			    predictor_type = atoi(argv[n+1]);
                
                if(predictor_type < 0 || predictor_type > 3) {
                    cerr << "Error: invalid p parameter requested\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid p parameter requested\n";
                return 1;
            }
			break;
		}

    for(int n = 1 ; n < argc ; n++)
		if(string(argv[n]) == "-m") {
            try {
			    golomb_m_parameter = atoi(argv[n+1]);

                if(golomb_m_parameter < 1) {
                    cerr << "Error: invalid m parameter requested\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid m parameter requested\n";
                return 1;
            }
			break;
		}

    GOLOMBCodec codec {golomb_m_parameter};
    BitStream bitStream { argv[argc-1], "w" };
    
    // Write golomb_m_parameter to coded file
	bitStream.write_n_bits(std::bitset<32>(golomb_m_parameter).to_string());
    // Write wavFileInput format to coded file
    string encoded_wavFileInputFormat = codec.encode(sndFile.format());
    bitStream.write_n_bits(encoded_wavFileInputFormat);
	// Write wavFileInput channels to coded file
    string encoded_wavFileInputChannels = codec.encode(sndFile.channels());
    bitStream.write_n_bits(encoded_wavFileInputChannels);
	// Write wavFileInput frames to coded file
    string encoded_wavFileInputFrames = codec.encode(sndFile.frames());
    bitStream.write_n_bits(encoded_wavFileInputFrames);
	// Write wavFileInput sampleRate to coded file
    string encoded_wavFileInputSampleRate = codec.encode(sndFile.samplerate());
    bitStream.write_n_bits(encoded_wavFileInputSampleRate);
    // Write predictor_type to coded file
    string encoded_predictor_type = codec.encode(predictor_type);
    bitStream.write_n_bits(encoded_predictor_type);

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        if (sndFile.channels() == 1) encodeMonoAudio(samples, predictor_type, bitStream, codec);
        else if (sndFile.channels() == 2) encodeStereoAudio(samples, predictor_type, bitStream, codec);
        
	}

    return 0;
}

