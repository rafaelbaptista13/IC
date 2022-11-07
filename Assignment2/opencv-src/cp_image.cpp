#include<iostream>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void copy_pixels(Mat& originalImg, Mat& copyImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            copyImg.at<Vec3b>(i, j) = originalImg.at<Vec3b>(i, j);
        }
    }
}

void negative_pixels(Mat& originalImg, Mat& copyImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            uchar r = 255 - originalImg.at<Vec3b>(i, j)[0];
            uchar g = 255 - originalImg.at<Vec3b>(i, j)[1];
            uchar b = 255 - originalImg.at<Vec3b>(i, j)[2];
            copyImg.at<Vec3b>(i, j) = Vec3b(r, g, b);
        }
    }
}

void mirror_vertical_pixels(Mat& originalImg, Mat& copyImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            copyImg.at<Vec3b>(originalImg.cols - i, j) = originalImg.at<Vec3b>(i, j);
        }
    }
}

void mirror_horizontal_pixels(Mat& originalImg, Mat& copyImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            copyImg.at<Vec3b>(i,originalImg.cols - j) = originalImg.at<Vec3b>(i, j);
        }
    }
}

void rotate_pixels(Mat& originalImg, Mat& copyImg, int degree) {

    switch (degree / 90 % 4) {
        case 0:     // 0 Degrees
            copy_pixels(originalImg, copyImg);
            break;
        case 1:     // 90 Degrees
            for(int i=0; i<originalImg.rows; i++) {
                for (int j=0; j<originalImg.cols; j++) {
                    copyImg.at<Vec3b>(originalImg.rows - i - 1, j) = originalImg.at<Vec3b>(j, i);
                }
            }
            break;
        case 2:     // 180 Degrees
            for(int i=0; i<originalImg.rows; i++) {
                for (int j=0; j<originalImg.cols; j++) {
                    copyImg.at<Vec3b>(i, j) = originalImg.at<Vec3b>(originalImg.rows - i, originalImg.cols - j);
                }
            }
            break;
        case 3:     // 270 Degrees
            for(int i=0; i<originalImg.rows; i++) {
                for (int j=0; j<originalImg.cols; j++) {
                    copyImg.at<Vec3b>(j, originalImg.rows - i - 1) = originalImg.at<Vec3b>(i, j);
                }
            }
            break;
        default:
            break;
    }   
}


void change_intensity_pixels(Mat& originalImg, Mat& copyImg, float intensity_factor) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            uchar r =  (originalImg.at<Vec3b>(i, j)[0] * intensity_factor > 255) ? 255 : originalImg.at<Vec3b>(i, j)[0] * intensity_factor;
            uchar g =  (originalImg.at<Vec3b>(i, j)[1] * intensity_factor > 255) ? 255 : originalImg.at<Vec3b>(i, j)[1] * intensity_factor;
            uchar b =  (originalImg.at<Vec3b>(i, j)[2] * intensity_factor > 255) ? 255 : originalImg.at<Vec3b>(i, j)[2] * intensity_factor;
            copyImg.at<Vec3b>(i, j) = Vec3b(r, g, b);
        }
    }
}



int main(int argc,const char** argv) {

    int degree { 90 };
    float intensity_factor { 2 };

    if (argc < 4 ) {
        cerr << "./cp_image [-r degree (def 90)] [-i intensity_factor (def 2)] <mode> <original_img> <output_img>\n";
        return 1;
    }

    string mode = argv[argc - 3];
    string original_file_name = argv[argc - 2];
    string output_file_name = argv[argc - 1];

    Mat originalImg;    // declaring a matrix named originalImg
    Mat copyImg;        // declaring a matrix named copyImg

    originalImg = imread(original_file_name);   // loading the original image in the matrix

    if (mode == "ROTATE") {
        for(int n = 1 ; n < argc ; n++)
            if(string(argv[n]) == "-r") {
                degree = atof(argv[n+1]);
                break;
            }

        // Check degree multiple of 90
        if (degree % 90 != 0 ) {
            cerr << "Degree should be multiple of 90.\n";
            return 1;
        }

        if (degree < 0) {
            degree = 360 + (degree % 360);
        }

        // Resize matrix according to the degree
        if (degree / 90 % 2 == 0) {
            // normal matrix size
            copyImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        } else {
            // change matrix size
            copyImg = Mat(originalImg.cols, originalImg.rows, originalImg.type());
        }
        rotate_pixels(originalImg, copyImg, degree);
    } else if (mode == "NEGATIVE") {
        copyImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        negative_pixels(originalImg, copyImg);
    } else if (mode == "MIRROR_VERTICAL") {
        copyImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        mirror_vertical_pixels(originalImg, copyImg);
    } else if (mode == "MIRROR_HORIZONTAL") {
        copyImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        mirror_horizontal_pixels(originalImg, copyImg);
    } else if (mode == "INTENSITY") {

        for(int n = 1 ; n < argc ; n++)
            if(string(argv[n]) == "-i") {
                intensity_factor = atof(argv[n+1]);
                break;
            }

        // Check degree multiple of 90
        if (intensity_factor <= 0 ) {
            cerr << "Intensity factor should be greater than 0.\n";
            return 1;
        }


        copyImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        change_intensity_pixels(originalImg, copyImg, intensity_factor);
    } else if (mode == "COPY") {
        copyImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        copy_pixels(originalImg, copyImg);
    }

    imwrite(output_file_name, copyImg);  

    cout << "Image is saved successfully…" << endl;
    return 0;
}

