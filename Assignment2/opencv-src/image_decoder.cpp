#include <iostream>
#include "GolombCode.h"
#include "BitStream.h"
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;
map<string, int> binaryToInt = {{"001", 1}, {"010", 2}, {"011", 3}, {"100", 4}, {"101", 5}, {"110", 6}, {"111", 7}};

void decodeImage(Mat& image, int rows, int cols, int predictor_type, BitStream &bitStreamRead, GolombCode golombCode) {

    Vec3b a = Vec3b(0,0,0);
    Vec3b b = Vec3b(0,0,0);
    Vec3b c = Vec3b(0,0,0);

    Vec3b residual;
    int selectedPredictor = predictor_type;
    if (predictor_type == 0) {
        selectedPredictor =  binaryToInt[bitStreamRead.get_n_bits(3)];
    }
    std::cout << "Selected Predictor: " << selectedPredictor << std::endl;

    for(int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {

            int residual_r = golombCode.decodeWithBitstream(bitStreamRead);
            int residual_g = golombCode.decodeWithBitstream(bitStreamRead);
            int residual_b = golombCode.decodeWithBitstream(bitStreamRead);
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
                b = image.at<Vec3b>(i-1, j+1);
                c = image.at<Vec3b>(i-1, j);
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
    // Get parameter m of Golomb Encoding
    std::string m_str = bitStreamRead.get_n_bits(32);
	int golomb_m_parameter = (int32_t) std::bitset<32>(m_str).to_ulong();

    GolombCode golombCode {golomb_m_parameter};

    // Get predictor type
    int predictor_type = golombCode.decodeWithBitstream(bitStreamRead);
    // Get number of rows
    int rows = golombCode.decodeWithBitstream(bitStreamRead);
    // Get number of cols
    int cols = golombCode.decodeWithBitstream(bitStreamRead);
    // Get number of cols
    int image_type = golombCode.decodeWithBitstream(bitStreamRead);
    
    Mat image = Mat(rows, cols, image_type);
    decodeImage(image, rows, cols, predictor_type, bitStreamRead, golombCode);

    // Write manipulated Matrix to output file
    imwrite(argv[argc-1], image);  

    bitStreamRead.close();

    return 0;
}

