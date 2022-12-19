#include "lang.cpp"

using namespace std;


int main(int argc,const char** argv) {

    int k {3};
    double alpha {0.5};
    
    if (argc < 2 ) {
        cerr << "Usage: ./locatelang target_file_name [-k context_length (def 3)]\n";
        cerr << "                                     [-a alpha_value (def 0.5)]\n";
        cerr << "Example: ./locatelang ../examples/lusiadas.txt -k 3 -a 0.4\n";
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
        }
    }

    secondMain(target_file_name, k, alpha);
    
    return 0;
}

void secondMain(string target_file_name, int k, double alpha) {

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

    
    
    // Small references texts
    // reference_file_dict = { {"AFG", "example/AFG/afghanistan-small.utf8"},
    //                         {"AFR", "example/AFR/afrikaans-small.utf8"},
    //                         {"ARA", "example/ARA/arabic-small.utf8"},
    //                         {"BUL", "example/BUL/bulgarian-small.utf8"},
    //                         {"CRO", "example/CRO/croatian-small.utf8"},
    //                         {"DEN", "example/DEN/danish-small.utf8"},
    //                         {"ENG", "example/ENG/gb_english.utf8"},
    //                         {"SPA", "example/ESP/spanish-small.utf8"},
    //                         {"FIN", "example/FIN/finnish-small.utf8"},
    //                         {"FRA", "example/FRA/french-small.utf8"},
    //                         {"GER", "example/GER/german-small.utf8"},
    //                         {"GRE", "example/GRE/greek-small.utf8"},
    //                         {"HUN", "example/HUN/hungarian-small.utf8"},
    //                         {"ICE", "example/ICE/icelandic-small.utf8"},
    //                         {"ITA", "example/ITA/italian-small.utf8"},
    //                         {"POL", "example/POL/polish-small.utf8"},
    //                         {"POR", "example/POR/portuguese-small.utf8"},
    //                         {"RUS", "example/RUS/russian-small.utf8"},
    //                         {"UKR", "example/UKR/ukrainian-small.utf8"}
    //                     }
                       

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      return;
    }

    set<wchar_t> target_alphabet;
    int target_file_length = 0;

    wchar_t ch;
    while (file.get(ch)) {
        target_file_length += 1;
        target_alphabet.insert(ch);
    }
    file.close();

    map<tuple<int,int>, vector<string>> sections_dict;
    for (auto pair: reference_file_dict) {
        string language = pair.first;

        FCM fcm_model = FCM(pair.second, k, alpha);
        fcm_model.calculate_probabilities();

        double num_bits;
        vector<tuple<int,int>> sections;

        auto [num_bits, sections] = get_number_of_bits_required_to_compress_v2(fcm_model, target_file_name, target_alphabet, k, true);
    
        //sections_dict = truncate_and_merge_sections(sections_dict, sections, language);
    }
}




void truncate_and_merge_sections(map<tuple<int,int>,vector<string>> sections_dict, vector<tuple<int,int>> sections, string language) {

    vector<vector<tuple<int,int>>> sections_to_remove;

    for (auto sections_dict_pair: sections_dict) {
        tuple<int,int> section = sections_dict_pair.first;

        if (find(sections_dict[section].begin(), sections_dict[section].end(), language) != sections_dict[section].end() ) {

        }
    }
}