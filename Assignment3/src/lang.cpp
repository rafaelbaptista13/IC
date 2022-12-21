#include <cmath>
#include <vector>
#include <cstring>
#include <lang.h>

using namespace std;

int main(int argc,const char** argv) {

    int k {3};
    double alpha {0.5};
    bool multipleModelsFlag {false};
    
    if (argc < 3 ) {
        cerr << "Usage: ./lang input_text_file target_file_name [-k context_length (def 3)]\n";
        cerr << "                                                   [-a alpha_value (def 0.5)]\n";
        cerr << "                                                   [--multiplemodels]\n";
        cerr << "Example: ./lang ../examples/lusiadas.txt -k 3 -a 0.4\n";
        return 1;
    }

    string input_file_name = argv[1]; 
    string target_file_name = argv[2];
    
    for(int n = 2 ; n < argc ; n++) {
        if(string(argv[n]) == "-k") {
            try {
                k = atoi(argv[n+1]);

                if (k <= 0) {
                    cerr << "Error: invalid k parameter requested. Must be an integer greater than 0.\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid k parameter requested. Must be an integer greater than 0.\n";
                return 1;
            }
        } else if(string(argv[n]) == "-a") {
            try {
                alpha = atof(argv[n+1]);

                if (alpha <= 0) {
                    cerr << "Error: invalid alpha parameter requested. Must be greater than 0.\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid alpha parameter requested. Must be greater than 0.\n";
                return 1;
            }
        } else if(string(argv[n]) == "--multiplemodels") {
            multipleModelsFlag = true;
        }
    }

    if (multipleModelsFlag) {
        cout << "K parameter = " << "2 and 4" << endl;
    } else {
        cout << "K parameter = " << k << endl;
    }
    cout << "Alpha parameter = " << alpha << endl;
    cout << "Multiple Models = " << multipleModelsFlag << endl;

    double bits;
    vector<tuple<int,int>> foreign_words;

    tuple<double, vector<tuple<int,int>>> return_val = secondMain(input_file_name, target_file_name, k, alpha, false, multipleModelsFlag);
    bits = get<0>(return_val);
    foreign_words = get<1>(return_val);

    printf("Number of bits required to compress: %.3f\n", bits);
    return 0;
}
