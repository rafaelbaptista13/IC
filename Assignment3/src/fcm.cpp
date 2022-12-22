#include "fcm.h"

using namespace std;

int main(int argc,const char** argv) {

    int k {3};
    double alpha {0.5};
    
    if (argc < 2 ) {
        cerr << "Usage: ./fcm input_text_file [-k context_length (def 3)]\n";
        cerr << "                             [-a alpha_value (def 0.5)]\n";
        cerr << "Example: ./fcm ../examples/example.txt -k 3 -a 0.4\n";
        return 1;
    }

    string file_name = argv[1]; 
    
    for(int n = 1 ; n < argc ; n++) {
        if(string(argv[n]) == "-k") {
            try {
                k = atoi(argv[n+1]);
                if (k <= 0) {
                    cerr << "Error: invalid k parameter requested. Must be an integer greater than 0.\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid k parameter requested.\n";
                return 1;
            }
        } else if(string(argv[n]) == "-a") {
            try {
                alpha = atof(argv[n+1]);
                if (alpha <= 0 || alpha > 1) {
                    cerr << "Error: invalid alpha parameter requested. Must be in [0, 1[.\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid alpha parameter requested.\n";
                return 1;
            }
        }
    }

    FCM fcm {file_name, k, alpha};
    double final_entropy = fcm.calculate_probabilities();
    std::cout << "Final Entropy: " << final_entropy << std::endl;
    return 0;
}