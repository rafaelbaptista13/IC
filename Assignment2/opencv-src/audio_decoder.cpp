#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"
#include "map"
#include "wav_quant.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames
map<string, int> binaryToInt = {{"00", 0}, {"01", 1}, {"10", 2}, {"11", 3}};

vector<short> decodeMonoAudio(vector<short> samples, int predictor_type, int num_of_elements, BitStream &bitStream, int blockSize, int window_size, WAVQuant* wavQuant) {

    // Number of samples read
    int numSamples = 0;
    // Sum of Residuals values
    long int sumResiduals = 0;
    // Alfa value for optimization of m parameter
    double alfa = 0;
    // Current m parameter
    int golomb_m_parameter = 100;       // Initial m = 100

    // Current Predictor
    int selectedPredictor;

    // Residual value
    int residual;
    // Array to store the last three values in order to calculate the samples
    int lastSamples[] = {0,0,0};

    // Initial Golomb Code
    GolombCode golombCode {golomb_m_parameter};
    // Map to store the used Golomb Codes
    map<int, GolombCode> golombCodes = {{100, golombCode}};

    // Iterate over all samples
    while (numSamples < num_of_elements) {
        
        // If the predictor type is the one that tries all the predictors and choose the best one
        if (predictor_type == 4) {
            // Read the predictor for the current block 
            selectedPredictor = binaryToInt[bitStream.get_n_bits(2)];
            // Read the first golomb m parameter of the block
            golomb_m_parameter = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
            if (golombCodes.count(golomb_m_parameter)) {
                // Get the golombCode from the map
                golombCode = golombCodes.find(golomb_m_parameter)->second;
            } else {
                // Create golombCode
                golombCode =  GolombCode(golomb_m_parameter);
                golombCodes.insert({golomb_m_parameter, golombCode});
            }
            // Update the sum of Residuals of the current block
            sumResiduals = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
        }
        else {
            // Update the selected predictor
            selectedPredictor = predictor_type;
        }

        // Calculate the current block size
        if (numSamples + blockSize > num_of_elements) {
            blockSize = num_of_elements - numSamples;
        }

        // Iterate over all samples in the block
        for (int i=0; i < blockSize; i++) {
            // Get residual
            residual = golombCode.decodeWithBitstream(bitStream);

            // Decode quantitized residual
            if (wavQuant)
                residual = wavQuant->decodeQuantized(residual);

            // Convert to original
            if (selectedPredictor == 0) {
                samples[numSamples] = residual;
            } else if (selectedPredictor == 1) {
                samples[numSamples] = lastSamples[0] + residual;
            } else if (selectedPredictor == 2) {
                samples[numSamples] = residual + (2*lastSamples[0]) - lastSamples[1];
            } else {
                samples[numSamples] = residual + 3 * lastSamples[0] - 3 * lastSamples[1] + lastSamples[2];
            }

            // Update the last three samples
            lastSamples[2] = lastSamples[1];
            lastSamples[1] = lastSamples[0];
            lastSamples[0] = samples[numSamples];
            
            // Increment sum of residuals and num of samples
            sumResiduals += abs(residual);
            numSamples += 1;

            if (numSamples % window_size == 0) {
                // Update Golomb Code
                double mean = (double) sumResiduals/ (double) window_size;
                alfa = mean / (mean + 1);
                golomb_m_parameter = ceil( -1/log2(alfa) );
                if (golombCodes.count(golomb_m_parameter)) {
                    // Get the golombCode from the map
                    golombCode = golombCodes.find(golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    golombCode =  GolombCode(golomb_m_parameter);
                    golombCodes.insert({golomb_m_parameter, golombCode});
                }
                // Reset sum of residuals
                sumResiduals = 0;
            }
        }
    }

    return samples;

}


vector<short> decodeStereoAudio(vector<short> samples, int predictor_type, int num_of_elements, BitStream &bitStream, int blockSize, int window_size, WAVQuant* wavQuant) {

    // Sum of Residuals values of MID channel
    int mid_sumResiduals = 0;
    // Sum of Residuals values of SIDE channel
    int side_sumResiduals = 0;
    // Number of samples read
    int numSamples = 0;
    // Alfa value for optimization of m parameter of MID channel
    double mid_alfa = 0;
    // Alfa value for optimization of m parameter of SIDE channel
    double side_alfa = 0;
    // Current M parameter of MID channel
    int mid_golomb_m_parameter = 100;          // Initial m = 
    // Current M parameter of SIDE channel
    int side_golomb_m_parameter = 100;         // Initial m = 100

    // Number of element read
    int elementsRead = 0;
    // Current Predictor
    int selectedPredictor;
    // Mid value
    int mid_val;
    // Residual of Mid channel
    int mid_residual;
    // Side value
    int side_val;
    // Residual of Side channel
    int side_residual;
    
    // Array to store the last three values of mean in order to calculate the original values
    int lastMeanValues[] = {0,0,0};
    // Array to store the last three values of difference in order to calculate the original values
    int lastDiffValues[] = {0,0,0};

    // Initial Golomb Code of MID channel
    GolombCode mid_golombCode {mid_golomb_m_parameter};
    // Initial Golomb Code of SIDE channel
    GolombCode side_golombCode {side_golomb_m_parameter};
    // Map to store the used Golomb Codes
    map<int, GolombCode> golombCodes = {{100, mid_golombCode}};

    // Iterate over all elements
    while (elementsRead < num_of_elements) {

        // If the predictor type is the one that tries all the predictors and choose the best one
        if (predictor_type == 4) {
            // Read the predictor for the current block 
            selectedPredictor = binaryToInt[bitStream.get_n_bits(2)];
            // Read the first golomb m parameter of side channel of the block
            side_golomb_m_parameter = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
            if (golombCodes.count(side_golomb_m_parameter)) {
                // Get the golombCode from the map
                side_golombCode = golombCodes.find(side_golomb_m_parameter)->second;
            } else {
                // Create golombCode
                side_golombCode =  GolombCode(side_golomb_m_parameter);
                golombCodes.insert({side_golomb_m_parameter, side_golombCode});
            }
            // Update the sum of Residuals of the current block of SIDE
            side_sumResiduals = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();

            // Read the first golomb m parameter of mid channel of the block
            mid_golomb_m_parameter = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
            if (golombCodes.count(mid_golomb_m_parameter)) {
                // Get the golombCode from the map
                mid_golombCode = golombCodes.find(mid_golomb_m_parameter)->second;
            } else {
                // Create golombCode
                mid_golombCode =  GolombCode(mid_golomb_m_parameter);
                golombCodes.insert({mid_golomb_m_parameter, mid_golombCode});
            }
            // Update the sum of Residuals of the current block of MID
            mid_sumResiduals = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
        }
        else {
            // Update the selected predictor
            selectedPredictor = predictor_type;
        }

        // Calculate the current block size
        if (elementsRead + blockSize > num_of_elements) {
            blockSize = num_of_elements - elementsRead;
        }

        // Iterate over all samples in the block
        for (int i=0; i < blockSize; i+=2) {

            // Get residuals
            mid_residual = mid_golombCode.decodeWithBitstream(bitStream);
            side_residual = side_golombCode.decodeWithBitstream(bitStream);

            // Convert to mid and side values
            if (selectedPredictor == 0) {
                mid_val = mid_residual;
                side_val = side_residual;
            } else if (selectedPredictor == 1) {
                mid_val = mid_residual + lastMeanValues[0];
                side_val = side_residual + lastDiffValues[0];
            } else if (selectedPredictor == 2) {
                mid_val = mid_residual + (2 * lastMeanValues[0]) + lastMeanValues[1];
                side_val = side_residual + (2 * lastDiffValues[0]) + lastDiffValues[1];
            } else {
                mid_val = mid_residual + (3 * lastMeanValues[0]) + (3 * lastMeanValues[1]) - lastMeanValues[2];
                side_val = side_residual + (3 * lastDiffValues[0]) + (3 * lastDiffValues[1]) - lastDiffValues[2];
            }

            if (side_val % 2 == 1) {
                mid_val += 0.5;
            }

            // Update the last three mean and diff values
            lastMeanValues[2] = lastMeanValues[1];
            lastMeanValues[1] = lastMeanValues[0];
            lastMeanValues[0] = mid_val;
            lastDiffValues[2] = lastDiffValues[1];
            lastDiffValues[1] = lastDiffValues[0];
            lastDiffValues[0] = side_val;

            // Convert to original
            samples[elementsRead] = (2*mid_val - side_val)/2;
            samples[elementsRead+1] = side_val + samples[elementsRead];
            elementsRead+=2;

            // Increment sum of residuals for both channels and num of samples
            mid_sumResiduals += abs(mid_residual);
            side_sumResiduals += abs(side_residual);
            numSamples += 1;
            
            if (numSamples % window_size == 0) {
                // Update Golomb Code for mid
                double mid_mean = (double) mid_sumResiduals/ (double) window_size;
                mid_alfa = mid_mean / ( mid_mean + 1);
                mid_golomb_m_parameter = ceil( -1/log2(mid_alfa) );
                if (golombCodes.count(mid_golomb_m_parameter)) {
                    // Get the golombCode from the map
                    mid_golombCode = golombCodes.find(mid_golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    mid_golombCode =  GolombCode(mid_golomb_m_parameter);
                    golombCodes.insert({mid_golomb_m_parameter, mid_golombCode});
                }
                // Reset sum of residuals in mid channel
                mid_sumResiduals = 0;

                // Update Golomb Code for side
                double side_mean = (double) side_sumResiduals/ (double) window_size;
                side_alfa = side_mean / ( side_mean + 1);
                side_golomb_m_parameter = ceil( -1/log2(side_alfa) );
                if (golombCodes.count(side_golomb_m_parameter)) {
                    // Get the golombCode from the map
                    side_golombCode = golombCodes.find(side_golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    side_golombCode =  GolombCode(side_golomb_m_parameter);
                    golombCodes.insert({side_golomb_m_parameter, side_golombCode});
                }
                // Reset sum of residuals in side channel
                side_sumResiduals = 0;
            }
        }
    }

    return samples;
        
}



int main(int argc,const char** argv) {

    if(argc < 3) {
		cerr << "Usage: ./audio_decoder input_file output_file\n";
		cerr << "Example: ./audio_decoder compressed.bin output.wav\n";
		return 1;
	}

    // Check if file exists
    if (FILE *file = fopen(argv[argc-2], "r")) {
        fclose(file);
    } else {
        cerr << "Error: invalid input file\n";
        return 1;
    }

	// BitStream to read coded file
	BitStream bitStreamRead { argv[argc-2], "r" };

	// Get wavFileInput format
    std::string format_str = bitStreamRead.get_n_bits(32);
	int format = (int32_t) std::bitset<32>(format_str).to_ulong();
	// Get wavFileInput channels
    std::string channels_str = bitStreamRead.get_n_bits(32);
	int channels = (int32_t) std::bitset<32>(channels_str).to_ulong();
	// Get wavFileInput frames
    std::string frames_str = bitStreamRead.get_n_bits(32);
	int frames = (int32_t) std::bitset<32>(frames_str).to_ulong();
	// Get wavFileInput sampleRate
    std::string sample_rate_str = bitStreamRead.get_n_bits(32);
	int sample_rate = (int32_t) std::bitset<32>(sample_rate_str).to_ulong();
    // Get block size
    std::string blockSize_str = bitStreamRead.get_n_bits(32);
	int blockSize = (int32_t) std::bitset<32>(blockSize_str).to_ulong();
    // Get predictor type
    std::string predictor_type_str = bitStreamRead.get_n_bits(32);
	int predictor_type = (int32_t) std::bitset<32>(predictor_type_str).to_ulong();
    // Get window size
    std::string window_size_str = bitStreamRead.get_n_bits(32);
	int window_size = (int32_t) std::bitset<32>(window_size_str).to_ulong();
    // Get quantize_bits
    std::string quantize_bits_str = bitStreamRead.get_n_bits(32);
	int quantize_bits = (int32_t) std::bitset<32>(quantize_bits_str).to_ulong();

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, format,
	  channels, sample_rate };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

    WAVQuant* wavQuant = nullptr;
    if (quantize_bits > 0){
        wavQuant = new WAVQuant(quantize_bits, 1);
    }

	vector<short> samples(channels * frames);

    if (channels == 1) samples = decodeMonoAudio(samples, predictor_type, frames * channels, bitStreamRead, blockSize * channels, window_size, wavQuant);
    else if (channels == 2) samples = decodeStereoAudio(samples, predictor_type, frames * channels, bitStreamRead, blockSize * channels, window_size, wavQuant);

    sfhOut.writef(samples.data(), channels * frames);

    return 0;
}

