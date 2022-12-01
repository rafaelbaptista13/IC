#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"
#include "wav_quant.h"
#include "map"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames


/*
    Function used to encode a residual value using the Golomb Code
*/

string encodeResidual(GolombCode golombCode, WAVQuant* wavQuant, int residual) {
    if (wavQuant != nullptr) {
        return golombCode.encode(wavQuant->quantizeAndEncode(residual));
    }
    else
        return golombCode.encode(residual);
}


/*
    Function used to optimize the Golomb Parameter m
*/

int optimizeGolombParameter(long int sumSamples, int numSamples) {
    double mean = (double) sumSamples/ (double) numSamples;
    double alfa = mean / (mean + 1);
    return ceil( -1/log2(alfa) ) ;
}


/*
    Function used to encode a Mono audio
*/

void encodeMonoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, WAVQuant* wavQuant, int window_size) {

    // Number of frames read
    size_t nFrames;
    // Samples read
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    // Sum of Residuals values for all predictors
    long int sumResiduals[] = {0, 0, 0, 0};
    // Number of samples read
    int numSamples = 0;
    // Current M parameter for all predictors
    int golomb_m_parameter[] = {100, 100, 100, 100};       // Initial m = 100

    // Residual value
    int residual = 0;
    // Predicted Value
    int predicted_value = 0;
    // String to store the encoded residuals of a block for all predictors, in order to compare them later
    string encoded_residuals[] = {"", "", "", ""};
    // Write first golomb m parameter of each predictor and initial sum residual in first block
    for (int predictor = 0; predictor < 4; predictor++) {
        encoded_residuals[predictor] += std::bitset<32>(golomb_m_parameter[predictor]).to_string();
        encoded_residuals[predictor] += std::bitset<32>(sumResiduals[predictor]).to_string();
    }
    // Array to store the last three values in order to calculate residuals for all predictors
    int lastSamples[] = {0, 0, 0};
    // Initial Golomb Code
    GolombCode golombCode {100};
    // Array to store the current Golomb Code of each predictor
    GolombCode currentGolombCodes[] = {golombCode, golombCode, golombCode, golombCode};
    // Map to store the used Golomb Codes
    map<int, GolombCode> golombCodes = {{100, golombCode}};

	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());
        
        // Iterate samples of this block
        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);

            numSamples += 1;
            // Calculate Residual
            if (predictor_type == 0) {
                residual = samples[index];
            } else if (predictor_type == 1) {
                residual = samples[index] - lastSamples[0];
            } else if (predictor_type == 2) {
                residual = samples[index] - (2 * lastSamples[0] - lastSamples[1]);
            } else if (predictor_type == 3) {
                residual = samples[index] - (3 * lastSamples[0] - 3 * lastSamples[1] + lastSamples[2]);
            } else if (predictor_type == 4) {
                // Calculate residuals for all predictors
                int residuals[4] = {samples[index],
                                    samples[index] - lastSamples[0],
                                    samples[index] - (2 * lastSamples[0] - lastSamples[1]),
                                    samples[index] - (3 * lastSamples[0] - 3 * lastSamples[1] + lastSamples[2])};
                for (int predictor = 0; predictor < 4; predictor++) {
                    // Encode and append to encoded string
                    encoded_residuals[predictor] += encodeResidual(currentGolombCodes[predictor], wavQuant, residuals[predictor]);
                    sumResiduals[predictor] += abs(residuals[predictor]);
                }
            }
            
            // If the predictor type is not the one that tries all the predictors and choose the best one
            if (predictor_type != 4) {
                // Encode residual and write
                string encoded_residual = encodeResidual(currentGolombCodes[predictor_type], wavQuant, residual);
                bitStream.write_n_bits(encoded_residual);
                if (!wavQuant)
                    sumResiduals[predictor_type] += abs(residual);
                else
                    sumResiduals[predictor_type] += abs(wavQuant->quantize_value(residual));
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

            // Update the last three samples
            lastSamples[2] = lastSamples[1];      // index - 3
            lastSamples[1] = lastSamples[0];      // index - 2
            if (!wavQuant)                        // index - 1
                lastSamples[0] = samples[index];
            else
                lastSamples[0] = predicted_value;

            if (numSamples % window_size == 0) {
                // Update golombCodes
                for (int predictor = 0; predictor < 4; predictor++) {
                    golomb_m_parameter[predictor] = optimizeGolombParameter(sumResiduals[predictor], window_size);
                    if (golombCodes.count(golomb_m_parameter[predictor])) {
                        // Get the golombCode from the map
                        currentGolombCodes[predictor] = golombCodes.find(golomb_m_parameter[predictor])->second;
                    } else {
                        // Create golombCode
                        currentGolombCodes[predictor] = GolombCode(golomb_m_parameter[predictor]);
                        golombCodes.insert({golomb_m_parameter[predictor], currentGolombCodes[predictor]});
                    }
                    // Reset sum of residuals
                    sumResiduals[predictor] = 0;
                }
            }
        }
        
        // If the predictor type is the one that tries all the predictors and choose the best one
        if (predictor_type == 4) {
            // Choose the best predictor based on the size of the encoded string
            unsigned int smallestSize = encoded_residuals[0].length();
            int bestPredictor = 0;
            for (int i = 1; i<4; i++) {
                if ( encoded_residuals[i].length() < smallestSize) {
                    smallestSize = encoded_residuals[i].length();
                    bestPredictor = i;
                }
            }

            // Write the best predictor value
            bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(30, 32));
            // Write the encoded string of the best predictor
            bitStream.write_n_bits(encoded_residuals[bestPredictor]);

            // Reset the encoded string by starting with the first golomb m parameter and the current sum of residuals
            for (int predictor = 0; predictor < 4; predictor++) {
                encoded_residuals[predictor] = std::bitset<32>(golomb_m_parameter[predictor]).to_string();
                encoded_residuals[predictor] += std::bitset<32>(sumResiduals[predictor]).to_string();
            }
        }
    }
}



/*
    Function used to encode a Stereo audio
*/

void encodeStereoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, WAVQuant* wavQuant, int window_size) {
    
    // Number of frames read
    size_t nFrames;
    // Samples read
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    // Sum of Residuals values of MID channel for all predictors
    long int mid_sumResiduals[] = {0, 0, 0, 0};
    // Sum of Residuals values of SIDE channel for all predictors
    long int side_sumResiduals[] = {0, 0, 0, 0};
    // Number of samples read
    int numSamples = 0;
    // Current M parameter of MID channel for all predictors
    uint mid_golomb_m_parameter[] = {100, 100, 100, 100};          // Initial m = 100
    // Current M parameter of SIDE channel for all predictors
    uint side_golomb_m_parameter[] = {100, 100, 100, 100};         // Initial m = 100

    // To store the last sample
    int lastSample = 0;
    // Mean value
    int meanValue;
    int predictedMeanValue=0;
    // Array to store the last three values of mean in order to calculate the residuals for all predictors
    int lastMeanValues[] = {0,0,0};
    // Difference value
    int diffValue;
    int predictedDiffValue=0;
    // Array to store the last three values of difference in order to calculate the residuals for all predictors
    int lastDiffValues[] = {0,0,0};
    
    // Residual value of Mid Channel
    int midChannelResidual = 0;
    // Residual value of Side Channel
    int sideChannelResidual = 0;

    // String to store the encoded residuals of a block for all predictors, in order to compare them later
    string encoded_residuals_array[] = {"", "", "", ""};
    // Write first golomb m parameter of each predictor and initial sum residual in first block for mid and side channels
    for (int predictor=0; predictor<4; predictor++) {
        encoded_residuals_array[predictor] = std::bitset<32>(side_golomb_m_parameter[predictor]).to_string();
        encoded_residuals_array[predictor] += std::bitset<32>(side_sumResiduals[predictor]).to_string();
        encoded_residuals_array[predictor] += std::bitset<32>(mid_golomb_m_parameter[predictor]).to_string();
        encoded_residuals_array[predictor] += std::bitset<32>(mid_sumResiduals[predictor]).to_string();
    }

    // Initial Golomb Code
    GolombCode golombCode {100};
    // Array to store the current Golomb Code of each predictor of Side Channel
    GolombCode currentSideGolombCodes[] = {golombCode, golombCode, golombCode, golombCode};
    // Array to store the current Golomb Code of each predictor of Mid Channel
    GolombCode currentMidGolombCodes[] = {golombCode, golombCode, golombCode, golombCode};
    // Map to store the used Golomb Codes
    map<int, GolombCode> golombCodes = {{100, golombCode}};

    while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());

        // Iterate samples of this block
        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);
            
            if (index % 2 == 0) {
                // Needed to save the sample in order to calculate the mean and difference between two samples
                lastSample = samples[index];
            } else {
                
                // Calculate Mean and Difference values
                meanValue = (lastSample + samples[index]) / 2;
                diffValue = (lastSample - samples[index]);
                numSamples += 1;

                // Calculate Residual for both channels
                if (predictor_type == 0) {
                    midChannelResidual = meanValue;
                    sideChannelResidual = diffValue;
                } else if (predictor_type == 1) {
                    midChannelResidual = meanValue - lastMeanValues[0];
                    sideChannelResidual = diffValue - lastDiffValues[0];
                } else if (predictor_type == 2) {
                    midChannelResidual = meanValue -  (2 * lastMeanValues[0] - lastMeanValues[1]);
                    sideChannelResidual = diffValue - (2 * lastDiffValues[0] - lastDiffValues[1]);
                } else if (predictor_type == 3) {
                    midChannelResidual = meanValue - (3 * lastMeanValues[0] - 3 * lastMeanValues[1] + lastMeanValues[2]);
                    sideChannelResidual = diffValue - (3 * lastDiffValues[0] - 3 * lastDiffValues[1] + lastDiffValues[2]);
                } else if (predictor_type == 4) {
                    // Calculate residuals for all predictors
                    int midChannelResiduals[4] = {meanValue, 
                                                meanValue - lastMeanValues[0], 
                                                meanValue -  (2 * lastMeanValues[0] - lastMeanValues[1]),
                                                meanValue - (3 * lastMeanValues[0] - (3 * lastMeanValues[1]) + lastMeanValues[2])};
                    int sideChannelResiduals[4] = {diffValue,
                                                diffValue - lastDiffValues[0],
                                                diffValue - (2 * lastDiffValues[0] - lastDiffValues[1]),
                                                diffValue - (3 * lastDiffValues[0] - (3 * lastDiffValues[1]) + lastDiffValues[2])};
                    
                    for (int predictor=0; predictor<4; predictor++) {
                        // Encode and append to encoded string
                        encoded_residuals_array[predictor] += encodeResidual(currentMidGolombCodes[predictor], wavQuant, midChannelResiduals[predictor]);
                        mid_sumResiduals[predictor] += abs(midChannelResiduals[predictor]);

                        // Encode and append to encoded string
                        encoded_residuals_array[predictor] += encodeResidual(currentSideGolombCodes[predictor], wavQuant, sideChannelResiduals[predictor]);
                        side_sumResiduals[predictor] += abs(sideChannelResiduals[predictor]);
                    }

                }

                // If the predictor type is not the one that tries all the predictors and choose the best one
                if (predictor_type != 4) {
                    // Encode and write
                    string mid_encoded_residual = encodeResidual(currentMidGolombCodes[predictor_type], wavQuant, midChannelResidual);
                    bitStream.write_n_bits(mid_encoded_residual);
                    if(!wavQuant)
                        mid_sumResiduals[predictor_type] += abs(midChannelResidual);
                    else
                        mid_sumResiduals[predictor_type] += abs(wavQuant->quantize_value(midChannelResidual));
                    // Encode and write
                    string side_encoded_residual = encodeResidual(currentSideGolombCodes[predictor_type], wavQuant, sideChannelResidual);
                    bitStream.write_n_bits(side_encoded_residual);
                    if(!wavQuant)
                        side_sumResiduals[predictor_type] += abs(sideChannelResidual);
                    else
                        side_sumResiduals[predictor_type] += abs(wavQuant->quantize_value(sideChannelResidual));
                }

                if (wavQuant){
                    if (predictor_type == 0) {
                        predictedMeanValue = wavQuant->quantize_value(midChannelResidual);
                        predictedDiffValue = wavQuant->quantize_value(sideChannelResidual);
                    } else if (predictor_type == 1) {
                        predictedMeanValue = lastMeanValues[0] + wavQuant->quantize_value(midChannelResidual);
                        predictedDiffValue = lastDiffValues[0] + wavQuant->quantize_value(sideChannelResidual);
                    } else if (predictor_type == 2) {
                        predictedMeanValue = wavQuant->quantize_value(midChannelResidual) + (2*lastMeanValues[0]) - lastMeanValues[1];
                        predictedDiffValue = wavQuant->quantize_value(sideChannelResidual) + (2*lastDiffValues[0]) - lastDiffValues[1];
                    } else if (predictor_type == 3){
                        predictedMeanValue = wavQuant->quantize_value(midChannelResidual) + 3 * lastMeanValues[0] - 3 * lastMeanValues[1] + lastMeanValues[2];
                        predictedDiffValue = wavQuant->quantize_value(sideChannelResidual) + 3 * lastDiffValues[0] - 3 * lastDiffValues[1] + lastDiffValues[2];
                    }

                    if (predictedDiffValue % 2 == 1) {
                        predictedMeanValue += 0.5;
                    }
                }

                // Update the last three mean values and difference values
                lastMeanValues[2] = lastMeanValues[1];
                lastMeanValues[1] = lastMeanValues[0];
                if(!wavQuant)
                    lastMeanValues[0] = meanValue;
                else
                    lastMeanValues[0] = predictedMeanValue;

                lastDiffValues[2] = lastDiffValues[1];
                lastDiffValues[1] = lastDiffValues[0];
                if(!wavQuant)
                    lastDiffValues[0] = diffValue;
                else
                    lastDiffValues[0] = predictedDiffValue;

                if (numSamples % window_size == 0) {
                    // Update golombCodes
                    for (int predictor = 0; predictor < 4; predictor++) {
                        // Side Channel
                        side_golomb_m_parameter[predictor] = optimizeGolombParameter(side_sumResiduals[predictor], window_size);
                        if (golombCodes.count(side_golomb_m_parameter[predictor])) {
                            // Get the golombCode from the map
                            currentSideGolombCodes[predictor] = golombCodes.find(side_golomb_m_parameter[predictor])->second;
                        } else {
                            // Create golombCode
                            currentSideGolombCodes[predictor] = GolombCode(side_golomb_m_parameter[predictor]);
                            golombCodes.insert({side_golomb_m_parameter[predictor], currentSideGolombCodes[predictor]});
                        }

                        // Mid Channel
                        mid_golomb_m_parameter[predictor] = optimizeGolombParameter(mid_sumResiduals[predictor], window_size);
                        if (golombCodes.count(mid_golomb_m_parameter[predictor])) {
                            // Get the golombCode from the map
                            currentMidGolombCodes[predictor] = golombCodes.find(mid_golomb_m_parameter[predictor])->second;
                        } else {
                            // Create golombCode
                            currentMidGolombCodes[predictor] = GolombCode(mid_golomb_m_parameter[predictor]);
                            golombCodes.insert({mid_golomb_m_parameter[predictor], currentMidGolombCodes[predictor]});
                        }

                        // Reset sum of residuals
                        side_sumResiduals[predictor] = 0;
                        mid_sumResiduals[predictor] = 0;
                    }
                }
            }
            
            
        }


        // If the predictor type is the one that tries all the predictors and choose the best one
        if (predictor_type == 4) {
            // Choose the best predictor based on the size of the encoded string
            unsigned int smallestSize = encoded_residuals_array[0].length();
            int bestPredictor = 0;
            for (int i = 1; i<4; i++) {
                if ( encoded_residuals_array[i].length() < smallestSize) {
                    smallestSize = encoded_residuals_array[i].length();
                    bestPredictor = i;
                }
            }

            // Write the best predictor value
            bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(30, 32));
            // Write the encoded string of the best predictor
            bitStream.write_n_bits(encoded_residuals_array[bestPredictor]);

            // Reset the encoded string by starting with the first golomb m parameter and the current sum of residuals for both channels
            for (int predictor=0; predictor<4; predictor++) {
                encoded_residuals_array[predictor] = std::bitset<32>(side_golomb_m_parameter[predictor]).to_string();
                encoded_residuals_array[predictor] += std::bitset<32>(side_sumResiduals[predictor]).to_string();
                encoded_residuals_array[predictor] += std::bitset<32>(mid_golomb_m_parameter[predictor]).to_string();
                encoded_residuals_array[predictor] += std::bitset<32>(mid_sumResiduals[predictor]).to_string();
            }
        }
    }

}


int main(int argc,const char** argv) {

    // Default values
    int predictor_type = 4;
    int quantize_bits = 0;
    int window_size = 50;

    if(argc < 3) {
		cerr << "Usage: ./audio_encoder [ -p predictor (def 4) ]\n";
		cerr << "                       [ -w window size (def 50) ]\n";
        cerr << "                       [ -q num_bits ]\n";
        cerr << "                      input_file output_file\n";
		cerr << "Example: ./audio_encoder -p 0 -w 10 ../audio-examples/sample01.wav compressed.bin\n";
		return 1;
	}

    // Open input file
	SndfileHandle sndFile { argv[argc-2] };
	if(sndFile.error()) {
		cerr << "Error: invalid input file\n";
		return 1;
    }

    // Check if it is Wav format
	if((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

    // Check if it it PCM 16
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

                if(quantize_bits < 1 || quantize_bits > 15) {
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

