#include <iostream>
#include "GolombCode.h"
#include "BitStream.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void encodeImage(Mat originalImg, int predictor_type, BitStream &bitStream, GolombCode golombCode) {

    Vec3b a = Vec3b(0,0,0);
    Vec3b b = Vec3b(0,0,0);
    Vec3b c = Vec3b(0,0,0);

    int residual[3];

    string encoded_residuals_array[] = {"", "", "", "", "", "", ""};

    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {

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

                // predictor 1
                residual[0] = current_pixel[0] - a[0];
                residual[1] = current_pixel[1] - a[1];
                residual[2] = current_pixel[2] - a[2];
                encoded_residuals_array[0] += golombCode.encode(residual[0]); 
                encoded_residuals_array[0] += golombCode.encode(residual[1]); 
                encoded_residuals_array[0] += golombCode.encode(residual[2]); 

                // predictor 2
                residual[0] = current_pixel[0] - b[0];
                residual[1] = current_pixel[1] - b[1];
                residual[2] = current_pixel[2] - b[2];
                encoded_residuals_array[1] += golombCode.encode(residual[0]); 
                encoded_residuals_array[1] += golombCode.encode(residual[1]); 
                encoded_residuals_array[1] += golombCode.encode(residual[2]); 

                // predictor 3
                residual[0] = current_pixel[0] - c[0];
                residual[1] = current_pixel[1] - c[1];
                residual[2] = current_pixel[2] - c[2];
                encoded_residuals_array[2] += golombCode.encode(residual[0]); 
                encoded_residuals_array[2] += golombCode.encode(residual[1]); 
                encoded_residuals_array[2] += golombCode.encode(residual[2]); 

                // predictor 4
                residual[0] = current_pixel[0] - (a[0] + b[0] - c[0]);
                residual[1] = current_pixel[1] - (a[1] + b[1] - c[1]);
                residual[2] = current_pixel[2] - (a[2] + b[2] - c[2]);
                encoded_residuals_array[3] += golombCode.encode(residual[0]); 
                encoded_residuals_array[3] += golombCode.encode(residual[1]); 
                encoded_residuals_array[3] += golombCode.encode(residual[2]); 

                // predictor 5
                residual[0] = current_pixel[0] - (a[0] + ((b[0] - c[0])/2));
                residual[1] = current_pixel[1] - (a[1] + ((b[1] - c[1])/2));
                residual[2] = current_pixel[2] - (a[2] + ((b[2] - c[2])/2));
                encoded_residuals_array[4] += golombCode.encode(residual[0]); 
                encoded_residuals_array[4] += golombCode.encode(residual[1]); 
                encoded_residuals_array[4] += golombCode.encode(residual[2]); 

                // predictor 6
                residual[0] = current_pixel[0] - (b[0] + ((a[0] - c[0])/2));
                residual[1] = current_pixel[1] - (b[1] + ((a[1] - c[1])/2));
                residual[2] = current_pixel[2] - (b[2] + ((a[2] - c[2])/2));
                encoded_residuals_array[5] += golombCode.encode(residual[0]); 
                encoded_residuals_array[5] += golombCode.encode(residual[1]); 
                encoded_residuals_array[5] += golombCode.encode(residual[2]); 

                // predictor 7
                residual[0] = current_pixel[0] - ((a[0] + b[0])/ 2);
                residual[1] = current_pixel[1] - ((a[1] + b[1])/ 2);
                residual[2] = current_pixel[2] - ((a[2] + b[2])/ 2);
                encoded_residuals_array[6] += golombCode.encode(residual[0]); 
                encoded_residuals_array[6] += golombCode.encode(residual[1]); 
                encoded_residuals_array[6] += golombCode.encode(residual[2]); 
            }

            if (predictor_type != 0) {
                // Write to file
                string encoded_residual_r = golombCode.encode(residual[0]);
                string encoded_residual_g = golombCode.encode(residual[1]);
                string encoded_residual_b = golombCode.encode(residual[2]);

                bitStream.write_n_bits(encoded_residual_r);
                bitStream.write_n_bits(encoded_residual_g);
                bitStream.write_n_bits(encoded_residual_b);
            }

            a = originalImg.at<Vec3b>(i, j);
            if (i != 0) {
                b = originalImg.at<Vec3b>(i-1, j+1);
                c = originalImg.at<Vec3b>(i-1, j);
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
    int golomb_m_parameter = 2;

    if(argc < 3) {
		cerr << "Usage: ./image_encoder [ -p predictor (def 0) ]\n";
		cerr << "                      input_file output_file\n";
		cerr << "Example: ./image_encoder -p 0 airplane.ppm coded.bin\n";
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
    }

    GolombCode golombCode {golomb_m_parameter};
    Mat originalImg = imread(argv[argc-2]);   // loading the original image in the matrix
    
    BitStream bitStream { argv[argc-1], "w" };
    // Write golomb_m_parameter to coded file
	bitStream.write_n_bits(std::bitset<32>(golomb_m_parameter).to_string());
    // Write predictor_type to coded file
    string encoded_predictor_type = golombCode.encode(predictor_type);
    bitStream.write_n_bits(encoded_predictor_type);
    // Write number of rows to coded file
    string encoded_rows = golombCode.encode(originalImg.rows);
    bitStream.write_n_bits(encoded_rows);
    // Write number of columns to coded file
    string encoded_cols = golombCode.encode(originalImg.cols);
    bitStream.write_n_bits(encoded_cols);
    // Write image type to coded file
    string encoded_type = golombCode.encode(originalImg.type());
    bitStream.write_n_bits(encoded_type);

    encodeImage(originalImg, predictor_type, bitStream, golombCode);

    bitStream.close();

    return 0;
}

