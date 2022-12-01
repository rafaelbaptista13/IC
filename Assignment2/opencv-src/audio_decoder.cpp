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

    int numSamples = 0;
    long int sumSamples = 0;
    double alfa = 0;
    int golomb_m_parameter = 100;       // Initial m = 100

    int selectedPredictor;

    int residual;
    int lastSamples[] = {0,0,0};

    GolombCode golombCode {golomb_m_parameter};
    map<int, GolombCode> golombCodes = {{100, golombCode}};

    while (numSamples < num_of_elements) {

        if (predictor_type == 4) {
            selectedPredictor = binaryToInt[bitStream.get_n_bits(2)];
            golomb_m_parameter = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
            if (golombCodes.count(golomb_m_parameter)) {
                golombCode = golombCodes.find(golomb_m_parameter)->second;
            } else {
                // Create golombCode
                golombCode =  GolombCode(golomb_m_parameter);
                golombCodes.insert({golomb_m_parameter, golombCode});
            }
            sumSamples = (int32_t) std::bitset<32>(bitStream.get_n_bits(32)).to_ulong();
        }
        else
            selectedPredictor = predictor_type;

        if (numSamples + blockSize > num_of_elements) {
            blockSize = num_of_elements - numSamples;
        }

        for (int i=0; i < blockSize; i++) {
            // Get residual
            residual = golombCode.decodeWithBitstream(bitStream);

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
            lastSamples[2] = lastSamples[1];
            lastSamples[1] = lastSamples[0];
            lastSamples[0] = samples[numSamples];
            
            sumSamples += abs(residual);
            numSamples += 1;

            if (numSamples % window_size == 0) {
                double mean = (double) sumSamples/ (double) window_size;
                alfa = mean / (mean + 1);
                golomb_m_parameter = ceil( -1/log2(alfa) );
                if (golombCodes.count(golomb_m_parameter)) {
                    golombCode = golombCodes.find(golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    golombCode =  GolombCode(golomb_m_parameter);
                    golombCodes.insert({golomb_m_parameter, golombCode});
                }
                sumSamples = 0;
            }
        }
    }

    return samples;

}


vector<short> decodeStereoAudio(vector<short> samples, int predictor_type, int num_of_elements, BitStream &bitStream, int blockSize, int window_size, WAVQuant* wavQuant) {

    int mid_sumSamples = 0;
    int side_sumSamples = 0;
    int numSamples = 0;
    double mid_alfa = 0;
    double side_alfa = 0;
    int mid_golomb_m_parameter = 100;          // Initial m = 100
    int side_golomb_m_parameter = 100;         // Initial m = 100

    int elementsRead = 0;
    int selectedPredictor;
    
    int mid_val;
    int mid_residual;

    int side_val;
    int side_residual;
    
    int lastMeanValues[] = {0,0,0};
    int lastDiffValues[] = {0,0,0};

    GolombCode mid_golombCode {mid_golomb_m_parameter};
    GolombCode side_golombCode {side_golomb_m_parameter};
    map<int, GolombCode> golombCodes = {{100, mid_golombCode}};

    while (elementsRead < num_of_elements) {

        if (predictor_type == 4)
            selectedPredictor = binaryToInt[bitStream.get_n_bits(2)];
        else
            selectedPredictor = predictor_type;

        if (elementsRead + blockSize > num_of_elements) {
            blockSize = num_of_elements - elementsRead;
        }
    
        for (int i=0; i < blockSize; i+=2) {

            // Get residuals
            mid_residual = mid_golombCode.decodeWithBitstream(bitStream);
            side_residual = side_golombCode.decodeWithBitstream(bitStream);

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

            mid_sumSamples += abs(mid_residual);
            side_sumSamples += abs(side_residual);
            numSamples += 1;
            
            if (numSamples % window_size == 0) {
                double mid_mean = (double) mid_sumSamples/ (double) window_size;
                mid_alfa = mid_mean / ( mid_mean + 1);
                mid_golomb_m_parameter = ceil( -1/log2(mid_alfa) );
                if (golombCodes.count(mid_golomb_m_parameter)) {
                    mid_golombCode = golombCodes.find(mid_golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    mid_golombCode =  GolombCode(mid_golomb_m_parameter);
                    golombCodes.insert({mid_golomb_m_parameter, mid_golombCode});
                }
                mid_sumSamples = 0;

                double side_mean = (double) side_sumSamples/ (double) window_size;
                side_alfa = side_mean / ( side_mean + 1);
                side_golomb_m_parameter = ceil( -1/log2(side_alfa) );
                if (golombCodes.count(side_golomb_m_parameter)) {
                    side_golombCode = golombCodes.find(side_golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    side_golombCode =  GolombCode(side_golomb_m_parameter);
                    golombCodes.insert({side_golomb_m_parameter, side_golombCode});
                }
                side_sumSamples = 0;
            }
        }
    }

    return samples;
        
}



int main(int argc,const char** argv) {

    if(argc < 3) {
		cerr << "Usage: ./audio_decoder input_file output_file\n";
		cerr << "Example: ./audio_decoder compressed.wav output.wav\n";
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

