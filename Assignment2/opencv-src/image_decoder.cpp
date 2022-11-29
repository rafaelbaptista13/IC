#include <iostream>
#include "GolombCode.h"
#include "BitStream.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;
map<string, int> binaryToInt = {{"001", 1}, {"010", 2}, {"011", 3}, {"100", 4}, {"101", 5}, {"110", 6}, {"111", 7}};

int optimizeGolombParameter(long int sumSamples, int numSamples) {
    double mean = (double) sumSamples/ (double) numSamples;
    double alfa = mean / (mean + 1);
    return ceil( -1/log2(alfa) ) ;
}

void decodeImage(Mat& image, int rows, int cols, int predictor_type, BitStream &bitStreamRead, int window_size) {

    int golomb_m_parameter = 16;
    long int sumResiduals = 0;
    double alfa = 0;
    int num_pixels = 0;

    Vec3b a = Vec3b(0,0,0);
    Vec3b b = Vec3b(0,0,0);
    Vec3b c = Vec3b(0,0,0);

    Vec3b residual;
    int selectedPredictor = predictor_type;
    if (predictor_type == 0) {
        selectedPredictor =  binaryToInt[bitStreamRead.get_n_bits(3)];
    }
    std::cout << "Selected Predictor: " << selectedPredictor << std::endl;
    std::cout << "Window size: " << window_size << std::endl;

    GolombCode golombCode {golomb_m_parameter};
    map<int, GolombCode> golombCodes = {{16, golombCode}};

    for(int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            
            num_pixels++;
            int residual_r = golombCode.decodeWithBitstream(bitStreamRead);
            int residual_g = golombCode.decodeWithBitstream(bitStreamRead);
            int residual_b = golombCode.decodeWithBitstream(bitStreamRead);
            sumResiduals += abs(residual_r);
            sumResiduals += abs(residual_g);
            sumResiduals += abs(residual_b);
            residual = Vec3b(residual_r, residual_g, residual_b);
            
            if (selectedPredictor == 1) {
                image.at<Vec3b>(i, j)[0] = residual[0] + a[0];
                image.at<Vec3b>(i, j)[1] = residual[1] + a[1];
                image.at<Vec3b>(i, j)[2] = residual[2] + a[2];
            } else if (selectedPredictor == 2) {
                image.at<Vec3b>(i, j)[0] = residual[0] + b[0];
                image.at<Vec3b>(i, j)[1] = residual[1] + b[1];
                image.at<Vec3b>(i, j)[2] = residual[2] + b[2];
            } else if (selectedPredictor == 3) {
                image.at<Vec3b>(i, j)[0] = residual[0] + c[0];
                image.at<Vec3b>(i, j)[1] = residual[1] + c[1];
                image.at<Vec3b>(i, j)[2] = residual[2] + c[2];
            } else if (selectedPredictor == 4) {
                image.at<Vec3b>(i, j)[0] = residual[0] + (a[0] + b[0] - c[0]);
                image.at<Vec3b>(i, j)[1] = residual[1] + (a[1] + b[1] - c[1]);
                image.at<Vec3b>(i, j)[2] = residual[2] + (a[2] + b[2] - c[2]);
            } else if (selectedPredictor == 5) {
                image.at<Vec3b>(i, j)[0] = residual[0] + (a[0] + ((b[0] - c[0])/2));
                image.at<Vec3b>(i, j)[1] = residual[1] + (a[1] + ((b[1] - c[1])/2));
                image.at<Vec3b>(i, j)[2] = residual[2] + (a[2] + ((b[2] - c[2])/2));
            } else if (selectedPredictor == 6) {
                image.at<Vec3b>(i, j)[0] = residual[0] + (b[0] + ((a[0] - c[0])/2));
                image.at<Vec3b>(i, j)[1] = residual[1] + (b[1] + ((a[1] - c[1])/2));
                image.at<Vec3b>(i, j)[2] = residual[2] + (b[2] + ((a[2] - c[2])/2));
            } else if (selectedPredictor == 7) {
                image.at<Vec3b>(i, j) = residual + ((a + b)/ 2);
                image.at<Vec3b>(i, j)[0] = residual[0] + ((a[0] + b[0])/ 2);
                image.at<Vec3b>(i, j)[1] = residual[1] + ((a[1] + b[1])/ 2);
                image.at<Vec3b>(i, j)[2] = residual[2] + ((a[2] + b[2])/ 2);
            }

            a = image.at<Vec3b>(i, j);
            if (i != 0) {
                // if it is the first line, this does not execute, b=(0,0,0) and c=(0,0,0)
                b = image.at<Vec3b>(i-1, j+1);
                c = image.at<Vec3b>(i-1, j);
            }

            if (num_pixels % window_size == 0) {
                double mean = (double) sumResiduals/ (double) window_size;
                alfa = mean / (mean + 1);
                golomb_m_parameter = ceil( -1/log2(alfa) );
                if (golomb_m_parameter == 0) continue;
                if (golombCodes.count(golomb_m_parameter)) {
                    golombCode = golombCodes.find(golomb_m_parameter)->second;
                } else {
                    // Create golombCode
                    golombCode =  GolombCode(golomb_m_parameter);
                    golombCodes.insert({golomb_m_parameter, golombCode});
                }
                sumResiduals = 0;
            }
        }
        a = Vec3b(0,0,0);
        b = image.at<Vec3b>(i, 0);
        c = Vec3b(0,0,0);
    }

}

int main(int argc,const char** argv) {

    if(argc < 3) {
		cerr << "Usage: ./image_decoder input_file output_file\n";
		cerr << "Example: ./image_decoder coded.bin output.ppm\n";
		return 1;
	}

    // BitStream to read coded file
    BitStream bitStreamRead { argv[argc-2], "r" };
    // Get predictor type
    std::string predictor_type_str = bitStreamRead.get_n_bits(32);
    int predictor_type = (int32_t) std::bitset<32>(predictor_type_str).to_ulong();
    // Get number of rows
    std::string rows_str = bitStreamRead.get_n_bits(32);
    int rows = (int32_t) std::bitset<32>(rows_str).to_ulong();
    // Get number of cols
    std::string cols_str = bitStreamRead.get_n_bits(32);
    int cols = (int32_t) std::bitset<32>(cols_str).to_ulong();
    // Get image type
    std::string type_str = bitStreamRead.get_n_bits(32);
    int image_type = (int32_t) std::bitset<32>(type_str).to_ulong();
    // Get window size
    std::string window_size_str = bitStreamRead.get_n_bits(32);
    int window_size = (int32_t) std::bitset<32>(window_size_str).to_ulong();
    
    Mat image = Mat(rows, cols, image_type);
    decodeImage(image, rows, cols, predictor_type, bitStreamRead, window_size);

    // Write manipulated Matrix to output file
    imwrite(argv[argc-1], image);  

    bitStreamRead.close();

    return 0;
}

