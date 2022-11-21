#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"
#include "wav_quant.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames




void encodeMonoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, GolombCode golombCode, WAVQuant* wavQuant) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

    int residual = 0;
    string encoded_residual;

    int last_residuals[] = {0, 0, 0};
    
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
        
            if (predictor_type == 0) {
                residual = samples[index];
            } else if (predictor_type == 1) {
                residual = samples[index] - last_residuals[0];
            } else if (predictor_type == 2) {
                residual = samples[index] - 2 * last_residuals[0] - last_residuals[1];
            } else if (predictor_type == 3) {
                residual = samples[index] - 3 * last_residuals[0] - 3 * last_residuals[1] + last_residuals[2];
            }

            if (wavQuant != nullptr)
                encoded_residual = golombCode.encode(wavQuant->quantize(residual));
            else
                encoded_residual = golombCode.encode(residual);

            bitStream.write_n_bits(encoded_residual);
            
            last_residuals[2] = last_residuals[1];      // index - 3
            last_residuals[1] = last_residuals[0];      // index - 2
            last_residuals[0] = samples[index];         // index - 1
        }
    }


}


void encodeStereoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, GolombCode golombCode, WAVQuant* wavQuant) {
    
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());

    int lastSample = 0;
    int meanValue;
    int lastMeanValues[] = {0,0,0};
    int diffValue;
    int lastDiffValues[] = {0,0,0};
    
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    string encoded_mid_channel_residual;
    string encoded_side_channel_residual;

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
                
            if (index % 2 == 0) {
                lastSample = samples[index];
            } else {

                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]) / 2;

                if (predictor_type == 0) {
                    midChannelResidual = meanValue;
                    sideChannelResidual = diffValue;
                } else if (predictor_type == 1) {
                    midChannelResidual = meanValue - lastMeanValues[0];
                    sideChannelResidual = diffValue - lastDiffValues[0];
                } else if (predictor_type == 2) {
                    midChannelResidual = meanValue -  (2 * lastMeanValues[0]) - lastMeanValues[1];
                    sideChannelResidual = diffValue - (2 * lastDiffValues[0]) - lastDiffValues[1];               
                } else if (predictor_type == 3) {
                    midChannelResidual = meanValue - (3 * lastMeanValues[0]) - (3 * lastMeanValues[1]) + lastMeanValues[2];
                    sideChannelResidual = diffValue - (3 * lastDiffValues[0]) - (3 * lastDiffValues[1]) + lastDiffValues[2];
                }

                if (wavQuant != nullptr){
                    encoded_mid_channel_residual = golombCode.encode( wavQuant->quantize(midChannelResidual));
                    encoded_side_channel_residual = golombCode.encode( wavQuant->quantize(sideChannelResidual));
                }
                else {
                    encoded_mid_channel_residual = golombCode.encode(midChannelResidual);
                    encoded_side_channel_residual = golombCode.encode(sideChannelResidual);
                }

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


int main(int argc,const char** argv) {

    int predictor_type = 0;
    int golomb_m_parameter = 6;
    int quantize_bits = 0;

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

    for(int n = 1 ; n < argc ; n++){
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
		}
		else if(string(argv[n]) == "-m") {
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
		}
        else if(string(argv[n]) == "-q") {
            try {
                quantize_bits = atoi(argv[n+1]);

                if(quantize_bits < 1) {
                    cerr << "Error: invalid q parameter requested\n";
                    return 1;
                }
            } catch (invalid_argument const&) {
                cerr << "Error: invalid q parameter requested\n";
                return 1;
            }
        }


    }

    WAVQuant* wavQuant = nullptr;
    GolombCode golombCode {golomb_m_parameter};
    BitStream bitStream { argv[argc-1], "w" };

    if (quantize_bits > 0){
        wavQuant = new WAVQuant(quantize_bits, 1);
    }
    
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

    if (sndFile.channels() == 1) encodeMonoAudio(sndFile, predictor_type, bitStream, golombCode, wavQuant);
    else if (sndFile.channels() == 2) encodeStereoAudio(sndFile, predictor_type, bitStream, golombCode, wavQuant);

    bitStream.close();

    return 0;
}

