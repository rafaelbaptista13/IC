#include "fcm.h"

int main(int argc,const char** argv) {

    int k {3};
    double a {0.5};
    
    if (argc < 2 ) {
        cerr << "Usage: ./fcm input_text_file [-k context_length (def 3)]\n";
        cerr << "                             [-a alpha_value (def 0.5)]\n";
        cerr << "Example: ./fcm ../examples/lusiadas.txt -k 3 -a 0.4\n";
        return 1;
    }

    string file_name = argv[1]; 
    
    for(int n = 1 ; n < argc ; n++) {
        if(string(argv[n]) == "-k") {
            try {
                k = atoi(argv[n+1]);
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid k parameter requested.\n";
                return 1;
            }
            break;
        } else if(string(argv[n]) == "-a") {
            try {
                a = atof(argv[n+1]);
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid a parameter requested.\n";
                return 1;
            }
            break;
        }
    }

    FCM fcm {file_name, k, a};
    fcm.calculate_probabilities();

    return 0;
}