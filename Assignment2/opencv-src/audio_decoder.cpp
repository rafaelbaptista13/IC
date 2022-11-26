#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"
#include "map"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames
map<string, int> binaryToInt = {{"00", 0}, {"01", 1}, {"10", 2}, {"11", 3}};

vector<short> decodeMonoAudio(vector<short> samples, int predictor_type, int num_of_elements, BitStream &bitStream, int blockSize) {

    double numSamples = 0;
    double sumSamples = 0;
    double alfa = 0;
    uint golomb_m_parameter = 100;       // Initial m = 100

    int selectedPredictor;

    int residual;
    int lastSamples[] = {0,0,0};

    while (numSamples < num_of_elements) {

        if (predictor_type == 4)
            selectedPredictor = binaryToInt[bitStream.get_n_bits(2)];
        else
            selectedPredictor = predictor_type;

        if (numSamples + blockSize > num_of_elements) {
            blockSize = num_of_elements - numSamples;
        }

        for (int i=0; i < blockSize; i++) {
            
            // Create golombCode
            GolombCode golombCode {golomb_m_parameter};

            // Get residual
            residual = golombCode.decodeWithBitstream(bitStream);

            // Convert to original
            if (selectedPredictor == 0) {
                samples[numSamples] = residual;
            } else if (selectedPredictor == 1) {
                samples[numSamples] = lastSamples[0] + residual;
            } else if (selectedPredictor == 2) {
                samples[numSamples] = residual + (2*lastSamples[0]) + lastSamples[1];
            } else {  
                samples[numSamples] = residual + 3 * lastSamples[0] + 3 * lastSamples[1] - lastSamples[2];
            }

            lastSamples[2] = lastSamples[1];
            lastSamples[1] = lastSamples[0];
            lastSamples[0] = samples[numSamples];
            
            sumSamples += residual;
            numSamples += 1;

            alfa = (sumSamples/numSamples) / ( (sumSamples/numSamples) + 1);
            golomb_m_parameter = ceil( -1/log2(alfa) );
        }
    }

    return samples;

}


vector<short> decodeStereoAudio(vector<short> samples, int predictor_type, int num_of_elements, BitStream &bitStream, int blockSize) {

    double mid_sumSamples = 0;
    double side_sumSamples = 0;
    double numSamples = 0;
    double mid_alfa = 0;
    double side_alfa = 0;
    uint mid_golomb_m_parameter = 100;          // Initial m = 100
    uint side_golomb_m_parameter = 100;         // Initial m = 100

    int elementsRead = 0;
    int selectedPredictor;
    
    int mid_val;
    int mid_residual;

    int side_val;
    int side_residual;
    
    int lastMeanValues[] = {0,0,0};
    int lastDiffValues[] = {0,0,0};

    while (elementsRead < num_of_elements) {

        if (predictor_type == 4)
            selectedPredictor = binaryToInt[bitStream.get_n_bits(2)];
        else
            selectedPredictor = predictor_type;

        if (elementsRead + blockSize > num_of_elements) {
            blockSize = num_of_elements - elementsRead;
        }
    
        for (int i=0; i < blockSize; i+=2) {

            // Create golombCodes
            GolombCode mid_golombCode {mid_golomb_m_parameter};
            GolombCode side_golombCode {side_golomb_m_parameter};

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

            mid_sumSamples += mid_residual;
            side_sumSamples += side_residual;
            numSamples += 1;

            mid_alfa = (mid_sumSamples/numSamples) / ( (mid_sumSamples/numSamples) + 1);
            side_alfa = (side_sumSamples/numSamples) / ( (side_sumSamples/numSamples) + 1);
            mid_golomb_m_parameter = ceil( -1/log2(mid_alfa) );
            side_golomb_m_parameter = ceil( -1/log2(side_alfa) );
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
    // Get parameter m of Golomb Encoding
    //std::string m_str = bitStreamRead.get_n_bits(32);
	//int m = (int32_t) std::bitset<32>(m_str).to_ulong();

    // Create Golomb codec
    //GolombCode golombCode {m};

	// Get wavFileInput format
	//int format = golombCode.decodeWithBitstream(bitStreamRead);
    std::string format_str = bitStreamRead.get_n_bits(32);
	int format = (int32_t) std::bitset<32>(format_str).to_ulong();
	// Get wavFileInput channels
	//int channels = golombCode.decodeWithBitstream(bitStreamRead);
    std::string channels_str = bitStreamRead.get_n_bits(32);
	int channels = (int32_t) std::bitset<32>(channels_str).to_ulong();
	// Get wavFileInput frames
	//int frames = golombCode.decodeWithBitstream(bitStreamRead);
    std::string frames_str = bitStreamRead.get_n_bits(32);
	int frames = (int32_t) std::bitset<32>(frames_str).to_ulong();
	// Get wavFileInput sampleRate
	//int sample_rate = golombCode.decodeWithBitstream(bitStreamRead);
    std::string sample_rate_str = bitStreamRead.get_n_bits(32);
	int sample_rate = (int32_t) std::bitset<32>(sample_rate_str).to_ulong();
    // Get block size
	//int blockSize = golombCode.decodeWithBitstream(bitStreamRead);
    std::string blockSize_str = bitStreamRead.get_n_bits(32);
	int blockSize = (int32_t) std::bitset<32>(blockSize_str).to_ulong();
    // Get predictor type
    //int predictor_type = golombCode.decodeWithBitstream(bitStreamRead);
    std::string predictor_type_str = bitStreamRead.get_n_bits(32);
	int predictor_type = (int32_t) std::bitset<32>(predictor_type_str).to_ulong();

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, format,
	  channels, sample_rate };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

	vector<short> samples(channels * frames);

    if (channels == 1) samples = decodeMonoAudio(samples, predictor_type, frames * channels, bitStreamRead, blockSize * channels);
    else if (channels == 2) samples = decodeStereoAudio(samples, predictor_type, frames * channels, bitStreamRead, blockSize * channels);

    sfhOut.writef(samples.data(), channels * frames);

    return 0;
}

