#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"
#include "wav_quant.h"
#include "map"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames


string encodeResidual(GolombCode golombCode, WAVQuant* wavQuant, int residual) {
    if (wavQuant != nullptr) {
        return golombCode.encode(wavQuant->quantizeAndEncode(residual));
    }
    else
        return golombCode.encode(residual);
}

int optimizeGolombParameter(long int sumSamples, int numSamples) {
    double mean = (double) sumSamples/ (double) numSamples;
    double alfa = mean / (mean + 1);
    return ceil( -1/log2(alfa) ) ;
}

void encodeMonoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, WAVQuant* wavQuant, int window_size) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    long int sumSamples_array[] = {0, 0, 0, 0};
    int numSamples = 0;
    int golomb_m_parameter_array[] = {100, 100, 100, 100};       // Initial m = 100

    int residual = 0;
    int predicted_value = 0;
    string encoded_residuals_array[] = {"", "", "", ""};
    int lastSamples[] = {0, 0, 0};
    GolombCode golombCode {100};
    GolombCode currentGolombCodes[] = {golombCode, golombCode, golombCode, golombCode};

    map<int, GolombCode> golombCodes = {{100, golombCode}};

	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());
        
        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);

            numSamples += 1;
            if (predictor_type == 0) {
                residual = samples[index];
            } else if (predictor_type == 1) {
                residual = samples[index] - lastSamples[0];
            } else if (predictor_type == 2) {
                residual = samples[index] - (2 * lastSamples[0] - lastSamples[1]);
            } else if (predictor_type == 3) {
                residual = samples[index] - (3 * lastSamples[0] - 3 * lastSamples[1] + lastSamples[2]);
            } else if (predictor_type == 4) {
                int residuals[4] = {samples[index],
                                    samples[index] - lastSamples[0],
                                    samples[index] - (2 * lastSamples[0] - lastSamples[1]),
                                    samples[index] - (3 * lastSamples[0] - 3 * lastSamples[1] + lastSamples[2])};

                for (int predictor = 0; predictor < 4; predictor++) {
                    // Encode and append to encodedstring
                    encoded_residuals_array[predictor] += encodeResidual(currentGolombCodes[predictor], wavQuant, residuals[predictor]);
                    sumSamples_array[predictor] += abs(residuals[predictor]);
                }
            }
            
            if (predictor_type != 4) {
                // Encode and write
                string encoded_residual = encodeResidual(currentGolombCodes[predictor_type], wavQuant, residual);
                bitStream.write_n_bits(encoded_residual);
                if (!wavQuant)
                    sumSamples_array[predictor_type] += abs(residual);
                else
                    sumSamples_array[predictor_type] += abs(wavQuant->quantize_value(residual));
            }

            if (wavQuant){
                if (predictor_type == 0) {
                    predicted_value = wavQuant->quantize_value(residual);
                } else if (predictor_type == 1) {
                    predicted_value = lastSamples[0] + wavQuant->quantize_value(residual);
                } else if (predictor_type == 2) {
                    predicted_value = wavQuant->quantize_value(residual) + (2*lastSamples[0]) - lastSamples[1];
                } else if (predictor_type == 3){
                    predicted_value = wavQuant->quantize_value(residual) + 3 * lastSamples[0] - 3 * lastSamples[1] + lastSamples[2];
                }
            }


            lastSamples[2] = lastSamples[1];      // index - 3
            lastSamples[1] = lastSamples[0];      // index - 2
            if (!wavQuant)                        // index - 1
                lastSamples[0] = samples[index];
            else
                lastSamples[0] = predicted_value;

            if (numSamples % window_size == 0) {
                // Update golombCodes
                for (int predictor = 0; predictor < 4; predictor++) {
                    golomb_m_parameter_array[predictor] = optimizeGolombParameter(sumSamples_array[predictor], window_size);
                    if (golombCodes.count(golomb_m_parameter_array[predictor])) {
                        currentGolombCodes[predictor] = golombCodes.find(golomb_m_parameter_array[predictor])->second;
                    } else {
                        // Create golombCode
                        currentGolombCodes[predictor] = GolombCode(golomb_m_parameter_array[predictor]);
                        golombCodes.insert({golomb_m_parameter_array[predictor], currentGolombCodes[predictor]});
                    }
                    sumSamples_array[predictor] = 0;
                }
            }
        }
        

        if (predictor_type == 4) {
            unsigned int smallestSize = encoded_residuals_array[0].length();
            int bestPredictor = 0;
            for (int i = 1; i<4; i++) {
                if ( encoded_residuals_array[i].length() < smallestSize) {
                    smallestSize = encoded_residuals_array[i].length();
                    bestPredictor = i;
                }
            }

            bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(30, 32));
            bitStream.write_n_bits(encoded_residuals_array[bestPredictor]);

            encoded_residuals_array[0] = "";
            encoded_residuals_array[1] = "";
            encoded_residuals_array[2] = "";
            encoded_residuals_array[3] = "";
        }
    }
}





void encodeStereoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, WAVQuant* wavQuant, int window_size) {
    
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    long int mid_sumSamples_array[] = {0, 0, 0, 0};
    long int side_sumSamples_array[] = {0, 0, 0, 0};
    int numSamples = 0;
    uint mid_golomb_m_parameter_array[] = {100, 100, 100, 100};          // Initial m = 100
    uint side_golomb_m_parameter_array[] = {100, 100, 100, 100};         // Initial m = 100

    int lastSample = 0;
    int meanValue;
    int lastMeanValues[] = {0,0,0};
    int diffValue;
    int lastDiffValues[] = {0,0,0};
    
    int midChannelResidual = 0;
    int sideChannelResidual = 0;

    string encoded_residuals_array[] = {"", "", "", ""};

    GolombCode golombCode {100};
    GolombCode currentSideGolombCodes[] = {golombCode, golombCode, golombCode, golombCode};
    GolombCode currentMidGolombCodes[] = {golombCode, golombCode, golombCode, golombCode};

    map<int, GolombCode> golombCodes = {{100, golombCode}};

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            
            if (index % 2 == 0) {
                lastSample = samples[index];
            } else {
                
                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]);
                numSamples += 1;

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
                } else if (predictor_type == 4) {
                    
                    int midChannelResiduals[4] = {meanValue, 
                                                meanValue - lastMeanValues[0], 
                                                meanValue -  (2 * lastMeanValues[0]) - lastMeanValues[1], 
                                                meanValue - (3 * lastMeanValues[0]) - (3 * lastMeanValues[1]) + lastMeanValues[2]};
                    int sideChannelResiduals[4] = {diffValue,
                                                diffValue - lastDiffValues[0],
                                                diffValue - (2 * lastDiffValues[0]) - lastDiffValues[1],
                                                diffValue - (3 * lastDiffValues[0]) - (3 * lastDiffValues[1]) + lastDiffValues[2]};

                    for (int predictor=0; predictor<4; predictor++) {
                        // Encode and append to encodedstring
                        encoded_residuals_array[predictor] += encodeResidual(currentMidGolombCodes[predictor], wavQuant, midChannelResiduals[predictor]);
                        mid_sumSamples_array[predictor] += abs(midChannelResiduals[predictor]);

                        // Encode and append to encodedstring
                        encoded_residuals_array[predictor] += encodeResidual(currentSideGolombCodes[predictor], wavQuant, sideChannelResiduals[predictor]);
                        side_sumSamples_array[predictor] += abs(sideChannelResiduals[predictor]);
                    }

                }

                if (predictor_type != 4) {
                    // Encode and write
                    string mid_encoded_residual = encodeResidual(currentMidGolombCodes[predictor_type], wavQuant, midChannelResidual);
                    bitStream.write_n_bits(mid_encoded_residual);
                    mid_sumSamples_array[predictor_type] += abs(midChannelResidual);

                    // Encode and write
                    string side_encoded_residual = encodeResidual(currentSideGolombCodes[predictor_type], wavQuant, sideChannelResidual);
                    bitStream.write_n_bits(side_encoded_residual);
                    side_sumSamples_array[predictor_type] += abs(sideChannelResidual);
                }

                lastMeanValues[2] = lastMeanValues[1];
                lastMeanValues[1] = lastMeanValues[0];
                lastMeanValues[0] = meanValue;
                lastDiffValues[2] = lastDiffValues[1];
                lastDiffValues[1] = lastDiffValues[0];
                lastDiffValues[0] = diffValue;

                if (numSamples % window_size == 0) {
                    // Update golombCodes
                    for (int predictor = 0; predictor < 4; predictor++) {
                        side_golomb_m_parameter_array[predictor] = optimizeGolombParameter(side_sumSamples_array[predictor], window_size);
                        mid_golomb_m_parameter_array[predictor] = optimizeGolombParameter(mid_sumSamples_array[predictor], window_size);
                        if (golombCodes.count(side_golomb_m_parameter_array[predictor])) {
                            currentSideGolombCodes[predictor] = golombCodes.find(side_golomb_m_parameter_array[predictor])->second;
                        } else {
                            // Create golombCode
                            currentSideGolombCodes[predictor] = GolombCode(side_golomb_m_parameter_array[predictor]);
                            golombCodes.insert({side_golomb_m_parameter_array[predictor], currentSideGolombCodes[predictor]});
                        }

                        if (golombCodes.count(mid_golomb_m_parameter_array[predictor])) {
                            currentMidGolombCodes[predictor] = golombCodes.find(mid_golomb_m_parameter_array[predictor])->second;
                        } else {
                            // Create golombCode
                            currentMidGolombCodes[predictor] = GolombCode(mid_golomb_m_parameter_array[predictor]);
                            golombCodes.insert({mid_golomb_m_parameter_array[predictor], currentMidGolombCodes[predictor]});
                        }

                        side_sumSamples_array[predictor] = 0;
                        mid_sumSamples_array[predictor] = 0;
                    }
                }
            }
            
            
        }


        if (predictor_type == 4) {

            unsigned int smallestSize = encoded_residuals_array[0].length();
            int bestPredictor = 0;
            for (int i = 1; i<4; i++) {

                if ( encoded_residuals_array[i].length() < smallestSize) {
                    smallestSize = encoded_residuals_array[i].length();
                    bestPredictor = i;
                }
            }

            bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(30, 32));
            bitStream.write_n_bits(encoded_residuals_array[bestPredictor]);

            encoded_residuals_array[0] = "";
            encoded_residuals_array[1] = "";
            encoded_residuals_array[2] = "";
            encoded_residuals_array[3] = "";
        }
    }

}


int main(int argc,const char** argv) {

    int predictor_type = 0;
    int quantize_bits = 0;
    int window_size = 50;

    if(argc < 3) {
		cerr << "Usage: ./audio_codec [ -p predictor (def 0) ]\n";
		cerr << "                      input_file output_file\n";
		cerr << "Example: ./audio_codec -p 0 -w 10 sample.wav compressed.wav\n";
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
                
                if(predictor_type < 0 || predictor_type > 4) {
                    cerr << "Error: invalid p parameter requested\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid p parameter requested\n";
                return 1;
            }
		}
		else if(string(argv[n]) == "-w") {
            try {
			    window_size = atoi(argv[n+1]);

                if(window_size < 1) {
                    cerr << "Error: invalid window size parameter requested\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid window size parameter requested\n";
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
    BitStream bitStream { argv[argc-1], "w" };

    if (quantize_bits > 0){
        wavQuant = new WAVQuant(quantize_bits, 1);
    }
    
    // Write wavFileInput format to coded file
    bitStream.write_n_bits(std::bitset<32>(sndFile.format()).to_string());
	// Write wavFileInput channels to coded file
    bitStream.write_n_bits(std::bitset<32>(sndFile.channels()).to_string());
	// Write wavFileInput frames to coded file
    bitStream.write_n_bits(std::bitset<32>(sndFile.frames()).to_string());
	// Write wavFileInput sampleRate to coded file
    bitStream.write_n_bits(std::bitset<32>(sndFile.samplerate()).to_string());
    // Write block size to coded file
    bitStream.write_n_bits(std::bitset<32>(FRAMES_BUFFER_SIZE).to_string());
    // Write predictor_type to coded file
    bitStream.write_n_bits(std::bitset<32>(predictor_type).to_string());
    // Write window_size to coded file
    bitStream.write_n_bits(std::bitset<32>(window_size).to_string());
    // Write quantize_bits to coded file
    bitStream.write_n_bits(std::bitset<32>(quantize_bits).to_string());
    

    if (sndFile.channels() == 1) encodeMonoAudio(sndFile, predictor_type, bitStream, wavQuant, window_size);
    else if (sndFile.channels() == 2) encodeStereoAudio(sndFile, predictor_type, bitStream, wavQuant, window_size);

    bitStream.close();

    return 0;
}

