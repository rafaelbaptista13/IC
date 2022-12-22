#include "lang.h"

using namespace std;


void findLangCalculation(string target_file_name, int k, double alpha, bool multipleModelsFlag) {

    map<string, string> reference_file_dict;
    reference_file_dict = { {"AFG", "../examples/AFG/afghanistan-medium.utf8"},
                            {"AFR", "../examples/AFR/afrikaans-small.utf8"},
                            {"ARA", "../examples/ARA/arabic-small.utf8"},
                            {"BUL", "../examples/BUL/bulgarian-medium.utf8"},
                            {"CRO", "../examples/CRO/croatian-medium.utf8"},
                            {"DEN", "../examples/DEN/danish-medium.utf8"},
                            {"ENG", "../examples/ENG/gb_english.utf8"},
                            {"SPA", "../examples/ESP/spanish-medium.utf8"},
                            {"FIN", "../examples/FIN/finnish-medium.utf8"},
                            {"FRA", "../examples/FRA/french-medium.utf8"},
                            {"GER", "../examples/GER/german-medium.utf8"},
                            {"GRE", "../examples/GRE/greek-medium.utf8"},
                            {"HUN", "../examples/HUN/hungarian-medium.utf8"},
                            {"ICE", "../examples/ICE/icelandic-medium.utf8"},
                            {"ITA", "../examples/ITA/italian-medium.utf8"},
                            {"POL", "../examples/POL/polish-medium.utf8"},
                            {"POR", "../examples/POR/portuguese-medium.txt"},
                            {"RUS", "../examples/RUS/russian-medium.utf8"},
                            {"UKR", "../examples/UKR/ukrainian-medium.utf8"}
                            };

    
    
    // Big references texts
    // reference_file_dict = { {"AFG", "../examples/AFG/afghanistan-medium.utf8"},
    //                         {"AFR", "../examples/AFR/afrikaans-big.utf8"},
    //                         {"ARA", "../examples/ARA/arabic-big.utf8"},
    //                         {"BUL", "../examples/BUL/bulgarian-big.utf8"},
    //                         {"CRO", "../examples/CRO/croatian-big.utf8"},
    //                         {"DEN", "../examples/DEN/danish-big.utf8"},
    //                         {"ENG", "../examples/ENG/gb_english.utf8"},
    //                         {"SPA", "../examples/ESP/spanish-big.utf8"},
    //                         {"FIN", "../examples/FIN/finnish-big.utf8"},
    //                         {"FRA", "../examples/FRA/french-big.utf8"},
    //                         {"GER", "../examples/GER/german-big.utf8"},
    //                         {"GRE", "../examples/GRE/greek-big.utf8"},
    //                         {"HUN", "../examples/HUN/hungarian-big.utf8"},
    //                         {"ICE", "../examples/ICE/icelandic-big.utf8"},
    //                         {"ITA", "../examples/ITA/italian-big.utf8"},
    //                         {"POL", "../examples/POL/polish-big.utf8"},
    //                         {"POR", "../examples/POR/portuguese-big.utf8"},
    //                         {"RUS", "../examples/RUS/russian-big.utf8"},
    //                         {"UKR", "../examples/UKR/ukrainian-big.utf8"}
    //                     }
    
                            

    cout << "Searching for the best language..." << endl;
    tuple<string, double> best_choice;
    best_choice = make_tuple("", 0.0);

    for (auto pair : reference_file_dict) {
        string language = pair.first;
        double num_bits;
        vector<tuple<int,int>> words;
        tuple<double, vector<tuple<int,int>>> return_val = langCalculation(pair.second, target_file_name, k, alpha, false, multipleModelsFlag);
        num_bits = get<0>(return_val);
        words = get<1>(return_val);

        if (get<1>(best_choice) == 0.0 || get<1>(best_choice) > num_bits) {
            best_choice = make_tuple(language, num_bits);
        }
    }

    cout << "Target file name: " <<  target_file_name << endl;
    cout << "Predicted Language: " <<  get<0>(best_choice) << endl;
}


int main(int argc,const char** argv) {

    int k {3};
    double alpha {0.5};
    bool multipleModelsFlag {false};
    
    if (argc < 2 ) {
        cerr << "Usage: ./findlang target_file_name [-k context_length (def 3)]\n";
        cerr << "                                   [-a alpha_value (def 0.5)]\n";
        cerr << "                                   [--multiplemodels]\n";
        cerr << "Example: ./findlang ../examples/example.txt -k 3 -a 0.4\n";
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
        } else if(string(argv[n]) == "-a") {
            try {
                alpha = atof(argv[n+1]);

                if (alpha <= 0 || alpha > 1) {
                    cerr << "Error: invalid alpha parameter requested. Must be in [0, 1[.\n";
                    return 1;
                }
            } catch (invalid_argument const&) {	
                cerr << "Error: invalid alpha parameter requested. Must be in [0, 1[.\n";
                return 1;
            }
        } else if(string(argv[n]) == "--multiplemodels") {
            multipleModelsFlag = true;
        }
    }

    findLangCalculation(target_file_name, k, alpha, multipleModelsFlag);
    
    return 0;
}
