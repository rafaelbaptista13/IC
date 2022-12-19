#include "lang.cpp"

using namespace std;


int main(int argc,const char** argv) {

    int k {3};
    double alpha {0.5};
    bool multipleModelsFlag {false};
    
    if (argc < 2 ) {
        cerr << "Usage: ./findlang target_file_name [-k context_length (def 3)]\n";
        cerr << "                                   [-a alpha_value (def 0.5)]\n";
        cerr << "                                   [--multiplemodels]\n";
        cerr << "Example: ./findlang ../examples/lusiadas.txt -k 3 -a 0.4\n";
        return 1;
    }

    string target_file_name = argv[1];
    
    for(int n = 1 ; n < argc ; n++) {
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
            break;
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
            break;
        } else if(string(argv[n]) == "--multiplemodels") {
            multipleModelsFlag = true;
        }
    }

    secondMain(target_file_name, k, alpha, multipleModelsFlag);
    
    return 0;
}

void secondMain(string target_file_name, int k, double alpha, bool multipleModelsFlag) {

    map<string, string> reference_file_dict;
    reference_file_dict = { {"AFG", "example/AFG/afghanistan-medium.utf8"},
                            {"AFR", "example/AFR/afrikaans-small.utf8"},
                            {"ARA", "example/ARA/arabic-small.utf8"},
                            {"BUL", "example/BUL/bulgarian-medium.utf8"},
                            {"CRO", "example/CRO/croatian-medium.utf8"},
                            {"DEN", "example/DEN/danish-medium.utf8"},
                            {"ENG", "example/ENG/gb_english.utf8"},
                            {"SPA", "example/ESP/spanish-medium.utf8"},
                            {"FIN", "example/FIN/finnish-medium.utf8"},
                            {"FRA", "example/FRA/french-medium.utf8"},
                            {"GER", "example/GER/german-medium.utf8"},
                            {"GRE", "example/GRE/greek-medium.utf8"},
                            {"HUN", "example/HUN/hungarian-medium.utf8"},
                            {"ICE", "example/ICE/icelandic-medium.utf8"},
                            {"ITA", "example/ITA/italian-medium.utf8"},
                            {"POL", "example/POL/polish-medium.utf8"},
                            {"POR", "example/POR/portuguese-medium.txt"},
                            {"RUS", "example/RUS/russian-medium.utf8"},
                            {"UKR", "example/UKR/ukrainian-medium.utf8"}
                            };

    
    
    // Big references texts
    // reference_file_dict = { {"AFG", "example/AFG/afghanistan-medium.utf8"},
    //                         {"AFR", "example/AFR/afrikaans-big.utf8"},
    //                         {"ARA", "example/ARA/arabic-big.utf8"},
    //                         {"BUL", "example/BUL/bulgarian-big.utf8"},
    //                         {"CRO", "example/CRO/croatian-big.utf8"},
    //                         {"DEN", "example/DEN/danish-big.utf8"},
    //                         {"ENG", "example/ENG/gb_english.utf8"},
    //                         {"SPA", "example/ESP/spanish-big.utf8"},
    //                         {"FIN", "example/FIN/finnish-big.utf8"},
    //                         {"FRA", "example/FRA/french-big.utf8"},
    //                         {"GER", "example/GER/german-big.utf8"},
    //                         {"GRE", "example/GRE/greek-big.utf8"},
    //                         {"HUN", "example/HUN/hungarian-big.utf8"},
    //                         {"ICE", "example/ICE/icelandic-big.utf8"},
    //                         {"ITA", "example/ITA/italian-big.utf8"},
    //                         {"POL", "example/POL/polish-big.utf8"},
    //                         {"POR", "example/POR/portuguese-big.utf8"},
    //                         {"RUS", "example/RUS/russian-big.utf8"},
    //                         {"UKR", "example/UKR/ukrainian-big.utf8"}
    //                     }
    
                            

    cout << "Searching for the best language..." << endl;
    tuple<string, double> best_choice;
    best_choice = make_tuple(NULL, NULL);

    for (auto pair : reference_file_dict) {
        string language = pair.first;
        double num_bits;
        vector<tuple<int,int>> words;
        auto [num_bits, words] = secondMain(pair.second, target_file_name, k, alpha, false, multipleModelsFlag);
        
        if (get<1>(best_choice) == NULL || get<1>(best_choice) > num_bits) {
            best_choice = make_tuple(language, num_bits);
        }
    }

    cout << "Target file name: " <<  target_file_name << endl;
    cout << "Predicted Language: " <<  get<0>(best_choice) << endl;
}
