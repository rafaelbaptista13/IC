#include<iostream>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
    Function used to copy an image pixel by pixel
*/

void copy_pixels(Mat& originalImg, Mat& manipulatedImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            manipulatedImg.at<Vec3b>(i, j) = originalImg.at<Vec3b>(i, j);
        }
    }
}


/*
    Function used to create a negative version of an image
*/

void negative_pixels(Mat& originalImg, Mat& manipulatedImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            uchar r = 255 - originalImg.at<Vec3b>(i, j)[0];
            uchar g = 255 - originalImg.at<Vec3b>(i, j)[1];
            uchar b = 255 - originalImg.at<Vec3b>(i, j)[2];
            manipulatedImg.at<Vec3b>(i, j) = Vec3b(r, g, b);
        }
    }
}


/*
    Function used to create an horizontal mirror of an image
*/

void mirror_horizontal_pixels(Mat& originalImg, Mat& manipulatedImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            manipulatedImg.at<Vec3b>(i,originalImg.cols - j) = originalImg.at<Vec3b>(i, j);
        }
    }
}


/*
    Function used to create a vertical mirror of an image
*/

void mirror_vertical_pixels(Mat& originalImg, Mat& manipulatedImg) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            manipulatedImg.at<Vec3b>(originalImg.cols - i, j) = originalImg.at<Vec3b>(i, j);
        }
    }
}


/*
    Function used to apply positive and negative rotation to an image.
*/

void rotate_pixels(Mat& originalImg, Mat& manipulatedImg, int degree) {

    switch (degree / 90 % 4) {
        case 0:     // 0 Degrees
            copy_pixels(originalImg, manipulatedImg);
            break;
        case 1:     // 90 Degrees
            for(int i=0; i<originalImg.rows; i++) {
                for (int j=0; j<originalImg.cols; j++) {
                    manipulatedImg.at<Vec3b>(originalImg.rows - i - 1, j) = originalImg.at<Vec3b>(j, i);
                }
            }
            break;
        case 2:     // 180 Degrees
            for(int i=0; i<originalImg.rows; i++) {
                for (int j=0; j<originalImg.cols; j++) {
                    manipulatedImg.at<Vec3b>(i, j) = originalImg.at<Vec3b>(originalImg.rows - i, originalImg.cols - j);
                }
            }
            break;
        case 3:     // 270 Degrees
            for(int i=0; i<originalImg.rows; i++) {
                for (int j=0; j<originalImg.cols; j++) {
                    manipulatedImg.at<Vec3b>(j, originalImg.rows - i - 1) = originalImg.at<Vec3b>(i, j);
                }
            }
            break;
        default:
            break;
    }   
}


/*
    Function used to change pixel's light intensity on an image
*/

void change_intensity_pixels(Mat& originalImg, Mat& manipulatedImg, float intensity_factor) {
    for(int i=0; i<originalImg.rows; i++) {
        for (int j=0; j<originalImg.cols; j++) {
            uchar r =  (originalImg.at<Vec3b>(i, j)[0] * intensity_factor > 255) ? 255 : originalImg.at<Vec3b>(i, j)[0] * intensity_factor;
            uchar g =  (originalImg.at<Vec3b>(i, j)[1] * intensity_factor > 255) ? 255 : originalImg.at<Vec3b>(i, j)[1] * intensity_factor;
            uchar b =  (originalImg.at<Vec3b>(i, j)[2] * intensity_factor > 255) ? 255 : originalImg.at<Vec3b>(i, j)[2] * intensity_factor;
            manipulatedImg.at<Vec3b>(i, j) = Vec3b(r, g, b);
        }
    }
}


/*
    Main Function
*/

int main(int argc,const char** argv) {

    int degree { 90 };
    float intensity_factor { 2 };

    if (argc < 4 ) {
        cerr << "./cp_image [-r degree (def 90)]\n";
        cerr << "           [-i intensity_factor (def 2)]\n";
        cerr << "           <mode> <original_img> <output_img>\n";
        return 1;
    }

    // Read Input Parameters
    string mode = argv[argc - 3];               // Program use mode. Define operation to apply.
    string original_file_name = argv[argc - 2]; // Original input file.
    string output_file_name = argv[argc - 1];   // Output file.

    // Declare Matrix variables
    Mat originalImg;    // declaring a matrix named originalImg
    Mat manipulatedImg; // declaring a matrix named manipulatedImg

    // Load the original image from file.
    originalImg = imread(original_file_name);   // loading the original image in the matrix

    if (mode == "ROTATE") {

        // Read degree paramter
        for(int n = 1 ; n < argc ; n++)
            if(string(argv[n]) == "-r") {
                try {
                    degree = atof(argv[n+1]);
                } catch (invalid_argument const&) {	
                    cerr << "Error: invalid r parameter requested.\n";
                    return 1;
                }
                break;
            }

        // Check degree multiple of 90
        if (degree % 90 != 0 ) {
            cerr << "Error: invalid r parameter requested. Degree should be multiple of 90.\n";
            return 1;
        }

        // Change negative rotations to corresponding positive rotations
        if (degree < 0) {
            degree = 360 + (degree % 360);
        }

        // Resize matrix according to the degree
        if (degree / 90 % 2 == 0) {
            // normal matrix size
            manipulatedImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());
        } else {
            // manipulated matrix size
            manipulatedImg = Mat(originalImg.cols, originalImg.rows, originalImg.type());
        }

        // Apply rotate operation
        rotate_pixels(originalImg, manipulatedImg, degree);
    } else if (mode == "NEGATIVE") {
        // Resize manipulated matrix
        manipulatedImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());

        // Apply negative operation
        negative_pixels(originalImg, manipulatedImg);
    } else if (mode == "MIRROR_VERTICAL") {
        // Resize manipulated matrix
        manipulatedImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());

        // Apply vertical mirror operation
        mirror_vertical_pixels(originalImg, manipulatedImg);
    } else if (mode == "MIRROR_HORIZONTAL") {
        // Resize manipulated matrix
        manipulatedImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());

        // Apply horizontal mirror operation
        mirror_horizontal_pixels(originalImg, manipulatedImg);
    } else if (mode == "INTENSITY") {

        // Read intensity_factor parameter
        for(int n = 1 ; n < argc ; n++)
            if(string(argv[n]) == "-i") {
                try {
                    intensity_factor = atof(argv[n+1]);
                } catch (invalid_argument const&) {	
                    cerr << "Error: invalid i parameter requested.\n";
                    return 1;
                }
                break;
            }

        // Check intensity factor is greater than 0
        if (intensity_factor <= 0 ) {
            cerr << "Error: invalid i parameter requested. Intensity factor should be greater than 0.\n";
            return 1;
        }

        // Resize manipulated matrix 
        manipulatedImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());

        // Apply change intensity operation
        change_intensity_pixels(originalImg, manipulatedImg, intensity_factor);
    } else if (mode == "COPY") {
        // Resize manipulated matrix
        manipulatedImg = Mat(originalImg.rows, originalImg.cols, originalImg.type());

        // Apply copy image operation
        copy_pixels(originalImg, manipulatedImg);
    } else {
        // Invalid mode selected. Notify user
        cerr << "Error: invalid mode selected.\n";
        return 1;
    }


    // Write manipulated Matrix to output file
    imwrite(output_file_name, manipulatedImg);  

    // Notify user. Operation was successfully done.
    cout << "Image is saved successfully…" << endl;
    return 0;
}

