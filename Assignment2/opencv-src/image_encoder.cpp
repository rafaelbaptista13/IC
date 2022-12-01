#include <iostream>
#include "GolombCode.h"
#include "BitStream.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;


/*
    Function used to optimize the Golomb Parameter m
*/

int optimizeGolombParameter(long int sumSamples, int numSamples) {
    double mean = (double) sumSamples/ (double) numSamples;
    double alfa = mean / (mean + 1);
    return ceil( -1/log2(alfa) ) ;
}


/*
    Function used to encode a image
*/

void encodeImage(Mat originalImg, int predictor_type, BitStream &bitStream, int window_size) {

    // Starting m values for all predictors
    int golomb_m_parameter[] = {16, 16, 16, 16, 16, 16, 16};   
    // Sum of Residuals values for all predictors
    long int sumResiduals[] = {0, 0, 0, 0, 0, 0, 0};
    // String to store the encoded residuals of a block for all predictors, in order to compare them later
    string encoded_residuals[] = {"", "", "", "", "", "", ""};
    // Write first golomb m parameter of each predictor and initial sum residual in first block
    for (int i= 0; i < 7; i++) {
        encoded_residuals[i] += std::bitset<32>(golomb_m_parameter[i]).to_string();
        encoded_residuals[i] += std::bitset<32>(sumResiduals[i]).to_string();
    }
    // Number of pixels read
    int num_pixels = 0;

    // Values of pixels for the calculation of residuals
    Vec3b a = Vec3b(0,0,0);
    Vec3b b = Vec3b(0,0,0);
    Vec3b c = Vec3b(0,0,0);
    // Residual value
    int residual[3];

    // Starting golombCode with m=16
    GolombCode golombCode {16};
    // Map to store all the golombCodes used, to prevent re-creation of golombCode objects with same m
    map<int, GolombCode> golombCodes = {{16, golombCode}};
    // Starting golombCode object for all predictors
    GolombCode currentGolombCodes[] = {golombCode, golombCode, golombCode, golombCode, golombCode, golombCode, golombCode};
    
    // Iterate over all pixels
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            num_pixels++;
            // Get current pixel
            int current_pixel[3] = {originalImg.at<Vec3b>(i, j)[0], originalImg.at<Vec3b>(i, j)[1], originalImg.at<Vec3b>(i, j)[2]};
            
            // Calculate residual
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
                // Calculate residuals for all predictors in order to choose the best one

                // predictor 1
                residual[0] = current_pixel[0] - a[0];
                residual[1] = current_pixel[1] - a[1];
                residual[2] = current_pixel[2] - a[2];
                // Append to encoded string of this predictor
                encoded_residuals[0] += currentGolombCodes[0].encode(residual[0]); 
                encoded_residuals[0] += currentGolombCodes[0].encode(residual[1]); 
                encoded_residuals[0] += currentGolombCodes[0].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[0] += abs(residual[0]);
                sumResiduals[0] += abs(residual[1]);
                sumResiduals[0] += abs(residual[2]);
                
                // predictor 2
                residual[0] = current_pixel[0] - b[0];
                residual[1] = current_pixel[1] - b[1];
                residual[2] = current_pixel[2] - b[2];
                // Append to encoded string of this predictor
                encoded_residuals[1] += currentGolombCodes[1].encode(residual[0]); 
                encoded_residuals[1] += currentGolombCodes[1].encode(residual[1]); 
                encoded_residuals[1] += currentGolombCodes[1].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[1] += abs(residual[0]);
                sumResiduals[1] += abs(residual[1]);
                sumResiduals[1] += abs(residual[2]);

                // predictor 3
                residual[0] = current_pixel[0] - c[0];
                residual[1] = current_pixel[1] - c[1];
                residual[2] = current_pixel[2] - c[2];
                // Append to encoded string of this predictor
                encoded_residuals[2] += currentGolombCodes[2].encode(residual[0]); 
                encoded_residuals[2] += currentGolombCodes[2].encode(residual[1]); 
                encoded_residuals[2] += currentGolombCodes[2].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[2] += abs(residual[0]);
                sumResiduals[2] += abs(residual[1]);
                sumResiduals[2] += abs(residual[2]);

                // predictor 4
                residual[0] = current_pixel[0] - (a[0] + b[0] - c[0]);
                residual[1] = current_pixel[1] - (a[1] + b[1] - c[1]);
                residual[2] = current_pixel[2] - (a[2] + b[2] - c[2]);
                // Append to encoded string of this predictor
                encoded_residuals[3] += currentGolombCodes[3].encode(residual[0]); 
                encoded_residuals[3] += currentGolombCodes[3].encode(residual[1]); 
                encoded_residuals[3] += currentGolombCodes[3].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[3] += abs(residual[0]);
                sumResiduals[3] += abs(residual[1]);
                sumResiduals[3] += abs(residual[2]);

                // predictor 5
                residual[0] = current_pixel[0] - (a[0] + ((b[0] - c[0])/2));
                residual[1] = current_pixel[1] - (a[1] + ((b[1] - c[1])/2));
                residual[2] = current_pixel[2] - (a[2] + ((b[2] - c[2])/2));
                // Append to encoded string of this predictor
                encoded_residuals[4] += currentGolombCodes[4].encode(residual[0]); 
                encoded_residuals[4] += currentGolombCodes[4].encode(residual[1]); 
                encoded_residuals[4] += currentGolombCodes[4].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[4] += abs(residual[0]);
                sumResiduals[4] += abs(residual[1]); 
                sumResiduals[4] += abs(residual[2]);
                
                // predictor 6
                residual[0] = current_pixel[0] - (b[0] + ((a[0] - c[0])/2));
                residual[1] = current_pixel[1] - (b[1] + ((a[1] - c[1])/2));
                residual[2] = current_pixel[2] - (b[2] + ((a[2] - c[2])/2));
                // Append to encoded string of this predictor
                encoded_residuals[5] += currentGolombCodes[5].encode(residual[0]); 
                encoded_residuals[5] += currentGolombCodes[5].encode(residual[1]); 
                encoded_residuals[5] += currentGolombCodes[5].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[5] += abs(residual[0]);
                sumResiduals[5] += abs(residual[1]);
                sumResiduals[5] += abs(residual[2]);

                // predictor 7
                residual[0] = current_pixel[0] - ((a[0] + b[0])/ 2);
                residual[1] = current_pixel[1] - ((a[1] + b[1])/ 2);
                residual[2] = current_pixel[2] - ((a[2] + b[2])/ 2);
                // Append to encoded string of this predictor
                encoded_residuals[6] += currentGolombCodes[6].encode(residual[0]); 
                encoded_residuals[6] += currentGolombCodes[6].encode(residual[1]); 
                encoded_residuals[6] += currentGolombCodes[6].encode(residual[2]); 
                // Increment sum of residuals of this predictor
                sumResiduals[6] += abs(residual[0]);
                sumResiduals[6] += abs(residual[1]);
                sumResiduals[6] += abs(residual[2]);
            
            }

            // If the predictor type is not the one that tries all the predictors and choose the best one
            if (predictor_type != 0) {
                // Get encoded residual
                string encoded_residual_r = currentGolombCodes[predictor_type-1].encode(residual[0]);
                string encoded_residual_g = currentGolombCodes[predictor_type-1].encode(residual[1]);
                string encoded_residual_b = currentGolombCodes[predictor_type-1].encode(residual[2]);
                // Write to file
                bitStream.write_n_bits(encoded_residual_r);
                bitStream.write_n_bits(encoded_residual_g);
                bitStream.write_n_bits(encoded_residual_b);
                // Update sum of residuals
                sumResiduals[predictor_type-1] += abs(residual[0]);
                sumResiduals[predictor_type-1] += abs(residual[1]);
                sumResiduals[predictor_type-1] += abs(residual[2]);
            }

            // Update a b and c pixels
            a = originalImg.at<Vec3b>(i, j);
            if (i != 0) {
                // if it is the first line, this does not execute, b=(0,0,0) and c=(0,0,0)
                b = originalImg.at<Vec3b>(i-1, j+1);
                c = originalImg.at<Vec3b>(i-1, j);
            }

            if (num_pixels % window_size == 0) {
                // Update golombCodes
                for (int predictor = 0; predictor < 7; predictor++) {
                    golomb_m_parameter[predictor] = optimizeGolombParameter(sumResiduals[predictor], window_size);
                    // If m is 0, then the m should be 1
                    if (golomb_m_parameter[predictor] == 0) golomb_m_parameter[predictor] = 1;
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

            // If the predictor type is the one that tries all the predictors and choose the best one
            if (predictor_type == 0 && num_pixels % 2000 == 0) {
                // Calculate the best predictor
                unsigned int smallestSize = encoded_residuals[0].length();
                int bestPredictor = 1;
                for (int i = 1; i<7; i++) {
                    if ( encoded_residuals[i].length() < smallestSize) {
                        smallestSize = encoded_residuals[i].length();
                        bestPredictor = i + 1;
                    }
                }
                // Write the best predictor value
                bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(29, 32));
                // Write the encoded string of the best predictor
                bitStream.write_n_bits(encoded_residuals[bestPredictor-1]);

                // Reset the encoded string by starting with the first golomb m parameter and the current sum of residuals
                for (int i= 0; i < 7; i++) {
                    encoded_residuals[i] = std::bitset<32>(golomb_m_parameter[i]).to_string();
                    encoded_residuals[i] += std::bitset<32>(sumResiduals[i]).to_string();
                }
            }

        }
        // In the first element of a line, the a and c pixel are outside the image, so should be 0
        a = Vec3b(0,0,0);
        b = originalImg.at<Vec3b>(i, 0);
        c = Vec3b(0,0,0);
    }

    // If the predictor type is the one that tries all the predictors and choose the best one
    if (predictor_type == 0) {
        // Write to file the remaining encoded string
        // Calculate the best predictor
        unsigned int smallestSize = encoded_residuals[0].length();
        int bestPredictor = 1;
        for (int i = 1; i<7; i++) {
            if ( encoded_residuals[i].length() < smallestSize) {
                smallestSize = encoded_residuals[i].length();
                bestPredictor = i + 1;
            }
        }
        // Write the best predictor value
        bitStream.write_n_bits(std::bitset<32>(bestPredictor).to_string().substr(29, 32));
        // Write the encoded string of the best predictor
        bitStream.write_n_bits(encoded_residuals[bestPredictor-1]);
    }
    
}

int main(int argc,const char** argv) {

    // Default values
    int predictor_type = 0;
    int window_size = 50;

    if(argc < 3) {
		cerr << "Usage: ./image_encoder [ -p predictor (def 0) ]\n";
        cerr << "                       [ -w window size (def 50) ]\n";
		cerr << "                      input_file output_file\n";
		cerr << "Example: ./image_encoder -p 0 -w 10 ../pics-examples/airplane.ppm compressed.bin\n";
		return 1;
	}

    // Get parameters of user
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
    // Write predictor_type to coded file
    bitStream.write_n_bits(std::bitset<32>(predictor_type).to_string());
    // Write number of rows to coded file
    bitStream.write_n_bits(std::bitset<32>(originalImg.rows).to_string());
    // Write number of columns to coded file
    bitStream.write_n_bits(std::bitset<32>(originalImg.cols).to_string());
    // Write image type to coded file
    bitStream.write_n_bits(std::bitset<32>(originalImg.type()).to_string());
    // Write window size to coded file
    bitStream.write_n_bits(std::bitset<32>(window_size).to_string());

    encodeImage(originalImg, predictor_type, bitStream, window_size);

    bitStream.close();

    return 0;
}

