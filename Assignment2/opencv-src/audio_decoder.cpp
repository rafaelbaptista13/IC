#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

vector<short> decodeMonoAudio(vector<short> samples, int predictor_type, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {

    int residual;
    int lastSamples[] = {0,0,0};
    
    for (int i=0; i < num_of_elements; i++) {
        // Get residual
        residual = golombCode.decodeWithBitstream(bitStreamRead);

        // Convert to original
        if (predictor_type == 0) {
            samples[i] = residual;
        } else if (predictor_type == 1) {
            samples[i] = lastSamples[0] + residual;
        } else if (predictor_type == 2) {
            samples[i] = residual + (2*lastSamples[0]) + lastSamples[1];
        } else {  
            samples[i] = residual + 3 * lastSamples[0] + 3 * lastSamples[1] - lastSamples[2];
        }

        lastSamples[2] = lastSamples[1];
        lastSamples[1] = lastSamples[0];
        lastSamples[0] = samples[i];
    }
    return samples;

}


vector<short> decodeStereoAudio(vector<short> samples, int predictor_type, GolombCode golombCode, int num_of_elements, BitStream &bitStream) {

    int mid_val;
    int mid_residual;

    int side_val;
    int side_residual;
    
    int lastMeanValues[] = {0,0,0};
    int lastDiffValues[] = {0,0,0};
    for (int i=0; i < num_of_elements; i+=2) {
        // Get residuals
        mid_residual = golombCode.decodeWithBitstream(bitStream);
        side_residual = golombCode.decodeWithBitstream(bitStream);

        if (predictor_type == 0) {
            mid_val = mid_residual;
            side_val = side_residual;
        } else if (predictor_type == 1) {
            mid_val = mid_residual + lastMeanValues[0];
            side_val = side_residual + lastDiffValues[0];
        } else if (predictor_type == 2) {
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
        samples[i] = (2*mid_val - side_val)/2;
        samples[i+1] = side_val + samples[i];
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
    std::string m_str = bitStreamRead.get_n_bits(32);
	int m = (int32_t) std::bitset<32>(m_str).to_ulong();

    // Create Golomb codec
    GolombCode golombCode {m};

	// Get wavFileInput format
	int format = golombCode.decodeWithBitstream(bitStreamRead);
	// Get wavFileInput channels
	int channels = golombCode.decodeWithBitstream(bitStreamRead);
	// Get wavFileInput frames
	int frames = golombCode.decodeWithBitstream(bitStreamRead);
	// Get wavFileInput sampleRate
	int sample_rate = golombCode.decodeWithBitstream(bitStreamRead);
    // Get predictor type
    int predictor_type = golombCode.decodeWithBitstream(bitStreamRead);

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, format,
	  channels, sample_rate };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

	vector<short> samples(channels * frames);

    if (channels == 1) samples = decodeMonoAudio(samples, predictor_type, golombCode, frames * channels, bitStreamRead);
    else if (channels == 2) samples = decodeStereoAudio(samples, predictor_type, golombCode, frames * channels, bitStreamRead);

    sfhOut.writef(samples.data(), channels * frames);

    return 0;
}

