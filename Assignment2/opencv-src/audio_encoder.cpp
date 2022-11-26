#include <iostream>
#include <sndfile.hh>
#include "GolombCode.h"
#include "BitStream.h"
#include "wav_quant.h"
#include "map"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames


string encodeResidual(GolombCode golombCode, WAVQuant* wavQuant, int residual) {
    if (wavQuant != nullptr)
        return golombCode.encode(wavQuant->quantize(residual));
    else
        return golombCode.encode(residual);
}

uint optimizeGolombParameter(double sumSamples, double numSamples) {
    cout << "sumSamples: " << sumSamples << endl;
    cout << "numSamples: " << numSamples << endl;
    double alfa = (sumSamples/numSamples) / ( (sumSamples/numSamples) + 1);
    return ceil( -1/log2(alfa) ) ;
}

void encodeMonoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, WAVQuant* wavQuant) {

    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    double sumSamples_array[] = {0, 0, 0, 0};
    double numSamples = 0;
    uint golomb_m_parameter_array[] = {100, 100, 100, 100};       // Initial m = 100

    int residual = 0;
    string encoded_residuals_array[] = {"", "", "", ""};
    int last_residuals[] = {0, 0, 0};
    GolombCode golombCode {100};

	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());

        for (auto it = samples.begin(); it != samples.end(); ++it) {
            int index = std::distance(samples.begin(), it);

            cout << "sample: " << index << endl;
            
            numSamples += 1;
            if (predictor_type == 0) {
                residual = samples[index];
            } else if (predictor_type == 1) {
                residual = samples[index] - last_residuals[0];
            } else if (predictor_type == 2) {
                residual = samples[index] - 2 * last_residuals[0] - last_residuals[1];
            } else if (predictor_type == 3) {
                residual = samples[index] - 3 * last_residuals[0] - 3 * last_residuals[1] + last_residuals[2];
            } else if (predictor_type == 4) {
                int residuals[4] = {samples[index],
                                    samples[index] - last_residuals[0],
                                    samples[index] - 2 * last_residuals[0] - last_residuals[1],
                                    samples[index] - 3 * last_residuals[0] - 3 * last_residuals[1] + last_residuals[2]};

                for (int predictor = 0; predictor < 4; predictor++) {
                    // Create golombCode
                    golombCode = GolombCode(golomb_m_parameter_array[predictor]);
                    // Encode and append to encodedstring
                    encoded_residuals_array[predictor] += encodeResidual(golombCode, wavQuant, residuals[predictor]);
                    sumSamples_array[predictor] += residuals[predictor];
                    // Otimize m parameter for next sample
                    golomb_m_parameter_array[predictor] = optimizeGolombParameter(sumSamples_array[predictor], numSamples);
                }
            }

            cout << "sai 1 if " << endl;
            
            if (predictor_type != 4) {
                cout << "golomb_m:  " << golomb_m_parameter_array[predictor_type] << endl;
                // Create golombCode
                golombCode = GolombCode(golomb_m_parameter_array[predictor_type]);
                cout << "criei codigo" << endl;
                // Encode and write
                string encoded_residual = encodeResidual(golombCode, wavQuant, residual);
                cout << "dei encode " << endl;
                bitStream.write_n_bits(encoded_residual);
                cout << "escrevi " << endl;
                sumSamples_array[predictor_type] += residual;
                cout << "vou otimizar next " << endl;
                // Otimize m parameter for next sample
                golomb_m_parameter_array[predictor_type] = optimizeGolombParameter(sumSamples_array[predictor_type], numSamples);
            }

            cout << "sai 2 if " << endl;

            last_residuals[2] = last_residuals[1];      // index - 3
            last_residuals[1] = last_residuals[0];      // index - 2
            last_residuals[0] = samples[index];         // index - 1   
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





void encodeStereoAudio(SndfileHandle sndFile, int predictor_type, BitStream &bitStream, WAVQuant* wavQuant) {
    
    size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    double mid_sumSamples_array[] = {0, 0, 0, 0};
    double side_sumSamples_array[] = {0, 0, 0, 0};
    double numSamples = 0;
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
                        golombCode = GolombCode(mid_golomb_m_parameter_array[predictor]);
                        // Encode and append to encodedstring
                        encoded_residuals_array[predictor] += encodeResidual(golombCode, wavQuant, midChannelResiduals[predictor]);
                        mid_sumSamples_array[predictor] += midChannelResiduals[predictor];
                        // Otimize m parameter for next sample
                        mid_golomb_m_parameter_array[predictor] = optimizeGolombParameter(mid_sumSamples_array[predictor], numSamples);

                        // Create golombCode
                        golombCode = GolombCode(side_golomb_m_parameter_array[predictor]);
                        // Encode and append to encodedstring
                        encoded_residuals_array[predictor] += encodeResidual(golombCode, wavQuant, sideChannelResiduals[predictor]);
                        side_sumSamples_array[predictor] += sideChannelResiduals[predictor];
                        // Otimize m parameter for next sample
                        side_golomb_m_parameter_array[predictor] = optimizeGolombParameter(side_sumSamples_array[predictor], numSamples);
                    }

                }
                /*
                if (wavQuant != nullptr){
                    encoded_mid_channel_residual = golombCode.encode( wavQuant->quantize(midChannelResidual));
                    encoded_side_channel_residual = golombCode.encode( wavQuant->quantize(sideChannelResidual));
                }
                else {
                    encoded_mid_channel_residual = golombCode.encode(midChannelResidual);
                    encoded_side_channel_residual = golombCode.encode(sideChannelResidual);
                }*/
                
                if (predictor_type != 4) {
                    // Create golombCode
                    golombCode = GolombCode(mid_golomb_m_parameter_array[predictor_type]);
                    // Encode and write
                    string mid_encoded_residual = encodeResidual(golombCode, wavQuant, midChannelResidual);
                    bitStream.write_n_bits(mid_encoded_residual);
                    mid_sumSamples_array[predictor_type] += midChannelResidual;
                    // Otimize m parameter for next sample
                    mid_golomb_m_parameter_array[predictor_type] = optimizeGolombParameter(mid_sumSamples_array[predictor_type], numSamples);

                    // Create golombCode
                    golombCode = GolombCode(side_golomb_m_parameter_array[predictor_type]);
                    // Encode and write
                    string side_encoded_residual = encodeResidual(golombCode, wavQuant, sideChannelResidual);
                    bitStream.write_n_bits(side_encoded_residual);
                    side_sumSamples_array[predictor_type] += sideChannelResidual;
                    // Otimize m parameter for next sample
                    side_golomb_m_parameter_array[predictor_type] = optimizeGolombParameter(side_sumSamples_array[predictor_type], numSamples);
                }

                lastMeanValues[2] = lastMeanValues[1];
                lastMeanValues[1] = lastMeanValues[0];
                lastMeanValues[0] = meanValue;
                lastDiffValues[2] = lastDiffValues[1];
                lastDiffValues[1] = lastDiffValues[0];
                lastDiffValues[0] = diffValue;

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
    int golomb_m_parameter = 6;
    int quantize_bits = 0;

    if(argc < 3) {
		cerr << "Usage: ./audio_codec [ -p predictor (def 0) ]\n";
		cerr << "                      input_file output_file\n";
		cerr << "Example: ./audio_codec -p 0 sample.wav compressed.wav\n";
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
    //GolombCode golombCode {golomb_m_parameter};
    BitStream bitStream { argv[argc-1], "w" };

    if (quantize_bits > 0){
        wavQuant = new WAVQuant(quantize_bits, 1);
    }
    
    // Write golomb_m_parameter to coded file
	//bitStream.write_n_bits(std::bitset<32>(golomb_m_parameter).to_string());
    // Write wavFileInput format to coded file
    //string encoded_wavFileInputFormat = golombCode.encode(sndFile.format());
    //bitStream.write_n_bits(encoded_wavFileInputFormat);
    bitStream.write_n_bits(std::bitset<32>(sndFile.format()).to_string());
	// Write wavFileInput channels to coded file
    // string encoded_wavFileInputChannels = golombCode.encode(sndFile.channels());
    // bitStream.write_n_bits(encoded_wavFileInputChannels);
    bitStream.write_n_bits(std::bitset<32>(sndFile.channels()).to_string());
	// Write wavFileInput frames to coded file
    // string encoded_wavFileInputFrames = golombCode.encode(sndFile.frames());
    // bitStream.write_n_bits(encoded_wavFileInputFrames);
    bitStream.write_n_bits(std::bitset<32>(sndFile.frames()).to_string());
	// Write wavFileInput sampleRate to coded file
    // string encoded_wavFileInputSampleRate = golombCode.encode(sndFile.samplerate());
    // bitStream.write_n_bits(encoded_wavFileInputSampleRate);
    bitStream.write_n_bits(std::bitset<32>(sndFile.samplerate()).to_string());
    // Write block size to coded file
    // string encoded_bufferSize = golombCode.encode(FRAMES_BUFFER_SIZE);
    // bitStream.write_n_bits(encoded_bufferSize);
    bitStream.write_n_bits(std::bitset<32>(FRAMES_BUFFER_SIZE).to_string());
    // Write predictor_type to coded file
    // string encoded_predictor_type = golombCode.encode(predictor_type);
    // bitStream.write_n_bits(encoded_predictor_type);
    bitStream.write_n_bits(std::bitset<32>(predictor_type).to_string());

    if (sndFile.channels() == 1) encodeMonoAudio(sndFile, predictor_type, bitStream, wavQuant);
    else if (sndFile.channels() == 2) encodeStereoAudio(sndFile, predictor_type, bitStream, wavQuant);

    bitStream.close();

    return 0;
}

