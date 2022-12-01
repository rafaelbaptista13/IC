#include <iostream>
#include "GolombCode.h"
#include "BitStream.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int optimizeGolombParameter(long int sumSamples, int numSamples) {
    double mean = (double) sumSamples/ (double) numSamples;
    double alfa = mean / (mean + 1);
    return ceil( -1/log2(alfa) ) ;
}

void encodeImage(Mat originalImg, int predictor_type, BitStream &bitStream, int window_size) {

    // Starting m values for all predictors
    int golomb_m_parameter_array[] = {16, 16, 16, 16, 16, 16, 16};   
    long int sumResiduals_array[] = {0, 0, 0, 0, 0, 0, 0};
    string encoded_residuals_array[] = {"", "", "", "", "", "", ""};
    int num_pixels = 0;

    Vec3b a = Vec3b(0,0,0);
    Vec3b b = Vec3b(0,0,0);
    Vec3b c = Vec3b(0,0,0);
    int residual[3];

    // Starting golombCode with m=16
    GolombCode golombCode {16};
    // Map to store all the golombCodes used, to prevent re-creation of golombCode objects with same m
    map<int, GolombCode> golombCodes = {{16, golombCode}};
    // Starting golombCode object for all predictors
    GolombCode currentGolombCodes[] = {golombCode, golombCode, golombCode, golombCode, golombCode, golombCode, golombCode};
       
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            num_pixels++;
            int current_pixel[3] = {originalImg.at<Vec3b>(i, j)[0], originalImg.at<Vec3b>(i, j)[1], originalImg.at<Vec3b>(i, j)[2]};

            if (predictor_type == 1) {
                residual[0] = current_pixel[0] - a[0];
                residual[1] = current_pixel[1] - a[1];
                residual[2] = current_pixel[2] - a[2]; 
            } else if (predictor_type == 2) {
                residual[0] = current_pixel[0] - b[0];
                residual[1] = current_pixel[1] - b[1];
                residual[2] = current_pixel[2] - b[2];
            } else if (predictor_type == 3) {
                residual[0] = current_pixel[0] - c[0];
                residual[1] = current_pixel[1] - c[1];
                residual[2] = current_pixel[2] - c[2];
            } else if (predictor_type == 4) {
                residual[0] = current_pixel[0] - (a[0] + b[0] - c[0]);
                residual[1] = current_pixel[1] - (a[1] + b[1] - c[1]);
                residual[2] = current_pixel[2] - (a[2] + b[2] - c[2]);
            } else if (predictor_type == 5) {
                residual[0] = current_pixel[0] - (a[0] + ((b[0] - c[0])/2));
                residual[1] = current_pixel[1] - (a[1] + ((b[1] - c[1])/2));
                residual[2] = current_pixel[2] - (a[2] + ((b[2] - c[2])/2));
            } else if (predictor_type == 6) {
                residual[0] = current_pixel[0] - (b[0] + ((a[0] - c[0])/2));
                residual[1] = current_pixel[1] - (b[1] + ((a[1] - c[1])/2));
                residual[2] = current_pixel[2] - (b[2] + ((a[2] - c[2])/2));
            } else if (predictor_type == 7) {
                residual[0] = current_pixel[0] - ((a[0] + b[0])/ 2);
                residual[1] = current_pixel[1] - ((a[1] + b[1])/ 2);
                residual[2] = current_pixel[2] - ((a[2] + b[2])/ 2);
            } else {
                // Choose the best predictor
                if (num_pixels % 2000 == 1) {
                    for (int i= 0; i < 7; i++) {
                        encoded_residuals_array[i] += std::bitset<32>(golomb_m_parameter_array[i]).to_string();
                        encoded_residuals_array[i] += std::bitset<32>(sumResiduals_array[i]).to_string();
                    }
                }

                // predictor 1
                residual[0] = current_pixel[0] - a[0];
                residual[1] = current_pixel[1] - a[1];
                residual[2] = current_pixel[2] - a[2];
                encoded_residuals_array[0] += currentGolombCodes[0].encode(residual[0]); 
                encoded_residuals_array[0] += currentGolombCodes[0].encode(residual[1]); 
                encoded_residuals_array[0] += currentGolombCodes[0].encode(residual[2]); 
                sumResiduals_array[0] += abs(residual[0]);
                sumResiduals_array[0] += abs(residual[1]);
                sumResiduals_array[0] += abs(residual[2]);
                
                // predictor 2
                residual[0] = current_pixel[0] - b[0];
                residual[1] = current_pixel[1] - b[1];
                residual[2] = current_pixel[2] - b[2];
                encoded_residuals_array[1] += currentGolombCodes[1].encode(residual[0]); 
                encoded_residuals_array[1] += currentGolombCodes[1].encode(residual[1]); 
                encoded_residuals_array[1] += currentGolombCodes[1].encode(residual[2]); 
                sumResiduals_array[1] += abs(residual[0]);
                sumResiduals_array[1] += abs(residual[1]);
                sumResiduals_array[1] += abs(residual[2]);

                // predictor 3
                residual[0] = current_pixel[0] - c[0];
                residual[1] = current_pixel[1] - c[1];
                residual[2] = current_pixel[2] - c[2];
                encoded_residuals_array[2] += currentGolombCodes[2].encode(residual[0]); 
                encoded_residuals_array[2] += currentGolombCodes[2].encode(residual[1]); 
                encoded_residuals_array[2] += currentGolombCodes[2].encode(residual[2]); 
                sumResiduals_array[2] += abs(residual[0]);
                sumResiduals_array[2] += abs(residual[1]);
                sumResiduals_array[2] += abs(residual[2]);

                // predictor 4
                residual[0] = current_pixel[0] - (a[0] + b[0] - c[0]);
                residual[1] = current_pixel[1] - (a[1] + b[1] - c[1]);
                residual[2] = current_pixel[2] - (a[2] + b[2] - c[2]);
                encoded_residuals_array[3] += currentGolombCodes[3].encode(residual[0]); 
                encoded_residuals_array[3] += currentGolombCodes[3].encode(residual[1]); 
                encoded_residuals_array[3] += currentGolombCodes[3].encode(residual[2]); 
                sumResiduals_array[3] += abs(residual[0]);
                sumResiduals_array[3] += abs(residual[1]);
                sumResiduals_array[3] += abs(residual[2]);

                // predictor 5
                residual[0] = current_pixel[0] - (a[0] + ((b[0] - c[0])/2));
                residual[1] = current_pixel[1] - (a[1] + ((b[1] - c[1])/2));
                residual[2] = current_pixel[2] - (a[2] + ((b[2] - c[2])/2));
                encoded_residuals_array[4] += currentGolombCodes[4].encode(residual[0]); 
                encoded_residuals_array[4] += currentGolombCodes[4].encode(residual[1]); 
                encoded_residuals_array[4] += currentGolombCodes[4].encode(residual[2]); 
                sumResiduals_array[4] += abs(residual[0]);
                sumResiduals_array[4] += abs(residual[1]); 
                sumResiduals_array[4] += abs(residual[2]);
                
                // predictor 6
                residual[0] = current_pixel[0] - (b[0] + ((a[0] - c[0])/2));
                residual[1] = current_pixel[1] - (b[1] + ((a[1] - c[1])/2));
                residual[2] = current_pixel[2] - (b[2] + ((a[2] - c[2])/2));
                encoded_residuals_array[5] += currentGolombCodes[5].encode(residual[0]); 
                encoded_residuals_array[5] += currentGolombCodes[5].encode(residual[1]); 
                encoded_residuals_array[5] += currentGolombCodes[5].encode(residual[2]); 
                sumResiduals_array[5] += abs(residual[0]);
                sumResiduals_array[5] += abs(residual[1]);
                sumResiduals_array[5] += abs(residual[2]);

                // predictor 7
                residual[0] = current_pixel[0] - ((a[0] + b[0])/ 2);
                residual[1] = current_pixel[1] - ((a[1] + b[1])/ 2);
                residual[2] = current_pixel[2] - ((a[2] + b[2])/ 2);
                encoded_residuals_array[6] += currentGolombCodes[6].encode(residual[0]); 
                encoded_residuals_array[6] += currentGolombCodes[6].encode(residual[1]); 
                encoded_residuals_array[6] += currentGolombCodes[6].encode(residual[2]); 
                sumResiduals_array[6] += abs(residual[0]);
                sumResiduals_array[6] += abs(residual[1]);
                sumResiduals_array[6] += abs(residual[2]);
            
            }

            if (predictor_type != 0) {
                // Write to file
                string encoded_residual_r = currentGolombCodes[predictor_type-1].encode(residual[0]);
                string encoded_residual_g = currentGolombCodes[predictor_type-1].encode(residual[1]);
                string encoded_residual_b = currentGolombCodes[predictor_type-1].encode(residual[2]);

                bitStream.write_n_bits(encoded_residual_r);
                bitStream.write_n_bits(encoded_residual_g);
                bitStream.write_n_bits(encoded_residual_b);

                // Update sum of residuals
                sumResiduals_array[predictor_type-1] += abs(residual[0]);
                sumResiduals_array[predictor_type-1] += abs(residual[1]);
                sumResiduals_array[predictor_type-1] += abs(residual[2]);
            }

            a = originalImg.at<Vec3b>(i, j);
            if (i != 0) {
                b = originalImg.at<Vec3b>(i-1, j+1);
                c = originalImg.at<Vec3b>(i-1, j);
            }

            if (num_pixels % window_size == 0) {
                // Update golombCodes
                for (int predictor = 0; predictor < 7; predictor++) {
                    golomb_m_parameter_array[predictor] = optimizeGolombParameter(sumResiduals_array[predictor], window_size);
                    if (golomb_m_parameter_array[predictor] == 0) golomb_m_parameter_array[predictor] = 1;
                    if (golombCodes.count(golomb_m_parameter_array[predictor])) {
                        currentGolombCodes[predictor] = golombCodes.find(golomb_m_parameter_array[predictor])->second;
                    } else {
                        // Create golombCode
                        currentGolombCodes[predictor] = GolombCode(golomb_m_parameter_array[predictor]);
                        golombCodes.insert({golomb_m_parameter_array[predictor], currentGolombCodes[predictor]});
                    }
                    sumResiduals_array[predictor] = 0;
                }
            }

            if (predictor_type == 0 && num_pixels % 2000 == 0) {
                // Calculate the best predictor
                unsigned int smallestSize = encoded_residuals_array[0].length();
                int bestPredictor = 1;
                for (int i = 1; i<7; i++) {
                    if ( encoded_residuals_array[i].length() < smallestSize) {
                        smallestSize = encoded_residuals_array[i].length();
                        bestPredictor = i + 1;
                    }
                }
                bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(29, 32));
                bitStream.write_n_bits(encoded_residuals_array[bestPredictor-1]);

                encoded_residuals_array[0] = "";
                encoded_residuals_array[1] = "";
                encoded_residuals_array[2] = "";
                encoded_residuals_array[3] = "";
                encoded_residuals_array[4] = "";
                encoded_residuals_array[5] = "";
                encoded_residuals_array[6] = "";
            }

        }
        a = Vec3b(0,0,0);
        b = originalImg.at<Vec3b>(i, 0);
        c = Vec3b(0,0,0);
    }

    if (predictor_type == 0) {
        // Calculate the best predictor
        unsigned int smallestSize = encoded_residuals_array[0].length();
        int bestPredictor = 1;
        for (int i = 1; i<7; i++) {
            if ( encoded_residuals_array[i].length() < smallestSize) {
                smallestSize = encoded_residuals_array[i].length();
                bestPredictor = i + 1;
            }
        }
        bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(29, 32));
        bitStream.write_n_bits(encoded_residuals_array[bestPredictor-1]);
    }
    
}

int main(int argc,const char** argv) {

    int predictor_type = 0;
    int window_size = 50;

    if(argc < 3) {
		cerr << "Usage: ./image_encoder [ -p predictor (def 0) ]\n";
        cerr << "                       [ -w window size (def 50) ]\n";
		cerr << "                      input_file output_file\n";
		cerr << "Example: ./image_encoder -p 0 -w 10 ../pics-examples/airplane.ppm compressed.bin\n";
		return 1;
	}

    for(int n = 1 ; n < argc ; n++){
		if(string(argv[n]) == "-p") {
            try {
			    predictor_type = atoi(argv[n+1]);

                if(predictor_type < 0 || predictor_type > 7) {
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
    }

    Mat originalImg = imread(argv[argc-2]);   // loading the original image in the matrix
    
    BitStream bitStream { argv[argc-1], "w" };
    // Write golomb_m_parameter to coded file
	//bitStream.write_n_bits(std::bitset<32>(golomb_m_parameter).to_string());
    // Write predictor_type to coded file
    //string encoded_predictor_type = golombCode.encode(predictor_type);
    //bitStream.write_n_bits(encoded_predictor_type);
    bitStream.write_n_bits(std::bitset<32>(predictor_type).to_string());
    // Write number of rows to coded file
    //string encoded_rows = golombCode.encode(originalImg.rows);
    //bitStream.write_n_bits(encoded_rows);
    bitStream.write_n_bits(std::bitset<32>(originalImg.rows).to_string());
    // Write number of columns to coded file
    //string encoded_cols = golombCode.encode(originalImg.cols);
    //bitStream.write_n_bits(encoded_cols);
    bitStream.write_n_bits(std::bitset<32>(originalImg.cols).to_string());
    // Write image type to coded file
    //string encoded_type = golombCode.encode(originalImg.type());
    //bitStream.write_n_bits(encoded_type);
    bitStream.write_n_bits(std::bitset<32>(originalImg.type()).to_string());
    // Write window size to coded file
    bitStream.write_n_bits(std::bitset<32>(window_size).to_string());

    encodeImage(originalImg, predictor_type, bitStream, window_size);

    bitStream.close();

    return 0;
}

