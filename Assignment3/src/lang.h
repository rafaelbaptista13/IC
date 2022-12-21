#ifndef LANG_H    
#define LANG_H

#include "fcm.h"
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

tuple<double, vector<tuple<int,int>>> get_number_of_bits_required_to_compress_v1(FCM fcm_model, string target_file_name, set<wchar_t> target_alphabet, bool multipleLangFlag) {

    long double total_num_bits {0};
    int different_chars_in_target {0};

    for (wchar_t ch: target_alphabet) {
        if (fcm_model.alphabet.find(ch) == fcm_model.alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    double threshold {(double) -log2(fcm_model.alpha / (fcm_model.alpha * fcm_model.alphabet.size()))};

    vector<tuple<int,int>> words;
    int char_position_in_text {0};

    auto it = fcm_model.model.begin();
    advance(it, rand() % fcm_model.model.size());
    wstring current_context = it->first;

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    bool word_creation {true};
    wstring new_word {L""};
    double word_total_bits {0};
    int initial_position {0};
    bool valid_word {true};
    
    wchar_t ch;
    while (file.get(ch)) {
        double num_bits {0};

        if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {

            if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
            } else {
                num_bits = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()));
        }

        total_num_bits += num_bits;

        if (multipleLangFlag) {
            if (word_creation) {
                if (ch != ' ' && ch != '\n' ) {
                    if (num_bits<threshold) {
                        new_word += ch;
                        word_total_bits += num_bits;
                    } else {
                        valid_word = false;
                    }
                } else {
                    if (valid_word && new_word.length() > 2) {
                        words.push_back( make_tuple(initial_position + 1, char_position_in_text) );
                    }
                    word_creation = false;
                }
            }

            if (ch == ' ' && !(word_creation)) {
                new_word = L"";
                word_total_bits = 0;
                initial_position = char_position_in_text;
                valid_word = true;
                word_creation = true;
            }
        }

        current_context.erase(0,1);
        current_context += ch;
        char_position_in_text += 1;
    }

    file.close();

    tuple <double, vector<tuple<int,int>>> returnValue(total_num_bits, words);

    return returnValue;

}




tuple<double, vector<tuple<int,int>>> get_number_of_bits_required_to_compress_v2(FCM fcm_model, string target_file_name, set<wchar_t> target_alphabet, int window_size, bool multipleLangFlag) {

    long double total_num_bits {0};
    int different_chars_in_target {0};

    for (wchar_t ch: target_alphabet) {
        if (fcm_model.alphabet.find(ch) == fcm_model.alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    double threshold {(double) log2( target_alphabet.size() ) / 2 };

    vector<tuple<int,int>> sections_dict;

    auto it = fcm_model.model.begin();
    advance(it, rand() % fcm_model.model.size());
    wstring current_context = it->first;

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    int window_initial_pos {0};
    vector<double> window_buffer = {};
    for (int i = 0; i<window_size; i++ ) {
        window_buffer.push_back(0.0);
    }
    
    wchar_t * initial_chars = new wchar_t [window_size];
    file.read(initial_chars, window_size);

    wchar_t ch;
    for (int i=0; i<window_size; i++ ){
        ch = initial_chars[i];
        double num_bits;

        if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
        
            if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
            } else {
                num_bits = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
        }

        total_num_bits += num_bits;
        window_buffer[i] = num_bits;
        current_context.erase(0,1);
        current_context += ch;
    }

    double window_total_bits {0};
    if (multipleLangFlag) {
        for (int i = 0; i<window_size; i++) {
            window_total_bits += window_buffer[i];
        }
        if ( ((double) window_total_bits/window_size) < threshold) {
            sections_dict.push_back( make_tuple(window_initial_pos, window_initial_pos+window_size) );
        }
    }

    while (file.get(ch)) {
        window_initial_pos += 1;
        double num_bits {0};
        
        if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
            if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
            } else {
                num_bits = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
        }

        total_num_bits += num_bits;
        
        if (multipleLangFlag) {
            int previous_char_index = (window_initial_pos + window_size - 1) % window_size;
            window_total_bits -= window_buffer[previous_char_index];
            window_total_bits += num_bits;
            window_buffer[previous_char_index] = num_bits;

            if (((double) window_total_bits/window_size) < threshold) {
                sections_dict.push_back( make_tuple(window_initial_pos, window_initial_pos+window_size) );
            }

        }
        
        current_context.erase(0,1);
        current_context += ch;
    }

    file.close();

    tuple <double, vector<tuple<int,int>>> returnValue(total_num_bits, sections_dict);

    return returnValue;
}




double get_number_of_bits_required_to_compress_multiplemodel(FCM first_fcm_model, FCM second_fcm_model, string target_file_name, set<wchar_t> target_alphabet) {
    double total_num_bits {0};
    double percentage_fist_model {0.5};
    double percentage_second_model {0.5};
    int different_chars_in_target {0};

    for (wchar_t ch: target_alphabet) {
        if (first_fcm_model.alphabet.find(ch) == first_fcm_model.alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    auto it_first_model = first_fcm_model.model.begin();
    advance(it_first_model, rand() % first_fcm_model.model.size());
    wstring current_context_first_model = it_first_model->first;

    auto it_second_model = second_fcm_model.model.begin();
    advance(it_second_model, rand() % second_fcm_model.model.size());
    wstring current_context_second_model = it_second_model->first;


    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      return -1.0;
    }

    wchar_t ch;
    while (file.get(ch)) {
        
        double num_bits_first_model {0};
        double num_bits_second_model {0};

        if (first_fcm_model.state_probabilities.find(current_context_first_model) != first_fcm_model.state_probabilities.end()) {
        
            if (first_fcm_model.state_probabilities[current_context_first_model].find(ch) != first_fcm_model.state_probabilities[current_context_first_model].end()) {
                num_bits_first_model = -log2( first_fcm_model.state_probabilities[current_context_first_model][ch] );
            } else {
                
                num_bits_first_model = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            cout << "entrei" << endl;
            num_bits_first_model = -log2( (double) first_fcm_model.alpha / (first_fcm_model.alpha*first_fcm_model.alphabet.size()) );
        }


        if (second_fcm_model.state_probabilities.find(current_context_second_model) != second_fcm_model.state_probabilities.end()) {
        
            if (second_fcm_model.state_probabilities[current_context_second_model].find(ch) != second_fcm_model.state_probabilities[current_context_second_model].end()) {
                num_bits_second_model = -log2( second_fcm_model.state_probabilities[current_context_second_model][ch] );
            } else {
                num_bits_second_model = -log2( (double) 1 / different_chars_in_target);
            }
        } else {
            num_bits_second_model = -log2( (double) second_fcm_model.alpha / (second_fcm_model.alpha*second_fcm_model.alphabet.size()) );
        }

        total_num_bits += (percentage_fist_model*num_bits_first_model) + (percentage_second_model*num_bits_second_model);

        if ((num_bits_first_model < num_bits_second_model) && (percentage_fist_model <= 0.97)) {
            percentage_fist_model += 0.02;
            percentage_second_model -= 0.02;
        } else if ( (num_bits_second_model < num_bits_first_model) && (percentage_second_model <= 0.97) ) {
            percentage_fist_model -= 0.02;
            percentage_second_model += 0.02;
        }

        current_context_first_model.erase(0,1);
        current_context_first_model += ch;

        current_context_second_model.erase(0,1);
        current_context_second_model += ch;
    }

    file.close();

    return total_num_bits;
}


tuple<double, vector<tuple<int,int>>> get_sections_from_remaining_sections(FCM fcm_model, string target_file_name, set<wchar_t> target_alphabet, int window_size, vector<tuple<int,int>> nonlangsections) {

    double total_num_bits = 0;
    int different_chars_in_target = 0;

    for (wchar_t ch: target_alphabet) {
        if (target_alphabet.find(ch) == target_alphabet.end()) {
            different_chars_in_target += 1;
        }
    }

    vector<tuple<int,int>> sections_dict;

    double threshold = (3*log2(target_alphabet.size())) / 4;

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    int initial_char_in_line_position {0};
    wstring line;
    getline(file, line);
    int number_of_chars_read = line.size();
    for (tuple<int,int> section: nonlangsections) {
        int section_initial_pos = get<0>(section);
        int section_final_pos =  get<1>(section);
        int window_initial_pos = section_initial_pos;
        int window_final_pos = section_initial_pos;
        wstring section_chars {L""};
        wstring current_context;

        while (true) {
            int initial_section_position_in_line_string = section_initial_pos - initial_char_in_line_position;
            int final_section_position_in_line_string = section_final_pos - initial_char_in_line_position;
            if (section_final_pos < number_of_chars_read) {
                if (section_initial_pos - window_size < 0) {
                    auto it = fcm_model.model.begin();
                    advance(it, rand() % fcm_model.model.size());
                    current_context = it->first;

                    section_chars = line.substr(initial_section_position_in_line_string, final_section_position_in_line_string);
                } else {
                    current_context = line.substr(initial_section_position_in_line_string - window_size, initial_section_position_in_line_string);

                    section_chars = line.substr(initial_section_position_in_line_string, final_section_position_in_line_string);
                }
                break;
            } else if (section_final_pos > number_of_chars_read && section_initial_pos < number_of_chars_read) {
                
                current_context = line.substr(initial_section_position_in_line_string - window_size, initial_section_position_in_line_string);

                section_chars = line.substr(initial_section_position_in_line_string);

                getline(file, line);
                initial_char_in_line_position = number_of_chars_read;
                number_of_chars_read += line.size();

                section_chars += line.substr(0, section_final_pos - initial_char_in_line_position);
                break;
            } else {
                getline(file, line);
                initial_char_in_line_position = number_of_chars_read;
                number_of_chars_read += line.size();
            }
        }

        vector<double> window_buffer = {};
        for (int i = 0; i<window_size; i++ ) {
            window_buffer.push_back(0);
        }
        window_final_pos += window_size;

        for (int i = 0; i<window_size; i++) {
            wchar_t ch = section_chars[i];

            double num_bits {0};
            if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
        
                if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                    num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
                } else {
                    num_bits = -log2( (double) 1 / different_chars_in_target);
                }
            } else {
                num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
            }

            total_num_bits += num_bits;

            window_buffer[i] = num_bits;
            current_context.erase(0,1);
            current_context += ch;
        }

        double window_total_bits {0};
        for (int i = 0; i<window_size; i++) {
            window_total_bits += window_buffer[i];
        }
        
        if (((double)window_total_bits/window_size) < threshold) {
            sections_dict.push_back( make_tuple(window_initial_pos, window_final_pos) );
        }

        window_final_pos += 1;
        window_initial_pos += 1;

        for (auto ch: section_chars.substr(window_size)) {

            double num_bits {0};
            if (fcm_model.state_probabilities.find(current_context) != fcm_model.state_probabilities.end()) {
        
                if (fcm_model.state_probabilities[current_context].find(ch) != fcm_model.state_probabilities[current_context].end()) {
                    num_bits = -log2( fcm_model.state_probabilities[current_context][ch] );
                } else {
                    num_bits = -log2( (double) 1 / different_chars_in_target);
                }
            } else {
                num_bits = -log2( (double) fcm_model.alpha / (fcm_model.alpha*fcm_model.alphabet.size()) );
            }

            total_num_bits += num_bits;

            int previous_char_index = (window_initial_pos + window_size - 1) % window_size;
            
            window_total_bits -= window_buffer[previous_char_index];
            
            window_total_bits += num_bits;

            window_buffer[previous_char_index] = num_bits;

            if (((double)window_total_bits/window_size) < threshold) {
                sections_dict.push_back( make_tuple(window_initial_pos, window_final_pos) );
            }

            current_context.erase(0,1);
            current_context += ch;
            window_final_pos += 1;
            window_initial_pos += 1;
        }
    }

    file.close();
    tuple <double, vector<tuple<int,int>>> returnValue(total_num_bits, sections_dict);

    return returnValue;

}

tuple<double, vector<tuple<int,int>>> secondMain(string input_file_name, string target_file_name, int k, double alpha, bool multipleLangFlag, bool multipleModelsFlag) {

    std::locale::global(std::locale(""));
    std::wifstream file(target_file_name);
    if (!file.is_open()){
      std::cerr << "Invalid target file: '" << target_file_name << "'" << std::endl;
      vector<tuple<int,int>> emptyVector {};
      tuple<double, vector<tuple<int,int>>> emptyReturnValue = make_tuple(NULL, emptyVector);
      return emptyReturnValue;
    }

    wchar_t ch;             
    set<wchar_t> target_alphabet;

    while (file.get(ch)) {
        target_alphabet.insert(ch);
    }
    file.close();

    if (multipleModelsFlag) {
        FCM first_fcm_model {input_file_name, 2, alpha};
        first_fcm_model.calculate_probabilities();

        FCM second_fcm_model {input_file_name, 4, alpha};
        second_fcm_model.calculate_probabilities();

        double bits;
        bits = get_number_of_bits_required_to_compress_multiplemodel(first_fcm_model, second_fcm_model, target_file_name, target_alphabet);
        
        tuple <double, vector<tuple<int,int>>> returnValue(bits, {});

        return returnValue;
    } else {
        FCM fcm_model {input_file_name, k, alpha};
        fcm_model.calculate_probabilities();

        double bits;
        vector<tuple<int,int>> sections;
        tuple<double, vector<tuple<int,int>>> return_val = get_number_of_bits_required_to_compress_v2(fcm_model, target_file_name, target_alphabet, k, multipleLangFlag);

        // Following line needs to be uncommented in order to use second implementation of locatelang.py. Detailed explanation was written in report document.
        //tuple<double, vector<tuple<int,int>>> return_val = get_number_of_bits_required_to_compress_v1(fcm_model, target_file_name, target_alphabet, multipleLangFlag);
        
        bits = get<0>(return_val);
        sections = get<1>(return_val);

        tuple <double, vector<tuple<int,int>>> returnValue(bits, sections);
        return returnValue;
    }
}

#endif
