#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

vector<short> decodePredictor0MonoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    for (int i=0; i < num_of_elements; i++) {
        // Get residual
        int residual = golombCode.decodeWithBitstream(bitStreamRead);

        // Convert to original
        samples[i] = residual;
    }
    return samples;
}

vector<short> decodePredictor0StereoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    for (int i=0; i < num_of_elements; i+=2) {
        
        // Get residuals
        int mid_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int side_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int mid_val = mid_residual;
        int side_val = side_residual;

        // Convert to original
        samples[i] = mid_val + side_val;
        samples[i+1] = mid_val - side_val;
    }
    return samples;
}

vector<short> decodePredictor1MonoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    samples[0] = golombCode.decodeWithBitstream(bitStreamRead);
    for (int i=1; i<num_of_elements; i++) {
        // Get residual
        int residual = golombCode.decodeWithBitstream(bitStreamRead);

        // Convert to original
        samples[i] = samples[i-1] + residual;
    }
    return samples;
}

vector<short> decodePredictor1StereoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    int lastMeanValue = 0;
    int lastDiffValue = 0;
    for (int i=0; i < num_of_elements; i+=2) {
        // Get residuals
        int mid_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int side_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int mid_val = mid_residual + lastMeanValue;
        int side_val = side_residual + lastDiffValue;

        lastMeanValue = mid_val;
        lastDiffValue = side_val;

        // Convert to original
        samples[i] = mid_val + side_val;
        samples[i+1] = mid_val - side_val;
    }
    return samples;
}

vector<short> decodePredictor2MonoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    samples[0] = golombCode.decodeWithBitstream(bitStreamRead);
    samples[1] = 2 * samples[0] + golombCode.decodeWithBitstream(bitStreamRead);
	for (int i=2; i<num_of_elements; i++) {
        // Get residual
        int residual = golombCode.decodeWithBitstream(bitStreamRead);

        // Convert to original
        samples[i] = residual + (2*samples[i-1]) + samples[i-2];
    }
    return samples;
}

vector<short> decodePredictor2StereoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    int lastMeanValues[] = {0,0};
    int lastDiffValues[] = {0,0};
    for (int i=0; i < num_of_elements; i+=2) {
        // Get residuals
        int mid_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int side_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int mid_val = mid_residual + (2 * lastMeanValues[0]) + lastMeanValues[1];
        int side_val = side_residual + (2 * lastDiffValues[0]) + lastDiffValues[1];

        lastMeanValues[1] = lastMeanValues[0];
        lastMeanValues[0] = mid_val;
        lastDiffValues[1] = lastDiffValues[0];
        lastDiffValues[0] = side_val;

        // Convert to original
        samples[i] = mid_val + side_val;
        samples[i+1] = mid_val - side_val;
    }
    return samples;
}

vector<short> decodePredictor3MonoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    samples[0] = golombCode.decodeWithBitstream(bitStreamRead);
    samples[1] = 3 * samples[0] + golombCode.decodeWithBitstream(bitStreamRead);
    samples[2] = 3 * samples[0] + 3 * samples[1] + golombCode.decodeWithBitstream(bitStreamRead);
    for (int i=3; i < num_of_elements; i++) {
        // Get residual
        int residual = golombCode.decodeWithBitstream(bitStreamRead);

        // Convert to original
        samples[i] = residual + 3 * samples[i-1] + 3 * samples[i-2] - samples[i-3];
    }
    return samples;
}

vector<short> decodePredictor3StereoChannel(vector<short> samples, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {
    int lastMeanValues[] = {0,0,0};
    int lastDiffValues[] = {0,0,0};
    for (int i=0; i < num_of_elements; i+=2) {
        // Get residuals
        int mid_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int side_residual = golombCode.decodeWithBitstream(bitStreamRead);
        int mid_val = mid_residual + (3 * lastMeanValues[0]) + (3 * lastMeanValues[1]) - lastMeanValues[2];
        int side_val = side_residual + (3 * lastDiffValues[0]) + (3 * lastDiffValues[1]) - lastDiffValues[2];

        lastMeanValues[2] = lastMeanValues[1];
        lastMeanValues[1] = lastMeanValues[0];
        lastMeanValues[0] = mid_val;
        lastDiffValues[2] = lastDiffValues[1];
        lastDiffValues[1] = lastDiffValues[0];
        lastDiffValues[0] = side_val;

        // Convert to original
        samples[i] = mid_val + side_val;
        samples[i+1] = mid_val - side_val;
    }
    return samples;
}

vector<short> decodeMonoAudio(vector<short> samples, int predictor_type, GolombCode golombCode, int num_of_elements, BitStream &bitStreamRead) {

    if (predictor_type == 0) {
        return decodePredictor0MonoChannel(samples, golombCode, num_of_elements, bitStreamRead);
    } else if (predictor_type == 1) {
		return decodePredictor1MonoChannel(samples, golombCode, num_of_elements, bitStreamRead);
    } else if (predictor_type == 2) {
		return decodePredictor2MonoChannel(samples, golombCode, num_of_elements, bitStreamRead);
    } else {
		return decodePredictor3MonoChannel(samples, golombCode, num_of_elements, bitStreamRead);
    }
    
}


vector<short> decodeStereoAudio(vector<short> samples, int predictor_type, GolombCode golombCode, int num_of_elements, BitStream &bitStream) {
    if (predictor_type == 0) {
        return decodePredictor0StereoChannel(samples, golombCode, num_of_elements, bitStream);
    } else if (predictor_type == 1) {
		return decodePredictor1StereoChannel(samples, golombCode, num_of_elements, bitStream);
    } else if (predictor_type == 2) {
		return decodePredictor2StereoChannel(samples, golombCode, num_of_elements, bitStream);
    } else {
		return decodePredictor3StereoChannel(samples, golombCode, num_of_elements, bitStream);
    }
}


int main(int argc,const char** argv) {

    if(argc < 3) {
		cerr << "Usage: ./audio_decoder input_file output_file\n";
		cerr << "Example: ./audio_decoder -p 3 sample.wav\n";
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
