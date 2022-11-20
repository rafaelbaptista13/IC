#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames


void encodePredictor0MonoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    int residual;
    string encoded_residual;
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            
            residual = samples[index];

            encoded_residual = golombCode.encode(residual);

            bitStream.write_n_bits(encoded_residual);
        }
    }
}


void encodePredictor0StereoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

    int lastSample = 0;
    int meanValue;
    int diffValue;
    int midChannelResidual;
    int sideChannelResidual;

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
                
            if (index % 2 == 0) {
                lastSample = samples[index];
            } else {

                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]) / 2;
                midChannelResidual = meanValue;
                sideChannelResidual = diffValue;
                std::cout << "Mid Channel "<< midChannelResidual << std::endl;
                std::cout << "Side Channel "<< sideChannelResidual << std::endl;

                string encoded_mid_channel_residual = golombCode.encode(midChannelResidual);
                string encoded_side_channel_residual = golombCode.encode(sideChannelResidual);

                bitStream.write_n_bits(encoded_mid_channel_residual);
                bitStream.write_n_bits(encoded_side_channel_residual);

            }

        }
    }
}


void encodePredictor1MonoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    int residual;
    string encoded_residual;
    int last_residual = 0;
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            
            residual = samples[index] - last_residual;

            encoded_residual = golombCode.encode(residual);

            bitStream.write_n_bits(encoded_residual);

            last_residual = samples[index];     // index - 1
        }
    }
}


void encodePredictor1StereoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

    int lastSample = 0;
    int meanValue;
    int lastMeanValue = 0;
    int diffValue;
    int lastDiffValue = 0;
    
    // The residual for the first bit is 0
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
                
            if (index % 2 == 0) {
                lastSample = samples[index];
            } else {

                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]) / 2;
                midChannelResidual = meanValue - lastMeanValue;
                sideChannelResidual = diffValue - lastDiffValue;

                string encoded_mid_channel_residual = golombCode.encode(midChannelResidual);
                string encoded_side_channel_residual = golombCode.encode(sideChannelResidual);

                bitStream.write_n_bits(encoded_mid_channel_residual);
                bitStream.write_n_bits(encoded_side_channel_residual);

                lastMeanValue = meanValue;
                lastDiffValue = diffValue;

            }

        }
    }
}


void encodePredictor2MonoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    int residual;
    string encoded_residual;
    int last_residuals[] = {0, 0};
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            
            residual = samples[index] - 2 * last_residuals[0] - last_residuals[1];

            encoded_residual = golombCode.encode(residual);

            bitStream.write_n_bits(encoded_residual);

            last_residuals[1] = last_residuals[0];   // index - 2
            last_residuals[0] = samples[index];     // index - 1
            
        }
    }
}


void encodePredictor2StereoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

    int lastSample = 0;
    int meanValue;
    int lastMeanValues[] = {0,0};
    int diffValue;
    int lastDiffValues[] = {0,0};
    
    // The residual for the first bit is 0
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
                
            if (index % 2 == 0) {
                lastSample = samples[index];
            } else {

                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]) / 2;
                midChannelResidual = meanValue -  (2 * lastMeanValues[0]) - lastMeanValues[1];
                sideChannelResidual = diffValue - (2 * lastDiffValues[0]) - lastDiffValues[1];

                string encoded_mid_channel_residual = golombCode.encode(midChannelResidual);
                string encoded_side_channel_residual = golombCode.encode(sideChannelResidual);

                bitStream.write_n_bits(encoded_mid_channel_residual);
                bitStream.write_n_bits(encoded_side_channel_residual);

                lastMeanValues[1] = lastMeanValues[0];
                lastMeanValues[0] = meanValue;
                lastDiffValues[1] = lastDiffValues[0];
                lastDiffValues[0] = diffValue;

            }

        }
    }
}


void encodePredictor3MonoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    int residual;
    string encoded_residual;
    int last_residuals[] = {0, 0, 0};
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            
            residual = samples[index] - 3 * last_residuals[0] - 3 * last_residuals[1] + last_residuals[2];

            encoded_residual = golombCode.encode(residual);

            bitStream.write_n_bits(encoded_residual);
            
            last_residuals[2] = last_residuals[1];   // index - 3
            last_residuals[1] = last_residuals[0];   // index - 2
            last_residuals[0] = samples[index];     // index - 1
        }
    }
}


void encodePredictor3StereoChannel(SndfileHandle sndFile, BitStream &bitStream, GolombCode golombCode) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

    int lastSample = 0;
    int meanValue;
    int lastMeanValues[] = {0,0,0};
    int diffValue;
    int lastDiffValues[] = {0,0,0};
    
    // The residual for the first bit is 0
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
                
            if (index % 2 == 0) {
                lastSample = samples[index];
            } else {

                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]) / 2;
                midChannelResidual = meanValue - (3 * lastMeanValues[0]) - (3 * lastMeanValues[1]) + lastMeanValues[2];
                sideChannelResidual = diffValue - (3 * lastDiffValues[0]) - (3 * lastDiffValues[1]) + lastDiffValues[2];

                string encoded_mid_channel_residual = golombCode.encode(midChannelResidual);
                string encoded_side_channel_residual = golombCode.encode(sideChannelResidual);

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
}


void encodeMonoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, GolombCode golombCode) {

    if (predictor_type == 0) {
        encodePredictor0MonoChannel(sndFile, bitStream, golombCode);
    } else if (predictor_type == 1) {
		encodePredictor1MonoChannel(sndFile, bitStream, golombCode);
    } else if (predictor_type == 2) {
		encodePredictor2MonoChannel(sndFile, bitStream, golombCode);
    } else if (predictor_type == 3) {
		encodePredictor3MonoChannel(sndFile, bitStream, golombCode);
    }

}


void encodeStereoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, GolombCode golombCode) {
    
    if (predictor_type == 0) {
        encodePredictor0StereoChannel(sndFile, bitStream, golombCode);
    } else if (predictor_type == 1) {
		encodePredictor1StereoChannel(sndFile, bitStream, golombCode);
    } else if (predictor_type == 2) {
		encodePredictor2StereoChannel(sndFile, bitStream, golombCode);
    } else if (predictor_type == 3) {
		encodePredictor3StereoChannel(sndFile, bitStream, golombCode);
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

    GolombCode golombCode {golomb_m_parameter};
    BitStream bitStream { argv[argc-1], "w" };
    
    // Write golomb_m_parameter to coded file
	bitStream.write_n_bits(std::bitset<32>(golomb_m_parameter).to_string());
    // Write wavFileInput format to coded file
    string encoded_wavFileInputFormat = golombCode.encode(sndFile.format());
    bitStream.write_n_bits(encoded_wavFileInputFormat);
	// Write wavFileInput channels to coded file
    string encoded_wavFileInputChannels = golombCode.encode(sndFile.channels());
    bitStream.write_n_bits(encoded_wavFileInputChannels);
	// Write wavFileInput frames to coded file
    string encoded_wavFileInputFrames = golombCode.encode(sndFile.frames());
    bitStream.write_n_bits(encoded_wavFileInputFrames);
	// Write wavFileInput sampleRate to coded file
    string encoded_wavFileInputSampleRate = golombCode.encode(sndFile.samplerate());
    bitStream.write_n_bits(encoded_wavFileInputSampleRate);
    // Write predictor_type to coded file
    string encoded_predictor_type = golombCode.encode(predictor_type);
    bitStream.write_n_bits(encoded_predictor_type);

    if (sndFile.channels() == 1) encodeMonoAudio(sndFile, predictor_type, bitStream, golombCode);
    else if (sndFile.channels() == 2) encodeStereoAudio(sndFile, predictor_type, bitStream, golombCode);

    bitStream.close();

    return 0;
}

