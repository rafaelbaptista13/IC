#include "lang.h"
#include <algorithm>

using namespace std;

map<tuple<int,int>,vector<string>> truncate_and_merge_sections(map<tuple<int,int>,vector<string>> sections_dict, vector<tuple<int,int>> sections, string language) {

    vector<tuple<int,int>> sections_to_remove;

    for (auto sections_dict_pair: sections_dict) {
        tuple<int,int> section = sections_dict_pair.first;
        if ( find(sections_dict[section].begin(), sections_dict[section].end(), language) != sections_dict[section].end() ) {
            sections.push_back(section);    // Append section to the new sections list
            if (sections_dict[section].size() == 1) {
                sections_to_remove.push_back(section);
            } else {
                sections_dict[section].erase(remove(sections_dict[section].begin(), sections_dict[section].end(), language), sections_dict[section].end());
            }
        }
    }

    // Remove those sections from the final dict in order to truncate them with the new ones
    for (tuple<int,int> section: sections_to_remove) {
        sections_dict.erase(section);
    }

    // Order the sections
    sort(sections.begin(), sections.end());

    vector<tuple<int,int>> merged_sections;
    if (sections.size() > 1) {
        tuple<int,int> initial_section = sections[0];
        tuple<int,int> merged_section = make_tuple(get<0>(initial_section), 0);
        tuple<int,int> current_section;
        for (int i = 1; i < (int) sections.size(); i++) {
            tuple<int,int> previous_section = sections[i-1];
            current_section = sections[i];

            if (get<1>(previous_section) < get<0>(current_section) - 1) {
                merged_section = make_tuple(get<0>(merged_section), get<1>(previous_section));
                merged_sections.push_back(merged_section);
                merged_section = make_tuple(get<0>(current_section), 0);
            }
        }
        merged_section = make_tuple(get<0>(merged_section), get<1>(current_section));
        merged_sections.push_back(merged_section);
    } else if (sections.size() == 1) {
        merged_sections.push_back(sections[0]);
    }

    for (tuple<int,int> section: merged_sections) {
        if (sections_dict.count(section)) {
            sections_dict[section].push_back(language);
        } else {
            sections_dict[section] = {language};
        }
    }

    return sections_dict;
}

void locateLangCalculation(string target_file_name, int k, double alpha) {

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

    
    
    // Small references texts
    // reference_file_dict = { {"AFG", "../examples/AFG/afghanistan-small.utf8"},
    //                         {"AFR", "../examples/AFR/afrikaans-small.utf8"},
    //                         {"ARA", "../examples/ARA/arabic-small.utf8"},
    //                         {"BUL", "../examples/BUL/bulgarian-small.utf8"},
    //                         {"CRO", "../examples/CRO/croatian-small.utf8"},
    //                         {"DEN", "../examples/DEN/danish-small.utf8"},
    //                         {"ENG", "../examples/ENG/gb_english.utf8"},
    //                         {"SPA", "../examples/ESP/spanish-small.utf8"},
    //                         {"FIN", "../examples/FIN/finnish-small.utf8"},
    //                         {"FRA", "../examples/FRA/french-small.utf8"},
    //                         {"GER", "../examples/GER/german-small.utf8"},
    //                         {"GRE", "../examples/GRE/greek-small.utf8"},
    //                         {"HUN", "../examples/HUN/hungarian-small.utf8"},
    //                         {"ICE", "../examples/ICE/icelandic-small.utf8"},
    //                         {"ITA", "../examples/ITA/italian-small.utf8"},
    //                         {"POL", "../examples/POL/polish-small.utf8"},
    //                         {"POR", "../examples/POR/portuguese-small.utf8"},
    //                         {"RUS", "../examples/RUS/russian-small.utf8"},
    //                         {"UKR", "../examples/UKR/ukrainian-small.utf8"}
    //                     }
                       

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
    }

    set<wchar_t> target_alphabet;
    int target_file_length = 0;

    wchar_t ch;
    while (file.get(ch)) {
        target_file_length += 1;
        target_alphabet.insert(ch);
    }
    target_file_length--;
    target_alphabet.erase(prev(target_alphabet.end()));
    file.close();

    // Sliding window implementation
    /*
    map<tuple<int,int>, vector<string>> sections_dict;
    for (auto pair: reference_file_dict) {
        string language = pair.first;

        // Create Finite Context Model from reference text
        FCM fcm_model = FCM(pair.second, k, alpha);

        // Calculate entropy
        fcm_model.calculate_probabilities();

        // Get the sections of target text that are well compressed using the language
        vector<tuple<int,int>> sections;
        tuple<double, vector<tuple<int,int>>> return_val = get_number_of_bits_required_to_compress_v2(fcm_model, target_file_name, target_alphabet, k, true);
        sections = get<1>(return_val);

        // Save the sections to main dictionary
        sections_dict = truncate_and_merge_sections(sections_dict, sections, language);
    }

    // get sections that were not well compressed by any language
    vector<int> remainder_positions;
    for (int i = 0; i < target_file_length; i++) {
        bool valid = true;
        for (auto sections_dict_pair: sections_dict) {
            tuple<int,int> section = sections_dict_pair.first;
            if (get<0>(section) == i) valid = false;
            else if (get<0>(section) < i && get<1>(section) >= i) valid = false;
        }
        if (valid) {
            remainder_positions.push_back(i);
        }
    }

    // convert the list of positions that were not compressed by any language to a list of sections [(start, end), (start, end), ...]
    vector<tuple<int, int>> remainder_sections;
    int initial_pos = 0;
    for (int i = 0; i < ((int) remainder_positions.size() - 1); i++) {
        if (remainder_positions[i] + 1 != remainder_positions[i+1]) {
            if (remainder_positions[i] - initial_pos > k) {
                remainder_sections.push_back(make_tuple(initial_pos, remainder_positions[i]));
            }
            initial_pos = remainder_positions[i + 1];
        }
    }

    // repeat the process for the final section
    if (remainder_positions.back() - initial_pos > k) {
        remainder_sections.push_back( make_tuple(initial_pos, remainder_positions.back()) );
    }

    for (auto language_pair: reference_file_dict) {
        string language = language_pair.first;
        
        // Create Finite Context Model from reference text
        FCM fcm_model = FCM(language_pair.second, k, alpha);

        // Calculate entropy
        fcm_model.calculate_probabilities();

        // Get the sections of target text that are well compressed using the language
        vector<tuple<int,int>> sections;
        tuple<double, vector<tuple<int,int>>> return_val = get_sections_from_remaining_sections(fcm_model, target_file_name, target_alphabet, k, remainder_sections);

        sections = get<1>(return_val);

        // Save the sections to main dictionary
        sections_dict = truncate_and_merge_sections(sections_dict, sections, language);
    }

    for (auto item_pair: sections_dict) {
        cout << "Positions: (" << get<0>(item_pair.first) << ", " << get<1>(item_pair.first) << "), Language: ";
        for (string language: item_pair.second)
            cout << language << ", ";
        cout << endl;
    }
    */



    /* 
        Words sections implementation. Detailed explanation was written in report document
    */
    map<string, vector<tuple<int,int>>> words_dict;
    for (auto language_pair: reference_file_dict) {
        string language = language_pair.first;

        // Create Finite Context Model from reference text
        FCM fcm_model = FCM(language_pair.second, k, alpha);

        // Calculate entropy
        fcm_model.calculate_probabilities();

        // Get the sections of target text that are well compressed using the language
        vector<tuple<int,int>> words;
        tuple<double, vector<tuple<int,int>>> return_val = get_number_of_bits_required_to_compress_v1(fcm_model, target_file_name, target_alphabet, true);
        words = get<1>(return_val);

        if (!words.empty()) words_dict[language] = words;
    }

    // Truncate step
    for (auto pair: words_dict) {
        string language = pair.first;
        vector<tuple<int,int>> sections = words_dict[language];
        if (sections.size() > 1) {
            tuple<int,int> initial_section = sections[0];
            tuple<int,int> merged_section = make_tuple(get<0>(initial_section), 0);
            vector<tuple<int, int>> merged_sections;
            tuple<int, int> current_section;
            for (int i = 1; i < (int) sections.size(); i++) {
                tuple<int, int> previous_section = sections[i-1];
                current_section = sections[i];
                
                if (get<1>(previous_section) < (get<0>(current_section) - 1) ) {
                    merged_section = make_tuple(get<0>(merged_section), get<1>(previous_section));
                    merged_sections.push_back(merged_section);
                    merged_section = make_tuple(get<0>(current_section), 0);
                }
            }
            merged_section = make_tuple(get<0>(merged_section), get<1>(current_section));
            merged_sections.push_back(merged_section);
            words_dict[language] = merged_sections;
        }
    }

    for (auto item_pair: words_dict) {
        cout << "Language: " << item_pair.first << " Words: ";
        for (std::tuple<int, int> section: item_pair.second)
            cout << "(" << get<0>(section) << ", " << get<1>(section) << ") ";
        cout << endl;
    }
    /*
        End of words sections implementation
    */

}


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

    locateLangCalculation(target_file_name, k, alpha);
    
    return 0;
}

